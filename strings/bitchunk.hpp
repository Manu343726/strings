#ifndef BITCHUNK_HPP
#define BITCHUNK_HPP

#include "utility.hpp"

#include <type_traits>
#include <utility>

namespace strings
{
	namespace detail
	{
		template<typename T, bool FullRange>
		struct bitchunk_base
		{
			bitchunk_base() = default;
			bitchunk_base(T* data_ref, bit_index_t begin = 0, bit_index_t end = sizeof_bits<T>()) :
				data_ref_{data_ref},
				begin_{ begin },
				end_{ end }
			{}

			bit_index_t begin() const
			{
				return begin_;
			}

			bit_index_t end() const
			{
				return end_;
			}

			const T& data() const
			{
				return *data_ref_;
			}

			T& data()
			{
				return *data_ref_;
			}

			T* data_ptr() const
			{
				return data_ref_;
			}

			T* data_ptr()
			{
				return data_ref_;
			}

		private:
			T* data_ref_ = nullptr;
			bit_index_t begin_ = 0, end_ = 0;
		};

		template<typename T>
		struct bitchunk_base<T, true>
		{
			bitchunk_base() = default;
			bitchunk_base(const T* data_ref, bit_index_t begin = 0, bit_index_t end = sizeof_bits<T>()) :
				data_{*data_ref}
			{}

			constexpr bit_index_t begin() const
			{
				return 0;
			}

			constexpr bit_index_t end() const
			{
				return sizeof_bits<T>();
			}

			const T& data() const
			{
				return data_;
			}

			T& data()
			{
				return data_;
			}

			T* data_ptr()
			{
				return &data_;
			}

			T* data_ptr() const
			{
				return &data_;
			}

		private:
			T data_;
		};

		template<typename T, bool FullRange = true>
		struct bitchunk : public bitchunk_base<T, FullRange>
		{
			using base = bitchunk_base<T, FullRange>;

			bitchunk() = default;

			bitchunk(const T& data, bit_index_t begin = 0, bit_index_t end = sizeof_bits<T>()) :
				base{ &data, begin, end }
			{}

			template<bool fullrange>
			bitchunk(bitchunk<T,fullrange>& other, bit_index_t begin = 0, bit_index_t end = sizeof_bits<T>()) :
				base{ other.data_ptr(), other.begin() + begin, other.begin() + end }
			{}

			raw_data_t operator()(bit_index_t begin, bit_index_t end) const
			{
				return read_chunk(base::data(), begin, end);
			}

			bitchunk<T,false> operator()(bit_index_t begin, bit_index_t end)
			{
				return { *this, begin, end };
			}

			raw_data_t operator()(bit_index_t bit) const
			{
				return (*this)(bit, bit + 1);
			}

			bitchunk<T, false> operator()(bit_index_t bit)
			{
				return (*this)(bit, bit + 1);
			}

			raw_data_t get() const
			{
				return read_chunk(base::data(), base::begin(), base::end());
			}

			operator raw_data_t() const
			{
				return get();
			}

			template<bool fullrange = FullRange, typename = std::enable_if_t<fullrange>>
			bitchunk& operator=(T data) {
				base::data() = data;

				return *this;
			}

			template<typename U>
			bitchunk& operator=(U data)
			{
				raw_data_t raw_data_ = raw_data();
				raw_data_t truncated = truncate(data, base::end() - base::begin());
				raw_data_t hi = high_part(raw_data_, base::end() - 1);
				raw_data_t lo = low_part(raw_data_, base::begin());
				raw_data_t result = (hi << (base::end() - 1)) | (truncated << base::begin()) | lo;

				raw_manip::set_by_ptr(base::data_ptr(), result);

				return *this;
			}

		private:
			raw_data_t raw_data() const
			{
				return raw_manip::get_by_ptr(base::data_ptr());
			}
		};

		namespace
		{
			template<typename T, typename = void>
			struct make_chunk_impl_
			{
				template<typename U>
				static bitchunk<T> apply(U&& i)
				{
					return{ std::forward<U>(i) };
				}
			};

			template<typename T>
			struct make_chunk_impl_<T, typename T::chunk_accessor_tag>
			{
				//static_assert(sizeof(T) != sizeof(T), "make_chunk_impl_<chunk_accessor>");

				template<typename U>
				static auto apply(U&& acc) -> bitchunk<std::decay_t<decltype(acc.get())>>
				{
					return{ acc.data_ref_,  };
				}
			};
		}

		template<typename T>
		bitchunk<std::decay_t<T>> make_bitchunk(T&& i)
		{
			return { std::forward<T>(i) };
		}
	}
}

#endif
