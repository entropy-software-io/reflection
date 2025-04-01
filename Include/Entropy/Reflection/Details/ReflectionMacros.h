// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

#define ENTROPY_DECLARE_SINGLE_EXECUTION_META_FUNCTION()                                                               \
    template <int _TCounter, typename _TClass, typename _TDummy = void>                                                \
    struct SingleExecutionMetaFunctionExists : std::bool_constant<false>                                               \
    {                                                                                                                  \
    };                                                                                                                 \
    template <int _TCounter, typename _TClass, typename _TDummy = void>                                                \
    struct SingleExecutionMetaFunction                                                                                 \
    {                                                                                                                  \
        static void Execute() {}                                                                                       \
    };

#define ENTROPY_SINGLE_EXECUTION_META_FUNCTION(...)                                                                    \
    template <typename _TDummy>                                                                                        \
    struct SingleExecutionMetaFunctionExists<ENTROPY_GET_COUNTER_VALUE(), ThisReflectedType, _TDummy>                  \
        : std::bool_constant<true>                                                                                     \
    {                                                                                                                  \
    };                                                                                                                 \
    template <typename _TDummy>                                                                                        \
    struct SingleExecutionMetaFunction<ENTROPY_GET_COUNTER_VALUE(), ThisReflectedType, _TDummy>                        \
    {                                                                                                                  \
        static void Execute() { __VA_ARGS__ }                                                                          \
    };

#define ENTROPY_DECLARE_UNARY_MEMBER_OPERATOR_FUNCTION                                                                 \
    template <int TCounter, typename TDummy = void>                                                                    \
    struct __UnaryMemberOperatorExists : std::bool_constant<false>                                                     \
    {                                                                                                                  \
    };                                                                                                                 \
    template <typename TThisType, typename TFunc, int TCounter>                                                        \
    struct __UnaryMemberOperator                                                                                       \
    {                                                                                                                  \
        static void Execute(TThisType&& src, TFunc&& callbackObj) {}                                                   \
    };

#define ENTROPY_UNARY_MEMBER_OPERATOR_FUNCTION(...)                                                                    \
    template <typename TDummy>                                                                                         \
    struct __UnaryMemberOperatorExists<ENTROPY_GET_COUNTER_VALUE(), TDummy> : std::bool_constant<true>                 \
    {                                                                                                                  \
    };                                                                                                                 \
    template <typename TThisType, typename TFunc>                                                                      \
    struct __UnaryMemberOperator<TThisType, TFunc, ENTROPY_GET_COUNTER_VALUE()>                                        \
    {                                                                                                                  \
        static void Execute(TThisType&& src, TFunc&& callbackObj) { __VA_ARGS__ }                                      \
    };

#define ENTROPY_DECLARE_BINARY_MEMBER_OPERATOR_FUNCTION                                                                \
    template <int TCounter, typename TDummy = void>                                                                    \
    struct BinaryMemberOperatorExists : std::bool_constant<false>                                                      \
    {                                                                                                                  \
    };                                                                                                                 \
    template <typename TOtherClass, template <typename, typename, typename...> class TFunc, int TCounter,              \
              typename... TAdditionalArgs>                                                                             \
    struct BinaryMemberOperator                                                                                        \
    {                                                                                                                  \
        static void Execute(const ThisReflectedType& src, TOtherClass& dest, TAdditionalArgs&&... additionalArgs) {}   \
    };

#define ENTROPY_BINARY_MEMBER_OPERATOR_FUNCTION(...)                                                                   \
    template <typename TDummy>                                                                                         \
    struct BinaryMemberOperatorExists<ENTROPY_GET_COUNTER_VALUE(), TDummy> : std::bool_constant<true>                  \
    {                                                                                                                  \
    };                                                                                                                 \
    template <typename TOtherClass, template <typename, typename, typename...> class TFunc,                            \
              typename... TAdditionalArgs>                                                                             \
    struct BinaryMemberOperator<TOtherClass, TFunc, ENTROPY_GET_COUNTER_VALUE(), TAdditionalArgs...>                   \
    {                                                                                                                  \
        static void Execute(const ThisReflectedType& src, TOtherClass& dest, TAdditionalArgs&&... additionalArgs)      \
        {                                                                                                              \
            __VA_ARGS__                                                                                                \
        }                                                                                                              \
    };

#define ENTROPY_DECLARE_MEMBER_OFFSET_OF_FUNCTION                                                                      \
    template <int TCounter, typename TDummy = void>                                                                    \
    struct MemberOffsetOfExists : std::bool_constant<false>                                                            \
    {                                                                                                                  \
    };                                                                                                                 \
    template <int TCounter, typename TFunc, typename TThisType>                                                        \
    struct MemberOffsetOf                                                                                              \
    {                                                                                                                  \
        static constexpr int Execute() { return -1; }                                                                  \
    };

#define ENTROPY_NULL_MEMBER_OFFSET_OF_FUNCTION()                                                                       \
    template <typename TDummy>                                                                                         \
    struct MemberOffsetOfExists<ENTROPY_GET_COUNTER_VALUE(), TDummy> : std::bool_constant<true>                        \
    {                                                                                                                  \
    };                                                                                                                 \
    template <typename TFunc, typename TThisType>                                                                      \
    struct MemberOffsetOf<ENTROPY_GET_COUNTER_VALUE(), TFunc, TThisType>                                               \
    {                                                                                                                  \
        static constexpr int Execute() { return -1; }                                                                  \
    };

#define ENTROPY_START_CLASS_REFLECTION_REGISTRATION(className, ...)                                                    \
    {                                                                                                                  \
        ::Entropy::ReflectAndAddAttributes(__VA_ARGS__);                                                               \
        ::Entropy::details::SetReflectedClassAttributes(Traits::GetObjectClassId<ThisReflectedType>{}());              \
        if constexpr (::Entropy::details::HasBaseClass_v<className>)                                                   \
        {                                                                                                              \
            ::Entropy::ReflectClass<Entropy::details::ObjectBaseClass_t<className>>();                                 \
            ::Entropy::details::SetReflectedClassBase(                                                                 \
                ::Entropy::Traits::GetObjectClassId<className>{}(),                                                    \
                ::Entropy::Traits::GetObjectClassId<Entropy::details::ObjectBaseClass_t<className>>{}(),               \
                ::Entropy::details::ObjectBaseClass<className>::GetPointerHandler());                                  \
        }                                                                                                              \
    }

#ifdef ENTROPY_RUNTIME_REFLECTION_ENABLED
#define ENTROPY_REFLECT_ON_LOAD(ClassName)                                                                             \
    static inline bool __sReflect##ClassName##OnLoad = ::Entropy::details::ReflectClassOnLoad<ClassName>();
#else
#define ENTROPY_REFLECT_ON_LOAD(ClassName)
#endif

#define ENTROPY_REFLECT_OBJECT_CLASS(className, ...)                                                                   \
    using ThisReflectedType = className;                                                                               \
    ENTROPY_DECLARE_COUNTER                                                                                            \
    ENTROPY_DECLARE_SINGLE_EXECUTION_META_FUNCTION()                                                                   \
    ENTROPY_SINGLE_EXECUTION_META_FUNCTION(ENTROPY_START_CLASS_REFLECTION_REGISTRATION(className, ##__VA_ARGS__))      \
    ENTROPY_DECLARE_UNARY_MEMBER_OPERATOR_FUNCTION                                                                     \
    ENTROPY_UNARY_MEMBER_OPERATOR_FUNCTION()                                                                           \
    ENTROPY_DECLARE_BINARY_MEMBER_OPERATOR_FUNCTION                                                                    \
    ENTROPY_BINARY_MEMBER_OPERATOR_FUNCTION()                                                                          \
    ENTROPY_DECLARE_MEMBER_OFFSET_OF_FUNCTION                                                                          \
    ENTROPY_NULL_MEMBER_OFFSET_OF_FUNCTION()                                                                           \
    ENTROPY_REFLECT_ON_LOAD(className)                                                                                 \
    template <typename TC, typename TM>                                                                                \
    friend struct ::Entropy::details::MemberVariableHelper;

#define ENTROPY_REFLECT_INTERFACE(className, ...) ENTROPY_REFLECT_OBJECT_CLASS(className, ##__VA_ARGS__)

#define ENTROPY_REFLECT_TEMPLATE_INTERFACE(className, ...) ENTROPY_REFLECT_OBJECT_CLASS(className, ##__VA_ARGS__)

#define ENTROPY_REFLECT_TEMPLATE_CLASS(className, ...) ENTROPY_REFLECT_OBJECT_CLASS(className, ##__VA_ARGS__)

#ifndef ENTROPY_REFLECT_CLASS
#define ENTROPY_REFLECT_CLASS(className, ...)                                                                          \
    ENTROPY_REFLECT_OBJECT_CLASS(className, ##__VA_ARGS__)                                                             \
    static const char* GetClassName() { return #className; }
#endif

#ifndef ENTROPY_REFLECT_CLASS_WITH_BASE
#define ENTROPY_REFLECT_CLASS_WITH_BASE(className, baseClassName, ...)                                                 \
    ENTROPY_REFLECT_CLASS(className)                                                                                   \
    using EntropySuper = baseClassName;
#endif

#ifndef ENTROPY_REFLECT_MEMBER
#define ENTROPY_REFLECT_MEMBER(memberName, ...)                                                                        \
    ENTROPY_DEFINE_LINE_MARKER                                                                                         \
    ENTROPY_SINGLE_EXECUTION_META_FUNCTION({                                                                           \
        ::Entropy::ReflectClass<decltype(memberName)>();                                                               \
        ::Entropy::ReflectAndAddAttributes(__VA_ARGS__);                                                               \
        ::Entropy::Function<const decltype(memberName)&(const ThisReflectedType* classObj)> getFn =                    \
            [](const ThisReflectedType* classObj) -> const decltype(memberName)& { return classObj->memberName; };     \
        ::Entropy::Function<void(ThisReflectedType * classObj, const decltype(memberName)& val)> setFn =               \
            ::Entropy::details::MemberVariableHelper<ThisReflectedType, decltype(memberName)>{}.GetSetterFunction(     \
                &ThisReflectedType::memberName);                                                                       \
        ::Entropy::details::AddReflectedClassMember(                                                                   \
            Traits::GetObjectClassId<ThisReflectedType>{}(), Traits::GetObjectClassId<decltype(memberName)>{}(),       \
            GetMemberDataType<decltype(memberName)>(), #memberName##_EStr, offsetof(ThisReflectedType, memberName),    \
            sizeof(decltype(memberName)), std::move(getFn), std::move(setFn));                                         \
    })                                                                                                                 \
    ENTROPY_UNARY_MEMBER_OPERATOR_FUNCTION({                                                                           \
        /* Note: the extra parens around src.memberName preserve the current const-ness of this object */              \
        ::Entropy::details::InvokeUnaryMemberFunction<decltype((src.memberName)), TFunc>(                              \
            ::Entropy::details::MakeReflectionMemberMetaData(#memberName, ##__VA_ARGS__), src.memberName,              \
            std::forward<TFunc>(callbackObj));                                                                         \
    })                                                                                                                 \
    ENTROPY_BINARY_MEMBER_OPERATOR_FUNCTION({                                                                          \
        /* Note: the extra parens around src/dest.memberName preserve the current const-ness of this object */         \
        ::Entropy::InvokeBinaryMemberFunction<TFunc, decltype((src.memberName)), decltype((dest.memberName))>(         \
            ::Entropy::StringView(#memberName), src.memberName, dest.memberName,                                       \
            std::forward<TAdditionalArgs>(additionalArgs)...);                                                         \
    })
#endif

#ifndef ENTROPY_REFLECT_METHOD_SIGNATURE
#define ENTROPY_REFLECT_METHOD_SIGNATURE(methodName, methodSig, ...)                                                   \
    ENTROPY_DEFINE_LINE_MARKER                                                                                         \
    ENTROPY_SINGLE_EXECUTION_META_FUNCTION({                                                                           \
        ::Entropy::ReflectClass<::Entropy::details::MemberReturnValueType_t<ThisReflectedType, methodSig>>();          \
        ::Entropy::ReflectAndAddAttributes(__VA_ARGS__);                                                               \
        ::Entropy::details::AddReflectedClassMethod(                                                                   \
            ::Entropy::Traits::GetObjectClassId<ThisReflectedType>{}(), #methodName##_EStr,                            \
            ::Entropy::Traits::GetObjectClassId<                                                                       \
                ::Entropy::details::MemberReturnValueType_t<ThisReflectedType, methodSig>>{}(),                        \
            ::Entropy::DynamicFunction(                                                                                \
                static_cast<::Entropy::details::MemberFunctionType_t<ThisReflectedType, methodSig>>(                   \
                    &ThisReflectedType::methodName)));                                                                 \
    })                                                                                                                 \
    ENTROPY_UNARY_MEMBER_OPERATOR_FUNCTION()                                                                           \
    ENTROPY_BINARY_MEMBER_OPERATOR_FUNCTION()                                                                          \
    ENTROPY_NULL_MEMBER_OFFSET_OF_FUNCTION()
#endif

#ifndef ENTROPY_REFLECT_METHOD
#define ENTROPY_REFLECT_METHOD(methodName, ...)                                                                        \
    ENTROPY_DEFINE_LINE_MARKER                                                                                         \
    ENTROPY_SINGLE_EXECUTION_META_FUNCTION({                                                                           \
        ::Entropy::ReflectClass<::Entropy::details::MemberReturnValueType_t<                                           \
            ThisReflectedType, decltype(&ThisReflectedType::methodName)>>();                                           \
        ::Entropy::ReflectAndAddAttributes(__VA_ARGS__);                                                               \
        ::Entropy::details::AddReflectedClassMethod(                                                                   \
            ::Entropy::Traits::GetObjectClassId<ThisReflectedType>{}(), #methodName##_EStr,                            \
            ::Entropy::Traits::GetObjectClassId<::Entropy::details::MemberReturnValueType_t<                           \
                ThisReflectedType, decltype(&ThisReflectedType::methodName)>>{}(),                                     \
            ::Entropy::DynamicFunction(&ThisReflectedType::methodName));                                               \
    })                                                                                                                 \
    ENTROPY_UNARY_MEMBER_OPERATOR_FUNCTION()                                                                           \
    ENTROPY_BINARY_MEMBER_OPERATOR_FUNCTION()                                                                          \
    ENTROPY_NULL_MEMBER_OFFSET_OF_FUNCTION()
#endif