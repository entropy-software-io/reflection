// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#include "Entropy/DynamicFunction.h"
#include "Entropy/Reflection.h"
#include "Entropy/Reflection/Details/TypeId.h"
#include <iostream>

class MyClass
{
public:
    MyClass(std::string&& name)
        : _myName(std::move(name))
    {
    }

    inline void MyFunction(int x, const std::string& y)
    {
        std::cout << "MyFunction called on object '" << _myName << "' with parameters [x: " << x << "] [y: " << y << "]"
                  << std::endl;
    }

    inline void MyConstFunction(int x, const std::string& y) const
    {
        std::cout << "MyFunction called on object '" << _myName << "' with parameters [x: " << x << "] [y: " << y << "]"
                  << std::endl;
    }

    inline int MyFunction2(int x)
    {
        std::cout << "MyFunction called on object '" << _myName << "' with parameters [x: " << x << "]" << std::endl;
        return x + 100;
    }

private:
    std::string _myName;
};

int main(int argc, char* argv[]) noexcept
{
    using namespace Entropy;

    MyClass myClass("myClass");

    {
        std::function<void(MyClass*, int, const std::string&)> fn =
            std::bind(&MyClass::MyFunction, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

        DynamicFunction func(std::move(fn));
        func(&myClass, 123, std::string("Foo"));
    }

    {
        auto fn = std::mem_fn(&MyClass::MyFunction);

        DynamicFunction func(std::move(fn));
        func(&myClass, 123, std::string("Foo"));
    }

    {
        auto fn = std::mem_fn(&MyClass::MyConstFunction);

        DynamicFunction func(std::move(fn));
        func(&myClass, 123, std::string("Foo"));
    }

    {
        auto fn = std::mem_fn(&MyClass::MyConstFunction);

        DynamicFunction func(std::move(fn));
        func(&myClass, 123, std::string("Foo"));
    }

    {
        auto fn = std::mem_fn(&MyClass::MyFunction2);

        DynamicFunction func(std::move(fn));

        int ret;
        func.InvokeWithReturnValue(ret, &myClass, 123);

        std::cout << "Ret: " << ret << std::endl;
    }

    return 0;
}
