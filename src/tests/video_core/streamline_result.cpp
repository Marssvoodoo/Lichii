// SPDX-License-Identifier: GPL-3.0-or-later

#include <limits>

#include <catch2/catch_test_macros.hpp>

#include "video_core/dlss_evaluation.h"
#include "video_core/vulkan_common/streamline_api.h"

TEST_CASE("Streamline VRAM warnings preserve a completed DLSS evaluation",
          "[video_core][dlss_contract][streamline]") {
    const auto evaluated = Vulkan::Streamline::DlssEvaluationSucceeded;
    CHECK(evaluated(sl::Result::eOk));
    CHECK(evaluated(sl::Result::eWarnOutOfVRAM));
    for (unsigned value = 1; value < static_cast<unsigned>(sl::Result::eWarnOutOfVRAM); ++value) {
        CHECK_FALSE(evaluated(static_cast<sl::Result>(value)));
    }
    CHECK_FALSE(evaluated(static_cast<sl::Result>((std::numeric_limits<unsigned>::max)())));

    bool restored = false;
    const auto result = VideoCommon::RecordDlssEvaluation(
        [] { return true; }, [] { return true; }, [] {}, [] { return true; },
        [&] { return evaluated(sl::Result::eWarnOutOfVRAM); }, [&] { restored = true; });
    CHECK(result == VideoCommon::DlssEvaluationResult::Succeeded);
    CHECK(restored);
}
