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

TypeInfo* CreateTypeInfo() noexcept;

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
        void operator()(AttributeCollection<TAttrTypes...>&& classAttr)
        {
            _handler->template HandleClass<TAttrTypes...>(*_module, _typeInfo, std::move(classAttr));
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

    template <typename... Un>
    struct HandleTemplateParameters
    {
        void operator()(ModuleHandlerType& handler, TModule& module) const {}
    };

    template <typename U1, typename... Un>
    struct HandleTemplateParameters<U1, Un...>
    {
        void operator()(ModuleHandlerType& handler, TModule& module) const
        {
            const TypeInfo* templateParamTypeInfo = ReflectTypeAndGetTypeInfo<U1>();
            handler.template HandleTemplateParameter<U1>(module, templateParamTypeInfo);

            HandleTemplateParameters<Un...>{}(handler, module);
        }
    };

    void operator()(ModuleHandlerType& handler, TModule& module) { HandleTemplateParameters<Tn...>{}(handler, module); }
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
        void operator()(const char* memberName, AttributeCollection<TAttrTypes...>&& memberAttr)
        {
            // TMember is always a reference because we use decltype((member)) to preserve the const of the type. This
            // forces a reference too.
            using TMemberNoRef = typename std::remove_reference<TMember>::type;

            const TypeInfo* typeInfo = ReflectTypeAndGetTypeInfo<TMemberNoRef>();

            _handler->template HandleClassMember<TMemberNoRef, TAttrTypes...>(*_module, memberName, typeInfo,
                                                                              std::move(memberAttr));
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
    void operator()(TypeInfo* typeInfo) const {}
};

template <typename TType, typename TFirstModule, typename... TOtherModules>
struct FillModuleTypes<TType, std::tuple<TFirstModule, TOtherModules...>>
{
    using ModuleFillerType = Entropy::Reflection::FillModuleTypeInfo<TFirstModule, TType>;

    void operator()(TypeInfo* typeInfo) const
    {
        ModuleFillerType filler;

        TFirstModule& module = typeInfo->Get<TFirstModule>();

        // Base Type
        filler.HandleType(module, typeInfo);

        // Class Type
        FillModuleTypeClass<TFirstModule, TType>{}(filler, module, typeInfo);

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
struct IsAllocatorDestructible : public std::false_type
{
};

template <typename T>
struct IsAllocatorDestructible<T,
                               typename std::enable_if<decltype(std::declval<T>().~T(), std::true_type())::value>::type>
    : public std::true_type
{
};

template <typename T, typename = void>
struct IsAllocatorConstructible : public std::false_type
{
};

template <typename T>
struct IsAllocatorConstructible<
    T, typename std::enable_if<decltype(new (std::declval<T*>()) T(), std::true_type())::value>::type>
    : public std::true_type
{
};

template <typename T, typename = void>
struct HandleIsConstructible
{
    inline void operator()(TypeInfo* typeInfo) const {}
};

template <typename T>
struct HandleIsConstructible<
    T, typename std::enable_if<IsAllocatorDestructible<typename std::remove_const<T>::type>::value &&
                               IsAllocatorConstructible<typename std::remove_const<T>::type>::value>::type>
{
    using NonConstT       = typename std::remove_const<T>::type;
    using ContainerTraits = Entropy::details::ReflectionContainerTraits<NonConstT>;

    inline void operator()(TypeInfo* typeInfo) const
    {
        typeInfo->SetConstructionHandler([]() { return AllocatorOps::CreateInstance<T>(); });
    }
};

//------------------------

template <typename T, typename = void>
struct IsAllocatorCopyConstructible : public std::false_type
{
};

template <typename T>
struct IsAllocatorCopyConstructible<
    T, typename std::enable_if<decltype(new (std::declval<T*>()) T(std::declval<const T&>()),
                                        std::true_type())::value>::type> : public std::true_type
{
};

template <typename T, typename = void>
struct HandleIsCopyConstructible
{
    inline void operator()(TypeInfo* typeInfo) const {}
};

template <typename T>
struct HandleIsCopyConstructible<
    T, typename std::enable_if<IsAllocatorDestructible<typename std::remove_const<T>::type>::value &&
                               IsAllocatorCopyConstructible<typename std::remove_const<T>::type>::value>::type>
{
    using NonConstT       = typename std::remove_const<T>::type;
    using ContainerTraits = Entropy::details::ReflectionContainerTraits<NonConstT>;

    inline void operator()(TypeInfo* typeInfo) const
    {
        typeInfo->SetCopyConstructionHandler(
            [](const void* data) { return AllocatorOps::CreateInstance<T>(*reinterpret_cast<const T*>(data)); });
    }
};

//------------------------

template <typename T, typename = void>
struct IsAllocatorMoveConstructible : public std::false_type
{
};

template <typename T>
struct IsAllocatorMoveConstructible<T, typename std::enable_if<decltype(new (std::declval<T*>()) T(std::declval<T&&>()),
                                                                        std::true_type())::value>::type>
    : public std::true_type
{
};

template <typename T, typename = void>
struct HandleIsMoveConstructible
{
    inline void operator()(TypeInfo* typeInfo) const {}
};

template <typename T>
struct HandleIsMoveConstructible<
    T, typename std::enable_if<IsAllocatorDestructible<typename std::remove_const<T>::type>::value &&
                               IsAllocatorMoveConstructible<typename std::remove_const<T>::type>::value>::type>
{
    using NonConstT       = typename std::remove_const<T>::type;
    using ContainerTraits = Entropy::details::ReflectionContainerTraits<NonConstT>;

    inline void operator()(TypeInfo* typeInfo) const
    {
        typeInfo->SetMoveConstructionHandler(
            [](void* data) { return AllocatorOps::CreateInstance<T>(std::move(*reinterpret_cast<NonConstT*>(data))); });
    }
};

//------------------------

template <typename T, typename = void>
struct HandleIsDestructible
{
    inline void operator()(TypeInfo* typeInfo) const {}
};

template <typename T>
struct HandleIsDestructible<
    T, typename std::enable_if<IsAllocatorDestructible<typename std::remove_const<T>::type>::value &&
                               (IsAllocatorConstructible<typename std::remove_const<T>::type>::value ||
                                (IsAllocatorCopyConstructible<typename std::remove_const<T>::type>::value) ||
                                (IsAllocatorMoveConstructible<typename std::remove_const<T>::type>::value))>::type>
{
    using NonConstT       = typename std::remove_const<T>::type;
    using ContainerTraits = Entropy::details::ReflectionContainerTraits<NonConstT>;

    inline void operator()(TypeInfo* typeInfo) const
    {
        typeInfo->SetDestructionHandler(
            [](void* dataPtr) { AllocatorOps::DestroyInstance(reinterpret_cast<NonConstT*>(dataPtr)); });
    }
};

template <typename TType>
struct FillCommonTypeInfo
{
    void operator()(TypeInfo* typeInfo) const
    {
        typeInfo->SetTypeName(MakeTypeName<TType>{}());
        typeInfo->SetTypeId(Traits::TypeIdOf<TType>{}());

        HandleIsConstructible<TType>{}(typeInfo);
        HandleIsCopyConstructible<TType>{}(typeInfo);
        HandleIsMoveConstructible<TType>{}(typeInfo);
        HandleIsDestructible<TType>{}(typeInfo);

        if ENTROPY_CONSTEXPR (std::is_const<TType>::value)
        {
            typeInfo->SetIsConst();
        }

        if ENTROPY_CONSTEXPR (std::is_pointer<TType>::value)
        {
            typeInfo->SetIsPointer();
        }

        if ENTROPY_CONSTEXPR (std::is_lvalue_reference<TType>::value)
        {
            typeInfo->SetIsLReference();
        }

        if ENTROPY_CONSTEXPR (std::is_rvalue_reference<TType>::value)
        {
            typeInfo->SetIsRReference();
        }

        if ENTROPY_CONSTEXPR (std::is_array<TType>::value)
        {
            typeInfo->SetIsArray();
        }

        if ENTROPY_CONSTEXPR (!Traits::IsUnqualifiedType<TType>::value)
        {
            if ENTROPY_CONSTEXPR (std::is_reference<TType>::value)
            {
                // References can never also be const.

                const TypeInfo* unqualifiedType =
                    ReflectTypeAndGetTypeInfo<typename std::remove_reference<TType>::type>();
                typeInfo->SetNextUnqualifiedType(unqualifiedType);
            }
            else if ENTROPY_CONSTEXPR (std::is_pointer<TType>::value)
            {
                // Pointers can also be const at the same time. std::remove_pointer removes any const too.

                const TypeInfo* unqualifiedType =
                    ReflectTypeAndGetTypeInfo<typename std::remove_pointer<TType>::type>();
                typeInfo->SetNextUnqualifiedType(unqualifiedType);
            }
            else if ENTROPY_CONSTEXPR (std::is_array<TType>::value)
            {
                // Arrays can also be const at the same time. Make sure we remove both qualifiers because
                // we have already recorded both of them. The order we remove matters.

                const TypeInfo* unqualifiedType = ReflectTypeAndGetTypeInfo<
                    typename std::remove_extent<typename std::remove_const<TType>::type>::type>();
                typeInfo->SetNextUnqualifiedType(unqualifiedType);
            }
            else if ENTROPY_CONSTEXPR (std::is_const<TType>::value)
            {
                // Catches just raw const types. For instance, const int& will eventually come here
                // because the reference cannot be const. Also, const int* will eventually come here, but int* const
                // will not.

                const TypeInfo* unqualifiedType = ReflectTypeAndGetTypeInfo<typename std::remove_const<TType>::type>();
                typeInfo->SetNextUnqualifiedType(unqualifiedType);
            }
        }
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

    static TypeInfo* typeInfo = details::CreateTypeInfo();
    static TypeInfoRef typeInfoRef(typeInfo);

    if (ENTROPY_UNLIKELY(typeInfo->RequireInitialization()))
    {
        details::FillCommonTypeInfo<T>{}(typeInfo);
        details::FillModuleTypes<T, TypeInfo::ModuleTypes>{}(typeInfo);
    }

    return typeInfo;
}

template <typename T>
void ReflectType()
{
    ReflectTypeAndGetTypeInfo<T>();
}

} // namespace Entropy
