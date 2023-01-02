#pragma once

#include <string_view>

#include "../utils/algorithms.hpp"
#include "./conbinator.hpp"
#include "./parser.hpp"

using d1::utils::option::NONE;
using d1::utils::option::None;
using d1::utils::option::SOME;
using d1::utils::option::Some;

using d1::core::parser::ParserInput;
using d1::core::parser::ParserOutput;

using namespace std::literals;

namespace d1::core::basic_parser_combinator
{
// parse a given char
constexpr auto ParseChar(char ch)
{
    return [ch](ParserInput code) -> ParserOutput<char> {
        return (code.empty() || code[0] != ch) ?
                   NONE :
                   SOME(std::make_pair(ch, ParserInput(code.data() + 1, code.size() - 1)));
    };
}

// parse a char in given set
constexpr auto ParseOneOfChars(std::string_view chs)
{
    return [chs](ParserInput code) -> ParserOutput<char> {
        return (code.empty() || calgo::find(chs.cbegin(), chs.cend(), code[0]) == chs.cend()) ?
                   NONE :
                   SOME(std::make_pair(code[0], ParserInput(code.data() + 1, code.size() - 1)));
    };
}

// parse a char not in given set
constexpr auto ParseNoneOfChars(std::string_view chs)
{
    return [chs](ParserInput code) -> ParserOutput<char> {
        return (code.empty() || calgo::find(chs.cbegin(), chs.cend(), code[0]) != chs.cend()) ?
                   NONE :
                   SOME(std::make_pair(code[0], ParserInput(code.data() + 1, code.size() - 1)));
    };
}

// parse a given string
constexpr auto ParseString(std::string_view str)
{
    return [str](ParserInput code) -> ParserOutput<std::string_view> {
        const auto pos = calgo::mismatch(str.cbegin(), str.cend(), code.cbegin(), code.cend());
        if (pos.first == str.cend())
        {
            // match success
            const auto len = calgo::distance(code.cend(), pos.second);
            return SOME(std::make_pair(str, ParserInput(pos.second, len)));
        }
        else
        {
            return NONE;
        }
    };
}

}  // namespace d1::core::basic_parser_combinator