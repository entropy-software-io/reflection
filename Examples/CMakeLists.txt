set(ENTROPY_NAMESPACE ${ENTROPY_NAMESPACE}::Examples)

add_subdirectory(CompileTimeReflection)
add_subdirectory(TransformDataStructures)

if (${ENTROPY_REFLECTION_ENABLE_RUNTIME})
    add_subdirectory(RuntimeReflection)
    add_subdirectory(RuntimeReflectionConstruction)
    add_subdirectory(DynamicMethodCalls)
endif()
