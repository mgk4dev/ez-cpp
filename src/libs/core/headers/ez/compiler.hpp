#pragma once

// Source: https://github.com/facebook/folly/blob/master/folly/Portability.h

/*
 * Copyright 2011-present Facebook, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// Generalize warning push/pop.
#if defined(_MSC_VER)
#define EZ_PUSH_WARNING __pragma(warning(push))
#define EZ_POP_WARNING __pragma(warning(pop))
// Disable the GCC warnings.
#define EZ_GNU_DISABLE_WARNING(warningName)
#define EZ_GCC_DISABLE_WARNING(warningName)
#define EZ_CLANG_DISABLE_WARNING(warningName)
#define EZ_MSVC_DISABLE_WARNING(warningNumber) __pragma(warning(disable : warningNumber))
#elif defined(__GNUC__)
// Clang & GCC
#define EZ_PUSH_WARNING _Pragma("GCC diagnostic push")
#define EZ_POP_WARNING _Pragma("GCC diagnostic pop")
#define EZ_GNU_DISABLE_WARNING_INTERNAL2(warningName) #warningName
#define EZ_GNU_DISABLE_WARNING(warningName)                                                                           \
_Pragma(EZ_GNU_DISABLE_WARNING_INTERNAL2(GCC diagnostic ignored warningName))
#ifdef __clang__
#define EZ_CLANG_DISABLE_WARNING(warningName) EZ_GNU_DISABLE_WARNING(warningName)
#define EZ_GCC_DISABLE_WARNING(warningName)
#else
#define EZ_CLANG_DISABLE_WARNING(warningName)
#define EZ_GCC_DISABLE_WARNING(warningName) EZ_GNU_DISABLE_WARNING(warningName)
#endif
#define EZ_MSVC_DISABLE_WARNING(warningNumber)
#else
#define EZ_PUSH_WARNING
#define EZ_POP_WARNING
#define EZ_GNU_DISABLE_WARNING(warningName)
#define EZ_GCC_DISABLE_WARNING(warningName)
#define EZ_CLANG_DISABLE_WARNING(warningName)
#define EZ_MSVC_DISABLE_WARNING(warningNumber)
#endif
