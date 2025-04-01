// Copyright (c) Entropy Software LLC
// This file is licensed under the MIT License.
// See the LICENSE file in the project root for more information.

#pragma once

#include <type_traits>

#ifndef ENTROPY_CONSTEXPR
#if __cplusplus >= 201703L
#define ENTROPY_CONSTEXPR constexpr
#else
#define ENTROPY_CONSTEXPR
#endif
#endif
