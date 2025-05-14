// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

// Everywhere in our code, we would include this header instead of the base reflection directly
#include "CustomReflection.h"

#include <iostream>

struct MyDerivedCustomType : public MyCustomType
{
};

struct ConvertableToMyCustomType
{
    operator MyCustomType() { return MyCustomType(); }
};

void PrintTypeInfo(const Entropy::TypeInfo* typeInfo)
{
    using namespace Entropy;
    using namespace Entropy::Reflection;

    std::cout << "Type Name: " << typeInfo->GetTypeName() << std::endl;
    std::cout << "  Is MyCustomType: " << (typeInfo->Get<CustomTypeInfo>().IsMyCustomType() ? "True" : "False")
              << std::endl;
    std::cout << "  Is Assignable To MyCustomType: "
              << (typeInfo->Get<CustomTypeInfo>().IsAssignableToMyCustomType() ? "True" : "False") << std::endl;
    std::cout << std::endl;
}

int main(int argc, char* argv[])
{
    using namespace Entropy;
    using namespace Entropy::Reflection;

    auto someLambda = [](int x) { return x; };

    PrintTypeInfo(ReflectTypeAndGetTypeInfo<int>());
    PrintTypeInfo(ReflectTypeAndGetTypeInfo<MyCustomType>());
    PrintTypeInfo(ReflectTypeAndGetTypeInfo<MyDerivedCustomType>());
    PrintTypeInfo(ReflectTypeAndGetTypeInfo<ConvertableToMyCustomType>());
    PrintTypeInfo(ReflectTypeAndGetTypeInfo<MyCustomType*>());

    return 0;
}
