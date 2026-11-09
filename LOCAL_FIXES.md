# Tested Ocarina fixes

The exact local runtime source is [a55df450](https://github.com/Marssvoodoo/Lichii/commit/a55df4503490363997a2a355e6fa35e3ce707167), tagged `cache-retention-candidate-20261108` on [the tested branch](https://github.com/Marssvoodoo/Lichii/tree/tested/oot-pipeline-fixes-20261108). It combines prepared shader replay, the worker cleanup fix and pipeline retention within a RAM budget. The source tag identifies the binary; this notes branch adds documentation only.

All five native suites passed (998 core cases); the retention policy adds 3 cases/14 assertions. The worker regression fails before the cleanup fix and passes afterward, including 50 repeated runs.

Runtime on Windows / RTX 5090: prepared 136,827 shader records reused in 3.8 seconds; no cache eviction logged after 29,989 presents / 512 seconds, with a 13,930-pipeline retention budget. Two validated camera-only segments (30 seconds input) had no frame intervals above 50 ms, worst 32.39 ms. This is a narrow route, not evidence that all first-use stutter is solved.

NVIDIA DLSS Super Resolution was separately verified evaluating 2560x1440 to 3840x2160, using Streamline 2.14.16 / NGX 310.9.16 preset K. Two camera segments ran 59.00 and 59.91 FPS, no intervals over 50 ms, worst 36.62 ms. No FPS increase is established in this already 60 FPS scene. DLSS is enabled locally; no DLSS 5 integration is claimed. Screenshots were inspected, but long-play temporal quality remains unverified.

## Upstream review - 2026-11-08

Checked current Lichii nx2 cbb512ee7a9d and Eglantine nx2 78743f30c251. The installed/tested runtime is a55df4503490; it is pinned to PR #896 plus the worker cleanup fix and a surgical retention port.

| Change | Decision |
|---|---|
| Lichii [PRs #895 and #896](https://github.com/zortos293/Lichii/pull/896), head `6a77bbba` | Included: prepared shader Info/SPIR-V replay and pointer-state fallback. The 136,827-record preparation is reused; it does not prove every future draw is covered. |
| Lichii [PR #408](https://github.com/zortos293/Lichii/pull/408), budget commit `4b65b8f8` | Included only the RAM budget and oldest-excess eviction policy, credited to ZeroOneTV. Kept shader/key/cache formats unchanged. Other PR #408 key changes require separate cache-version handling. |
| Our [PR #939](https://github.com/zortos293/Lichii/pull/939) | Submitted: release task captures before incrementing work_done. Tested cleanup-order regression; not a claim that all hitching is fixed. |
| Lichii [PR #579](https://github.com/zortos293/Lichii/pull/579) | Investigated; latest author retesting still reports first-visit stalls. Do not stack the whole evolving shader-library branch into the current prepared-cache build. |
| Eglantine [PR #769](https://github.com/Eglantine-Community/Eglantine/pull/769) | Reviewed code changes. A broad 304-file experimental bundle with draw-budget skipping, background prebuild, shader lowering and FPS patches. No blanket merge into the measured build. |
| Lichii [PR #902](https://github.com/zortos293/Lichii/pull/902), from Eglantine PR #769 | Worth a focused follow-up: removes a global SPIR-V emission race. Already merged into current upstream nx2 and thus the fork's nx2 branch; absent from the pinned installed candidate. Author reports unchanged SPIR-V, but our strict eviction-only compatibility proof does not cover this additional patch. |
| Lichii [PR #910](https://github.com/zortos293/Lichii/pull/910) and [PR #921](https://github.com/zortos293/Lichii/pull/921) | Fix null-image/alias guard bugs in newer memoized fast paths. Those fast paths are absent in the pinned candidate; do not copy fixes without their prerequisites or import the optimization bundle untested. Current fork nx2 already includes them. |
| Lichii [PR #878](https://github.com/zortos293/Lichii/pull/878) | Lowers warm-precache memory; runtime builds explicitly unchanged. Useful for a future refresh, not a remedy for the observed in-game waits on this high-RAM machine. |
| Eglantine [PR #680](https://github.com/Eglantine-Community/Eglantine/pull/680) | OptiScaler/Reflex/PCL integration; tested by author in Fire Emblem. Current stock Lichii already enables manual hooking. No OptiScaler is installed or requested, so this does not justify the 290-line integration port for OoT. |

DLSS: official public [Streamline 2.14.16](https://github.com/NVIDIA-RTX/Streamline/releases/tag/v2.14.16) / NGX 310.9.16 package verified against its release SHA256 and NVIDIA signatures. The integration requests DLSS Super Resolution; this is not a DLSS 5 neural-rendering integration. Actual runtime evaluation and screenshots, not the Ready label, determine whether DLSS is functioning.

Eglantine worker patch: published in the owner's [fork branch](https://github.com/Marssvoodoo/Eglantine/tree/fix/eglantine-worker-cleanup). Focused test uses the production worker header, with OS thread naming/affinity stubbed: before fails the cleanup-order assertion, after passes, 50/50 repeats. A full Eglantine app was not built. Upstream submission awaits human-authored commit/PR text under its [AI policy](https://github.com/Eglantine-Community/Eglantine/blob/nx2/docs/policies/AI.md).
