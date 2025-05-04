// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

#include "Entropy/Core/Details/Defines.h"
#include <tuple>
#include <type_traits>

namespace Entropy
{

template <typename... TAttrTypes>
struct AttributeCollection
{
private:
    template <typename TType, typename TThisType>
    static ENTROPY_CONSTEXPR bool IsSameTypeImpl()
    {
        return std::is_same<TType, TThisType>::value;
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
    template <typename... UAttrTypes>
    constexpr AttributeCollection(UAttrTypes&&... attrs)
        : _attrs(std::forward<UAttrTypes>(attrs)...)
    {
    }

    constexpr AttributeCollection() {}

    template <std::size_t Idx>
    auto GetAt()
    {
        return std::get<Idx>(_attrs);
    }

    template <std::size_t Idx>
    auto GetAt() const
    {
        return std::get<Idx>(_attrs);
    }

private:
    std::tuple<TAttrTypes...> _attrs;
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

    constexpr ReflectionMemberMetaData(const char* memberName, AttributeCollection<TAttrTypes...>&& attrs)
        : memberName(memberName)
        , attributes(std::move(attrs))
    {
    }

    template <typename TType>
    constexpr static bool HasAttributeOfType()
    {
        return AttributeCollection<TAttrTypes...>::template HasAttributeOfType<TType>();
    }

    const char* memberName = nullptr;
    AttributeCollection<TAttrTypes...> attributes;
};

namespace details
{

template <typename... TTypes>
constexpr AttributeCollection<TTypes...> MakeAttributeCollection(TTypes&&... vals)
{
    return AttributeCollection<TTypes...>(std::forward<TTypes>(vals)...);
}

template <typename... TTypes>
constexpr ReflectionMemberMetaData<TTypes...> MakeReflectionMemberMetaData(const char* memberName, TTypes&&... types)
{
    return ReflectionMemberMetaData<TTypes...>(memberName, MakeAttributeCollection(std::forward<TTypes>(types)...));
}

} // namespace details

} // namespace Entropy