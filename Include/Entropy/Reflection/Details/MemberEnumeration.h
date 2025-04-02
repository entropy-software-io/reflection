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

} // namespace details

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
/// Calls into callbackObject for each reflected member in sourceObject.
/// </summary>
/// <param name="callbackObject">
/// Any callable object that can called with the following (templated) signature:
///     (const char* memberName, TMemberType& member)
///   OR
///     (const char* memberName, TMemberType& member, const Entropy::AttributeTypeCollection<TAttrTypes...>&
///     attributes)
/// </param>
template <typename TFunc, bool TIncludeSubclasses, typename TClass>
void ForEachReflectedMember(TClass& sourceObject, TFunc callbackObject)
{
    if ENTROPY_CONSTEXPR (Traits::IsReflectedType_v<TClass>)
    {
        details::UnaryMemberOperation<TClass, TFunc, TIncludeSubclasses, 0>{}(sourceObject, callbackObject);
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

} // namespace details

} // namespace Entropy
