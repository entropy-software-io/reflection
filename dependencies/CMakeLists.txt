include(FetchContent)

if (${ENTROPY_REFLECTION_ENABLE_RUNTIME})
    FetchContent_Declare(
        entropy-core
        GIT_REPOSITORY https://github.com/entropy-software-io/core.git
        GIT_TAG origin/working
    )

    FetchContent_MakeAvailable(entropy-core)
endif()
