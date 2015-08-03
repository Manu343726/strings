#ifndef BITCHUNK_HPP
#define BITCHUNK_HPP

#include "utility.hpp"

#include <type_traits>

namespace strings
{
	namespace detail
	{
		template<typename T>
		struct bitchunk
		{
			//static_assert(std::is_integral<T>::value || std::is_pointer<T>::value, "bitchunk only holds integrals or pointers");

			bitchunk(T data) :
				data_{ data }
			{}

			template<typename T_ = T>
			struct chunk_accessor;

			raw_data_t operator()(bit_index_t begin, bit_index_t end) const
			{
				return read_chunk(data_, begin, end);
			}

			chunk_accessor<> operator()(bit_index_t begin, bit_index_t end)
			{
				return chunk_accessor<>(&data_, begin, end);
			}

			raw_data_t operator()(bit_index_t bit) const
			{
				return (*this)(bit, bit + 1);
			}

			chunk_accessor<> operator()(bit_index_t bit)
			{
				return (*this)(bit, bit + 1);
			}

			T get() const
			{
				return data_;
			}

			T& get()
			{
				return data_;
			}

			bitchunk& operator=(T data)
			{
				data_ = data;

				return *this;
			}

		private:
			T data_;

			static raw_data_t read_chunk(T data, bit_index_t begin, bit_index_t end)
			{
				raw_data_t mask = ((0ull - 1ull) >> (sizeof_bits<T>() - end));
				raw_data_t result = (((raw_data_t)data) & mask) >> begin;
				return result;
			}

			template<typename T_ = T>
			struct chunk_accessor
			{
				using chunk_accessor_tag = void;

				using value_type = T;

				chunk_accessor() = default;

				chunk_accessor(T* data_ref, bit_index_t begin, bit_index_t end) :
					begin{ begin },
					end{ end },
					data_ref_{ data_ref }
				{}

				raw_data_t get() const
				{
					return read_chunk(*data_ref_, begin, end);
				}

				operator raw_data_t() const
				{
					return get();
				}

				template<typename U>
				raw_data_t operator=(const U& data)
				{
					return operator=((raw_data_t)data);
				}

				raw_data_t operator=(raw_data_t data)
				{
					bit_index_t data_width = end - begin;
					raw_data_t truncated = data & ((0ull - 1ull) >> (sizeof_bits<T>() - data_width));
					raw_data_t hi = (raw_data_t)*data_ref_ >> (end - 1);
					raw_data_t lo = (raw_data_t)*data_ref_ & ((0ull - 1ull) >> (sizeof_bits<T>() - 1 - begin));
					raw_data_t result = (hi << (end - 1)) | (truncated << begin) | lo;

					assign_data<T>::apply(data_ref_, result);

					return result;
				}

				template<typename U, typename = void>
				struct assign_data
				{
					static void apply(T* data_ref, raw_data_t data)
					{
						*data_ref = (T)data;
					}
				};

				template<typename U>
				struct assign_data<U, std::enable_if_t<!std::is_integral<U>::value && !std::is_pointer<U>::value>>
				{
					static void apply(T* data_ref, raw_data_t data)
					{
						*data_ref = data;
					}
				};

				
				// Note this proxy class is not designed to be assigned, instead performs asignment of referenced chunk

				template<typename U>
				raw_data_t operator=(const chunk_accessor<U>& acc)
				{
					return (*this) = *acc.data_ref_;
				}

				template<typename U>
				raw_data_t operator=(chunk_accessor<U>&& acc)
				{
					return (*this) = *acc.data_ref_;
				}

				// The above two operators break The Rule Of Five, let's fix with default semantics:

				chunk_accessor(const chunk_accessor&) = default;
				chunk_accessor(chunk_accessor&&) = default;
				~chunk_accessor() = default;

			private:
				T* data_ref_ = nullptr;
				bit_index_t begin = 0, end = 0;
			};
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
