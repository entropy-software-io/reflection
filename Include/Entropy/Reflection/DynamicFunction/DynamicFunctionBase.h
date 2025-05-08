// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

#include "Entropy/Core/Log.h"
#include "Entropy/Reflection/DataObject/DataObjectFactory.h"
#include <type_traits>

namespace Entropy
{

class TypeInfo;

namespace details
{

class DynamicFunctionBase;
class DynamicFunction;

template <template <typename> class TFn, typename T>
DynamicFunctionBase* MakeDynamicFunction(const TFn<T>& func);

template <template <typename> class TFn, typename T>
DynamicFunctionBase* MakeDynamicFunction(TFn<T>&& func);

void DestroyDynamicFunction(DynamicFunctionBase* func);

struct DynamicFuncParam
{
    using ContainerTraits = ReflectionContainerTraits<DynamicFuncParam>;
    using StringType      = ContainerTraits::StringType;

public:
    DynamicFuncParam(std::nullptr_t) {}

    StringType GetTypeName() const;

    inline bool IsValid() const { return _dataObj; }

    inline operator bool() const { return IsValid(); }

private:
    template <typename T, typename = void>
    struct MakeParam
    {
        inline DataObject operator()(T&& param) const { return DataObjectFactory::Wrap<T>(std::forward<T>(param)); }
    };

    template <typename T>
    struct MakeParam<T, typename std::enable_if<std::is_same<T, void>::value>::type>
    {
        inline DataObject operator()(...) const { return DataObject(nullptr); }
    };

    template <typename T, typename = void>
    struct MakeReturnValue
    {
        using TReturnValue = typename std::remove_reference<T>::type&;

        inline DataObject operator()(T&& param) const { return MakeParam<TReturnValue>{}(param); }
    };

    template <typename T>
    struct MakeReturnValue<T, typename std::enable_if<std::is_same<T, void>::value>::type>
    {
        inline DataObject operator()(...) const { return MakeParam<void>{}(nullptr); }
    };

    template <typename T>
    struct MakeReturnValue<T, typename std::enable_if<std::is_pointer<T>::value>::type>
    {
        using TReturnValue = typename std::remove_pointer<T>::type&;

        inline DataObject operator()(T&& param) const
        {
            // Remove the pointer for the return value
            return DataObjectFactory::Wrap<TReturnValue>(&*param);
        }
    };

    template <typename T, bool TIsReturnValue>
    struct MakeDataObject
    {
        inline DataObject operator()(T&& param) const { return MakeParam<T>{}(std::forward<T>(param)); }
    };

    template <typename T>
    struct MakeDataObject<T, true>
    {
        inline DataObject operator()(T&& param) const { return MakeReturnValue<T>{}(std::forward<T>(param)); }
    };

    template <bool TIsReturnValue, typename T>
    static DynamicFuncParam Create(T&& param);

    DynamicFuncParam(DataObject&& dataObj)
        : _dataObj(std::move(dataObj))
    {
    }

    DataObject _dataObj{};

    friend class DynamicFunctionBase;
};

class DynamicFunctionBase
{
public:
    virtual ~DynamicFunctionBase() {}

    /// <summary>
    /// Attempts to invoke the function with the specified args.
    /// </summary>
    /// <remarks>
    /// If a class object is present, the function must be a class method.
    /// If a out return variable is provided, the function must return a value.
    /// </remarks>
    /// <returns>true if the invocation was successful; false otherwise</returns>
    template <typename TReturnValue, typename... T>
    bool Invoke(TReturnValue&& outRet, T&&... args) const;

    virtual DynamicFunctionBase* Clone() = 0;

    virtual operator bool() const = 0;

protected:
    template <typename T, typename = void>
    struct ReturnValueParameterType
    {
        using type = T*;
    };

    template <typename T>
    struct ReturnValueParameterType<T, typename std::enable_if<std::is_reference<T>::value>::type>
    {
        using Type = typename ReturnValueParameterType<typename std::remove_reference<T>::type>::type*;
    };

    //---------

    template <typename T>
    static bool IsReturnValueConvertible(const DynamicFuncParam& param);

    template <typename T>
    static bool IsParameterConvertible(const DynamicFuncParam& param);

    template <typename... TRest>
    static typename std::enable_if<sizeof...(TRest) == 0, bool>::type AreParametersConvertible(
        const DynamicFuncParam* params, int index, int paramCount)
    {
        return true;
    }

    template <typename T, typename... TRest>
    static bool AreParametersConvertible(const DynamicFuncParam* params, int index, int paramCount);

    static bool IsParameterEmpty(const DynamicFuncParam* param) { return !param; }

    template <std::size_t Index, class TArg>
    static typename std::remove_reference<TArg>::type& ConvertType(const DynamicFuncParam* param);

    template <typename TClass, typename ReturnValue, typename TCallFn, typename... Args>
    typename std::enable_if<std::is_same<ReturnValue, void>::value, bool>::type _InvokeImpl(
        const DynamicFuncParam& classObj, const DynamicFuncParam& outRet, TCallFn&& callFn,
        const DynamicFuncParam* firstParam, int paramCount) const;

    template <typename TClass, typename ReturnValue, typename TCallFn, typename... Args>
    typename std::enable_if<!std::is_same<ReturnValue, void>::value, bool>::type _InvokeImpl(
        const DynamicFuncParam& classObj, const DynamicFuncParam& outRet, TCallFn&& callFn,
        const DynamicFuncParam* firstParam, int paramCount) const;

    template <typename TClass, typename ReturnValue, typename TCallFn, typename... Args>
    bool InvokeImpl(const DynamicFuncParam& classObj, const DynamicFuncParam& outRet, TCallFn&& callFn,
                    const DynamicFuncParam* firstParam, int paramCount) const;

    virtual bool DoInvoke(DynamicFuncParam outRet, std::initializer_list<DynamicFuncParam> params) const = 0;
};

template <template <typename> class TFn, typename T>
class TDynamicFunction;

template <template <typename> class TFn, typename ReturnValue, typename... Args>
class TDynamicFunction<TFn, ReturnValue(Args...)> : public DynamicFunctionBase
{
    template <typename T>
    using Function = TFn<T>;

public:
    TDynamicFunction() {}

    TDynamicFunction(const Function<ReturnValue(Args...)>& func)
        : _func(func)
    {
    }

    TDynamicFunction(Function<ReturnValue(Args...)>&& func)
        : _func(std::move(func))
    {
    }

    virtual operator bool() const override { return !Traits::IsNull(_func); }

    virtual DynamicFunctionBase* Clone() override { return MakeDynamicFunction(_func); }

protected:
    template <class Tuple, std::size_t... I>
    ReturnValue DoCall(const DynamicFuncParam* params, std::index_sequence<I...>) const;

    virtual bool DoInvoke(DynamicFuncParam outRet, std::initializer_list<DynamicFuncParam> params) const override;

private:
    Function<ReturnValue(Args...)> _func;
};

template <template <typename> class TFn, typename TClass, typename ReturnValue, typename... Args>
class TDynamicFunction<TFn, ReturnValue (TClass::*)(Args...)> : public DynamicFunctionBase
{
    template <typename T>
    using Function = TFn<T>;

public:
    TDynamicFunction() {}

    TDynamicFunction(const Function<ReturnValue (TClass::*)(Args...)>& func)
        : _func(func)
    {
    }

    TDynamicFunction(Function<ReturnValue (TClass::*)(Args...)>&& func)
        : _func(std::move(func))
    {
    }

    virtual operator bool() const override { return !Traits::IsNull(_func); }

    virtual DynamicFunctionBase* Clone() override { return MakeDynamicFunction(_func); }

protected:
    template <class Tuple, std::size_t... I>
    ReturnValue DoCall(const DynamicFuncParam* classObj, const DynamicFuncParam* params,
                       std::index_sequence<I...>) const;

    virtual bool DoInvoke(DynamicFuncParam outRet, std::initializer_list<DynamicFuncParam> params) const override;

private:
    Function<ReturnValue (TClass::*)(Args...)> _func;
};

template <template <typename> class TFn, typename TClass, typename ReturnValue, typename... Args>
class TDynamicFunction<TFn, ReturnValue (TClass::*)(Args...) const> : public DynamicFunctionBase
{
    template <typename T>
    using Function = TFn<T>;

public:
    TDynamicFunction() {}

    TDynamicFunction(const Function<ReturnValue (TClass::*)(Args...) const>& func)
        : _func(func)
    {
    }

    TDynamicFunction(Function<ReturnValue (TClass::*)(Args...) const>&& func)
        : _func(std::move(func))
    {
    }

    virtual operator bool() const override { return !Traits::IsNull(_func); }

    virtual DynamicFunctionBase* Clone() override { return MakeDynamicFunction(_func); }

protected:
    template <class Tuple, std::size_t... I>
    ReturnValue DoCall(const DynamicFuncParam* classObj, const DynamicFuncParam* params,
                       std::index_sequence<I...>) const;

    virtual bool DoInvoke(DynamicFuncParam outRet, std::initializer_list<DynamicFuncParam> params) const override;

private:
    Function<ReturnValue (TClass::*)(Args...) const> _func;
};

} // namespace details
} // namespace Entropy

#include "DynamicFunctionBase.inl"
