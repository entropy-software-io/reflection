// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

#include "Entropy/Reflection/DataObject/DataObject.h"
#include "Entropy/Reflection/TypeInfo/RuntimeReflectionMethods.h"

namespace Entropy
{

template <typename T>
inline DataObject CreateDataObject()
{
    TypeInfoPtr typeInfo = ReflectTypeAndGetTypeInfo<T>();
    if (ENTROPY_UNLIKELY(!typeInfo || !typeInfo->CanConstruct()))
    {
        return nullptr;
    }

    return typeInfo->Construct();
}

template <typename T>
inline DataObject CreateDataObject(const T& copy)
{
    TypeInfoPtr typeInfo = ReflectTypeAndGetTypeInfo<T>();
    if (ENTROPY_UNLIKELY(!typeInfo || !typeInfo->CanCopyConstruct()))
    {
        return nullptr;
    }

    return typeInfo->DangerousCopyConstruct(static_cast<const void*>(&copy));
}

template <typename T>
inline DataObject CreateDataObject(T&& move)
{
    TypeInfoPtr typeInfo = ReflectTypeAndGetTypeInfo<T>();
    if (ENTROPY_UNLIKELY(!typeInfo || !typeInfo->CanMoveConstruct()))
    {
        return nullptr;
    }

    return typeInfo->DangerousMoveConstruct(static_cast<void*>(&move));
}

} // namespace Entropy
