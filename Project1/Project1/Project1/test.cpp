#include "hasher.hpp"
#include <iostream>

#define PROTOCOL_STRING "DolitBT-DLBT\0\0\0\0\0\0\0"

char *Read(char *filePath, DWORD *size)
{
	HANDLE pFile;
	DWORD fileSize;
	char *buffer, *tmpBuf;
	DWORD dwBytesRead, dwBytesToRead, tmpLen;

	pFile = CreateFile(filePath, GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,         
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (pFile == INVALID_HANDLE_VALUE)
	{
		printf("open file error!\n");
		CloseHandle(pFile);
		return FALSE;
	}

	//得到文件的大小
	fileSize = GetFileSize(pFile, NULL);          
	*size = fileSize;

	buffer = (char *)malloc(fileSize);
	ZeroMemory(buffer, fileSize);
	dwBytesToRead = fileSize;
	dwBytesRead = 0;
	tmpBuf = buffer;

	do {                                       
		
		//循环读文件，确保读出完整的文件    
		ReadFile(pFile, tmpBuf, dwBytesToRead, &dwBytesRead, NULL);

		if (dwBytesRead == 0)
			break;

		dwBytesToRead -= dwBytesRead;
		tmpBuf += dwBytesRead;

	} while (dwBytesToRead > 0);

	//  TODO 处理读到的数据 buffer
	//free(buffer);
	CloseHandle(pFile);

	return buffer;
}

bool __writeHandshake(sha1_hash infoHash)
{

	// add handshake to the send buffer
	const int string_len = sizeof(PROTOCOL_STRING) - 1;

	char handshake[1 + string_len + 8 + 20 + 20];
	char *ptr = handshake;

	// length of version string
	handshake[0] = string_len;
	ptr += 1;

	// protocol identifier
	memcpy(ptr, &PROTOCOL_STRING, string_len);
	ptr += string_len;

	// 8 zeroes, reserved bits
	//char reserve[8] = { 0 };
	//memcpy(ptr, reserve, 8);
	memset(ptr, 0, 8);
	ptr += 8;
	
	// info hash
	char out[20];
	to_hex(reinterpret_cast<char const*>(&infoHash[0]), sha1_hash::size, out);
	memcpy(ptr, out, 20);
	ptr += 20;

	// peer_id
	//memcpy(ptr, &m_peerInfo->pid[0], 20);

	return true;

}

int main()
{
	DWORD size = 0;
	char *buffer = NULL;
	char filePath[] = "jx3hd_c_1.0.0.1319-to-1.0.0.1321_zhcn_hd_patch.exe.torrent";

	buffer = Read(filePath, &size);
	hasher hs(buffer, size);

	sha1_hash infoHash = hs.final();

	std::cout << infoHash << std::endl;

	__writeHandshake(infoHash);
	getchar();
}