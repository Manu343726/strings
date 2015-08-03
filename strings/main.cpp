#ifdef _MSC_VER
#define _SCL_SECURE_NO_WARNINGS
#endif

#include "tagged_string.hpp"

#include <iostream>

int main()
{
	auto i = strings::detail::make_bitchunk(0x00000000);

	i(0, 4) = 0xf;

	std::cout << std::hex;
	std::cout << i << std::endl;

	std::cin.get();
}