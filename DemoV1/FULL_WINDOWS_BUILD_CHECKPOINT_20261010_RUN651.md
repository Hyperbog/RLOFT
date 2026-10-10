# Demo v1 — Full Windows build #651 (2026-10-10)

**Outcome: SUCCESS.** [GitHub Actions run #38026056424](https://github.com/Hyperbog/RLOFT/actions/runs/38026056424) completed successfully with every registered native regression passing.

| Field | Verified evidence |
| --- | --- |
| GitHub Actions run number / ID | **#651 / `38026056424`** |
| Run event | User-authorized temporary marker-only push, Full Windows mode |
| Build checkout/embedded source revision | `f9a13a7ac8c013f0752d300dcd78fbf8bb3d5419` |
| Verified gameplay/test source revision (before temporary CI metadata) | `338f4f180d5417853e3d689ca39ee8ceeb4e8e0f` |
| Last source-verified original CODE.2 modal batch | `f615302ec350618e74b3d4a606182559a1db2e4d` |
| Verified Windows C++ compilation | PASS |
| CODE.2 canonical C++17 syntax gate | PASS |
| Full Windows CTest suite | **182/182 PASS, 100%** |
| `original_res2_asset_bank_tests` | PASS |
| `original_party_modal_blit_tests` | PASS |
| `original_modal_graphics_contract_tests` | PASS |
| `dev_session_trace_tests` | PASS |
| Windows ZIP artifact | `Ravenloft-Demo-v1-Windows-x64` |
| Artifact ID | `11660735578` |
| Artifact compressed size | **812,204 bytes** |
| Executable SHA-256 | `f622113e0813cd16826b8a2e92fc40ae31f7ea6358780191d822459738c09786` |
| Binary publishing commit | `b4e09b1b42e809191aa97006b071920ef0e0569d` |
| Completed | 2026-10-10 05:08:28 UTC |

## Why #650 was not the completed build

Earlier, [Full Windows #650](https://github.com/Hyperbog/RLOFT/actions/runs/38025690116) failed during C++ test compilation because of an erroneously escaped JSON string assertion in `DemoV1/tests/dev_session_trace_tests.cpp` (MSVC C2017/C3680 at line 134). This was a **test-source syntax error**, not a failed test of original game behavior. The corrected assertion was committed at `338f4f180d5417853e3d689ca39ee8ceeb4e8e0f` before the explicitly authorized #651 retry. No underlying original gameplay semantics were modified for the retry. Full #651 then passed the entire CTest suite.

## What's in the built executable

The successful artifact includes:
- the original CODE.2 `2E510` complete-resource party/inventory backdrop correction;
- lazy original `RES2` archive loading for the Level22 encumbrance bitmap and container panels;
- complete original `RES2` blits and source-derived actor-major container/outline drawing order;
- original `12988` encumbrance burden-label placement and width;
- `Diagnose-Level22.cmd` and the F12-indexed framebuffer, palette, draw-operation and event recorder;
- other original Level22 systems already present in the prior development baseline.

## Manual acceptance instructions

Extract the [successful Windows artifact](https://github.com/Hyperbog/RLOFT/actions/runs/38026056424) into a folder. Run **`Diagnose-Level22.cmd`**. Check original Level22 movement, party portraits, inventory backgrounds/grid, all available actor slots, the previously crashing first-actor encumbrance control (original VGA `x=68..77,y=16..39`), container opening, automap, top menus and quit. Press **F12** at every graphical mismatch. On exit the launcher creates a ZIP under `%TEMP%\\RLOFT-Demo-Diagnostics` (manual compression of the latest session folder if it fails). Upload the ZIP and screenshots if needed. Original game files remain separate and unchanged; the EXE uses the previously remembered original installation path.

**Verification limits:** Windows native compilation and CTests are verified at 182/182. The current new EXE has **not** been manually tested for Level22 graphics, the fixed encumbrance interaction, complete mist ending, timing, or pixel/state equality against DOS. Original-DOS acceptance remains zero until those comparisons are performed. Level23 independent gameplay still deferred. Engineering readiness scoring unchanged.

**CI/build discipline:** Following queuing #651, the temporary workflow edit was reverted byte-for-byte to blob `7c8e5720f38c0690d01457ea3daf556124725ef2`; the one-time marker was removed. The workflow is **manual-only**. No additional build is authorized by a routine documentation push.
