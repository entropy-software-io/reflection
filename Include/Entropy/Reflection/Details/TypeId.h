// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

#include "Entropy/Core/Details/StrongAlias.h"
#include "Entropy/Reflection/Details/ContainerTypes.h"

namespace Entropy
{

ENTROPY_DEFINE_STRONG_ALIAS(TypeId, unsigned int)

static constexpr TypeId cInvalidTypeId = TypeId(0);

namespace details
{

ReflectionContainerTraits<TypeId>::StringType MakeTypeNameFromRawName(const char* rawTypeName);
ReflectionContainerTraits<TypeId>::StringType MakeTypeNameNoTemplateParamsFromRawName(const char* rawTypeName);

template <typename TFn, typename T, typename = void>
struct MakeTypeNameFn
{
    ReflectionContainerTraits<TypeId>::StringType operator()(TFn fn) const { return fn(typeid(T).name()); }
};

template <typename TFn, typename T>
struct MakeTypeNameFn<
    TFn, T,
    typename std::enable_if<std::is_const<T>::value && !std::is_pointer<T>::value && !std::is_array<T>::value>::type>
{
    ReflectionContainerTraits<TypeId>::StringType operator()(TFn fn) const
    {
        using ContainerTraits = ReflectionContainerTraits<TypeId>;
        using StrOps          = StringOps<ContainerTraits::StringType>;

        ReflectionContainerTraits<TypeId>::StringType ret =
            MakeTypeNameFn<TFn, typename std::remove_const<T>::type>{}(fn);
        StrOps::Append(ret, " const");
        return ret;
    }
};

template <typename TFn, typename T>
struct MakeTypeNameFn<TFn, T, typename std::enable_if<!std::is_const<T>::value && std::is_pointer<T>::value>::type>
{
    ReflectionContainerTraits<TypeId>::StringType operator()(TFn fn) const
    {
        using ContainerTraits = ReflectionContainerTraits<TypeId>;
        using StrOps          = StringOps<ContainerTraits::StringType>;

        ReflectionContainerTraits<TypeId>::StringType ret =
            MakeTypeNameFn<TFn, typename std::remove_pointer<T>::type>{}(fn);
        StrOps::Append(ret, "*");
        return ret;
    }
};

template <typename TFn, typename T>
struct MakeTypeNameFn<TFn, T, typename std::enable_if<std::is_const<T>::value && std::is_pointer<T>::value>::type>
{
    ReflectionContainerTraits<TypeId>::StringType operator()(TFn fn) const
    {
        using ContainerTraits = ReflectionContainerTraits<TypeId>;
        using StrOps          = StringOps<ContainerTraits::StringType>;

        // remove_pointer removes const too
        ReflectionContainerTraits<TypeId>::StringType ret =
            MakeTypeNameFn<TFn, typename std::remove_pointer<T>::type>{}(fn);
        StrOps::Append(ret, "*const");
        return ret;
    }
};

template <typename TFn, typename T>
struct MakeTypeNameFn<TFn, T[]>
{
    ReflectionContainerTraits<TypeId>::StringType operator()(TFn fn) const
    {
        using ContainerTraits = ReflectionContainerTraits<TypeId>;
        using StrOps          = StringOps<ContainerTraits::StringType>;

        ReflectionContainerTraits<TypeId>::StringType ret =
            MakeTypeNameFn<TFn, typename std::remove_extent<T[I]>::type>{}(fn);
        StrOps::Append(ret, "[]");
        return ret;
    }
};

template <typename TFn, typename T, std::size_t I>
struct MakeTypeNameFn<TFn, T[I]>
{
    ReflectionContainerTraits<TypeId>::StringType operator()(TFn fn) const
    {
        using ContainerTraits = ReflectionContainerTraits<TypeId>;
        using StrOps          = StringOps<ContainerTraits::StringType>;

        char iBuf[20] = {0};
        std::snprintf(iBuf, sizeof(iBuf) / sizeof(char), "%llo", I);

        ReflectionContainerTraits<TypeId>::StringType ret = MakeTypeNameFn<TFn, T>{}(fn);
        StrOps::Append(ret, "[");
        StrOps::Append(ret, iBuf);
        StrOps::Append(ret, "]");
        return ret;
    }
};

// Arrays are processed in reading order. char [1][2][3] will be processed <char[2][3], 1>, then <char[3], 2>, etc. If
// left alone, we produce the string char[3][2][1]. Instead, handle some common multi-dimensional arrays explicitly.
template <typename TFn, typename T, std::size_t I, std::size_t J>
struct MakeTypeNameFn<TFn, T[I][J]>
{
    ReflectionContainerTraits<TypeId>::StringType operator()(TFn fn) const
    {
        using ContainerTraits = ReflectionContainerTraits<TypeId>;
        using StrOps          = StringOps<ContainerTraits::StringType>;

        char iBuf[20] = {0};
        std::snprintf(iBuf, sizeof(iBuf) / sizeof(char), "%llo", I);

        char jBuf[20] = {0};
        std::snprintf(jBuf, sizeof(jBuf) / sizeof(char), "%llo", J);

        ReflectionContainerTraits<TypeId>::StringType ret = MakeTypeNameFn<TFn, T>{}(fn);
        StrOps::Append(ret, "[");
        StrOps::Append(ret, iBuf);
        StrOps::Append(ret, "][");
        StrOps::Append(ret, jBuf);
        StrOps::Append(ret, "]");
        return ret;
    }
};

// Arrays are processed in reading order. char [1][2][3] will be processed <char[2][3], 1>, then <char[3], 2>, etc. If
// left alone, we produce the string char[3][2][1]. Instead, handle some common multi-dimensional arrays explicitly.
template <typename TFn, typename T, std::size_t I, std::size_t J, std::size_t K>
struct MakeTypeNameFn<TFn, T[I][J][K]>
{
    ReflectionContainerTraits<TypeId>::StringType operator()(TFn fn) const
    {
        using ContainerTraits = ReflectionContainerTraits<TypeId>;
        using StrOps          = StringOps<ContainerTraits::StringType>;

        char iBuf[20] = {0};
        std::snprintf(iBuf, sizeof(iBuf) / sizeof(char), "%llo", I);

        char jBuf[20] = {0};
        std::snprintf(jBuf, sizeof(jBuf) / sizeof(char), "%llo", J);

        char kBuf[20] = {0};
        std::snprintf(kBuf, sizeof(kBuf) / sizeof(char), "%llo", K);

        ReflectionContainerTraits<TypeId>::StringType ret = MakeTypeNameFn<TFn, T>{}(fn);
        StrOps::Append(ret, "[");
        StrOps::Append(ret, iBuf);
        StrOps::Append(ret, "][");
        StrOps::Append(ret, jBuf);
        StrOps::Append(ret, "][");
        StrOps::Append(ret, kBuf);
        StrOps::Append(ret, "]");
        return ret;
    }
};

template <typename TFn, typename T>
struct MakeTypeNameFn<TFn, T, typename std::enable_if<std::is_lvalue_reference<T>::value>::type>
{
    ReflectionContainerTraits<TypeId>::StringType operator()(TFn fn) const
    {
        using ContainerTraits = ReflectionContainerTraits<TypeId>;
        using StrOps          = StringOps<ContainerTraits::StringType>;

        ReflectionContainerTraits<TypeId>::StringType ret =
            MakeTypeNameFn<TFn, typename std::remove_reference<T>::type>{}(fn);
        StrOps::Append(ret, "&");
        return ret;
    }
};

template <typename TFn, typename T>
struct MakeTypeNameFn<TFn, T, typename std::enable_if<std::is_rvalue_reference<T>::value>::type>
{
    ReflectionContainerTraits<TypeId>::StringType operator()(TFn fn) const
    {
        using ContainerTraits = ReflectionContainerTraits<TypeId>;
        using StrOps          = StringOps<ContainerTraits::StringType>;

        ReflectionContainerTraits<TypeId>::StringType ret =
            MakeTypeNameFn<TFn, typename std::remove_reference<T>::type>{}(fn);
        StrOps::Append(ret, "&&");
        return ret;
    }
};

template <typename TFn, typename T>
struct MakeTypeNameFn<TFn, T, typename std::enable_if<std::is_volatile<T>::value>::type>
{
    ReflectionContainerTraits<TypeId>::StringType operator()(TFn fn) const
    {
        using ContainerTraits = ReflectionContainerTraits<TypeId>;
        using StrOps          = StringOps<ContainerTraits::StringType>;

        ReflectionContainerTraits<TypeId>::StringType ret =
            MakeTypeNameFn<TFn, typename std::remove_volatile<T>::type>{}(fn);
        StrOps::Append(ret, " volatile");
        return ret;
    }
};

template <typename TFn, typename... T>
struct MakeTypeNameTemplateParamFn
{
    ReflectionContainerTraits<TypeId>::StringType operator()(TFn fn) const
    {
        return ReflectionContainerTraits<TypeId>::StringType();
    }
};

template <typename TFn, typename TFirst, typename... TRest>
struct MakeTypeNameTemplateParamFn<TFn, TFirst, TRest...>
{
    ReflectionContainerTraits<TypeId>::StringType operator()(TFn fn) const
    {
        using ContainerTraits = ReflectionContainerTraits<TypeId>;
        using StrOps          = StringOps<ContainerTraits::StringType>;

        ReflectionContainerTraits<TypeId>::StringType ret = MakeTypeNameFn<TFn, TFirst>{}(fn);
        if ENTROPY_CONSTEXPR (sizeof...(TRest) > 0)
        {
            StrOps::Append(ret, ", ");
            StrOps::Append(ret, MakeTypeNameTemplateParamFn<TFn, TRest...>{}(fn));
        }
        return ret;
    }
};

template <typename TFn, template <typename...> class T, typename... Tn>
struct MakeTypeNameFn<TFn, T<Tn...>>
{
    ReflectionContainerTraits<TypeId>::StringType operator()(TFn fn) const
    {
        using ContainerTraits = ReflectionContainerTraits<TypeId>;
        using StrOps          = StringOps<ContainerTraits::StringType>;

        auto baseFn = &MakeTypeNameNoTemplateParamsFromRawName;

        ReflectionContainerTraits<TypeId>::StringType ret = baseFn(typeid(T<Tn...>).name());
        StrOps::Append(ret, "<");
        StrOps::Append(ret, MakeTypeNameTemplateParamFn<TFn, Tn...>{}(fn));
        StrOps::Append(ret, ">");
        return ret;
    }
};

template <typename T, typename = void>
struct MakeTypeName
{
    ReflectionContainerTraits<TypeId>::StringType operator()() const
    {
        auto fn = &MakeTypeNameFromRawName;
        return MakeTypeNameFn<decltype(fn), T>{}(fn);
    }
};

TypeId MakeTypeIdFromTypeName(const ReflectionContainerTraits<TypeId>::StringType& typeName);

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
    using ContainerTraits = ::Entropy::details::ReflectionContainerTraits<TypeId>;

    inline ContainerTraits::StringType operator()() const
    {
        static ContainerTraits::StringType name = Entropy::details::MakeTypeName<T>{}();
        return name;
    }
};

} // namespace Traits

} // namespace Entropy
