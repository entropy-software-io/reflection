// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

#include <unordered_map>
#include <string>

namespace Entropy
{
namespace details
{

template <typename T>
struct ReflectionContainerTraits
{
    template <typename U>
    using Allocator = std::allocator<U>;

    template <typename TKey, typename TValue>
    using MapType = std::unordered_map<TKey, TValue, std::hash<TKey>, std::equal_to<TKey>,
                                       Allocator<std::pair<const TKey, TValue>>>;

    using StringType = std::string;
};

} // namespace details
} // namespace Entropy
