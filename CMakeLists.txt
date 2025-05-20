option(ENTROPY_REFLECTION_ENABLE_RUNTIME "True to enable runtime reflection (requires bringing in Entropy Core dependency)" ON)
option(ENTROPY_REFLECTION_BUILD_EXAMPLES "True to build example projects" OFF)
option(ENTROPY_REFLECTION_BUILD_TESTS "True to build tests" OFF)

# Configuration Options
option (ENTROPY_REFLECTION_TYPEINFO_INCLUDE_DEFAULT_MODULES "False to exclude the default TypeInfo modules" ON)

# Example config values
# set(ENTROPY_REFLECTION_TYPEINFO_EXTRA_MODULE_LIST_INCLUDES "#include \"Entropy/Reflection/TypeInfoModules/ClassTypeInfo.h\"")
# set(ENTROPY_REFLECTION_TYPEINFO_EXTRA_MODULE_LIST_IMPL_INCLUDES "#include \"Entropy/Reflection/TypeInfoModules/ClassTypeInfo.inl\"")
# set(ENTROPY_REFLECTION_TYPEINFO_EXTRA_MODULE_LIST "ClassTypeInfo")
# set(ENTROPY_REFLECTION_EXTRA_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/Include")
# set(ENTROPY_REFLECTION_EXTRA_LINK_LIBRARIES "")

if ("${ENTROPY_REFLECTION_TYPEINFO_EXTRA_MODULE_LIST}" STREQUAL "")
    set (ENTROPY_REFLECTION_TYPEINFO_HAVE_EXTRA_MODULES OFF)
else()
    set (ENTROPY_REFLECTION_TYPEINFO_HAVE_EXTRA_MODULES ON)
endif()

add_subdirectory(Dependencies)

configure_file(Config/TypeInfoModuleList.h.in Include/Entropy/Reflection/TypeInfo/TypeInfoModuleList.h)
configure_file(Config/TypeInfoModuleListImpl.h.in Include/Entropy/Reflection/TypeInfo/TypeInfoModuleListImpl.h)

set(ENTROPY_NAMESPACE entropy)

set(REFLECTION_SRC
    Src/DataObject/DataObject.cpp
    Src/TypeInfo/TypeInfo.cpp
    Src/TypeInfo/TypeInfoRef.cpp
    Src/TypeInfoModules/ClassTypeInfo.cpp
)
add_library(entropy-reflection OBJECT ${REFLECTION_SRC})
add_library(${ENTROPY_NAMESPACE}::reflection ALIAS entropy-reflection)

target_include_directories(entropy-reflection PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/Include>
    $<INSTALL_INTERFACE:Include>
)

if (NOT "${ENTROPY_REFLECTION_EXTRA_INCLUDE_DIRECTORIES}" STREQUAL "")
    target_include_directories(entropy-reflection PUBLIC ${ENTROPY_REFLECTION_EXTRA_INCLUDE_DIRECTORIES})
endif()

if (NOT "${ENTROPY_REFLECTION_EXTRA_LINK_LIBRARIES}" STREQUAL "")
    target_link_libraries(entropy-reflection PUBLIC ${ENTROPY_REFLECTION_EXTRA_LINK_LIBRARIES})
endif()

target_link_libraries(entropy-reflection PUBLIC entropy::core::common)

if (${ENTROPY_REFLECTION_ENABLE_RUNTIME})
    target_compile_definitions(entropy-reflection PUBLIC ENTROPY_RUNTIME_REFLECTION_ENABLED)
endif()

if (${ENTROPY_REFLECTION_BUILD_EXAMPLES})
    add_subdirectory(Examples)
endif()

if (${ENTROPY_REFLECTION_BUILD_TESTS})
    add_subdirectory(Tests)
endif()
