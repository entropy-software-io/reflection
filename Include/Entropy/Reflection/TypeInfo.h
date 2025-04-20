// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

#include "Entropy/Reflection/TypeInfoTraits.h"

namespace Entropy
{

/// <summary>
/// Holds runtime type information for any type that are accessible through different modules. The list of modules can
/// be set by overriding the TypeInfoTraits.
///
/// Any C++ type can have TypeInfo - not just reflected classes.
/// </summary>
class TypeInfo
{
public:
    using ModuleTypes = Reflection::TypeInfoTraits<TypeInfo>::ModuleTypes;

private:
    template <typename TModule, typename TModuleTypes, std::size_t Index = 0>
    struct ModuleIndexHelper;

    template <typename TModule, typename TFirstModule, typename... TOtherModules, std::size_t Index>
    struct ModuleIndexHelper<TModule, std::tuple<TFirstModule, TOtherModules...>, Index>
        : std::conditional<std::is_same<TModule, TFirstModule>::value, std::integral_constant<std::size_t, Index>,
                           ModuleIndexHelper<TModule, std::tuple<TOtherModules...>, Index + 1>>::type
    {
    };

    template <typename TModule, std::size_t Index>
    struct ModuleIndexHelper<TModule, std::tuple<>, Index>
    {
#if __cplusplus >= 201300
        static_assert(false, "Module type not found");
#endif
    };

public:
    template <typename TModule>
    const TModule& Get() const
    {
        static constexpr std::size_t idx = ModuleIndexHelper<TModule, ModuleTypes>::value;
        return std::get<idx>(_modules);
    }

    template <typename TModule>
    TModule& Get()
    {
        static constexpr std::size_t idx = ModuleIndexHelper<TModule, ModuleTypes>::value;
        return std::get<idx>(_modules);
    }

private:
    ModuleTypes _modules;
};

} // namespace Entropy
