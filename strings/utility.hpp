#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <climits>
#include <cstdint>

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

		constexpr raw_data_t allbitson = (0ull - 1ull);

		raw_data_t bitmask_clear(std::size_t begin, std::size_t end)
		{
			raw_data_t lo = ~(allbitson << begin);
			raw_data_t hi = ~(allbitson << (end-1));

			return hi & lo;
		}

		template<typename T>
		raw_data_t clear_high_bits(T i, bit_index_t begin)
		{
			return (raw_data_t)i & bitmask_clear(begin, sizeof_bits<T>());
		}

		template<typename T>
		raw_data_t truncate(T i, std::size_t width)
		{
			return clear_high_bits(i, width);
		}

		template<typename T>
		raw_data_t high_part(T i, bit_index_t pivot)
		{
			return (raw_data_t)i >> pivot;
		}

		template<typename T>
		raw_data_t low_part(T i, bit_index_t pivot)
		{
			return clear_high_bits(i, pivot);
		}
	}
}

#endif
