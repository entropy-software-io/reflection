// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

#include "Entropy/Core/Details/StrongAlias.h"
#include "Entropy/Reflection/Details/ContainerTypes.h"

namespace Entropy
{

ENTROPY_DEFINE_STRONG_ALIAS(TypeId, unsigned int)

namespace details
{

ReflectionContainerTraits<TypeId>::StringType MakeTypeName(const char* rawTypeName);

template <typename T>
ReflectionContainerTraits<TypeId>::StringType MakeTypeName()
{
    return MakeTypeName(typeid(T).name());
}

TypeId MakeTypeIdFromTypeName(const char* rawTypeName);

template <typename T>
TypeId MakeTypeIdFromTypeName()
{
    return MakeTypeIdFromTypeName(typeid(T).name());
}

} // namespace details

namespace Traits
{

template <typename T, typename = void>
struct TypeId
{
    inline Entropy::TypeId operator()() const
    {
        static Entropy::TypeId id = Entropy::details::MakeTypeIdFromTypeName<T>();
        return id;
    }
};

} // namespace Traits

} // namespace Entropy
