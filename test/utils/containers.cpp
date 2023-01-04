#include <gtest/gtest.h>
#include <stdexcept>

#include "../../inc/utils/containers.hpp"

using d1::utils::containers::StaticVector;

TEST(StaticVector, Construct)
{
    constexpr StaticVector<int>  cvec{1, 2, 3, 4, 5};
    constexpr std::array<int, 5> carr{1, 2, 3, 4, 5};

    static_assert(calgo::equal(cvec.cbegin(), cvec.end(), carr.cbegin(), carr.cend()));
}

TEST(StaticVector, Empty)
{
    constexpr StaticVector<int> cvec;

    static_assert(cvec.empty());
}

TEST(StaticVector, Capacity)
{
    constexpr StaticVector<int> cvec;

    static_assert(cvec.capacity() == 16);
}

TEST(StaticVector, Size)
{
    constexpr StaticVector<int> cvec{1, 2, 3, 4, 5};

    static_assert(cvec.size() == 5);
}

TEST(StaticVector, IndexOperator)
{
    constexpr StaticVector<int> cvec{1, 2, 3, 4, 5};

    static_assert(cvec[1] == 2);
}

TEST(StaticVector, IndexOperatorOutOfRange)
{
    constexpr StaticVector<int> cvec{1, 2, 3, 4, 5};

    static_assert(cvec[6] == 0);
    // static_assert(cvec[16] == 0);
}

TEST(StaticVector, At)
{
    constexpr StaticVector<int> cvec{1, 2, 3, 4, 5};

    static_assert(cvec[1] == 2);
}

TEST(StaticVector, AtOutOfRange)
{
    constexpr StaticVector<int> cvec{1, 2, 3, 4, 5};

    EXPECT_THROW(cvec.at(6), std::range_error);
}

TEST(StaticVector, Back)
{
    constexpr StaticVector<int> cvec{1, 2, 3, 4, 5};

    static_assert(cvec.back() == 5);
}

TEST(StaticVector, PushBack)
{
    StaticVector<int> cvec{1, 2, 3, 4, 5};

    cvec.push_back(6);

    EXPECT_EQ(cvec.back(), 6);
}

TEST(StaticVector, PushBackOutOfRange)
{
    StaticVector<int, 5> cvec{1, 2, 3, 4, 5};

    EXPECT_THROW(cvec.push_back(6), std::range_error);
}

TEST(StaticVector, OperatorEqual)
{
    constexpr StaticVector<int> cvec1{1, 2, 3, 4, 5};
    constexpr StaticVector<int> cvec2{1, 2, 3, 4, 5};

    static_assert(cvec1 == cvec2);
    EXPECT_EQ(cvec1, cvec2);
}

TEST(StaticVector, OperatorNotEqual)
{
    constexpr StaticVector<int, 6> cvec1{1, 2, 3, 4, 5};
    constexpr StaticVector<int>    cvec2{1, 2, 3, 4, 5};

    static_assert(cvec1 != cvec2);
    EXPECT_NE(cvec1, cvec2);
}
