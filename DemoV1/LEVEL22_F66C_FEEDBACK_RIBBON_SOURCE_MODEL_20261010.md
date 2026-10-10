# Level22 feedback ribbon: original CODE.2 F66C source model (2026-10-10)

**Checkpoint:** verified original-binary layout and RNG contract, independent native C++17 model and unit test. This is NOT yet wired to the live party-modal presenter; neither graphics acceptance nor gameplay-completion/readiness percentage changes. Windows Full #651 remains last tested at 182/182, before the newer unbuilt RES2 bank correction and this batch. No Windows build was triggered.

## Source identity and binary proof

Original user-supplied immutable `CODE.2`: 558,689 bytes, SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`; object-1 physical base `0x3AF24`. The local verifier `tools/verify_original_feedback_ribbon_bytes.py` passed **11 independently captured exact instruction windows**, covering original logical `F673..F75B`, `F7B4..F7CB`, and `30CA2..30CC3` (physical `04A597..04A67F`, `04A6D8..04A6EF`, `06BBC6..06BBE7`). Disassembly was regenerated from the immutable original; instruction boundaries and call targets were checked, not inferred from documentation. The adjacent original resource inventory identifies original RES2 as a 38-entry type-1 EH archive.

Original `F790` at `F7B4` checks original global `6DAC` and branches to `F66C(0x759C)` while pending, before the ordinary held-object description. `F66C` calls original text-width service `30288(text,font=1)` and emits repeated original `2975C(frame, row=0x62, column, graphic=3, bank=2)` calls, followed by `30508(row=0x65, column=0, span=320, text,font=1)`.

## Exact original tiling (source offsets)

* `F681..F6B3`: signed `(text_width+16)/2`, round up by the encoded `AND 0x0000FFFC`, center relative to 160; if tile origin is divisible by 8, shift left 4.
* `F6B4..F6C9`: if `(origin+4)` divides evenly by 80, shift left a further 8 (the original four actor-lane seam special case).
* `F6CA..F6E3`: terminal tile positions use `0x138-origin` and exclusive loop bound `0x140-origin`; advance columns by 8.
* `F6E4..F6F2`: first tile always uses `RES2:3` frame **0**.
* `F6F3..F714`: terminal tile uses frame **5**, otherwise the original actor-lane seam `(column+4)%80==0` uses frame **1**.
* `F715..F735`: all other tiles alone call **shared original RNG** `30CA2`, selecting frame `2 + ((3 * random15) >> 15)` = **2..4**. No random draw occurs for frames 0, 1 or 5. The 32-bit original RNG service at `30CA2..30CC3` advances the original state using multiplier `0x41C64E6D` and increment `0x3039`, returning `((state >> 16) & 0x7FFF)` when the 30C9C state pointer exists; if absent, 30CA2 returns zero without an update.
* `F736..F75B`: `2975C` selected-frame draw per tile; final original font-one centered string at row `0x65` across 320 pixels.

The pure header `include/rloft/original_feedback_ribbon_layout.hpp` contains no global or private RNG, no synthetic art and no gameplay side effects. The caller must supply the original shared RNG callback when live-wired. `tests/original_feedback_ribbon_layout_tests.cpp` covers exact short, multi-seam, clipped-wide coordinates, tiled frame ordering, random draws only at source-defined branches, and RNG threshold outcomes. Local C++17 compilation and test execution passed; `tools/verify_original_feedback_ribbon_bytes.py /mnt/data/CODE.2` passed 11/11 original instruction windows.

## Source-verified 30288/30508 font-width and 112EC render ownership (Part 10)

An additional six original CODE.2 instruction windows were matched byte-for-byte against the same SHA-256-identified shipped original, bringing the verifier to **17 exact instruction windows**. The new logical windows are 112EC caller 11316 (physical 4C23A), width service 30288 and 302A6 (physical 6B1AC and 6B1CA), centered text service 30547 (physical 6B46B), and shared RNG accessor/seed 30C9C/30CC4 (physical 6BBC0/6BBE8). The F790 source call at 1134C (within the verified 11316 window) occurs after the original 112EC four-column draw loop; original 112EC draws a single inventory-feedback pass when it is invoked. Its invocation cadence is NOT the native host's unconditionally repeated 70-Hz presentation loop.

CODE.2 30288 resolves font index 1 via 2F29C(font,archive=0), then sums unsigned metric bytes at font-record +4 indexed by each **sign-extended character byte** (302B1..302BC). For valid glyphs 0..127 this equals native `Type2Font.widths`. Input bytes >=128 index *before* that table in the original; their actual behavior is unproved and the new isolated model explicitly refuses to fabricate CP437 replacement semantics. CODE.2 30508 separately measures the string and calculates `base + ((span - measured_width) SAR 1)`. A negative odd difference rounds toward negative infinity, unlike native C++ division toward zero. The ribbon's final arguments are `row=0x65,base=0,span=0x140,font=1`.

The new `OriginalFeedbackRibbonText` model combines exact metrics, signed centered-column arithmetic and the already verified `F66C` tile planner without accessing or altering Win32 game state. The C++17 `original_feedback_ribbon_text_tests` passed locally: variable-width glyphs, text centering with negative odd differences, exact 10- and 160-pixel tile layouts, shared RNG consumption at only interior tiles, and rejection of unproven signed-negative glyph lookups.

The additional original `30C9C` instruction bytes **always return the fixed DWORD state address 0xF6F4**. Therefore `30CA2`'s null-pointer branch exists but is unreachable through this shipped accessor. The existing native `OriginalRandom` is already owned by `ExplorationRuntime`; the separate `PartyModalPresenter` is constructed without an RNG reference. **Do not connect a copied/independent PRNG to the presenter.** In particular the original-code-verified F790 after party-column redraw and the Win32 host's per-VGA-frame redraw are not yet proven one-to-one. Rendering random tiles from the presenter on every host frame would silently consume extra gameplay RNG values.

**Gate:** 184 CTests registered (Python present); last completed Full Windows #651 was 182/182 on earlier source. This text model was compiled and executed locally in strict C++17, original binary verifier passed 17/17, and neither of the two newest source-model/RES2 batches have been compiled as a full Windows binary. Gameplay code remains unchanged, no new percentage or DOS acceptance, no Level23 work.


## Integration blocker / next step

`PartyModalPresenter::draw_held_cursor_and_description` currently uses a provisional filled strip even for `6DAC` pending feedback; it is **not** source-faithful to `F66C`. Do not replace it by randomly chosen tiles or a UI-local PRNG. The next correct integration must route the existing shared original `30CA2` RNG state through the caller without multiplying draws across frame presentations, resolve `30288` font measurement behavior and exact `F790` render cadence, load untouched `RES2:3` through the already corrected bank route, then perform original-DOS pixel/event comparison. Keep this model source-only until that entire dependency path is verified.

**Build status:** new C++ regression is registered as one additional CTest (**183 registered tests** when Python is available), but the current source changes are **unbuilt on Windows**, and only the standalone local header test was compiled/executed. Do not dispatch CI without separate user authorization. No independent Level23 gameplay or modernization.


## Follow-on F790 ordinary-held-item RNG correction (2026-10-10)

Important clarification: F790's pending 6DAC path **does not reseed** the original shared RNG; however the **ordinary nonempty held-item path also invokes F66C**, bracketed by original 30CC4(held ID) and 30CC4(current 30700 IRQ tick), with the tick read after drawing. Only an empty ordinary held-item sentinel -1 skips F66C. This distinction was established by three additional exact instruction windows, incorporated into the original F790 cadence verifier (9/9 total). The isolated epoch adapter was corrected before live wiring; a native indexed RES2:3 cached-ribbon renderer is now implemented but not connected to the Windows frame loop. See [full source correction and remaining fidelity gates](LEVEL22_F790_SHARED_RNG_AND_REPLAY_20261010.md). Last tested Full Windows remains #651; 187 registered CTests are pending a user-authorized new build.
