// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#include "Entropy/Core/Details/Defines.h"
#include "Entropy/Core/Details/Hash.h"
#include "Entropy/Core/Details/StringOps.h"

#ifndef ENTROPY_MSVC
#include <cxxabi.h>
#endif

namespace Entropy
{

namespace details
{

inline StringOps::StringType MakeTypeNameFromRawName(const char* rawTypeName)
{
#ifdef ENTROPY_MSVC
    StringOps::StringType ret = rawTypeName;
    StringOps::Replace(ret, "struct ", "");
    StringOps::Replace(ret, "class ", "");
    StringOps::Replace(ret, "enum ", "");
    return ret;
#else
    int status;
    char* realName = abi::__cxa_demangle(rawTypeName, 0, 0, &status);

    StringOps::StringType ret = realName;
    free(realName);
    StringOps::Replace(ret, ", ", ",");
    StringOps::Replace(ret, "::__2::", "::");
    return ret;
#endif
}

inline StringOps::StringType MakeTypeNameNoTemplateParamsFromRawName(const char* rawTypeName)
{
    auto ret = MakeTypeNameFromRawName(rawTypeName);

    int first = StringOps::FindFirst(ret, '<');
    if (first >= 0)
    {
        StringOps::SubString(ret, 0, first);
    }

    return ret;
}

inline TypeId MakeTypeIdFromTypeName(const StringOps::StringType& typeName)
{
    return TypeId(StringHash(StringOps::GetStr(typeName), StringOps::GetLength(typeName)));
}

} // namespace details

} // namespace Entropy
