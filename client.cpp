#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#pragma comment(lib, "ws2_32.lib")

#define MCASTADDR "239.192.152.143" //本例使用的多播组地址。
#define MCASTPORT 6771 //绑定的本地端口号。
#define BUFSIZE 1024 //接收数据缓冲大小。

int test1(int argc, char ** argv)
{
	WSADATA wsd;
	struct sockaddr_in local, remote, from;
	SOCKET sock, sockM;
	TCHAR recvbuf[BUFSIZE];

	int len = sizeof(struct sockaddr_in);
	int ret;

	//初始化WinSock2.2
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		printf("WSAStartup() failed\n");
		return -1;
	}

	if ((sock = WSASocket(AF_INET, SOCK_DGRAM, 0, NULL, 0,
		WSA_FLAG_MULTIPOINT_C_LEAF | WSA_FLAG_MULTIPOINT_D_LEAF |
		WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
	{
		printf("socket failed with:%d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}

	//将sock绑定到本机某端口上。
	local.sin_family = AF_INET;
	local.sin_port = htons(MCASTPORT);
	local.sin_addr.s_addr = INADDR_ANY;
	if (bind(sock, (struct sockaddr*)&local, sizeof(local)) == SOCKET_ERROR)
	{
		printf("bind failed with:%d \n", WSAGetLastError());
		closesocket(sock);
		WSACleanup();
		return -1;
	}

	//加入多播组
	remote.sin_family = AF_INET;
	remote.sin_port = htons(MCASTPORT);
	remote.sin_addr.s_addr = inet_addr(MCASTADDR);

	if ((sockM = WSAJoinLeaf(sock, (SOCKADDR*)&remote, sizeof(remote),
		NULL, NULL, NULL, NULL,
		JL_BOTH)) == INVALID_SOCKET)
	{
		printf("WSAJoinLeaf() failed:%d\n", WSAGetLastError());
		closesocket(sock);
		WSACleanup();
		return -1;
	}

	//接收多播数据，当接收到的数据为"QUIT"时退出。
	while (1)
	{
		if ((ret = recvfrom(sock, recvbuf, BUFSIZE, 0,
			(struct sockaddr*)&from, &len)) == SOCKET_ERROR)
		{
			printf("recvfrom failed with:%d\n", WSAGetLastError());
			closesocket(sockM);
			closesocket(sock);
			WSACleanup();
			return -1;
		}

		if (strcmp(recvbuf, "QUIT") == 0)
		{
			break;
		}
		else 
		{
			recvbuf[ret] = '\0';
			printf("RECV:' %s ' FROM <%s> \n", recvbuf, inet_ntoa(from.sin_addr));
		}
	}

	closesocket(sockM);
	closesocket(sock);
	WSACleanup();
	return 0;
}