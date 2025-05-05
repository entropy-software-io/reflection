// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

#include "Entropy/Reflection/TypeInfoModules/TypeInfoModule.h"
#include "MyCustomType.h"

namespace Entropy
{
namespace Reflection
{

/// <summary>
/// Custom module to demonstrate how to add your own metadata to types
/// </summary>
class CustomTypeInfo
{
public:
    inline bool IsMyCustomType() const { return _isMyCustomType; }
    inline bool IsAssignableToMyCustomType() const { return _isAssignableToMyCustomType; }

private:
    void SetIsMyCustomType(bool isMyCustomType) { _isMyCustomType = isMyCustomType; }
    void SetIsAssignableToMyCustomType(bool isAssignable) { _isAssignableToMyCustomType = isAssignable; }

    bool _isMyCustomType             = false;
    bool _isAssignableToMyCustomType = false;

    template <typename, typename, typename>
    friend struct FillModuleTypeInfo;
};

//----------------

/// <summary>
/// Initializes the BasicTypeInfo module. Called once per reflected type
/// </summary>
template <typename T>
struct FillModuleTypeInfo<CustomTypeInfo, T> : public DefaultFillModuleTypeInfo<CustomTypeInfo>
{
    void HandleType(CustomTypeInfo& module, TypeInfoPtr thisTypeInfo)
    {
        module.SetIsMyCustomType(std::is_same<T, MyCustomType>::value);
        module.SetIsAssignableToMyCustomType(std::is_assignable<MyCustomType, T>::value);
    }
};

} // namespace Reflection
} // namespace Entropy
