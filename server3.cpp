#include <stdio.h>  
#include <winsock2.h>  
#include <ws2tcpip.h>  
#pragma comment(lib, "ws2_32.lib")

int test3()
{
	int iRet = 0;
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	//addr.sin_addr.S_un.S_addr = INADDR_ANY;
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(9411);

	bool bOptval = true;
	iRet = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&bOptval, sizeof(bOptval));
	if (iRet != 0) {
		printf("setsockopt fail:%d", WSAGetLastError());
		return -1;
	}

	iRet = bind(sock, (sockaddr*)&addr, sizeof(addr));
	if (iRet != 0) {
		printf("bind fail:%d", WSAGetLastError());
		return -1;
	}
	printf("socket:%d bind success\n", sock);

	// ¼ÓÈë×é²¥  
	ip_mreq multiCast;
	//multiCast.imr_interface.S_un.S_addr = INADDR_ANY;
	multiCast.imr_interface.S_un.S_addr = inet_addr("127.0.0.1");
	multiCast.imr_multiaddr.S_un.S_addr = inet_addr("234.2.2.2");
	iRet = setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&multiCast, sizeof(multiCast));
	if (iRet != 0) {
		printf("setsockopt fail:%d", WSAGetLastError());
		return -1;
	}

	printf("udp group start\n");

	int len = sizeof(sockaddr);
	char strRecv[1024] = { 0 };
	while (true)
	{
		memset(strRecv, 0, sizeof(strRecv));
		iRet = recvfrom(sock, strRecv, sizeof(strRecv) - 1, 0, (sockaddr*)&addr, &len);
		if (iRet <= 0) {
			printf("recvfrom fail:%d", WSAGetLastError());
			return -1;
		}
		printf("recv data:%s\n", strRecv);
	}

	closesocket(sock);
	WSACleanup();

	return 0;
}
