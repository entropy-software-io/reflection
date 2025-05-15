// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

namespace Entropy
{
namespace Reflection
{

template <std::size_t Idx, typename... TAttrTypes>
inline typename std::enable_if<Idx != sizeof...(TAttrTypes), void>::type AttributeContainer::AddAttribute(
    AttributeCollection<TAttrTypes...>& attr)
{
    using TAttr = Entropy::Traits::UnqualifiedType_t<decltype(attr.template GetAt<Idx>())>;

    const TypeInfo* typeInfo = ReflectTypeAndGetTypeInfo<TAttr>();

    if (typeInfo)
    {
        if (typeInfo->CanMoveConstruct())
        {
            DataObject obj = DataObjectFactory::Create<TAttr>(std::move(attr.template GetAt<Idx>()));
            if (ENTROPY_LIKELY(obj != nullptr))
            {
                MapOps::AddOrUpdate(_attributes, Entropy::Traits::TypeIdOf<TAttr>{}(), AttributeData(std::move(obj)));
            }
        }
        else if (typeInfo->CanCopyConstruct())
        {
            DataObject obj = DataObjectFactory::Create<TAttr>(attr.template GetAt<Idx>());
            if (ENTROPY_LIKELY(obj != nullptr))
            {
                MapOps::AddOrUpdate(_attributes, Entropy::Traits::TypeIdOf<TAttr>{}(), AttributeData(std::move(obj)));
            }
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
