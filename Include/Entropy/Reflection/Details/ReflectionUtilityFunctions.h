// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

namespace Entropy
{

namespace details
{

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
