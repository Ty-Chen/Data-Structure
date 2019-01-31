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

void test4()
{
	char msg[] = { 0, 0, 0, 1, 5 };
	char choke[] = { 0, 0, 0, 1, 0 };

	int cmp1 = strcmp(msg, choke);
	int cmp2 = strcmp(choke, msg);
	std::cout << cmp1 << std::endl;
	std::cout << cmp2 << std::endl;

}

void test5()
{
	char msg[] = { 0, 0, 0, 1, 5 };
	char interest[] = { 0, 0, 0, 1, 2 };
	char unintrest[] = { 0, 0, 0, 1, 3 };

	char tmp = *(msg + 4);
	char tmp2 = *(interest + 4);
	char tmp3 = *(unintrest + 4);

	if (tmp == '\x5')
	{ 
		std::cout << "tmp equal" << std::endl;
	}

	if (tmp2 == '\x2')
	{
		std::cout << "tmp2 equal" << std::endl;
	}

	if (tmp3 == '\x3')
	{
		std::cout << "tmp3 equal" << std::endl;
	}
}

void int_to_hex(int val, char *ptr)
{
	for (int i = int(sizeof(val)) - 1; i >= 0; --i)
	{
		*ptr = static_cast<unsigned char>((val >> (i * 8)) & 0xff);
		++ptr;
	}
}

int hex_to_int(char in)
{
	if (in >= '0' && in <= '9') return int(in) - '0';
	if (in >= 'A' && in <= 'F') return int(in) - 'A' + 10;
	if (in >= 'a' && in <= 'f') return int(in) - 'a' + 10;
	return -1;
}

int hexs_to_int(char *in, int len)
{
	int val = 0, count = 0;
	while (len >= 0)
	{
		val += hex_to_int(in[len]) * (10 ^ count);
		count++;
		len--;
	}
	return val;
}

int hexLen(char *in, int len)
{
	int val = 0;
	int base, multi;
	for (int i = 0; i < len; i ++)
	{
		base = int(*(in + i));
		multi = pow(2, 8 * (len - i -1));
		std::cout << "base = " << base << ", multi = " << multi << std::endl;
		val += base * multi;
	}
	std::cout << std::endl;
	return val;
}

void test6()
{
	char ch[8] = {0, 0, 0, 1};
	int_to_hex(10000, ch);
	int_to_hex(6666, ch + 4);
	
	int len = hexLen(ch, 4);
	int num = hexLen(ch + 4, 4);

	std::cout << "len = " << len << ", num = " << num << std::endl;
}

struct peer_request
{
	// the index of the piece in which the range starts.
	int piece;
	// the offset within that piece where the range starts.
	int start;
	// the size of the range, in bytes.
	int length;

	/* returns true if the right hand side peer_request refers to the same
	 * range as this does.
	 */
	bool operator==(peer_request const& r) const
	{
		return piece == r.piece && start == r.start && length == r.length;
	}
};

void test7()
{
	peer_request pr;
	pr.length = 65536;
	pr.piece = 270;
	pr.start = (270 - 1) * 65536;

	char msg[17] = { 0, 0, 0, 13, 6 };
	char *ptr = msg + 5;
	int_to_hex(pr.piece, ptr); // index 270
	int_to_hex(pr.start, ptr + 4); // begin 
	int_to_hex(pr.length, ptr + 8); // length 65536

	peer_request r;

	r.piece = hexLen(msg + 5, 4);
	r.start = hexLen(msg + 9, 4);
	r.length = hexLen(msg + 13, 4);

	std::cout << "r.piece = " << r.piece << ", r.start = " << r.start << ", r.length = " << r.length << std::endl;

}


int main()
{
	//test1();

	//test2();

	//test3();

	//test4();

	//test5();

	//test6();

	test7();

    getchar();

}
