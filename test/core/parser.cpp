#include <gtest/gtest.h>

#include "../../inc/core/basic_parser_conbinators.hpp"
#include "../../inc/core/conbinator.hpp"
#include "../../inc/core/parser.hpp"

using d1::core::basic_parser_combinator::ParseChar;
using d1::core::basic_parser_combinator::ParseOneOfChars;
using d1::core::basic_parser_combinator::ParseString;
using d1::core::combinator::Any;
using d1::core::combinator::Combine;
using d1::core::combinator::Exactly;
using d1::core::combinator::Many;

using namespace std::literals;

TEST(BasicParserCombinators, StringParser)
{
    constexpr auto message_parser = ParseString(".message"sv);

    constexpr auto result = message_parser(".message @loki this is a message."sv);

    static_assert(result.unwrap().first == ".message"sv);
    static_assert(result.unwrap().second == " @loki this is a message."sv);
}

TEST(BasicParserCombinators, IntWithoutRangeExceeded)
{
    constexpr auto digit_parser = ParseOneOfChars("0123456789"sv);
    constexpr auto int_parser   = Many(digit_parser, 0, [](int acc, char ch) { return acc * 10 + (ch - '0'); });

    constexpr auto result = int_parser("12345def"sv);

    static_assert(result.unwrap().first == 12345);
    static_assert(result.unwrap().second == "def"sv);
}