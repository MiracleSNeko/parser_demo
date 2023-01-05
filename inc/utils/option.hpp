#pragma once

#include "./type_traits.hpp"

#include <concepts>
#include <optional>
#include <stdexcept>
#include <string_view>
#include <type_traits>
#include <utility>
#include <variant>

namespace d1::utils::option
{
using namespace std::literals;

constexpr auto MODULE_NAME{"utils/option.hpp"sv};

template <typename T>
class Option;

namespace __impl
{
    struct None
    {
        /* static cast */
        template <typename T>
        constexpr operator Option<T>()
        {
            return None{};
        }

        /* for gtest */
        constexpr __always_inline bool operator==(const None& _) const noexcept
        {
            return true;
        }
    };

    template <typename T>
    class Some
    {
    public:
        /* for type traits */
        using value_type = T;

        constexpr Some(const T& t) noexcept : _value(t) {}

        constexpr operator const T&() const noexcept
        {
            return _value;
        }

        /* for gtest */
        template <typename U>
        constexpr __always_inline bool operator==(const Some<U>& rhs) const noexcept
        {
            return std::is_same_v<T, U> && this->_value == rhs._value;
        }

    private:
        T _value{};
    };

    template <typename... Fns>
    class Overload : Fns...
    {
    public:
        constexpr Overload(const Fns&... fns) : Fns{fns}... {}

        using Fns::operator()...;
    };
}  // namespace __impl

// `Option` is a Monad
template <typename T>
class Option
{
public:
    /* for type traits */
    using value_type = T;

    constexpr Option(const T& t) : _option(__impl::Some(t)) {}
    constexpr Option(const __impl::Some<T>& t) : _option(t) {}
    constexpr Option(__impl::None) : _option(__impl::None{}) {}

    constexpr auto to_variant() const noexcept
    {
        return _option;
    }

    constexpr bool is_some() const noexcept
    {
        return _option.index() == 0;
    }

    constexpr bool is_none() const noexcept
    {
        return _option.index() == 1;
    }

    constexpr T unwrap() const
    {
        return std::visit(__impl::Overload{[](const __impl::Some<T>& t) -> T { return static_cast<T>(t); },
                                           [](const __impl::None& _) -> T {
                                               throw std::runtime_error("unwrapping a `None` value, panic!");
                                           }},
                          _option);
    }

    /* for gtest */
    template <typename U>
    constexpr __always_inline bool operator==(const Option<U>& rhs) const noexcept
    {
        return std::is_same_v<T, U> &&
               ((this->is_none() && rhs.is_none()) ||
                (this->is_some() && rhs.is_some() &&
                 std::get<__impl::Some<T>>(this->_option) == std::get<__impl::Some<U>>(rhs._option)));
    }

private:
    std::variant<__impl::Some<T>, __impl::None> _option;
};

// unit:: t -> Option<T>
template <typename T>
using Some = __impl::Some<T>;

template <typename T>
constexpr auto SOME(T t) -> Option<T>
{
    return t;
};

using None = __impl::None;
constexpr __impl::None NONE{};

namespace __impl
{
    using namespace d1::utils::function_traits;
    using namespace d1::utils::template_traits;

    template <typename T, typename Fn>
    concept bindable_to =
        requires {
            {
                std::same_as<T, std::remove_cvref_t<typename __impl::function_traits<Fn>::template args<0>::type>>
            };
            {
                __impl::is_template_of_v<Option, typename __impl::function_traits<Fn>::return_type>
            };
        };

}  // namespace __impl

// bind:: Option<T> -> (T -> Option<U>) -> Option<U>
template <typename T, typename Fn>
// TODO: bugs here.
//       need proxy class instead of specialization explicitly.
//   requires __impl::bindable_to<T, Fn>
constexpr auto operator>>=(const Option<T>& opt, Fn&& fn)
{
    using U = std::invoke_result_t<Fn, T>;
    return std::visit(__impl::Overload{[=](const __impl::Some<T>& t) -> U { return fn(static_cast<T>(t)); },
                                       [](const __impl::None& _) -> U { return NONE; }},
                      opt.to_variant());
}

template <typename Fn1, typename Fn2>
    requires __impl::callable_object<Fn1> && __impl::callable_object<Fn2>
constexpr auto operator<<=(Fn1&& fn1, Fn2&& fn2)
{
    return [=](const auto& input) { return fn1(input) >>= fn2; };
}

namespace operators
{
    using d1::utils::option::operator>>=;
    using d1::utils::option::operator<<=;
}  // namespace operators

}  // namespace d1::utils::option