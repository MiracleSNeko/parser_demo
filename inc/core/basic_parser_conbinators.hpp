#pragma once

#include <limits>
#include <string_view>

#include "../utils/algorithms.hpp"
#include "../utils/containers.hpp"
#include "../utils/option.hpp"
#include "./combinator.hpp"
#include "./parser.hpp"

using d1::core::combinator::Combine;
using d1::core::combinator::DoWhile;
using d1::core::combinator::Try;
using d1::core::parser::Map;
using d1::core::parser::ParserInput;
using d1::core::parser::ParserOutput;
using d1::utils::containers::StaticString;
using d1::utils::option::NONE;
using d1::utils::option::None;
using d1::utils::option::SOME;
using d1::utils::option::Some;

namespace d1::core::basic_parser_combinator
{
using namespace d1::utils::option::operators;
using namespace d1::core::combinator::operators;
using namespace std::literals;

constexpr auto MODULE_NAME{"core/basic_parser_combinator.hpp"sv};

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
            const auto len = calgo::distance(pos.second, code.cend());
            return SOME(std::make_pair(str, ParserInput(pos.second, len)));
        }
        else
        {
            return NONE;
        }
    };
}

// parse a escaped char
constexpr auto ParseEscapeChar()
{
    constexpr auto backslash_parser    = ParseChar('\\');
    constexpr auto special_char_parser = ParseOneOfChars("abfnrtv'") || ParseChar('\\') || ParseChar('\"');

    constexpr auto convert_special_char = [](char c) {
        switch (c)
        {
            case 'a': return '\a';
            case 'b': return '\b';
            case 'f': return '\f';
            case 'n': return '\n';
            case 'r': return '\r';
            case 't': return '\t';
            case 'v': return '\v';
            default: return c;
        }
    };

    return Map(backslash_parser >> special_char_parser, convert_special_char);
}

constexpr auto ParseCStringChar()
{
    return ParseEscapeChar() || ParseNoneOfChars("\\\"");
}

template <std::size_t Capacity = 128>
constexpr auto ParseCString()
{
    return Many(ParseCStringChar(), StaticString<char, Capacity>(), [](auto acc, auto ch) {
        acc.push_back(ch);
        return acc;
    });
}

inline namespace literals
{
    // [a-zA-Z]
    constexpr auto alphabet_parser = ParseOneOfChars("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"sv);

    // [a-z]
    constexpr auto lowercase_parser = ParseOneOfChars("abcdefghijklmnopqrstuvwxyz"sv);

    // [A-Z]
    constexpr auto uppercase_parser = ParseOneOfChars("ABCDEFGHIJKLMNOPQRSTUVWXYZ"sv);

    // [0-9]
    constexpr auto digit_parser =
        Map(ParseOneOfChars("0123456789"sv), [](char ch) { return static_cast<int>(ch - '0'); });

    // Escape charactors
    constexpr auto escape_chara_parser = ParseEscapeChar();

    // c_str charactors
    constexpr auto c_str_chara_parser = ParseCStringChar();

    // c_str
    template <std::size_t Capacity = 128>
    constexpr auto c_str_parser = ParseCString<Capacity>();

}  // namespace literals

// parse a int32_value
constexpr auto ParseInt32()
{
    constexpr std::int32_t INT32_MAX_DIV_10 = std::numeric_limits<std::int32_t>::max() / 10;
    constexpr std::int32_t INT32_MAX_MOD_10 = std::numeric_limits<std::int32_t>::max() % 10;

    constexpr auto abs_parser = [](std::int32_t limit_dig) {
        return DoWhile(
            digit_parser, static_cast<std::uint32_t>(0), [](std::uint32_t acc, int dig) { return acc * 10 + dig; },
            [limit_dig](std::uint32_t acc, int dig) { return acc < INT32_MAX_DIV_10 || dig <= limit_dig; });
    };

    constexpr auto neg_parser = Combine(
        Map(ParseChar('-'), [](char _) { return -1; }), abs_parser(INT32_MAX_MOD_10 + 1),
        [](int _, std::uint32_t val) { return static_cast<std::int32_t>(-1 * static_cast<std::int64_t>(val)); });

    constexpr auto pos_parser =
        Combine(Try(Map(ParseChar('+'), [](char _) { return 1; }), 1), abs_parser(INT32_MAX_MOD_10),
                [](int _, std::uint32_t val) { return static_cast<std::int32_t>(val); });

    // Only one of them will be satisfied, so choose any one of them is ok.
    return neg_parser || pos_parser;
}

// parse a int64_value
constexpr auto ParseInt64()
{
    constexpr std::int64_t INT64_MAX_DIV_10 = std::numeric_limits<std::int64_t>::max() / 10;
    constexpr std::int64_t INT64_MAX_MOD_10 = std::numeric_limits<std::int64_t>::max() % 10;

    constexpr auto abs_parser = [](std::int64_t limit_dig) {
        return DoWhile(
            digit_parser, static_cast<std::uint64_t>(0), [](std::uint64_t acc, int dig) { return acc * 10 + dig; },
            [limit_dig](std::uint64_t acc, int dig) { return acc < INT64_MAX_DIV_10 || dig <= limit_dig; });
    };

    constexpr auto neg_parser = Combine(
        Map(ParseChar('-'), [](char _) { return -1; }), abs_parser(INT64_MAX_MOD_10 + 1), [](int _, std::uint64_t val) {
            return val == static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max()) + 1 ?
                       std::numeric_limits<std::int64_t>::min() :
                       -1 * static_cast<std::int64_t>(val);
        });

    constexpr auto pos_parser =
        Combine(Try(Map(ParseChar('+'), [](char _) { return 1; }), 1), abs_parser(INT64_MAX_MOD_10),
                [](int _, std::uint64_t val) { return static_cast<std::int64_t>(val); });

    // Only one of them will be satisfied, so choose any one of them is ok.
    return neg_parser || pos_parser;
}

// parse a uint32_t value
constexpr auto ParseUint32()
{
    constexpr std::uint32_t UINT32_MAX_DIV_10 = std::numeric_limits<std::uint32_t>::max() / 10;
    constexpr std::uint32_t UINT32_MAX_MOD_10 = std::numeric_limits<std::uint32_t>::max() % 10;

    return DoWhile(
        digit_parser, static_cast<std::uint32_t>(0), [](std::uint32_t acc, int dig) { return acc * 10 + dig; },
        [](std::uint32_t acc, int dig) { return acc < UINT32_MAX_DIV_10 || dig <= UINT32_MAX_MOD_10; });
}

// parse a uint64_t value
constexpr auto ParseUint64()
{
    constexpr std::uint64_t UINT64_MAX_DEV_10 = std::numeric_limits<std::uint64_t>::max() / 10;
    constexpr std::uint64_t UINT64_MAX_MOD_10 = std::numeric_limits<std::uint64_t>::max() % 10;

    return DoWhile(
        digit_parser, static_cast<std::uint64_t>(0), [](std::uint64_t acc, int dig) { return acc * 10 + dig; },
        [](std::uint64_t acc, int dig) { return acc < UINT64_MAX_DEV_10 || dig <= UINT64_MAX_MOD_10; });
}

inline namespace literals
{
    // int32_t
    constexpr auto int32_parser = ParseInt32();

    // int64_t
    constexpr auto int64_parser = ParseInt64();

    // uint32_t
    constexpr auto uint32_parser = ParseUint32();

    // uint64_t
    constexpr auto uint64_parser = ParseUint64();
}  // namespace literals

}  // namespace d1::core::basic_parser_combinator