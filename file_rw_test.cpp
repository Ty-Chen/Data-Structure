//gcc -o test_rw file_rw_test.cpp -I src/include/ -L src/ -Wall -O2 -D_GNU_SOURCE -luring -lstdc++ -g

#include "src/include/liburing.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/uio.h>
#include <unistd.h>
#include <chrono>
#include <iostream>
#include <fstream>
#include<sys/ioctl.h>
#include <assert.h>
#define COLUMN 10000000
#define LINE   100
#define BUF_SIZE COLUMN * LINE

char sz_test[COLUMN][LINE];

#define QD  64
#define BS  (32*1024)

static int infd, outfd;

struct io_data {
    int read;
    off_t first_offset, offset;
    size_t first_len;
    struct iovec iov;
};

static int setup_context(unsigned entries, struct io_uring *ring)
{
    int ret;

    ret = io_uring_queue_init(entries, ring, 0);
    if (ret < 0) {
        fprintf(stderr, "queue_init: %s\n", strerror(-ret));
        return -1;
    }

    return 0;
}

static int get_file_size(int fd, off_t *size)
{
    struct stat st;

    if (fstat(fd, &st) < 0)
        return -1;
    if (S_ISREG(st.st_mode)) {
        *size = st.st_size;
        return 0;
    } else if (S_ISBLK(st.st_mode)) {
        unsigned long long bytes;

        if (ioctl(fd, BLKGETSIZE64, &bytes) != 0)
            return -1;

        *size = bytes;
        return 0;
    }

    return -1;
}

static void queue_prepped(struct io_uring *ring, struct io_data *data)
{
    struct io_uring_sqe *sqe;

    sqe = io_uring_get_sqe(ring);
    assert(sqe);

    if (data->read)
        io_uring_prep_readv(sqe, infd, &data->iov, 1, data->offset);
    else
        io_uring_prep_writev(sqe, outfd, &data->iov, 1, data->offset);

    io_uring_sqe_set_data(sqe, data);
}

static int queue_read(struct io_uring *ring, off_t size, off_t offset)
{
    struct io_uring_sqe *sqe;
    struct io_data *data;

    data = (io_data*)malloc(size + sizeof(*data));
    if (!data)
        return 1;

    sqe = io_uring_get_sqe(ring);
    if (!sqe) {
        free(data);
        return 1;
    }

    data->read = 1;
    data->offset = data->first_offset = offset;

    data->iov.iov_base = data + 1;
    data->iov.iov_len = size;
    data->first_len = size;

    io_uring_prep_readv(sqe, infd, &data->iov, 1, offset);
    io_uring_sqe_set_data(sqe, data);
    return 0;
}

static void queue_write(struct io_uring *ring, struct io_data *data)
{
    data->read = 0;
    data->offset = data->first_offset;

    data->iov.iov_base = data + 1;
    data->iov.iov_len = data->first_len;

    queue_prepped(ring, data);
    io_uring_submit(ring);
}

static int copy_file(struct io_uring *ring, off_t insize)
{
    unsigned long reads, writes;
    struct io_uring_cqe *cqe;
    off_t write_left, offset;
    int ret;

    write_left = insize;
    writes = reads = offset = 0;

    while (insize || write_left) {
        unsigned long had_reads;
        int got_comp;
    
        /*
         * Queue up as many reads as we can
         */
        had_reads = reads;
        while (insize) {
            off_t this_size = insize;

            if (reads + writes >= QD)
                break;
            if (this_size > BS)
                this_size = BS;
            else if (!this_size)
                break;

            if (queue_read(ring, this_size, offset))
                break;

            insize -= this_size;
            offset += this_size;
            reads++;
        }

        if (had_reads != reads) {
            ret = io_uring_submit(ring);
            if (ret < 0) {
                fprintf(stderr, "io_uring_submit: %s\n", strerror(-ret));
                break;
            }
        }

        /*
         * Queue is full at this point. Find at least one completion.
         */
        got_comp = 0;
        while (write_left) {
            struct io_data *data;

            if (!got_comp) {
                ret = io_uring_wait_cqe(ring, &cqe);
                got_comp = 1;
            } else {
                ret = io_uring_peek_cqe(ring, &cqe);
                if (ret == -EAGAIN) {
                    cqe = NULL;
                    ret = 0;
                }
            }
            if (ret < 0) {
                fprintf(stderr, "io_uring_peek_cqe: %s\n",
                            strerror(-ret));
                return 1;
            }
            if (!cqe)
                break;

            data = (io_data*)io_uring_cqe_get_data(cqe);
            if (cqe->res < 0) {
                if (cqe->res == -EAGAIN) {
                    queue_prepped(ring, data);
                    io_uring_submit(ring);
                    io_uring_cqe_seen(ring, cqe);
                    continue;
                }
                fprintf(stderr, "cqe failed: %s\n",
                        strerror(-cqe->res));
                return 1;
            } else if ((size_t)cqe->res != data->iov.iov_len) {
                /* Short read/write, adjust and requeue */
                data->iov.iov_base += cqe->res;
                data->iov.iov_len -= cqe->res;
                data->offset += cqe->res;
                queue_prepped(ring, data);
                io_uring_submit(ring);
                io_uring_cqe_seen(ring, cqe);
                continue;
            }

            /*
             * All done. if write, nothing else to do. if read,
             * queue up corresponding write.
             */
            if (data->read) {
                queue_write(ring, data);
                write_left -= data->first_len;
                reads--;
                writes++;
            } else {
                free(data);
                writes--;
            }
            io_uring_cqe_seen(ring, cqe);
        }
    }

    /* wait out pending writes */
    while (writes) {
        struct io_data *data;

        ret = io_uring_wait_cqe(ring, &cqe);
        if (ret) {
            fprintf(stderr, "wait_cqe=%d\n", ret);
            return 1;
        }
        if (cqe->res < 0) {
            fprintf(stderr, "write res=%d\n", cqe->res);
            return 1;
        }
        data = (io_data*)io_uring_cqe_get_data(cqe);
        free(data);
        writes--;
        io_uring_cqe_seen(ring, cqe);
    }

    return 0;
}

int test_io_uring_sqe() {
    struct io_uring ring;
    int ret = 0;
    auto start = std::chrono::steady_clock::now();

    infd = open("test.csv", O_RDONLY);
    if (infd < 0) {
        perror("open infile");
        return 1;
    }

    outfd = open("io_uring_output.csv", O_WRONLY | O_TRUNC | O_CREAT | O_DIRECT, 0644);
    if (outfd < 0) {
        perror("open outfile");
        return 1;
    }

    ret = ftruncate(outfd, BUF_SIZE);

    if (setup_context(QD, &ring))
        return 1;

    ret = copy_file(&ring, BUF_SIZE);

    io_uring_queue_exit(&ring);
    close(infd);
    close(outfd);
    auto end = std::chrono::steady_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;
    return 0;
}

#define SQNUM 128

void test_io_uring_write() {
    struct io_uring ring;
    struct iovec* iovecs;
    struct io_uring_sqe* sqe;
    struct io_uring_cqe* cqe;
    int fd;
    int  offset = 0;

    auto start = std::chrono::steady_clock::now();

    int ret = io_uring_queue_init(SQNUM, &ring, 0);

    fd = open("tmp_file.csv", O_WRONLY | O_TRUNC | O_CREAT | O_DIRECT, 0644);
    ret = ftruncate(fd, BUF_SIZE);

    sqe = io_uring_get_sqe(&ring);

    offset = BUF_SIZE / SQNUM;

    for (int i = 0; i < SQNUM; ++i) {
        io_uring_prep_write(sqe, fd, sz_test + offset * i, offset, offset * i);
        ret = io_uring_submit(&ring);
    }
    
    int cnt = SQNUM;

    while (cnt) {
        ret = io_uring_wait_cqe(&ring, &cqe);
        if (!ret) {
            printf("cqe->res: %d\n", cqe->res);
        }
        cnt--;
    }

    auto end = std::chrono::steady_clock::now();
    std::cout << "io_uring : " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;
    io_uring_cqe_seen(&ring, cqe);

    io_uring_queue_exit(&ring);
    close(fd);
    return;
}

int test_normal_write() {
    char buff[BUF_SIZE];
    int count(0);
    char delimiter = ',';
    int  offset = 0;

    auto start = std::chrono::steady_clock::now();
    std::ofstream ofs("test.csv", std::ofstream::out);
//     for (int r = 0; r < COLUMN; r++)
//     {
//         //ofs << r + 1 << delimiter << c + 1 << delimiter << s + 1 << delimiter << a[c][r][s] << '\n';
//             ofs << *sz_test[r];
//     }

    ofs.write((const char*)sz_test, BUF_SIZE);
    ofs.close();
    auto end = std::chrono::steady_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;
    return count;
}

int test_normal_read_write() {
    char buff[BUF_SIZE];
    int count(0);
    char delimiter = ',';
    int  offset = 0;
    char content[256];

    auto start = std::chrono::steady_clock::now();
    std::ifstream ifs("test.csv", std::ifstream::in);
    std::ofstream ofs("normal_output.csv", std::ofstream::out);
    //     for (int r = 0; r < COLUMN; r++)
    //     {
    //         //ofs << r + 1 << delimiter << c + 1 << delimiter << s + 1 << delimiter << a[c][r][s] << '\n';
    //             ofs << *sz_test[r];
    //     }

    //ofs.write((const char*)sz_test, BUF_SIZE);

    while (!ifs.eof()) {
        ifs.getline(content, 128);
        ofs << content;
    }

    ofs.close();
    ifs.close();
    auto end = std::chrono::steady_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;
    return count;
}

int main() {

    for (int i = 0; i < COLUMN; ++i) {
        strcpy(sz_test[i], "hello world\n");
    }

    test_io_uring_write();

//     std::cout << std::endl << "io_uring cpy file" << std::endl;
//     test_io_uring_sqe();

    std::cout << std::endl << "ofstream : " ;
    test_normal_write();

//     std::cout << std::endl << "normal cpy file" << std::endl;
//     test_normal_read_write();
}
