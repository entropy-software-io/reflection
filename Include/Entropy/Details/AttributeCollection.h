// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

#include "Entropy/Details/Defines.h"
#include <type_traits>

namespace Entropy
{

template <typename... TAttrTypes>
struct AttributeTypeCollection
{
private:
    template <typename TType, typename TThisType>
    static ENTROPY_CONSTEXPR bool IsSameTypeImpl()
    {
        return std::is_same_v<TType, TThisType>;
    }

    template <typename TType>
    static ENTROPY_CONSTEXPR bool HasTypeImpl()
    {
        return false;
    }

    template <typename TType, typename TThisType, typename... TOtherTypes>
    static ENTROPY_CONSTEXPR bool HasTypeImpl()
    {
        if ENTROPY_CONSTEXPR (IsSameTypeImpl<TType, TThisType>())
        {
            return true;
        }
        else
        {
            return HasTypeImpl<TType, TOtherTypes...>();
        }
    }

public:
    constexpr AttributeTypeCollection() {}

    template <typename TType>
    ENTROPY_CONSTEXPR bool HasType() const
    {
        return HasTypeImpl<TType, TAttrTypes...>();
    }

    template <typename TType>
    static ENTROPY_CONSTEXPR bool HasAttributeOfType()
    {
        return HasTypeImpl<TType, TAttrTypes...>();
    }
};

template <typename... TAttrTypes>
struct ReflectionMemberMetaData
{
    constexpr ReflectionMemberMetaData()
        : memberName(nullptr)
    {
    }

    constexpr ReflectionMemberMetaData(const char* memberName)
        : memberName(memberName)
    {
    }

    constexpr ReflectionMemberMetaData(const char* memberName, AttributeTypeCollection<TAttrTypes...>&& attrs)
        : memberName(memberName)
        , attributes(std::move(attrs))
    {
    }

    template <typename TType>
    constexpr static bool HasAttributeOfType()
    {
        return AttributeTypeCollection<TAttrTypes...>::template HasAttributeOfType<TType>();
    }

    const char* memberName = nullptr;
    AttributeTypeCollection<TAttrTypes...> attributes;
};

namespace details
{

template <typename... TTypes>
constexpr AttributeTypeCollection<TTypes...> MakeAttributeCollection(const TTypes&...)
{
    return AttributeTypeCollection<TTypes...>();
}

template <typename... TTypes>
constexpr ReflectionMemberMetaData<TTypes...> MakeReflectionMemberMetaData(const char* memberName,
                                                                           const TTypes&... types)
{
    return ReflectionMemberMetaData<TTypes...>(memberName, MakeAttributeCollection(types...));
}

} // namespace details

} // namespace Entropy