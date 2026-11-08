// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <algorithm>
#include <cstddef>

#include "common/common_types.h"

namespace VideoCommon {

constexpr u64 GraphicsPipelineBytes = 450'000;

constexpr size_t GraphicsPipelineBudget(u64 total_physical_memory) {
    return static_cast<size_t>(total_physical_memory / 16 / GraphicsPipelineBytes);
}

constexpr size_t GraphicsPipelinesToEvict(size_t cached, size_t stale, size_t budget) {
    if (cached <= budget) {
        return 0;
    }
    return std::min(stale, cached - budget);
}

} // namespace VideoCommon
