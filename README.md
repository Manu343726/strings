# String

Just another strings library. No serious purposes, but a way to practice and improve my C++ skills. 

# Contents

This library aims to implement the most common implementations for the string data structure: A basic data structre in all CS courses, but which implementation is subject of lot of techniques and optimizations to improve its performance on different scenarios.   
This techniques are not usually covered by any CS course, despite the fact this are needed to provide enough performance for real world usage.

## Copy On Write string

The usual [copy-on-write idiom](https://en.wikipedia.org/wiki/Copy-on-write) applied to strings. This technique consists in delaying buffer copy (hence allocation) until the "cloned" string requests a write. 

I have a simple example of cow strings as a gist [here](https://gist.github.com/Manu343726/02287de75bb24f2cef00), which I wrote for teaching purposes. It was not written with performance in mind (Uses `std::shared_ptr` under the hood for buffer sharing), but to understand the technique.

GNU stdlibc++'s `std::string` was usually built using this technique, but C++11 introduced changes on iterator invalidation requirements preventing using COW. 

## Short String Optimization

Consists in adding a static buffer to the string type itself, so when strings are short enough to fit in this buffer, it's used as storage directly, avoiding dynamic memory allocation and improving cache friendliness.

```
template<typename Char>
struct sso_string
{
	TODO ¬¬

private:
	Char* dynamic_buffer_;
	Char static_buffer_[20];
};
```

## Short String Optimization using tagged pointers

Based on [this article](). Due to x86_64 architecture, even if modern processors have 64bit addressing, only 48bits are used for addressing an up to 256TB memory space. This is enough RAM these days, and adding pins and lines to communicate between CPU and RAM takes money.

The idea is to use the remaining 16 higher bits to store data. In my case, `strings::basic_tagged_string` type has a tagged pointer as member with the following layout:

```
On short string mode:

64             63                48                      0
+--------------+-----------------+-----------------------+
|       1      |         5       |        "hello"        |
+--------------+-----------------+-----------------------+
 <------------> <---------------> <--------------------->
  short string    string length           string 
      flag

On wide string mode:

64             63                48                      0
+--------------+-----------------+-----------------------+    @0x0ab3211c225f1
|       0      |        11       |    0x0ab3211c225f1    | ------------------> "hello world"
+--------------+-----------------+-----------------------+
 <------------> <---------------> <--------------------->
  short string    string length         pointer to
      flag                                string
```

The string operates in two different modes:

 - **Short**: When the string is short enough, string characters are stored directly in the first 48 bits of the pointer. The pointer becomes no longer a pointer but a container for string data. The next 15 bits store string length (Both in short and wide modes), and MSB acts as a flag to indicate current operating mode (1=short, 0=wide).

 - **Wide**: The string does not fit in the pointer, so usual dynamic memory allocation is done. The buffer address is stored in the pointer.


The implementation starts with a wrapper around a tagged pointer, `strings::detail::tagged_pointer<T>` that provides `::data()`, `address()`, `::pointer()`, etc accessors for easy manipulation of tagged pointer data:

``` cpp
int main()
{
    int i
    strings::detail::tagged_pointer<int> ptr = &i;

    ptr.data() = 0xabcd;
    *ptr = 5;

    std::cout << ptr.data(); // Gives 0xabcd
    std::cout << ptr.address(); // Gives i address (0xWHATEVER)
    std::cout << ptr.pointer(); // Gives pointer to i
    std::cout << i; // Gives 5
}
```

Finally, `strings::detail::tagged_pointer<T>` type is built upon `strings::detail::bitchunk<T>`, an integer wrapper for simple bit access like `std::bitset`, but with manipulation of chunks of bits in mind, instead of individual bits:

``` cpp
int main()
{
    strings::detail::bitchunk<int> i = 0xFFFFF00F;

    i(4,8) = 0xA;
    std::cout << i(0,4);   // Gives 0xF 
    std::cout << i(4,8);   // Gives 0xA
    std::cout << i(8,12);  // Gives 0x0
    std::cout << i(12,16); // Gives 0xF
    ...
}
```

# License

This project is licensed under the terms of the MIT license. See `LICENSE.md` file for more details.