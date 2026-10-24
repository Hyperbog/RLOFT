# Part 33 — original 274A4 compass accepted; 1F120/1F8FC presentation pending

## Accepted Git revisions and evidence

- Source compass integration: [9d20f2e984fd](https://github.com/Hyperbog/RLOFT/commit/9d20f2e984fd020915aebbf410fdac2f1c023293).
- Original Application52 callback ABI correction (296C0 returns u32): [009b25b45eba](https://github.com/Hyperbog/RLOFT/commit/009b25b45ebaef0b7ad4fc472d5db49ef67ce282).
- Windows original-media acceptance: [run 40055873423](https://github.com/Hyperbog/RLOFT/actions/runs/40055873423), success.
- Linux portable C++ regression: [run 40055873366](https://github.com/Hyperbog/RLOFT/actions/runs/40055873366), success.
- Canonical CODE.1/CODE.2 source link: [run 40055873415](https://github.com/Hyperbog/RLOFT/actions/runs/40055873415), success.

## Exact source execution frontier

Original retained Level 22 source now follows:

`1F8FC → 1F1C8 → 34DE8 → 33E6C → 34110 → 1BDAC → 2C294 → 335DC → 36CEC → 274A4 → 273F0 → 1F120 (next pending callback)`

The compiled original `274A4` computes its compass location from the guest camera heading `C658` and draw page `EF4D`, invokes `296C0` clipping, resolves the original RES0 compass image through `2F29C`, draws it twice via original `29804 → 29414 → 2A380/2A6B0`, runs `273F0` using genuine original VGA port/memory callbacks, and restores the original clip. The retained `1F1C8` call then returns and its caller enters `1F120`.

### Verified original-media counters

Source `CODE2_ORIGINAL_LIVE_BOOT=1F8FC_COMPOSITOR_CHAIN` in run 40055873423:

| Metric | Observed |
| --- | ---: |
| Polygon quads / original polygon scanline draws | 8 / 212 |
| Original actor update returns | 1 |
| Original RES3 scene sprite draws | 175 |
| RES3 indexed sprite pixel writes | 41,231 |
| Original 2C294 scene returns | 1 |
| Original compass calls / returns | 1 / 1 |
| Original compass clipping calls | 2 |
| Compass RES0 image draws | 2 |
| Direct VGA compass pixel writes | 280 |
| Original 1A5B0 presentations | 0 |
| Completed original gameplay frames | 0 |

Expected diagnostic marker: `next=1F120_FRAME_UPDATE`. This is **not** an original full gameplay frame.

## Pending frame-presentation integration

1. Connect canonical `1F120` callbacks: `2B1BC` actor census, `29048` source audio mode, `28AE0`/ `288FC` sound selection, `28F58` volume, `29008` tempo, and `30CA2` source RNG. Require an explicit fail-closed path when a real audio driver is active but its callback graph is not connected.
2. Audit the original `1F8FC` call to `325B0` against retained `C034` palette mode and existing mode-4-only validation. Do not override the source mode or fabricate RGB6 data. Use original COLORS media.
3. Continue source `29234` DAC streaming, `1A5B0` page presentation/cursor restoration, and `1F8FC` return with page flip. Demand completed source frame and nonzero actual presentation counters in Windows CI.
4. Measure repeated input-driven source frames and retail DOS pixel parity separately.

## Explicit stopped edit

The uncommitted frame-update implementation was rejected by local exact-text replace checks before any GitHub write. First check failed because the existing function returns `u32`, not `void`. The second fixed that match but failed at the `1F120_FRAME_UPDATE` display string indentation. No partial code mutation was saved. Resume with function-bounded edits or exact content inspection rather than broad whitespace-sensitive replacements.

**Do not claim completed frames or pixel parity until source callbacks and real Windows original-media run confirm them.**
