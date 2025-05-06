// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

#include "Entropy/Reflection/Details/AttributeCollection.h"

namespace Entropy
{

class TypeInfoPtr;

namespace Reflection
{

template <typename TModule>
struct DefaultFillModuleTypeInfo
{
    void HandleType(TModule& module, const TypeInfoPtr& thisTypeInfo) {}

    /// <summary>
    /// Called if the type is a reflected class. If the type is not reflected, this is never called.
    /// </summary>
    template <typename... TAttrTypes>
    void HandleClass(TModule& module, const TypeInfoPtr& thisTypeInfo,
                     const AttributeCollection<TAttrTypes...>& classAttr)
    {
    }

    /// <summary>
    /// Called for each member on a reflected class. If the type is not reflected, this is never called.
    /// </summary>
    template <typename TMember, typename... TAttrTypes>
    void HandleClassMember(TModule& module, const char* memberName, const TypeInfoPtr& memberTypeInfo,
                           const AttributeCollection<TAttrTypes...>& memberAttr)
    {
    }

    /// <summary>
    /// Called for the reflected class's declared base class. If the type is not reflected or has no declared base
    /// class, this is never called.
    /// </summary>
    template <typename TBaseClass>
    void HandleBaseClass(TModule& module, const TypeInfoPtr& baseClassTypeInfo)
    {
    }

    /// <summary>
    /// Called for each template parameter on this type. This is called even if the type is not reflected.
    /// </summary>
    template <typename TTemplateClass>
    void HandleTemplateParameter(TModule& module, const TypeInfoPtr& templateParamTypeInfo)
    {
    }
};

template <typename TModule, typename TType, typename = void>
struct FillModuleTypeInfo : public DefaultFillModuleTypeInfo<TModule>
{
};

} // namespace Reflection
} // namespace Entropy
