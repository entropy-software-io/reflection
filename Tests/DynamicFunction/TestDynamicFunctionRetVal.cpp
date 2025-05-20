// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#include "Entropy/DynamicFunction.h"
#include "TestMacros.h"

enum class MethodCalledState
{
    NotCalled,
    InvalidParam,
    SuccessfullyCalled
};

static MethodCalledState sMethodCalledState = MethodCalledState::InvalidParam;

//---------------------

int IntAddFunction(int incomingValue)
{
    sMethodCalledState = MethodCalledState::SuccessfullyCalled;
    return incomingValue + 100;
}

double DoubleAddFunction(double incomingValue)
{
    sMethodCalledState = MethodCalledState::SuccessfullyCalled;
    return incomingValue + 100.0;
}

//---------------------

class MyDynFnTestClass
{
public:
    float& GetMyFloat()
    {
        sMethodCalledState = MethodCalledState::SuccessfullyCalled;

        VerifyObjectState();

        return _myFloat;
    }

    float* GetMyFloatPtr()
    {
        sMethodCalledState = MethodCalledState::SuccessfullyCalled;

        VerifyObjectState();

        return &_myFloat;
    }

    std::string _myStr{"Bar"};
    float _myFloat{123.456f};

private:
    void VerifyObjectState() const
    {
        // Use "this" to ensure it was also passed correctly
        if (_myStr != "Bar" || _myFloat != 123.456f)
        {
            ENTROPY_LOG_ERROR("Invalid object state");
            sMethodCalledState = MethodCalledState::InvalidParam;
        }
    }
};

//---------------------

template <typename TRet, typename TFunc, typename TVerifyFn, typename... TArgs>
bool TestCallFunction(TFunc func, TVerifyFn verifyFn, TArgs&&... args)
{
    sMethodCalledState = MethodCalledState::NotCalled;

    std::function<typename std::remove_pointer<TFunc>::type> funcWrapper = func;
    Entropy::DynamicFunction dynFn(std::move(funcWrapper));

    TRet retVal;

    ENTROPY_CHECK_RETURN_VAL_FUNC(dynFn.InvokeWithReturnValue(retVal, std::forward<TArgs>(args)...), false,
                                  "Failed to invoke function of type '" << Entropy::Traits::TypeNameOf<TFunc>{}()
                                                                        << "'");

    ENTROPY_CHECK_RETURN_VAL_FUNC(sMethodCalledState != MethodCalledState::NotCalled, false,
                                  "Function of type '" << Entropy::Traits::TypeNameOf<TFunc>{}()
                                                       << "' was not invoked");
    ENTROPY_CHECK_RETURN_VAL_FUNC(sMethodCalledState != MethodCalledState::InvalidParam, false,
                                  "Function of type '" << Entropy::Traits::TypeNameOf<TFunc>{}()
                                                       << "' was invoked with bad or corrupted parameters");

    ENTROPY_CHECK_RETURN_VAL_FUNC(verifyFn(retVal), false, "Failed to verify return value");

    return true;
}

template <typename TRet, typename TMethod, typename TVerifyFn, typename... TArgs>
bool TestCallClassMethod(TMethod method, TVerifyFn verifyFn, TArgs&&... args)
{
    sMethodCalledState = MethodCalledState::NotCalled;

    auto methodWrapper = std::mem_fn(method);
    Entropy::DynamicFunction dynFn(std::move(methodWrapper));

    TRet retVal{};

    ENTROPY_CHECK_RETURN_VAL_FUNC(dynFn.InvokeWithReturnValue(retVal, std::forward<TArgs>(args)...), false,
                                  "Failed to invoke method of type '" << Entropy::Traits::TypeNameOf<TMethod>{}()
                                                                      << "'");

    ENTROPY_CHECK_RETURN_VAL_FUNC(sMethodCalledState != MethodCalledState::NotCalled, false,
                                  "Method of type '" << Entropy::Traits::TypeNameOf<TMethod>{}()
                                                     << "' was not invoked");
    ENTROPY_CHECK_RETURN_VAL_FUNC(sMethodCalledState != MethodCalledState::InvalidParam, false,
                                  "Method of type '" << Entropy::Traits::TypeNameOf<TMethod>{}()
                                                     << "' was invoked with bad or corrupted parameters");

    ENTROPY_CHECK_RETURN_VAL_FUNC(verifyFn(retVal), false, "Failed to verify return value");

    return true;
}

int DynamicFunction_TestDynamicFunctionRetVal(int argc, char** const argv)
{
    {
        int x = 123;
        TestCallFunction<int>(&IntAddFunction, [](int ret) { return (ret == 223); }, x);
    }

    {
        double x = 123.0;
        TestCallFunction<double>(&DoubleAddFunction, [](double ret) { return (ret == 223.0); }, x);
    }

    //-----------

    // We can capture a returned reference type by pointer
    {
        MyDynFnTestClass myTestClassObj;

        TestCallClassMethod<float*>(
            &MyDynFnTestClass::GetMyFloat,
            [](float* ret) {
                if (*ret != 123.456f)
                {
                    return false;
                }

                *ret += 100.0f;

                return true;
            },
            &myTestClassObj);

        ENTROPY_CHECK_RETURN_VAL_FUNC(myTestClassObj._myFloat == (123.456f + 100.0f), 1,
                                      "Failed to modify class value");
    }

    // Can store in a regular float too.
    {
        MyDynFnTestClass myTestClassObj;

        TestCallClassMethod<float>(
            &MyDynFnTestClass::GetMyFloat,
            [](float& ret) {
                if (ret != 123.456f)
                {
                    return false;
                }

                // Does not change myTestClassObj
                ret += 100.0f;

                return true;
            },
            &myTestClassObj);

        ENTROPY_CHECK_RETURN_VAL_FUNC(myTestClassObj._myFloat == 123.456f, 1,
                                      "Class value modified when it shouldn't be modified");
    }

    // Regular pointers can be returned normally.
    {
        MyDynFnTestClass myTestClassObj;

        TestCallClassMethod<float*>(
            &MyDynFnTestClass::GetMyFloatPtr,
            [](float* ret) {
                if (*ret != 123.456f)
                {
                    return false;
                }

                *ret += 100.0f;

                return true;
            },
            &myTestClassObj);

        ENTROPY_CHECK_RETURN_VAL_FUNC(myTestClassObj._myFloat == (123.456f + 100.0f), 1,
                                      "Failed to modify class value");
    }

    return 0;
}
