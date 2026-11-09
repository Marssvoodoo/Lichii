# Ocarina forward-walking shader tests - 2026-11-08

The newer installed profile uses GPL and complete Wait-mode draws. See the
[2026-11-09 pipeline-library validation](GPL-PERFORMANCE-2026-11-09.md).

Walking into new ground reproduced long driver pipeline builds even after the
prepared shader infos were reused. Enabling an extra explicit Vulkan pipeline
cache did not eliminate the freezes. The existing per-game **Bounded wait** mode
was a useful workaround in the accepted movement samples.

Runtime source: `485af70dd2fa527d06085be5a69d83a9eee8fb15`.
Windows, RTX 5090, NVIDIA 621.14. No C++ changes or new binary were needed for
this settings experiment. [Sanitized timing evidence](FORWARD-WALK-2026-11-08.json).

| Mode | Accepted present intervals | Worst frame | Frames over 50 ms | Relevant driver builds |
|---|---:|---:|---:|---|
| Wait, extra explicit cache loaded, longer forward sample | 396 | 533.8 ms | 2 | 262.2 and 514.8 ms |
| Bounded 8 ms, extra explicit cache loaded, three movement samples | 1,187 | 39.3 ms | 0 | Three builds at 218.3-233.6 ms |
| Bounded 8 ms, default driver-cache policy, three final movement samples | 828 | 45.5 ms | 0 | Six builds at 116.3-238.4 ms |

The last final movement sample had a 29.1 ms worst frame while the six slow
driver builds occurred. Logs confirm bounded waits and skipped lookups for
unfinished pipelines. The slow keys from the Wait sample were present in the
136,827-record prepared-info index; this was not another complete extraction
or preparation run. The final boot reused preparation at 6.813 seconds.

These are native guest-present intervals, excluding generated/display frames.
They are different new pipeline keys and slightly different paths, with existing
driver-cache history preserved. They are **not** an identical cold-cache A/B
benchmark and do not establish a percentage speedup. No driver cache was cleared.

## Applied setting and limits

Only Ocarina's per-game `native_async_draws` override is set to `Bounded` (1).
The wait budget remains the existing 8 ms default. Global mode remains `Wait`;
asynchronous shader building stays enabled. Extra cache/trace flags were removed
from the startup switches. The final running session uses the default cache
policy; its already-open diagnostic trace lasts until that session exits.

Bounded mode waits briefly and can then drop a draw whose pipeline is unfinished.
Effects may pop in. Native draws can also write persistent resources, so ending
screenshots do **not** establish complete visual correctness. Essential draws
that the policy preserves can still wait. Cutscenes and a full traversal remain
untested. This is a measured workaround, not complete shader-hitch elimination.

One later trial (`bounded-forward4`) ended on a game-over screen after an enemy
attacked during an unpaused interval between checks. That sample is excluded
completely. The latest autosave was reloaded. Subsequent final movement images
show four hearts, and the game was paused in its Items menu between checks.
Original and latest save snapshots are preserved; no old snapshot was restored
over newer progress. Raw game/save/shader data and private logs stay local.

## Stronger pipeline-path candidate

A fresh `vulkaninfo` query of this actual GPU/driver reports
`graphicsPipelineLibrary`, `graphicsPipelineLibraryFastLinking`, and independent
interpolation decoration support as true. The current installed renderer has no
pipeline-library build path. This makes ahead-of-time library compilation and
fast linking a relevant next experiment, rather than a VRAM-capacity change.

[Khronos describes](https://docs.vulkan.org/features/latest/features/proposals/VK_EXT_graphics_pipeline_library.html)
compiling reusable shader/state libraries early, linking new combinations
without link-time optimization, and preparing optimized replacements separately.
Driver support alone does not prove correct or hitch-free integration.

Current [Lichii PR #579](https://github.com/zortos293/Lichii/pull/579), inspected at
`cf996d0a5e79848653efc709005223318bcd2e37`, contains a library implementation,
independent descriptor layouts, optional background optimized linking, and
fallbacks. It changes 39 files, including shader/binding and renderer paths.
Its author reports remaining first-visit stalls after warm-up. It has **not**
been imported or installed in this follow-up; old compatible cache stamps must
not simply be reused across a different shader/binding layout.

[Eglantine PR #794](https://github.com/Eglantine-Community/Eglantine/pull/794),
head `c6d2acd37a1cd0f999c833482e4bef5e885dbaab` against `nx2-experimental`, still
explicitly preserves the native-channel wait before its async skip paths. Its
description acknowledges first-use Ocarina shader hitches. It is not a verified
drop-in cure for this measurement.
