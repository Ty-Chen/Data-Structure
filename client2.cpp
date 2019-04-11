#include <iostream>
#include <winsock2.h> //注意这里的include文件顺序
#include <Ws2tcpip.h>
#include <process.h> //_beginthread要求

#pragma comment(lib, "ws2_32.lib")

using namespace std;

//const char* MULTICAST_IP = "224.0.0.99"; //多播组地址
const char* MULTICAST_IP = "239.192.152.143"; //多播组地址
const int MULTICAST_PORT = 6771; //多播组端口

const int BUFFER_SIZE = 1024;

void do_send(void* arg); //读取用户输入并发送到多播组线程函数
void do_read(void* arg); //读物多播组数据函数

int main()
{
	WSAData wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		cout << "Error in WSAStartup" << endl;
		return 0;
	}

	SOCKET server;
	server = socket(AF_INET, SOCK_DGRAM, 0); //创建一个UDP套接口
	cout << "create socket: " << server << endl;

	int ret;

	const int on = 1; //允许程序的多个实例运行在同一台机器上
	ret = setsockopt(server, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
	if (ret == SOCKET_ERROR)
	{
		WSACleanup();

		cout << "Error in setsockopt(SO_REUSEADDR): " << WSAGetLastError() << endl;
		return 0;
	}

	const int routenum = 10;
	ret = setsockopt(server, IPPROTO_IP, IP_MULTICAST_TTL, \
		(char*)&routenum, sizeof(routenum));
	if (ret == SOCKET_ERROR)
	{
		WSACleanup();

		cout << "Error in setsockopt(IP_MULTICAST_TTL): " << WSAGetLastError() << endl;
		return 0;
	}

	const int loopback = 1; //允许回馈
	ret = setsockopt(server, IPPROTO_IP, IP_MULTICAST_LOOP, \
		(char*)&loopback, sizeof(loopback));
	if (ret == SOCKET_ERROR)
	{
		WSACleanup();

		cout << "Error in setsockopt(IP_MULTICAST_LOOP): " << WSAGetLastError() << endl;
		return 0;
	}

	sockaddr_in local;
	memset(&local, 0, sizeof(local));
	local.sin_family = AF_INET;
	local.sin_port = htons(MULTICAST_PORT);
	local.sin_addr.S_un.S_addr = INADDR_ANY;

	ret = bind(server, (sockaddr*)(&local), sizeof(local));
	if (ret == SOCKET_ERROR)
	{
		WSACleanup();

		cout << "Error in bind: " << WSAGetLastError() << endl;
		return 0;
	}

	ip_mreq mreq;
	memset(&mreq, 0, sizeof(mreq));
	mreq.imr_interface.S_un.S_addr = INADDR_ANY;
	mreq.imr_multiaddr.S_un.S_addr = inet_addr(MULTICAST_IP);

	//加入一个多播组
	ret = setsockopt(server, IPPROTO_IP, IP_ADD_MEMBERSHIP, \
		(char*)&mreq, sizeof(mreq));
	if (ret == SOCKET_ERROR)
	{
		WSACleanup();

		cout << "Error in setsockopt(IP_ADD_MEMBERSHIP): " << WSAGetLastError() << endl;
		return 0;
	}

	//创建了两个线程，一个读用户输入并发送，一个读多播组数据
	HANDLE hHandle[2];
	hHandle[0] = (HANDLE)_beginthread(do_send, 0, (void*)server);
	hHandle[1] = (HANDLE)_beginthread(do_read, 0, (void*)server);

	//如果用户输入结束，程序就终止了
	WaitForSingleObject(hHandle[0], INFINITE);

	WSACleanup();

	return 0;
}

void do_send(void* arg)
{
	SOCKET server = (SOCKET)arg;

	char sendline[BUFFER_SIZE + 1];

	sockaddr_in remote;
	memset(&remote, 0, sizeof(remote));
	remote.sin_addr.s_addr = inet_addr(MULTICAST_IP);
	remote.sin_family = AF_INET;
	remote.sin_port = htons(MULTICAST_PORT);

	for (;;) //读取用户输入知道用户输入"end"
	{
		std::cout << "Send: ";
		std::cin >> sendline;
		//cin.getline(sendline, BUFFER_SIZE);

		if (strncmp(sendline, "end", 3) == 0)
			break;

		//发送用户输入的数据到多播组
		sendto(server, sendline, strlen(sendline), 0, (sockaddr*)(&remote), sizeof(remote));
	}

	cout << "do_send end..." << endl;
}

void do_read(void* arg)
{
	SOCKET server = (SOCKET)arg;

	char buf[BUFFER_SIZE + 1];
	int ret;

	sockaddr_in client;
	int clientLen;

	for (;;) //一直读取知道主线程终止
	{
		clientLen = sizeof(client);
		memset(&client, 0, clientLen);

		ret = recvfrom(server, buf, BUFFER_SIZE, 0, (sockaddr*)(&clientLen), &clientLen);
		if (ret == 0) //do_read在用户直接回车发送了一个空字符串
		{
			continue;
		}
		else if (ret == SOCKET_ERROR)
		{
			if (WSAGetLastError() == WSAEINTR) //主线程终止recvfrom返回的错
				break;

			cout << "Error in recvfrom: " << WSAGetLastError() << endl;
			break;
		}
		buf[ret] = '\0';
		cout << std::endl << "received: " << buf << endl;
		std::cout << "Send: ";
	}

	cout << "do_read end..." << endl;
}