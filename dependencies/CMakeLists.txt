include(FetchContent)

FetchContent_Declare(
        entropy-core-common
        GIT_REPOSITORY https://github.com/entropy-software-io/core-common.git
        GIT_TAG        25ac9b5783cb1525eb902f5b87f5e25bc90cd5a5
    )

FetchContent_MakeAvailable(entropy-core-common)
