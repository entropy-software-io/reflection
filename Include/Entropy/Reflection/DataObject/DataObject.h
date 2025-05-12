// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

#include "Entropy/Core/Details/Defines.h"
#include "Entropy/Reflection/Details/ContainerTypes.h"
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
    using ContainerTraits = details::ReflectionContainerTraits<DataObject>;

    struct DataObjectContainer
    {
        TypeInfoRef _typeInfo{};
        void* _data{};
        std::atomic_int _refCount{1};
        bool deallocate = false;
    };

    DataObject(const TypeInfo* typeInfo, void* data, bool deallocate);

    void Release();

public:
    DataObject() = default;
    DataObject(std::nullptr_t);
    DataObject(const DataObject& other);
    DataObject(DataObject&& other);

    ~DataObject();

    /// <summary>
    /// Casts the data into a usable type. This method does not have safety checks; check against null and use IsType<>
    /// to check if the cast is safe.
    /// </summary>
    template <typename T>
    inline const T& GetData() const
    {
        return *reinterpret_cast<const T*>(_container->_data);
    }

    /// <summary>
    /// Casts the data into a usable type. This method does not have safety checks; check against null and use
    /// IsExactType<> or CanCastTo<> to check if the cast is safe.
    /// </summary>
    template <typename T>
    inline T& GetData()
    {
        return *reinterpret_cast<T*>(_container->_data);
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

    inline DataObject& operator=(const DataObject& other);
    inline DataObject& operator=(DataObject&& other);

private:
    DataObjectContainer* _container{};

    friend class TypeInfo;
    friend struct DataObjectFactory;
};

} // namespace Entropy
