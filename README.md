# C++ Reflection Library
A no-dependency, type-safe, C++11 library that allows for both compile time and runtime reflection (plus more!).
This library does _not_ require you to add any pre or post build steps to your build pipeline.
There is no pre-processing sweep of the code. It is simply markup and run.

## Sample Declaration of a Reflected Class
```
#include "Entropy/Reflection.h"

// Struct works also...
class MyClass
{
public:
  // This is required for each reflected class. The macro parameter must match the name of the class.
  // It must appear above any other reflection related macros.

  ENTROPY_REFLECT_CLASS(MyClass)


  // This is required for each reflected member. The macro parameter must match the member name.
  // If a member does not have this macro, it will not be visible to the reflection system.
  // These macros can technically appear anywhere below the class macro and in any order, but by
  // convention, they are put directly above the member declaration.

  ENTROPY_REFLECT_MEMBER(MyVar)
  float MyVar{1.0f};
};
```

## Sample Query of a Reflected Class (Runtime Evaluated)
```
{
  using namespace Entropy;
  using namespace Entropy::Reflection;

  // Every C++ type can have a TypeInfo, not just reflected classes.
  // It is created when this method is called for the type for the first time.
  const TypeInfo* typeInfo = ReflectTypeAndGetTypeInfo<MyClass>();
  std::cout << "Type Name: " << typeInfo->GetTypeName() << "\n";

  // IsReflectedClass() will return true (with an available class description) only for
  // classes with the ENTROPY_REFLECT_CLASS() macro.
  const ClassTypeInfo& classInfo = typeInfo->Get<ClassTypeInfo>();
  if (classInfo.IsReflectedClass())
  {
    const ClassDescription* classDesc = classInfo.GetClassDescription();
    for (const auto& memberKVP : classDesc->GetMembers())
    {
      std::cout << "Member Name: " << memberKVP.first << "\n";
      std::cout << "Member Type: " << memberKVP.second.GetMemberType()->GetTypeName() << "\n\n";
    }
  }
}
```

## Recommended Make Integration
```
include(FetchContent)

FetchContent_Declare(
        entropy-reflection
        GIT_REPOSITORY https://github.com/entropy-software-io/reflection.git
    )

# Enable Runtime Reflection
set (ENTROPY_REFLECTION_ENABLE_RUNTIME ON)

FetchContent_MakeAvailable(entropy-reflection)

target_link_libraries([YOUR_LIBRARY] PRIVATE entropy::reflection)
```
## Features
### Compile Time Reflection
Compile time reflection allows you to create templated callbacks that are evaluated in a type-safe (compiler enforced) manner.
Each call for each member is compiled into the code.
While not as powerful as runtime reflection, this can be useful for serialization or to convert data between two similar classes.

> [!TIP]
> See the following folders for examples:
> * _Examples/CompileTimeReflection_ - Print out basic information of each member of a reflected class.
> * _Examples/TransformDataStructures_ - Copy data between two similar structs, converting types as needed.

### Runtime Object Creation
Given a ```TypeInfo```, you can allocate an instance (if the type allows). You are given back a ```DataObject``` which wraps a ```TypeInfo``` and a ```void*```. Safety checks are provided with casting methods to help prevent aiming the gun too close to your foot.

### Attributes
Every reflected member and class can be annotated with user-defined attributes in the form of classes / structs. Attributes may hold data, but the data should be set in the constructor of the attribute. Attributes and their associated data can be pulled from ```TypeInfo```.

> [!NOTE]
> Each class / member may only have one attribute of a particular type as they are stored internally as a map keyed off the attribute type.

Example:
```
struct MyAttribute
{
    MyAttribute(int val)
        : value(val)
    {
    }

    int value = 0;
};

struct MyStruct
{
    ENTROPY_REFLECT_CLASS(MyStruct, MyAttribute(123))
};

...

const TypeInfo* typeInfo = ReflectTypeAndGetTypeInfo<MyStruct>();
const ClassDescription* classDesc = typeInfo->Get<ClassTypeInfo>().GetClassDescription();

const MyAttribute* attr = classDesc->TryGetAttribute<MyAttribute>();
```

### Dynamic Function Calls
The ```DynamicFunction``` class wraps a callable type in a non-templated type. A single ```DynamicFunction``` object can be set to any ```std::function<>``` or ```std::mem_fn()``` type (think ```System.Reflection.MethodInfo``` in .NET).

When invoking a ```DynamicFunction``` object, you pass in fully typed objects. The function object ensures that the types are appropriate and line up. One draw back is that type conversion cannot happen. If a method takes some form of an ```std::string```, you cannot pass in a ```const char*```.

When used with ```TypeInfo```, this class allows for a fully automated UI property system. ```TypeInfo``` is used to gather the names of classes and members while ```DynamicFunction``` is used to get and set the values of object instances.

### Extending
