#ifdef _MSC_VER
#define _SCL_SECURE_NO_WARNINGS
#endif

#include "tagged_string.hpp"

#include <iostream>

int main()
{
	strings::detail::bitchunk<unsigned int> i = strings::detail::make_bitchunk(0xFFFFFFFF);

	strings::tagged_string str = "h";

	for(char c : str)
		std::cout << c;

	std::cin.get();
}