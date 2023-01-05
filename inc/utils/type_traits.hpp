#pragma once

#include <functional>
#include <string_view>
#include <type_traits>

namespace d1::utils::template_traits
{
using namespace std::literals;

constexpr auto MODULE_NAME{"utils/type_traits.hpp"sv};

template <typename T, typename = std::void_t<>>
struct is_callable_object : std::false_type
{
};

template <typename T>
struct is_callable_object<T, std::void_t<decltype(std::declval<T>().operator())>> : std::true_type
{
};

template <typename T>
constexpr bool is_callable_object_v = is_callable_object<T>::value;

template <typename T>
concept callable_object = requires { is_callable_object_v<T>; };

template <typename T>
concept uncallable_object = requires { !is_callable_object_v<T>; };

template <template <typename...> typename T, typename U>
struct is_template_of : std::false_type
{
};

template <template <typename...> typename T, typename... Args>
struct is_template_of<T, T<Args...>> : std::true_type
{
};

template <template <typename...> typename T, typename... Args>
constexpr bool is_template_of_v = is_template_of<T, Args...>::value;

}  // namespace d1::utils::template_traits

namespace d1::utils::function_traits
{

namespace __impl
{
    using namespace d1::utils::template_traits;
}

template <typename>
struct function_traits;

template <typename Return, typename... Args>
struct function_traits<Return(Args...)>
{
    constexpr static std::size_t arity = sizeof...(Args);

    using return_type   = Return;
    using function_type = std::function<Return(Args...)>;

    template <std::size_t Idx>
    struct args
    {
        static_assert(Idx < arity, "function args' index out of range!");

        using type = typename std::tuple_element_t<Idx, std::tuple<Args...>>;
    };
};

template <typename Return, typename... Args>
struct function_traits<Return (*)(Args...)> : function_traits<Return(Args...)>
{
};

template <typename Return, typename Class, typename... Args>
struct function_traits<Return (Class::*)(Args...)> : function_traits<Return(Args...)>
{
};

template <typename Return, typename Class, typename... Args>
struct function_traits<Return (Class::*)(Args...) const> : function_traits<Return(Args...)>
{
};

template <typename Return, typename Class, typename... Args>
struct function_traits<Return (Class::*)(Args...) volatile> : function_traits<Return(Args...)>
{
};

template <typename Return, typename Class, typename... Args>
struct function_traits<Return (Class::*)(Args...) const volatile> : function_traits<Return(Args...)>
{
};

template <typename Callable>
struct function_traits : function_traits<decltype(&Callable::operator())>
{
};

}  // namespace d1::utils::function_traits