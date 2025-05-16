// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

#include "Entropy/Core/Details/Defines.h"
#include "Entropy/Core/Details/TypeTraits.h"

namespace Entropy
{

namespace Traits
{

namespace details
{

template <typename T>
class HasReflectionMethod
{
    struct TDummyFunc
    {
    };

    template <typename TClass>
    static std::true_type Exists(typename TClass::template __ClassTypeOperator<TClass, TDummyFunc>*);
    template <typename>
    static std::false_type Exists(...);

    typedef decltype(Exists<T>(nullptr)) ResultType; // NOLINT

public:
    static constexpr bool value = ResultType::value;
};

} // namespace details

template <typename T, typename = void>
struct IsReflectedType : std::false_type
{
};

template <typename T>
struct IsReflectedType<T, typename std::enable_if<std::is_class<RemoveConstRef_t<T>>::value &&
                                                  details::HasReflectionMethod<RemoveConstRef_t<T>>::value>::type>
    : std::true_type
{
};

#if __cplusplus >= 201300
template <typename T>
constexpr bool IsReflectedType_v = IsReflectedType<T>::value;
#endif

//--------------

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
struct IsAllocatorMoveConstructible : public std::false_type
{
};

template <typename T>
struct IsAllocatorMoveConstructible<T, typename std::enable_if<decltype(new (std::declval<T*>()) T(std::declval<T&&>()),
                                                                        std::true_type())::value>::type>
    : public std::true_type
{
};

} // namespace Traits

} // namespace Entropy