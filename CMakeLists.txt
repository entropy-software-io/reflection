option(ENTROPY_REFLECTION_ENABLE_RUNTIME "True to enable runtime reflection (requires bringing in Entropy Core dependency)" ON)
option(ENTROPY_BUILD_EXAMPLES "True to build example projects" ON)

add_subdirectory(Dependencies)

set(ENTROPY_NAMESPACE entropy)

set(REFLECTION_SRC "src/Test.cpp")
add_library(entropy-reflection OBJECT ${REFLECTION_SRC})
add_library(${ENTROPY_NAMESPACE}::reflection ALIAS entropy-reflection)

target_include_directories(entropy-reflection PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>
    $<INSTALL_INTERFACE:Include>
)

target_link_libraries(entropy-reflection PUBLIC entropy::core::common)

if (${ENTROPY_REFLECTION_ENABLE_RUNTIME})
    target_compile_definitions(entropy-reflection PUBLIC ENTROPY_RUNTIME_REFLECTION_ENABLED)
endif()

if (${ENTROPY_BUILD_EXAMPLES})
    add_subdirectory(examples)
endif()