// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

// Code normally in a cpp file is inlined because member types can be changed via type traits by the user.

namespace Entropy
{
namespace Reflection
{

AttributeData::AttributeData(DataObject&& dataObj)
    : _dataObj(std::move(dataObj))
{
}

//=======================

template <std::size_t Idx, typename... TAttrTypes>
inline typename std::enable_if<Idx != sizeof...(TAttrTypes), void>::type AttributeContainer::AddAttribute(
    const AttributeCollection<TAttrTypes...>& attr)
{
    using TAttr = Entropy::Traits::UnqualifiedType_t<decltype(attr.template GetAt<Idx>())>;

    const TypeInfo* typeInfo = ReflectTypeAndGetTypeInfo<TAttr>();

    if (typeInfo && typeInfo->CanCopyConstruct())
    {
        DataObject obj = CreateDataObject<TAttr>(attr.template GetAt<Idx>());
        if (ENTROPY_LIKELY(obj != nullptr))
        {
            _attributes[Entropy::Traits::TypeId<TAttr>{}()] = AttributeData(std::move(obj));
        }
    }

    AddAttribute<Idx + 1>(attr);
}

template <typename... TAttrTypes>
void AttributeContainer::AddAttributes(const AttributeCollection<TAttrTypes...>& attr)
{
    AddAttribute<0>(attr);
}

//=======================

void ClassDescription::AddTemplateParameter(const TypeInfo* templateParameter)
{
    _templateParameters.push_back(templateParameter);
}

void ClassDescription::SetBaseClass(const TypeInfo* baseClass) { _baseClassTypeInfo = baseClass; }

void ClassDescription::AddMember(const char* name, MemberDescription&& memberInfo)
{
    _members.emplace(name, std::move(memberInfo));
}

//=======================

ClassTypeInfo::~ClassTypeInfo()
{
    if (_classDesc)
    {
        ContainerTraits::Allocator<ClassDescription> alloc;
        std::allocator_traits<decltype(alloc)>::destroy(alloc, _classDesc);
        std::allocator_traits<decltype(alloc)>::deallocate(alloc, _classDesc, 1);
        _classDesc = nullptr;
    }
}

ClassDescription* ClassTypeInfo::GetOrAddClassDescription()
{
    if (!_classDesc)
    {
        ContainerTraits::Allocator<ClassDescription> alloc;
        _classDesc = std::allocator_traits<decltype(alloc)>::allocate(alloc, 1);
        if (ENTROPY_LIKELY(_classDesc))
        {
            std::allocator_traits<decltype(alloc)>::construct(alloc, _classDesc);
        }
    }

    return _classDesc;
}

} // namespace Reflection
} // namespace Entropy
