/* SHA1_HASH class implement
 *
 * Kingsoft Seasun, chen tianyu 2018/12/07
 *
 * In this file, we define sha1_hash class.
 * This type holds a SHA-1 digest or any other kind of 20 byte
 * sequence. It implements a number of convenience functions, such
 * as bit operations, comparison operators etc.
 *
 * In our P2P download, it is primarily used to hold info-hashes, piece-hashes,
 * peer IDs, node IDs etc.
 */
#include "sha1_hash.hpp"

int hex_to_int(char in)
{
	if (in >= '0' && in <= '9') return int(in) - '0';
	if (in >= 'A' && in <= 'F') return int(in) - 'A' + 10;
	if (in >= 'a' && in <= 'f') return int(in) - 'a' + 10;
	return -1;
}

int hexs_to_int(char *in, int len)
{
	int val = 0;
	int base, multi;
	for (int i = 0; i < len; i++)
	{
		base = int(*(in + i));
		multi = pow(2, 8 * (len - i - 1));
		val += base * multi;
	}
	return val;
}

int hexs_to_int2(char *in, int len)
{
	int val = 0;
	int base, multi = pow(10, len - 1);
	while (len)
	{
		base = hex_to_int(*in);
		if (base == -1)
		{
			return -1;
		}
		val += base * multi;
		len--;
		in++;
		multi /= 10;
	}
	return val;
}

void int_to_hex(int val, char *ptr)
{
	for (int i = int(sizeof(val)) - 1; i >= 0; --i)
	{
		*ptr = static_cast<unsigned char>((val >> (i * 8)) & 0xff);
		++ptr;
	}
}

void to_hex(char const *in, int len, char* out)
{
	for (char const* end = in + len; in < end; ++in)
	{
		*out++ = hex_chars[unsigned char(*in) >> 4];
		*out++ = hex_chars[unsigned char(*in) & 0xf];
	}
	*out = '\0';
}

bool from_hex(char const *in, int len, char* out)
{
	for (char const* end = in + len; in < end; ++in, ++out)
	{
		int t = hex_to_int(*in);
		if (t == -1)
			return false;
		*out = t << 4;
		++in;
		t = hex_to_int(*in);
		if (t == -1)
			return false;
		*out |= t & 15;
	}
	return true;
}
