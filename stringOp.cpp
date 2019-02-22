#include <iostream>

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

void int_to_hex(int val, char *ptr)
{
	for (int i = int(sizeof(val)) - 1; i >= 0; --i)
	{
		*ptr = static_cast<unsigned char>((val >> (i * 8)) & 0xff);
		++ptr;
	}
}

int hexs_to_int(char *in, int len)
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

void test1()
{
	char ch[8] = {0, 0, 0, 1};
	int_to_hex(10000, ch);
	int_to_hex(6666, ch + 4);
	
	int len = hexs_to_int(ch, 4);
	int num = hexs_to_int(ch + 4, 4);

	std::cout << "len = " << len << ", num = " << num << std::endl;
}

void test2()
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

	r.piece = hexs_to_int(msg + 5, 4);
	r.start = hexs_to_int(msg + 9, 4);
	r.length = hexs_to_int(msg + 13, 4);

	std::cout << "r.piece = " << r.piece << ", r.start = " << r.start << ", r.length = " << r.length << std::endl;

}


int main()
{
	test1();

	test2();

    getchar();

}
