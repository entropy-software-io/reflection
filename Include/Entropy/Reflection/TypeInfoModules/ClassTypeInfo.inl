// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#include "Entropy/Reflection/DataObject/DataObjectFactory.h"

namespace Entropy
{
namespace Reflection
{

namespace details
{

template <typename TAttr, typename = void>
struct AllocateAttribute
{
    inline DataObject operator()(TAttr&& attrData) const { return Entropy::DataObjectFactory::Create<TAttr>(attrData); }
};

template <typename TAttr>
struct AllocateAttribute<TAttr, typename std::enable_if<Traits::IsAllocatorMoveConstructible<TAttr>::value>::type>
{
    inline DataObject operator()(TAttr&& attrData) const
    {
        return Entropy::DataObjectFactory::Create<TAttr>(std::move(attrData));
    }
};

} // namespace details

template <std::size_t Idx, typename... TAttrTypes>
inline typename std::enable_if<Idx != sizeof...(TAttrTypes), void>::type AttributeContainer::AddAttribute(
    AttributeCollection<TAttrTypes...>& attr)
{
    using TAttr = Entropy::Traits::UnqualifiedType_t<decltype(attr.template GetAt<Idx>())>;

    const TypeInfo* typeInfo = ReflectTypeAndGetTypeInfo<TAttr>();
    if (ENTROPY_LIKELY(typeInfo))
    {
        DataObject obj = details::AllocateAttribute<TAttr>{}(std::move(attr.template GetAt<Idx>()));
        if (ENTROPY_LIKELY(obj))
        {
            MapOps::AddOrUpdate(_attributes, Entropy::Traits::TypeIdOf<TAttr>{}(), AttributeData(std::move(obj)));
        }
        else
        {
            ENTROPY_LOG_ERROR("Failed to create DataObject for attribute [Attr Name: " << Traits::TypeNameOf<TAttr>{}()
                                                                                       << "]");
        }
    }

    AddAttribute<Idx + 1>(attr);
}

template <typename... TAttrTypes>
inline void AttributeContainer::AddAttributes(AttributeCollection<TAttrTypes...>&& attr)
{
    AddAttribute<0>(attr);
}

} // namespace Reflection
} // namespace Entropy
