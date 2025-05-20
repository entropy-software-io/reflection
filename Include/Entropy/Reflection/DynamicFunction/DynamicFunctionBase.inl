// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

namespace Entropy
{
namespace details
{

inline StringOps::StringType DynamicFuncParam::GetTypeName() const
{
    if (ENTROPY_LIKELY(_dataObj))
    {
        return _dataObj.GetTypeInfo()->GetTypeName();
    }
    else
    {
        return "< NULL >";
    }
}

template <bool TIsReturnValue, typename T>
DynamicFuncParam DynamicFuncParam::Create(T&& param)
{
    // The type will always have a reference qualifier. This is okay because
    // when we pass in as a parameter, it will also be a reference.

    return DynamicFuncParam(MakeDataObject<T, TIsReturnValue>{}(std::forward<T>(param)));
}

//==================

template <typename TReturnValue, typename... T>
bool DynamicFunctionBase::Invoke(TReturnValue&& outRet, T&&... args) const
{
    if ENTROPY_CONSTEXPR (std::is_same<
                              typename std::remove_pointer<typename std::remove_reference<TReturnValue>::type>::type,
                              void>::value)
    {
        ENTROPY_ASSERT(!outRet);
        return DoInvoke(DynamicFuncParam(nullptr), {DynamicFuncParam::Create<false>(std::forward<T>(args))...});
    }
    else
    {
        // Add a reference back in for the return value because it is normally expected
        return DoInvoke(DynamicFuncParam::Create<false>(std::forward<TReturnValue>(outRet)),
                        {DynamicFuncParam::Create<false>(std::forward<T>(args))...});
    }
}

template <typename T>
bool DynamicFunctionBase::IsReturnValueConvertible(const DynamicFuncParam& param)
{
    if (ENTROPY_UNLIKELY(!param))
    {
        return false;
    }

    const TypeInfo* thisFnReturnTypeInfo = ReflectTypeAndGetTypeInfo<typename std::decay<T>::type>();
    if (ENTROPY_UNLIKELY(!thisFnReturnTypeInfo))
    {
        return false;
    }

    return (thisFnReturnTypeInfo == param._dataObj.GetTypeInfo());
}

template <typename T>
bool DynamicFunctionBase::IsParameterConvertible(const DynamicFuncParam& param)
{
    if (ENTROPY_UNLIKELY(!param))
    {
        return false;
    }

    return param._dataObj.CanCastTo<T>();
}

template <typename T, typename... TRest>
bool DynamicFunctionBase::AreParametersConvertible(const DynamicFuncParam* params, int index, int paramCount)
{
    ENTROPY_CHECK_RETURN_VAL_FUNC(index >= 0 && index < paramCount, false, "Bad params");
    if (!IsParameterConvertible<T>(params[index]))
    {
        ENTROPY_LOG_ERROR_FUNC("Parameter is incorrect type. [Index: "
                               << index << "] [Expected: " << Traits::TypeNameOf<T>{}()
                               << "] [Got: " << params[index].GetTypeName() << "]");
        return false;
    }

    return AreParametersConvertible<TRest...>(params, index + 1, paramCount);
}

template <std::size_t Index, class TArg>
typename std::remove_reference<TArg>::type& DynamicFunctionBase::ConvertType(const DynamicFuncParam* param)
{
    auto actualParam     = (param + Index);
    auto argTypeInfo     = ReflectTypeAndGetTypeInfo<typename std::remove_reference<TArg>::type>();
    DataObject paramData = actualParam->_dataObj;
    ENTROPY_ASSERT(paramData);

    return paramData.GetData<typename std::remove_reference<TArg>::type>();
}

template <typename TClass, typename ReturnValue, typename TCallFn, typename... Args>
typename std::enable_if<std::is_same<ReturnValue, void>::value, bool>::type DynamicFunctionBase::_InvokeImpl(
    const DynamicFuncParam& classObj, const DynamicFuncParam& outRet, TCallFn&& callFn,
    const DynamicFuncParam* firstParam, int paramCount) const
{
    // Return value is void
    ENTROPY_CHECK_RETURN_VAL(!outRet.IsValid(), false,
                             "No return value should be provided when the function returns void");

    callFn(&classObj, firstParam);

    return true;
}

template <typename TClass, typename ReturnValue, typename TCallFn, typename TVoid>
bool DynamicFunctionBase::DoCallWithReturnValue<TClass, ReturnValue, TCallFn, TVoid>::operator()(
    const DynamicFunctionBase* dynFn, const DynamicFuncParam& classObj, const DynamicFuncParam& outRet,
    TCallFn&& callFn, const DynamicFuncParam* firstParam, int paramCount) const
{
    if (dynFn->IsParameterConvertible<ReturnValue>(outRet))
    {
        dynFn->ConvertType<0, ReturnValue>(&outRet) = callFn(&classObj, firstParam);

        return true;
    }

    if (dynFn->IsParameterConvertible<ReturnValue*>(outRet))
    {
        *dynFn->ConvertType<0, ReturnValue*>(&outRet) = callFn(&classObj, firstParam);

        return true;
    }

    ENTROPY_LOG_ERROR("Return value is incorrect type. [Expected Compatibility With: "
                      << Traits::TypeNameOf<ReturnValue>{}() << "] [Got: " << outRet.GetTypeName() << "]");
    return false;
}

template <typename TClass, typename ReturnValue, typename TCallFn>
bool DynamicFunctionBase::DoCallWithReturnValue<TClass, ReturnValue, TCallFn,
                                                typename std::enable_if<std::is_reference<ReturnValue>::value>::type>::
operator()(const DynamicFunctionBase* dynFn, const DynamicFuncParam& classObj, const DynamicFuncParam& outRet,
           TCallFn&& callFn, const DynamicFuncParam* firstParam, int paramCount) const
{
    if (dynFn->IsParameterConvertible<typename std::remove_reference<ReturnValue>::type*>(outRet))
    {
        dynFn->ConvertType<0, typename std::remove_reference<ReturnValue>::type*>(&outRet) =
            &callFn(&classObj, firstParam);

        return true;
    }

    if (dynFn->IsParameterConvertible<ReturnValue>(outRet))
    {
        dynFn->ConvertType<0, ReturnValue>(&outRet) = callFn(&classObj, firstParam);

        return true;
    }

    ENTROPY_LOG_ERROR("Return value is incorrect type. [Expected Compatibility With: "
                      << Traits::TypeNameOf<ReturnValue>{}() << "] [Got: " << outRet.GetTypeName() << "]");
    return false;
}

template <typename TClass, typename ReturnValue, typename TCallFn, typename... Args>
typename std::enable_if<!std::is_same<ReturnValue, void>::value, bool>::type DynamicFunctionBase::_InvokeImpl(
    const DynamicFuncParam& classObj, const DynamicFuncParam& outRet, TCallFn&& callFn,
    const DynamicFuncParam* firstParam, int paramCount) const
{
    // Return value is not void

    ENTROPY_CHECK_RETURN_VAL(outRet.IsValid(), false, "A return value is expected, but none was provided");

    /*if ENTROPY_CONSTEXPR (std::is_copy_assignable<typename std::remove_const<
                              typename std::remove_reference<ReturnValue>::type>::type>::value)
    {
        if (this->IsParameterConvertible<
                typename std::remove_const<typename std::remove_reference<ReturnValue>::type>::type>(outRet))
        {
            this->ConvertType<0, typename std::remove_const<typename std::remove_reference<ReturnValue>::type>::type>(
                &outRet) = callFn(&classObj, firstParam);

            return true;
        }
    }

    if ENTROPY_CONSTEXPR (std::is_reference<ReturnValue>::value)
    {
        if (this->IsParameterConvertible<ReturnValue*>(outRet))
        {
            this->ConvertType<0, ReturnValue*>(&outRet) = &callFn(&classObj, firstParam);

            return true;
        }
    }

    ENTROPY_LOG_ERROR("Return value is incorrect type. [Expected Compatibility With: "
                      << Traits::TypeNameOf<ReturnValue>{}() << "] [Got: " << outRet.GetTypeName() << "]");
    return false;*/

    return DoCallWithReturnValue<TClass, ReturnValue, TCallFn>{}(this, classObj, outRet, std::forward<TCallFn>(callFn),
                                                                 firstParam, paramCount);
}

template <typename TClass, typename ReturnValue, typename TCallFn, typename... Args>
bool DynamicFunctionBase::InvokeImpl(const DynamicFuncParam& classObj, const DynamicFuncParam& outRet, TCallFn&& callFn,
                                     const DynamicFuncParam* firstParam, int paramCount) const
{
    if ENTROPY_CONSTEXPR (std::is_same<TClass, void>::value)
    {
        ENTROPY_CHECK_RETURN_VAL(!classObj.IsValid(), false,
                                 "Class object should not be passed to non-member function");
    }
    else
    {
        ENTROPY_CHECK_RETURN_VAL(classObj.IsValid(), false, "Class object must be passed to member function");

        ENTROPY_CHECK_RETURN_VAL(this->IsParameterConvertible<TClass>(classObj), false,
                                 "Invalid class object pointer. [Expected: "
                                     << Traits::TypeNameOf<TClass>{}() << "] [Got: " << classObj.GetTypeName() << "]");
    }

    ENTROPY_CHECK_RETURN_VAL(paramCount == sizeof...(Args), false,
                             "Invalid number of args. [Expected: " << sizeof...(Args) << "] [Got: " << paramCount
                                                                   << "]");

    ENTROPY_CHECK_RETURN_VAL(AreParametersConvertible<Args...>(firstParam, 0, sizeof...(Args)), false,
                             "Invalid parameters");

    return _InvokeImpl<TClass, ReturnValue, TCallFn, Args...>(classObj, outRet, std::move(callFn), firstParam,
                                                              paramCount);
}

//==================

template <template <typename> class TFn, typename ReturnValue, typename... Args>
template <class Tuple, std::size_t... I>
ReturnValue TDynamicFunction<TFn, ReturnValue(Args...)>::DoCall(const DynamicFuncParam* params,
                                                                std::index_sequence<I...>) const
{
    return _func(std::forward<typename std::tuple_element<I, Tuple>::type>(
        this->ConvertType<I, typename std::tuple_element<I, Tuple>::type>(params))...);
}

template <template <typename> class TFn, typename ReturnValue, typename... Args>
bool TDynamicFunction<TFn, ReturnValue(Args...)>::DoInvoke(DynamicFuncParam outRet,
                                                           std::initializer_list<DynamicFuncParam> params) const
{
    ENTROPY_CHECK_RETURN_VAL(!Traits::IsNull(_func), false, "Underlying function is not valid");

    auto callFn = [this](const DynamicFuncParam* classObj, const DynamicFuncParam* params) -> ReturnValue {
        return DoCall<std::tuple<Args...>>(params, std::index_sequence_for<Args...>());
    };

    return this->InvokeImpl<void, ReturnValue, decltype(callFn), Args...>(
        DynamicFuncParam(nullptr), outRet, std::move(callFn), params.begin(), (int)params.size());
}

//==================

template <template <typename> class TFn, typename TClass, typename ReturnValue, typename... Args>
template <class Tuple, std::size_t... I>
ReturnValue TDynamicFunction<TFn, ReturnValue (TClass::*)(Args...)>::DoCall(const DynamicFuncParam* classObj,
                                                                            const DynamicFuncParam* params,
                                                                            std::index_sequence<I...>) const
{
    return _func(this->ConvertType<0, TClass*>(classObj),
                 std::forward<typename std::tuple_element<I, Tuple>::type>(
                     this->ConvertType<I, typename std::tuple_element<I, Tuple>::type>(params))...);
}

template <template <typename> class TFn, typename TClass, typename ReturnValue, typename... Args>
bool TDynamicFunction<TFn, ReturnValue (TClass::*)(Args...)>::DoInvoke(
    DynamicFuncParam outRet, std::initializer_list<DynamicFuncParam> params) const
{
    ENTROPY_CHECK_RETURN_VAL(!Traits::IsNull(_func), false, "Underlying function is not valid");

    ENTROPY_CHECK_RETURN_VAL(params.size() > 0, false, "Expected at least one parameter for the class object");

    auto callFn = [this](const DynamicFuncParam* classObj, const DynamicFuncParam* params) -> ReturnValue {
        return DoCall<std::tuple<Args...>>(classObj, params, std::index_sequence_for<Args...>());
    };

    return this->InvokeImpl<TClass*, ReturnValue, decltype(callFn), Args...>(
        *params.begin(), outRet, std::move(callFn), params.begin() + 1, (int)params.size() - 1);
}

//==================

template <template <typename> class TFn, typename TClass, typename ReturnValue, typename... Args>
template <class Tuple, std::size_t... I>
ReturnValue TDynamicFunction<TFn, ReturnValue (TClass::*)(Args...) const>::DoCall(const DynamicFuncParam* classObj,
                                                                                  const DynamicFuncParam* params,
                                                                                  std::index_sequence<I...>) const
{
    return _func(this->ConvertType<0, const TClass*>(classObj),
                 std::forward<typename std::tuple_element<I, Tuple>::type>(
                     this->ConvertType<I, typename std::tuple_element<I, Tuple>::type>(params))...);
}

template <template <typename> class TFn, typename TClass, typename ReturnValue, typename... Args>
bool TDynamicFunction<TFn, ReturnValue (TClass::*)(Args...) const>::DoInvoke(
    DynamicFuncParam outRet, std::initializer_list<DynamicFuncParam> params) const
{
    ENTROPY_CHECK_RETURN_VAL(!Traits::IsNull(_func), false, "Underlying function is not valid");

    ENTROPY_CHECK_RETURN_VAL(params.size() > 0, false, "Expected at least one parameter for the class object");

    auto callFn = [this](const DynamicFuncParam* classObj, const DynamicFuncParam* params) -> ReturnValue {
        return DoCall<std::tuple<Args...>>(classObj, params, std::index_sequence_for<Args...>());
    };

    return this->InvokeImpl<const TClass*, ReturnValue, decltype(callFn), Args...>(
        *params.begin(), outRet, std::move(callFn), params.begin() + 1, (int)params.size() - 1);
}

//==================

template <template <typename> class TFn, typename T>
inline DynamicFunctionBase* MakeDynamicFunction(const TFn<T>& func)
{
    return AllocatorOps::CreateInstance<TDynamicFunction<TFn, T>>(func);
}

template <template <typename> class TFn, typename T>
inline DynamicFunctionBase* MakeDynamicFunction(TFn<T>&& func)
{
    return AllocatorOps::CreateInstance<TDynamicFunction<TFn, T>>(std::move(func));
}

inline void DestroyDynamicFunction(DynamicFunctionBase* func) { return AllocatorOps::DestroyInstance(func); }

} // namespace details
} // namespace Entropy
