// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

namespace Entropy
{
namespace Reflection
{

/// <summary>
/// Declare an instance of this for each type
/// </summary>
template <typename TModule, typename TType, typename = void>
struct FillReflectionInfo
{
    // template <typename T>
    // void operator()(TModule& typeInfoModule) const {}
};

} // namespace Reflection
} // namespace Entropy
