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

Example: Iterating through all data members in reflected order. See the _Exmaples/CompileTimeReflection_ folder for the full code.
```
#include "Entropy/Reflection.h"

// Our custom operation to perform on the reflected class
struct PrintName
{
    //
    // This will be called for each member. A parameter of type "Entropy::AttributeCollection<TAttrTypes...>&&"
    // can be added if you wish to get the list of attributes for each member too.
    //
    template <typename TMember>
    void operator()(const char* memberName, const TMember& memberValue)
    {
        std::cout << "Member: " << memberName << ", Value: " << memberValue << std::endl;
    }
};

...

void SomeFunction()
{
    MyReflectedClass myClass;

    // This will PrintName's operator() method for each member.
    Entropy::ForEachReflectedMember<true>(myClass, PrintName{});
}
```

Example: Iterating through each same named member of two classes to transform the data. See the _Exmaples/TransformDataStructures_ folder for the full code.
```
// Perhaps this is how we represent 3D vectors internally...
struct Vector3
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

// And this is how we expose 3D vectors in an external API...
struct Array3
{
    float values[3] = {0};
};

// Our data structure that we use internally and do not share
struct MyInternalStruct
{
    ENTROPY_REFLECT_CLASS(MyInternalStruct)

    ENTROPY_REFLECT_MEMBER(MyFloatValue)
    float MyFloatValue = 1.23f;

    ENTROPY_REFLECT_MEMBER(PositionValue)
    Vector3 PositionValue;

    ENTROPY_REFLECT_MEMBER(MyInternalOnlyInt)
    int MyInternalOnlyInt = 123;
};

// This is what we expose externally
struct MyExternalStruct
{
    ENTROPY_REFLECT_CLASS(MyExternalStruct)

    ENTROPY_REFLECT_MEMBER(MyFloatValue)
    float MyFloatValue;

    ENTROPY_REFLECT_MEMBER(PositionValue)
    Array3 PositionValue;

    ENTROPY_REFLECT_MEMBER(MyExternalOnlyInt)
    int MyExternalOnlyInt;
};

// This will be called for each same named member. Note: MyInternalOnlyInt and MyExternalOnlyInt will not be evaluated because they only in either struct.
struct Conversion
{
    // General copy
    template <typename TMemberA, typename TMemberB>
    void operator()(const char* name, const TMemberA& src, TMemberB& dest)
    {
        std::cout << "Copying member [Name: " << name << "] [Value: " << src << "]" << std::endl;
        dest = src;
    }

    // Special case to convert Vector3 to Array3
    template <>
    void operator()(const char* name, const Vector3& src, Array3& dest)
    {
        std::cout << "Copying Vector3 value [Name: " << name << "] [Value: " << src.x << ", " << src.y << ", " << src.z
                  << "]" << std::endl;
        dest.values[0] = src.x;
        dest.values[1] = src.y;
        dest.values[2] = src.z;
    }
};

int main(int argc, char* argv[])
{
    MyInternalStruct internalStruct{};
    internalStruct.PositionValue.x = 1.0f;
    internalStruct.PositionValue.y = 2.0f;
    internalStruct.PositionValue.z = 3.0f;

    MyExternalStruct externalStruct{};

    // "true" means to also iterate through base classes (which we do not have in this example)
    Entropy::ForEachReflectedMemberInBoth<true>(internalStruct, externalStruct, Conversion{});

    return 0;
}
```

