// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

#define ENTROPY_DECLARE_SINGLE_EXECUTION_META_FUNCTION()                                                               \
    template <int _TCounter, typename _TClass, typename _TDummy = void>                                                \
    struct SingleExecutionMetaFunctionExists : std::false_type                                                         \
    {                                                                                                                  \
    };                                                                                                                 \
    template <int _TCounter, typename _TClass, typename _TDummy = void>                                                \
    struct SingleExecutionMetaFunction                                                                                 \
    {                                                                                                                  \
        static void Execute() {}                                                                                       \
    };

#define ENTROPY_SINGLE_EXECUTION_META_FUNCTION(...)                                                                    \
    template <typename _TDummy>                                                                                        \
    struct SingleExecutionMetaFunctionExists<ENTROPY_GET_COUNTER_VALUE(), ThisReflectedType, _TDummy> : std::true_type \
    {                                                                                                                  \
    };                                                                                                                 \
    template <typename _TDummy>                                                                                        \
    struct SingleExecutionMetaFunction<ENTROPY_GET_COUNTER_VALUE(), ThisReflectedType, _TDummy>                        \
    {                                                                                                                  \
        static void Execute() { __VA_ARGS__ }                                                                          \
    };

#define ENTROPY_DECLARE_MEMBER_TYPE_OPERATOR_FUNCTION                                                                  \
    template <int TCounter, typename TDummy = void>                                                                    \
    struct __MemberTypeOperatorExists : std::false_type                                                                \
    {                                                                                                                  \
    };                                                                                                                 \
    template <typename TThisType, typename TFunc, int TCounter>                                                        \
    struct __MemberTypeOperator                                                                                        \
    {                                                                                                                  \
        static void Execute(TFunc callbackObj) {}                                                                      \
    };

#define ENTROPY_MEMBER_TYPE_OPERATOR_FUNCTION(...)                                                                     \
    template <typename TDummy>                                                                                         \
    struct __MemberTypeOperatorExists<ENTROPY_GET_COUNTER_VALUE(), TDummy> : std::true_type                            \
    {                                                                                                                  \
    };                                                                                                                 \
    template <typename TThisType, typename TFunc>                                                                      \
    struct __MemberTypeOperator<TThisType, TFunc, ENTROPY_GET_COUNTER_VALUE()>                                         \
    {                                                                                                                  \
        static void Execute(TFunc callbackObj) { __VA_ARGS__ }                                                         \
    };

#define ENTROPY_DECLARE_UNARY_MEMBER_OPERATOR_FUNCTION                                                                 \
    template <int TCounter, typename TDummy = void>                                                                    \
    struct __UnaryMemberOperatorExists : std::false_type                                                               \
    {                                                                                                                  \
    };                                                                                                                 \
    template <typename TThisType, typename TFunc, int TCounter>                                                        \
    struct __UnaryMemberOperator                                                                                       \
    {                                                                                                                  \
        static void Execute(TThisType& src, TFunc callbackObj) {}                                                      \
    };

#define ENTROPY_UNARY_MEMBER_OPERATOR_FUNCTION(...)                                                                    \
    template <typename TDummy>                                                                                         \
    struct __UnaryMemberOperatorExists<ENTROPY_GET_COUNTER_VALUE(), TDummy> : std::true_type                           \
    {                                                                                                                  \
    };                                                                                                                 \
    template <typename TThisType, typename TFunc>                                                                      \
    struct __UnaryMemberOperator<TThisType, TFunc, ENTROPY_GET_COUNTER_VALUE()>                                        \
    {                                                                                                                  \
        static void Execute(TThisType& src, TFunc callbackObj) { __VA_ARGS__ }                                         \
    };

#define ENTROPY_DECLARE_BINARY_MEMBER_OPERATOR_FUNCTION                                                                \
    template <int TCounter, typename TDummy = void>                                                                    \
    struct __BinaryMemberOperatorExists : std::false_type                                                              \
    {                                                                                                                  \
    };                                                                                                                 \
    template <typename TOtherType, int TCounter, typename = void>                                                      \
    struct __BinaryMemberOperatorExistsOnOther : std::false_type                                                       \
    {                                                                                                                  \
    };                                                                                                                 \
    template <typename TThisType, typename TOtherType, typename TFunc, int TCounter, typename = void>                  \
    struct __BinaryMemberOperator                                                                                      \
    {                                                                                                                  \
        static void Execute(TThisType& thisObj, TOtherType& otherObj, TFunc callbackObj) {}                            \
    };

#define ENTROPY_EMPTY_BINARY_MEMBER_OPERATOR_FUNCTION()                                                                \
    template <typename TDummy>                                                                                         \
    struct __BinaryMemberOperatorExists<ENTROPY_GET_COUNTER_VALUE(), TDummy> : std::true_type                          \
    {                                                                                                                  \
    };

#define ENTROPY_BINARY_MEMBER_OPERATOR_FUNCTION(memberName, ...)                                                       \
    ENTROPY_EMPTY_BINARY_MEMBER_OPERATOR_FUNCTION()                                                                    \
    template <typename TOtherType>                                                                                     \
    struct __BinaryMemberOperatorExistsOnOther<TOtherType, ENTROPY_GET_COUNTER_VALUE(),                                \
                                               decltype(&TOtherType::memberName, void())> : std::true_type             \
    {                                                                                                                  \
    };                                                                                                                 \
    template <typename TThisType, typename TOtherType, typename TFunc>                                                 \
    struct __BinaryMemberOperator<TThisType, TOtherType, TFunc, ENTROPY_GET_COUNTER_VALUE(),                           \
                                  typename std::enable_if<__BinaryMemberOperatorExistsOnOther<                         \
                                      TOtherType, ENTROPY_GET_COUNTER_VALUE()>::value>::type>                          \
    {                                                                                                                  \
        static void Execute(TThisType& thisObj, TOtherType& otherObj, TFunc callbackObj) { __VA_ARGS__ }               \
    };

#define ENTROPY_DECLARE_MEMBER_OFFSET_OF_FUNCTION                                                                      \
    template <int TCounter, typename TDummy = void>                                                                    \
    struct MemberOffsetOfExists : std::false_type                                                                      \
    {                                                                                                                  \
    };                                                                                                                 \
    template <int TCounter, typename TFunc, typename TThisType>                                                        \
    struct MemberOffsetOf                                                                                              \
    {                                                                                                                  \
        static constexpr int Execute() { return -1; }                                                                  \
    };

#define ENTROPY_NULL_MEMBER_OFFSET_OF_FUNCTION()                                                                       \
    template <typename TDummy>                                                                                         \
    struct MemberOffsetOfExists<ENTROPY_GET_COUNTER_VALUE(), TDummy> : std::true_type                                  \
    {                                                                                                                  \
    };                                                                                                                 \
    template <typename TFunc, typename TThisType>                                                                      \
    struct MemberOffsetOf<ENTROPY_GET_COUNTER_VALUE(), TFunc, TThisType>                                               \
    {                                                                                                                  \
        static constexpr int Execute() { return -1; }                                                                  \
    };

#define ENTROPY_START_CLASS_REFLECTION_REGISTRATION(className, ...)

#if defined(ENTROPY_RUNTIME_REFLECTION_ENABLED) && (__cplusplus >= 201703L)
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
    ENTROPY_DECLARE_MEMBER_TYPE_OPERATOR_FUNCTION                                                                      \
    ENTROPY_MEMBER_TYPE_OPERATOR_FUNCTION()                                                                            \
    ENTROPY_DECLARE_UNARY_MEMBER_OPERATOR_FUNCTION                                                                     \
    ENTROPY_UNARY_MEMBER_OPERATOR_FUNCTION()                                                                           \
    ENTROPY_DECLARE_BINARY_MEMBER_OPERATOR_FUNCTION                                                                    \
    ENTROPY_EMPTY_BINARY_MEMBER_OPERATOR_FUNCTION()                                                                    \
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
    ENTROPY_REFLECT_CLASS(className, ##__VA_ARGS__)                                                                    \
    using EntropySuper = baseClassName;
#endif

#ifndef ENTROPY_REFLECT_MEMBER
#define ENTROPY_REFLECT_MEMBER(memberName, ...)                                                                        \
    ENTROPY_DEFINE_LINE_MARKER                                                                                         \
    ENTROPY_SINGLE_EXECUTION_META_FUNCTION()                                                                           \
    ENTROPY_MEMBER_TYPE_OPERATOR_FUNCTION({                                                                            \
        /* Note: the extra parens around src.memberName preserve the current const-ness of this object */              \
        ::Entropy::details::InvokeMemberTypeFunction<decltype((memberName)), TFunc>(                                   \
            ::Entropy::details::MakeReflectionMemberMetaData(#memberName, ##__VA_ARGS__), callbackObj);                \
    })                                                                                                                 \
    ENTROPY_UNARY_MEMBER_OPERATOR_FUNCTION({                                                                           \
        /* Note: the extra parens around src.memberName preserve the current const-ness of this object */              \
        ::Entropy::details::InvokeUnaryMemberFunction<decltype((src.memberName)), TFunc>(                              \
            ::Entropy::details::MakeReflectionMemberMetaData(#memberName, ##__VA_ARGS__), src.memberName,              \
            callbackObj);                                                                                              \
    })                                                                                                                 \
    ENTROPY_BINARY_MEMBER_OPERATOR_FUNCTION(memberName, {                                                              \
        /* Note: the extra parens around thisObj/otherObj.memberName preserve the current const-ness of this object */ \
        ::Entropy::details::InvokeBinaryMemberFunction<decltype((thisObj.memberName)),                                 \
                                                       decltype((otherObj.memberName)), TFunc>(                        \
            #memberName, thisObj.memberName, otherObj.memberName, callbackObj);                                        \
    })
#endif

#ifndef ENTROPY_REFLECT_METHOD_SIGNATURE
#define ENTROPY_REFLECT_METHOD_SIGNATURE(methodName, methodSig, ...)                                                   \
    ENTROPY_DEFINE_LINE_MARKER                                                                                         \
    ENTROPY_SINGLE_EXECUTION_META_FUNCTION()                                                                           \
    ENTROPY_MEMBER_TYPE_OPERATOR_FUNCTION()                                                                            \
    ENTROPY_UNARY_MEMBER_OPERATOR_FUNCTION()                                                                           \
    ENTROPY_EMPTY_BINARY_MEMBER_OPERATOR_FUNCTION()                                                                    \
    ENTROPY_NULL_MEMBER_OFFSET_OF_FUNCTION()
#endif

#ifndef ENTROPY_REFLECT_METHOD
#define ENTROPY_REFLECT_METHOD(methodName, ...)                                                                        \
    ENTROPY_DEFINE_LINE_MARKER                                                                                         \
    ENTROPY_SINGLE_EXECUTION_META_FUNCTION()                                                                           \
    ENTROPY_MEMBER_TYPE_OPERATOR_FUNCTION()                                                                            \
    ENTROPY_UNARY_MEMBER_OPERATOR_FUNCTION()                                                                           \
    ENTROPY_EMPTY_BINARY_MEMBER_OPERATOR_FUNCTION()                                                                    \
    ENTROPY_NULL_MEMBER_OFFSET_OF_FUNCTION()
#endif