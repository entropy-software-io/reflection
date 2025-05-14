// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

#include "Entropy/Reflection/TypeInfo/RuntimeReflectionMethods.h"
#include "Entropy/Reflection/TypeInfo/TypeInfo.h"

namespace Entropy
{

inline DataObject::DataObject(std::nullptr_t) {}

inline DataObject::DataObject(const TypeInfo* typeInfo, void* data, bool deallocate)
{
    _container = AllocatorOps::CreateInstance<DataObjectContainer>();
    ENTROPY_ASSERT(_container)

    if (ENTROPY_LIKELY(_container != nullptr))
    {
        _container->_typeInfo  = typeInfo;
        _container->_data      = data;
        _container->deallocate = deallocate;
    }
}

inline DataObject::DataObject(const DataObject& other)
    : _container(other._container)
{
    if (_container != nullptr)
    {
        ++_container->_refCount;
    }
}

inline DataObject::DataObject(DataObject&& other)
    : _container(other._container)
{
    other._container = nullptr;
}

inline DataObject::~DataObject() { Release(); }

inline void DataObject::Release()
{
    if (_container)
    {
        if (--_container->_refCount == 0)
        {
            if (_container->deallocate)
            {
                _container->_typeInfo->Destruct(_container->_data);
            }

            AllocatorOps::DestroyInstance(_container);
        }

        _container = nullptr;
    }
}

template <typename T>
inline bool DataObject::IsExactType() const
{
    if (ENTROPY_LIKELY(_container))
    {
        return _container->_typeInfo == ReflectTypeAndGetTypeInfo<T>();
    }
    return false;
}

template <typename T>
inline bool DataObject::CanCastTo() const
{
    if (ENTROPY_LIKELY(_container))
    {
        return _container->_typeInfo->CanCastTo(ReflectTypeAndGetTypeInfo<T>());
    }
    return false;
}

inline DataObject& DataObject::operator=(const DataObject& other)
{
    Release();
    new (this) DataObject(other);
    return *this;
}

inline DataObject& DataObject::operator=(DataObject&& other)
{
    Release();
    new (this) DataObject(std::move(other));
    return *this;
}

} // namespace Entropy
