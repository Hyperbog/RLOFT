# Level 22 user field audit — original CODE.2 RES2 bank routing (2026-10-10)

**Status:** original-file verified gameplay correction committed as a source-only checkpoint after Full Windows build #651. **No build dispatched** by this checkpoint. Scope: original party-panel graphics resource identity and source-only forensic trace completeness. The original files are immutable and no replacement art is used.

## Build #651 user upload and visual evidence

The user supplied `1791609452644-8660.zip`, two native inventory screenshots, and `latest-session.txt`. The ZIP manifest identifies embedded native revision `f9a13a7ac8c013f0752d300dcd78fbf8bb3d5419`, format `rloft-level22-diagnostic-v1`, and original media `installed GOG game.gog CD image`. Files were examined locally; no raw diagnostic/character/save data are committed to Git.

| Field observation | Verified evidence |
| --- | --- |
| ZIP entries and extracted bytes | **545** entries, **26,018,619** bytes |
| JSONL events | **313** (including `session.finish`; no `error` events) |
| Native original-resolution automatic indexed frames | **96** (cap reached) |
| Native F12 diagnostic marker frames | **3** |
| Renderer reference TSV captures | **48** |
| Screen distribution | 56 party panels, 31 viewport, 5 automap, 4 top-edge menu |
| Encumbrance UI | `party.encumbrance.open` appears **5** times; red original RES2 panel and original source-derived weight/rank text visibly render; previous #649 fatal missing-bank error no longer occurs |
| Remaining UI defect | Portrait/equipment/background from multiple actor columns overlap with unrelated movement/HUD artwork in inactive slots |
| Draw-call audit limitation | Some modal frames exceed the old `1024` per-presentation cap, omitting **224–538** later calls in representative sessions |

## Original-code provenance and exact root cause

Immutable original supplied `CODE.2`: **558,689 bytes**, SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`. Object 1 begins at physical file `0x3AF24`. The six CODE.2 instruction windows below were checked **byte-for-byte** against that original file, not inferred from the user's images.

The **original archive-slot contract** is established by canonical `Code2/Code2.cpp` `function_2e7f0`: it loops over numbered slots, constructs filenames from `RES` plus the numeric `iu`, opens the corresponding numbered archive, stores its per-slot pointer table in `F0B8 + slot*0x10` and entry count at `+8`. `function_2f29c(item,slot)` calls `2ECB4` using the same direct slot and obtains the entry at `F0B8[slot] + item*9`. Hence **slot 2 is original file `RES2`** and the graphic is the unmodified absolute **RES2 entry index**. `INIT[0] = 15` is a **separate original RES0 graphics namespace base** and must not be applied to archive slot 2.

Independent immutable resource inventory `Resources/RES/INVENTORY.md` identifies **RES2: 63,736 bytes, exactly 38 EH type-1 entries (0..0x25), no gaps**. These cover all source party-panel/chrome IDs. The previous native `draw_res0_local` and `draw_res0_complete` incorrectly requested `RES0[INIT[0]+local_id]`, even though the canonical CODE.2 always passed slot 2. This was an **implementation oversight**, not a missing game file.

| Original CODE.2 routine | Source parameters | Binary instruction window verified at logical/physical | Correct native resource |
| --- | --- | --- | --- |
| `0FF3C` | `2E510(y=0,x=lane,id=0,slot=2)` | `0x0FF48 / 0x4AE6C` | `RES2:0` active actor backdrop |
| `11268` | `2E510(y=0,x=lane,id=2,slot=2)` | `0x11286 / 0x4C1AA` | `RES2:2` inactive actor backdrop |
| `1118C` | `2975C(frame=0,y=0x45,x=lane+0x38,id=1,slot=2)` | `0x1118E / 0x4C0B2` | `RES2:1` one-frame inventory chrome |
| `1118C` | `2E510(y=0x128,x=lane,id=4,slot=2)` | `0x111A7 / 0x4C0CB` | `RES2:4` whole-frame inventory grid |
| `10BBC` | `2E510(y=0x72,x=lane,id=sex + side*2 + 5,slot=2)` | `0x10BEB / 0x4BB0F` | `RES2:5..8` equipment/body silhouettes |
| `10158` | `2975C(frame=0,y=0x45,x=lane+0x38,id=0x24,slot=2)` | `0x1015F / 0x4B083` | `RES2:0x24` one-frame character chrome |

The full callsite contract also covers original `0FF3C` one-frame status overlays **RES2:0x12/0x13** at row `0x2A`, and preserves the distinction between original `2975C` single-frame and `2E510→2E4A4` all-frame operations.

**Why the visual symptoms were misleading:** previously rendering more scanlines via `2E510` was a correct *drawing-mode* repair, but it was still blitting the **wrong file's image data** (RES0). That brought viewport/HUD/movement graphics into the party-modal surface. The direct bank mapping correction is a prerequisite to any further clipping/position/palette judgment.

## Corrections, tests, and separate acceptance

- `PartyModalPresenter::draw_res2_local` and `draw_res2_complete` now call `assets_.type1(2,resource)`, with *no* RES0 INIT offset. Every original party-panel background, equipment silhouette, status badge and character-sheet overlay uses these corrected source-derived wrappers. The native font still reads the original RES0 type-2 font, as originally intended; portrait/equipment objects continue their separately documented RES1 paths.
- Existing `original_party_modal_blit_tests` retains whole-frame versus selected-frame fidelity.
- `original_res2_asset_bank_tests` now exercises **twelve original CODE.2 actor-modal IDs** from synthetic EH type-1 fixtures, proves direct index routing and cached RES2 behavior, and retains rejection of unused/unsupported IDs. No copyrighted RES2 content is embedded.
- `test_original_modal_graphics_contract.py` now checks the `2E7F0→2F29C` slot-to-numbered-archive mapping, RES2 original inventory, six additional exact original-binary instruction signatures, and canonical-versus-native callers/wrappers. Its `--original-code2` option validates the immutable original file hash and **10** targeted original instruction windows. All **six new** windows were separately checked against the supplied original file on 2026-10-10 and matched.
- **Developer tooling only:** `DevSessionTrace::kMaxDrawsPerPresent` raised from **1,024** to **4,096** audit entries, still bounded and opt-in; graphics and game-state mutations are unchanged. This covers every actual per-presentation draw in the first #651 field ZIP. F12 marker cap and ordinary frame cap remain unchanged.

**Current build gate:** Full Windows #651 last accepted **182/182 CTests**, with source revision `f9a13a7ac8c013f0752d300dcd78fbf8bb3d5419`. This source-only patch is **unbuilt**. Its CTest improvements are **authored**, not passed until next user-authorized Full Windows build. Original DOS pixel, item placement, character panel timing, animated container lifecycle, map and complete Level22 gameplay still require separate direct comparison. Do not increment provisional engineering readiness (49.8%) or original-DOS gameplay acceptance (0%) for this untested patch.

**Priority:** fully playable original-faithful Level22 through all original 22 mist frames, before independent Level23 gameplay and before any modernization.
