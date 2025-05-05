// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#include "TypeInfo.h"

namespace Entropy
{

TypeInfoPtr::TypeInfoPtr(const TypeInfo* ptr)
    : _ptr(ptr)
{
    if (_ptr)
    {
        _ptr->AddRef();
    }
}

TypeInfoPtr::TypeInfoPtr(const TypeInfoPtr& other)
    : TypeInfoPtr(other._ptr)
{
}

TypeInfoPtr::TypeInfoPtr(TypeInfoPtr&& other)
    : _ptr(other._ptr)
{
    other._ptr = nullptr;
}

TypeInfoPtr::~TypeInfoPtr()
{
    if (_ptr)
    {
        _ptr->Release();
        _ptr = nullptr;
    }
}

TypeInfoPtr& TypeInfoPtr::operator=(const TypeInfoPtr& other)
{
    this->~TypeInfoPtr();
    new (this) TypeInfoPtr(other);
    return *this;
}

TypeInfoPtr& TypeInfoPtr::operator=(TypeInfoPtr&& other)
{
    this->~TypeInfoPtr();
    new (this) TypeInfoPtr(std::move(other));
    return *this;
}

} // namespace Entropy
