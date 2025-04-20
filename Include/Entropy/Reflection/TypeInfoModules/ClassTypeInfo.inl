// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

// Code normally in a cpp file is inlined because member types can be changed via type traits by the user.

namespace Entropy
{
namespace Reflection
{

void ClassTypeInfo::AddTemplateParameter(const TypeInfo* templateParameter)
{
    _templateParameters.push_back(templateParameter);
}

void ClassTypeInfo::SetBaseClass(const TypeInfo* baseClass) { _baseClassTypeInfo = baseClass; }

void ClassTypeInfo::AddMember(const char* name, MemberTypeInfo&& memberInfo)
{
    _members.emplace(name, std::move(memberInfo));
}

} // namespace Reflection
} // namespace Entropy
