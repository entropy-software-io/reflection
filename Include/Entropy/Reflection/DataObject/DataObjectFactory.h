// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

#include "Entropy/Reflection/DataObject/DataObject.h"
#include "Entropy/Reflection/TypeInfo/RuntimeReflectionMethods.h"
#include <type_traits>

namespace Entropy
{

struct DataObjectFactory
{
private:
    template <typename T, typename = void>
    struct ToVoid
    {
        void* operator()(T&& param) const { return reinterpret_cast<void*>(&param); }
    };

    template <typename T>
    struct ToVoid<T, typename std::enable_if<std::is_const<typename std::remove_reference<T>::type>::value>::type>
    {
        void* operator()(T&& param) const { return const_cast<void*>(reinterpret_cast<const void*>(&param)); }
    };

public:
    template <typename T>
    inline static DataObject Create()
    {
        const TypeInfo* typeInfo = ReflectTypeAndGetTypeInfo<T>();
        if (ENTROPY_UNLIKELY(!typeInfo || !typeInfo->CanConstruct()))
        {
            return nullptr;
        }

        return typeInfo->Construct();
    }

    template <typename T>
    inline static DataObject Create(const T& copy)
    {
        const TypeInfo* typeInfo = ReflectTypeAndGetTypeInfo<T>();
        if (ENTROPY_UNLIKELY(!typeInfo || !typeInfo->CanCopyConstruct()))
        {
            return nullptr;
        }

        return typeInfo->DangerousCopyConstruct(static_cast<const void*>(&copy));
    }

    template <typename T>
    inline static DataObject Create(T&& move)
    {
        const TypeInfo* typeInfo = ReflectTypeAndGetTypeInfo<T>();
        if (ENTROPY_UNLIKELY(!typeInfo || !typeInfo->CanMoveConstruct()))
        {
            return nullptr;
        }

        return typeInfo->DangerousMoveConstruct(static_cast<void*>(&move));
    }

    template <typename T>
    inline static DataObject Wrap(T&& value)
    {
        const TypeInfo* typeInfo = ReflectTypeAndGetTypeInfo<T>();
        return DataObject(typeInfo, ToVoid<T>{}(std::forward<T>(value)), false /* deallocate */);
    }
};

} // namespace Entropy
