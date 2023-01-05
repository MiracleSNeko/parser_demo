#include <gtest/gtest.h>
#include <memory>

#include "../../inc/parser_demo"

using namespace d1;
using namespace std::literals;

TEST(ConstexprFind, FindSuccess)
{
    constexpr std::array<int, 5> vec{1, 2, 3, 4, 5};

    static_assert(d1::find(vec.cbegin(), vec.cend(), 3) == std::addressof(vec[2]));
}

TEST(ConstexprFind, FindFailed)
{
    constexpr std::array<int, 5> vec{1, 2, 3, 4, 5};

    static_assert(d1::find(vec.cbegin(), vec.cend(), 6) == vec.cend());
}

TEST(ConstexprFindIf, FindSuccess)
{
    constexpr std::array<int, 5> vec{1, 2, 3, 4, 5};

    static_assert(d1::find_if(vec.cbegin(), vec.cend(), [](int i) { return i % 2 == 0; }) == std::addressof(vec[1]));
}

TEST(ConstexprFindIf, FindFailed)
{
    constexpr std::array<int, 5> vec{1, 2, 3, 4, 5};

    static_assert(d1::find_if(vec.cbegin(), vec.cend(), [](int i) { return i == 0; }) == vec.cend());
}

TEST(ConstexprAll, True)
{
    constexpr std::array<int, 5> vec{1, 2, 3, 4, 5};

    static_assert(d1::all(vec.cbegin(), vec.cend(), [](int i) { return i > 0; }));
}

TEST(ConstexprAll, False)
{
    constexpr std::array<int, 5> vec{1, 2, 3, 4, 5};

    static_assert(!d1::all(vec.cbegin(), vec.cend(), [](int i) { return i > 1; }));
}

TEST(ConstexprAny, True)
{
    constexpr std::array<int, 5> vec{1, 2, 3, 4, 5};

    static_assert(d1::any(vec.cbegin(), vec.cend(), [](int i) { return i >= 5; }));
}

TEST(ConstexprAny, False)
{
    constexpr std::array<int, 5> vec{1, 2, 3, 4, 5};

    static_assert(!d1::any(vec.cbegin(), vec.cend(), [](int i) { return i >= 6; }));
}

TEST(ConstexprCopy, StaticString)
{
    auto           s1 = StaticString<char, 1024>();
    constexpr auto s2 = "hello, world"sv;
    d1::copy(s2.begin(), s2.end(), d1::back_insert_iterator(s1));

    EXPECT_EQ(s1, "hello, world"sv);
}