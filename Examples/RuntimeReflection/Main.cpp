// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#include "Entropy/Reflection.h"

struct MyStruct
{
    ENTROPY_REFLECT_CLASS(MyStruct)

    ENTROPY_REFLECT_MEMBER(MyFloatValue)
    float MyFloatValue = 1.23f;
};

int main(int argc, char* argv[])
{
    const Entropy::TypeInfo* typeInfo = Entropy::ReflectTypeAndGetTypeInfo<MyStruct>();

    return 0;
}
