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

} // namespace Entropy

#define ENTROPY_DECLARE_LINE_MARKER(line)                                                                              \
    template <int TLine, typename TDummy = void>                                                                       \
    struct __LineMarker : public std::integral_constant<int, -1>                                                       \
    {                                                                                                                  \
        static constexpr bool Exists = false;                                                                          \
    };                                                                                                                 \
    template <typename TDummy>                                                                                         \
    struct __LineMarker<line - 5, TDummy> : public std::integral_constant<int, -1>                                     \
    {                                                                                                                  \
        static constexpr bool Exists = true;                                                                           \
    };                                                                                                                 \
    template <typename TDummy>                                                                                         \
    struct __LineMarker<line - 4, TDummy> : public std::integral_constant<int, -1>                                     \
    {                                                                                                                  \
        static constexpr bool Exists = true;                                                                           \
    };                                                                                                                 \
    template <typename TDummy>                                                                                         \
    struct __LineMarker<line - 3, TDummy> : public std::integral_constant<int, -1>                                     \
    {                                                                                                                  \
        static constexpr bool Exists = true;                                                                           \
    };                                                                                                                 \
    template <typename TDummy>                                                                                         \
    struct __LineMarker<line - 2, TDummy> : public std::integral_constant<int, -1>                                     \
    {                                                                                                                  \
        static constexpr bool Exists = true;                                                                           \
    };                                                                                                                 \
    template <typename TDummy>                                                                                         \
    struct __LineMarker<line - 1, TDummy> : public std::integral_constant<int, -1>                                     \
    {                                                                                                                  \
        static constexpr bool Exists = true;                                                                           \
    };                                                                                                                 \
    template <typename TDummy>                                                                                         \
    struct __LineMarker<line, TDummy> : public std::integral_constant<int, 0>                                          \
    {                                                                                                                  \
        static constexpr bool Exists = true;                                                                           \
    };

#define ENTROPY_DEFINE_LINE_MARKER(line)                                                                               \
    template <typename TDummy>                                                                                         \
    struct __LineMarker<line, TDummy>                                                                                  \
    {                                                                                                                  \
        static constexpr int value   = __GetCounterValue<line - 1>::value + 1;                                         \
        static constexpr bool Exists = true;                                                                           \
    };

#define ENTROPY_DECLARE_GET_COUNTER_VALUE(line)                                                                        \
    template <int TLine, typename TDummy = void>                                                                       \
    struct __GetCounterValue                                                                                           \
        : public ::Entropy::TernaryValue<__LineMarker<TLine>::Exists, int, __LineMarker<TLine>::value,                 \
                                         __GetCounterValue<TLine - 1>::value>                                          \
    {                                                                                                                  \
    };                                                                                                                 \
    template <typename TDummy>                                                                                         \
    struct __GetCounterValue<line, TDummy> : public std::integral_constant<int, 0>                                     \
    {                                                                                                                  \
    };                                                                                                                 \
    template <typename TDummy>                                                                                         \
    struct __GetCounterValue<line - 1, TDummy> : public std::integral_constant<int, -1>                                \
    {                                                                                                                  \
    };                                                                                                                 \
    template <typename TDummy>                                                                                         \
    struct __GetCounterValue<line - 2, TDummy> : public std::integral_constant<int, -1>                                \
    {                                                                                                                  \
    };                                                                                                                 \
    template <typename TDummy>                                                                                         \
    struct __GetCounterValue<line - 3, TDummy> : public std::integral_constant<int, -1>                                \
    {                                                                                                                  \
    };                                                                                                                 \
    template <typename TDummy>                                                                                         \
    struct __GetCounterValue<line - 4, TDummy> : public std::integral_constant<int, -1>                                \
    {                                                                                                                  \
    };

#define ENTROPY_GET_COUNTER_VALUE(line) __GetCounterValue<line>::value

#define ENTROPY_DECLARE_COUNTER(line)                                                                                  \
    ENTROPY_DECLARE_LINE_MARKER(line)                                                                                  \
    ENTROPY_DECLARE_GET_COUNTER_VALUE(line)
