#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <climits>
#include <cstdint>
#include <type_traits>

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

		template<typename T, typename = void>
		struct raw_data_accessor
		{
			static raw_data_t get(T* data_ref)
			{
				return (raw_data_t)(*data_ref);
			}

			static void set(T* data_ref, raw_data_t newdata)
			{
				*data_ref = (T)(newdata);
			}
		};

		template<typename T>
		struct raw_data_accessor<T, std::enable_if_t<std::is_floating_point<T>::value>>
		{
			static raw_data_t get(T* data_ref)
			{
				return *(reinterpret_cast<raw_data_t*>(data_ref));
			}

			static void set(T* data_ref, raw_data_t newdata)
			{
				*data_ref = *(reinterpret_cast<T*>(&newdata));
			}
		};

		namespace raw_manip
		{
			template<typename T>
			inline raw_data_t get(const T& value)
			{
				return raw_data_accessor<T>::get(&value);
			}

			template<typename T>
			inline raw_data_t get(T& value)
			{
				return raw_data_accessor<T>::get(&value);
			}

			template<typename T>
			inline raw_data_t get_by_ptr(T* value_ref)
			{
				return raw_data_accessor<T>::get(value_ref);
			}

			template<typename T>
			inline void set(T& value, raw_data_t new_value)
			{
				return raw_data_accessor<T>::set(&value, new_value);
			}

			template<typename T>
			inline void set_by_ptr(T* value_ref, raw_data_t new_value)
			{
				return raw_data_accessor<T>::set(value_ref, new_value);
			}
		}

		inline raw_data_t bitmask_ononly(std::size_t begin, std::size_t end)
		{
			return (allbitson >> (sizeof_bits<raw_data_t>() - end)) << begin;
		}

		inline raw_data_t bitmask_clear(std::size_t begin, std::size_t end)
		{
			return (begin < end) ? ~bitmask_ononly(begin, end) : allbitson;
		}

		template<typename T>
		inline raw_data_t clear_high_bits(T i, bit_index_t begin)
		{
			return raw_manip::get(i) & bitmask_clear(begin,sizeof_bits<T>());
		}

		template<typename T>
		inline raw_data_t truncate(T i, std::size_t width)
		{
			return clear_high_bits(i, width);
		}

		template<typename T>
		inline raw_data_t high_part(T i, bit_index_t pivot)
		{
			return raw_manip::get(i) >> pivot;
		}

		template<typename T>
		inline raw_data_t low_part(T i, bit_index_t pivot)
		{
			return clear_high_bits(i, pivot);
		}

		template<typename T>
		inline raw_data_t read_chunk(T data, bit_index_t begin, bit_index_t end)
		{
			return clear_high_bits(data, end) >> begin;
		}
	}
}

#endif
