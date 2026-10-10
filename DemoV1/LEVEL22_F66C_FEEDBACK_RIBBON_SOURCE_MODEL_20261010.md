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

## Integration blocker / next step

`PartyModalPresenter::draw_held_cursor_and_description` currently uses a provisional filled strip even for `6DAC` pending feedback; it is **not** source-faithful to `F66C`. Do not replace it by randomly chosen tiles or a UI-local PRNG. The next correct integration must route the existing shared original `30CA2` RNG state through the caller without multiplying draws across frame presentations, resolve `30288` font measurement behavior and exact `F790` render cadence, load untouched `RES2:3` through the already corrected bank route, then perform original-DOS pixel/event comparison. Keep this model source-only until that entire dependency path is verified.

**Build status:** new C++ regression is registered as one additional CTest (**183 registered tests** when Python is available), but the current source changes are **unbuilt on Windows**, and only the standalone local header test was compiled/executed. Do not dispatch CI without separate user authorization. No independent Level23 gameplay or modernization.
