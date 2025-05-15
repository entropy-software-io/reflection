// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#include "Entropy/Reflection/TypeInfoModules/ClassTypeInfo.h"
#include "Entropy/Core/Details/AllocatorTraits.h"

namespace Entropy
{
namespace Reflection
{

 AttributeData::AttributeData(DataObject&& dataObj)
    : _dataObj(std::move(dataObj))
{
}

//=======================

 void ClassDescription::AddTemplateParameter(const TypeInfo* templateParameter)
{
    VectorOps::Add(_templateParameters, templateParameter);
}

 void ClassDescription::SetBaseClass(const TypeInfo* baseClass) { _baseClassTypeInfo = baseClass; }

 void ClassDescription::AddMember(const char* name, MemberDescription&& memberInfo)
{
    MapOps::AddUnique(_members, name, std::move(memberInfo));
}

//=======================

 ClassTypeInfo::~ClassTypeInfo()
{
    if (_classDesc)
    {
        AllocatorOps::DestroyInstance(_classDesc);
        _classDesc = nullptr;
    }
}

 ClassDescription* ClassTypeInfo::GetOrAddClassDescription()
{
    if (!_classDesc)
    {
        _classDesc = AllocatorOps::CreateInstance<ClassDescription>();
    }

    return _classDesc;
}

} // namespace Reflection
} // namespace Entropy
