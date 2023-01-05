#pragma once

#include <concepts>
#include <iterator>
#include <type_traits>

namespace d1::utils::algorithm
{
using namespace std::literals;

constexpr auto MODULE_NAME{"utils/algorithms.hpp"sv};

namespace __impl
{
    template <typename Predicator, typename Iterator>
    concept iter_predicatable = std::predicate<Predicator, typename std::iterator_traits<Iterator>::value_type>;
}  // namespace __impl

// The following functions should be constexpr in std...
template <typename InputIterator, typename T>
constexpr InputIterator find(InputIterator first, InputIterator last, const T& value)
{
    for (; first != last; ++first)
    {
        if (*first == value)
        {
            return first;
        }
    }
    return last;
}

template <typename InputIterator, typename Predicator>
    requires __impl::iter_predicatable<Predicator, InputIterator>
constexpr InputIterator find_if(InputIterator first, InputIterator last, Predicator predicator)
{
    for (; first != last; ++first)
    {
        if (predicator(*first))
        {
            return first;
        }
    }
    return last;
}

template <typename InputIterator, typename Predicator>
    requires __impl::iter_predicatable<Predicator, InputIterator>
constexpr bool all(InputIterator first, InputIterator last, Predicator predicator)
{
    return find_if(first, last, [&predicator](auto elem) { return !predicator(elem); }) == last;
}

template <typename InputIterator, typename Predicator>
    requires __impl::iter_predicatable<Predicator, InputIterator>
constexpr bool any(InputIterator first, InputIterator last, Predicator predicator)
{
    return find_if(first, last, predicator) != last;
}

template <typename InputIterator>
constexpr auto count(InputIterator first, InputIterator last)
{
    typename std::iterator_traits<InputIterator>::difference_type diff = 0;
    for (; first != last; ++first)
    {
        ++diff;
    }
    return diff;
}

template <typename InputIterator, typename Predicator>
    requires __impl::iter_predicatable<Predicator, InputIterator>
constexpr auto count_if(InputIterator first, InputIterator last, Predicator predicator)
{
    typename std::iterator_traits<InputIterator>::difference_type diff = 0;
    for (; first != last; ++first)
    {
        if (predicator(*first))
        {
            ++diff;
        }
    }
    return diff;
}

template <typename InputIterator1, typename InputIterator2>
constexpr std::pair<InputIterator1, InputIterator2> mismatch(InputIterator1 first1, InputIterator1 last1,
                                                             InputIterator2 first2, InputIterator2 last2)
{
    while (first1 != last1 && first2 != last2 && *first1 == *first2)
    {
        ++first1;
        ++first2;
    }
    return std::make_pair(first1, first2);
}

template <typename InputIterator1, typename InputIterator2>
constexpr bool equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2)
{
    while (first1 != last1 && first2 != last2 && *first1 == *first2)
    {
        ++first1;
        ++first2;
    }
    return first1 == last1 && first2 == last2;
}

template <typename InputIterator>
constexpr auto distance(InputIterator first, InputIterator last)
{
    using diff_t = typename std::iterator_traits<InputIterator>::difference_type;

    return static_cast<diff_t>(last - first);
}

template <typename InputIterator, typename OutputIterator>
constexpr OutputIterator copy(InputIterator src_first, InputIterator src_last, OutputIterator dest)
{
    for (; src_first != src_last; ++src_first)
    {
        *dest = *src_first;
        ++dest;
    }
    return dest;
}

template <typename InputIterator, typename OutputIterator, typename Predicator>
    requires __impl::iter_predicatable<Predicator, InputIterator>
constexpr OutputIterator copy_if(InputIterator src_first, InputIterator src_last, OutputIterator dest,
                                 Predicator predicator)
{
    for (; src_first != src_last; ++src_first)
    {
        if (predicator(*src_first))
        {
            *dest = *src_first;
            ++dest;
        }
    }
    return dest;
}

template <typename InputIterator, typename OutputIterator>
constexpr OutputIterator copy_n(InputIterator src_first, InputIterator src_last, OutputIterator dest, std::size_t n)
{
    for (; n != 0 && src_first != src_last; ++src_first, --n)
    {
        *dest = *src_first;
        ++dest;
    }
    return dest;
}

template <typename InputIterator, typename OutputIterator>
constexpr OutputIterator move(InputIterator src_first, InputIterator src_last, OutputIterator dest)
{
    for (; src_first != src_last; ++src_first)
    {
        *dest = std::move(*src_first);
    }
    return dest;
}

template <typename InputIterator, typename OutputIterator, typename Predicator>
    requires __impl::iter_predicatable<Predicator, InputIterator>
constexpr OutputIterator move_if(InputIterator src_first, InputIterator src_last, OutputIterator dest,
                                 Predicator predicator)
{
    for (; src_first != src_last; ++src_first)
    {
        if (predicator(*src_first))
        {
            *dest = std::move(*src_first);
            ++dest;
        }
    }
    return dest;
}

template <typename InputIterator, typename OutputIterator>
constexpr OutputIterator move_n(InputIterator src_first, InputIterator src_last, OutputIterator dest, std::size_t n)
{
    for (; n != 0 && src_first != src_last; ++src_first, --n)
    {
        *dest = std::move(*src_first);
        ++dest;
    }
    return dest;
}

template <typename InputIterator, typename T>
    requires std::convertible_to<T, typename std::iterator_traits<InputIterator>::value_type>
constexpr void fill(InputIterator src_first, InputIterator src_last, T value)
{
    for (; src_first != src_last; ++src_first)
    {
        *src_first = value;
    }
}

template <typename InputIterator, typename T>
    requires std::convertible_to<T, typename std::iterator_traits<InputIterator>::value_type>
constexpr void fill_n(InputIterator src_first, InputIterator src_last, T value, std::size_t n)
{
    for (; n != 0 && src_first != src_last; ++src_first, --n)
    {
        *src_first = value;
    }
}

}  // namespace d1::utils::algorithm

// namespace alias
namespace calgo = d1::utils::algorithm;