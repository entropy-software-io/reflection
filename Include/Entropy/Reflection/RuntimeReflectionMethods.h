// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

#include "Entropy/Reflection/Details/TypeId.h"
#include "Entropy/Reflection/Details/TypeTraits.h"
#include "TypeInfo.h"

namespace Entropy
{

template <typename T>
const TypeInfo* ReflectTypeAndGetTypeInfo() noexcept;

namespace details
{

template <typename TModule, typename TType, typename = void>
struct FillModuleTypeClass
{
    void operator()(Entropy::Reflection::FillModuleTypeInfo<TModule, TType>& handler, TModule& module,
                    const TypeInfo* typeInfo)
    {
    }
};

template <typename TModule, typename TType>
struct FillModuleTypeClass<TModule, TType, typename std::enable_if<Traits::IsReflectedType<TType>::value>::type>
{
    using ModuleHandlerType = Entropy::Reflection::FillModuleTypeInfo<TModule, TType>;

    struct HandleClass
    {
        HandleClass(ModuleHandlerType* handler, TModule* module, const TypeInfo* typeInfo)
            : _handler(handler)
            , _module(module)
            , _typeInfo(typeInfo)
        {
        }

        template <typename TClass, typename... TAttrTypes>
        void operator()(const AttributeTypeCollection<TAttrTypes...>& classAttr)
        {
            _handler->template HandleClass<TAttrTypes...>(*_module, _typeInfo, classAttr);
        }

    private:
        ModuleHandlerType* _handler = nullptr;
        TModule* _module            = nullptr;
        const TypeInfo* _typeInfo   = nullptr;
    };

    void operator()(ModuleHandlerType& handler, TModule& module, const TypeInfo* typeInfo) const
    {
        ForEachReflectedClass<false /* IncludeSubclasses */, TType>(HandleClass(&handler, &module, typeInfo));
    }
};

//===========================

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
        filler.HandleType(module, &typeInfo);

        // Class Type
        FillModuleTypeClass<TFirstModule, TType>{}(filler, module, &typeInfo);

        // Template Parameters
        FillModuleTypeTemplateParameters<TFirstModule, TType>{}(filler, module);

        // Class Base Type
        FillModuleTypeBaseClass<TFirstModule, TType>{}(filler, module);

        // Members
        FillModuleTypeClassMembers<TFirstModule, TType>{}(filler, module);

        // Handle the rest of the modules
        FillModuleTypes<TType, std::tuple<TOtherModules...>>{}(typeInfo);
    }
};

//------------------------

template <typename T, typename = void>
struct HandleIsConstructible
{
    inline void operator()(TypeInfo* typeInfo) const {}
};

template <typename T>
struct HandleIsConstructible<T, typename std::enable_if<std::is_default_constructible<T>::value>::type>
{
    using ContainerTraits = Entropy::details::ReflectionContainerTraits<T>;

    inline void operator()(TypeInfo* typeInfo) const
    {
        typeInfo->SetConstructionHandler([]() {
            typename ContainerTraits::template Allocator<T> alloc;
            T* obj = std::allocator_traits<decltype(alloc)>::allocate(alloc, 1);
            if (ENTROPY_LIKELY(obj != nullptr))
            {
                std::allocator_traits<decltype(alloc)>::construct(alloc, obj);
            }
            return obj;
        });

        typeInfo->SetDestructionHandler([](void* dataPtr) {
            if (ENTROPY_LIKELY(dataPtr != nullptr))
            {
                typename ContainerTraits::template Allocator<T> alloc;
                std::allocator_traits<decltype(alloc)>::destroy(alloc, reinterpret_cast<T*>(dataPtr));
                std::allocator_traits<decltype(alloc)>::deallocate(alloc, reinterpret_cast<T*>(dataPtr), 1);
            }
        });
    }
};

template <typename TType>
struct FillCommonTypeInfo
{
    void operator()(TypeInfo* typeInfo) const
    {
        typeInfo->SetTypeName(MakeTypeName<TType>());

        HandleIsConstructible<TType>{}(typeInfo);
    }
};

} // namespace details

template <typename T>
const TypeInfo* ReflectTypeAndGetTypeInfo() noexcept
{
    // We cannot make the allocation and initialization one step because re-entrancy will cause a hang.
    // Consider the declaration:
    //     struct Derived : public Base<Derived> { ENTROPY_REFLECT_CLASS_WITH_BASE(Derived, Base<Derived>) };
    // While processing Derived's base class' type info, we will again be trying to grab Derive's type info during
    // template parameter processing. By separating the allocation from the initialization, we avoid the hang during
    // this re-entrant call. Instead of a hang, the base type will be given a partially initialized Derived type info as
    // the template parameter.

    static TypeInfo typeInfo{};
    if (ENTROPY_UNLIKELY(typeInfo.RequireInitialization()))
    {
        details::FillCommonTypeInfo<T>{}(&typeInfo);
        details::FillModuleTypes<T, TypeInfo::ModuleTypes>{}(typeInfo);
    }
    return &typeInfo;
}

template <typename T>
void ReflectType()
{
    ReflectTypeAndGetTypeInfo<T>();
}

} // namespace Entropy
