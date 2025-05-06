// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

#include "Entropy/Reflection/DataObject/DataObject.h"
#include "Entropy/Reflection/Details/ContainerTypes.h"
#include "Entropy/Reflection/TypeInfoPtr.h"
#include "Entropy/Reflection/TypeInfoTraits.h"
#include <atomic>

namespace Entropy
{

namespace details
{
template <typename>
struct FillCommonTypeInfo;

template <typename, typename>
struct HandleIsConstructible;

template <typename, typename>
struct HandleIsCopyConstructible;

template <typename, typename>
struct HandleIsMoveConstructible;

template <typename, typename>
struct HandleIsDestructible;
} // namespace details

class DataObject;
class TypeInfoPtr;

/// <summary>
/// Holds runtime type information for any type that are accessible through different modules. The list of modules can
/// be set by overriding the TypeInfoTraits.
///
/// Any C++ type can have TypeInfo - not just reflected classes.
/// </summary>
class TypeInfo final
{
public:
    using ModuleTypes = Reflection::TypeInfoTraits<>::ModuleTypes;

private:
    using ContainerTraits = Entropy::details::ReflectionContainerTraits<TypeInfo>;

    using ConstructionHandler     = ContainerTraits::FunctionType<void*()>;
    using CopyConstructionHandler = ContainerTraits::FunctionType<void*(const void*)>;
    using MoveConstructionHandler = ContainerTraits::FunctionType<void*(void*)>;
    using DestructionHandler      = ContainerTraits::FunctionType<void(void*)>;

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
    };

public:
    ~TypeInfo();

    inline const ContainerTraits::StringType& GetTypeName() const { return _typeName; }

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

    inline bool RequireInitialization()
    {
        bool required = true;

        // Returns "exchanged = true" exactly once when we flip from the value from true -> false
        return _requireInitialization.compare_exchange_strong(required, false);
    }

    bool CanConstruct() const;
    DataObject Construct() const;

    bool CanCopyConstruct() const;
    /// <summary>
    /// src _must_ be the same type as what is being represented by this type info.
    /// </summary>
    /// <remarks>
    /// Use CreateDataObject<> for the safe version
    /// </remarks>
    DataObject DangerousCopyConstruct(const void* src) const;

    bool CanMoveConstruct() const;
    /// <summary>
    /// src _must_ be the same type as what is being represented by this type info.
    /// </summary>
    /// <remarks>
    /// Use CreateDataObject<> for the safe version
    /// </remarks>
    DataObject DangerousMoveConstruct(void* src) const;

private:
    void AddRef() const;
    void Release() const;

    void SetTypeName(ContainerTraits::StringType&& name);

    void SetConstructionHandler(ConstructionHandler&& handler);
    void SetCopyConstructionHandler(CopyConstructionHandler&& handler);
    void SetMoveConstructionHandler(MoveConstructionHandler&& handler);
    void SetDestructionHandler(DestructionHandler&& handler);

    inline void Destruct(void* dataPtr) const;

    ContainerTraits::StringType _typeName{};

    ConstructionHandler _constructionFn{};
    CopyConstructionHandler _copyConstructionFn{};
    MoveConstructionHandler _moveConstructionFn{};
    DestructionHandler _destructionFn{};

    ModuleTypes _modules;

    mutable std::atomic_int _refCount{0};

    std::atomic_bool _requireInitialization{true};

    //-----

    template <typename>
    friend TypeInfoPtr ReflectTypeAndGetTypeInfo() noexcept;

    template <typename>
    friend struct details::FillCommonTypeInfo;

    template <typename, typename>
    friend struct details::HandleIsConstructible;

    template <typename, typename>
    friend struct details::HandleIsCopyConstructible;

    template <typename, typename>
    friend struct details::HandleIsMoveConstructible;

    template <typename, typename>
    friend struct details::HandleIsDestructible;

    friend class DataObject;
    friend class TypeInfoPtr;
};

} // namespace Entropy
