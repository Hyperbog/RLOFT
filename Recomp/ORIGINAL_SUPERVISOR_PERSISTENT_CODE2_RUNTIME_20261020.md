# Part 30 - R.EXE Native Supervisor, Persistent CODE.2 State (2026-10-20)

STATUS: IMPLEMENTED SOURCE-LEVEL LIFECYCLE / PERSISTENT CODE.2 RESOURCE GUEST. NOT A PLAYABLE OR VISUALLY BOOTED NATIVE GAME.

## Original process order

The original 16-bit R.EXE supervisor (SHA-256 a6d5874dc91fb068a3ed0a07290547c310ee7f4f50be4d8176c4788f251631b0) starts CODE.2 first. CODE.2 exit status 10 launches CODE.1; CODE.1 exit status 10 resumes CODE.2 and produces a 0x920-byte CHARS image; CODE.2 exit status 12 erases CHARS and restarts CODE.2. Other terminal process exits stop and erase CHARS. Sources: Runtime/STATE_TRANSITION_PROTOCOL.md and PreservationArchive/01_EXECUTABLE_CONTAINER_AND_MODULE_CYCLE.md.

## Added native runtime lifecycle

- Recomp/src/original_r_exe_native_session.hpp: a resumable R.EXE supervisor state machine with retained module object across arbitrarily many Pending steps. Only a genuine child Exited response may cause supervisor status transitions. Absent child callback blocks without manufacturing exit 10 or a pretend successful boot.
- CHARS is an opaque exact-width 0x920-byte payload (four 0x248 original party records). CODE.1 status 10 requires a producer; CODE.2 receives each byte intact on next actual launch. Status 12 and ordinary termination wipe the payload.
- Recomp/tests/original_r_exe_native_session_test.cpp: tests persistent CODE.2 pending, real transition protocol with synthetic child exit events, unchanged 2336-byte payload, missing CHARS refusal and missing child refusal. Synthetic tests certify protocol, not actual original gameplay.

## Persistent CODE.2 source guest

- Recomp/src/original_code2_guest_session.hpp: opaque bounded live session API.
- Recomp/src/code2_original_res_startup.cpp: authentic original CODE.2 data object, 16D04 original INIT, 2E7F0 INFO/11 archive cache initialization, original scene-selector bank, source-derived 325B0 COLORS read, compiled 32150 brightness and compiled 29CA0 DAC output. Resource guest remains alive after the first dispatch.
- Recomp/src/code1_code2_staged_boot.cpp: original R.EXE SHA pinned and CODE.2 started first as a real native SupervisorSession child. Executes two consecutive Pending steps against the SAME guest RAM, original resource descriptors and palette; verifies memory pointer, resource cache base, archive count, scene bank, 769 DAC port writes and 16MiB memory-state fingerprint are unchanged. No child completion or guest reset is manufactured. Independent CODE.1 constructors run as explicitly separate diagnostics, not as an unauthorized R.EXE child.

This proves memory and resource persistence, not execution of the original 20608 gameplay entry or an animated frame. The original CODE.2 runtime code at Recomp/src/code2_startup_gate.cpp already enters 20608 through original 353A0 startup under bounded DOS/4GW fixtures, but that runtime and this long-lived resource guest are still separate. Unifying them into a single guest with source input interrupt, DPMI heap, host OS services and code2 1FC14..20293 simulation loop remains the critical blocker.

Acceptance gates: Recomp/CMakeLists.txt portable and Windows tests; original-media Windows milestone also requires RLOFT_CODE2_SAME_GUEST_PERSISTENCE_PASS marker. No original game files, proprietary palettes or user saves included in source changes.

NO USER-VISIBLE BOOTABLE EXE IS CLAIMED FROM THIS CHECKPOINT. Only call a new build a native game when original main, initial first frame and subsequent event-driven input/updates execute.
