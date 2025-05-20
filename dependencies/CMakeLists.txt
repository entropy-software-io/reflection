include(FetchContent)

FetchContent_Declare(
        entropy-core-common
        GIT_REPOSITORY https://github.com/entropy-software-io/core-common.git
        GIT_TAG        4698899406c750297484735f562b9db0f0f7dd9d
    )

FetchContent_MakeAvailable(entropy-core-common)
