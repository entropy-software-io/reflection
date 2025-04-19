// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

#include "Entropy/Reflection/Details/TypeTraits.h"
#include "TypeInfo.h"

namespace Entropy
{

namespace details
{

template <typename TType, typename TModuleTypes>
struct FillModuleTypes;

template <typename TType>
struct FillModuleTypes<TType, std::tuple<>>
{
    void operator()(TypeInfo& typeInfo) const {}
};

template <typename TType, typename TFirstModule, typename... TOtherModules>
struct FillModuleTypes<TType, std::tuple<TFirstModule, TOtherModules...>>
{
    void operator()(TypeInfo& typeInfo) const
    {
        Entropy::Reflection::FillReflectionInfo<TFirstModule, TType>{}(typeInfo.GetModule<TFirstModule>());
        FillModuleTypes<TType, std::tuple<TOtherModules...>>{}(typeInfo);
    }
};

//---------------

template <typename T>
TypeInfo MakeSingleTypeInfo()
{
    TypeInfo typeInfo{};

    FillModuleTypes<T, TypeInfo::ModuleTypes>{}(typeInfo);

    return typeInfo;
}

} // namespace details

template <typename T>
const TypeInfo* ReflectTypeAndGetTypeInfo()
{
    static TypeInfo typeInfo = details::MakeSingleTypeInfo<T>();
    return &typeInfo;
}

template <typename T>
void ReflectType()
{
    ReflectTypeAndGetTypeInfo<T>();
}

} // namespace Entropy
