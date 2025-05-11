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
    ContainerTraits::Allocator<DataObjectContainer> alloc;
    _container = std::allocator_traits<decltype(alloc)>::allocate(alloc, 1);
    if (ENTROPY_LIKELY(_container != nullptr))
    {
        std::allocator_traits<decltype(alloc)>::construct(alloc, _container);

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

            ContainerTraits::Allocator<DataObjectContainer> alloc;
            std::allocator_traits<decltype(alloc)>::destroy(alloc, _container);
            std::allocator_traits<decltype(alloc)>::deallocate(alloc, _container, 1);
        }

        _container = nullptr;
    }
}

template <typename T>
inline bool DataObject::IsType() const
{
    if (ENTROPY_LIKELY(_container))
    {
        return _container->_typeInfo == ReflectTypeAndGetTypeInfo<T>();
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
