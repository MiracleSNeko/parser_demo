#include <gtest/gtest.h>

#include <string_view>

#include "../inc/parser.hpp"

using namespace std::literals;

TEST(CompileTimeFormatter, SingleArgs)
{
    constexpr auto result = fmt::format_parser<128, 1024, std::string_view>("This is a {0}"sv, "test"sv);

    static_assert(result.unwrap().first == "This is a test"sv);
}

TEST(CompileTimeFormatter, MultiArgs)
{
    constexpr auto result = fmt::format_parser<128, 1024, std::string_view, std::string_view>(
        "This is a {1} test by {0}"sv, "ZYUJLIN"sv, "formatter"sv);

    static_assert(result.unwrap().first == "This is a formatter test by ZYUJLIN"sv);
}