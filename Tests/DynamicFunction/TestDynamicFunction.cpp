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

void EmptyVoidFunction() { sMethodCalledState = MethodCalledState::SuccessfullyCalled; }

void OverloadedVoidFunction(int x)
{
    sMethodCalledState = MethodCalledState::InvalidParam;

    if (x == 123456789)
    {
        sMethodCalledState = MethodCalledState::SuccessfullyCalled;
    }
}

void OverloadedVoidFunction(const char* str)
{
    sMethodCalledState = MethodCalledState::InvalidParam;

    if (std::strcmp(str, "Foo") == 0)
    {
        sMethodCalledState = MethodCalledState::SuccessfullyCalled;
    }
}

void OverloadedVoidFunction(const std::string& str)
{
    sMethodCalledState = MethodCalledState::InvalidParam;

    if (str == "Foo")
    {
        sMethodCalledState = MethodCalledState::SuccessfullyCalled;
    }
}

//---------------------

class MyDynFnTestClass
{
public:
    void EmptyVoidFunction() { sMethodCalledState = MethodCalledState::SuccessfullyCalled; }

    void OverloadedVoidFunction(int x)
    {
        sMethodCalledState = MethodCalledState::InvalidParam;

        if (x == 123456789)
        {
            sMethodCalledState = MethodCalledState::SuccessfullyCalled;
        }

        VerifyObjectState();
    }

    void OverloadedVoidFunction(const char* str)
    {
        sMethodCalledState = MethodCalledState::InvalidParam;

        if (std::strcmp(str, "Foo") == 0)
        {
            sMethodCalledState = MethodCalledState::SuccessfullyCalled;
        }

        VerifyObjectState();
    }

    void OverloadedVoidFunction(const std::string& str)
    {
        sMethodCalledState = MethodCalledState::InvalidParam;

        if (str == "Foo")
        {
            sMethodCalledState = MethodCalledState::SuccessfullyCalled;
        }

        VerifyObjectState();
    }

    //--------

    void EmptyVoidFunctionConst() const { sMethodCalledState = MethodCalledState::SuccessfullyCalled; }

    void OverloadedVoidFunctionConst(int x) const
    {
        sMethodCalledState = MethodCalledState::InvalidParam;

        if (x == 123456789)
        {
            sMethodCalledState = MethodCalledState::SuccessfullyCalled;
        }

        VerifyObjectState();
    }

    void OverloadedVoidFunctionConst(const char* str) const
    {
        sMethodCalledState = MethodCalledState::InvalidParam;

        if (std::strcmp(str, "Foo") == 0)
        {
            sMethodCalledState = MethodCalledState::SuccessfullyCalled;
        }

        VerifyObjectState();
    }

    void OverloadedVoidFunctionConst(const std::string& str) const
    {
        sMethodCalledState = MethodCalledState::InvalidParam;

        if (str == "Foo")
        {
            sMethodCalledState = MethodCalledState::SuccessfullyCalled;
        }

        VerifyObjectState();
    }

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

    std::string _myStr{"Bar"};
    float _myFloat{123.456f};
};

//---------------------

template <typename TFunc, typename... TArgs>
bool TestCallVoidFunction(TFunc func, TArgs&&... args)
{
    sMethodCalledState = MethodCalledState::NotCalled;

    std::function<typename std::remove_pointer<TFunc>::type> funcWrapper = func;
    Entropy::DynamicFunction dynFn(std::move(funcWrapper));

    ENTROPY_CHECK_RETURN_VAL_FUNC(dynFn(std::forward<TArgs>(args)...), false,
                                  "Failed to invoke function of type '" << Entropy::Traits::TypeNameOf<TFunc>{}()
                                                                        << "'");

    ENTROPY_CHECK_RETURN_VAL_FUNC(sMethodCalledState != MethodCalledState::NotCalled, false,
                                  "Function of type '" << Entropy::Traits::TypeNameOf<TFunc>{}()
                                                       << "' was not invoked");
    ENTROPY_CHECK_RETURN_VAL_FUNC(sMethodCalledState != MethodCalledState::InvalidParam, false,
                                  "Function of type '" << Entropy::Traits::TypeNameOf<TFunc>{}()
                                                       << "' was invoked with bad or corrupted parameters");

    return true;
}

template <typename TMethod, typename... TArgs>
bool TestCallVoidClassMethod(TMethod method, TArgs&&... args)
{
    sMethodCalledState = MethodCalledState::NotCalled;

    auto methodWrapper = std::mem_fn(method);
    Entropy::DynamicFunction dynFn(std::move(methodWrapper));

    ENTROPY_CHECK_RETURN_VAL_FUNC(dynFn(std::forward<TArgs>(args)...), false,
                                  "Failed to invoke method of type '" << Entropy::Traits::TypeNameOf<TMethod>{}()
                                                                      << "'");

    ENTROPY_CHECK_RETURN_VAL_FUNC(sMethodCalledState != MethodCalledState::NotCalled, false,
                                  "Method of type '" << Entropy::Traits::TypeNameOf<TMethod>{}()
                                                     << "' was not invoked");
    ENTROPY_CHECK_RETURN_VAL_FUNC(sMethodCalledState != MethodCalledState::InvalidParam, false,
                                  "Method of type '" << Entropy::Traits::TypeNameOf<TMethod>{}()
                                                     << "' was invoked with bad or corrupted parameters");

    return true;
}

int DynamicFunction_TestDynamicFunction(int argc, char** const argv)
{
    ENTROPY_VERIFY_FUNC(TestCallVoidFunction(&EmptyVoidFunction));
    ENTROPY_VERIFY_FUNC(TestCallVoidFunction<void(int)>(&OverloadedVoidFunction, 123456789));

    // The method takes in an int that we cannot convert to from an int64
    ENTROPY_VERIFY_NOT_FUNC(TestCallVoidFunction<void(int)>(&OverloadedVoidFunction, 123456789LL));

    ENTROPY_VERIFY_FUNC(TestCallVoidFunction<void(const std::string&)>(&OverloadedVoidFunction, std::string("Foo")));

    // Conversion from string constant to std::string is not allowed
    ENTROPY_VERIFY_NOT_FUNC(TestCallVoidFunction<void(const std::string&)>(&OverloadedVoidFunction, "Foo"));

    // We can convert from const char[4] to const char*
    ENTROPY_VERIFY_FUNC(TestCallVoidFunction<void(const char*)>(&OverloadedVoidFunction, "Foo"));

    //-----------

    MyDynFnTestClass myTestClassObj;

    ENTROPY_VERIFY_FUNC(TestCallVoidClassMethod(&MyDynFnTestClass::EmptyVoidFunction, &myTestClassObj));
    ENTROPY_VERIFY_FUNC(TestCallVoidClassMethod<void (MyDynFnTestClass::*)(int)>(
        &MyDynFnTestClass::OverloadedVoidFunction, &myTestClassObj, 123456789));

    // The method takes in an int that we cannot convert to from an int64
    ENTROPY_VERIFY_NOT_FUNC(TestCallVoidClassMethod<void (MyDynFnTestClass::*)(int)>(
        &MyDynFnTestClass::OverloadedVoidFunction, &myTestClassObj, 123456789LL));

    ENTROPY_VERIFY_FUNC(TestCallVoidClassMethod<void (MyDynFnTestClass::*)(const std::string&)>(
        &MyDynFnTestClass::OverloadedVoidFunction, &myTestClassObj, std::string("Foo")));

    // Conversion from string constant to std::string is not allowed
    ENTROPY_VERIFY_NOT_FUNC(TestCallVoidClassMethod<void (MyDynFnTestClass::*)(const std::string&)>(
        &MyDynFnTestClass::OverloadedVoidFunction, &myTestClassObj, "Foo"));

    // We can convert from const char[4] to const char*
    ENTROPY_VERIFY_FUNC(TestCallVoidClassMethod<void (MyDynFnTestClass::*)(const char*)>(
        &MyDynFnTestClass::OverloadedVoidFunction, &myTestClassObj, "Foo"));

    //-----------

    ENTROPY_VERIFY_FUNC(TestCallVoidClassMethod(&MyDynFnTestClass::EmptyVoidFunctionConst, &myTestClassObj));
    ENTROPY_VERIFY_FUNC(TestCallVoidClassMethod<void (MyDynFnTestClass::*)(int) const>(
        &MyDynFnTestClass::OverloadedVoidFunctionConst, &myTestClassObj, 123456789));

    // The method takes in an int that we cannot convert to from an int64
    ENTROPY_VERIFY_NOT_FUNC(TestCallVoidClassMethod<void (MyDynFnTestClass::*)(int) const>(
        &MyDynFnTestClass::OverloadedVoidFunctionConst, &myTestClassObj, 123456789LL));

    ENTROPY_VERIFY_FUNC(TestCallVoidClassMethod<void (MyDynFnTestClass::*)(const std::string&) const>(
        &MyDynFnTestClass::OverloadedVoidFunctionConst, &myTestClassObj, std::string("Foo")));

    // Conversion from string constant to std::string is not allowed
    ENTROPY_VERIFY_NOT_FUNC(TestCallVoidClassMethod<void (MyDynFnTestClass::*)(const std::string&) const>(
        &MyDynFnTestClass::OverloadedVoidFunctionConst, &myTestClassObj, "Foo"));

    // We can convert from const char[4] to const char*
    ENTROPY_VERIFY_FUNC(TestCallVoidClassMethod<void (MyDynFnTestClass::*)(const char*) const>(
        &MyDynFnTestClass::OverloadedVoidFunctionConst, &myTestClassObj, "Foo"));

    return 0;
}
