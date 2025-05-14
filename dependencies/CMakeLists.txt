include(FetchContent)

FetchContent_Declare(
        entropy-core-common
        GIT_REPOSITORY https://github.com/entropy-software-io/core-common.git
        GIT_TAG        ca9decf4c4c0f22b2cc241a4e209778ac3b68872
    )

FetchContent_MakeAvailable(entropy-core-common)
