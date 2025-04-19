// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

namespace Entropy
{

namespace details
{

template <typename TFunc, typename TMember, typename... TAttrs>
inline constexpr int InvokeMemberOffsetOfFunction(int offset, AttributeTypeCollection<TAttrs...> attrs)
{
    if (TFunc{}.template operator()<TMember>(attrs))
    {
        return offset;
    }
    return -1;
}

#ifdef ENTROPY_RUNTIME_REFLECTION_ENABLED

template <typename TClass, typename TMember>
struct MemberVariableHelper
{
};

#else

template <typename TClass, typename TMember>
struct MemberVariableHelper
{
};

#endif

} // namespace details

} // namespace Entropy
