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
        auto& classInfo = typeInfo->Get<ClassTypeInfo>();

        if (!classInfo.IsReflectedClass())
        {
            std::cout << typeInfo->GetTypeName() << " is not a reflected class" << std::endl;
            return;
        }

        const ClassDescription* classDesc = classInfo.GetClassDescription();

        std::cout << "Member list for '" << typeInfo->GetTypeName() << "':" << std::endl;
        for (const auto& memberKvp : classDesc->GetMembers())
        {
            std::cout << "   " << memberKvp.first << " (" << memberKvp.second.GetMemberType()->GetTypeName() << ")"
                      << std::endl;
        }

        const auto& templateParams = classDesc->GetTemplateParameters();
        if (templateParams.size() > 0)
        {
            std::cout << "Template Parameters Types: ";

            for (int i = 0, count = (int)templateParams.size(); i < count; ++i)
            {
                std::cout << templateParams[i]->GetTypeName();
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

        if (classDesc->GetBaseClassTypeInfo())
        {
            std::cout << "Base class: '" << classDesc->GetBaseClassTypeInfo()->GetTypeName() << "'"
                      << std::endl;

            PrintClassInfo(classDesc->GetBaseClassTypeInfo());
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
