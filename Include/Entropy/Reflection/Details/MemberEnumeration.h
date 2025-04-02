// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

#include "Entropy/Reflection/Details/AttributeCollection.h"
#include "Entropy/Reflection/Details/TypeTraits.h"

namespace Entropy
{

namespace details
{

template <typename TClass, typename TFunc, bool TIncludeSubclasses, int TCounter, typename = void>
struct UnaryMemberOperation
{
    // void operator()(TClass& sourceObject, TFunc callbackObject) const;
};

template <typename TClassA, typename TClassB, typename TFunc, bool TIncludeSubclasses, int TCounter, typename = void>
struct BinaryMemberOperation
{
    // void operator()(TClassA& objectA, TClassB& objectB, TFunc callbackObject) const;
};

} // namespace details

//=========================

/// <summary>
/// Calls into callbackObject for each reflected member in sourceObject.
/// </summary>
/// <param name="callbackObject">
/// Any callable object that can called with the following (templated) signature:
///     (const char* memberName, TMemberType& member)
///   OR
///     (const char* memberName, TMemberType& member, const Entropy::AttributeTypeCollection<TAttrTypes...>&
///     attributes)
/// </param>
template <bool TIncludeSubclasses, typename TClass, typename TFunc>
void ForEachReflectedMember(TClass& sourceObject, TFunc callbackObject)
{
    if ENTROPY_CONSTEXPR (Traits::IsReflectedType_v<TClass>)
    {
        details::UnaryMemberOperation<TClass, TFunc, TIncludeSubclasses, 0>{}(sourceObject, callbackObject);
    }
}

/// <summary>
/// Calls into callbackObject for each reflected member that exists in both sourceObjectA and sourceObjectB.
/// This can be used to transform classes. E.g. if you need to transform between an internal representation of a struct
/// to one that can be passed to external users. The members do not need to be the same type; they just need to have the
/// same name.
/// </summary>
/// <param name="callbackObject">
/// Any callable object that can called with the following (templated) signature:
///     (const char* memberName, TMemberTypeA& memberA, TMemberTypeB& memberB)
/// </param>
template <bool TIncludeSubclasses, typename TFunc, typename TClassA, typename TClassB>
void ForEachReflectedMemberInBoth(TClassA& sourceObjectA, TClassB& sourceObjectB, TFunc callbackObject)
{
    if ENTROPY_CONSTEXPR (Traits::IsReflectedType_v<TClassA> && Traits::IsReflectedType_v<TClassB>)
    {
        details::BinaryMemberOperation<TClassA, TClassB, TFunc, TIncludeSubclasses, 0>{}(sourceObjectA, sourceObjectB,
                                                                                         callbackObject);
    }
}

namespace details
{

template <typename TMember, typename TFunc, typename... TMemberAttrs>
inline std::enable_if_t<
    Traits::IsInvocable_v<TFunc, const char*, TMember, const AttributeTypeCollection<TMemberAttrs...>&>>
InvokeUnaryMemberFunction(const ReflectionMemberMetaData<TMemberAttrs...>& metaData, TMember& member, TFunc callbackObj)
{
    callbackObj(metaData.memberName, member, metaData.attributes);
}

template <typename TMember, typename TFunc, typename... TMemberAttrs>
inline std::enable_if_t<Traits::IsInvocable_v<TFunc, const char*, TMember>> InvokeUnaryMemberFunction(
    const ReflectionMemberMetaData<TMemberAttrs...>& metaData, TMember& member, TFunc callbackObj)
{
    callbackObj(metaData.memberName, member);
}

template <typename TMember, typename TFunc, typename... TMemberAttrs>
inline std::enable_if_t<Traits::IsInvocable_v<TFunc, TMember>> InvokeUnaryMemberFunction(
    const ReflectionMemberMetaData<TMemberAttrs...>& metaData, TMember& member, TFunc callbackObj)
{
    callbackObj(member);
}

// There is a reflected member for this counter
template <typename TClass, typename TFunc, bool TIncludeSubclasses, int TCounter>
struct UnaryMemberOperation<
    TClass, TFunc, TIncludeSubclasses, TCounter,
    std::enable_if_t<ENTROPY_REMOVE_CONST_REF(TClass)::template __UnaryMemberOperatorExists<TCounter>::value>>
{
    inline void operator()(TClass& sourceObject, TFunc callbackObject) const
    {
        ENTROPY_REMOVE_CONST_REF(TClass)::template __UnaryMemberOperator<TClass, TFunc, TCounter>::Execute(
            sourceObject, callbackObject);
        UnaryMemberOperation<TClass, TFunc, TIncludeSubclasses, TCounter + 1>{}(sourceObject, callbackObject);
    }
};

// There are no more reflected members, but we have a base class and want to enumerate them
template <typename TClass, typename TFunc, int TCounter>
struct UnaryMemberOperation<
    TClass, TFunc, true /* TIncludeSubclasses */, TCounter,
    std::enable_if_t<!ENTROPY_REMOVE_CONST_REF(TClass)::template __UnaryMemberOperatorExists<TCounter>::value &&
                     Traits::HasBaseClass_v<TClass>>>
{
    inline void operator()(TClass& sourceObject, TFunc callbackObject) const
    {
        UnaryMemberOperation<Traits::BaseClassWithQualifiersOf_t<TClass>, TFunc, true /* TIncludeSubclasses */, 0>{}(
            sourceObject, callbackObject);
    }
};

// There are no more reflected members and we either don't want to enumerate base classes or we don't have one.
template <typename TClass, typename TFunc, bool TIncludeSubclasses, int TCounter>
struct UnaryMemberOperation<
    TClass, TFunc, TIncludeSubclasses, TCounter,
    std::enable_if_t<!ENTROPY_REMOVE_CONST_REF(TClass)::template __UnaryMemberOperatorExists<TCounter>::value &&
                     (!TIncludeSubclasses || !Traits::HasBaseClass_v<TClass>)>>
{
    inline void operator()(TClass& sourceObject, TFunc callbackObject) const {}
};

//==================

template <typename TMemberA, typename TMemberB, typename TFunc>
inline std::enable_if_t<Traits::IsInvocable_v<TFunc, const char*, TMemberA, TMemberB>> InvokeBinaryMemberFunction(
    const char* memberName, TMemberA& memberA, TMemberB& memberB, TFunc callbackObj)
{
    callbackObj(memberName, memberA, memberB);
}

// There is a reflected member for this counter
template <typename TClassA, typename TClassB, typename TFunc, bool TIncludeSubclasses, int TCounter>
struct BinaryMemberOperation<
    TClassA, TClassB, TFunc, TIncludeSubclasses, TCounter,
    std::enable_if_t<ENTROPY_REMOVE_CONST_REF(TClassA)::template __BinaryMemberOperatorExists<TCounter>::value>>
{
    inline void operator()(TClassA& sourceObjectA, TClassB& sourceObjectB, TFunc callbackObject) const
    {
        ENTROPY_REMOVE_CONST_REF(TClassA)::template __BinaryMemberOperator<TClassA, TClassB, TFunc, TCounter>::Execute(
            sourceObjectA, sourceObjectB, callbackObject);
        BinaryMemberOperation<TClassA, TClassB, TFunc, TIncludeSubclasses, TCounter + 1>{}(sourceObjectA, sourceObjectB,
                                                                                           callbackObject);
    }
};

// There are no more reflected members, but we have a base class and want to enumerate them
template <typename TClassA, typename TClassB, typename TFunc, bool TIncludeSubclasses, int TCounter>
struct BinaryMemberOperation<
    TClassA, TClassB, TFunc, TIncludeSubclasses, TCounter,
    std::enable_if_t<!ENTROPY_REMOVE_CONST_REF(TClassA)::template __BinaryMemberOperatorExists<TCounter>::value &&
                     Traits::HasBaseClass_v<TClassA>>>
{
    inline void operator()(TClassA& sourceObjectA, TClassB& sourceObjectB, TFunc callbackObject) const
    {
        BinaryMemberOperation<Traits::BaseClassWithQualifiersOf_t<TClassA>, TClassB, TFunc,
                              true /* TIncludeSubclasses */, 0>{}(sourceObjectA, sourceObjectB, callbackObject);
    }
};

// There are no more reflected members and we either don't want to enumerate base classes or we don't have one.
template <typename TClassA, typename TClassB, typename TFunc, bool TIncludeSubclasses, int TCounter>
struct BinaryMemberOperation<
    TClassA, TClassB, TFunc, TIncludeSubclasses, TCounter,
    std::enable_if_t<!ENTROPY_REMOVE_CONST_REF(TClassA)::template __BinaryMemberOperatorExists<TCounter>::value &&
                     (!TIncludeSubclasses || !Traits::HasBaseClass_v<TClassA>)>>
{
    inline void operator()(TClassA& sourceObjectA, TClassB& sourceObjectB, TFunc callbackObject) const {}
};

} // namespace details

} // namespace Entropy
