#pragma once

#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>

#include "../utils/option.hpp"
#include "./parser.hpp"

using namespace std::literals;

using d1::core::parser::ParserInput;
using d1::core::parser::ParserOutput;
using d1::utils::option::None;
using d1::utils::option::NONE;
using d1::utils::option::Some;
using d1::utils::option::SOME;

namespace d1::core::combinator
{

constexpr auto MODULE_NAME{"core/combinator.hpp"sv};

namespace __impl
{
    using d1::core::parser::__impl::ParsedMirType;
    using d1::core::parser::__impl::ParsedResultType;
    using d1::utils::option::__impl::Overload;

    template <typename Predicator, typename... Args>
    concept is_predicatable = std::same_as<std::invoke_result_t<Predicator, Args...>, bool>;

}  // namespace __impl

// The basic combinator of `Parser`: cascade
// Run the given two parsers in sequence, then combine the output by given fn.
//
// Fn :: a -> b -> c
// Combine :: Parser a -> Parser b -> (a -> b -> c) -> Parser c
template <typename Parser1, typename Parser2, typename Fn>
constexpr auto Combine(Parser1&& parser1, Parser2&& parser2, Fn&& fn)
{
    using Mir1 = __impl::ParsedMirType<Parser1>;
    using T1   = __impl::ParsedResultType<Parser1>;
    using Mir2 = __impl::ParsedMirType<Parser2>;
    using T2   = __impl::ParsedResultType<Parser2>;
    using R    = std::invoke_result_t<Fn, T1, T2>;

    return [=](ParserInput code) -> ParserOutput<R> {
        // return parser1(code) >>= [=](const Mir1& mir1) {
        //     parser2(mir1.second) >>=
        //         [r1 = mir1.first](const Mir2& mir2) { return SOME(std::make_tuple(r1, mir2.first, mir2.second)); };
        // } >>= [=](const auto& tuple1_2) -> ParserOutput<R> {
        //     const auto [r1, r2, code] = tuple1_2;
        //     return SOME(std::make_pair(fn(r1, r2), code));
        // };
        const auto mir1 = parser1(code);

        if (mir1.is_none())
        {
            return NONE;
        }
        else
        {
            const auto result1 = mir1.unwrap().first;
            const auto code    = mir1.unwrap().second;

            const auto mir2 = parser2(code);

            if (mir2.is_none())
            {
                return NONE;
            }
            else
            {
                const auto result2 = mir2.unwrap().first;
                const auto code    = mir2.unwrap().second;

                return SOME(std::make_pair(fn(result1, result2), code));
            }
        }
    };
}

// The basic combinator of `Parser`: shunt
// Try the first parser. If failed, try another.
// The given parser MUST both return the same type.
template <typename Parser1, typename Parser2>
    requires std::same_as<__impl::ParsedMirType<Parser1>, __impl::ParsedMirType<Parser2>> &&
             std::same_as<__impl::ParsedResultType<Parser1>, __impl::ParsedResultType<Parser2>>
constexpr auto operator||(Parser1&& parser1, Parser2&& parser2)
{
    using Mir = __impl::ParsedMirType<Parser1>;
    using T   = __impl::ParsedResultType<Parser1>;

    return [=](ParserInput code) -> ParserOutput<T> {
        return std::visit(__impl::Overload{
                              [](const Some<Mir>& mir) -> ParserOutput<T> { return mir; },
                              [&](const None& _) -> ParserOutput<T> { return parser2(code); },
                          },
                          parser1(code).to_variant());
    };
}

// Select the first parser's result in a combined `Parser`
//
// operator << :: Parser a -> Parser b -> Parser a
template <typename Parser1, typename Parser2>
constexpr auto operator<<(Parser1&& parser1, Parser2&& parser2)
{
    return Combine(parser1, parser2, [](const auto& lhs, const auto& rhs) { return lhs; });
}

// Select the right parser's result in a combined `Parser`
//
// operator >> :: Parser a -> Parser b -> Parser b
template <typename Parser1, typename Parser2>
constexpr auto operator>>(Parser1&& parser1, Parser2&& parser2)
{
    return Combine(parser1, parser2, [](const auto& lhs, const auto& rhs) { return rhs; });
}

namespace __impl
{
    // Fold :: Parser a -> b -> (b -> a -> b) -> ParserInput -> ParserOutput b
    template <typename Parser, typename Acc, typename Fn>
    constexpr auto Fold(Parser&& parser, ParserInput code, Acc acc, Fn&& fn)
    {
        while (true)
        {
            const auto mir = parser(code);

            if (mir.is_none())
            {
                return SOME(std::make_pair(acc, code));
            }
            else
            {
                acc  = fn(acc, mir.unwrap().first);
                code = mir.unwrap().second;
            }
        }
    }

    // Fold with exact times.
    template <typename Parser, typename Acc, typename Fn>
    constexpr auto FoldExactly(Parser&& parser, ParserInput code, Acc acc, std::size_t times, Fn&& fn)
    {
        while (times != 0)
        {
            const auto mir = parser(code);

            if (mir.is_none())
            {
                return SOME(std::make_pair(acc, code));
            }
            else
            {
                acc  = fn(acc, mir.unwrap().first);
                code = mir.unwrap().second;
                times -= 1;
            }
        }

        // if fold 0 times...
        return SOME(std::make_pair(acc, code));
    }

    // Fold while the reduced result satisfy the condition.
    template <typename Parser, typename Acc, typename Fn, typename Predicator>
        requires is_predicatable<Predicator, Acc, ParsedResultType<Parser>>
    constexpr auto FoldWhile(Parser&& parser, ParserInput code, Acc acc, Fn&& fn, Predicator&& predicator)
    {
        while (true)
        {
            const auto mir = parser(code);

            if (mir.is_some() && predicator(acc, mir.unwrap().first))
            {
                acc  = fn(acc, mir.unwrap().first);
                code = mir.unwrap().second;
            }
            else
            {
                return SOME(std::make_pair(acc, code));
            }
        }
    }

}  // namespace __impl

// Apply ? (0 or 1) of a parser. If 0 patterns matched, return the given default value.
//
// Try :: Parser a -> a -> Parser a
template <typename Parser, typename T = __impl::ParsedResultType<Parser>>
constexpr auto Try(Parser&& parser, T&& default_value)
{
    using Mir = __impl::ParsedMirType<Parser>;

    return [=](ParserInput code) -> ParserOutput<T> {
        return std::visit(
            __impl::Overload{
                [](const Some<Mir>& mir) -> ParserOutput<T> { return Option(mir); },
                [&](const None& _) -> ParserOutput<T> { return SOME(std::make_pair(default_value, code)); },
            },
            parser(code).to_variant());
    };
}

// Apply * of a parser, then accumulate the results.
//
// Fn :: b -> a -> b
// Any :: Parser a -> b -> (b -> a -> b) -> Parser b
template <typename Parser, typename Acc = __impl::ParsedResultType<Parser>, typename Fn>
constexpr auto Any(Parser&& parser, Acc&& acc, Fn&& fn)
{
    return [=](ParserInput code) -> ParserOutput<Acc> { return __impl::Fold(parser, code, acc, fn); };
};

// Apply + of a parser, then accumulate the results.
//
// Fn :: b -> a -> b
// Many :: Parser a -> b -> (b -> a -> b) -> Parser b
template <typename Parser, typename Acc = __impl::ParsedResultType<Parser>, typename Fn>
constexpr auto Many(Parser&& parser, Acc&& acc, Fn&& fn)
{
    using Mir = __impl::ParsedMirType<Parser>;

    return [=](ParserInput code) -> ParserOutput<Acc> {
        return parser(code) >>=
               [=](const Mir& mir) { return __impl::Fold(parser, mir.second, fn(acc, mir.first), fn); };
    };
}

// Apply * of a parser while condition satisfied, then accumulate the results.
//
// Fn :: b -> a -> b
// Predicator :: b -> a -> bool
// While :: Parser a -> b -> (b -> a -> b) -> (b -> a -> bool) -> Parser b
template <typename Parser, typename Acc = __impl::ParsedResultType<Parser>, typename Fn, typename Predicator>
    requires __impl::is_predicatable<Predicator, Acc, __impl::ParsedResultType<Parser>>
constexpr auto While(Parser&& parser, Acc&& acc, Fn&& fn, Predicator&& predicator)
{
    return [=](ParserInput code) -> ParserOutput<Acc> { return __impl::FoldWhile(parser, code, acc, fn, predicator); };
}

// Apply + of a parser while condition satisfied, then accumulate the results.
//
// Fn :: b -> a -> b
// Predicator :: b -> a -> bool
//
// DoWhile :: Parser a -> b -> (b -> a -> b) -> (b -> a -> bool) -> Parser b
template <typename Parser, typename Acc = __impl::ParsedResultType<Parser>, typename Fn, typename Predicator>
    requires __impl::is_predicatable<Predicator, Acc, __impl::ParsedResultType<Parser>>
constexpr auto DoWhile(Parser&& parser, Acc&& acc, Fn&& fn, Predicator&& predicator)
{
    using Mir = __impl::ParsedMirType<Parser>;

    return [=](ParserInput code) -> ParserOutput<Acc> {
        return parser(code) >>= [=](const Mir& mir) {
            return __impl::FoldWhile(parser, mir.second, fn(acc, mir.first), fn, predicator);
        };
    };
}

// Apply a parser exactly n times, then accumulate the results.
//
// Fn :: b -> a -> b
// Exactly :: Parser a -> b -> std::size_t -> (b -> a -> b) -> Parser b
template <typename Parser, typename Acc = __impl::ParsedResultType<Parser>, typename Fn>
constexpr auto Exactly(Parser&& parser, Acc&& acc, std::size_t times, Fn&& fn)
{
    return [=](ParserInput code) -> ParserOutput<Acc> { return __impl::FoldExactly(parser, code, acc, times, fn); };
}

}  // namespace d1::core::combinator