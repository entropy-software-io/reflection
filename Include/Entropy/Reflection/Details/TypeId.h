// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

#include "Entropy/Core/Details/StringOps.h"
#include "Entropy/Core/Details/StrongAlias.h"

namespace Entropy
{

ENTROPY_DEFINE_STRONG_ALIAS(TypeId, unsigned int)

static constexpr TypeId cInvalidTypeId = TypeId(0);

namespace details
{

StringOps::StringType MakeTypeNameFromRawName(const char* rawTypeName);
StringOps::StringType MakeTypeNameNoTemplateParamsFromRawName(const char* rawTypeName);

template <typename TFn, typename T, typename = void>
struct MakeTypeNameFn
{
    StringOps::StringType operator()(TFn fn) const { return fn(typeid(T).name()); }
};

template <typename TFn, typename T>
struct MakeTypeNameFn<
    TFn, T,
    typename std::enable_if<std::is_const<T>::value && !std::is_pointer<T>::value && !std::is_array<T>::value>::type>
{
    StringOps::StringType operator()(TFn fn) const
    {
        StringOps::StringType ret = MakeTypeNameFn<TFn, typename std::remove_const<T>::type>{}(fn);
        StringOps::Append(ret, " const");
        return ret;
    }
};

template <typename TFn, typename T>
struct MakeTypeNameFn<TFn, T, typename std::enable_if<!std::is_const<T>::value && std::is_pointer<T>::value>::type>
{
    StringOps::StringType operator()(TFn fn) const
    {
        StringOps::StringType ret = MakeTypeNameFn<TFn, typename std::remove_pointer<T>::type>{}(fn);
        StringOps::Append(ret, "*");
        return ret;
    }
};

template <typename TFn, typename T>
struct MakeTypeNameFn<TFn, T, typename std::enable_if<std::is_const<T>::value && std::is_pointer<T>::value>::type>
{
    StringOps::StringType operator()(TFn fn) const
    {
        // remove_pointer removes const too
        StringOps::StringType ret = MakeTypeNameFn<TFn, typename std::remove_pointer<T>::type>{}(fn);
        StringOps::Append(ret, "*const");
        return ret;
    }
};

template <typename TFn, typename T>
struct MakeTypeNameFn<TFn, T[]>
{
    StringOps::StringType operator()(TFn fn) const
    {
        StringOps::StringType ret = MakeTypeNameFn<TFn, typename std::remove_extent<T[]>::type>{}(fn);
        StringOps::Append(ret, "[]");
        return ret;
    }
};

template <typename TFn, typename T, std::size_t I>
struct MakeTypeNameFn<TFn, T[I]>
{
    StringOps::StringType operator()(TFn fn) const
    {
        char iBuf[20] = {0};
        std::snprintf(iBuf, sizeof(iBuf) / sizeof(char), "%to", I);

        StringOps::StringType ret = MakeTypeNameFn<TFn, T>{}(fn);
        StringOps::Append(ret, "[");
        StringOps::Append(ret, iBuf);
        StringOps::Append(ret, "]");
        return ret;
    }
};

// Arrays are processed in reading order. char [1][2][3] will be processed <char[2][3], 1>, then <char[3], 2>, etc. If
// left alone, we produce the string char[3][2][1]. Instead, handle some common multi-dimensional arrays explicitly.
template <typename TFn, typename T, std::size_t I, std::size_t J>
struct MakeTypeNameFn<TFn, T[I][J]>
{
    StringOps::StringType operator()(TFn fn) const
    {
        char iBuf[20] = {0};
        std::snprintf(iBuf, sizeof(iBuf) / sizeof(char), "%to", I);

        char jBuf[20] = {0};
        std::snprintf(jBuf, sizeof(jBuf) / sizeof(char), "%to", J);

        StringOps::StringType ret = MakeTypeNameFn<TFn, T>{}(fn);
        StringOps::Append(ret, "[");
        StringOps::Append(ret, iBuf);
        StringOps::Append(ret, "][");
        StringOps::Append(ret, jBuf);
        StringOps::Append(ret, "]");
        return ret;
    }
};

// Arrays are processed in reading order. char [1][2][3] will be processed <char[2][3], 1>, then <char[3], 2>, etc. If
// left alone, we produce the string char[3][2][1]. Instead, handle some common multi-dimensional arrays explicitly.
template <typename TFn, typename T, std::size_t I, std::size_t J, std::size_t K>
struct MakeTypeNameFn<TFn, T[I][J][K]>
{
    StringOps::StringType operator()(TFn fn) const
    {
        char iBuf[20] = {0};
        std::snprintf(iBuf, sizeof(iBuf) / sizeof(char), "%to", I);

        char jBuf[20] = {0};
        std::snprintf(jBuf, sizeof(jBuf) / sizeof(char), "%to", J);

        char kBuf[20] = {0};
        std::snprintf(kBuf, sizeof(kBuf) / sizeof(char), "%to", K);

        StringOps::StringType ret = MakeTypeNameFn<TFn, T>{}(fn);
        StringOps::Append(ret, "[");
        StringOps::Append(ret, iBuf);
        StringOps::Append(ret, "][");
        StringOps::Append(ret, jBuf);
        StringOps::Append(ret, "][");
        StringOps::Append(ret, kBuf);
        StringOps::Append(ret, "]");
        return ret;
    }
};

template <typename TFn, typename T>
struct MakeTypeNameFn<TFn, T, typename std::enable_if<std::is_lvalue_reference<T>::value>::type>
{
    StringOps::StringType operator()(TFn fn) const
    {
        StringOps::StringType ret = MakeTypeNameFn<TFn, typename std::remove_reference<T>::type>{}(fn);
        StringOps::Append(ret, "&");
        return ret;
    }
};

template <typename TFn, typename T>
struct MakeTypeNameFn<TFn, T, typename std::enable_if<std::is_rvalue_reference<T>::value>::type>
{
    StringOps::StringType operator()(TFn fn) const
    {
        StringOps::StringType ret = MakeTypeNameFn<TFn, typename std::remove_reference<T>::type>{}(fn);
        StringOps::Append(ret, "&&");
        return ret;
    }
};

template <typename TFn, typename T>
struct MakeTypeNameFn<TFn, T, typename std::enable_if<std::is_volatile<T>::value>::type>
{
    StringOps::StringType operator()(TFn fn) const
    {
        StringOps::StringType ret = MakeTypeNameFn<TFn, typename std::remove_volatile<T>::type>{}(fn);
        StringOps::Append(ret, " volatile");
        return ret;
    }
};

template <typename TFn, typename... T>
struct MakeTypeNameTemplateParamFn
{
    StringOps::StringType operator()(TFn fn) const { return StringOps::StringType(); }
};

template <typename TFn, typename TFirst, typename... TRest>
struct MakeTypeNameTemplateParamFn<TFn, TFirst, TRest...>
{
    StringOps::StringType operator()(TFn fn) const
    {
        StringOps::StringType ret = MakeTypeNameFn<TFn, TFirst>{}(fn);
        if ENTROPY_CONSTEXPR (sizeof...(TRest) > 0)
        {
            StringOps::Append(ret, ", ");
            StringOps::Append(ret, MakeTypeNameTemplateParamFn<TFn, TRest...>{}(fn));
        }
        return ret;
    }
};

template <typename TFn, template <typename...> class T, typename... Tn>
struct MakeTypeNameFn<TFn, T<Tn...>>
{
    StringOps::StringType operator()(TFn fn) const
    {
        auto baseFn = &MakeTypeNameNoTemplateParamsFromRawName;

        StringOps::StringType ret = baseFn(typeid(T<Tn...>).name());
        StringOps::Append(ret, "<");
        StringOps::Append(ret, MakeTypeNameTemplateParamFn<TFn, Tn...>{}(fn));
        StringOps::Append(ret, ">");
        return ret;
    }
};

template <typename T, typename = void>
struct MakeTypeName
{
    StringOps::StringType operator()() const
    {
        auto fn = &MakeTypeNameFromRawName;
        return MakeTypeNameFn<decltype(fn), T>{}(fn);
    }
};

TypeId MakeTypeIdFromTypeName(const StringOps::StringType& typeName);

template <typename T>
TypeId MakeTypeIdFromTypeName()
{
    return MakeTypeIdFromTypeName(MakeTypeName<T>{}());
}

} // namespace details

namespace Traits
{

template <typename T, typename = void>
struct TypeIdOf
{
    inline TypeId operator()() const
    {
        static TypeId id = Entropy::details::MakeTypeIdFromTypeName<T>();
        return id;
    }
};

template <typename T, typename = void>
struct TypeNameOf
{
    inline StringOps::StringType operator()() const
    {
        static StringOps::StringType name = Entropy::details::MakeTypeName<T>{}();
        return name;
    }
};

} // namespace Traits

} // namespace Entropy
