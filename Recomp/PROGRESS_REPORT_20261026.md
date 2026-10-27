# RLOFT Recomp — evidence-based progress report (2026-10-26 UTC)

**Active branch:** `Recomp`. **Historical baseline:** `ed001028d1c7128480eb9ccd1cb8eafa86458f3d` (earlier acceptance checkpoint); **current source-wide verification:** Windows canonical [#40401277234](https://github.com/Hyperbog/RLOFT/actions/runs/40401277234), Linux [#40401277474](https://github.com/Hyperbog/RLOFT/actions/runs/40401277474), 32+17 original media [#40400676966](https://github.com/Hyperbog/RLOFT/actions/runs/40400676966), and independent native saved-portal [#40400460281](https://github.com/Hyperbog/RLOFT/actions/runs/40400460281). Original game files remain untouched on `DnD-Classics`. This report separates acceptance coverage, native source diagnostics, test counts and independent DOS equivalence. It does **not** estimate the fraction of a commercially playable game implemented.

## Progress numbers — independently bounded

The whole-game acceptance ledger stays **3/12 = 25%**. The [shared actor/script/dialogue checkpoint](ORIGINAL_SHARED_ACTOR_SCRIPT_DIALOGUE_20261026.md) confirms the original source type-15 saving throw and death cleanup, 100 passing source CTests, original indexed dialogue text decoder, and a shipped-world script corpus of **394 active descriptors, 555 commands**. Eligible-target near-combat, modal 1C77C, interactive input and Miles AIL remain open; the archive census is not a completion metric.

**Implementation strategy:** [Engine-wide original CODE.1/CODE.2 execution plan](ENGINE_WIDE_RECOMPILATION_PLAN_20261026.md), with the [shared call-graph and subsystem closure report](SHARED_ENGINE_CALLBACK_CLOSURE_20261026.md). Only shared subsystem functionality can be promoted toward whole-game acceptance. Current named levels are test inputs, **not** a denominator for game completion or a development workstream. Static source callback inventory **is now quantified** (985 reconstructed function bodies, 671 direct edges, 1,933 indirect ABI mentions, 457 live callback assignments). **Executed/reachable game callback completion percentage is still not measured**; a lexical code inventory is not execution coverage.

| Metric | Current verified numerator / denominator | Percentage or scope | Exact meaning |
| --- | ---: | ---: | --- |
| Coarse whole-game acceptance | **3 / 12** (R01–R03) | **25.00% accepted**, 9 still open | Authoritative `ACCEPTANCE_GATES.json` / `tools/recomp_progress.py`. These are equal-weight acceptance gates, not comparable work hours or instruction coverage. |
| Current shared source CTest suite | **100 / 100** | **100% of registered CTests passed** | [Windows canonical source-link #40428870100](https://github.com/Hyperbog/RLOFT/actions/runs/40428870100) includes original `1BDAC→0D550` type-15 attack/cleanup, `002A8` mixed script and `1C368` indexed dialogue text tests. These are bounded source regressions, not retail DOS parity. |
| Original authenticated type-8 portal directions | **2 / 2** | Both Level 23→26 and 26→23 verified in the scoped native saved-slot diagnostic | [Strict native #40321360454](https://github.com/Hyperbog/RLOFT/actions/runs/40321360454); compiled original `2D3D8` and original unmodified `LEVELS.DAT` records, invoked by private diagnostic commands (not walking into the tiles). |
| Original level transitions across the saved-slot case | **3** | 07938 restore to 23, then 23→26, then 26→23 | Same [strict run](https://github.com/Hyperbog/RLOFT/actions/runs/40321360454), `transitions=3`, final live original guest Level 23. |
| Audited post-transition source-frame scanouts | **3 / 3** | All demanded post-level frames authenticated in this **one bounded diagnostic sequence** | The original `1FC14` scene + `1A510` page + HUD/audio source callback audit returned for saved Level 23, portal Level 26 and reciprocal Level 23; `audited_frames=3` and 0 exit. See `ORIGINAL_RETAINED_07938_LEVEL23_20261026.md` for exact frame/palette hashes. |
| Retained private host-input ticks in the saved-slot roundtrip | **6 / 6** | All expected `0x30B40` polls in the strict case | Host diagnostic command sequence includes private `0xFFF*` codes and zero ticks; **not** a physical DOS IRQ9/keyboard equivalence metric. |
| Native framebuffer footprint per post-portal receipt | **320 × 400** (128,000 indices) | **256 DAC entries × 3 RGB6 channels = 768 six-bit values** | Original VGA host indexed scanout/CRTC and source DAC capture, with SHA-256 receipts on three levels. No original DOS image parity accepted. |
| Audited registered C++ source inputs and included source fragments | **50 inputs and 7 `*.inc` fragments** | No detected duplicate targets, orphan inputs or fragment cycles at audit | [Source-audit checkpoint](CODE_AUDIT_20261026.md); layout number was counted in the audit, not re-inventoried from this report. |

The project's **only official overall progress percentage is 25.00% of the 12 coarse gates**. It must not be relabelled as game completion. No reliable weighted percentage exists for compiled original-instruction coverage, full playable-game completion, original DOS behavioral parity, or all resources/levels/scripts; report those as **not measured** rather than making a numerical estimate.

**Latest shared eligible-hit combat checkpoint (2026-10-27 UTC):** source commit [`2798d2f8`](https://github.com/Hyperbog/RLOFT/commit/2798d2f8cba9fef060ecfccb31bccdc7117ecc3b) runs compiled `1BDAC→0D550→0D4E8→0C9A0→0EFA0→0CEE4→0CD44→28110` for genuinely selected, bounded hit paths. The guard follows the original seed/dice sequence and source target selection without synthesizing combat results; it refuses misses requiring original text formatting, equipment-hand callbacks, nested container weight evaluation, unconnected extended statuses, lethal cleanup and game-over. New canonical CTest checks natural d20 hit, ordinary modified hit, original failed saving throws, status bit updates, nonlethal HP/time/source audio queue, and guarded would-be lethal damage. [Canonical Windows source-link #40428870100](https://github.com/Hyperbog/RLOFT/actions/runs/40428870100) passed **100/100 CTests**. [Linux portable #40428870043](https://github.com/Hyperbog/RLOFT/actions/runs/40428870043), [retained native portal #40428870092](https://github.com/Hyperbog/RLOFT/actions/runs/40428870092), [Windows player build #40428870116](https://github.com/Hyperbog/RLOFT/actions/runs/40428870116), [original-media saved loader #40428870042](https://github.com/Hyperbog/RLOFT/actions/runs/40428870042) and [RES4 #40428870048](https://github.com/Hyperbog/RLOFT/actions/runs/40428870048) passed. [Comprehensive Windows milestone #40428870076](https://github.com/Hyperbog/RLOFT/actions/runs/40428870076) must be checked independently. Current static source inventory: **985 bodies, 671 direct edges, 1,933 indirect ABI mentions, 457 live callback wirings, 58 guarded sites**; dynamic execution coverage not measured. Complete ordinary combat, retail-DOS parity and whole-game acceptance remain open.

## Acceptance ledger, unchanged

- **Accepted:** R01 canonical reconstructed CODE.1/CODE.2 compilation and executable source linking; R02 authentic LE/page/relocation image validation; R03 original compiled CODE.2 bounded startup and initializers.
- **Open (nine):** R04 DOS memory/DPMI lifecycle parity; R05 complete DOS file services; R06 authentic CODE.1 character creation/handoff; R07 DOS input/timer/IRQ behavior; R08 complete renderer/palette/audio correctness; R09 broadly playable source gameplay; R10 full shipped world and completion; R11 full save/load and CODE.1/CODE.2 gameplay workflows; R12 independent full original-DOS behavioral comparison.
- The strict saved-Level23→26→23 roundtrip improves **partial evidence inside R08–R11** but does not independently finish any of those **whole-game** scopes. Do not auto-increment gate totals from a single passing integration test.

## Strict portal acceptance (current PASS)

[Strict native Windows run #40321360454](https://github.com/Hyperbog/RLOFT/actions/runs/40321360454) on source `bc7caf26f9f079fa33e39516fba090cb6f43e8a2` exited successfully:

```text
CODE2_07938_RETAINED_LEVEL23_PORTAL_26_23_FULL_CYCLE_PASS
  original_07938=YES original_2D3D8=YES transitions=3
  audited_frames=3 live_guest=UNCHANGED owner_files=READ_ONLY
  independent_DOS_frame_parity=OPEN
RETAINED_07938_ACCEPTANCE_SOURCE restored=True portal_full_cycle=True exited=True code=0
```

| Original scene | Screen start | Indexed 320×400 SHA-256 | RGB6 palette SHA-256 |
| --- | ---: | --- | --- |
| Level 23 after saved restoration | `0x8000` | `5560e3395b79eb19bc7034bc539eca0c4d0f24dcb014fbf8f309b1480f0345d5` | `0af8ea393f1f266596af59c1a2013350b51000b7456d377ab284ebbc43db0812` |
| Level 26 after original portal | `0x0000` | `24c5a5c9d022db9f17e623f394b41ef61a80eaf3b122488dbace8d24ba1feace` | `546331d1ac5e96419f019e7b654bae64a8707c4b14a06b8cafeb2e4ffa71996a` |
| Level 23 after return portal | `0x8000` | `84f349176897e539b283e1657a0d78f11bd52132647629017b755f5be33cdae0` | `0af8ea393f1f266596af59c1a2013350b51000b7456d377ab284ebbc43db0812` |

Original level-portal record coordinates are Level 23 `(9,1)`→Level 26 `(68,29)`, and Level 26 `(68,30)`→Level 23 `(9,2)`. The Level 26 view legitimately generated **zero polygon quads and zero sprites**, but completed **540** original world-cell queries, **16** source vertex projections, one actor return, one wall pass, one scene return and one compass return. The validator now accepts source-attested empty raster output without inventing geometry or disabling callbacks.

## Current CI health: distinguish suites and workflows

| Workflow / evidence | Result | Interpretation |
| --- | --- | --- |
| [Strict 07938 saved-slot/native portal #40321360454](https://github.com/Hyperbog/RLOFT/actions/runs/40321360454) | **PASS** | Mandatory source markers, native exit code 0, three Level23/26/23 scans and original portal directions. |
| [Earlier full Windows milestone #40320888229](https://github.com/Hyperbog/RLOFT/actions/runs/40320888229), implementation `b9c07cea` | **PASS** | Includes original redirected-stdin guest-motion step, not physical DOS IRQ9. |
| [Comprehensive Windows milestone #40400460189](https://github.com/Hyperbog/RLOFT/actions/runs/40400460189), implementation `a4eae4db` | **PASS** | Shared source code built, 95/95 CTests and original stdin source integration completed; not IRQ9/retail DOS input parity. |
| [Historical Windows milestone #40321896354](https://github.com/Hyperbog/RLOFT/actions/runs/40321896354), `ed001028` | **FAIL (historical, intermittent)** | Its source console-input test stopped after 2/3 required `0x30B40` reentries at 45s. Later green run does not independently diagnose cause; keep the lifecycle investigation open. |
| [Canonical source-link #40401277234](https://github.com/Hyperbog/RLOFT/actions/runs/40401277234), common-source implementation | **PASS** | 95/95 canonical CTests; required source dependency census with 985 functions, 671 direct edges, 1,933 callback ABI mentions. |
| [Windows player package #40320888137](https://github.com/Hyperbog/RLOFT/actions/runs/40320888137) / [Linux regression #40320888096](https://github.com/Hyperbog/RLOFT/actions/runs/40320888096), same implementation `b9c07cea` | **PASS / PASS** | Experimental native package and Linux regression; not a retail-equivalent release. |

## Next work, ordered by proof gaps

1. **Complete shared original CODE.2 engine dependencies:** [source callback closure inventory](SHARED_ENGINE_CALLBACK_CLOSURE_20261026.md) identifies original `0D550` actor special/near actions, `002A8` script dispatch, `1C77C` dialogue and AIL active-device handling as general outstanding callbacks. Investigate historical intermittent third-input guest-worker timeout while retaining bounded native tests.
2. **Physical player-controlled portal traversal:** integrate authenticated saved Level 23 startup into the interactive Windows GUI and enter the *real* archived portal tile via original keyboard/mouse movement/collision events, without private source-test commands.
3. **Independent retail DOS oracle:** synchronize original DOSBox(-X)/DOS screenshots and RGB6 palette at the same save/camera/frame/page, then compare exact indexed bytes, event timing, and audio. Native self-hashes are reproducibility receipts, not parity by themselves.
4. **Finish coarse R04–R11 edges:** independently validate original DOS memory/selector lifetimes, complete file services, CODE.1 handoff, IRQ/timer input, AIL sound, more shipped levels/scripts and user-visible saves.
5. **Release-mode stability:** reproduce the historical Windows `0xC0000409` fast-fail; strict original-media source acceptance currently uses Debug.

### Repeatable local progress calculation

```shell
python Recomp/tools/recomp_progress.py --json
```

This intentionally yields `accepted=3`, `total=12`, `percent=25.0`. A future percentage change requires changing the authoritative acceptance manifest **only after a gate's entire scope is actually verified**, not simply because this report documents additional partial progress.
