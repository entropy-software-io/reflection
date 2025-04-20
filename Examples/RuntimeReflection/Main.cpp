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

template <typename T1, typename T2>
struct MyTemplateStruct
{
    ENTROPY_REFLECT_CLASS(MyTemplateStruct)

    ENTROPY_REFLECT_MEMBER(MyIntValue)
    int MyIntValue = 1;
};

// Reflecting this type is interesting because the template parameters reference the type we are reflecting, so we end
// up with a re-entrant call. During the processing of the template parameters, MyRecursiveTemplateStruct's type info
// will only be partially initialized, and will be passed itself as the type info pointer.
struct MyRecursiveTemplateStruct : public MyTemplateStruct<MyRecursiveTemplateStruct, MyRecursiveTemplateStruct>
{
    ENTROPY_REFLECT_CLASS_WITH_BASE(
        MyRecursiveTemplateStruct,
        ENTROPY_LITERAL(MyTemplateStruct<MyRecursiveTemplateStruct, MyRecursiveTemplateStruct>))

    ENTROPY_REFLECT_MEMBER(MyIntValue)
    int MyIntValue = 1;
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

        const auto& templateParams = classInfo.GetTemplateParameters();
        if (templateParams.size() > 0)
        {
            std::cout << "Template Parameters Types: ";

            for (int i = 0, count = (int)templateParams.size(); i < count; ++i)
            {
                std::cout << templateParams[i]->Get<BasicTypeInfo>().GetTypeName();
                if (i < templateParams.size() - 1)
                {
                    std::cout << ", ";
                }
                else
                {
                    std::cout << std::endl;
                }
            }
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

    {
        const TypeInfo* typeInfo = ReflectTypeAndGetTypeInfo<MyStruct>();
        PrintClassInfo(typeInfo);
    }

    std::cout << std::endl;

    {
        const TypeInfo* typeInfo = ReflectTypeAndGetTypeInfo<MyTemplateStruct<double, char>>();
        PrintClassInfo(typeInfo);
    }

    std::cout << std::endl;

    {
        const TypeInfo* typeInfo = ReflectTypeAndGetTypeInfo<MyRecursiveTemplateStruct>();
        PrintClassInfo(typeInfo);
    }

    return 0;
}
