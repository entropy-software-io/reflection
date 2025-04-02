// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#include "Entropy/Reflection.h"
#include <iostream>

struct MyStruct
{
    ENTROPY_REFLECT_CLASS(MyStruct)

    ENTROPY_REFLECT_MEMBER(MyIntVal)
    int MyIntVal = 123;

    ENTROPY_REFLECT_MEMBER(MyFloatVal)
    float MyFloatVal = 456.7f;
};

// Sanity checks
static_assert(Entropy::Traits::IsReflectedType_v<MyStruct>, "MyStruct is not reflected");

struct MyDerivedStruct : public MyStruct
{
    ENTROPY_REFLECT_CLASS_WITH_BASE(MyDerivedStruct, MyStruct)

    ENTROPY_REFLECT_MEMBER(MyStringVal)
    const char* MyStringVal = "String Value";
};

// Sanity checks
static_assert(Entropy::Traits::IsReflectedType_v<MyDerivedStruct>, "MyDerivedStruct is not reflected");
static_assert(Entropy::Traits::HasBaseClass_v<MyDerivedStruct>, "MyDerivedStruct has no defined base class");
static_assert(std::is_same_v<MyStruct, Entropy::Traits::BaseClassOf_t<MyDerivedStruct>>,
              "Incorrect defined base class of MyDerivedStruct");

struct PrintName
{
    template <typename TMember>
    void operator()(const char* memberName, const TMember& memberValue)
    {
        std::cout << "Member: " << memberName << ", Value: " << memberValue << std::endl;
    }
};

int main(int argc, char* argv[])
{
    const MyDerivedStruct myDerivedStruct;

    Entropy::ForEachReflectedMember<true>(myDerivedStruct, PrintName{});

    return 0;
}
