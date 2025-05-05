// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

#include "Entropy/Reflection/DataObject/DataObject.h"
#include "Entropy/Reflection/Details/AttributeCollection.h"
#include "Entropy/Reflection/Details/ContainerTypes.h"
#include "Entropy/Reflection/Details/TypeId.h"
#include "Entropy/Reflection/Details/TypeTraits.h"
#include "Entropy/Reflection/TypeInfoPtr.h"
#include "TypeInfoModule.h"

namespace Entropy
{
namespace Reflection
{

struct AttributeData
{
    AttributeData() {}
    AttributeData(DataObject&& dataObj);

    AttributeData(const AttributeData&) = default;
    AttributeData(AttributeData&&)      = default;

    inline TypeInfoPtr GetTypeInfo() const { return _dataObj.GetTypeInfo(); }

    template <typename T>
    inline bool IsType() const
    {
        return _dataObj.IsType<T>();
    }

    template <typename T>
    inline const T* TryGetData() const
    {
        if (IsType<T>())
        {
            return &_dataObj.GetData<T>();
        }

        return nullptr;
    }

    template <typename T>
    inline const T* GetData() const
    {
        return &_dataObj.GetData<T>();
    }

    AttributeData& operator=(const AttributeData& other) = default;
    AttributeData& operator=(AttributeData&& other)      = default;

    inline bool operator==(const AttributeData& other) const
    {
        return _dataObj.GetTypeInfo() == other._dataObj.GetTypeInfo();
    }

private:
    DataObject _dataObj;
};

class AttributeContainer
{
private:
    using ContainerTraits = Entropy::details::ReflectionContainerTraits<AttributeContainer>;

public:
    inline const ContainerTraits::MapType<TypeId, AttributeData>& GetAllAttributes() const { return _attributes; }

private:
    template <std::size_t Idx = 0, typename... TAttrTypes>
    inline typename std::enable_if<Idx == sizeof...(TAttrTypes), void>::type AddAttribute(
        AttributeCollection<TAttrTypes...>&)
    {
    }

    template <std::size_t Idx = 0, typename... TAttrTypes>
    inline typename std::enable_if<Idx != sizeof...(TAttrTypes), void>::type AddAttribute(
        AttributeCollection<TAttrTypes...>&);

    template <typename... TAttrTypes>
    inline void AddAttributes(AttributeCollection<TAttrTypes...>&& attr);

    ContainerTraits::MapType<TypeId, AttributeData> _attributes;

    template <typename, typename, typename>
    friend struct FillModuleTypeInfo;
};

/// <summary>
/// Member information. Accessible through class type info
/// </summary>
class MemberDescription : public AttributeContainer
{
private:
    using ContainerTraits = Entropy::details::ReflectionContainerTraits<MemberDescription>;

public:
    MemberDescription(const char* memberName, TypeInfoPtr memberType)
        : _memberName(memberName)
        , _memberType(memberType)
    {
    }

    inline const char* GetMemberName() const { return _memberName; }
    inline TypeInfoPtr GetMemberType() const { return _memberType; }

private:
    const char* _memberName;
    TypeInfoPtr _memberType;
};

/// <summary>
/// Contains class hierarchy and member information
/// </summary>
class ClassDescription : public AttributeContainer
{
private:
    using ContainerTraits = Entropy::details::ReflectionContainerTraits<ClassDescription>;

public:
    inline bool IsReflectedClass() const { return _isReflectedClass; }
    inline const ContainerTraits::VectorType<TypeInfoPtr> GetTemplateParameters() const { return _templateParameters; }
    inline TypeInfoPtr GetBaseClassTypeInfo() const { return _baseClassTypeInfo; }
    inline const ContainerTraits::MapType<const char*, MemberDescription>& GetMembers() const { return _members; }

private:
    void AddTemplateParameter(TypeInfoPtr templateParameter);
    void SetBaseClass(TypeInfoPtr baseClass);
    void AddMember(const char* name, MemberDescription&& memberInfo);
    void SetIsReflectedClass(bool isReflectedClass) { _isReflectedClass = isReflectedClass; }

    TypeInfoPtr _baseClassTypeInfo = nullptr;
    ContainerTraits::MapType<const char*, MemberDescription> _members{};
    ContainerTraits::VectorType<TypeInfoPtr> _templateParameters{};
    bool _isReflectedClass = false;

    template <typename, typename, typename>
    friend struct FillModuleTypeInfo;
};

/// <summary>
/// Contains class hierarchy and member information
/// </summary>
class ClassTypeInfo
{
private:
    using ContainerTraits = Entropy::details::ReflectionContainerTraits<ClassTypeInfo>;

public:
    ~ClassTypeInfo();

    inline bool IsReflectedClass() const { return (_classDesc != nullptr); }
    inline const ClassDescription* GetClassDescription() const { return _classDesc; }

private:
    inline ClassDescription* GetOrAddClassDescription();

    ClassDescription* _classDesc = nullptr;

    template <typename, typename, typename>
    friend struct FillModuleTypeInfo;
};

//----------------

/// <summary>
/// Called once per type to initialize module type info.
/// </summary>
template <typename T>
struct FillModuleTypeInfo<ClassTypeInfo, T> : public DefaultFillModuleTypeInfo<ClassTypeInfo>
{
    void HandleType(ClassTypeInfo& module, TypeInfoPtr thisTypeInfo) {}

    template <typename... TAttrTypes>
    void HandleClass(ClassTypeInfo& module, TypeInfoPtr thisTypeInfo, AttributeCollection<TAttrTypes...>&& classAttr)
    {
        module.GetOrAddClassDescription()->AddAttributes(std::move(classAttr));
    }

    template <typename TMember, typename... TAttrTypes>
    void HandleClassMember(ClassTypeInfo& module, const char* memberName, TypeInfoPtr memberTypeInfo,
                           AttributeCollection<TAttrTypes...>&& memberAttr)
    {
        MemberDescription memberInfo(memberName, memberTypeInfo);
        memberInfo.AddAttributes(std::move(memberAttr));

        module.GetOrAddClassDescription()->AddMember(memberName, std::move(memberInfo));
    }

    template <typename TBaseClass>
    void HandleBaseClass(ClassTypeInfo& module, TypeInfoPtr baseClassTypeInfo)
    {
        module.GetOrAddClassDescription()->SetBaseClass(baseClassTypeInfo);
    }

    template <typename TTemplateClass>
    void HandleTemplateParameter(ClassTypeInfo& module, TypeInfoPtr templateParamTypeInfo)
    {
        module.GetOrAddClassDescription()->AddTemplateParameter(templateParamTypeInfo);
    }
};

} // namespace Reflection
} // namespace Entropy
