// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

#include "Entropy/Core/Details/FunctionTraits.h"
#include "Entropy/Core/Details/StringOps.h"
#include "Entropy/Reflection/DataObject/DataObject.h"
#include "Entropy/Reflection/Details/TypeId.h"
#include "Entropy/Reflection/TypeInfo/TypeInfoModuleList.h"
#include "Entropy/Reflection/TypeInfo/TypeInfoRef.h"
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

/// <summary>
/// Holds runtime type information for any type that are accessible through different modules. The list of modules can
/// be set by overriding the TypeInfoTraits.
///
/// Any C++ type can have TypeInfo - not just reflected classes.
/// </summary>
class TypeInfo final
{
    // NOLINTBEGIN(clang-analyzer-optin.core.EnumCastOutOfRange)
    enum class Flags : int
    {
        None         = 0x0,
        IsConst      = 1 << 0,
        IsPointer    = 1 << 1,
        IsLReference = 1 << 2,
        IsRReference = 1 << 3,
        IsArray      = 1 << 4, // Static allocated array. (e.g. char[4])
    };
    friend inline constexpr Flags operator|(Flags x, Flags y)
    {
        return static_cast<Flags>(static_cast<typename std::underlying_type<Flags>::type>(x) |
                                  static_cast<typename std::underlying_type<Flags>::type>(y));
    }
    friend inline constexpr Flags operator&(Flags x, Flags y)
    {
        return static_cast<Flags>(static_cast<typename std::underlying_type<Flags>::type>(x) &
                                  static_cast<typename std::underlying_type<Flags>::type>(y));
    }
    friend inline Flags& operator|=(Flags& x, Flags y)
    {
        reinterpret_cast<typename std::underlying_type<Flags>::type&>(x) |=
            static_cast<typename std::underlying_type<Flags>::type>(y);
        return x;
    }
    // NOLINTEND(clang-analyzer-optin.core.EnumCastOutOfRange)

public:
    using ModuleTypes = Reflection::TypeInfoModuleTraits::ModuleTypes;

private:
    using ConstructionHandler     = Traits::FunctionTraits<void*()>::Function;
    using CopyConstructionHandler = Traits::FunctionTraits<void*(const void*)>::Function;
    using MoveConstructionHandler = Traits::FunctionTraits<void*(void*)>::Function;
    using DestructionHandler      = Traits::FunctionTraits<void(void*)>::Function;

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

    inline const StringOps::StringType& GetTypeName() const { return _typeName; }
    inline TypeId GetTypeId() const { return _typeId; }

    template <typename TModule>
    const TModule& Get() const noexcept
    {
        static constexpr std::size_t idx = ModuleIndexHelper<TModule, ModuleTypes>::value;
        return std::get<idx>(_modules);
    }

    template <typename TModule>
    TModule& Get() noexcept
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

    bool IsConst() const;
    bool IsPointer() const;
    bool IsArray() const;
    bool IsPointerOrArray() const;
    bool IsLValueReference() const;
    bool IsRValueReference() const;
    bool IsReference() const;

    /// <summary>
    /// Returns true if we have any qualifiers, like const, *, &, etc...
    /// </summary>
    inline bool IsQualifiedType() const;

    /// <summary>
    /// Returns the type with one layer of qualifiers removed. If this type is not qualified, this object is returned.
    /// </summary>
    /// <remarks>
    /// const T -> T
    /// const T& -> const T
    /// const T* -> const T
    /// T* const -> T (both const and * are at the same level in this case)
    /// </remarks>
    inline const TypeInfo* GetNextUnqualifiedType() const;

    /// <summary>
    /// Returns the type with all qualifiers removed. If this type is not qualified, this object is returned.
    /// </summary>
    /// <remarks>
    /// T const*& -> T
    /// </remarks>
    const TypeInfo* GetFullyUnqualifiedType() const;

    /// <summary>
    /// Conservative check to see if a DataObject of this type can be cast to the specified type.
    /// </summary>
    /// <remarks>
    /// This will not allow conversions of the unqualified data type to a different type. Static casts and implicit
    /// conversions that are normally allowed by C++ are not accepted because we cast a raw void*. Instead, a cast to
    /// the underlying type must be made and then let the compiler do the actual conversion to the desired type.
    /// <remarks>
    bool CanCastTo(const TypeInfo* other) const noexcept;

private:
    void AddRef() const;
    void Release() const;

    void SetTypeName(StringOps::StringType&& name);
    void SetTypeId(TypeId typeId);

    void SetConstructionHandler(ConstructionHandler&& handler);
    void SetCopyConstructionHandler(CopyConstructionHandler&& handler);
    void SetMoveConstructionHandler(MoveConstructionHandler&& handler);
    void SetDestructionHandler(DestructionHandler&& handler);

    void SetIsConst();
    void SetIsPointer();
    void SetIsLReference();
    void SetIsRReference();
    void SetIsArray();

    void SetNextUnqualifiedType(const TypeInfo* typeInfo);

    inline void Destruct(void* dataPtr) const;

    StringOps::StringType _typeName{};

    ConstructionHandler _constructionFn{};
    CopyConstructionHandler _copyConstructionFn{};
    MoveConstructionHandler _moveConstructionFn{};
    DestructionHandler _destructionFn{};

    ModuleTypes _modules{};

    TypeInfoRef _nextUnqualifiedType{};

    Flags _flags = Flags::None;

    TypeId _typeId = cInvalidTypeId;

    mutable std::atomic_int _refCount{0};

    std::atomic_bool _requireInitialization{true};

    //-----

    template <typename>
    friend const TypeInfo* ReflectTypeAndGetTypeInfo() noexcept;

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
    friend class TypeInfoRef;
};

} // namespace Entropy
