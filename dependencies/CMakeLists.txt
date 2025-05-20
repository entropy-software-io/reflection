include(FetchContent)

FetchContent_Declare(
        entropy-core-common
        GIT_REPOSITORY https://github.com/entropy-software-io/core-common.git
        GIT_TAG        66b8575c9f883b4e0289990455c3a5628199111b
    )

FetchContent_MakeAvailable(entropy-core-common)
