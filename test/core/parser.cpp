#include <gtest/gtest.h>

#include "../../inc/core/basic_parser_conbinators.hpp"

using d1::core::basic_parser_combinator::ParseString;

using namespace std::literals;

TEST(BasicParserConbinators, StringParser)
{
    constexpr auto message_parser = ParseString(".message"sv);

    constexpr auto result = message_parser(".message @loki this is a message."sv);

    static_assert(result.unwrap().first == ".message"sv);
    static_assert(result.unwrap().second == " @loki this is a message."sv);
}