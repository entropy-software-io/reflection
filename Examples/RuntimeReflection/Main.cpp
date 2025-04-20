// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#include "Entropy/Reflection.h"

#include <iostream>

struct MyBaseStruct
{
    ENTROPY_REFLECT_CLASS(MyBaseStruct)

    ENTROPY_REFLECT_MEMBER(MyBaseIntValue)
    int MyBaseIntValue = 1;

    ENTROPY_REFLECT_MEMBER(MyBaseFloatValue)
    float MyBaseFloatValue = 1.23f;
};

struct MyStruct : public MyBaseStruct
{
    ENTROPY_REFLECT_CLASS_WITH_BASE(MyStruct, MyBaseStruct)

    ENTROPY_REFLECT_MEMBER(MyIntValue)
    int MyIntValue = 1;

    ENTROPY_REFLECT_MEMBER(MyFloatValue)
    float MyFloatValue = 1.23f;
};

void PrintClassInfo(const Entropy::TypeInfo* typeInfo)
{
    using namespace Entropy;
    using namespace Entropy::Reflection;

    if (typeInfo)
    {
        auto& basicInfo = typeInfo->Get<BasicTypeInfo>();
        auto& classInfo = typeInfo->Get<ClassTypeInfo>();

        std::cout << "Member list for '" << basicInfo.GetTypeName() << "':" << std::endl;
        for (const auto& memberKvp : classInfo.GetMembers())
        {
            std::cout << "   " << memberKvp.first << " ("
                      << memberKvp.second.GetMemberType()->Get<BasicTypeInfo>().GetTypeName() << ")" << std::endl;
        }

        if (classInfo.GetBaseClassTypeInfo())
        {
            std::cout << "Base class: '" << classInfo.GetBaseClassTypeInfo()->Get<BasicTypeInfo>().GetTypeName() << "'"
                      << std::endl;

            PrintClassInfo(classInfo.GetBaseClassTypeInfo());
        }
    }
}

int main(int argc, char* argv[])
{
    using namespace Entropy;
    using namespace Entropy::Reflection;

    const TypeInfo* typeInfo = ReflectTypeAndGetTypeInfo<MyStruct>();
    PrintClassInfo(typeInfo);

    return 0;
}
