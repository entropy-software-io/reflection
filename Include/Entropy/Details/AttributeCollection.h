// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

#include <type_traits>

namespace Entropy
{

template <typename... TAttrTypes>
struct AttributeTypeCollection
{
private:
    template <typename TType, typename TThisType>
    static constexpr bool IsSameTypeImpl()
    {
        return std::is_same_v<TType, TThisType>;
    }

    template <typename TType>
    static constexpr bool HasTypeImpl()
    {
        return false;
    }

    template <typename TType, typename TThisType, typename... TOtherTypes>
    static constexpr bool HasTypeImpl()
    {
        if constexpr (IsSameTypeImpl<TType, TThisType>())
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
    constexpr bool HasType() const
    {
        return HasTypeImpl<TType, TAttrTypes...>();
    }

    template <typename TType>
    static constexpr bool HasAttributeOfType()
    {
        return HasTypeImpl<TType, TAttrTypes...>();
    }
};

namespace details
{

template <typename... TTypes>
constexpr AttributeTypeCollection<TTypes...> MakeAttributeCollection(const TTypes&...)
{
    return AttributeTypeCollection<TTypes...>();
}

} // namespace details

} // namespace Entropy