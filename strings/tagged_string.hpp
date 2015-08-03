#ifndef TAGGED_STRING_HPP
#define TAGGED_STRING_HPP

#include "tagged_ptr.hpp"

#include <memory>

namespace strings
{
	template<typename Char, typename Alloc = std::allocator<Char>>
	struct basic_tagged_string
	{
		template<std::size_t N>
		basic_tagged_string(const char(&str)[N])
		{
			if (N-1 <= short_string_threshold())
			{
				short_string_bit_() = true;
				copy_to_short_string_(str, N);
			}
			else
			{
				data_ = alloc_.allocate(N);
				std::copy(str, str + N, data_.pointer());
				short_string_bit_() = false;
			}

			string_length_() = N - 1; // N - 1, beware of \0
		}

		auto operator[](std::size_t i) const
		{
			return *(cbegin() + i);
		}

		auto operator[](std::size_t i)
		{
			return *(begin() + i);
		}

		std::size_t length() const
		{
			return string_length_();
		}

		std::size_t size() const
		{
			return length();
		}

		~basic_tagged_string()
		{
			if (!short_string_bit_())
				alloc_.deallocate(data_.pointer(), length());
		}

	private:
		static constexpr strings::detail::bit_index_t size_start_ = 48;
		static constexpr strings::detail::bit_index_t flag_start_ = 63;
		using ptr_t = strings::detail::tagged_ptr<Char>;

		/*
		 * On short string mode:
		 *
		 * 64             63                48                      0
		 * +--------------+-----------------+-----------------------+
		 * |       1      |         5       |        "hello"        |
		 * +--------------+-----------------+-----------------------+
		 *  <------------> <---------------> <--------------------->
		 *   short string    string length           string 
		 *       flag
		 *
		 * On wide string mode:
		 *
		 * 64             63                48                      0
		 * +--------------+-----------------+-----------------------+    @0x0ab3211c225f1
		 * |       0      |        11       |    0x0ab3211c225f1    | ------------------> "hello world"
		 * +--------------+-----------------+-----------------------+
		 *  <------------> <---------------> <--------------------->
		 *   short string    string length         pointer to
		 *       flag                                string
		 */

		// General methods

		constexpr std::size_t short_string_threshold() const
		{
			return 48 / CHAR_BIT;
		}

		constexpr std::size_t max_string_length() const
		{
			return 0x1 << (flag_start_ - size_start_);
		}

		bool short_string_bit_() const
		{
			return static_cast<bool>(data_(63));
		}

		auto short_string_bit_()
		{
			return data_(63);
		}

		std::size_t string_length_() const
		{
			return (std::size_t)(data_(48,63));
		}

		auto string_length_()
		{
			return data_(48,63);
		}

		// Short string mode methods

		static strings::detail::bit_index_t char_index_(std::size_t i)
		{
			return i * CHAR_BIT;
		}

		char short_string_get_char_(std::size_t i) const
		{
			auto index = char_index_(i);

			return (char)data_(index, index + CHAR_BIT);
		}

		auto short_string_get_char_(std::size_t i)
		{
			auto index = char_index_(i);

			return data_(index, index + CHAR_BIT);
		}

		void copy_to_short_string_(const Char* str, std::size_t length)
		{
			for (std::size_t i = 0; i < length - 1; ++i) // length - 1, don't copy \0
				short_string_get_char_(i) = str[i];
		}

		// Wide string methods

		char wide_string_get_char_(std::size_t i) const
		{
			return data_.pointer()[i];
		}

		char& wide_string_get_char_(std::size_t i)
		{
			return data_.pointer()[i];
		}

		ptr_t data_;
		Alloc alloc_;

		friend struct iterator_base;

		template<typename It>
		struct iterator_base
		{
			iterator_base(const basic_tagged_string* str, std::size_t index) :
				str_{ str },
				index_{ index }
			{}

			It& operator++()
			{
				index_++;

				return static_cast<It&>(*this);
			}

			It operator++(int)
			{
				It tmp = *this;
				(*this)++;
				return tmp;
			}

			It& operator--()
			{
				index_--;
				return static_cast<It&>(*this);
			}

			It operator--(int)
			{
				It tmp = *this;
				(*this)--;
				return tmp;
			}

			It& operator+=(std::size_t offset)
			{
				index_ += offset;
				return static_cast<It&>(*this);
			}

			It& operator-=(std::size_t offset)
			{
				index_ -= offset;
				return static_cast<It&>(*this);
			}

			friend bool operator==(It lhs, It rhs)
			{
				return lhs.index_ == rhs.index_;
			}

			friend bool operator!=(It lhs, It rhs)
			{
				return !(lhs == rhs);
			}

			friend It operator+(It lhs, std::size_t offset)
			{
				return lhs += offset;
			}

			friend It operator-(It lhs, std::size_t offset)
			{
				return lhs -= offset;
			}

			friend It operator+(std::size_t offset, It rhs)
			{
				return rhs + offset;
			}

			friend It operator-(std::size_t offset, It rhs)
			{
				return rhs - offset;
			}

		protected:
			class one_more_assign_proxy
			{
				char* char_ref_;
				strings::detail::bitchunk<Char*,false> acc_;

			public:
				one_more_assign_proxy(char* char_ref, decltype(acc_) acc) :
					char_ref_{ char_ref },
					acc_{ acc }
				{}

				one_more_assign_proxy(char* char_ref) : one_more_assign_proxy{ char_ref, decltype(acc_){} }
				{}

				one_more_assign_proxy(decltype(acc_) acc) : one_more_assign_proxy{ nullptr, acc }
				{}

				Char operator=(Char character)
				{
					if (char_ref_ == nullptr)
						acc_ = character;
					else
						*char_ref_ = character;
				}

				operator char() const
				{
					if (char_ref_ == nullptr)
						return acc_.get();
					else
						return *char_ref_;
				}
			};

			Char deref_read() const
			{
				if (str_->short_string_bit_())
					return str_->short_string_get_char_(index_);
				else
					return str_->wide_string_get_char_(index_);
			}
			
			one_more_assign_proxy deref_write()
			{
				if (str_->short_string_bit_())
					return { const_cast<basic_tagged_string*>(str_)->short_string_get_char_(index_) };
				else
					return{ &const_cast<basic_tagged_string*>(str_)->wide_string_get_char_(index_) };
			}
			
		private:
			const basic_tagged_string* str_ = nullptr;
			std::size_t index_ = 0;
		};

	public:
		struct iterator : iterator_base<iterator>
		{
			using base = iterator_base<iterator>;
			using base::base;

			auto operator*() const
			{
				return base::deref_read();
			}

			auto operator*()
			{
				return base::deref_write();
			}
		};

		struct const_iterator : iterator_base<const_iterator>
		{
			using base = iterator_base<const_iterator>;
			using base::base;

			auto operator*() const
			{
				return base::deref_read();
			}
		};

		iterator begin() const
		{
			return { this, 0 };
		}

		iterator end() const
		{
			return{ this, length() };
		}

		const_iterator cbegin() const
		{
			return{ this, 0 };
		}

		const_iterator cend() const
		{
			return{ this, length() };
		}

		friend std::ostream& operator<<(std::ostream& os, const basic_tagged_string& str)
		{
			for (auto c : str)
				os << c;
			return os << '\0';
		}
	};

	using tagged_string = basic_tagged_string<char, std::allocator<char>>;
}

#endif