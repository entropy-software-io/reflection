// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

// Code normally in a cpp file is inlined because we need to ensure the correct TypeInfo layout, which can be
// manipulated externally.

namespace Entropy
{
namespace Reflection
{

void ClassTypeInfo::SetBaseClass(const TypeInfo* baseClass) { _baseClassTypeInfo = baseClass; }

void ClassTypeInfo::AddMember(const char* name, MemberTypeInfo&& memberInfo)
{
    _members.emplace(name, std::move(memberInfo));
}

} // namespace Reflection
} // namespace Entropy
