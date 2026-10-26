# RLOFT Recomp — evidence-based progress report (2026-10-26 UTC)

**Active branch:** `Recomp`. **Verification baseline:** `ed001028d1c7128480eb9ccd1cb8eafa86458f3d` (documentation/ledger HEAD at inspection). Original game files remain untouched on `DnD-Classics`. This report separates acceptance coverage, native source diagnostics, test counts and independent DOS equivalence. It does **not** estimate the fraction of a commercially playable game implemented.

## Progress numbers — independently bounded

**Implementation strategy:** [Engine-wide original CODE.1/CODE.2 execution plan](ENGINE_WIDE_RECOMPILATION_PLAN_20261026.md). Only shared subsystem functionality can be promoted toward whole-game acceptance. Current named levels are test inputs, **not** a denominator for game completion or a development workstream. Engine callback coverage is **not yet quantified**; it requires a complete source-dependency census first.

| Metric | Current verified numerator / denominator | Percentage or scope | Exact meaning |
| --- | ---: | ---: | --- |
| Coarse whole-game acceptance | **3 / 12** (R01–R03) | **25.00% accepted**, 9 still open | Authoritative `ACCEPTANCE_GATES.json` / `tools/recomp_progress.py`. These are equal-weight acceptance gates, not comparable work hours or instruction coverage. |
| Most recent Windows CTest suite | **93 / 93** | **100% of registered CTests passed** | [Comprehensive Windows run #40321896354](https://github.com/Hyperbog/RLOFT/actions/runs/40321896354) at the baseline HEAD, before a later independent stdin-integration-step **failure**. A passing CTest suite does not make that whole workflow green. |
| Original authenticated type-8 portal directions | **2 / 2** | Both Level 23→26 and 26→23 verified in the scoped native saved-slot diagnostic | [Strict native #40321360454](https://github.com/Hyperbog/RLOFT/actions/runs/40321360454); compiled original `2D3D8` and original unmodified `LEVELS.DAT` records, invoked by private diagnostic commands (not walking into the tiles). |
| Original level transitions across the saved-slot case | **3** | 07938 restore to 23, then 23→26, then 26→23 | Same [strict run](https://github.com/Hyperbog/RLOFT/actions/runs/40321360454), `transitions=3`, final live original guest Level 23. |
| Audited post-transition source-frame scanouts | **3 / 3** | All demanded post-level frames authenticated in this **one bounded diagnostic sequence** | The original `1FC14` scene + `1A510` page + HUD/audio source callback audit returned for saved Level 23, portal Level 26 and reciprocal Level 23; `audited_frames=3` and 0 exit. See `ORIGINAL_RETAINED_07938_LEVEL23_20261026.md` for exact frame/palette hashes. |
| Retained private host-input ticks in the saved-slot roundtrip | **6 / 6** | All expected `0x30B40` polls in the strict case | Host diagnostic command sequence includes private `0xFFF*` codes and zero ticks; **not** a physical DOS IRQ9/keyboard equivalence metric. |
| Native framebuffer footprint per post-portal receipt | **320 × 400** (128,000 indices) | **256 DAC entries × 3 RGB6 channels = 768 six-bit values** | Original VGA host indexed scanout/CRTC and source DAC capture, with SHA-256 receipts on three levels. No original DOS image parity accepted. |
| Audited registered C++ source inputs and included source fragments | **50 inputs and 7 `*.inc` fragments** | No detected duplicate targets, orphan inputs or fragment cycles at audit | [Source-audit checkpoint](CODE_AUDIT_20261026.md); layout number was counted in the audit, not re-inventoried from this report. |

The project's **only official overall progress percentage is 25.00% of the 12 coarse gates**. It must not be relabelled as game completion. No reliable weighted percentage exists for compiled original-instruction coverage, full playable-game completion, original DOS behavioral parity, or all resources/levels/scripts; report those as **not measured** rather than making a numerical estimate.

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
| [Latest Windows milestone #40321896354](https://github.com/Hyperbog/RLOFT/actions/runs/40321896354), current HEAD `ed001028` | **FAIL** | **93/93 CTests passed**, but separate source console-input test reached **2 of 3** required `0x30B40` source keyboard reentries before the **45-second watchdog**. Root cause not established. Later edits since `b9c07cea` were CI log selection and docs/ledger, not reconstructed gameplay-source changes. Do not mark this run green or assume the timeout is harmless. |
| [Canonical source-link #40321896330](https://github.com/Hyperbog/RLOFT/actions/runs/40321896330), current HEAD | **PASS** | Source link/inventory and synthetic source tests, not full DOS parity. |
| [Windows player package #40320888137](https://github.com/Hyperbog/RLOFT/actions/runs/40320888137) / [Linux regression #40320888096](https://github.com/Hyperbog/RLOFT/actions/runs/40320888096), same implementation `b9c07cea` | **PASS / PASS** | Experimental native package and Linux regression; not a retail-equivalent release. |

## Next work, ordered by proof gaps

1. **Reproduce and diagnose the latest Windows stdin third-event timeout.** Capture the final source callback, distinguish live guest-worker synchronization from source renderer/texture cost; preserve the 45-second watchdog. Review documented guest-worker lifecycle/ownership rather than silently bypassing the failing integration gate.
2. **Physical player-controlled portal traversal:** integrate authenticated saved Level 23 startup into the interactive Windows GUI and enter the *real* archived portal tile via original keyboard/mouse movement/collision events, without private source-test commands.
3. **Independent retail DOS oracle:** synchronize original DOSBox(-X)/DOS screenshots and RGB6 palette at the same save/camera/frame/page, then compare exact indexed bytes, event timing, and audio. Native self-hashes are reproducibility receipts, not parity by themselves.
4. **Finish coarse R04–R11 edges:** independently validate original DOS memory/selector lifetimes, complete file services, CODE.1 handoff, IRQ/timer input, AIL sound, more shipped levels/scripts and user-visible saves.
5. **Release-mode stability:** reproduce the historical Windows `0xC0000409` fast-fail; strict original-media source acceptance currently uses Debug.

### Repeatable local progress calculation

```shell
python Recomp/tools/recomp_progress.py --json
```

This intentionally yields `accepted=3`, `total=12`, `percent=25.0`. A future percentage change requires changing the authoritative acceptance manifest **only after a gate's entire scope is actually verified**, not simply because this report documents additional partial progress.
