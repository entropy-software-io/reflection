// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

#include "Entropy/Reflection/Details/TypeTraits.h"
#include "TypeInfo.h"

namespace Entropy
{

template <typename T>
const TypeInfo* ReflectTypeAndGetTypeInfo() noexcept;

namespace details
{

template <typename TModule, typename TType>
struct FillModuleTypeTemplateParameters
{
    using ModuleHandlerType = Entropy::Reflection::FillModuleTypeInfo<TModule, TType>;

    void operator()(ModuleHandlerType& handler, TModule& module) {}
};

template <typename TModule, template <typename...> class TType, typename... Tn>
struct FillModuleTypeTemplateParameters<TModule, TType<Tn...>>
{
    using ModuleHandlerType = Entropy::Reflection::FillModuleTypeInfo<TModule, TType<Tn...>>;

    template <typename U>
    void HandleTemplateParameters(ModuleHandlerType& handler, TModule& module)
    {
        const TypeInfo* templateParamTypeInfo = ReflectTypeAndGetTypeInfo<U>();
        handler.template HandleTemplateParameter<U>(module, templateParamTypeInfo);
    }

    template <typename U1, typename U2, typename... Un>
    void HandleTemplateParameters(ModuleHandlerType& handler, TModule& module)
    {
        HandleTemplateParameters<U1>(handler, module);

        HandleTemplateParameters<U2, Un...>(handler, module);
    }

    void operator()(ModuleHandlerType& handler, TModule& module) { HandleTemplateParameters<Tn...>(handler, module); }
};

//===================

template <typename TModule, typename TType, typename = void>
struct FillModuleTypeBaseClass
{
    void operator()(Entropy::Reflection::FillModuleTypeInfo<TModule, TType>& handler, TModule& module) {}
};

template <typename TModule, typename TType>
struct FillModuleTypeBaseClass<TModule, TType, typename std::enable_if<Traits::HasBaseClass<TType>::value>::type>
{
    void operator()(Entropy::Reflection::FillModuleTypeInfo<TModule, TType>& handler, TModule& module) const
    {
        const TypeInfo* baseClassTypeInfo = ReflectTypeAndGetTypeInfo<Traits::BaseClassOf_t<TType>>();

        handler.template HandleBaseClass<Traits::BaseClassOf_t<TType>>(module, baseClassTypeInfo);
    }
};

//===================

template <typename TModule, typename TType, typename = void>
struct FillModuleTypeClassMembers
{
    using ModuleHandlerType = Entropy::Reflection::FillModuleTypeInfo<TModule, TType>;

    void operator()(ModuleHandlerType& handler, TModule& module) {}
};

template <typename TModule, typename TType>
struct FillModuleTypeClassMembers<TModule, TType, typename std::enable_if<Traits::IsReflectedType<TType>::value>::type>
{
    using ModuleHandlerType = Entropy::Reflection::FillModuleTypeInfo<TModule, TType>;

    struct HandleMember
    {
        HandleMember(ModuleHandlerType* handler, TModule* module)
            : _handler(handler)
            , _module(module)
        {
        }

        template <typename TMember, typename... TAttrTypes>
        void operator()(const char* memberName, const AttributeTypeCollection<TAttrTypes...>& memberAttr)
        {
            const TypeInfo* typeInfo = ReflectTypeAndGetTypeInfo<TMember>();

            _handler->template HandleClassMember<TMember, TAttrTypes...>(*_module, memberName, typeInfo, memberAttr);
        }

    private:
        ModuleHandlerType* _handler = nullptr;
        TModule* _module            = nullptr;
    };

    void operator()(ModuleHandlerType& handler, TModule& module) const
    {
        ForEachReflectedMemberType<false /* IncludeSubclasses */, TType>(HandleMember(&handler, &module));
    }
};

//===================

template <typename TType, typename TModuleTypes>
struct FillModuleTypes;

template <typename TType>
struct FillModuleTypes<TType, std::tuple<>>
{
    void operator()(TypeInfo& typeInfo) const {}
};

template <typename TType, typename TFirstModule, typename... TOtherModules>
struct FillModuleTypes<TType, std::tuple<TFirstModule, TOtherModules...>>
{
    using ModuleFillerType = Entropy::Reflection::FillModuleTypeInfo<TFirstModule, TType>;

    void operator()(TypeInfo& typeInfo) const
    {
        ModuleFillerType filler;

        TFirstModule& module = typeInfo.Get<TFirstModule>();

        // Base Type
        filler.HandleType(module);

        // Template Parameters
        FillModuleTypeTemplateParameters<TFirstModule, TType>{}(filler, module);

        // Base Type
        FillModuleTypeBaseClass<TFirstModule, TType>{}(filler, module);

        // Members
        FillModuleTypeClassMembers<TFirstModule, TType>{}(filler, module);

        // Handle the rest of the modules
        FillModuleTypes<TType, std::tuple<TOtherModules...>>{}(typeInfo);
    }
};

//---------------

template <typename T>
TypeInfo MakeSingleTypeInfo()
{
    TypeInfo typeInfo{};

    FillModuleTypes<T, TypeInfo::ModuleTypes>{}(typeInfo);

    return typeInfo;
}

} // namespace details

template <typename T>
const TypeInfo* ReflectTypeAndGetTypeInfo() noexcept
{
    static TypeInfo typeInfo = details::MakeSingleTypeInfo<T>();
    return &typeInfo;
}

template <typename T>
void ReflectType()
{
    ReflectTypeAndGetTypeInfo<T>();
}

} // namespace Entropy
