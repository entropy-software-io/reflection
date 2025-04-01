// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

#include "Entropy/Details/Defines.h"
#include "Entropy/Details/TypeTraits.h"

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
struct HasReflectionMethod<T, std::enable_if_t<HasRawReflectionMethod<T>::value>>
{
    static constexpr bool value = T::template SingleExecutionMetaFunctionExists<0, T>::value;
};

} // namespace details

template <typename T>
constexpr bool IsReflectedType_v =
    std::is_class_v<ENTROPY_REMOVE_CONST_REF(T)> && details::HasReflectionMethod<ENTROPY_REMOVE_CONST_REF(T)>::value;

} // namespace Traits

} // namespace Entropy