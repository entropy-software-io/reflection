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
    // static const char* GetStr(const TString& str);
    // static int GetLength(const TString& str);
    // static int FindFirst(const TString& str, char value);
    // static int FindLast(const TString& str, char value);
    // static void SubString(TString& str, int start, int count);
    // static void Replace(TString& str, const TString& find, const TString& replacement);
    // static void Append(TString& str, const TString& other);
};

template <>
struct StringOps<std::string>
{
    static const char* GetStr(const std::string& str) { return str.c_str(); }
    static int GetLength(const std::string& str) { return (int)str.length(); }
    static int FindFirst(const std::string& str, char value)
    {
        auto ret = str.find(value);
        if (ret == std::string::npos)
        {
            return -1;
        }
        return (int)ret;
    }
    static int FindLast(const std::string& str, char value)
    {
        auto ret = str.rfind(value);
        if (ret == std::string::npos)
        {
            return -1;
        }
        return (int)ret;
    }
    static void SubString(std::string& str, int start, int count) { str = str.substr(start, count); }
    static void Replace(std::string& str, const std::string& find, const std::string& replacement)
    {
        if (GetLength(find) != 0)
        {
            auto pos = str.find(find, 0);
            while (pos != std::string::npos)
            {
                str.replace(pos, find.length(), replacement);

                pos = str.find(find, pos + replacement.length());
            }
        }
    }
    static void Append(std::string& str, const std::string& other) { str += other; }
};

} // namespace details
} // namespace Entropy
