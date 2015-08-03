#ifndef TAGGED_PTR_HPP
#define TAGGED_PTR_HPP

#include "bitchunk.hpp"

namespace strings
{
	namespace detail
	{
		template<typename T, bit_index_t address_width = 48>
		struct tagged_ptr : public bitchunk<T*>
		{
			static_assert(sizeof(void*) == 8, "This only works on x86_64 archs with 48bit addressing");
			static_assert(sizeof(void*) == sizeof(raw_data_t), "Raw access to address bits will be done through raw_data_t. Its size should match ptr_t's");

			using chunk_t = bitchunk<T*>;

			tagged_ptr(T* ptr = nullptr) :
				chunk_t{ ptr }
			{}

			raw_data_t data() const
			{
				return chunk_t::operator()(address_width, sizeof_bits<T*>());
			}

			auto data()
			{
				return chunk_t::operator()(address_width, sizeof_bits<T*>());
			}

			raw_data_t address() const
			{
				return chunk_t::operator()(0, address_width);
			}

			auto address()
			{
				return chunk_t::operator()(0, address_width);
			}

			T* pointer() const
			{
				return reinterpret_cast<T*>(address());
			}

			const T& operator*() const
			{
				return *pointer();
			}

			T& operator*()
			{
				return *pointer();
			}
		};
	}
}

#endif