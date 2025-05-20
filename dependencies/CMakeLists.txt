include(FetchContent)

FetchContent_Declare(
        entropy-core-common
        GIT_REPOSITORY https://github.com/entropy-software-io/core-common.git
        GIT_TAG        572efcf9bde3a0268ad844e2f2d6d84e960508eb
    )

FetchContent_MakeAvailable(entropy-core-common)
