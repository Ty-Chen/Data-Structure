#include <iostream>
#define PROTOCOL_STRING "BitTorrent protocol"

void test1()
{
	const int string_len = sizeof(PROTOCOL_STRING) - 1;
	char handshake[1 + string_len + 8 + 20 + 20];
	char *ptr = handshake;

	std::cout << "sizeof handshake is " << sizeof(handshake) << std::endl;
	std::cout << "strlen handshake is " << strlen(handshake) << std::endl;

	// length of version string
	memcpy(ptr, &string_len, 1);
	ptr += 1;

	std::cout << "sizeof handshake is " << sizeof(handshake) << std::endl;
	std::cout << "strlen handshake is " << strlen(handshake) << std::endl;

	// protocol identifier
	memcpy(ptr, &PROTOCOL_STRING, string_len);
	ptr += string_len;

	std::cout << "sizeof handshake is " << sizeof(handshake) << std::endl;
	std::cout << "strlen handshake is " << strlen(handshake) << std::endl;

	// 8 zeroes, reserved bits
	memset(ptr, 0, 8);
	ptr += 8;
	std::cout << "sizeof handshake is " << sizeof(handshake) << std::endl;
	std::cout << "strlen handshake is " << strlen(handshake) << std::endl;

	return;
}

void test2()
{
	char hello[6] = "hello";
	char *msg = (char *)malloc(10);

	std::cout << "sizeof(msg) = " << sizeof(msg) << " strlen(msg) = " << strlen(msg) << std::endl;
	std::string str = msg;
	std::cout << "str.length() = " << str.length() << std::endl;

	memcpy(msg, hello, sizeof(hello));

	std::cout << "sizeof(msg) = " << sizeof(msg) << " strlen(msg) = " << strlen(msg) << std::endl;
	str = msg;
	std::cout << "str.length() = " << str.length() << std::endl;

	return;
}

void test3()
{
	char msg[] = { 0, 0, 0, 0, 1, 0, 0, 0 };
	std::cout << "sizeof(msg) = " << sizeof(msg) << " strlen(msg) = " << strlen(msg) << std::endl;
	return;
}

int main()
{
  test1();
  test2();
  test3();
  getchar();
}
