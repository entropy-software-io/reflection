// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#include "Entropy/Core/Log.h"
#include "Entropy/Reflection.h"
#include "TestMacros.h"

namespace MyNamespace
{

const float& MyFunction(const Entropy::int64& x, double y);

struct MyType
{
    const char* MyMethod(const Entropy::int64& x, MyType* y);
    const char* MyConstMethod(const Entropy::int64& x, MyType* y) const;
};

template <typename T1, typename T2>
struct MyTemplate
{
};

} // namespace MyNamespace

//---------------

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
    using namespace MyNamespace;

    ENTROPY_VERIFY_FUNC(CheckTypeName<bool>("bool"));
    ENTROPY_VERIFY_FUNC(CheckTypeName<char>("char"));
    ENTROPY_VERIFY_FUNC(CheckTypeName<unsigned char>("unsigned char"));
    ENTROPY_VERIFY_FUNC(CheckTypeName<short>("short"));
    ENTROPY_VERIFY_FUNC(CheckTypeName<unsigned short>("unsigned short"));
    ENTROPY_VERIFY_FUNC(CheckTypeName<int>("int"));
    ENTROPY_VERIFY_FUNC(CheckTypeName<unsigned int>("unsigned int"));
    ENTROPY_VERIFY_FUNC(CheckTypeName<long long>("__int64"));
    ENTROPY_VERIFY_FUNC(CheckTypeName<unsigned long long>("unsigned __int64"));
    ENTROPY_VERIFY_FUNC(CheckTypeName<float>("float"));
    ENTROPY_VERIFY_FUNC(CheckTypeName<double>("double"));

    ENTROPY_VERIFY_FUNC(CheckTypeName<const char*>("char const*"));
    ENTROPY_VERIFY_FUNC(CheckTypeName<const char*&>("char const*&"));
    ENTROPY_VERIFY_FUNC(CheckTypeName<int* const>("int* const"));

    ENTROPY_VERIFY_FUNC(CheckTypeName<MyType>("MyNamespace::MyType"));
    ENTROPY_VERIFY_FUNC(CheckTypeName<MyType&>("MyNamespace::MyType&"));

    ENTROPY_VERIFY_FUNC(CheckTypeName<MyTemplate<float&, const MyType*>>(
        "MyNamespace::MyTemplate<float&, MyNamespace::MyType const*>"));

    ENTROPY_VERIFY_FUNC(CheckTypeName<const float[]>("float const[]"));
    ENTROPY_VERIFY_FUNC(CheckTypeName<const float[3]>("float const[3]"));
    ENTROPY_VERIFY_FUNC(CheckTypeName<const float[3][4]>("float const[3][4]"));

    ENTROPY_VERIFY_FUNC(CheckTypeName<void()>("void ()"));

    // For some reason, the compiler is dropping the const on "MyType* const"
    ENTROPY_VERIFY_FUNC(CheckTypeName<const char*(int&, MyType* const)>("char const* (int&, MyNamespace::MyType*)"));

    ENTROPY_VERIFY_FUNC(CheckTypeName<decltype(&MyFunction)>("float const& (*)(__int64 const&, double)"));
    ENTROPY_VERIFY_FUNC(CheckTypeName<decltype(&MyType::MyMethod)>(
        "char const* (MyNamespace::MyType::*)(__int64 const&, MyNamespace::MyType*)"));
    ENTROPY_VERIFY_FUNC(CheckTypeName<decltype(&MyType::MyConstMethod)>(
        "char const* (MyNamespace::MyType::*)(__int64 const&, MyNamespace::MyType*) const"));

    return 0;
}
