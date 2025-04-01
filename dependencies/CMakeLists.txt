include(FetchContent)

FetchContent_Declare(
        entropy-core-common
        GIT_REPOSITORY https://github.com/entropy-software-io/core-common.git
    )

if (${ENTROPY_REFLECTION_ENABLE_RUNTIME})
    FetchContent_Declare(
        entropy-core
        GIT_REPOSITORY https://github.com/entropy-software-io/core.git
        GIT_TAG origin/working
    )
endif()

FetchContent_MakeAvailable(entropy-core-common)

if (${ENTROPY_REFLECTION_ENABLE_RUNTIME})
    FetchContent_MakeAvailable(entropy-core)
endif()