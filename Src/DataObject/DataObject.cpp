// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#include "Entropy/Reflection/DataObject/DataObject.h"
#include "Entropy/Core/Details/AllocatorTraits.h"
#include "Entropy/Core/Log.h"
#include "Entropy/Reflection/TypeInfo/RuntimeReflectionMethods.h"
#include "Entropy/Reflection/TypeInfo/TypeInfo.h"

namespace Entropy
{

DataObject::DataObject(std::nullptr_t) {}

DataObject::DataObject(const TypeInfo* typeInfo, void* data, bool wrapped, DataPointerType pointerType)
{
    _container = AllocatorOps::CreateInstance<DataObjectContainer>();
    ENTROPY_ASSERT(_container);

    if (ENTROPY_LIKELY(_container != nullptr))
    {
        _container->_typeInfo    = typeInfo;
        _container->_data        = data;
        _container->_wrapped     = wrapped;
        _container->_pointerType = pointerType;
    }
}

DataObject::DataObject(const DataObject& other)
    : _container(other._container)
{
    if (_container != nullptr)
    {
        ++_container->_refCount;
    }
}

DataObject::DataObject(DataObject&& other) noexcept
    : _container(other._container)
{
    other._container = nullptr;
}

DataObject::~DataObject() { Release(); }

void DataObject::Release()
{
    if (_container)
    {
        if (--_container->_refCount == 0)
        {
            if (!_container->_wrapped)
            {
                _container->_typeInfo->Destruct(_container->_data);
            }

            AllocatorOps::DestroyInstance(_container);
        }

        _container = nullptr;
    }
}

DataObject& DataObject::operator=(const DataObject& other)
{
    Release();
    new (this) DataObject(other);
    return *this;
}

DataObject& DataObject::operator=(DataObject&& other)
{
    Release();
    new (this) DataObject(std::move(other));
    return *this;
}

bool DataObject::CanCastTo(const TypeInfo* typeInfo) const { return _container->_typeInfo->CanCastTo(typeInfo); }

} // namespace Entropy
