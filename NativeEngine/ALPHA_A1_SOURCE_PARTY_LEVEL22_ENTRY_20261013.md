# Alpha A1 — original CHARS party into source Level 22

**Repository:** `Hyperbog/RLOFT`, **branch:** `native-engine`  
**Source checkpoint:** `c78c1272040ad17686a728a94ff51b5c6c1704b1`  
**Date:** October 13, 2026  
**Gate:** Code integrated, **new revision not yet Windows-compiled, visually accepted, or game-played**.

## Native first-party entry (new code)

The existing source-backed CODE.2/CODE.1 presentation, character choices/stats, final decision and name-entry flow now hands the shared original 0x920-byte `CHARS` image to an **independent native party importer**, `NativeEngine/include/rloft_native/native_party_start.hpp` and `src/native_party_start.cpp`.

- The importer retains every byte of all four 0x248-byte records and decodes **two accepted actors** with original name/sex/race/class/portrait/alignment and current/max HP fields. It fails on malformed status, identity, class, original-name byte or unterminated name. This is a fresh CODE.1 generated-party importer; it does not yet certify arbitrary DOS saved-party imports.
- `NativeEngine/src/win32_native_startup.cpp` now calls `open_native_gpu_world_session` with the completed in-memory CHARS data. The normal `BootFlow` advances to **CODE.2 Exploration** only after native world window creation succeeds. If graphics initialization fails, the source transfer is retained and Enter retries without rerolling or changing original files.
- The independent GPU runtime now starts a **party-bound** scene at original **Level 22**, **(4.5, 4.5)**, **270°**; it rejects a nonpassable original starting cell instead of teleporting to a chosen debug location. The source party and character names remain attached to the runtime window. Free map cycling and noclip are disabled for this party-bound session, while the separate technical viewport preview keeps its diagnostic controls.
- After selecting the original installation in the Win32 launcher, the **ordinary character/cinematic startup launches automatically** instead of requiring the developer inspector's second button. The separate inspector is preserved; `--play <path>` also selects the ordinary startup, while prior headless verify flags and `--install` stay available.
- No `CHARS` or profile save file is written. The session stores CHARS and mutable level world state in memory only.

## Evidence

- Original supplied `LEVELS.DAT` was independently inspected: level index **22** has dimensions **30×15** and cell **(4,4)** has unmodified word **0x0000**, consistent with the original source ordinary-passable tile. Prior DemoV1 source documented the exact start as `(4.5,4.5), 270°, 14:00`. This native session does **not yet** implement the complete CODE.2 time/event/game-state startup.
- New `native_party_start_tests` round-trips two **synthetic test-only** actors through the existing shared CODE.1 serializer then native importer; checks preserved `0x920` bytes, record boundaries, source starting pose and rejection of malformed actor fields.
- `rloft_native_startup_audit` now reads the actual original `LEVELS.DAT` and asserts source Level 22 dimensions, exact cell 0x0000, and ordinary collision passability. Windows startup CI adds the original LEVELS.DAT SHA guard and JSON acceptance.
- Final GitHub source check found matching definitions/call sites, registered new source and test targets, and balanced C++ lexical delimiters and string literals. **This is not a compiler run or observed Windows interaction.**

## Remaining A1 / A2 / playable-Alpha gaps

1. **Exact-source acceptance:** obtain a passing Windows x64 MSVC compile and CTest run including the party importer, source original-level audit and shared-source DemoV1 regressions; then have the owner exercise the complete GUI flow on an actual GPU. A past 38/38 Windows pass applies to older cinematic-only code, **not** this revision.
2. **Startup productization:** the launcher still exists behind the game window and requires the user to identify their owned original installation. The new `--play` mode and auto-start after source selection are the first normal-entry routes, not a finished launcher or existing-CHARS import.
3. **World A2:** Source Level 22 is **rendered using the provisional GPU geometry** and has collision and camera, but party movement has no full CODE.2 event/interaction/actor/inventory state. RES3 scenery/door/encounter handling and original GPU/DOS parity remain open.
4. **Alpha A3/A4:** Source event progression, inventory/equipment/combat, native profile save and restart/load, cinematic VOC/XMIDI audio, complete screens/transition timing, and end-to-end user field acceptance are not integrated.

**Next:** solve any current-HEAD CI failures before calling this integration accepted; then move level22's original authored placements, event/controller logic and party-based inventory into this session, with a separate profile save/reload gate. Do not prioritize Level22→23 transition before normal startup and first-level playability.
