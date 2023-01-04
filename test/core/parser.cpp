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

using namespace d1::core::basic_parser_combinator::literals;
using namespace std::literals;

TEST(BasicParserCombinators, StringParser)
{
    constexpr auto message_parser = ParseString(".message"sv);

    constexpr auto result = message_parser(".message @loki this is a message."sv);

    static_assert(result.unwrap().first == ".message"sv);
    static_assert(result.unwrap().second == " @loki this is a message."sv);
}

TEST(BasicParserCombinators, CreateIntParserWithoutRangeExceeded)
{
    constexpr auto digit_parser = ParseOneOfChars("0123456789"sv);
    constexpr auto int_parser   = Many(digit_parser, 0, [](int acc, char ch) { return acc * 10 + (ch - '0'); });

    constexpr auto result = int_parser("12345def"sv);

    static_assert(result.unwrap().first == 12345);
    static_assert(result.unwrap().second == "def"sv);
}

TEST(BasicParserCombinators, Alphabet)
{
    constexpr auto result = alphabet_parser("a");

    static_assert(result.unwrap().first == 'a');
    static_assert(result.unwrap().second == ""sv);
}

TEST(BasicParserCombinators, Int32WithRangeExceeded)
{
    constexpr auto result_pos1 = int32_parser("2147483647"sv);
    constexpr auto result_pos2 = int32_parser("+2147483648"sv);

    static_assert(result_pos1.unwrap().first == 2147483647);
    static_assert(result_pos1.unwrap().second == ""sv);
    static_assert(result_pos2.unwrap().first == 214748364);
    static_assert(result_pos2.unwrap().second == "8"sv);

    constexpr auto result_neg1 = int32_parser("-2147483648"sv);
    constexpr auto result_neg2 = int32_parser("-2147483649"sv);

    static_assert(result_neg1.unwrap().first == -2147483648);
    static_assert(result_neg1.unwrap().second == ""sv);
    static_assert(result_neg2.unwrap().first == -214748364);
    static_assert(result_neg2.unwrap().second == "9"sv);

    constexpr auto result_none  = int32_parser("+"sv);
    constexpr auto result_none2 = int32_parser("-"sv);

    static_assert(result_none.is_none());
    static_assert(result_none2.is_none());
}

TEST(BasicParserCombinators, Uint32WithRangeExceeded)
{
    constexpr auto result  = uint32_parser("4294967295"sv);
    constexpr auto result2 = uint32_parser("4294967296"sv);

    static_assert(result.unwrap().first == 4294967295);
    static_assert(result.unwrap().second == ""sv);

    static_assert(result2.unwrap().first == 429496729);
    static_assert(result2.unwrap().second == "6"sv);

    constexpr auto none = uint32_parser(""sv);

    static_assert(none.is_none());
}

TEST(BasicParserCombinators, Int64WithRangeExceeded)
{
    constexpr auto result_pos1 = int64_parser("9223372036854775807"sv);
    constexpr auto result_pos2 = int64_parser("+9223372036854775808"sv);

    static_assert(result_pos1.unwrap().first == 9223372036854775807);
    static_assert(result_pos1.unwrap().second == ""sv);
    static_assert(result_pos2.unwrap().first == 922337203685477580);
    static_assert(result_pos2.unwrap().second == "8"sv);

    constexpr auto result_neg1 = int64_parser("-9223372036854775808"sv);
    constexpr auto result_neg2 = int64_parser("-9223372036854775809"sv);

    static_assert(result_neg1.unwrap().first == -9223372036854775808);
    static_assert(result_neg1.unwrap().second == ""sv);
    static_assert(result_neg2.unwrap().first == -922337203685477580);
    static_assert(result_neg2.unwrap().second == "9"sv);

    constexpr auto result_none  = int64_parser("+"sv);
    constexpr auto result_none2 = int64_parser("-"sv);

    static_assert(result_none.is_none());
    static_assert(result_none2.is_none());
}

TEST(BasicParserCombinators, Uint64WithRangeExceeded)
{
    constexpr auto result  = uint64_parser("18446744073709551615"sv);
    constexpr auto result2 = uint64_parser("18446744073709551616"sv);

    static_assert(result.unwrap().first == 18446744073709551615);
    static_assert(result.unwrap().second == ""sv);

    static_assert(result2.unwrap().first == 1844674407370955161);
    static_assert(result2.unwrap().second == "6"sv);

    constexpr auto none = uint64_parser(""sv);

    static_assert(none.is_none());
}