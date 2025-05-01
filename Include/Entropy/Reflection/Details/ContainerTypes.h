// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace Entropy
{
namespace details
{

template <typename T, typename = void>
struct ReflectionContainerTraits
{
    template <typename U>
    using Allocator = std::allocator<U>;

    template <typename TKey, typename TValue>
    using MapType = std::unordered_map<TKey, TValue, std::hash<TKey>, std::equal_to<TKey>,
                                       Allocator<std::pair<const TKey, TValue>>>;

    template <typename TValue>
    using VectorType = std::vector<TValue, Allocator<TValue>>;

    using StringType = std::string;

    template <typename TFn>
    using FunctionType = std::function<TFn>;
};

//--------------

template <typename TString>
struct StringOps
{
    // TString Replace(const TString& str, const char* find, const char* replacement);
};

template <>
struct StringOps<std::string>
{
    static const char* GetStr(const std::string& str) { return str.c_str(); }
    static int GetLength(const std::string& str) { return (int)str.length(); }
    static std::string Replace(const std::string& str, const char* find, const char* replacement)
    {
        // TODO: Implement
        return str;
    }
};

} // namespace details
} // namespace Entropy
