// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#include "Entropy/Core/Log.h"
#include "Entropy/Reflection.h"
#include "TestMacros.h"

template <typename T>
bool CheckTypeName(const std::string& expectedValue)
{
    using namespace Entropy;

    auto traitsTypeName = Traits::TypeNameOf<T>{}();
    ENTROPY_CHECK_RETURN_VAL_FUNC(std::strcmp(expectedValue.c_str(), StringOps::GetStr(traitsTypeName)) == 0, false,
                                  "TypeInfo name does not match expected value. [Expected: "
                                      << expectedValue << "] [TypeInfo Name: " << traitsTypeName << "]");

    const TypeInfo* typeInfo = ReflectTypeAndGetTypeInfo<T>();
    ENTROPY_CHECK_RETURN_VAL_FUNC(typeInfo, false, "Failed to get type info for '" << Traits::TypeNameOf<T>{}() << "'");

    ENTROPY_CHECK_RETURN_VAL_FUNC(std::strcmp(expectedValue.c_str(), StringOps::GetStr(typeInfo->GetTypeName())) == 0,
                                  false,
                                  "TypeInfo name does not match expected value. [Expected: "
                                      << expectedValue << "] [TypeInfo Name: " << typeInfo->GetTypeName() << "]");

    return true;
}

int TypeInfo_TestTypeName(int argc, char** const argv)
{
    ENTROPY_VERIFY_FUNC(CheckTypeName<bool>("bool"));
    ENTROPY_VERIFY_FUNC(CheckTypeName<char>("char"));
    ENTROPY_VERIFY_FUNC(CheckTypeName<unsigned char>("unsigned char"));
    ENTROPY_VERIFY_FUNC(CheckTypeName<short>("short"));
    ENTROPY_VERIFY_FUNC(CheckTypeName<unsigned short>("unsigned short"));
    ENTROPY_VERIFY_FUNC(CheckTypeName<int>("int"));
    ENTROPY_VERIFY_FUNC(CheckTypeName<unsigned int>("unsigned int"));
    ENTROPY_VERIFY_FUNC(CheckTypeName<long long>("__int64"));
    ENTROPY_VERIFY_FUNC(CheckTypeName<unsigned long long>("unsigned __int64"));

    ENTROPY_VERIFY_FUNC(CheckTypeName<const char*>("char const*"));
    ENTROPY_VERIFY_FUNC(CheckTypeName<const char*&>("char const*&"));

    return 0;
}
