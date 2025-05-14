include(FetchContent)

FetchContent_Declare(
        entropy-core-common
        GIT_REPOSITORY https://github.com/entropy-software-io/core-common.git
        GIT_TAG        configure-customization
    )

FetchContent_MakeAvailable(entropy-core-common)
