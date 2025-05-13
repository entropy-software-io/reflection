# C++ Reflection Library
A no-dependency, C++11 library that allows for both compile time and runtime reflection (plus more!).
This library does _not_ require you to add any pre or post build steps to your build pipeline.
There is no pre-processing sweep of the code. It is simply markup and run.

## Sample Declaration of a Reflected Class
```
#include "Entropy/Reflection.h"

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
