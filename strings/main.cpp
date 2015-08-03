#ifdef _MSC_VER
#define _SCL_SECURE_NO_WARNINGS
#endif

#include "tagged_string.hpp"

#include <iostream>

int main()
{
	strings::detail::bitchunk<int> i = strings::detail::make_bitchunk(0);
	strings::tagged_string str = "hellooooooooo";

	int j = i(1,4)(1,3) = 0b11;

	std::cout << std::hex;
	std::cout << i(0,8) << std::endl;

	std::cout << std::endl << str << std::endl;

	std::cin.get();
}