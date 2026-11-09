# Ocarina graphics pipeline libraries: source review

Source review completed before any configure, compiler or build invocation. Thirty distinct scoped passes by one reviewer; this is not thirty independent reviewers. Compilation, tests and runtime validation are pending at this source checkpoint.

Baseline 485af70dd2fa527d06085be5a69d83a9eee8fb15. Integrated Lichii nx2 and GPL PR 579 pinned at cf996d0a5e79848653efc709005223318bcd2e37. Original installation, saves and prepared shader data are preserved.

| Pass | Area |
|---|---|
| 1 | Vulkan feature discovery/enable chains |
| 2 | Unsupported capability fallback |
| 3 | Stage binding and SPIR-V descriptor sets |
| 4 | Descriptor counts/limits/visibility |
| 5 | Pipeline layout/push constants |
| 6 | Vertex input/input assembly |
| 7 | Pre-rasterization completeness |
| 8 | Geometry/tessellation/mesh/task/TF paths |
| 9 | Fragment interface/interpolation |
| 10 | Fragment output/blend/render pass |
| 11 | Multisample/depth/stencil sharing |
| 12 | Dynamic state/pNext/creation flags |
| 13 | Library cache keys/hash/padding |
| 14 | Cache synchronization/single flight |
| 15 | Failed library builds/fallback |
| 16 | Worker capture/completion ordering |
| 17 | Handle/layout/library ownership |
| 18 | Optimized replacement publication |
| 19 | Retirement versus GPU fences |
| 20 | Optimizer cancellation/shutdown |
| 21 | Disk cache/stamp/profile persistence |
| 22 | Warm record generation/parse bounds |
| 23 | Warm cancellation/restart/markers |
| 24 | Compute/non-library paths |
| 25 | Wait/Bounded/Skip/clear policy |
| 26 | Descriptor buffer/push bind paths |
| 27 | Inline control/shader fix preservation |
| 28 | Memory retention/eviction/pressure |
| 29 | Game-file/IPC/local correctness guards |
| 30 | Integrated callers/tests/build/rollback snapshot |

Review fixes: five-set capability limit; fragment scaling offset on delta draws; allocation-safe library cache insertion; compute completion on every failure; join optimized links before final driver-cache snapshot; no-inline profile in retained SPIR-V stamp; exact emitted words for diagnostics, kept modules and audit; overflow-safe warm-record table extents.

Added regression sources cover capability boundaries, concurrent library failures, no-retain retries, optional-link cancellation and malformed cache counts. They must pass on the candidate build.

Runtime acceptance requires Wait mode with heavy/draw skipping disabled, observed correct rendering and measured forward travel. Shader warm-up uses guessed single-stage state; unseen state/partner variants can still compile at runtime. Runtime shared library retention grows with encountered unique shaders and needs memory observation. Cutscenes and full-game traversal are not verified by source review.
