#include <gtest/gtest.h>
#include <stdexcept>
#include <sys/cdefs.h>
#include <variant>

#include "../../inc/utils/option.hpp"

using d1::utils::option::NONE;
using d1::utils::option::None;
using d1::utils::option::Option;
using d1::utils::option::SOME;
using d1::utils::option::Some;

TEST(OptionConstruct, WithValue)
{
    Option int_1 = SOME(1);

    EXPECT_TRUE(int_1.is_some());
    EXPECT_EQ(int_1, Option(1));
    EXPECT_EQ(int_1.unwrap(), 1);
}

TEST(OptionConstruct, WithoutValue)
{
    Option<int> none = NONE;

    EXPECT_TRUE(none.is_none());
    EXPECT_THROW(none.unwrap(), std::runtime_error);
}

TEST(OptionConstruct, UserDefinedStruct)
{
    struct UserDefindStruct
    {
        int         v1;
        char        v2;
        std::string v3;

        UserDefindStruct(int v1, char v2, std::string v3) : v1(v1), v2(v2), v3(v3){};
    };

    Option uds_1 = SOME(UserDefindStruct(1, '2', "345"));

    EXPECT_TRUE(uds_1.is_some());
    EXPECT_EQ(uds_1.unwrap().v1, 1);
    EXPECT_EQ(uds_1.unwrap().v2, '2');
    EXPECT_EQ(uds_1.unwrap().v3, std::string("345"));
}

TEST(OptionConstruct, CompileTimeConstructionWithValue)
{
    constexpr Option cint_1 = SOME(1);

    static_assert(cint_1.is_some());
    static_assert(cint_1 == Option<int>(1));
}

TEST(OptionConstruct, CompileTimeConstructionWithoutValue)
{
    constexpr Option<int> cnone = NONE;

    static_assert(cnone.is_none());
    static_assert(cnone == Option<int>(NONE));
}

TEST(OptionConstruct, CompileTimeUserDefinedStruct)
{
    struct UserDefinedStruct
    {
        int              v1;
        char             v2;
        std::string_view v3;

        constexpr UserDefinedStruct(int v1, char v2, std::string_view v3) : v1(v1), v2(v2), v3(v3){};

        constexpr __always_inline bool operator==(const UserDefinedStruct& rhs) const noexcept
        {
            return this->v1 == rhs.v1 && this->v2 == rhs.v2 && this->v3 == rhs.v3;
        }
    };
    constexpr Option cuds_1 = SOME(UserDefinedStruct{1, '2', {"345"}});

    static_assert(cuds_1.is_some());
    static_assert(cuds_1 == Option(UserDefinedStruct{1, '2', {"345"}}));
}

TEST(OptionContinuation, SomeTContinueationToSomeU)
{
    Option int_2 = SOME(1) >>= [](int i) { return SOME(i + 1); };

    EXPECT_TRUE(int_2.is_some());
    EXPECT_EQ(int_2.unwrap(), 2);
}

TEST(OptionContinuation, SomeTContinueationToNone)
{
    Option<int> none = SOME(1) >>= [](int _) { return NONE; };

    EXPECT_TRUE(none.is_none());
    EXPECT_THROW(none.unwrap(), std::runtime_error);
}

TEST(OptionContinuation, NoneContinueationToSomeU)
{
    Option none = Option<int>(NONE) >>= [](int i) { return SOME(i + 1); };

    EXPECT_TRUE(none.is_none());
    EXPECT_THROW(none.unwrap(), std::runtime_error);
}

TEST(OptionContinuation, CompileTimeContinuation)
{
    constexpr Option cint_2 = SOME(1) >>= [](int i) { return SOME(i + 1); };

    static_assert(cint_2.is_some());
    static_assert(cint_2 == SOME(2));
}