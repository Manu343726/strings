#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <climits>

namespace strings
{
	namespace detail
	{
		using raw_data_t = unsigned long long int;
		using bit_index_t = unsigned int;

		template<typename T>
		constexpr std::size_t sizeof_bits()
		{
			return sizeof(T) * CHAR_BIT;
		}

		template<typename...>
		using void_t = void;
	}
}

#endif
