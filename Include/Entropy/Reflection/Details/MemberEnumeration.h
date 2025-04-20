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
struct MemberTypeOperation
{
    // void operator()(TFunc callbackObject) const;
};

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
/// Calls into callbackObject for each reflected member type in sourceObject.
/// </summary>
/// <param name="callbackObject">
/// Any callable object that can called with the following (templated) signature:
///     ()
///   OR
///     (const char* memberName)
///   OR
///     (const char* memberName, const Entropy::AttributeTypeCollection<TAttrTypes...>& attributes)
/// </param>
template <bool TIncludeSubclasses, typename TClass, typename TFunc>
void ForEachReflectedMemberType(TFunc callbackObject)
{
    if ENTROPY_CONSTEXPR (Traits::IsReflectedType<TClass>::value)
    {
        details::MemberTypeOperation<TClass, TFunc, TIncludeSubclasses, 0>{}(callbackObject);
    }
}

/// <summary>
/// Calls into callbackObject for each reflected member in sourceObject.
/// </summary>
/// <param name="callbackObject">
/// Any callable object that can called with the following (templated) signature:
///     (const char* memberName, TMemberType& member)
///   OR
///     (const char* memberName, TMemberType& member, const Entropy::AttributeTypeCollection<TAttrTypes...>& attributes)
/// </param>
template <bool TIncludeSubclasses, typename TClass, typename TFunc>
void ForEachReflectedMember(TClass& sourceObject, TFunc callbackObject)
{
    if ENTROPY_CONSTEXPR (Traits::IsReflectedType<TClass>::value)
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
    if ENTROPY_CONSTEXPR (Traits::IsReflectedType<TClassA>::value && Traits::IsReflectedType<TClassB>::value)
    {
        details::BinaryMemberOperation<TClassA, TClassB, TFunc, TIncludeSubclasses, 0>{}(sourceObjectA, sourceObjectB,
                                                                                         callbackObject);
    }
}

namespace details
{

template <typename TMember, typename TFunc, typename... TMemberAttrs>
inline typename std::enable_if<
    Traits::IsClassMethodInvocable<TFunc, decltype(&TFunc::template operator()<TMember>), const char*,
                                   const AttributeTypeCollection<TMemberAttrs...>&>::value>::
    type InvokeMemberTypeFunction(const ReflectionMemberMetaData<TMemberAttrs...>& metaData, TFunc callbackObj)
{
    callbackObj.template operator()<TMember>(metaData.memberName, metaData.attributes);
}

template <typename TMember, typename TFunc, typename... TMemberAttrs>
inline typename std::enable_if<
    Traits::IsClassMethodInvocable<TFunc, decltype(&TFunc::template operator()<TMember>), const char*>::value>::
    type InvokeMemberTypeFunction(const ReflectionMemberMetaData<TMemberAttrs...>& metaData, TFunc callbackObj)
{
    callbackObj(metaData.memberName);
}

template <typename TMember, typename TFunc, typename... TMemberAttrs>
inline typename std::enable_if<
    Traits::IsClassMethodInvocable<TFunc, decltype(&TFunc::template operator()<TMember>)>::value>::
    type InvokeMemberTypeFunction(const ReflectionMemberMetaData<TMemberAttrs...>& metaData, TFunc callbackObj)
{
    callbackObj();
}

// There is a reflected member for this counter
template <typename TClass, typename TFunc, bool TIncludeSubclasses, int TCounter>
struct MemberTypeOperation<TClass, TFunc, TIncludeSubclasses, TCounter,
                           typename std::enable_if<Traits::RemoveConstRef_t<
                               TClass>::template __MemberTypeOperatorExists<TCounter>::value>::type>
{
    inline void operator()(TFunc callbackObject) const
    {
        Traits::RemoveConstRef_t<TClass>::template __MemberTypeOperator<TClass, TFunc, TCounter>::Execute(
            callbackObject);
        MemberTypeOperation<TClass, TFunc, TIncludeSubclasses, TCounter + 1>{}(callbackObject);
    }
};

// There are no more reflected members, but we have a base class and want to enumerate them
template <typename TClass, typename TFunc, int TCounter>
struct MemberTypeOperation<
    TClass, TFunc, true /* TIncludeSubclasses */, TCounter,
    typename std::enable_if<!Traits::RemoveConstRef_t<TClass>::template __MemberTypeOperatorExists<TCounter>::value &&
                            Traits::HasBaseClass<TClass>::value>::type>
{
    inline void operator()(TFunc callbackObject) const
    {
        MemberTypeOperation<Traits::BaseClassWithQualifiersOf_t<TClass>, TFunc, true /* TIncludeSubclasses */, 0>{}(
            callbackObject);
    }
};

// There are no more reflected members and we either don't want to enumerate base classes or we don't have one.
template <typename TClass, typename TFunc, bool TIncludeSubclasses, int TCounter>
struct MemberTypeOperation<
    TClass, TFunc, TIncludeSubclasses, TCounter,
    typename std::enable_if<!Traits::RemoveConstRef_t<TClass>::template __MemberTypeOperatorExists<TCounter>::value &&
                            (!TIncludeSubclasses || !Traits::HasBaseClass<TClass>::value)>::type>
{
    inline void operator()(TFunc callbackObject) const {}
};

//==================

template <typename TMember, typename TFunc, typename... TMemberAttrs>
inline typename std::enable_if<
    Traits::IsInvocable<TFunc, const char*, TMember, const AttributeTypeCollection<TMemberAttrs...>&>::value>::type
InvokeUnaryMemberFunction(const ReflectionMemberMetaData<TMemberAttrs...>& metaData, TMember& member, TFunc callbackObj)
{
    callbackObj(metaData.memberName, member, metaData.attributes);
}

template <typename TMember, typename TFunc, typename... TMemberAttrs>
inline typename std::enable_if<Traits::IsInvocable<TFunc, const char*, TMember>::value>::type InvokeUnaryMemberFunction(
    const ReflectionMemberMetaData<TMemberAttrs...>& metaData, TMember& member, TFunc callbackObj)
{
    callbackObj(metaData.memberName, member);
}

template <typename TMember, typename TFunc, typename... TMemberAttrs>
inline typename std::enable_if<Traits::IsInvocable<TFunc, TMember>::value>::type InvokeUnaryMemberFunction(
    const ReflectionMemberMetaData<TMemberAttrs...>& metaData, TMember& member, TFunc callbackObj)
{
    callbackObj(member);
}

// There is a reflected member for this counter
template <typename TClass, typename TFunc, bool TIncludeSubclasses, int TCounter>
struct UnaryMemberOperation<TClass, TFunc, TIncludeSubclasses, TCounter,
                            typename std::enable_if<Traits::RemoveConstRef_t<
                                TClass>::template __UnaryMemberOperatorExists<TCounter>::value>::type>
{
    inline void operator()(TClass& sourceObject, TFunc callbackObject) const
    {
        Traits::RemoveConstRef_t<TClass>::template __UnaryMemberOperator<TClass, TFunc, TCounter>::Execute(
            sourceObject, callbackObject);
        UnaryMemberOperation<TClass, TFunc, TIncludeSubclasses, TCounter + 1>{}(sourceObject, callbackObject);
    }
};

// There are no more reflected members, but we have a base class and want to enumerate them
template <typename TClass, typename TFunc, int TCounter>
struct UnaryMemberOperation<
    TClass, TFunc, true /* TIncludeSubclasses */, TCounter,
    typename std::enable_if<!Traits::RemoveConstRef_t<TClass>::template __UnaryMemberOperatorExists<TCounter>::value &&
                            Traits::HasBaseClass<TClass>::value>::type>
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
    typename std::enable_if<!Traits::RemoveConstRef_t<TClass>::template __UnaryMemberOperatorExists<TCounter>::value &&
                            (!TIncludeSubclasses || !Traits::HasBaseClass<TClass>::value)>::type>
{
    inline void operator()(TClass& sourceObject, TFunc callbackObject) const {}
};

//==================

template <typename TMemberA, typename TMemberB, typename TFunc>
inline typename std::enable_if<Traits::IsInvocable<TFunc, const char*, TMemberA, TMemberB>::value>::type
InvokeBinaryMemberFunction(const char* memberName, TMemberA& memberA, TMemberB& memberB, TFunc callbackObj)
{
    callbackObj(memberName, memberA, memberB);
}

// There is a reflected member for this counter
template <typename TClassA, typename TClassB, typename TFunc, bool TIncludeSubclasses, int TCounter>
struct BinaryMemberOperation<TClassA, TClassB, TFunc, TIncludeSubclasses, TCounter,
                             typename std::enable_if<Traits::RemoveConstRef_t<
                                 TClassA>::template __BinaryMemberOperatorExists<TCounter>::value>::type>
{
    inline void operator()(TClassA& sourceObjectA, TClassB& sourceObjectB, TFunc callbackObject) const
    {
        Traits::RemoveConstRef_t<TClassA>::template __BinaryMemberOperator<TClassA, TClassB, TFunc, TCounter>::Execute(
            sourceObjectA, sourceObjectB, callbackObject);
        BinaryMemberOperation<TClassA, TClassB, TFunc, TIncludeSubclasses, TCounter + 1>{}(sourceObjectA, sourceObjectB,
                                                                                           callbackObject);
    }
};

// There are no more reflected members, but we have a base class and want to enumerate them
template <typename TClassA, typename TClassB, typename TFunc, bool TIncludeSubclasses, int TCounter>
struct BinaryMemberOperation<
    TClassA, TClassB, TFunc, TIncludeSubclasses, TCounter,
    typename std::enable_if<
        !Traits::RemoveConstRef_t<TClassA>::template __BinaryMemberOperatorExists<TCounter>::value &&
        Traits::HasBaseClass<TClassA>::value>::type>
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
    typename std::enable_if<
        !Traits::RemoveConstRef_t<TClassA>::template __BinaryMemberOperatorExists<TCounter>::value &&
        (!TIncludeSubclasses || !Traits::HasBaseClass<TClassA>::value)>::type>
{
    inline void operator()(TClassA& sourceObjectA, TClassB& sourceObjectB, TFunc callbackObject) const {}
};

} // namespace details

} // namespace Entropy
