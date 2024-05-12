//client.c
#include <arpa/inet.h>
#include <linux/net_tstamp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/poll.h>
#include <netinet/in.h>

#define MAX_EVENTS 10
#define MAX_BUFFER_SIZE 1024

enum {
    esIdle = -1,
    esSend1,
    esSend2,
    esRecv3,
    esRecv4
};

enum {
    eSendSync,
    eRecvSync,
    eSendResp,
    eRecvResp
};

typedef struct Head {
    int nID;
}Head;

typedef struct Send1{
    int nID;
    uint64_t ullSec;
    uint64_t ullNsec;
}Send1;

typedef struct Send2 {
    int nID;
    uint64_t ullSec;
    uint64_t ullNsec;
}Send2;

typedef struct Recv3{
    int nID;
    uint64_t ullSec1;
    uint64_t ullNsec1;
    uint64_t ullSec2;
    uint64_t ullNsec2;
}Recv3;

typedef struct Recv4{
    int nID;
    uint64_t ullSec;
    uint64_t ullNsec;
}Recv4;

void die(char* s)
{
    perror(s);
    exit(1);
}

int g_state;
struct timespec g_TSData[4];

void scan_cb_buf(struct msghdr* msg, struct timespec* ts)
{
    struct cmsghdr* cmsg;
    //printf("scan_cb_buf...\n");

    cmsg = CMSG_FIRSTHDR(msg);
    if (!cmsg)
        printf("INITIAL CMSG_FIRSTHDR(msg) == NULL\n");

    for (; cmsg; cmsg = CMSG_NXTHDR(msg, cmsg)) {
        //printf("!\n");
        if (cmsg->cmsg_level != SOL_SOCKET)
            continue;

        //printf("!!\n");

        switch (cmsg->cmsg_type) {
        case SO_TIMESTAMPNS:
        case SO_TIMESTAMPING: {
            struct timespec* stamp = (struct timespec*)CMSG_DATA(cmsg);
            printf("ts[0] %ld.%06ld\n", (long)stamp[0].tv_sec, (long)stamp[0].tv_nsec);
//             printf("ts[1] %ld.%06ld\n",
//                 (long)stamp[1].tv_sec, (long)stamp[1].tv_nsec);
//             printf("ts[2] %ld.%06ld\n",
//                 (long)stamp[2].tv_sec, (long)stamp[2].tv_nsec);
            *ts = stamp[0];
        }
        break;
        default:
            printf("???\n");
        break;
        }
    }

    struct timespec tp;

    clock_gettime(CLOCK_REALTIME, &tp);
    printf("CLOCK_REALTIME ts %ld.%06ld\n\n", (long)tp.tv_sec, (long)tp.tv_nsec);

//     clock_gettime(CLOCK_MONOTONIC, &tp);
//     printf("CLOCK_MONOTONIC ts %ld.%06ld\n", (long)tp.tv_sec, (long)tp.tv_nsec);
// 
//     clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tp);
//     printf("CLOCK_PROCESS_CPUTIME_ID ts %ld.%06ld\n", (long)tp.tv_sec, (long)tp.tv_nsec);
// 
//     clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tp);
//     printf("CLOCK_THREAD_CPUTIME_ID ts %ld.%06ld\n", (long)tp.tv_sec, (long)tp.tv_nsec);

}

int set_nonblocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl(F_GETFL) failed");
        return -1;
    }
    flags |= O_NONBLOCK;
    if (fcntl(sockfd, F_SETFL, flags) == -1) {
        perror("fcntl(F_SETFL) failed");
        return -1;
    }
    return 0;
}

static short sk_events = POLLPRI;
static short sk_revents = POLLPRI;

int receive(int fd, void* buf, int buflen, int flags, struct timespec* ts)
{
    char control[256];
    int cnt = 0, res = 0, level, type;
    struct cmsghdr* cm;
    struct iovec iov = { buf, buflen };
    struct msghdr msg;
    struct timespec* sw = NULL;
    struct sockaddr_in addr;

    memset(control, 0, sizeof(control));
    memset(&msg, 0, sizeof(msg));
 
    msg.msg_name = &addr;
    msg.msg_namelen = sizeof(addr);

    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = control;
    msg.msg_controllen = sizeof(control);

    if (flags == MSG_ERRQUEUE) {
        struct pollfd pfd = { fd, sk_events, 0 };
        res = poll(&pfd, 1, 1);
        /* Retry once on EINTR to avoid logging errors before exit */
        if (res < 0 && errno == EINTR)
            res = poll(&pfd, 1, 1);
        if (res < 0) {
            perror("poll for tx timestamp failed");
            return -errno;
        }
        else if (!res) {
            perror("timed out while polling for tx timestamp");
            perror("increasing tx_timestamp_timeout or increasing "
                "kworker priority may correct this issue, "
                "but a driver bug likely causes it");
            errno = ETIME;
            return -1;
        }
        else if (!(pfd.revents & sk_revents)) {
            perror("poll for tx timestamp woke up on non ERR event");
            return -1;
        }
    }

    cnt = recvmsg(fd, &msg, flags);
    if (cnt < 0) {
        printf("recvmsg%sfailed: %m",
            flags == MSG_ERRQUEUE ? " tx timestamp " : " ");
        return -1;
    }

     scan_cb_buf(&msg, ts);

    return cnt < 0 ? -errno : cnt;
}

int main(int argc, char* argv[])
{
    g_state = esIdle;

    if (argc != 3) {
        printf("usage: %s ip port", argv[0]);
        return -1;
    }

    char* ip = argv[1];
    int port = atoi(argv[2]);
    if (port <= 0 || port > 65535) {
        printf("invalid port number");
        return -1;
    }

    int client_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_sockfd == -1) {
        perror("socket() failed");
        return -1;
    }

    if (set_nonblocking(client_sockfd) == -1) {
        return -1;
    }

    int timestamp_flags = SOF_TIMESTAMPING_TX_SOFTWARE | SOF_TIMESTAMPING_RX_SOFTWARE | SOF_TIMESTAMPING_SOFTWARE;
    //SOF_TIMESTAMPING_RX_HARDWARE
    //| SOF_TIMESTAMPING_RAW_HARDWARE 

    if (setsockopt(client_sockfd, SOL_SOCKET, SO_TIMESTAMPING, &timestamp_flags, sizeof(timestamp_flags)) < 0) {
        die("setsockopt()");
    }

    int flags = 1;
    if (setsockopt(client_sockfd, SOL_SOCKET, SO_SELECT_ERR_QUEUE, &flags, sizeof(flags)) < 0) {
        die("SO_SELECT_ERR_QUEUE");
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &server_addr.sin_addr);

    int epollfd = epoll_create1(0);
    if (epollfd == -1) {
        perror("epoll_create1() failed");
        return -1;
    }

    struct epoll_event event, events[MAX_EVENTS];
    memset(&event, 0, sizeof(event));
    event.events = EPOLLIN;
    event.data.fd = client_sockfd;

    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, client_sockfd, &event) == -1) {
        perror("epoll_ctl(EPOLL_CTL_ADD) failed");
        return -1;
    }

    char buffer[MAX_BUFFER_SIZE];
    char recvBuf[MAX_BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));
    memset(recvBuf, 0, sizeof(recvBuf));
    strncpy(buffer, "Hello", sizeof(buffer) - 1);

    struct timespec ts;

    while (1) {

        if (g_state == esIdle) {
            sleep(5);
            printf("\n/**************************************************************/\n");

            Send1 s;
            s.nID = esSend1;
            s.ullSec = 0;
            s.ullNsec = 0;

            ssize_t len = sendto(client_sockfd, &s, sizeof(s), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
            if (len == -1) {
                perror("sendto() failed");
                continue;
            }
            printf("Sent %zd bytes to %s:%d\n", len, ip, port);
            g_state = esSend1;
            receive(client_sockfd, recvBuf, MAX_BUFFER_SIZE, MSG_ERRQUEUE, &ts);

            g_TSData[eSendSync].tv_sec = ts.tv_sec;
            g_TSData[eSendSync].tv_nsec = ts.tv_nsec;

            Send2 s2;
            s2.nID = esSend2;
            s2.ullSec = g_TSData[eSendSync].tv_sec;
            s2.ullNsec = g_TSData[eSendSync].tv_nsec;

            len = sendto(client_sockfd, &s2, sizeof(s2), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
            if (len == -1) {
                perror("sendto() failed");
                continue;
            }
            printf("Sent %zd bytes to %s:%d\n", len, ip, port);
            g_state = esSend2;
            receive(client_sockfd, recvBuf, MAX_BUFFER_SIZE, MSG_ERRQUEUE, &ts);
        }

        int nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            if (errno == EINTR) {
                continue;
            }
            perror("epoll_wait() failed");
            break;
        }

        for (int i = 0; i < nfds; i++) {
            int sockfd = events[i].data.fd;

            if ((events[i].events & EPOLLIN) && (sockfd == client_sockfd)) {
                printf("receive something back from server\n");
                receive(client_sockfd, recvBuf, MAX_BUFFER_SIZE, MSG_DONTWAIT, &ts);

                Head* head = (Head*)recvBuf;
                if (head->nID == esRecv3) {
                    printf("Get Recv3\n");
                    g_state = esRecv3;
                    Recv3* recv = (Recv3*)recvBuf;
                    g_TSData[eRecvResp].tv_sec = ts.tv_sec;
                    g_TSData[eRecvResp].tv_nsec = ts.tv_nsec;
                    g_TSData[eRecvSync].tv_sec = recv->ullSec2;
                    g_TSData[eRecvSync].tv_nsec = recv->ullNsec2;
                }
                else if (head->nID == esRecv4) {
                    g_state = esIdle;
                    printf("Get Recv4\n");
                    Recv4* recv = (Recv4*)recvBuf;
                    g_TSData[eSendResp].tv_sec = recv->ullSec;
                    g_TSData[eSendResp].tv_nsec = recv->ullNsec;
                    uint64_t d1 = g_TSData[eRecvSync].tv_nsec - g_TSData[eSendSync].tv_nsec;
                    uint64_t d2 = g_TSData[eRecvResp].tv_nsec - g_TSData[eSendResp].tv_nsec;
                    printf(
                        "T1 = %ld.%09ld   T2 = %ld.%09ld\n"
                        "T3 = %ld.%09ld   T4 = %ld.%09ld\n"
                        "Offset = %ldns     Delay = %ldns\n",
                        g_TSData[eSendSync].tv_sec, g_TSData[eSendSync].tv_nsec,
                        g_TSData[eRecvSync].tv_sec, g_TSData[eRecvSync].tv_nsec,
                        g_TSData[eSendResp].tv_sec, g_TSData[eSendResp].tv_nsec,
                        g_TSData[eRecvResp].tv_sec, g_TSData[eRecvResp].tv_nsec,
                        (d1 + d2) / 2, (d1 - d2) /2
                    );
                }
                memset(recvBuf, 0, sizeof(recvBuf));
            }
        }
    }

    close(epollfd);
    close(client_sockfd);
    return 0;
}
