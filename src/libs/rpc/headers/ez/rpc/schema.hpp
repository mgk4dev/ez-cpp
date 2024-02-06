#pragma once

#include <ez/rpc/function.hpp>

#define EZ_RPC_NAMESAPCE(name) static inline constexpr std::string_view name_space = EZ_STR(name)
