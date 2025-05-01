// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

#include "Entropy/Core/Details/StrongAlias.h"

namespace Entropy
{

ENTROPY_DEFINE_STRONG_ALIAS(TypeId, unsigned int)

namespace details
{

TypeId MakeTypeIdFromTypeName(const char* typeName);

}

namespace Traits
{

template <typename T, typename = void>
struct TypeId
{
    inline TypeId operator()() const
    {
        static TypeId id = Entropy::details::MakeTypeIdFromTypeName(typeid(T).name());
        return id;
    }
};

} // namespace Traits

} // namespace Entropy
