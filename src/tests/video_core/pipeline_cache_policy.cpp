// SPDX-License-Identifier: GPL-3.0-or-later

#include <catch2/catch_test_macros.hpp>

#include "video_core/pipeline_cache_policy.h"

using namespace VideoCommon;

TEST_CASE("Graphics pipeline retention scales with host RAM", "[video_core][pipeline_retention]") {
    constexpr u64 GiB = u64{1} << 30;
    STATIC_REQUIRE(GraphicsPipelineBudget(0) == 0);
    STATIC_REQUIRE(GraphicsPipelineBudget(16 * GraphicsPipelineBytes - 1) == 0);
    STATIC_REQUIRE(GraphicsPipelineBudget(16 * GraphicsPipelineBytes) == 1);
    CHECK(GraphicsPipelineBudget(16 * GiB) == 2386);
    CHECK(GraphicsPipelineBudget(32 * GiB) == 4772);
}

TEST_CASE("Old graphics pipelines are kept within the RAM budget",
          "[video_core][pipeline_retention]") {
    constexpr u64 GiB = u64{1} << 30;
    CHECK(GraphicsPipelinesToEvict(0, 0, 100) == 0);
    CHECK(GraphicsPipelinesToEvict(50, 50, 100) == 0);
    CHECK(GraphicsPipelinesToEvict(100, 100, 100) == 0);
    // The live session evicted these small-cache entries solely because of their age.
    CHECK(GraphicsPipelinesToEvict(1836, 151, GraphicsPipelineBudget(96 * GiB)) == 0);
}

TEST_CASE("Graphics pipeline eviction removes only old excess entries",
          "[video_core][pipeline_retention]") {
    CHECK(GraphicsPipelinesToEvict(101, 101, 100) == 1);
    CHECK(GraphicsPipelinesToEvict(150, 80, 100) == 50);
    CHECK(GraphicsPipelinesToEvict(150, 20, 100) == 20);
    CHECK(GraphicsPipelinesToEvict(150, 0, 100) == 0);
    CHECK(GraphicsPipelinesToEvict(10, 7, 0) == 7);
}
