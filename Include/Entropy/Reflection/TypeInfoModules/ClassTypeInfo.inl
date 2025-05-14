// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

// Code normally in a cpp file is inlined because member types can be changed via type traits by the user.

namespace Entropy
{
namespace Reflection
{

inline AttributeData::AttributeData(DataObject&& dataObj)
    : _dataObj(std::move(dataObj))
{
}

//=======================

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
                _attributes[Entropy::Traits::TypeIdOf<TAttr>{}()] = AttributeData(std::move(obj));
            }
        }
        else if (typeInfo->CanCopyConstruct())
        {
            DataObject obj = DataObjectFactory::Create<TAttr>(attr.template GetAt<Idx>());
            if (ENTROPY_LIKELY(obj != nullptr))
            {
                _attributes[Entropy::Traits::TypeIdOf<TAttr>{}()] = AttributeData(std::move(obj));
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

//=======================

inline void ClassDescription::AddTemplateParameter(const TypeInfo* templateParameter)
{
    _templateParameters.push_back(templateParameter);
}

inline void ClassDescription::SetBaseClass(const TypeInfo* baseClass) { _baseClassTypeInfo = baseClass; }

inline void ClassDescription::AddMember(const char* name, MemberDescription&& memberInfo)
{
    _members.emplace(name, std::move(memberInfo));
}

//=======================

inline ClassTypeInfo::~ClassTypeInfo()
{
    if (_classDesc)
    {
        AllocatorOps::DestroyInstance(_classDesc);
        _classDesc = nullptr;
    }
}

inline ClassDescription* ClassTypeInfo::GetOrAddClassDescription()
{
    if (!_classDesc)
    {
        _classDesc = AllocatorOps::CreateInstance<ClassDescription>();
    }

    return _classDesc;
}

} // namespace Reflection
} // namespace Entropy
