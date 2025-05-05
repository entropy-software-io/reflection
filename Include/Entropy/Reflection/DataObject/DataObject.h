// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

#include "Entropy/Core/Details/Defines.h"
#include "Entropy/Reflection/Details/ContainerTypes.h"
#include "Entropy/Reflection/TypeInfoPtr.h"
#include <atomic>

namespace Entropy
{

class DataObject;

template <typename T, typename... TArgs>
DataObject CreateDataObject(TArgs&&...);

/// <summary>
/// Wraps an arbitrary data object through type info. This can be treated like a smart pointer.
/// </summary>
class DataObject final
{
private:
    using ContainerTraits = details::ReflectionContainerTraits<DataObject>;

    struct DataObjectContainer
    {
        TypeInfoPtr _typeInfo{};
        void* _data{};
        std::atomic_int _refCount{1};
    };

    DataObject(TypeInfoPtr typeInfo, void* data);

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
    /// Casts the data into a usable type. This method does not have safety checks; check against null and use IsType<>
    /// to check if the cast is safe.
    /// </summary>
    template <typename T>
    inline T& GetData()
    {
        return *reinterpret_cast<T*>(_container->_data);
    }

    inline TypeInfoPtr GetTypeInfo() const
    {
        if (ENTROPY_LIKELY(_container))
        {
            return _container->_typeInfo;
        }
        return nullptr;
    }

    template <typename T>
    inline bool IsType() const;

    inline bool operator==(std::nullptr_t) const { return (_container == nullptr); }
    inline bool operator!=(std::nullptr_t) const { return (_container != nullptr); }
    inline operator bool() const { return _container != nullptr; }

    inline DataObject& operator=(const DataObject& other);
    inline DataObject& operator=(DataObject&& other);

private:
    DataObjectContainer* _container{};

    friend class Entropy::TypeInfo;

    template <typename T>
    friend DataObject CreateDataObject();

    template <typename T>
    friend DataObject CreateDataObject(const T&);

    template <typename T>
    friend DataObject CreateDataObject(T&&);
};

} // namespace Entropy
