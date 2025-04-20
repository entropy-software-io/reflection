// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

#include "Entropy/Reflection/Details/AttributeCollection.h"
#include "Entropy/Reflection/Details/ContainerTypes.h"
#include "Entropy/Reflection/Details/TypeTraits.h"
#include "TypeInfoModule.h"

namespace Entropy
{
namespace Reflection
{

/// <summary>
/// Member information. Accessible through class type info
/// </summary>
class MemberTypeInfo
{
private:
    using ContainerTraits = details::ReflectionContainerTraits<MemberTypeInfo>;

public:
    MemberTypeInfo(const char* memberName, const TypeInfo* memberType)
        : _memberName(memberName)
        , _memberType(memberType)
    {
    }

    inline const char* GetMemberName() const { return _memberName; }
    inline const TypeInfo* GetMemberType() const { return _memberType; }

private:
    const char* _memberName;
    const TypeInfo* _memberType;
};

/// <summary>
/// Contains class hierarchy and member information
/// </summary>
class ClassTypeInfo
{
private:
    using ContainerTraits = details::ReflectionContainerTraits<ClassTypeInfo>;

public:
    inline bool IsReflectedClass() const { return _isReflectedClass; }
    inline const ContainerTraits::VectorType<const TypeInfo*> GetTemplateParameters() const
    {
        return _templateParameters;
    }
    inline const TypeInfo* GetBaseClassTypeInfo() const { return _baseClassTypeInfo; }
    inline const ContainerTraits::MapType<const char*, MemberTypeInfo>& GetMembers() const { return _members; }

private:
    void AddTemplateParameter(const TypeInfo* templateParameter);
    void SetBaseClass(const TypeInfo* baseClass);
    void AddMember(const char* name, MemberTypeInfo&& memberInfo);
    void SetIsReflectedClass(bool isReflectedClass) { _isReflectedClass = isReflectedClass; }

    const TypeInfo* _baseClassTypeInfo = nullptr;
    ContainerTraits::MapType<const char*, MemberTypeInfo> _members{};
    ContainerTraits::VectorType<const TypeInfo*> _templateParameters{};
    bool _isReflectedClass = false;

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
    void HandleType(ClassTypeInfo& module) { module.SetIsReflectedClass(Traits::IsReflectedType<T>::value); }

    template <typename TMember, typename... TAttrTypes>
    void HandleClassMember(ClassTypeInfo& module, const char* memberName, const TypeInfo* memberTypeInfo,
                           const AttributeTypeCollection<TAttrTypes...>& memberAttr)
    {
        MemberTypeInfo memberInfo(memberName, memberTypeInfo);

        module.AddMember(memberName, std::move(memberInfo));
    }

    template <typename TBaseClass>
    void HandleBaseClass(ClassTypeInfo& module, const TypeInfo* baseClassTypeInfo)
    {
        module.SetBaseClass(baseClassTypeInfo);
    }

    template <typename TTemplateClass>
    void HandleTemplateParameter(ClassTypeInfo& module, const TypeInfo* templateParamTypeInfo)
    {
        module.AddTemplateParameter(templateParamTypeInfo);
    }
};

} // namespace Reflection
} // namespace Entropy

#include "ClassTypeInfo.inl"
