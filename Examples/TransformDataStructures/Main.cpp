// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#include "Entropy/Reflection.h"
#include <iostream>

struct Vector3
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

struct Array3
{
    float values[3] = {0};
};

struct MyInternalStruct
{
    ENTROPY_REFLECT_CLASS(MyInternalStruct)

    ENTROPY_REFLECT_MEMBER(MyFloatValue)
    float MyFloatValue = 1.23f;

    ENTROPY_REFLECT_MEMBER(PositionValue)
    Vector3 PositionValue;

    ENTROPY_REFLECT_MEMBER(MyInternalOnlyInt)
    int MyInternalOnlyInt = 123;
};

struct MyExternalStruct
{
    ENTROPY_REFLECT_CLASS(MyExternalStruct)

    ENTROPY_REFLECT_MEMBER(MyFloatValue)
    float MyFloatValue;

    ENTROPY_REFLECT_MEMBER(PositionValue)
    Array3 PositionValue;

    ENTROPY_REFLECT_MEMBER(MyExternalOnlyInt)
    int MyExternalOnlyInt;
};

struct Conversion
{
    template <typename TMemberA, typename TMemberB>
    void operator()(const char* name, const TMemberA& src, TMemberB& dest)
    {
        std::cout << "Copying member [Name: " << name << "] [Value: " << src << "]" << std::endl;
        dest = src;
    }

    template <>
    void operator()(const char* name, const Vector3& src, Array3& dest)
    {
        std::cout << "Copying Vector3 value [Name: " << name << "] [Value: " << src.x << ", " << src.y << ", " << src.z
                  << "]" << std::endl;
        dest.values[0] = src.x;
        dest.values[1] = src.y;
        dest.values[2] = src.z;
    }
};

int main(int argc, char* argv[])
{
    MyInternalStruct internalStruct{};
    internalStruct.PositionValue.x = 1.0f;
    internalStruct.PositionValue.y = 2.0f;
    internalStruct.PositionValue.z = 3.0f;

    MyExternalStruct externalStruct{};

    Entropy::ForEachReflectedMemberInBoth<true>(internalStruct, externalStruct, Conversion{});

    return 0;
}
