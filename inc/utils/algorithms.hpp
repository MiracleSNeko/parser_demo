#pragma once

#include <concepts>
#include <cstddef>
#include <iterator>
#include <type_traits>

namespace d1::utils::algorithm
{

template <typename Predicator, typename Iterator>
using PredicatorResult = std::invoke_result_t<Predicator, typename std::iterator_traits<Iterator>::value_type>;

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
    requires std::same_as<PredicatorResult<Predicator, InputIterator>, bool>
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
    requires std::same_as<PredicatorResult<Predicator, InputIterator>, bool>
constexpr bool all(InputIterator first, InputIterator last, Predicator predicator)
{
    return find_if(first, last, [&predicator](auto elem) { return !predicator(elem); }) == last;
}

template <typename InputIterator, typename Predicator>
    requires std::same_as<PredicatorResult<Predicator, InputIterator>, bool>
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
    requires std::same_as<PredicatorResult<Predicator, InputIterator>, bool>
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

}  // namespace d1::utils::algorithm

// namespace alias
namespace calgo = d1::utils::algorithm;