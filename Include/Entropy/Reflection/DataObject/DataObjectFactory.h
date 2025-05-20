// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

#include "Entropy/Reflection/DataObject/DataObject.h"

#include <type_traits>

namespace Entropy
{

template <typename T>
const TypeInfo* ReflectTypeAndGetTypeInfo() noexcept;

struct DataObjectFactory
{
private:
    // These methods take the value to wrap and stuff it into the pointer that the data object holds.
    // If the value is itself a pointer, we hold on to that value directly. Otherwise, we grab a reference to what was
    // passed in.
    template <typename T, typename = void>
    struct ToVoid
    {
        static constexpr DataObject::DataPointerType PointerType = DataObject::DataPointerType::AddressOf;

        void* operator()(T&& param) const { return reinterpret_cast<void*>(&param); }
    };

    template <typename T>
    struct ToVoid<
        T, typename std::enable_if<
               std::is_array<typename std::remove_reference<T>::type>::value &&
               !std::is_const<typename std::remove_extent<typename std::remove_reference<T>::type>::type>::value>::type>
    {
        static constexpr DataObject::DataPointerType PointerType = DataObject::DataPointerType::Direct;

        void* operator()(T&& param) const { return reinterpret_cast<void*>(param); }
    };

    template <typename T>
    struct ToVoid<
        T, typename std::enable_if<
               std::is_array<typename std::remove_reference<T>::type>::value &&
               std::is_const<typename std::remove_extent<typename std::remove_reference<T>::type>::type>::value>::type>
    {
        static constexpr DataObject::DataPointerType PointerType = DataObject::DataPointerType::Direct;

        void* operator()(T&& param) const { return const_cast<void*>(reinterpret_cast<const void*>(param)); }
    };

    template <typename T>
    struct ToVoid<T, typename std::enable_if<!std::is_array<typename std::remove_reference<T>::type>::value &&
                                             std::is_const<typename std::remove_reference<T>::type>::value>::type>
    {
        static constexpr DataObject::DataPointerType PointerType = DataObject::DataPointerType::AddressOf;

        void* operator()(T&& param) const { return const_cast<void*>(reinterpret_cast<const void*>(&param)); }
    };

public:
    /// <summary>
    /// Attempts to allocate an object of the specified type. The type must have a public constructor taking zero
    /// arguments.
    /// </summary>
    /// <remarks>
    /// The allocated object will be cleaned up when the DataObject's ref count reaches zero.
    /// </remarks>
    template <typename T>
    inline static DataObject Create()
    {
        const TypeInfo* typeInfo = ReflectTypeAndGetTypeInfo<T>();
        if (ENTROPY_UNLIKELY(!typeInfo || !typeInfo->CanConstruct()))
        {
            return nullptr;
        }

        return typeInfo->Construct();
    }

    /// <summary>
    /// Attempts to allocate an object of the specified type. The type must have a public copy constructor.
    /// </summary>
    /// <remarks>
    /// The allocated object will be cleaned up when the DataObject's ref count reaches zero.
    /// </remarks>
    template <typename T>
    inline static DataObject Create(const T& copy)
    {
        const TypeInfo* typeInfo = ReflectTypeAndGetTypeInfo<T>();
        if (ENTROPY_UNLIKELY(!typeInfo || !typeInfo->CanCopyConstruct()))
        {
            return nullptr;
        }

        return typeInfo->DangerousCopyConstruct(static_cast<const void*>(&copy));
    }

    /// <summary>
    /// Attempts to allocate an object of the specified type. The type must have a public move constructor.
    /// </summary>
    /// <remarks>
    /// The allocated object will be cleaned up when the DataObject's ref count reaches zero.
    /// </remarks>
    template <typename T>
    inline static DataObject Create(T&& move)
    {
        const TypeInfo* typeInfo = ReflectTypeAndGetTypeInfo<T>();
        if (ENTROPY_UNLIKELY(!typeInfo || !typeInfo->CanMoveConstruct()))
        {
            return nullptr;
        }

        return typeInfo->DangerousMoveConstruct(static_cast<void*>(&move));
    }

    /// <summary>
    /// Wraps a pointer to an existing object in a DataObject.
    /// </summary>
    /// <remarks>
    /// The value passed in is referenced, but never copied. It is extremely important that the DataObject not live
    /// passed the lifetime of the value.
    /// </remarks>
    template <typename T>
    inline static DataObject Wrap(T&& value)
    {
        bool is_rvalue_ref = std::is_rvalue_reference<decltype((value))>::value;
        bool is_lvalue_ref = std::is_lvalue_reference<decltype((value))>::value;

        bool is_t_rvalue_ref = std::is_rvalue_reference<T&&>::value;
        bool is_t_lvalue_ref = std::is_lvalue_reference<T>::value;

        const TypeInfo* typeInfo = ReflectTypeAndGetTypeInfo<T>();
        return DataObject(typeInfo, ToVoid<T>{}(std::forward<T>(value)), true /* wrapped */, ToVoid<T>::PointerType);
    }
};

} // namespace Entropy
