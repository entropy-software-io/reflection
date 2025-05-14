// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

#include "Entropy/Core/Details/AllocatorTraits.h"
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
    template <typename TKey, typename TValue>
    using MapType = std::unordered_map<TKey, TValue, std::hash<TKey>, std::equal_to<TKey>,
                                       typename Traits::AllocatorTraits<std::pair<const TKey, TValue>>::Allocator>;

    template <typename TValue>
    using VectorType = std::vector<TValue, typename Traits::AllocatorTraits<TValue>::Allocator>;

    using StringType = std::string;

    template <typename TFn>
    using FunctionType = std::function<TFn>;
};

} // namespace details
} // namespace Entropy
