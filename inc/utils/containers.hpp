#pragma once

#include "./algorithms.hpp"
#include "./iterator.hpp"

#include <array>
#include <concepts>
#include <initializer_list>
#include <iterator>
#include <stdexcept>
#include <utility>

namespace d1::utils::containers
{

namespace __impl
{
    constexpr auto oor_handle = []() { throw std::range_error("index out of range!"); };

    template <typename T, typename U>
    concept suitable_type = std::same_as<T, U> || std::convertible_to<U, T>;
}  // namespace __impl

// a 'vector' with static memory alloc
template <typename T, std::size_t Capacity = 16>
class StaticVector
{
public:
    using value_type      = T;
    using iterator        = typename std::array<T, Capacity>::iterator;
    using const_iterator  = typename std::array<T, Capacity>::const_iterator;
    using reference       = typename std::array<T, Capacity>::reference;
    using const_reference = typename std::array<T, Capacity>::const_reference;

    template <typename InputIterator>
        requires __impl::suitable_type<T, typename std::iterator_traits<InputIterator>::value_type>
    constexpr StaticVector(InputIterator first, const InputIterator& last)
    {
        for (; first != last; ++first)
        {
            push_back(*first);
        }
    }

    template <typename U>
        requires __impl::suitable_type<T, U>
    constexpr StaticVector(std::initializer_list<U> init) : StaticVector(init.begin(), init.end())
    {
    }

    constexpr StaticVector() = default;

    constexpr auto begin() const
    {
        return _data.begin();
    }
    constexpr auto begin()
    {
        return _data.begin();
    }
    constexpr auto cbegin() const
    {
        return _data.cbegin();
    }

    constexpr auto end() const
    {
        return std::next(_data.begin(), _size);
    }
    constexpr auto end()
    {
        return std::next(_data.begin(), _size);
    }
    constexpr auto cend() const
    {
        return std::next(_data.cbegin(), _size);
    }

    // like std::vector, operator[] has no index check.
    // if size <= i < capacity, T{} will be returned.
    // else, compile failed.
    constexpr const T& operator[](const std::size_t i) const noexcept
    {
        return _data[i];
    }
    constexpr const T& operator[](const std::size_t i) noexcept
    {
        return _data[i];
    }

    // but .at() has.
    constexpr const T& at(const std::size_t i) const
    {
        if (i >= _size)
        {
            __impl::oor_handle();
        }
        else
        {
            return _data[i];
        }
    }
    constexpr const T& at(const std::size_t i)
    {
        if (i >= _size)
        {
            __impl::oor_handle();
        }
        else
        {
            return _data[i];
        }
    }

    // SAFETY: before calling `back()`, please ensure `empty()` returns false.
    constexpr const T& back() const
    {
        return _data[_size - 1];
    }
    constexpr const T& back()
    {
        return _data[_size - 1];
    }

    constexpr auto capacity() const
    {
        return Capacity;
    }

    constexpr auto size() const
    {
        return _size;
    }

    constexpr bool empty() const
    {
        return _size == 0;
    }

    constexpr void clear()
    {
        _size = 0;
    }

    constexpr const T* data() const
    {
        return _data.data();
    }

    template <typename U>
        requires __impl::suitable_type<T, U>
    constexpr void push_back(U value)
    {
        if (_size >= Capacity)
        {
            __impl::oor_handle();
        }
        else
        {
            _data[_size++] = std::move(value);
        }
    }

private:
    std::array<T, Capacity> _data{};
    std::size_t             _size{0};
};

template <typename T, std::size_t Cap1, std::size_t Cap2>
constexpr bool operator==(const StaticVector<T, Cap1>& lhs, const StaticVector<T, Cap2>& rhs)
{
    return Cap1 == Cap2 && calgo::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

template <typename T, std::size_t Cap1, std::size_t Cap2>
constexpr bool operator!=(const StaticVector<T, Cap1>& lhs, const StaticVector<T, Cap2>& rhs)
{
    return !(lhs == rhs);
}

}  // namespace d1::utils::containers