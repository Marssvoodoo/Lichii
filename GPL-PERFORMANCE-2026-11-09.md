# Ocarina pipeline-library validation - 2026-11-09

The installed Windows candidate now uses graphics pipeline libraries, batch shader
preparation, and **Wait** mode. It preserves complete native draws instead of using
the earlier Bounded-mode workaround. Resolution, effects, game speed and the
owner's disabled DLSS selection were retained. Short forward-walking checks are
substantially less severe than the earlier half-second driver-build examples,
but rare 90-100 ms frames remain and cutscenes have not been validated.

Tested integration: [`f0589f1e`](https://github.com/Marssvoodoo/Lichii/tree/fix/oot-gpl-20261108).
Hardware: Windows, RTX 5090, NVIDIA 621.14, Ryzen 9 9950X3D.
The source incorporates Lichii `cbb512ee` and the implementation behind
[upstream PR 579](https://github.com/zortos293/Lichii/pull/579), with local
integration and correctness guards. Thirty distinct scoped source-review passes
were completed by one reviewer before configuration or compilation began.
Affected passes were rechecked after final changes; this is not a claim of thirty
independent reviewers or exhaustive correctness.

## Observed checks

All five native test suites and sixteen debug-adapter tests passed. A later
focused run passed six GPL cases / 27 assertions. The standalone precompile tool
reports cache version 54. The actual installed executable and 28 companion files
were hash-checked; both previous candidate aliases point to that installation.
Original prepared data and saves were preserved, with rollback copies retained.

| Test condition | Present intervals | Worst frame | Over 50 ms | Over 100 ms |
|---|---:|---:|---:|---:|
| GPL, existing guest cache, before new batch warm-up, five walks | 1,321 | 97.1932 ms | 1 | 0 |
| GPL, static batch reused, guest cache on, three walks | 707 | 100.0345 ms | 1 | 1 |
| GPL, warmed driver, guest cache disabled, three walks | 706 | 76.7494 ms | 2 | 0 |
| Actual primary installation, three walks | 711 | 90.7442 ms | 1 | 0 |
| Clean diagnostic-free test session, Normal priority, three walks | 709 | 38.8856 ms | 0 | 0 |

All these short samples have complete bounded native-present history and zero
frames over 250 ms. World and inventory images were inspected locally after
each accepted segment; all ended with four hearts. The images are private.
The routes, cache history and conditions differ, so the table does **not** prove
a causal FPS percentage improvement or superiority of Normal priority.

The guest-cache-disabled control caused 46 logged new worker pipeline builds;
the largest worker build was 10.0 ms and the largest observed GPU-thread creation
was 22.5 ms. Fast links were about 0.010 ms on average, at most 0.028 ms in that
control. Optional background optimized links still took up to 345.5 ms. These
tests used a warmed NVIDIA driver cache; this was not a cold-driver comparison.

## What batch preparation covers

The extraction produced 59,996 graphics stage entries. The driver warm-up built
59,324 in 177.6 seconds with zero reported driver failures; 672 entries were not
translatable offline. Compute preparation built 4,359 entries in that pass with
zero reported driver failures. The extractor also reported member errors, so
these counts are not proof that every shader in every archive was found.

The next launch reused both done markers instead of repeating the batch. First
presents were observed about ten seconds after polling began. Runtime state
combinations can still require new pipelines. The `total_libraries` log counter
also counts no-retain warm builds; it is not the count of live retained handles.

## Further performance research and tests

Firecrawl was used to check primary upstream and vendor sources before deciding
on further changes:

- Fast draw, texture memo corrections, and GPU thread pinning are already in the
  tested integration. Current Lichii `nx2` remained at `cbb512ee`; Eglantine's
  checked `nx2` remained at `78743f30`. Their open PRs were inspected separately.
- The [Khronos GPL sample](https://docs.vulkan.org/samples/latest/samples/extensions/graphics_pipeline_library/README.html)
  describes partial compilation and background executable linking. This supports
  the implemented approach; it does not promise every first-use stall disappears.
- [NVIDIA's driver page](https://developer.nvidia.com/vulkan-driver) listed an
  older-numbered Windows developer beta than the installed general driver. No
  evidence here justified changing drivers or the global shader cache.
- [Lichii PR 1002](https://github.com/zortos293/Lichii/pull/1002) proposes Windows
  timing and background-worker placement changes based on a six-core laptop.
  That is not evidence of a gain on this sixteen-core host; it was not imported.
- [PR 970](https://github.com/zortos293/Lichii/pull/970) removes synchronization
  from texture-descriptor reads and explicitly acknowledges a pending-write
  risk. It was not imported under the complete-rendering requirement.

Renderer profiling saw substantial CPU-side coordination waits. In one walking
window, guest storage reads peaked at only 299.4 microseconds, with none over a
millisecond, despite an 86.7 ms frame. This does not establish a universal cause
for the remaining hitches. Profiling overhead is excluded from accepted normal
performance samples.

Process Lasso's local log confirmed automatic Below Normal priority adjustments.
A same-position, warmed-cache A/B/B/A check measured 59.79/60.03 FPS at Normal and
59.84/59.78 FPS at Below Normal. All four windows stayed below 30 ms worst-frame
time. No priority-related speedup was established under the observed low CPU
contention. A persistent exception was not applied: Windows denied writing the
administrator-owned configuration, whose original hash was verified unchanged.
The vendor [documents the priority adjustment and application exclusions](https://bitsum.com/process-lasso-faq/#does-pro-balance-slow-processes-down).

The optional internal stack sampler also froze its isolated test session while
a guest CPU thread was suspended. The one diagnostic suspension was identified
and resumed, and the session exited normally later. No guest-CPU sampling is
left enabled in the installed profile. This diagnostic issue is separate from
the normal shader-building stalls and remains a follow-up.

The tested GPL/Wait profile remains the recommendation. There is no verified
additional large FPS multiplier without changing rendering or game timing.
No new native optimization was installed on the strength of search snippets.

## Upstream contribution

The tested combined integration is published in the owner's fork. A focused
[draft guard PR](https://github.com/PantACRO4life/Lichii/pull/3) targets the author
branch behind PR 579: the five-set feature gate, allocation-safe library entry
publication, malformed warm-record bounds, and synthetic regressions. It avoids
resubmitting the existing fragment rescaling fix or unrelated integration work.
Those implementations were tested in the integration above; the extracted clean
branch has not had a separate native build and the PR says so. It is not merged.

Cutscenes, full world traversal, cold-driver behavior, and all transient or
persistent rendering cases remain outside this validation. Complete stutter
elimination and a measured overall FPS gain are not claimed.
