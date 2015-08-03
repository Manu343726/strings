#ifdef _MSC_VER
#define _SCL_SECURE_NO_WARNINGS
#endif

#include "tagged_string.hpp"

#include <iostream>

int main()
{
	strings::tagged_string str = "hello, I'm no longer a short string";
	strings::tagged_string short_string = "short!";

	std::cout << str << std::endl;
	std::cout << short_string << std::endl;

	std::cin.get();
}