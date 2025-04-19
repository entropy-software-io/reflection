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

    template <typename T, std::size_t Index>
    struct ModuleIndexHelper<T, std::tuple<>, Index>
    {
        static_assert(false, "Module type not found");
    };

public:
    template <typename TModule>
    const TModule& GetModule() const
    {
        static constexpr std::size_t idx = ModuleIndexHelper<TModule, ModuleTypes>::value;
        return std::get<idx>(_modules);
    }

    template <typename TModule>
    TModule& GetModule()
    {
        static constexpr std::size_t idx = ModuleIndexHelper<TModule, ModuleTypes>::value;
        return std::get<idx>(_modules);
    }

private:
    ModuleTypes _modules;
};

} // namespace Entropy
