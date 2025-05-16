// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#include "Entropy/Reflection/TypeInfo/TypeInfoRef.h"
#include "Entropy/Reflection/TypeInfo/TypeInfo.h"

namespace Entropy
{

TypeInfoRef::TypeInfoRef(const TypeInfo* ptr)
    : _ptr(ptr)
{
    if (_ptr)
    {
        _ptr->AddRef();
    }
}

TypeInfoRef::TypeInfoRef(const TypeInfoRef& other)
    : TypeInfoRef(other._ptr)
{
}

TypeInfoRef::TypeInfoRef(TypeInfoRef&& other)
    : _ptr(other._ptr)
{
    other._ptr = nullptr;
}

TypeInfoRef::~TypeInfoRef()
{
    if (_ptr)
    {
        _ptr->Release();
        _ptr = nullptr;
    }
}

TypeInfoRef& TypeInfoRef::operator=(const TypeInfoRef& other)
{
    this->~TypeInfoRef();
    new (this) TypeInfoRef(other);
    return *this;
}

TypeInfoRef& TypeInfoRef::operator=(TypeInfoRef&& other)
{
    this->~TypeInfoRef();
    new (this) TypeInfoRef(std::move(other));
    return *this;
}

} // namespace Entropy
