// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

// We need to override the traits used by TypeInfo

#include "CustomTypeInfoModule.h"
#include "Entropy/Reflection/TypeInfoTraits.h"

namespace Entropy
{
namespace Reflection
{

template <>
struct TypeInfoTraits<UserOverride>
{
    // ModuleTypes must be an std::tuple
    using ModuleTypes = std::tuple<BasicTypeInfo, ClassTypeInfo, CustomTypeInfo>;
};

} // namespace Reflection
} // namespace Entropy

// Now we can include the reflection code
#include "Entropy/Reflection.h"
