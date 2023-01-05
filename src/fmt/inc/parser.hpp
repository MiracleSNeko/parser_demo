#pragma once

#include <bits/utility.h>
#include <string_view>
#include <tuple>
#include <utility>

#include "../../../inc/parser_demo"

namespace fmt
{

using namespace std::literals;

namespace __impl
{
    template <typename Tuple>
    constexpr std::string_view nth_formatted_element(std::size_t n, Tuple&& tuple, std::index_sequence<>)
    {
        return ""sv;
    }

    template <typename Tuple, std::size_t... Idx>
    constexpr std::string_view nth_formatted_element(std::size_t n, Tuple&& tuple, std::index_sequence<Idx...>)
    {
        constexpr std::size_t i = std::index_sequence<Idx...>::size() - 1;
        if (i == n)
        {
            // TODO: call constexpr `to_string()` here
            return static_cast<std::string_view>(std::get<i>(tuple));
        }
        else
        {
            return nth_formatted_element(n, tuple, std::make_index_sequence<i>{});
        }
    }

}  // namespace __impl

constexpr auto indexer_parser = d1::ParseChar('{') >> d1::uint64_parser << d1::ParseChar('}');

template <std::size_t Capacity>
constexpr auto str_parser =
    d1::Many(d1::ParseNoneOfChars("{}"), d1::StaticString<char, Capacity>(), [](auto acc, char ch) {
        acc.push_back(ch);
        return acc;
    });

template <std::size_t ElemCapacity, std::size_t Capacity, typename... Args>
// requrires Args has a constexpr `to_string()`
constexpr auto format_parser = [](d1::ParserInput code, Args&&... args) {
    const auto packed_args        = std::make_tuple(args...);
    const auto format_elem_parser = Combine(str_parser<ElemCapacity>, indexer_parser, [=](auto str, std::uint64_t n) {
        // then we can call nth_arg.to_string() here to build string at compile time
        const auto nth_arg = __impl::nth_formatted_element(
            n, packed_args, std::make_index_sequence<std::tuple_size_v<std::tuple<Args...>>>{});
        d1::copy(std::begin(nth_arg), std::end(nth_arg), d1::back_insert_iterator(str));
        return str;
    });
    return Many(format_elem_parser, d1::StaticString<char, Capacity>(), [](auto acc, auto str) {
        d1::copy(str.begin(), str.end(), d1::back_insert_iterator(acc));
        return acc;
    })(code);
};

}  // namespace fmt