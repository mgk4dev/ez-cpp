#pragma once

#include <ez/Utils.hpp>

namespace ez::rpl {

enum class ProcessingMode { Incremental, Batch };

}  // namespace ez::rpl

#define EZ_RPL_STAGE_INFO(in_mode, out_mode)               \
    static constexpr auto input_processing_mode = in_mode; \
    static constexpr auto output_processing_mode = out_mode;
