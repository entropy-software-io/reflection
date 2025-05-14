// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

#include "Entropy/Core/Log.h"
#include "Entropy/Reflection.h"
#include "Entropy/Reflection/DynamicFunction/DynamicFunctionBase.h"

namespace Entropy
{

class TypeInfo;

/// <summary>
/// Similar to a regular Function<> object, except that parameters are checked at runtime.
///
/// The class is not templated and thus can refer to any Function<> object. This makes it useful for calling reflected
/// methods in a generic way.
/// </summary>
class DynamicFunction
{
    using ContainerTraits = details::ReflectionContainerTraits<DynamicFunction>;

public:
    DynamicFunction() noexcept                             = default;
    DynamicFunction(const DynamicFunction& other) noexcept = default;
    DynamicFunction(DynamicFunction&& other) noexcept      = default;

    constexpr DynamicFunction(std::nullptr_t) noexcept {}

    template <template <typename> class TFn, typename T>
    DynamicFunction(TFn<T>&& func) noexcept
    {
        _func = details::MakeDynamicFunction<TFn, T>(std::move(func));
    }

    ~DynamicFunction()
    {
        details::DestroyDynamicFunction(_func);
        _func = nullptr;
    }

    DynamicFunction& operator=(const DynamicFunction& other) noexcept = default;
    DynamicFunction& operator=(DynamicFunction&& other) noexcept      = default;

    DynamicFunction& operator=(std::nullptr_t) noexcept
    {
        this->~DynamicFunction();
        new (this) DynamicFunction(nullptr);
        return *this;
    }

    template <typename T>
    DynamicFunction& operator=(T&& func) noexcept
    {
        this->~DynamicFunction();
        new (this) DynamicFunction(std::move(func));
        return *this;
    }

    template <template <typename> class TFn, typename T>
    DynamicFunction& operator=(TFn<T>&& func) noexcept
    {
        this->~DynamicFunction();
        new (this) DynamicFunction(std::move(func));
        return *this;
    }

    inline operator bool() { return (_func != nullptr); }

    /// <summary>
    /// Attempts to invoke the function with the specified args.
    /// </summary>
    /// <remarks>
    /// The function must not be a class member function, nor return a value.
    /// </remarks>
    /// <returns>true if the invocation was successful; false otherwise</returns>
    template <typename... T>
    bool Invoke(T&&... args) const noexcept
    {
        ENTROPY_CHECK_RETURN_VAL(_func, false, "Underlying function is not valid");

        return _func->Invoke((void*)nullptr, std::forward<T>(args)...);
    }

    /// <summary>
    /// Attempts to invoke the function with the specified args. The
    /// function is expected to return a non-void value.
    /// </summary>
    /// <remarks>
    /// If the function is a method, the class object must be the second parameter
    /// </remarks>
    /// <returns>true if the invocation was successful; false otherwise</returns>
    template <typename TReturnValue, typename... TArgs>
    bool InvokeWithReturnValue(TReturnValue&& outRet, TArgs&&... args) const noexcept
    {
        ENTROPY_CHECK_RETURN_VAL(_func, false, "Underlying function is not valid");

        return _func->Invoke(outRet, std::forward<TArgs>(args)...);
    }

    /// <summary>
    /// Attempts to invoke the function with the specified args. The
    /// function is expected to return a non-void value.
    /// </summary>
    /// <returns>true if the invocation was successful; false otherwise</returns>
    template <typename... T>
    inline bool operator()(T&&... args) const noexcept
    {
        return Invoke(std::forward<T>(args)...);
    }

private:
    details::DynamicFunctionBase* _func = nullptr;
};

} // namespace Entropy
