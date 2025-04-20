// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

// The traits in this class can be overridden if this file is included before including TypeInfo.h (or Reflection.h)
// If you do override them, it is very important that the same header include order is used everywhere!

#pragma once

#include "Entropy/Reflection/Details/UserOverrideDummy.h"
#include "TypeInfoModules/BasicTypeInfo.h"
#include "TypeInfoModules/ClassTypeInfo.h"
#include <tuple>

namespace Entropy
{
namespace Reflection
{

template <typename = void>
struct TypeInfoTraits
{
    // ModuleTypes must be an std::tuple
    using ModuleTypes = std::tuple<BasicTypeInfo, ClassTypeInfo>;
};

} // namespace Reflection
} // namespace Entropy
