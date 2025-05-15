// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

#include "Entropy/Core/Details/MapOps.h"
#include "Entropy/Core/Details/TypeId.h"
#include "Entropy/Core/Details/VectorOps.h"
#include "Entropy/Reflection/DataObject/DataObject.h"
#include "Entropy/Reflection/Details/AttributeCollection.h"
#include "Entropy/Reflection/Details/TypeTraits.h"
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

    inline const TypeInfo* GetTypeInfo() const { return _dataObj.GetTypeInfo(); }

    template <typename T>
    inline bool IsType() const
    {
        return _dataObj.IsExactType<T>();
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
public:
    inline const MapOps::MapType<TypeId, AttributeData>& GetAllAttributes() const { return _attributes; }

    template <typename T>
    inline const T* TryGetAttribute() const
    {
        TypeId typeId = Traits::TypeIdOf<T>{}();

        const AttributeData* data = nullptr;
        if (MapOps::TryGetValue(_attributes, typeId, &data))
        {
            return data->GetData<T>();
        }

        return nullptr;
    }

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

    MapOps::MapType<TypeId, AttributeData> _attributes;

    template <typename, typename, typename>
    friend struct FillModuleTypeInfo;
};

/// <summary>
/// Member information. Accessible through class type info
/// </summary>
class MemberDescription : public AttributeContainer
{
public:
    MemberDescription(const char* memberName, const TypeInfo* memberType)
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
class ClassDescription : public AttributeContainer
{
public:
    inline bool IsReflectedClass() const { return _isReflectedClass; }
    inline const VectorOps::VectorType<const TypeInfo*>& GetTemplateParameters() const { return _templateParameters; }
    inline const TypeInfo* GetBaseClassTypeInfo() const { return _baseClassTypeInfo; }
    inline const MapOps::MapType<const char*, MemberDescription>& GetMembers() const { return _members; }

private:
    void AddTemplateParameter(const TypeInfo* templateParameter);
    void SetBaseClass(const TypeInfo* baseClass);
    void AddMember(const char* name, MemberDescription&& memberInfo);
    inline void SetIsReflectedClass(bool isReflectedClass) { _isReflectedClass = isReflectedClass; }

    const TypeInfo* _baseClassTypeInfo = nullptr;
    MapOps::MapType<const char*, MemberDescription> _members{};
    VectorOps::VectorType<const TypeInfo*> _templateParameters{};
    bool _isReflectedClass = false;

    template <typename, typename, typename>
    friend struct FillModuleTypeInfo;
};

/// <summary>
/// Contains class hierarchy and member information
/// </summary>
class ClassTypeInfo
{
public:
    ~ClassTypeInfo();

    inline bool IsReflectedClass() const { return (_classDesc != nullptr); }
    inline const ClassDescription* GetClassDescription() const { return _classDesc; }

private:
    ClassDescription* GetOrAddClassDescription();

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
    template <typename... TAttrTypes>
    void HandleClass(ClassTypeInfo& module, const TypeInfo* thisTypeInfo,
                     AttributeCollection<TAttrTypes...>&& classAttr)
    {
        module.GetOrAddClassDescription()->AddAttributes(std::move(classAttr));
    }

    template <typename TMember, typename... TAttrTypes>
    void HandleClassMember(ClassTypeInfo& module, const char* memberName, const TypeInfo* memberTypeInfo,
                           AttributeCollection<TAttrTypes...>&& memberAttr)
    {
        MemberDescription memberInfo(memberName, memberTypeInfo);
        memberInfo.AddAttributes(std::move(memberAttr));

        module.GetOrAddClassDescription()->AddMember(memberName, std::move(memberInfo));
    }

    template <typename TBaseClass>
    void HandleBaseClass(ClassTypeInfo& module, const TypeInfo* baseClassTypeInfo)
    {
        module.GetOrAddClassDescription()->SetBaseClass(baseClassTypeInfo);
    }

    template <typename TTemplateClass>
    void HandleTemplateParameter(ClassTypeInfo& module, const TypeInfo* templateParamTypeInfo)
    {
        // We get template parameters for non-reflected types too. Don't waste space recording if the type isn't
        // reflected.
        if (module.IsReflectedClass())
        {
            module.GetOrAddClassDescription()->AddTemplateParameter(templateParamTypeInfo);
        }
    }
};

} // namespace Reflection
} // namespace Entropy

#include "ClassTypeInfo.inl"
