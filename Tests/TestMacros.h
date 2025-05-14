// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

#include "Entropy/Core/Log.h"

#define ENTROPY_VERIFY_FUNC(...)                                                                                       \
    do                                                                                                                 \
    {                                                                                                                  \
        bool ret = !!(__VA_ARGS__);                                                                                    \
        if (!ret)                                                                                                      \
        {                                                                                                              \
            ENTROPY_LOG_ERROR_FUNC("Verify failed: " << #__VA_ARGS__);                                                 \
            return 1;                                                                                                  \
        }                                                                                                              \
    } while (false)

#define ENTROPY_VERIFY_NOT_FUNC(...)                                                                                   \
    do                                                                                                                 \
    {                                                                                                                  \
        bool ret = !!(__VA_ARGS__);                                                                                    \
        if (ret)                                                                                                       \
        {                                                                                                              \
            ENTROPY_LOG_ERROR_FUNC("Verify not failed: " << #__VA_ARGS__);                                             \
            return 1;                                                                                                  \
        }                                                                                                              \
    } while (false)
