// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

#include "Entropy/Reflection/RuntimeReflectionMethods.h"

namespace Entropy
{

DataObject::DataObject(std::nullptr_t) {}

DataObject::DataObject(const TypeInfo* typeInfo, void* data)
{
    ContainerTraits::Allocator<DataObjectContainer> alloc;
    _container = std::allocator_traits<decltype(alloc)>::allocate(alloc, 1);
    if (ENTROPY_LIKELY(_container != nullptr))
    {
        std::allocator_traits<decltype(alloc)>::construct(alloc, _container);

        _container->_typeInfo = typeInfo;
        _container->_data     = data;
    }
}

DataObject::~DataObject() { Release(); }

void DataObject::Release()
{
    if (_container)
    {
        if (--_container->_refCount == 0)
        {
            _container->_typeInfo->Get<Reflection::BasicTypeInfo>().Destruct(_container->_data);

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

} // namespace Entropy
