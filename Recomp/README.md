# Canonical CODE.1 / CODE.2 whole-source compilation and linking gate

## R04 source-backed DOS fallback — Windows #122 (2026-10-12)

[Canonical Windows #122](https://github.com/Hyperbog/RLOFT/actions/runs/38287503152)
passed **55/55 CTests** at source
`2dacfa6caaa3d18856612d1d961f9091cf7f1046`.
Four new opt-in continuous original CODE.2 source startup tests
execute **353A0→35418→395E2** through game entry **20608**, argc=1
sound initialization and mouse reset, after original `39C7B` selects
DOS AH=48h fallback for a deliberately rejected DPMI 0501 request.
Tracked conventional DOS paragraph pages are physically isolated
from static LE DGROUP. The original source creates all heap arenas,
and the host validates DOS probe/request/49h release, ownership and
free-list metadata. Earlier live DPMI startup modes remain independently
green.

CODE.1 `DB5B→10590` source free-list split/coalesce and original
`107CB` DPMI/DOS reclamation were also independently verified at
[Windows #117](https://github.com/Hyperbog/RLOFT/actions/runs/38271837071)
(51/51). The earlier pending wording below is historical.

[Detailed checkpoint and remaining gaps](R04_CODE2_CONTINUOUS_DOS48_FALLBACK_20261012.md).

**R04 is still OPEN: overall recompilation 3/12 = 25.00%.**
The continuous CODE.2 alternate `39E1C` source-selected DS-break
mode, cross-CODE.1/CODE.2 unified DOS/DPMI descriptor lifetimes, and
independently captured original-DOS/4GW memory observations are not
yet implemented or verified. These source CTests do not constitute
DOS runtime parity or a playable game.



## Latest R04 CODE.1 source free-list checkpoint (2026-10-12)

The original CODE.1 `DB5B -> 10590` reconstructed free-list path is
now wired into both the host-DPMI and DOS-paragraph fallback source
regressions. The tests exercise original `104E0` split, original
`DB5B/10590` circular insertion and adjacent coalescence, followed by
source `107CB` reclamation. The DPMI `1083F` relocation precondition
also comes from an original `104E0` allocation instead of test-authored
allocated-bit mutation. Only four authenticated original CODE.1
DGROUP heap-list root words are seeded; all dynamic arenas remain
host-owned and source-allocated.

**CI for these new commits is not yet independently verified.** The
last recorded successful Windows gate is still [#113](https://github.com/Hyperbog/RLOFT/actions/runs/38260777313),
51/51 CTests. See
[the CODE.1 DB5B/10590 source checkpoint](R04_CODE1_DB5B_SOURCE_FREE_LIST_20261012.md).

**R04 remains NOT ACCEPTED; 3/12 = 25.00%.** The continuous alternative
startup modes, unified selector semantics and original-DOS behavioral
comparison are unresolved.

## Latest R04 checkpoint — DOS fallback, extender growth, free-list repair (2026-10-12)

[Windows build #113](https://github.com/Hyperbog/RLOFT/actions/runs/38260777313)
passed **51/51 CTests**. Compiled original CODE.2 now exercises
`39C7B` DOS paragraph allocation fallback, probe rollback, and original
`39A2C` DOS selector reclamation against a separate shared guest
conventional-memory aperture. An additional source test executes original
`39E1C` mode-2 DS-limit/break extension, multiple adjacent arena
extensions, full `398E0` free/coalescence and large-block reuse without
new host allocation.

The expanded source test exposed a **real reconstruction defect** in
`Code2/Code2.cpp`: the old `398E0` search skipped the circular-list
sentinel during high-address free insertion. The correction was checked
against original x86 instructions `39939..39946`, including the
`JA 3999E` sentinel insertion branch. The original binary was not
changed. See
[the verified CODE.2 DOS/39E1C source checkpoint](R04_CODE2_DOS_FALLBACK_BREAK_GROWTH_20261012.md).

**R04 remains unaccepted; recompilation is 3/12 = 25.00%.**
CODE.1 `DB5B/10590` source free-list integration, non-DPMI
startup-mode linkage, unified selector/limit ownership and original
DOS differential memory observations remain outstanding.
The chronological status sections below are archival checkpoints.


## Current R04 checkpoint — continuous original CODE.2 host DPMI heap (2026-10-12)

[Windows #106](https://github.com/Hyperbog/RLOFT/actions/runs/38257869955)
**50/50 CTests passed** at exact source SHA
`d09522858ebcc3b8ba659251f37973642252f116`.
Four opt-in original-source startup integration tests now reach the
four original CODE.2 initializers, `20608` game entry, argc=1 sound
initialization, and the mouse-reset stop boundary using **a live
unseeded host DPMI 0501-backed heap**. Original reconstructed
`37B42/39D4D/39C7B/39BBC/39838/398E0` owns its arena structures;
the host supplies tracked guest pages and source-bound dispatch.
The original `35418` extender handshake invokes a host `0006`
selector base lookup, and reconstructed `3A395` enables the heap
through a host-initiated source call.

The original DOS version, extender-presence result, PSP/environment,
interrupts, mouse and hardware remain **bounded fixtures**. This is
neither a playable game nor full DOS/4GW memory parity. Older fixed
fixture heap tests remain separate.

[Full R04 continuous-startup audit](R04_CODE2_CONTINUOUS_HOST_DPMI_STARTUP_20261012.md).

**R04 remains UNACCEPTED; recompilation is 3/12 = 25.00%.**
Next: complete CODE.1 `DB5B` free-list integration, CODE.2
`39E1C` and DOS fallback, cross-runtime descriptor ownership,
and original-DOS comparison. Continue R04 before other milestones.


## Current R04 focus: verified original CODE.2 0503/0502 lifecycle (2026-10-12)

[Windows source gate #98](https://github.com/Hyperbog/RLOFT/actions/runs/38254878225)
passed **46/46 CTests** after correcting a source-reconstruction defect in
`Code2/Code2.cpp`: original CODE.2 instruction `39A43: CMP EAX,EDX`
compares the full-free-block-plus-header length with the **arena size**,
not the numeric arena pointer. The new regression exercises authentic
reconstructed original `39A8A` DPMI `0503` resize/relocation,
`37C37/398E0` free and coalescing, and original `39A2C` automatic
DPMI `0502` reclamation with zero preseeded arena headers. Original
source user bytes survive and old handles/linear mappings are retired.
The test failed before this correction in Windows #97, and passed
afterward in #98. See the
[original-byte and lifecycle audit](R04_CODE2_SOURCE_HEAP_RELOCATION_RECLAMATION_20261012.md).

**R04 remains unaccepted; 3/12 = 25.00%.** The next major obstacle is
continuous original CODE.2 startup using the live source-backed heap
instead of its historical fixture-initialized arena, followed by
complete DOS/4GW original-behavior verification.


## Latest R04 audit — cross-thread reconciliation (2026-10-12)

The 16 commits after the prior main-thread checkpoint `2ad11d1`
were verified to modify **only `Recomp/`** and advance the
**canonical memory-runtime R04 gate**, not NativeEngine or DemoV1.

[Windows #96](https://github.com/Hyperbog/RLOFT/actions/runs/38252199410)
passed **46/46 CTests** at `898898dae8992978df728043d560c44eb4213601`.
The expanded source-backed CODE.2 heap witness allocates real host DPMI
pages through original `37B42/39D4D/39C7B/39BBC` from an empty arena
chain, then verifies split/free/reuse and out-of-memory behavior.
The original CODE.1 DPMI `0006` selector and ownership-header tests
also pass. A separate audit regression checks shared CODE.2 guest-byte
`0503` relocation/rollback and stale-handle release.
See [full cross-thread verification](R04_CROSS_THREAD_COMMIT_AUDIT_20261012.md).

**R04 is still not accepted; recompilation is 3/12 = 25.00%.**
Source-backed CODE.2 `39A8A` relocation and `39A2C` automatic
reclamation, complete continuous startup, and authentic DOS/4GW parity
remain open. The earlier dated build notes below are historical.


## R04 focused checkpoint — original CODE.1 host-backed heap execution (2026-10-12)

[Windows source build #81](https://github.com/Hyperbog/RLOFT/actions/runs/38245588733)
passed **43/43 CTests** at SHA
`875617f4b6a47dcbb48bc209eeb0abfa2d2b4075`.

The compiled original CODE.1 `10AA9/109C1/1069B` now establishes
a heap using real stateful DPMI `0501` host allocations, original `107CB`
reclaims the DPMI-backed or DOS `AH=48h` fallback arena, and original
`1083F` executes a DPMI `0503` relocation with retired handles.
Separately, original `CCDE` performs its Phar Lap selector/data-break
`AH=4Ah` request in **4 KiB units** through an actual host-backed resized
DGROUP. The conventional DOS `AH=48/49/4A` paragraph allocator has separate
bounded tests. These are real source-bound memory transfers, **not**
a complete DOS/4GW runtime. CODE.1's `DB5B` release adapter remains
test-owned; CODE.2 original `39C7B/39D4D/39E1C` growth is not yet
connected and startup still uses a seeded arena. A real DPMI `0006`
selector descriptor/base service and actual DOS behavioral checks
remain mandatory.

**R04 remains unaccepted; recompilation is 3/12 = 25.00%.**
See [R04 memory-runtime closure checkpoint](R04_MEMORY_RUNTIME_CLOSURE_CHECKPOINT_20261012.md).
**Continue R04** before beginning R05-R12.


## Current R04 closure work — DPMI linear memory (2026-10-12)

A host-backed stateful DPMI `0501h/0502h/0503h` linear allocation/resize/free
provider and an independent lifecycle CTest have been added, alongside the
existing source-backed `0100h/0101h` conventional memory provider.
The CMake suite now registers **38 CTests**; the newly registered test
still requires independent Windows CI verification. **R04 remains unaccepted**
pending real original CODE.1/CODE.2 heap establishment and growth, DOS memory
services and removal of the seeded test arena. The coarse accepted score stays
**3/12 = 25.0%**. See
[the exact R04 acceptance checkpoint](DPMI_LINEAR_MEMORY_0501_0503_CHECKPOINT_20261012.md).


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

## 2026-10-12 verified stateful host DOS/DPMI service progression

[Windows canonical source build #60](https://github.com/Hyperbog/RLOFT/actions/runs/38242155253)
passed **37/37 CTests** at source SHA
`4f13ebda062f0db124acf52f8795ac368ab21696`.

The DOS read-only file provider now implements `AH=42h` signed
seek/position and independent guest cursors, including seek-after-EOF.
Original compiled CODE.2 `37E77/38106/380F8` now has an opt-in
**bounded guest-path/multiple-descriptor host bridge**. A separate
original-source postlude reads an additional host-backed OBJECTS.DAT
using two concurrent guest handles; its returned bytes are checked
against an independent file snapshot. The postlude is NOT a natural
gameplay resource access.

New stateful DPMI DOS conventional-memory `INT31h AX=0100/0101`
allocator/release and checked guest selector translation replace
test-only allocation/free receipts in opt-in original compiled
CODE.2 `30D80/30D52/30DE8` paths. Synthetic DOS PSP, INT31h 0300
mouse simulation, INT33 mouse outcomes, interrupt vectors and game
resource initialization remain bounded test fixtures. **R04 and R05
are still unaccepted**; 3/12 whole-game gates = **25.00%**.

[Full stateful runtime checkpoint](STATEFUL_DOS_FILE_DPMI_BRIDGE_CHECKPOINT_20261012.md).

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
