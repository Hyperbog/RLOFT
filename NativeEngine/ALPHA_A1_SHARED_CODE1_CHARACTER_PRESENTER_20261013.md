# Alpha A1 — shared original CODE.1 visual presenter through actor/portrait choices

**Canonical branch:** `native-engine`  
**Code checkpoint:** `eea3fbc65986cbf77e62c546ec786da81b7f09c8`  
**Date:** October 13, 2026  
**Acceptance:** Implemented source; compiled CI and physical Windows GUI acceptance **NOT confirmed** at this checkpoint. **Pre-Alpha.**

## Changes
- Moved the original `CharacterCreationPresenter` (2,608 lines), `CharacterPaletteRemaps` (322 lines), and `CharacterStatsModel` (364 lines) without algorithm changes from `DemoV1/src` into `Shared/StartupPresentation/src`; moved their original headers into shared include directory. DemoV1's previous public headers now forward there and preserve its preexisting transitive resource import.
- The shared presentation library links the existing neutral `Shared/CharacterCreation` logic. DemoV1's CMake source list no longer compiles duplicate character code; NativeEngine uses that same shared implementation and its own `NativeStartupVisualSource` (original files read-only).
- In the independent native Windows title window, original CODE.2 character option selects the original supervisor and enters the 70-frame CODE.1 opening at approximately 70-Hz host presentation. The real original first actor controller follows; clicks are source-mapped through `CharacterCreationFlow::hit_test` and `apply_event` for actor, sex, race, class, option and portrait-page choice stages. User clicks are resolved from the current event coordinates instead of stale hover.
- Portrait acceptance is deliberately blocked before original RNG/stats, final naming, `CHARS` creation, party construction and world entry. No synthetic party or installation-file writes.
- `native_startup_visuals_tests` now invokes the shared CODE.1 opening at first/last frames using noncopyrighted EH1 fixtures, and validates invalid opening frame rejection.
- `rloft_native_startup_audit` now draws all original source CODE.1 opening frames, first actor screen and first sex-selection screen. `native-startup-windows-ci.yml` hashes original `RES0`, `RES1`, `RES7`, `RES8`, `CODE.1`, `T`, `COLORS`, `INIT` before and after.

## Evidence and limits
The GitHub branch paths and source organization have been inspected. The prior [Windows original cinematic run #38346464375](https://github.com/Hyperbog/RLOFT/actions/runs/38346464375) passed 38/38 **for an older SHA only**. No passing new-head CTest, original game media audit result, actual Win32 user input or DOS indexed-frame equivalence is available to claim yet.

**Next gate:** Confirm the current-code Windows compiler/CTest and real-original source audit; repair any concrete source/asset integration defects. Then implement source-backed character stat generation, final decision/name screens, real original 0x920 CHARS payload handoff and first gameplay session.
