#pragma once

#include <ez/Utils.hpp>

namespace ez::rpl {

enum class ProcessingStyle { Incremental, All };

}  // namespace ez::rpl

#define EZ_RPL_STAGE_INFO(in_style, out_style)               \
    static constexpr auto input_processing_style = in_style; \
    static constexpr auto output_processing_style = out_style;
