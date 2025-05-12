// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#include "Entropy/Core/Log.h"
#include "Entropy/Reflection.h"
#include "TestMacros.h"

namespace Entropy
{
namespace Tests
{
namespace TypeInfo
{

struct MyCastTestStruct
{
};

struct MyDerivedCastTestStruct : public MyCastTestStruct
{
};

template <typename TFrom, typename TTo>
bool CheckCanCastTo()
{
    const Entropy::TypeInfo* fromTypeInfo = Entropy::ReflectTypeAndGetTypeInfo<TFrom>();
    ENTROPY_CHECK_RETURN_VAL_FUNC(fromTypeInfo, 1, "Failed to get TFrom type info");

    const Entropy::TypeInfo* toTypeInfo = Entropy::ReflectTypeAndGetTypeInfo<TTo>();
    ENTROPY_CHECK_RETURN_VAL_FUNC(fromTypeInfo, 1, "Failed to get TTo type info");

    return fromTypeInfo->CanCastTo(toTypeInfo);
}

} // namespace TypeInfo
} // namespace Tests
} // namespace Entropy

int TypeInfo_TestCanCastTo(int argc, char** const argv)
{
    using namespace Entropy::Tests::TypeInfo;

    ENTROPY_VERIFY_FUNC(CheckCanCastTo<int, int>());

    // Right now, we don't allow changing pointer counts, but potentially could allow one level of change because
    // DataObject holds on to a void*.
    ENTROPY_VERIFY_NOT_FUNC(CheckCanCastTo<int, int*>());
    ENTROPY_VERIFY_NOT_FUNC(CheckCanCastTo<int*, int**>());
    ENTROPY_VERIFY_NOT_FUNC(CheckCanCastTo<int*, int>());
    ENTROPY_VERIFY_NOT_FUNC(CheckCanCastTo<int**, int*>());

    // Requires static_cast, which we don't allow
    ENTROPY_VERIFY_NOT_FUNC(CheckCanCastTo<int, float>());

    ENTROPY_VERIFY_FUNC(CheckCanCastTo<float, const float>());
    ENTROPY_VERIFY_FUNC(CheckCanCastTo<float*, const float*>());

    // Can't remove const
    ENTROPY_VERIFY_NOT_FUNC(CheckCanCastTo<const float*, float*>());

    // Reference doesn't matter
    ENTROPY_VERIFY_FUNC(CheckCanCastTo<int&, int>());
    ENTROPY_VERIFY_FUNC(CheckCanCastTo<int, int&>());
    ENTROPY_VERIFY_NOT_FUNC(CheckCanCastTo<const int&, int>());
    ENTROPY_VERIFY_FUNC(CheckCanCastTo<int**&, const int**>());

    ENTROPY_VERIFY_FUNC(CheckCanCastTo<MyCastTestStruct, MyCastTestStruct>());
    ENTROPY_VERIFY_FUNC(CheckCanCastTo<MyCastTestStruct, const MyCastTestStruct&>());
    ENTROPY_VERIFY_NOT_FUNC(CheckCanCastTo<const MyCastTestStruct&, MyCastTestStruct>());

    // Don't allow conversion to base classes. Possible in the future if both are reflected.
    ENTROPY_VERIFY_NOT_FUNC(CheckCanCastTo<MyDerivedCastTestStruct, MyCastTestStruct>());

    // Downcasting not supported. Whatever concrete type was allocated will be lost when stuffed into a DataObject of a
    // base type. TypeInfo is static and not unique per allocation.
    ENTROPY_VERIFY_NOT_FUNC(CheckCanCastTo<MyCastTestStruct, MyDerivedCastTestStruct>());

    // Array to pointer conversion is allowed
    ENTROPY_VERIFY_FUNC(CheckCanCastTo<const char[4], const char*>());

    return 0;
}
