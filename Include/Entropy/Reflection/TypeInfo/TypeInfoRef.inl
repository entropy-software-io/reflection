// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#include "TypeInfo.h"

namespace Entropy
{

inline TypeInfoRef::TypeInfoRef(const TypeInfo* ptr)
    : _ptr(ptr)
{
    if (_ptr)
    {
        _ptr->AddRef();
    }
}

inline TypeInfoRef::TypeInfoRef(const TypeInfoRef& other)
    : TypeInfoRef(other._ptr)
{
}

inline TypeInfoRef::TypeInfoRef(TypeInfoRef&& other)
    : _ptr(other._ptr)
{
    other._ptr = nullptr;
}

inline TypeInfoRef::~TypeInfoRef()
{
    if (_ptr)
    {
        _ptr->Release();
        _ptr = nullptr;
    }
}

inline TypeInfoRef& TypeInfoRef::operator=(const TypeInfoRef& other)
{
    this->~TypeInfoRef();
    new (this) TypeInfoRef(other);
    return *this;
}

inline TypeInfoRef& TypeInfoRef::operator=(TypeInfoRef&& other)
{
    this->~TypeInfoRef();
    new (this) TypeInfoRef(std::move(other));
    return *this;
}

} // namespace Entropy
