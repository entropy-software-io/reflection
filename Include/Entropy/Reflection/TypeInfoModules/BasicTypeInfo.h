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
    inline const ContainerTraits::StringType& GetTypeName() const { return _typeName; }

private:
    void SetClassName(ContainerTraits::StringType&& name) { _typeName = std::move(name); }

    ContainerTraits::StringType _typeName{};

    template <typename, typename, typename>
    friend struct FillModuleTypeInfo;
};

//----------------

/// <summary>
/// Initializes the BasicTypeInfo module. Called once per reflected type
/// </summary>
template <typename T>
struct FillModuleTypeInfo<BasicTypeInfo, T> : public DefaultFillModuleTypeInfo<BasicTypeInfo>
{
    void HandleType(BasicTypeInfo& module) { module.SetClassName(typeid(T).name()); }
};

} // namespace Reflection
} // namespace Entropy
