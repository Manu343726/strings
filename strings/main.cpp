#ifdef _MSC_VER
#define _SCL_SECURE_NO_WARNINGS
#endif

#include "tagged_string.hpp"

#include <iostream>

int main()
{
	strings::detail::bitchunk<unsigned int> i = strings::detail::make_bitchunk(0xFFFFFFFF);
	strings::tagged_string str = "hello";
	strings::detail::tagged_ptr<strings::tagged_string> ptr = &str;

	std::cout << std::hex;
	std::cout << "length: " << str.length() << " (short: " << str.is_short() << ")\n";
	std::cout << "addr: " << ptr.address() << " (from " << &str << ")\n";

	for (int i = 0; i < str.length(); ++i)
		std::cout << str[i] << std::endl;

	std::cin.get();
}