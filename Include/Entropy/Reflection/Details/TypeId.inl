// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#include "Entropy/Core/Details/Defines.h"
#include "Entropy/Core/Details/Hash.h"
#include "Entropy/Reflection/Details/ContainerTypes.h"

#ifndef ENTROPY_MSVC
#include <cxxabi.h>
#endif

namespace Entropy
{

namespace details
{

ReflectionContainerTraits<TypeId>::StringType MakeTypeName(const char* rawTypeName)
{
    using ContainerTraits = ReflectionContainerTraits<TypeId>;
    using StrOps          = StringOps<ContainerTraits::StringType>;

#ifdef ENTROPY_MSVC
    ContainerTraits::StringType ret = rawTypeName;
    ret                             = StrOps::Replace(ret, "struct ", "");
    ret                             = StrOps::Replace(ret, "class ", "");
    ret                             = StrOps::Replace(ret, "enum ", "");
    ret                             = StrOps::Replace(ret, " >", ">");
    return ret;
#else
    int status;
    char* realName = abi::__cxa_demangle(rawTypeName, 0, 0, &status);

    String ret = realName;
    free(realName);
    ret = ret.Replace(", "_EStr, ","_EStr).Replace("::__2::"_EStr, "::"_EStr);
    return ret;
#endif
}

TypeId MakeTypeIdFromTypeName(const char* rawTypeName)
{
    using ContainerTraits = ReflectionContainerTraits<TypeId>;
    using StrOps          = StringOps<ContainerTraits::StringType>;

    auto typeName = MakeTypeName(rawTypeName);

    return TypeId(StringHash(StrOps::GetStr(typeName), StrOps::GetLength(typeName)));
}

} // namespace details

} // namespace Entropy
