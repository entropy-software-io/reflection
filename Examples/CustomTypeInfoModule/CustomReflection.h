// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

// We can override the storage containers used by type info as long as it conforms to the stl API
#include "Entropy/Reflection/Details/ContainerTypes.h"

#include <list>
#include <map>

namespace Entropy
{
namespace details
{

template <typename T>
struct ReflectionContainerTraits<T>
{
    template <typename U>
    using Allocator = std::allocator<U>;

    template <typename TKey, typename TValue>
    using MapType = std::map<TKey, TValue, std::less<TKey>, Allocator<std::pair<const TKey, TValue>>>;

    template <typename TValue>
    using VectorType = std::list<TValue, Allocator<TValue>>;

    using StringType = std::string;

    template <typename TFn>
    using FunctionType = std::function<TFn>;
};

} // namespace details
} // namespace Entropy

// We need to override the traits used by TypeInfo

#include "CustomTypeInfoModule.h"
#include "Entropy/Reflection/TypeInfoTraits.h"

namespace Entropy
{
namespace Reflection
{

template <>
struct TypeInfoTraits<void>
{
    // ModuleTypes must be an std::tuple
    using ModuleTypes = std::tuple<ClassTypeInfo, CustomTypeInfo>;
};

} // namespace Reflection
} // namespace Entropy

// Now we can include the reflection code
#include "Entropy/Reflection.h"
