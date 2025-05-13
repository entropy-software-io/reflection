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

template <typename TFunc, typename... TArgs>
bool TestCallVoidFunction(TFunc* func, TArgs&&... args)
{
    sMethodCalledState = MethodCalledState::NotCalled;

    std::function<TFunc> funcWrapper = func;
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

    return 0;
}
