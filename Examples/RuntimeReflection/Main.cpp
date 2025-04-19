// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#include "Entropy/Reflection.h"

#include <iostream>

struct MyStruct
{
    ENTROPY_REFLECT_CLASS(MyStruct)

    ENTROPY_REFLECT_MEMBER(MyIntValue)
    int MyIntValue = 1;

    ENTROPY_REFLECT_MEMBER(MyFloatValue)
    float MyFloatValue = 1.23f;
};

int main(int argc, char* argv[])
{
    using namespace Entropy;
    using namespace Entropy::Reflection;

    const TypeInfo* typeInfo = ReflectTypeAndGetTypeInfo<MyStruct>();

    std::cout << "Member list for '" << typeInfo->Get<BasicTypeInfo>().GetTypeName() << "':" << std::endl;
    for (const auto& memberKvp : typeInfo->Get<ClassTypeInfo>().GetMembers())
    {
        std::cout << "   " << memberKvp.first << " ("
                  << memberKvp.second.GetMemberType()->Get<BasicTypeInfo>().GetTypeName() << ")" << std::endl;
    }

    return 0;
}
