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

ReflectionContainerTraits<TypeId>::StringType MakeTypeNameFromRawName(const char* rawTypeName)
{
    using ContainerTraits = ReflectionContainerTraits<TypeId>;
    using StrOps          = StringOps<ContainerTraits::StringType>;

#ifdef ENTROPY_MSVC
    ContainerTraits::StringType ret = rawTypeName;
    StrOps::Replace(ret, "struct ", "");
    StrOps::Replace(ret, "class ", "");
    StrOps::Replace(ret, "enum ", "");
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

ReflectionContainerTraits<TypeId>::StringType MakeTypeNameNoTemplateParamsFromRawName(const char* rawTypeName)
{
    using ContainerTraits = ReflectionContainerTraits<TypeId>;
    using StrOps          = StringOps<ContainerTraits::StringType>;

    auto ret = MakeTypeNameFromRawName(rawTypeName);

    int first = StrOps::FindFirst(ret, '<');
    if (first >= 0)
    {
        StrOps::SubString(ret, 0, first);
    }

    return ret;
}

TypeId MakeTypeIdFromTypeName(const ReflectionContainerTraits<TypeId>::StringType& typeName)
{
    using ContainerTraits = ReflectionContainerTraits<TypeId>;
    using StrOps          = StringOps<ContainerTraits::StringType>;

    return TypeId(StringHash(StrOps::GetStr(typeName), StrOps::GetLength(typeName)));
}

} // namespace details

} // namespace Entropy
