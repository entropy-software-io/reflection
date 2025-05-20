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

static_assert(Entropy::Traits::IsAllocatorConstructible<MyStruct>::value);
static_assert(Entropy::Traits::IsAllocatorDestructible<MyStruct>::value);

const Entropy::TypeInfo* GetTypeInfo() { return Entropy::ReflectTypeAndGetTypeInfo<MyStruct>(); }

int main(int argc, char* argv[])
{
    using namespace Entropy;
    using namespace Entropy::Reflection;

    // Construct an instance of an object from some type info we are given
    auto typeInfo = GetTypeInfo();

    if (!typeInfo->CanConstruct())
    {
        std::cerr << "Type is not constructible" << std::endl;
        return 1;
    }

    DataObject dataObj = typeInfo->Construct();
    if (dataObj == nullptr)
    {
        std::cerr << "Failed to construct object" << std::endl;
        return 1;
    }

    if (!dataObj.IsExactType<MyStruct>())
    {
        std::cerr << "Data object is not the type we expected" << std::endl;
        return 1;
    }

    // This is now safe to do
    MyStruct& obj = dataObj.GetData<MyStruct>();

    std::cout << "Value of data: " << obj.MyFloatValue << std::endl;

    return 0;
}
