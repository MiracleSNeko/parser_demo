#include <gtest/gtest.h>
#include <memory>

#include "../../inc/utils/algorithms.hpp"

TEST(ConstexprFind, FindSuccess)
{
    constexpr std::array<int, 5> vec{1, 2, 3, 4, 5};

    static_assert(calgo::find(vec.cbegin(), vec.cend(), 3) == std::addressof(vec[2]));
}

TEST(ConstexprFind, FindFailed)
{
    constexpr std::array<int, 5> vec{1, 2, 3, 4, 5};

    static_assert(calgo::find(vec.cbegin(), vec.cend(), 6) == vec.cend());
}

TEST(ConstexprFindIf, FindSuccess)
{
    constexpr std::array<int, 5> vec{1, 2, 3, 4, 5};

    static_assert(calgo::find_if(vec.cbegin(), vec.cend(), [](int i) { return i % 2 == 0; }) == std::addressof(vec[1]));
}

TEST(ConstexprFindIf, FindFailed)
{
    constexpr std::array<int, 5> vec{1, 2, 3, 4, 5};

    static_assert(calgo::find_if(vec.cbegin(), vec.cend(), [](int i) { return i == 0; }) == vec.cend());
}

TEST(ConstexprAll, True)
{
    constexpr std::array<int, 5> vec{1, 2, 3, 4, 5};

    static_assert(calgo::all(vec.cbegin(), vec.cend(), [](int i) { return i > 0; }));
}

TEST(ConstexprAll, False)
{
    constexpr std::array<int, 5> vec{1, 2, 3, 4, 5};

    static_assert(!calgo::all(vec.cbegin(), vec.cend(), [](int i) { return i > 1; }));
}

TEST(ConstexprAny, True)
{
    constexpr std::array<int, 5> vec{1, 2, 3, 4, 5};

    static_assert(calgo::any(vec.cbegin(), vec.cend(), [](int i) { return i >= 5; }));
}

TEST(ConstexprAny, False)
{
    constexpr std::array<int, 5> vec{1, 2, 3, 4, 5};

    static_assert(!calgo::any(vec.cbegin(), vec.cend(), [](int i) { return i >= 6; }));
}