// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

#include "Entropy/Reflection/Details/AttributeCollection.h"
#include "Entropy/Reflection/Details/ContainerTypes.h"
#include "Entropy/Reflection/Details/TypeTraits.h"
#include "Entropy/Reflection/RuntimeReflectionMethods.h"
#include "TypeInfoModule.h"

namespace Entropy
{

class TypeInfo;

template <typename T>
const TypeInfo* ReflectTypeAndGetTypeInfo() noexcept;

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
    void AddMember(const char* name, MemberTypeInfo&& memberInfo);
    void SetBaseClass(const TypeInfo* baseClass);

    inline const TypeInfo* GetBaseClassTypeInfo() const { return _baseClassTypeInfo; }
    inline const ContainerTraits::MapType<const char*, MemberTypeInfo>& GetMembers() const { return _members; }

private:
    const TypeInfo* _baseClassTypeInfo = nullptr;
    ContainerTraits::MapType<const char*, MemberTypeInfo> _members{};
};

//----------------

/// <summary>
/// Called once per type to initialize module type info.
/// </summary>
template <typename T>
struct FillReflectionInfo<ClassTypeInfo, T, typename std::enable_if<!Traits::IsReflectedType<T>::value>::type>
{
    void operator()(ClassTypeInfo& classTypeInfo) const {}
};

template <typename T>
struct FillReflectionInfo<ClassTypeInfo, T, typename std::enable_if<Traits::IsReflectedType<T>::value>::type>
{
    struct GatherMembers
    {
        GatherMembers(ClassTypeInfo* classTypeInfo)
            : _classTypeInfo(classTypeInfo)
        {
        }

        template <typename TMemberType, typename... TAttrTypes>
        void operator()(const char* memberName, const Entropy::AttributeTypeCollection<TAttrTypes...>& attributes)
        {
            const TypeInfo* memberTypeInfo = ReflectTypeAndGetTypeInfo<TMemberType>();

            MemberTypeInfo memberInfo(memberName, memberTypeInfo);

            _classTypeInfo->AddMember(memberName, std::move(memberInfo));
        }

    private:
        ClassTypeInfo* _classTypeInfo = nullptr;
    };

    void operator()(ClassTypeInfo& classTypeInfo) const
    {
        ForEachReflectedMemberType<false /* IncludeSubclasses*/, T>(GatherMembers(&classTypeInfo));

        if ENTROPY_CONSTEXPR (Traits::HasBaseClass<T>::value)
        {
            const TypeInfo* baseClassType = ReflectTypeAndGetTypeInfo<Traits::BaseClassOf_t<T>>();
            classTypeInfo.SetBaseClass(baseClassType);
        }
    }
};

} // namespace Reflection
} // namespace Entropy

#include "ClassTypeInfo.inl"
