# Alpha A1 checkpoint — source-neutral CODE.1 character media seam

**Date:** October 13, 2026.
**Branch:** `native-engine`; source checkpoint before this note: `3efb04a2eacf0d03d1bc4330192352fe03870b89`.
**Status:** interface and read-only source adapter implemented; **character UI and playable Alpha remain OPEN**.

## Source changes
- Added `rloft::CharacterVisualSource` in `Shared/StartupPresentation`, extending the existing palette, RES0 and T API with original EH1 resource banks and CODE.1 DGROUP byte/word/DWORD/C-string access.
- `NativeStartupVisualSource` now lazily obtains original RES1, RES7, RES8 and CODE.1 bytes through the existing independent `NativeMedia` reader. RES0 remains the existing checked EH1 path; unsupported archives and invalid record/offset bounds throw. CODE.1 must match the original `0x43D1B` size with DGROUP starting at `0x3E724`. The reader snapshots the indexed media description to avoid dangling references if an installation selection changes.
- The unchanged original DemoV1 presenter/rendering and source-derived character-stat algorithms now accept `CharacterVisualSource`, not concrete `RavenloftBootAssets`. The DemoV1 loader implements this interface without altering its resource decode or character rules.
- Extended `native_startup_visuals_tests` with disposable noncopyrighted RES1/RES7/RES8 and CODE.1 fixtures, polymorphic read checks, little-endian CODE.1 fields, string termination and rejected invalid archive/offset cases.

## Evidence / acceptance boundary
- GitHub branch and changed file reads reconfirmed; original supplied `CODE.1` independently checked for length **277787** bytes, original DGROUP offset **0x3E724**, and bounded length **22007** bytes.
- **No new compiled Windows or Linux test result is claimed here.** The test changes are authored; the new source has not been independently certified by a successful current-HEAD CTest in this review. Earlier 38/38 A1 cinematic CI covers an older source, not this checkpoint.
- This commit does **not** render CODE.1 character screens inside `rloft_native.exe`, play character audiovisual effects, generate CHARS, create a party, modify original installations or make the initial level playable.

## Next bounded integration
Move the existing `DemoV1` `CharacterCreationPresenter`, `CharacterPaletteRemaps`, and `CharacterStatsModel` implementations into a neutral shared CMake target without modifying their algorithms. Link both executables to one implementation and expose the native first CODE.1 source-backed opening/controller frame before wiring input, CHARS transfer and world launch. Require a real Windows CTest + original-media rendering audit and owner GUI inspection before declaring A1 presenter integration accepted.
