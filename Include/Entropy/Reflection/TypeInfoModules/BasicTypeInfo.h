// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

#include "Entropy/Reflection/Details/ContainerTypes.h"
#include "TypeInfoModule.h"

namespace Entropy
{
namespace Reflection
{

/// <summary>
/// Contains basic type information that can be accessed at runtime
/// </summary>
class BasicTypeInfo
{
private:
    using ContainerTraits = details::ReflectionContainerTraits<BasicTypeInfo>;

public:
    void SetClassName(ContainerTraits::StringType&& name) { _typeName = std::move(name); }

    inline const ContainerTraits::StringType& GetTypeName() const { return _typeName; }

private:
    ContainerTraits::StringType _typeName{};
};

//----------------

/// <summary>
/// Initializes the BasicTypeInfo module. Called once per reflected type
/// </summary>
template <typename T>
struct FillReflectionInfo<BasicTypeInfo, T>
{
    void operator()(BasicTypeInfo& basicTypeInfo) const { basicTypeInfo.SetClassName(typeid(T).name()); }
};

} // namespace Reflection
} // namespace Entropy
