// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

#include "Entropy/Reflection/DataObject/DataObject.h"
#include "Entropy/Reflection/Details/ContainerTypes.h"
#include "TypeInfoModule.h"

namespace Entropy
{

class DataObject;

namespace Reflection
{

/// <summary>
/// Contains basic type information that can be accessed at runtime
/// </summary>
class BasicTypeInfo
{
private:
    using ContainerTraits     = Entropy::details::ReflectionContainerTraits<BasicTypeInfo>;
    using ConstructionHandler = ContainerTraits::FunctionType<void*()>;
    using DestructionHandler  = ContainerTraits::FunctionType<void(void*)>;

public:
    inline const ContainerTraits::StringType& GetTypeName() const { return _typeName; }

    inline bool CanConstruct() const;
    inline DataObject Construct() const;

    void SetConstructionHandler(ConstructionHandler&& handler) { _constructionFn = std::move(handler); }
    void SetDestructionHandler(DestructionHandler&& handler) { _destructionFn = std::move(handler); }

private:
    void SetTypeInfo(const TypeInfo* typeInfo) { _typeInfo = typeInfo; }
    void SetClassName(ContainerTraits::StringType&& name) { _typeName = std::move(name); }

    void Destruct(void* dataPtr) const;

    const TypeInfo* _typeInfo{};
    ContainerTraits::StringType _typeName{};

    ConstructionHandler _constructionFn{};
    DestructionHandler _destructionFn{};

    template <typename, typename, typename>
    friend struct FillModuleTypeInfo;

    friend class Entropy::DataObject;
};

//====================

namespace details
{
template <typename T, typename = void>
struct HandleIsConstructible
{
    inline void operator()(BasicTypeInfo& module) const {}
};

template <typename T>
struct HandleIsConstructible<T, typename std::enable_if<std::is_default_constructible<T>::value>::type>
{
    using ContainerTraits = Entropy::details::ReflectionContainerTraits<T>;

    inline void operator()(BasicTypeInfo& module) const
    {
        module.SetConstructionHandler([]() {
            typename ContainerTraits::template Allocator<T> alloc;
            T* obj = std::allocator_traits<decltype(alloc)>::allocate(alloc, 1);
            if (ENTROPY_LIKELY(obj != nullptr))
            {
                std::allocator_traits<decltype(alloc)>::construct(alloc, obj);
            }
            return obj;
        });

        module.SetDestructionHandler([](void* dataPtr) {
            if (ENTROPY_LIKELY(dataPtr != nullptr))
            {
                typename ContainerTraits::template Allocator<T> alloc;
                std::allocator_traits<decltype(alloc)>::destroy(alloc, reinterpret_cast<T*>(dataPtr));
                std::allocator_traits<decltype(alloc)>::deallocate(alloc, reinterpret_cast<T*>(dataPtr), 1);
            }
        });
    }
};

} // namespace details

/// <summary>
/// Initializes the BasicTypeInfo module. Called once per reflected type
/// </summary>
template <typename T>
struct FillModuleTypeInfo<BasicTypeInfo, T> : public DefaultFillModuleTypeInfo<BasicTypeInfo>
{
    void HandleType(BasicTypeInfo& module, const TypeInfo* thisTypeInfo)
    {
        module.SetTypeInfo(thisTypeInfo);
        module.SetClassName(typeid(T).name());

        details::HandleIsConstructible<T>{}(module);
    }
};

} // namespace Reflection
} // namespace Entropy

#include "BasicTypeInfo.inl"
