// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

#include "Entropy/Core/Details/Defines.h"
#include "Entropy/Reflection/TypeInfo/TypeInfoRef.h"
#include <atomic>

namespace Entropy
{

class DataObject;
struct DataObjectFactory;

/// <summary>
/// Wraps an arbitrary data object through type info. This can be treated like a smart pointer.
/// </summary>
class DataObject final
{
private:
    enum class DataPointerType : byte
    {
        AddressOf,
        Direct
    };

    // These helpers grab the correct void* value to convert.
    // Normally, we just want to return what we have, but there is a special case with wrapped pointer type. There, we
    // store the pointer value directly, so we return the address of our data member to de-reference the right value.
    //
    // See DataObjectFactory::Wrap for the other end of this.
    template <bool TDirectDataPointer, typename T, typename = void>
    struct GetVoidPtr
    {
        void* operator()(void*& data) const { return data; }
    };

    template <typename T>
    struct GetVoidPtr<true, T, typename std::enable_if<std::is_pointer<T>::value>::type>
    {
        void* operator()(void*& data) const { return &data; }
    };

    template <bool TDirectDataPointer, typename T, typename = void>
    struct GetConstVoidPtr
    {
        const void* operator()(const void*& data) const { return data; }
    };

    template <typename T>
    struct GetConstVoidPtr<true, T, typename std::enable_if<std::is_pointer<T>::value>::type>
    {
        const void* operator()(const void*& data) const { return &data; }
    };

    struct DataObjectContainer
    {
        TypeInfoRef _typeInfo{};
        void* _data{};
        std::atomic_int _refCount{1};
        DataPointerType _pointerType = DataPointerType::AddressOf;
        bool _wrapped                = false;
    };

    DataObject(const TypeInfo* typeInfo, void* data, bool wrapped, DataPointerType pointerType);

    void Release();

public:
    DataObject() = default;
    DataObject(std::nullptr_t);
    DataObject(const DataObject& other);
    DataObject(DataObject&& other) noexcept;

    ~DataObject();

    /// <summary>
    /// Casts the data into a usable type. This method does not have safety checks; check against null and use IsType<>
    /// to check if the cast is safe.
    /// </summary>
    template <typename T>
    inline const T& GetData() const
    {
        if (_container->_pointerType == DataPointerType::Direct)
        {
            return *reinterpret_cast<const T*>(GetConstVoidPtr<true, T>{}(_container->_data));
        }
        else
        {
            return *reinterpret_cast<const T*>(GetConstVoidPtr<false, T>{}(_container->_data));
        }
    }

    /// <summary>
    /// Casts the data into a usable type. This method does not have safety checks; check against null and use
    /// IsExactType<> or CanCastTo<> to check if the cast is safe.
    /// </summary>
    template <typename T>
    inline T& GetData()
    {
        if (_container->_pointerType == DataPointerType::Direct)
        {
            return *reinterpret_cast<T*>(GetVoidPtr<true, T>{}(_container->_data));
        }
        else
        {
            return *reinterpret_cast<T*>(GetVoidPtr<false, T>{}(_container->_data));
        }
    }

    inline const TypeInfo* GetTypeInfo() const
    {
        if (ENTROPY_LIKELY(_container))
        {
            return _container->_typeInfo;
        }
        return nullptr;
    }

    /// <summary>
    /// Quick check to see if we are the exact type.
    /// </summary>
    template <typename T>
    inline bool IsExactType() const;

    /// <summary>
    /// A more involved check to see if we can cast the specified type.
    /// </summary>
    template <typename T>
    inline bool CanCastTo() const;

    inline bool operator==(std::nullptr_t) const { return (_container == nullptr); }
    inline bool operator!=(std::nullptr_t) const { return (_container != nullptr); }
    inline operator bool() const { return _container != nullptr; }

    DataObject& operator=(const DataObject& other);
    DataObject& operator=(DataObject&& other);

private:
    bool CanCastTo(const TypeInfo* typeInfo) const;

    DataObjectContainer* _container{};

    friend class TypeInfo;
    friend struct DataObjectFactory;
};

} // namespace Entropy

#include "DataObject.inl"
