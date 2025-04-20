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
class HasRawReflectionMethod
{
    template <typename TClass>
    static std::true_type Exists(typename TClass::template SingleExecutionMetaFunctionExists<0, TClass>*);
    template <typename>
    static std::false_type Exists(...);

    typedef decltype(Exists<T>(nullptr)) ResultType; // NOLINT

public:
    static constexpr bool value = ResultType::value;
};

template <typename T, typename = void>
struct HasReflectionMethod : public std::false_type
{
};

template <typename T>
struct HasReflectionMethod<T, typename std::enable_if<HasRawReflectionMethod<T>::value>::type>
{
    static constexpr bool value = T::template SingleExecutionMetaFunctionExists<0, T>::value;
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

} // namespace Traits

} // namespace Entropy