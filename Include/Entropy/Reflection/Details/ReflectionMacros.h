// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

// This is a hack to get around an C++11 MSVC bug when using reflection with a template type.
// The code will generate a partial specialization that will be similar to:
//      struct __MemberTypeOperatorExists<MyType<T1>::__GetCounterValue<123>::value, ...
// It mistakes the "MyType<T1>" as involving a template parameter for our non-type counter parameter. This isn't
// strictly true as "T1" is just a consequence of being inside a templated class and not a true requirement.
//
// To get around this, we use std::integral_constant instead of int as the template parameter. Now, the counter isn't a
// non-type and MSVC won't complain.
#define ENTROPY_FAKE_INT_CONSTANT(x) std::integral_constant<int, x>

#define ENTROPY_CLASS_TYPE_OPERATOR_FUNCTION(...)                                                                      \
    template <typename TThisType, typename TFunc, typename _TDummy = void>                                             \
    struct __ClassTypeOperator                                                                                         \
    {                                                                                                                  \
        static void Execute(TFunc callbackObj) { __VA_ARGS__ }                                                         \
    };

#define ENTROPY_DECLARE_MEMBER_TYPE_OPERATOR_FUNCTION                                                                  \
    template <typename TCounter, typename TDummy = void>                                                               \
    struct __MemberTypeOperatorExists : public std::false_type                                                         \
    {                                                                                                                  \
    };                                                                                                                 \
    template <typename TThisType, typename TFunc, typename TCounter, typename TDummy = void>                           \
    struct __MemberTypeOperator                                                                                        \
    {                                                                                                                  \
        static void Execute(TFunc callbackObj) {}                                                                      \
    };

#define ENTROPY_MEMBER_TYPE_OPERATOR_FUNCTION(line, ...)                                                               \
    template <typename TDummy>                                                                                         \
    struct __MemberTypeOperatorExists<ENTROPY_FAKE_INT_CONSTANT(ENTROPY_GET_COUNTER_VALUE(line)), TDummy>              \
        : public std::true_type                                                                                        \
    {                                                                                                                  \
    };                                                                                                                 \
    template <typename TThisType, typename TFunc, typename TDummy>                                                     \
    struct __MemberTypeOperator<TThisType, TFunc, ENTROPY_FAKE_INT_CONSTANT(ENTROPY_GET_COUNTER_VALUE(line)), TDummy>  \
    {                                                                                                                  \
        static void Execute(TFunc callbackObj) { __VA_ARGS__ }                                                         \
    };

#define ENTROPY_DECLARE_UNARY_MEMBER_OPERATOR_FUNCTION                                                                 \
    template <typename TCounter, typename TDummy = void>                                                               \
    struct __UnaryMemberOperatorExists : public std::false_type                                                        \
    {                                                                                                                  \
    };                                                                                                                 \
    template <typename TThisType, typename TFunc, typename TCounter, typename TDummy = void>                           \
    struct __UnaryMemberOperator                                                                                       \
    {                                                                                                                  \
        static void Execute(TThisType& src, TFunc callbackObj) {}                                                      \
    };

#define ENTROPY_UNARY_MEMBER_OPERATOR_FUNCTION(line, ...)                                                              \
    template <typename TDummy>                                                                                         \
    struct __UnaryMemberOperatorExists<ENTROPY_FAKE_INT_CONSTANT(ENTROPY_GET_COUNTER_VALUE(line)), TDummy>             \
        : public std::true_type                                                                                        \
    {                                                                                                                  \
    };                                                                                                                 \
    template <typename TThisType, typename TFunc, typename TDummy>                                                     \
    struct __UnaryMemberOperator<TThisType, TFunc, ENTROPY_FAKE_INT_CONSTANT(ENTROPY_GET_COUNTER_VALUE(line)), TDummy> \
    {                                                                                                                  \
        static void Execute(TThisType& src, TFunc callbackObj) { __VA_ARGS__ }                                         \
    };

#define ENTROPY_DECLARE_BINARY_MEMBER_OPERATOR_FUNCTION                                                                \
    template <typename TCounter, typename TDummy = void>                                                               \
    struct __BinaryMemberOperatorExists : public std::false_type                                                       \
    {                                                                                                                  \
    };                                                                                                                 \
    template <typename TOtherType, typename TCounter, typename = void>                                                 \
    struct __BinaryMemberOperatorExistsOnOther : public std::false_type                                                \
    {                                                                                                                  \
    };                                                                                                                 \
    template <typename TThisType, typename TOtherType, typename TFunc, typename TCounter, typename = void>             \
    struct __BinaryMemberOperator                                                                                      \
    {                                                                                                                  \
        static void Execute(TThisType& thisObj, TOtherType& otherObj, TFunc callbackObj) {}                            \
    };

#define ENTROPY_EMPTY_BINARY_MEMBER_OPERATOR_FUNCTION(line)                                                            \
    template <typename TDummy>                                                                                         \
    struct __BinaryMemberOperatorExists<ENTROPY_FAKE_INT_CONSTANT(ENTROPY_GET_COUNTER_VALUE(line)), TDummy>            \
        : public std::true_type                                                                                        \
    {                                                                                                                  \
    };

#define ENTROPY_BINARY_MEMBER_OPERATOR_FUNCTION(line, memberName, ...)                                                 \
    ENTROPY_EMPTY_BINARY_MEMBER_OPERATOR_FUNCTION(line)                                                                \
    template <typename TOtherType>                                                                                     \
    struct __BinaryMemberOperatorExistsOnOther<TOtherType, ENTROPY_FAKE_INT_CONSTANT(ENTROPY_GET_COUNTER_VALUE(line)), \
                                               decltype(&TOtherType::memberName, void())> : public std::true_type      \
    {                                                                                                                  \
    };                                                                                                                 \
    template <typename TThisType, typename TOtherType, typename TFunc>                                                 \
    struct __BinaryMemberOperator<                                                                                     \
        TThisType, TOtherType, TFunc, ENTROPY_FAKE_INT_CONSTANT(ENTROPY_GET_COUNTER_VALUE(line)),                      \
        typename std::enable_if<__BinaryMemberOperatorExistsOnOther<                                                   \
            TOtherType, ENTROPY_FAKE_INT_CONSTANT(ENTROPY_GET_COUNTER_VALUE(line))>::value>::type>                     \
    {                                                                                                                  \
        static void Execute(TThisType& thisObj, TOtherType& otherObj, TFunc callbackObj) { __VA_ARGS__ }               \
    };

#define ENTROPY_DECLARE_MEMBER_OFFSET_OF_FUNCTION                                                                      \
    template <typename TCounter, typename TDummy = void>                                                               \
    struct MemberOffsetOfExists : public std::false_type                                                               \
    {                                                                                                                  \
    };                                                                                                                 \
    template <typename TCounter, typename TFunc, typename TThisType>                                                   \
    struct MemberOffsetOf                                                                                              \
    {                                                                                                                  \
        static constexpr int Execute() { return -1; }                                                                  \
    };

#define ENTROPY_NULL_MEMBER_OFFSET_OF_FUNCTION(line)                                                                   \
    template <typename TDummy>                                                                                         \
    struct MemberOffsetOfExists<ENTROPY_FAKE_INT_CONSTANT(ENTROPY_GET_COUNTER_VALUE(line)), TDummy>                    \
        : public std::true_type                                                                                        \
    {                                                                                                                  \
    };                                                                                                                 \
    template <typename TFunc, typename TThisType>                                                                      \
    struct MemberOffsetOf<ENTROPY_FAKE_INT_CONSTANT(ENTROPY_GET_COUNTER_VALUE(line)), TFunc, TThisType>                \
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

#define ENTROPY_REFLECT_OBJECT_CLASS(line, className, ...)                                                             \
    using ThisReflectedType = className;                                                                               \
    ENTROPY_DECLARE_COUNTER(line)                                                                                      \
    ENTROPY_CLASS_TYPE_OPERATOR_FUNCTION({                                                                             \
        /* Note: the extra parens around src.memberName preserve the current const-ness of this object */              \
        ::Entropy::details::InvokeClassTypeFunction<ThisReflectedType, TFunc>(                                         \
            ::Entropy::details::MakeAttributeCollection(__VA_ARGS__), callbackObj);                                    \
    })                                                                                                                 \
    ENTROPY_DECLARE_MEMBER_TYPE_OPERATOR_FUNCTION                                                                      \
    ENTROPY_MEMBER_TYPE_OPERATOR_FUNCTION(line)                                                                        \
    ENTROPY_DECLARE_UNARY_MEMBER_OPERATOR_FUNCTION                                                                     \
    ENTROPY_UNARY_MEMBER_OPERATOR_FUNCTION(line)                                                                       \
    ENTROPY_DECLARE_BINARY_MEMBER_OPERATOR_FUNCTION                                                                    \
    ENTROPY_EMPTY_BINARY_MEMBER_OPERATOR_FUNCTION(line)                                                                \
    ENTROPY_DECLARE_MEMBER_OFFSET_OF_FUNCTION                                                                          \
    ENTROPY_NULL_MEMBER_OFFSET_OF_FUNCTION(line)                                                                       \
    ENTROPY_REFLECT_ON_LOAD(ENTROPY_LITERAL(className))

#define ENTROPY_REFLECT_INTERFACE(className, ...)                                                                      \
    ENTROPY_REFLECT_OBJECT_CLASS(__LINE__, ENTROPY_LITERAL(className), ##__VA_ARGS__)

#define ENTROPY_REFLECT_TEMPLATE_INTERFACE(className, ...)                                                             \
    ENTROPY_REFLECT_OBJECT_CLASS(__LINE__, ENTROPY_LITERAL(className), ##__VA_ARGS__)

#define ENTROPY_REFLECT_TEMPLATE_CLASS(className, ...)                                                                 \
    ENTROPY_REFLECT_OBJECT_CLASS(__LINE__, ENTROPY_LITERAL(className), ##__VA_ARGS__)

#ifndef ENTROPY_REFLECT_CLASS
#define ENTROPY_REFLECT_CLASS(className, ...)                                                                          \
    ENTROPY_REFLECT_OBJECT_CLASS(__LINE__, ENTROPY_LITERAL(className), ##__VA_ARGS__)
#endif

#ifndef ENTROPY_REFLECT_CLASS_WITH_BASE
#define ENTROPY_REFLECT_CLASS_WITH_BASE(className, baseClassName, ...)                                                 \
    ENTROPY_REFLECT_OBJECT_CLASS(__LINE__, ENTROPY_LITERAL(className), ##__VA_ARGS__)                                  \
    using EntropySuper = baseClassName;
#endif

#ifndef ENTROPY_REFLECT_MEMBER
#define ENTROPY_REFLECT_MEMBER(memberName, ...)                                                                        \
    ENTROPY_DEFINE_LINE_MARKER(__LINE__)                                                                               \
    ENTROPY_MEMBER_TYPE_OPERATOR_FUNCTION(__LINE__, {                                                                  \
        /* Note: the extra parens around src.memberName preserve the current const-ness of this object */              \
        ::Entropy::details::InvokeMemberTypeFunction<decltype((memberName)), TFunc>(                                   \
            ::Entropy::details::MakeReflectionMemberMetaData(#memberName, ##__VA_ARGS__), callbackObj);                \
    })                                                                                                                 \
    ENTROPY_UNARY_MEMBER_OPERATOR_FUNCTION(__LINE__, {                                                                 \
        /* Note: the extra parens around src.memberName preserve the current const-ness of this object */              \
        ::Entropy::details::InvokeUnaryMemberFunction<decltype((src.memberName)), TFunc>(                              \
            ::Entropy::details::MakeReflectionMemberMetaData(#memberName, ##__VA_ARGS__), src.memberName,              \
            callbackObj);                                                                                              \
    })                                                                                                                 \
    ENTROPY_BINARY_MEMBER_OPERATOR_FUNCTION(__LINE__, memberName, {                                                    \
        /* Note: the extra parens around thisObj/otherObj.memberName preserve the current const-ness of this object */ \
        ::Entropy::details::InvokeBinaryMemberFunction<decltype((thisObj.memberName)),                                 \
                                                       decltype((otherObj.memberName)), TFunc>(                        \
            #memberName, thisObj.memberName, otherObj.memberName, callbackObj);                                        \
    })
#endif

#ifndef ENTROPY_REFLECT_METHOD_SIGNATURE
#define ENTROPY_REFLECT_METHOD_SIGNATURE(methodName, methodSig, ...)                                                   \
    ENTROPY_DEFINE_LINE_MARKER(__LINE__)                                                                               \
    ENTROPY_MEMBER_TYPE_OPERATOR_FUNCTION(__LINE__)                                                                    \
    ENTROPY_UNARY_MEMBER_OPERATOR_FUNCTION(__LINE__)                                                                   \
    ENTROPY_EMPTY_BINARY_MEMBER_OPERATOR_FUNCTION(__LINE__)                                                            \
    ENTROPY_NULL_MEMBER_OFFSET_OF_FUNCTION(__LINE__)
#endif

#ifndef ENTROPY_REFLECT_METHOD
#define ENTROPY_REFLECT_METHOD(methodName, ...)                                                                        \
    ENTROPY_DEFINE_LINE_MARKER(__LINE__)                                                                               \
    ENTROPY_MEMBER_TYPE_OPERATOR_FUNCTION(__LINE__)                                                                    \
    ENTROPY_UNARY_MEMBER_OPERATOR_FUNCTION(__LINE__)                                                                   \
    ENTROPY_EMPTY_BINARY_MEMBER_OPERATOR_FUNCTION(__LINE__)                                                            \
    ENTROPY_NULL_MEMBER_OFFSET_OF_FUNCTION(__LINE__)
#endif