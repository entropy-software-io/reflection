// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

#include <type_traits>

namespace Entropy
{

template <bool TPredicate, typename T, T TTrueValue, T TFalseValue>
struct TernaryValue : public std::integral_constant<T, TTrueValue>
{
};

template <typename T, T TTrueValue, T TFalseValue>
struct TernaryValue<false, T, TTrueValue, TFalseValue> : public std::integral_constant<T, TFalseValue>
{
};

template <bool TPredicate, typename T, T TTrueValue, T TFalseValue>
constexpr T TernaryValue_v = TernaryValue<TPredicate, T, TTrueValue, TFalseValue>::value;

} // namespace Entropy

#define ENTROPY_DECLARE_LINE_MARKER                                                                                    \
    template <int TLine, typename TDummy = void>                                                                       \
    struct LineMarker : public std::integral_constant<int, -1>                                                         \
    {                                                                                                                  \
        static constexpr bool Exists = false;                                                                          \
    };                                                                                                                 \
    template <typename TDummy>                                                                                         \
    struct LineMarker<__LINE__ - 5, TDummy> : public std::integral_constant<int, -1>                                   \
    {                                                                                                                  \
        static constexpr bool Exists = true;                                                                           \
    };                                                                                                                 \
    template <typename TDummy>                                                                                         \
    struct LineMarker<__LINE__ - 4, TDummy> : public std::integral_constant<int, -1>                                   \
    {                                                                                                                  \
        static constexpr bool Exists = true;                                                                           \
    };                                                                                                                 \
    template <typename TDummy>                                                                                         \
    struct LineMarker<__LINE__ - 3, TDummy> : public std::integral_constant<int, -1>                                   \
    {                                                                                                                  \
        static constexpr bool Exists = true;                                                                           \
    };                                                                                                                 \
    template <typename TDummy>                                                                                         \
    struct LineMarker<__LINE__ - 2, TDummy> : public std::integral_constant<int, -1>                                   \
    {                                                                                                                  \
        static constexpr bool Exists = true;                                                                           \
    };                                                                                                                 \
    template <typename TDummy>                                                                                         \
    struct LineMarker<__LINE__ - 1, TDummy> : public std::integral_constant<int, -1>                                   \
    {                                                                                                                  \
        static constexpr bool Exists = true;                                                                           \
    };                                                                                                                 \
    template <typename TDummy>                                                                                         \
    struct LineMarker<__LINE__, TDummy> : public std::integral_constant<int, 0>                                        \
    {                                                                                                                  \
        static constexpr bool Exists = true;                                                                           \
    };                                                                                                                 \
    template <int TLine, typename TDummy = void>                                                                       \
    static constexpr int LineMarker_v = LineMarker<TLine, TDummy>::value;

#define ENTROPY_DEFINE_LINE_MARKER                                                                                     \
    template <typename TDummy>                                                                                         \
    struct LineMarker<__LINE__, TDummy>                                                                                \
    {                                                                                                                  \
        static constexpr int value   = GetCounterValue<__LINE__ - 1>::value + 1;                                       \
        static constexpr bool Exists = true;                                                                           \
    };

#define ENTROPY_DECLARE_GET_COUNTER_VALUE                                                                              \
    template <int TLine, typename TDummy = void>                                                                       \
    struct GetCounterValue                                                                                             \
    {                                                                                                                  \
        static constexpr int value = ::Entropy::TernaryValue_v<LineMarker<TLine>::Exists, int, LineMarker_v<TLine>,    \
                                                               GetCounterValue<TLine - 1>::value>;                     \
    };                                                                                                                 \
    template <typename TDummy>                                                                                         \
    struct GetCounterValue<__LINE__, TDummy> : public std::integral_constant<int, 0>                                   \
    {                                                                                                                  \
    };                                                                                                                 \
    template <typename TDummy>                                                                                         \
    struct GetCounterValue<__LINE__ - 1, TDummy> : public std::integral_constant<int, -1>                              \
    {                                                                                                                  \
    };                                                                                                                 \
    template <typename TDummy>                                                                                         \
    struct GetCounterValue<__LINE__ - 2, TDummy> : public std::integral_constant<int, -1>                              \
    {                                                                                                                  \
    };                                                                                                                 \
    template <typename TDummy>                                                                                         \
    struct GetCounterValue<__LINE__ - 3, TDummy> : public std::integral_constant<int, -1>                              \
    {                                                                                                                  \
    };                                                                                                                 \
    template <typename TDummy>                                                                                         \
    struct GetCounterValue<__LINE__ - 4, TDummy> : public std::integral_constant<int, -1>                              \
    {                                                                                                                  \
    };

#define ENTROPY_GET_COUNTER_VALUE() GetCounterValue<__LINE__>::value

#define ENTROPY_DECLARE_COUNTER                                                                                        \
    ENTROPY_DECLARE_LINE_MARKER                                                                                        \
    ENTROPY_DECLARE_GET_COUNTER_VALUE
