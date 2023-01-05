#pragma once

#include <functional>
#include <string_view>
#include <type_traits>
#include <utility>

#include "../utils/option.hpp"

using d1::utils::option::NONE;
using d1::utils::option::Option;
using d1::utils::option::SOME;

namespace d1::core::parser
{
using namespace std::literals;

constexpr auto MODULE_NAME{"core/parser.hpp"sv};

using ParserInput = std::string_view;

template <typename T>
using ParserOutput = Option<std::pair<T, ParserInput>>;

namespace __impl
{
    // The Mid-level Intermediate Representation of Parser-Continuation's result.
    template <typename Parser>
    using ParsedMirType = typename std::invoke_result_t<Parser, ParserInput>::value_type;

    template <typename Parser>
    using ParsedResultType = typename ParsedMirType<Parser>::first_type;
}  // namespace __impl

// `Parser a :: String -> [(a, String )]` is a Monad.
template <typename T>
// using Parser = auto(*)(ParserInput) -> ParserOutput<T>;
using Parser = std::function<ParserOutput<T>(ParserInput)>;

// map a function into a `Parser a`
// Fn b :: a -> b
// Map :: Fn b -> Parser a -> (ParserInput -> ParserOutput b)
template <typename Parser, typename Fn>
constexpr auto Map(Parser&& parser, Fn&& fn)
{
    using Mir = __impl::ParsedMirType<Parser>;
    using T   = __impl::ParsedResultType<Parser>;
    using R   = ParserOutput<std::invoke_result_t<Fn, T>>;

    return [=](ParserInput code) -> R {
        return parser(code) >>= [=](const Mir& mir) -> R { return SOME(std::make_pair(fn(mir.first), mir.second)); };
    };
}

// bind a function into a `Parser a`
// Fn b :: a -> ParserInput -> ParserOutput b
// Bind :: Fn b -> Parser a -> (a -> ParserInput -> ParserOutput b)
template <typename Parser, typename Fn>
constexpr auto Bind(Parser&& parser, Fn&& fn)
{
    using Mir = __impl::ParsedMirType<Parser>;
    using T   = __impl::ParsedResultType<Parser>;
    using R   = ParserOutput<std::invoke_result_t<Fn, T, ParserInput>>;

    return [=](ParserInput code) -> R {
        return parser(code) >>= [=](const Mir& mir) -> R { return fn(mir.first, mir.second); };
    };
}

// A `Parser` which always fails.
template <typename T>
constexpr auto Except(T _)
{
    return [](ParserInput code) -> ParserOutput<T> { return NONE; };
}

// And it's "hooked" version...
template <typename T, typename Hook>
constexpr auto ExceptWith(T _, Hook&& hook)
{
    return [hook = std::forward(hook)](ParserInput code) -> ParserOutput<T> {
        hook();
        return NONE;
    };
}

}  // namespace d1::core::parser