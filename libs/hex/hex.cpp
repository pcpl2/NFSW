#include <stdexcept>
#include "hex.h"

const char lookup[32] =
{ 0, 10, 11, 12, 13, 14, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0 };

std::string HEX2STR(std::string str)
{
	std::string out;
	out.reserve(str.size() / 2);

	const char* tmp = str.c_str();

	unsigned char ch = 0, last = 1;
	while (*tmp)
	{
		ch <<= 4;
		ch |= lookup[*tmp & 0x1f];
		if (last ^= 1)
			out += ch;
		tmp++;
	}
	return out;
}