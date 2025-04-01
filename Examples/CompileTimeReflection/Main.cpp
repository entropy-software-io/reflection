// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#include <iostream>
#include "Entropy/Reflection.h"

struct MyStruct
{
    ENTROPY_REFLECT_CLASS(MyStruct)

    ENTROPY_REFLECT_MEMBER(MyIntVal)
    int MyIntVal = 123;
};

int main(int argc, char *argv[])
{
    return 0;
}
