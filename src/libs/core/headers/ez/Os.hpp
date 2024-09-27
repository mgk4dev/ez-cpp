#pragma once

namespace ez::os {

#if defined(_MSC_VER)
constexpr bool windows = true;
constexpr bool linux_ = false;
constexpr bool mac = false;
#define EZ_OS_WINDOWS 1
#elif defined(__GNUC__) && !defined(__APPLE__)
constexpr bool windows = false;
constexpr bool linux_ = true;
constexpr bool mac = false;
#define EZ_OS_LINUX 1
#elif defined(__GNUC__) && defined(__APPLE__)
constexpr bool windows = false;
constexpr bool linux_ = false;
constexpr bool mac = true;
#define EZ_OS_MACOS 1
#else
constexpr bool windows = false;
constexpr bool linux_ = false;
constexpr bool mac = false;
#endif
}  // namespace ez::os
