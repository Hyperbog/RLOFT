# Canonical CODE.1 / CODE.2 whole-source compilation and linking gate

This is the first **actual C++ compilation, linker and executable runtime**
gate on the `code1-decompilation` branch. The target includes both canonical
`Code1/Code1.cpp` and `Code2/Code2.cpp`, unchanged, in separate compilation
units and links them into one executable. Its three lightweight probes call
**compiled canonical reconstruction functions**, not synthetic replacements:

- CODE.1 `7F50`: real DGROUP 32-bit tick read.
- CODE.2 `3A80A`: real DGROUP 32-bit value read.
- CODE.2 `353A0`: the real original entry stub, checked against its original
  `tail_35418` boundary contract (the downstream callback is deliberately an
  observation witness, **not** a fabricated implementation of DOS startup).

Run on Windows with MSVC and CMake:

```powershell
cmake -S Recomp -B build/recomp -A x64
cmake --build build/recomp --config Debug --parallel 2
ctest --test-dir build/recomp -C Debug --output-on-failure
```

**What a pass proves:** the two current source files successfully compile as
code (not `-fsyntax-only`), their symbols can link into an executable, and
three restricted source-backed entry/leaf operations execute successfully.

**What a pass does NOT prove:** original LE loading, correct loaded data,
DOS services/interrupts/graphics, the entry-to-35418 *implementation*, a
working character creation screen, gameplay, or DOS pixel/timing parity.
The whole-game recompilation remains **unbuilt** until those gates pass.

Next milestone after this build gate is the first original entry-point execution
with authenticated data-object initialization and a source-backed DOS service
boundary (no null or fabricated callback counted as progress). Explicitly
record the first actual boot blocker and fix it, instead of more compiler
fingerprinting research.

This project is strictly separate from `demo-v1` and `native-engine`. Do not
invoke their full Windows build workflows from this target.

## 2026-10-12 first host-backed original DOS file-service integration

[Windows build #49](https://github.com/Hyperbog/RLOFT/actions/runs/38208279160)
compiled and passed **33/33 CTests** at exact source SHA
e91263ff89dccf20b3335d4171ff5d76dfd3dbed.

Actual compiled canonical CODE.2 sound initialization now optionally runs its
original DOS AH=3Dh open, AH=3Fh read and AH=3Eh close wrappers through a
**host-backed, strictly read-only** file service. The tests use synthetic
startup/PSP/mouse fixtures and a generated noncommercial 59-byte SOUND.CFG.
The next boundary is the original unsupported DOS mouse-reset interrupt.

For Windows owner testing, invoke:

    .\build\recomp\Debug\rloft_code2_startup_gate.exe --to-mouse-reset-host-file "C:\RLOFT"

The CODE.2 image remains synthetic in this mode. No playable DOS boot, real
interrupts or whole-game parity is claimed. Acceptance remains **3/12 =
25.00%** because complete filesystem services (R05) remain unimplemented.

[Source-execution checkpoint](HOST_READONLY_DOS_FILE_SOURCE_GATE_20261012.md).

## 2026-10-12 verified canonical recompilation expansion

[Windows build #41](https://github.com/Hyperbog/RLOFT/actions/runs/38207402256):
**31/31 CTests passed** at exact tested source
`fbbc7ae61c6793d41264275a2a1115a941962533`; latest compiled
developer tools include a fourth `rloft_code1_startup_gate` executable
and an extended `rloft_code2_startup_gate`.

The actual CODE.1 source `CC1C` startup now executes to its original
DOS version boundary, the `CD17 -> CD50` DOS AX=FF00h request and the
`CCDE` DOS AH=4Ah resize boundary (three explicit fixture-controlled
branches). Actual reconstructed CODE.2 runtime additionally executes
DPMI `0101h` release, mouse X/Y range `33D18/33D38`, and can
return from original `2F2EC` into `20440 -> 202F0 -> 35286`,
stopping before DOS `INT21h AH=36h` disk-free inquiry. The CODE.2
authenticated-media runner has **19 distinct source-execution modes**.
These are bounded source-execution witnesses, not actual DOS hardware
implementations or game boot.

The accepted major milestone score remains **3/12 = 25.00%**. The
remaining 9 milestones cover actual DOS/DPMI service providers, original
character-creation handoff, original graphics/sound/input and full
gameplay behavior and DOS parity.

[Complete source-test/evidence checkpoint](CANONICAL_CODE1_CODE2_STARTUP_CHECKPOINT_20261012.md).

## Measured recompilation progress — 25.0% (3 / 12 accepted gates)

The canonical recompilation percentage is now measured using
[the fixed 12-gate acceptance ledger](ACCEPTANCE_GATES.json), computed by
`python Recomp/tools/recomp_progress.py` and tested on every Windows source
build. **3/12 = 25.0%** major acceptance milestones have passed:

1. Both original CODE.1 and CODE.2 reconstructed translation units compile,
   link and execute source-backed probes.
2. Both original LE executable memory images (objects, pages, relocations)
   are independently reconstructed and checked against user-supplied bytes.
3. The actual compiled CODE.2 startup, original initializer scheduler,
   four original initializers and source-backed game-entry path execute under
   strictly labeled bounded host fixtures.

The remaining **9/12** acceptance milestones cover real DOS/DPMI memory,
full filesystem services, CODE.1 gameplay/CHARS handoff, game-visible
hardware interrupt handling, graphics/audio resource output, first
interactive gameplay, all levels/events, save/load, and independent
original-DOS runtime parity. Those milestones are **not** accepted simply
because related routines compile or test-only service fixtures work.
This percentage is a disciplined engineering milestone count, **not
25% playable-game completion**.

The newest CODE.2 original-code source witness additionally executes
`33B44 → 30D80 → 30D52 → 37A73` with an explicitly simulated
successful DPMI `0100h` allocation result, then executes the original
`33AA0` real-mode mouse interrupt bridge and stops **before
`INT 31h / 0300h`**. No original DOS interrupt service is asserted.
The no-device branch executes `311E8` and stops before DOS AH=09h.
Both paths are included in the **15-mode authenticated media matrix**.

[Windows source build #35](https://github.com/Hyperbog/RLOFT/actions/runs/38206495432)
at exact source `5c1efa61022ccbe93cf19ed3c98b755a269ab80c`
passed **24/24 CTests**, including the original reconstructed-code
DPMI 0300 source path and acceptance-score invariants. Windows CI does
not bundle owner-original media or verify DOS game parity.

## 2026-10-12 latest verified canonical recompilation checkpoint

**The current canonical workstream is `Recomp/` on `code1-decompilation`,
not either native engine.** [Windows canonical source build #34](https://github.com/Hyperbog/RLOFT/actions/runs/38202963930)
successfully compiled/linked the existing original CODE.1 and CODE.2
reconstructions and passed **22/22 CTests** (exact source
`f7f94d8c38153707fde6eacc2474556442c50ec4`).

Unlike the earlier game-entry stop, the extended compiled source
now executes the original CODE.2 startup through `20608`,
authored original ISR/vector/allocator/audio open/read/close paths
and `33B44` mouse reset. It selects both original mouse branches:
the device-present branch executes canonical `30D80 → 30D52 →
37A73` before halting at the genuine external `INT 31h AX=0100h`
allocation dependency; the device-absent branch executes canonical
`311E8` before stopping at the real external `INT 21h AH=09`
output dependency. Those branches are driven by **explicit test-only
hardware outcomes**, not a claimed original DOS trace.

The original media matrix runs **14 bounded execution modes**, enforces
the archived original CODE.2/SOUND.CFG SHA-256 identities, requires
the precise first unsupported original service boundary, and emits
a machine-readable audit. The Windows CI uses synthetic media, while
local owners can explicitly supply original CODE.2/SOUND.CFG to the
same compiled original-code executable. Details and commands:
[Original media and mouse source-execution evidence](ORIGINAL_MEDIA_AND_MOUSE_BRANCH_GATE_20261012.md).

**Still missing:** actual DOS/4GW services/memory/IRQ/graphics,
full CODE.1-to-CODE.2 live game handoff, real DOS gameplay-equivalence
and complete original game execution. CI passing cannot certify those.

## Whole original LE image integration (2026-10-11)

The current source tree now also builds `rloft_original_image_gate`, which reconstructs **the entire original LE code/data memory image** for both modules and validates all original object/page/fixup records against the original executable bytes. Its synthetic, media-independent CTest is registered beside the existing canonical source-link CTest.

After building, to run against personally installed original media (not included in this repository):

```powershell
.\build\recomp\Debug\rloft_original_image_gate.exe "C:\RLOFT\CODE.1" "C:\RLOFT\CODE.2"
.\build\recomp\Debug\rloft_recomp_link_gate.exe "C:\RLOFT\CODE.1" "C:\RLOFT\CODE.2"
```

The latter additionally calls the **compiled canonical CODE.1/CODE.2 memory getter functions** with the actual loaded data objects. Without those two arguments, CI explicitly reports `CANONICAL_SOURCE_WITH_ORIGINAL_DGROUP=NOT_RUN_ASSETS_ABSENT`; it never silently substitutes synthetic data for a claimed original-file acceptance.

The original-file loader passed GCC C++17 validation using authenticated `CODE.1` and `CODE.2` files: **12,430/12,430 original fixups, seven signed backward-page cases**. Windows [run #38173341351](https://github.com/Hyperbog/RLOFT/actions/runs/38173341351) passed the expanded two-test source-only CMake project at commit `7d16425d014a6afd20a907ae0569cec85bc4b113`. See [whole-image evidence and limitations](ORIGINAL_LE_IMAGE_INTEGRATION_20261011.md).

**Next gate is real startup:** `CODE.2:353A0→35418` with verified DOS/4GW services and original selector/PSP/environment state. This loader does not by itself execute or certify DOS game startup.

## 2026-10-11 original CODE.2 startup to game-entry boundary

The extended Windows CTest now **executes the canonical reconstructed original `353A0→35418→395E2` startup, all four actual initializer callbacks `3A16E,3A2AD,3A1B3,3968E`, the original core allocator request path `37B42`, and post initializer `39589`**, then stops at the actual game-entry callback `20608`. The original selector environment adapter and arena memory allocator remain **test-only bounded fixtures**, so this is not DOS/Watcom heap parity and does not boot the game. The earlier first-initializer gate remains a separate CTest; the expanded CMake project has **4 tests**. [Implementation, original span oracle and acceptance](INITIALIZER_20608_INTEGRATION_20261011.md).

[Windows source-only Actions run #38186500373](https://github.com/Hyperbog/RLOFT/actions/runs/38186500373) passed **4/4** tests at executable source commit `7af91b0efe974ff781c1eb4b6642e92ee9b001b5`. The owner-uploaded CODE.2 original is checked separately with `Recomp/tools/verify_code2_startup_oracle.py`; no original game files are in the Windows runner.

## Original termination callback source execution — 2026-10-11

The CODE.2 static LE table resolver independently authenticated an otherwise easily missed **termination callback at `0x3A228`**. Original DGROUP record `0x12738..0x1273D` has **five file-backed bytes and one LE zero-fill byte**, producing target `0x3A228`, priority `0x20`, initial flag `0`. The terminating dispatcher executes the real original `function_3962d` callback scanning routine, which calls the reconstructed original `function_3a228` (stream close then exit) when its source callback is selected.

`Recomp/src/code2_startup_gate.cpp` now exposes a separate `--termination-probe` test path, using the original static table values with a zeroed source stream list and a **fail-closed synthetic OS exit handler**. It checks the selected original `0x3A228` callback and the original scheduler's argument `0xFF00` before stopping at the unimplemented DOS exit `0x3A63E`. A full original stream-close path and actual DOS process termination are **not executed**.

[Windows canonical source CI run #38186890574](https://github.com/Hyperbog/RLOFT/actions/runs/38186890574), source commit `07e70d64e493dc1fd0a4a74547aca00706828e2a`, passed **all 5/5 CTests**, including the existing four-initializer game-entry test and new original shutdown scheduler probe. The original media were **not included** in Windows CI. The owner-uploaded `CODE.2` was separately authenticated locally with its full SHA-256, original scheduler instruction-span SHA-256s, exact indirect `FF D0` call opcodes, the four initialization records, and the six-byte shutdown record (five original file bytes plus one virtual zero-fill byte). Source analysis does not establish dynamic DOS target immutability.

## 2026-10-11 actual original allocator arena source integration

The canonical CODE.2 startup and stand-alone arena/rollback probes now execute reconstructed **`37B42 → 39838`** block selection, splitting and linked-list metadata, plus **`37C37 → 398E0`** free/coalescing. The original-byte verified `37BC2 MOV EAX,EDI` request-ABI discrepancy was fixed (raw seven-byte requests produce 12-byte arena blocks rather than 16). Test-only memory still seeds one bounded guest arena and explicitly injects one failure; neither test models DOS/DPMI allocation. [Detailed evidence and exact branch sync](CODE2_ORIGINAL_ARENA_CLOSURE_20261011.md).

[Canonical Windows source compile/link CTest #38190078813](https://github.com/Hyperbog/RLOFT/actions/runs/38190078813) passed **7/7** on `16a2ca0f93adb2e90403e1e45995edccbc22bd72`. The original `20608` game application code is still **not executed**; DOS runtime parity and native game boot are not established.

## 2026-10-11 canonical original game-main execution (new integration)

The existing source gate previously stopped *before* the original application
`CODE.2:20608`. Three new Windows CTests execute the actual compiled
`353A0→35418→395E2→39589→20608→20440→2F2EC` path, using the four
original callback initializers and the existing bounded DOS service/allocator
fixture. The original 20608 game-state writes, actual 352EE getenv lookup,
39526 case comparator, 35349 decimal parser and first original 2F2EC
audio/resource initialization writes are all exercised. The source stops at
the **real original DOS/4GW 375FE far callback installation boundary**;
it does not fabricate a successful interrupt or continue pretending to
boot gameplay. A default path, configured original environment and
negative/overflow variant are separately asserted.

Run:
```powershell
ctest --test-dir build/recomp -C Debug --output-on-failure
.\build\recomp\Debug\rloft_code2_startup_gate.exe --enter-game "C:\RLOFT\CODE.2"
python Recomp/tools/verify_original_game_entry_oracle.py "C:\RLOFT\CODE.2"
```

Original source spans and actual owner-provided binary settings were
independently hashed and authenticated. See
[the original game-entry integration audit](GAME_ENTRY_20608_SOURCE_EXECUTION_20261011.md).
**Build verification:** [canonical Windows CI run #38196507014](https://github.com/Hyperbog/RLOFT/actions/runs/38196507014)
compiled and passed **10/10 CTests** on exact commit
`56139409213a0bf4af19dc0adb060b477a5baa16`, including all three
new original game-entry runtime executions.

**Further source integration:** The actual canonical `function_375fe` now
runs from the original `2F2EC` callback, preserving its original DOS
extender-dependent interrupt-vector choice; the source stops **inside
375FE at the DOS vector-install service** rather than at a preceding
callback. A fourth independent integration CTest exercises all 256 mode
values against the original selector/vector/handler contract. The
[original game-entry source audit](GAME_ENTRY_20608_SOURCE_EXECUTION_20261011.md)
documents this change. A fresh Windows build is required for this extension.

**No full DOS game boot, visual gameplay or native-engine milestone is
claimed.** Passing CTest proves the named source-execution boundaries only,
not original-DOS hardware, audio or frame equivalence.

## 2026-10-11 actual canonical CODE.2 sound.cfg I/O boundary

[Windows canonical CI #38196695480](https://github.com/Hyperbog/RLOFT/actions/runs/38196695480)
passed **11/11 CTests** at exact source revision
`7d62306bc860136e9564e7f45e22dd7896f109aa`, including the
256-mode original 375FE source callback switch. That checkout reaches and
executes original game application 20608, audio setup 2F2EC and vector
selection 375FE.

The next original-source integration now also executes real
`30BB4` IRQ vector save/install, `290D0/30CC4` RNG seeding,
`3104C/37B34/37B42/39838` checked source allocation, and
`30D30/379D0` original zero fill, then real
`312A0` until the `37E77` **sound.cfg DOS file-open service**.
No host interrupts or copyrighted game-data file are accessed by the
synthetic Windows CTests. The *external* DOS IVT is explicitly a
bounded in-memory test fixture; compiled original source owns the
vector save/install and hardware setup ordering. Two CTests use the
default original and fully configured source environment paths.

The [complete game-entry integration record](GAME_ENTRY_20608_SOURCE_EXECUTION_20261011.md)
describes source evidence and acceptance boundaries. A passing new
Windows build is required for these added paths. **Still not a
DOS-accurate game boot or native engine.**

## 2026-10-11 canonical startup now executes BOTH real audio entry paths

[Windows canonical build #22](https://github.com/Hyperbog/RLOFT/actions/runs/38197663147)
passed **15/15** tests at exact source checkout
`4c10427ed1d73e7d2c9ad1b96db9e3e1f939ed0d`. This is
source execution only, not DOS gameplay parity.

The source now executes original `353A0→35418→3968E→20608→2F2EC`,
including the **two genuinely different original argc branches**:
`argc=1` calls original checked `3104C` allocator and original
`30D30` memset; `argc=2` uses the literal PSP command-line
argument parsed by the original `3968E` and original `35349`.
Both default and explicitly configured environment modes are covered.
The separate DOS/4GW extender selector byte `12662h` is **not
argc**; it selects `375FE`'s interrupt mode. In canonical
`Code2.cpp`, the actual original `375FE` code now also stores the
incoming old far address/selector into DGROUP `105F2/105F6` as
required by original x86 instructions. This fixes a concrete bug in
the formerly byte-certified original C++ reconstruction.

**Further source advancement:** from either argc branch, both original
`312A0` and `37E77→37E99` DOS open-path functions are now
executed and the process **halts at the actual external DOS AH=3Dh
sound.cfg open call**. Its read access/share mask is checked against
original encoded semantics. No synthetic success return is supplied
from DOS, no hardware vectors are touched in the host, and no sound
file is read by the CI fixture. See
[full original game-entry and DOS-boundary evidence](GAME_ENTRY_20608_SOURCE_EXECUTION_20261011.md)
and [separate CODE.2 semantic correction](../Code2/PROGRESS.md).

The current branch's new DOS-service-boundary C++ additions require
their **own exact-SHA passing Windows CTest**, independently of build
#22 and of any future original DOS game/visual acceptance.

## 2026-10-11 source-executed sound.cfg open/read/close to mouse setup

[Canonical Windows source CI #38197945672](https://github.com/Hyperbog/RLOFT/actions/runs/38197945672)
passed **15/15** original-reconstructed-code CTests at exact checkout
`c0767e73e1fd93c435f623eb8280aad768d33ee9`, including
source-executed `312A0 -> 37E77 -> 37E99` DOS open control flow
stopping at actual DOS AH=3Dh.

**Next integrated source checkpoint** (this revision):
for four PSP/environment scenarios, execute the complete original
`312A0→37E77→37E99` open, `312EC→38106` read,
`312DC→380F8` close and the original `33B44` mouse setup;
**stop at the actual DOS INT 33h mouse-reset dependency**.

The new open/read/close return values and 59-byte SOUND.CFG sample
are **test-only virtual DOS fixtures**, not original files or a
claimed DOS game boot; original game files are never distributed in CI.
All game application, arithmetic, path, descriptor, allocation,
character/keyboard/timer setup and file-wrapper decisions under test
execute *compiled canonical CODE.2 source*, not the native engine.
[Technical source evidence and limits](GAME_ENTRY_20608_SOURCE_EXECUTION_20261011.md).

The new four CTests and their binary integration are separate
from the previous 15/15 green checkpoint until CI confirms the
exact new checkout.

## 2026-10-11 compiled original SOURCE file-read and DOS mouse boundary — VERIFIED

**[Windows canonical source build #29](https://github.com/Hyperbog/RLOFT/actions/runs/38198547164):
19/19 CTests PASS**, exact compiled source
`5e91a0fa7a13b8c5a933a16ea0e200571c321afa`.
Four tests cover default and configured DOS environment under both authentic
CODE.2 `argc=1` allocation and `argc=2` decimal pointer mode. This
build executes the original CODE.1/CODE.2 source through
`353A0 -> 35418 -> 395E2 -> 20608 -> 2F2EC -> 375FE -> 30BB4 ->
312A0 -> 37E77 -> 37E99 -> 312EC -> 38106 -> 312DC ->
380F8 -> 33B44` and **stops at the actual DOS INT 33h mouse-reset
service**. The no-arg route additionally executes the original checked
allocator and zero-fill path. The test-only read-only SOUND.CFG fixture is
59 bytes; its exact copy destination is the *base* of the original audio
descriptor, followed by separate `+0x64/+0x66` state updates, all asserted.
No game install files, proprietary resources or original DOS interrupts
are loaded in Windows CI, and the **full game does not boot yet**.

Canonical `Code2.cpp`'s historical `375FE` C++ omission was repaired:
original x86 stores the previous far pointer at `105F2/105F6` and uses
extender byte `12662`, whereas global `105FC` is parsed argument count.
`Recomp/tools/verify_original_game_entry_oracle.py` independently pins
**23 original CODE.2 instruction spans** and exact control/data opcode
windows against owner-supplied authentic original CODE.2 SHA-256.
See [detailed engineering and original binary evidence](GAME_ENTRY_20608_SOURCE_EXECUTION_20261011.md).

The Windows workflow now publishes **all three** compiled canonical
developer executables (not just the older leaf probe), exact source SHA,
and plain-text usage/disclaimer. This packaging change is subject to a
new exact-SHA CI gate. It does not publish or claim a playable game.
