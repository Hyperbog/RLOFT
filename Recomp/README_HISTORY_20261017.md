> **Enhanced PC port scope update (owner decision, 2026-10-13):** the native game's deliverable is now a **playable enhanced Windows PC port**. Canonical CODE.1/CODE.2 recompilation and its original-DOS memory/selector/timing oracle remain preserved as **optional research and compatibility tooling**, **not** prerequisites for the port's Alpha, Beta or Final release. The R01–R12 research milestones and historical `3/12` progress below still describe *this independent canonical-recompilation workstream*, **not** the enhanced native game's readiness. Prioritize reusable content, assets, script/rule insights and decompiled algorithms that help the [native-port charter](https://github.com/Hyperbog/RLOFT/blob/demo-v1/DemoV1/ENHANCED_PC_PORT_CHARTER.md). Do not expend port-blocking effort on exact DOS/4GW startup or allocator parity when an equivalent safe native subsystem already works. Keep all original reconstruction evidence available for future research.

# Canonical CODE.1 / CODE.2 whole-source compilation and linking gate

## R04 current — source DAC9 allocator, pinned original-media identity (2026-10-12)

[Windows build #153](https://github.com/Hyperbog/RLOFT/actions/runs/38325531978)
passes **63/63 existing canonical CTests**. Original CODE.1 FCE8
stdio/argv constructors now acquire their guest heap allocations through
the *actual compiled original* `DAC9→DAD9→104E0/10AA9/10C46`
allocator veneer, including source-owned retry and error handling,
rather than the earlier test-owned allocator retry loop. The core
guest DS-break service and in-memory free list remain original
reconstructed C++; the host only handles explicitly external guest
memory/OS services. [Source checkpoint](R04_CODE1_CD97_CONSTRUCTOR_HEAP_CHECKPOINT_20261012.md).

The CODE.1 and CODE.2 startup programs now **reject same-sized,
modified originals** using portable SHA-256 before LE parsing. Owner-
supplied CODE.1/CODE.2 identities were checked against their pinned
original hashes locally, and existing CTests validate the SHA-256
implementation against standard public vectors without bundling
original media. [Original-DOS memory capture protocol](R04_ORIGINAL_DOS_MEMORY_ORACLE_CAPTURE_PROTOCOL.md)
is now documented; **no original DOS/4GW trace has yet been captured**.

**R04 remains OPEN. Whole-game recompilation acceptance is still
R01–R03 = 3/12 (25.00%).** The major blocker is independent original
DOS/4GW memory/selector/error-path behavioral evidence. CODE.1 gameplay
beyond `508C` is separately scoped to R06. No new CTest targets were
added in this checkpoint.


## Latest R04 CODE.1 source startup checkpoint — 63/63 Windows CTests (2026-10-12)

[Windows source build #147](https://github.com/Hyperbog/RLOFT/actions/runs/38323318475)
passed **63/63 CTests** with the temporary diagnostics removed
(code SHA `78d421ef71c2410efed3ea48a21363a9fa4d9eb7`).

A corrected `CC1C→CCDE` Phar Lap DOS-version fixture supplies
`BL='2'`; **original CODE.1 `CD97`** then publishes its own
mode-2 byte. The real source `FCE8` ordered constructor table now
executes original `10E81` five-stream initialization,
`10438` stack-selector capture, and `FD6E` argv setup with the
original `FE21` scanner and **source-owned 10AA9/10DD7/104E0
guest heap**. Original `FC98→10D1C` sets the heap minimum and
stops explicitly at the unimplemented `508C` gameplay call.
The guest heap starts with an **empty allocator arena**, using
only the four audited original static sentinel/root values.

The regression first revealed an actual CODE.1 C++ reconstruction
error at `10BB4: CMP ESI,EAX`: original x86 compares the **desired
break-end address** against the previous break, not the requested
increment. The byte-backed correction uses `desired<=old` and
`desired-old`; 62/62 passed at Windows #138. A later constructor
test exposed an **asset-free fixture error**—its initial circular-list
sentinel words were missing. The original LE DGROUP was authenticated,
the pinned values were restored in fixture setup, and 63/63 passed at
Windows #146 and clean #147. The public CTest has a 12-second
timeout to fail closed on renewed free-list loops.

[Full original x86, CODE.1 static data and source-constructor
audit](R04_CODE1_CD97_CONSTRUCTOR_HEAP_CHECKPOINT_20261012.md).

**R04 remains NOT ACCEPTED. Whole-game progress remains
R01–R03 = 3/12 (25.00%).** The host still stops at the original
`508C` application entry. It has not authenticated original
DOS/4GW runtime selector and memory behavior by independent emulator
traces. CODE.1 allocator veneer caller chains also remain incomplete.
These tests do not boot the game.


## Current R04 checkpoint — 60/60 source-backed DOS/DPMI memory tests (2026-10-12)

[Windows canonical #133](https://github.com/Hyperbog/RLOFT/actions/runs/38320942043)
passed **60/60 CTests** at original-source change
`9a1ab2a383127a8223879b5952bc6b71f993c390`.
CODE.2 original Phar Lap `35418` mode-2 startup is now carried
through four initializers, game entry, sound initialization and the
mouse boundary using **original source `39E1C` guest DS-break
heap growth**, without seeded arena headers. The original x86 branch,
PSP:005Ch break and AH=4Ah (4 KiB units) were checked directly against
the authenticated owner-original `CODE.2` bytes.

Original CODE.1 and CODE.2 host DPMI `0006/0100/0101` selector
providers now share a loader-owned descriptor registry with independently
tracked live bases, limits and releases; CODE.2 `39C7B` DOS fallback
uses the same table with an explicitly **test-defined** segment-selector
alias. A new CODE.1 test reuses the same DGROUP from original `CCDE`
extender resize through original `10AA9/104E0/DB5B/10590/107CB`
DPMI heap allocation, split, coalescing and cleanup; authenticated
original CODE.1 heap-root data is verified rather than overwritten.
This **post-startup CODE.1 heap call is still host-triggered**.

[Complete source and original-byte evidence, plus remaining R04
blockers](R04_MEMORY_RUNTIME_PROGRESS_AUDIT_20261012.md).

**R04 IS NOT ACCEPTED. Recompilation remains 3/12 = 25.00%.**
Natural CODE.1 heap-mode startup and independently observed original
DOS/4GW selector/memory behavior are still required. These completed
source CTests do not substitute for DOS observation or game boot.
Older checkpoints below are retained for historical provenance.


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


---

# Consolidated historical notes (2026-10-26 housekeeping)

These sections are preserved verbatim as historical research. Dates, artifact retention notices, milestones and acceptance assertions below describe **past revisions**, not current `Recomp` HEAD. For authoritative current instructions use [README.md](README.md) and [DOCS_INDEX.md](DOCS_INDEX.md).

## Former `Recomp/README.md` (historical)

# Ravenloft CODE.1/CODE.2 — recompilation workstream

**Owning branch:** `recomp`. Original-source recompilation, DOS/4GW compatibility, native link/startup experiments and independent original-image tests. **Not** the canonical enhanced Windows game: that is [`demo-v1`](https://github.com/Hyperbog/RLOFT/tree/demo-v1/DemoV1).

## Build and test (Windows x64 / MSVC)

```powershell
cmake -S Recomp -B build/recomp -A x64
cmake --build build/recomp --config Debug --parallel 2
ctest --test-dir build/recomp -C Debug --output-on-failure
```

**Linux GCC/Clang:** run `bash dev/linux.sh test` from the repository root; [Linux-first workstream setup](../dev/README.md) covers dependencies. [Verified Linux C++ run #39082582548](https://github.com/Hyperbog/RLOFT/actions/runs/39082582548) passed **63/63 CTests** on its recorded revision.

**Windows milestone builds:** [schedule, manual selection, artifact downloads and limits](WINDOWS_MILESTONE_BUILDS.md). [First R04 probe #39082555268](https://github.com/Hyperbog/RLOFT/actions/runs/39082555268) compiled and passed **63/63 MSVC CTests** while R04 remains unaccepted. Artifacts are engineering tests, not playable-game releases.

The owning [.github/workflows/recomp-source-link-gate.yml](../.github/workflows/recomp-source-link-gate.yml) is scoped to `recomp` and packages **engineering executables, not a playable game**. The reconstructed `Code1/` and `Code2/` directories remain at their established paths because these are compile inputs. [Workstream/branch guide](BRANCH_GUIDE.md) documents ownership.

## Acceptance boundary

Historical research reached R01–R03 of 12 planned gates, with R04 DOS/DPMI memory/startup investigation incomplete; the older milestone count is **not a current measure of enhanced-game completeness**. The last historical README claimed Windows source run #38325531978 with 63/63 CTests at its stated revision. Retest current HEAD before updating any verification claim. Neither a successful C++ linker nor bounded source entry tests proves original DOS boot, complete original-machine equivalence or player gameplay.

## Evidence and historical notes

The previous extensive milestone log remains in [README_HISTORY_20261017.md](README_HISTORY_20261017.md), in its original directory to preserve relative links to source-address audits. Original-file identity checks, DOS memory selectors and initializers must remain independently attributable to CODE.1/CODE.2, with no proprietary originals in build artifacts.

[Current GitHub branch/CI observation](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/docs/LIVE_STATUS.md) is automated, but does not establish recompilation semantic acceptance.

## Graphics recompilation — original CODE.2 VGA

[Graphics source map, binary SHA witnesses, and remaining integration boundaries](../Code2/graphics/ORIGINAL_VGA_SOURCE_MAP.md) document the original 0x29953 mode setup, 0x2AB38 planar pixel address, 0x29CA0 DAC6 stream and 0x2ABF4 CRTC page start. The separate [portable host component](src/original_vga_host.hpp) and [CTest](src/test_original_vga_host.cpp) implement and test four 64 KiB video planes, palette bytes, latches, GC read and write modes 0–3, bit-mask/rotate/AND/OR/XOR and synthetic indexed scanout. The original CODE.2 image was independently authenticated for the six cited instruction ranges, and the proof helper is shared exactly with `code1-decompilation`.

**Important:** the source graphics calls are not yet hooked to the host adapter, original VGA timing/CRTC modes are incomplete and no real game frame has been produced by recompilation. This is a graphics *hardware-layer regression milestone*, not a playable graphics release.

## Original Ravenloft RES image rendering (verified)

[Actual RES0/RES1 type-1 graphics source integration](../Code2/graphics/REAL_RES_TYPE1_SOURCE_RENDER.md) now decodes owner-owned RES bytes via a bounded read-only adapter and calls **original reconstructed CODE.2 `2A20C` image packing, `2F29C` cached item lookup, `2975C` resource drawing and `2A380` VGA output**. Authentic `RES0:15` and `RES1:336` rendered on a Windows CI runner with original `COLORS` bank 0, with original source hashes checked, and 68/68 CTests passed. [Windows milestone #39088036213](https://github.com/Hyperbog/RLOFT/actions/runs/39088036213) provides the current engineering executable. Original resources and their decoded frames are never bundled in CI downloads. Full DOS resource file opens, composed 3D/gameplay rendering, and visual parity remain open.

## Unloaded original CODE.2 resource-loader milestone

[Original RES loader source and evidence](../Code2/graphics/REAL_RES_TYPE1_SOURCE_RENDER.md): the original reconstructed `2975C` drawing request now invokes **`2F29C` → `2ECB4` → `2EB60` → `2FD9C`** through read-only archive seek/read/position callbacks. Unlike the previous diagnostic, no already-loaded guest image is installed; CODE.2 populates its own image cache, and repeated lookups do not re-read the archive. The raw type-1 raster loader follows the original decoded byte-stream path; compressed type-1 method 3 invokes the corrected canonical CODE.2 `37700` via original `2FB04`/`2FA30`. [Linux #39089790975](https://github.com/Hyperbog/RLOFT/actions/runs/39089790975), [Windows #39089791043](https://github.com/Hyperbog/RLOFT/actions/runs/39089791043) passed **68/68 CTests**. On preserved authentic RES0:15 and RES1:336, indexed SHA outputs matched the preloaded path. This does **not** certify DOS startup, complete world rendering or acceptance gate R04.

## CODE.2 37700, source resource initialization and RES4 milestone

[Original source and authentic RES4 report](../Code2/graphics/RES4_FIRST_PERSON_SOURCE.md): recovered instruction-derived `37700` live guest `2B6C2` termination is used in the compiled CODE.2 compressed-data pipeline with original `2F9E4` byte reads and `2FA30` output. The source `2E7F0` archive initializer now allocates unloaded guest records through read-only DOS-open callbacks (isolated graphics-subsystem startup only, **not** game entry-point boot). RES4 type-6 resources now use source `2ECB4` dispatch and **compiled `3395C` + `338C8`** to render 128-texel texture columns with shade subtraction into indexed VGA frames. Type-6 uses compiled original `37590`, and source `335DC`/`3263C` now execute on synthetic wall-sweep records with real RES4 texture bytes.

[Linux GCC #39091993852](https://github.com/Hyperbog/RLOFT/actions/runs/39091993852) and [Windows/MSVC #39092086421](https://github.com/Hyperbog/RLOFT/actions/runs/39092086421) passed **70/70 CTests**. Windows additionally authenticated real RES4 entries 0, 100 and 500 (each 8,192 visible source-index pixels) and preserved previous RES0/RES1 frame hashes; no original art was shipped. Test output is **a flat first-person texture-column diagnostic**, not a real level view, 3D camera, full interface or playable game. R04 remains open.

Usage on owner-supplied originals:

```powershell
.\rloft_original_code2_graphics_frame.exe --res4 "C:\Games\RLOFT\RES4" 100 128 0 "C:\Temp\RES4-preview" "C:\Games\RLOFT\COLORS" 0 0
```

Creates owner-local `.idx` and `.bmp` graphics; do not redistribute extracted original media.

## Canonical compressed resources, INIT/INFO and 11-archive source startup

[Windows/MSVC milestone #39096508171](https://github.com/Hyperbog/RLOFT/actions/runs/39096508171) **passed 71/71 CTests** and verified authentic source `INIT` header, `INFO.DAT` path flags, and all 11 `RES0..RES10` archived item tables (installed + CD roots). Corrected canonical `37700`, compiled `37590`, and original `335DC` → `3263C` → `338C8` compositor produce identical indexed frames to the direct sampler for RES4:0/100/500/600. Source `334C4`/`32954` camera and level-driven occlusion are **not** connected; neither is continuous CODE.1-to-CODE.2 game boot. R04 remains **unaccepted**. [Technical report](../Code2/graphics/CANONICAL_STARTUP_AND_WALL_SWEEP_20261018.md).

## Real LEVELS.DAT into original CODE.2 world view preparation

The [source-level 334C4 grid integration](../Code2/graphics/ORIGINAL_LEVEL_VIEW_PREP_20261018.md) now transfers all 32 authentic LEVELS.DAT headers and 115,584 original 16-bit packed cells into their **original CODE.2 guest locations**, runs **compiled `334C4`** visibility reset, and stops at `32954` with an explicit unexecuted camera/raycast boundary. The media-free synthetic CTest also supplies marked cells and verifies the reset preserves every flag except `0x0040`. [Windows 72/72 milestone #39097776940](https://github.com/Hyperbog/RLOFT/actions/runs/39097776940) additionally validates original-level SHA and all 32 source-initialization results. This is not CODE.1 game boot, ray-driven wall generation, world scene parity or R04 acceptance.

## Authentic SINTAB-driven original CODE.2 Level 22 rays

[Original 03838 SINTAB + 32954 world raycast checkpoint](ORIGINAL_32954_SINTAB_RAYCAST_20261018.md): a compiled original `181B4` loader now feeds authentic Level 22 cells to source `334C4`, with original `03838` loading the preserved SINTAB and authentic `20608` *default startup camera values* replayed (tile 4,4; 270°). Compiled `32954`, original fixed-point helpers and wall/depth record generators executed **256 rays**, materialized **256 wall/depth records** and marked **231 cells** on [successful Windows/MSVC #39100156051](https://github.com/Hyperbog/RLOFT/actions/runs/39100156051) (**72/72 CTests plus real-media validation**). The `20608` entire game entry and original-to-recomp independent DOS parity are **not** proved; the generated records are not yet connected to the genuine RES4 frame compositor, so this is not a rendered playable world or R04 acceptance.

## Original SAVE10 Level 5 deserialization and outdoor renderer checkpoint

[Source-executed original `07938` save load and Level 5 outdoor dispatch](ORIGINAL_07938_SAVE5_OUTDOOR_SOURCE_20261018.md): original CODE.2 initialized DGROUP + `SAVE10/SAVE.DAT` enter compiled `07938/06CAD`, which invokes compiled `181B4` for the standalone mutable `LEV005.DAT`. The same guest world now drives authentic `334C4/32954` rays and compiled `335DC/3263C/338C8` wall composition; see [passing source-world Linux CI](https://github.com/Hyperbog/RLOFT/actions/runs/39135916368). Original compiled `1F1C8` additionally selects **two authentic RES4 textured outdoor backgrounds via `34A74` callbacks**, verified by [dedicated CI](https://github.com/Hyperbog/RLOFT/actions/runs/39136699731). Those callbacks authenticate which textures and branches are used but **do not yet render the sky/ground textured pixels**. `34A74/342E4/34954`, RES3 scenery/trees, UI layers, original DOS pixel comparison, and continuous CODE.1→CODE.2 boot are still open.


## Current source-scene checkpoint (supersedes older historical graphics notes)

[Part 21 original SAVE5 quads, RES3 shape and independent DOS oracle handoff](ORIGINAL_SAVE5_RES3_SCENE_AND_DOS_CAPTURE_20261018.md) records the verified current source execution. Original CODE.2 source now deserializes the saved Level 5 state, renders source sky/ground (RES4:634/635), 256-ray world walls, one source-visible world quad through compiled 34DE8 → 34BB0 → 33E6C, and 12 original RES0 HUD blits. The original NPCS/OBJECTS/INIT descriptor banks were initialized in the same graphics session. Source NPC type 61 resolved authenticated RES3 shape 1829; original 2F29C/37590 decompressed its 4,219-byte EH7 body and passed cache reuse. A compiled 2C294/2B6D0 scene projection gate is connected but yields **zero visible sprite draw requests** at the actual saved Level 5 camera, so **36CEC sprite pixel rendering is still unimplemented**. This is neither the complete outdoors game nor a reference DOS capture. Latest source verification [saved Level 5 #39164949396](https://github.com/Hyperbog/RLOFT/actions/runs/39164949396), [Level 11 world regression #39164949379](https://github.com/Hyperbog/RLOFT/actions/runs/39164949379), [Windows #39164949386](https://github.com/Hyperbog/RLOFT/actions/runs/39164949386), all passed. No original-media pixel files were published.

## Part 24 — original DOSBox-X 2026.10.01 DAC provenance hook (2026-10-20)

The [instrumented DOSBox-X capture kit](tools/dosboxx_dac_capture/README.md) pins
upstream VGA source to Git blob `509d42fbead67a56594e49cec55549bc6fcc6d7c`
and adds opt-in, post-lockout/mask capture at the actual `0x3C8/0x3C9`
VGA byte write handlers, preserving the individual bytes from `REP OUTSB`.
Its JSONL converter rejects unverified upstream hooks, overflow,
truncated DAC histories and original-save palette divergence.

The [dedicated CI](https://github.com/Hyperbog/RLOFT/actions/workflows/recomp-dosboxx-dac-hook.yml)
passed exact release-source patching, compiled standalone C++11 hook
tests, and compiled an isolated harness using the **actual patched
upstream VGA handler bodies**. This is **not** yet a completed Windows
DOSBox-X executable, an original Ravenloft runtime capture or an accepted
native graphics parity check. Keep original-game SAV, port captures,
palette bytes and indexed frames private. The retail eight-entry
`COLORS` bank-3 update caller remains unverified.

## Part 26 — original COLORS bank selection explains DOS DAC entries 246–253

[Authenticated COLORS→CODE.2 scene selector chain](ORIGINAL_DOS_DAC_RUNTIME_BANK3_20261020.md)
now identifies the source of the eight-color change: **original COLORS bank 0
exactly equals gameplay DAC submission 28**, while **COLORS bank 3 equals
submissions 29/30 and independently saved original VGA DAC**. These banks
differ only in 24 RGB6 channels at indices 246–253; no skeleton RES image
patch or guessed palette interpolation is needed to explain the transition.
Source object-1 18C8D → 1F8E0 updates palette selector C034 using the original
Level-5 scene descriptor at 81AC; authenticated source loader 325B0 reads
the selected COLORS bank into C054; wrapper 29234 → 29CA0 emits the DAC
bytes. The owner's SAV independently contains selector C034=3 and an exact
COLORS bank-3 palette at C054. A SHA-pinned
[palette-selection audit](tools/audit_original_colors_scene_palette.py)
passed [original-media Linux CI](https://github.com/Hyperbog/RLOFT/actions/runs/39402847517).
**Source bank identity is verified; exact dynamic producer call ancestry,
emulator-save frame phase and DOS/native indexed pixel parity are not.**

## Part 27 — Native source scene palette now uses full COLORS bank (2026-10-20)

[Native palette integration](SOURCE_SCENE_PALETTE_NATIVE_INTEGRATION_20261020.md)
replaces the prior 246–253 bank-3 splice with **complete 768-byte bank
selection** from original CODE.2 level scene descriptors at object-2
0x81AC + 3*level. For the authentic Level-5 save the source table
selects bank 3. The native palette loader stages the selected complete
original COLORS bank at 0xC054 and runs compiled original CODE.2 32150
brightness and 29CA0 DAC output. There is no copied eight-color overlay
or special case for skeleton sprites.

[Authentic saved-world Linux gate](https://github.com/Hyperbog/RLOFT/actions/runs/39419812617)
**passed** scene-selector and malformed-input unit tests, unchanged RES3
skeleton/RES0 HUD/world compilation, and **full 768-byte** native output
palette equality against owner-original COLORS bank 3. The --world
LEVELS-only diagnostic remains an explicitly named bank0 fallback because
it has no original CODE.2 scene table. Full original boot sequence,
retail dynamic setter call ancestry, animation-phase synchronization and
DOS/native indexed pixel parity remain NOT VERIFIED. This is a native
saved-world renderer integration, not a completed playable game.

## Part 28 — host-ordered original CODE.1 → CODE.2 source boot and COLORS loader

[Implementation and acceptance boundary](ORIGINAL_STAGED_BOOT_AND_PALETTE_20261020.md):
new non-playable `rloft_code1_code2_staged_boot` target executes the
reconstructed CODE.1 CC1C/CD97/FCE8/FC98 constructor/startup checkpoint
(with its original 508C **host stop**) and then, under explicit host
scheduling, enters a CODE.2 session executing original 16D04 INIT,
2E768/2E7F0 INFO/11 resource initialization, authenticated source
scene palette selection, source-derived 325B0 read-only COLORS bank
open/seek/read, compiled original 32150 brightness processing and
compiled original 29CA0 VGA DAC submission (769 real adapter-port calls).
**This is a staged host progression, not a reconstructed DOS guest transfer.**
The same `325B0` source-derived file callbacks also replace host-side
palette copying in the native saved-world renderer. No original media or
save state is needed to initialize the CODE.2 resource/palette session
once owner-original INIT/INFO/RES/CODE.2/COLORS files are provided.

[Passing owner-original Windows regression](https://github.com/Hyperbog/RLOFT/actions/runs/39509429048)
verified the assembled startup chain, its source media identity, full
scene bank-3 VGA submission, and existing source startup/rendering tests.
The Windows engineering milestone ZIP includes the stage runner; it
is **not a playable executable** and takes an extracted read-only CD
resource directory for RES9 and RES10 (not a mounted `game.gog` image).
The unresolved CODE.1 game-entry overlay switch, CODE.2 runtime loop,
original CPU call ancestry and DOS/native frame parity remain open.

## Part 30 - persistent R.EXE native child protocol and CODE.2 source guest (2026-10-20)

[Implementation and boundaries](ORIGINAL_SUPERVISOR_PERSISTENT_CODE2_RUNTIME_20261020.md):
a new resumable R.EXE supervisor lifecycle retains child objects across
uncompleted Pending dispatches, routes only real CODE.2 status 10 to
CODE.1, verifies the exact 0x920 CHARS handoff on CODE.1 completion,
and routes CODE.2 status 12 to CHARS cleanup/restart. Missing child
execution or CHARS producer fails closed rather than inventing success.
The portable child transitions are exercised with openly labeled
synthetic outcomes; they are **not** the live original game loop.

The original CODE.2 runtime-resource session now exposes a persistent
guest object across host dispatches. Original INIT, INFO.DAT and eleven
RES archive descriptors, original CODE.2 scene selection, original
COLORS-bank loading, compiled 32150 brightness and compiled 29CA0 VGA
hardware output share the same retained guest. The source-staged
R.EXE-first executable now checks two Pending steps retain the exact
guest memory address, a full memory fingerprint, the RES cache base,
11 archive descriptors, selected palette bank and 769 port writes,
without re-initializing or fabricating a CODE.2 child process exit.

[Owner-original Windows source/regression check](https://github.com/Hyperbog/RLOFT/actions/runs/39515268398)
**passed** the new pending-guest marker and all 79 Windows CTests.

The remaining **hard stop** is wiring the canonical CODE.2 353A0/20608
game entry and 1FC14..20293 gameplay loop, true input/interrupt host
services and frame presentation into this **same** guest memory/runtime.
The previous preview build is not a game boot and should not be
delivered as a playable milestone. The next user-facing game build
requires actual original startup and at least one live event-driven
gameplay frame.


## Part 31 - canonical CODE.2 main and shared original heap/RES/VGA (2026-10-20)

[Original CODE.2 single-owner source integration](CANONICAL_CODE2_SINGLE_GUEST_TO_MOUSE_20261020.md)
replaces the separate canonical startup/resource guests. Source CODE.2
353A0 startup and 20608 main retain the same RAM, original Watcom
37B42/37C37 allocator and DPMI heap, 11 RES archive descriptors,
COLORS selected bank and VGA palette. This source execution continues
past original 375FE keyboard vector installation and owner SOUND.CFG
read. It then stops at original DOS INT33 mouse reset, a remaining
native input service boundary.

[Original-media Windows regression](https://github.com/Hyperbog/RLOFT/actions/runs/39520936708)
passed integrated original startup, source resource loading, palette
and heap-release changes. The bounded CPU call stack was unwound on
host stop and cannot be resumed by the supervisor Pending dispatcher;
no original gameplay tick, real window, or playable build is claimed.

Next: connect a real native window/mouse INT33 host service and
continue the original CODE.2 20608 path toward 1FC14..20293.

## Part 31 - original CODE.2 stackful service-by-service execution

[Original retail CODE.2 execution checkpoint](ORIGINAL_CODE2_STACKFUL_MOUSE_CONTINUATION_20261020.md):
the canonical 353A0 -> 20608 startup now has a stack-preserving native
worker that suspends at actual DOS INT33 mouse reset. The persistent
INIT/RES/palette session borrows its authentic machine/heap/RAM while
the source CPU stack remains suspended. A test-only explicit mouse
response lets actual compiled CODE.2 execute its tracked DPMI
INT31 AX=0100 DOS-memory allocation and reach the NEXT original DOS
service, INT31 AX=0300 / 33AA0 real-mode mouse simulation. That second
service waits for a real host register-frame implementation, without
inventing a return value or restarting original CODE.2 startup.

[Passed Windows original-media stackful / DPMI regression](https://github.com/Hyperbog/RLOFT/actions/runs/39523067077).
The virtual-mouse response is test-only; live Win32 input, the DPMI
0300 register exchange, further DOS services, original gameplay loop
1FC14..20293 and frame presentation are NOT yet connected. This
checkpoint is not a playable game.



---

## Former `Recomp/WINDOWS_MILESTONE_BUILDS.md` (historical)

# Recompilation Windows milestone builds

**Owning branch:** `recomp`. The regular [source-link Windows workflow](../.github/workflows/recomp-source-link-gate.yml) runs as sources change. The independent [milestone Windows workflow](../.github/workflows/recomp-milestone-windows.yml) offers longer-lived, downloadable MSVC x64 test builds.

## Frequency and manual runs

- **Automatic:** the first and fifteenth of each month at **13:37 UTC**, scheduled by the copy of the workflow on the repository's default branch. The scheduled run checks out the then-current `recomp` branch.
- **On milestone acceptance-file changes:** pushing `Recomp/ACCEPTANCE_GATES.json` on `recomp` triggers a run using that exact branch commit.
- **On demand:** use [Actions → RLOFT Recomp Windows milestone validation](https://github.com/Hyperbog/RLOFT/actions/workflows/recomp-milestone-windows.yml), click **Run workflow**, then select `next` or one of `R01`–`R12`. For `next`, the report selects the first not-yet-accepted gate.

Manual selection is a **test focus label**, not acceptance. No build automatically changes `ACCEPTANCE_GATES.json`.

## Contents and evidence

On successful MSVC Debug build and CTest, Actions produces **RLOFT-Recomp-Windows-Milestone-<run>** with a ZIP containing `rloft_recomp_link_gate.exe`, CODE.1 and CODE.2 startup gate executables, original-image verifier, SHA256SUMS, exact source commit and R01–R12 evidence-labeled report. The independent `RLOFT-Recomp-Windows-Evidence-<run>` contains JUnit CTest results, progress manifest and failure diagnostics (when available). Both artifact types are retained **60 days**. No original proprietary binaries or playable Windows game are bundled.

**Compiled original CODE.2 indexed graphics and palette milestone — verified:** [Windows MSVC milestone #39086139502](https://github.com/Hyperbog/RLOFT/actions/runs/39086139502), [Linux GCC #39086139521](https://github.com/Hyperbog/RLOFT/actions/runs/39086139521), and [Windows source-link #39086139589](https://github.com/Hyperbog/RLOFT/actions/runs/39086139589) each passed **67/67 CTests**, including compiled CODE.2 planar image rendering, DAC banks, 200/400 modes, page/split timing adapters, original 32150 brightness and palettized 320×400 BMP validation. Download [Windows compiled graphics engineering ZIP](https://github.com/Hyperbog/RLOFT/actions/runs/39086139502/artifacts/12120312840) or [MSVC/JUnit evidence](https://github.com/Hyperbog/RLOFT/actions/runs/39086139502/artifacts/12120417572). The artifact includes `rloft_original_code2_graphics_frame.exe` and synthetic-source `code2-compiled-frame.idx`/`.bmp`. **This is not real-game artwork or DOS gameplay parity; R04 remains unaccepted.** [Source execution map](../Code2/graphics/COMPILED_GRAPHICS_PATH.md).

**Graphics VGA milestone — verified:** [Windows MSVC milestone #39084747886](https://github.com/Hyperbog/RLOFT/actions/runs/39084747886) passed **65/65 CTests**, including the CODE.2 planar address/DAC/CRTC/latch/write-modes regression and binary-span verifier self-test. The artifact now contains `rloft_original_vga_host_tests.exe` alongside the original-source startup diagnostics. Download [Windows graphics engineering ZIP](https://github.com/Hyperbog/RLOFT/actions/runs/39084747886/artifacts/12119994330) and [JUnit/milestone evidence](https://github.com/Hyperbog/RLOFT/actions/runs/39084747886/artifacts/12119804960). **R04 remains unaccepted**. This build does not launch the real CODE.2 renderer or show game frames.

**First verified milestone run:** [#39082555268](https://github.com/Hyperbog/RLOFT/actions/runs/39082555268): **MSVC x64 63/63 CTests passed**, artifact [milestone engineering ZIP](https://github.com/Hyperbog/RLOFT/actions/runs/39082555268/artifacts/12119900798), [test evidence](https://github.com/Hyperbog/RLOFT/actions/runs/39082555268/artifacts/12119940619). The selected probe was **R04**, which **remains unaccepted** because independent original DOS memory/selector behavior is still unproven. That run was not a playable game build.

**Linux cross-check:** [Ubuntu/CMake/Ninja C++ run #39082582548](https://github.com/Hyperbog/RLOFT/actions/runs/39082582548) passed **63/63 CTests** on its exact source commit. Run `bash dev/linux.sh test` locally from the `recomp` branch. Windows WPF/Win32 gameplay acceptance belongs to the distinct `demo-v1` branch.

## Future milestone policy

R01–R03 have separately recorded historical research acceptance. R04–R12 remain open until their evidence is certified in the source manifest. Passing the same 63 tests at a future milestone does **not** establish a new gate; genuinely deeper source behavior, host compatibility and independent original-machine comparisons must be added and tested. Do not distribute an engineering executable as a playable game or overwrite original retail saves.

## Genuine original RES graphics smoke test

[Windows milestone #39088036213](https://github.com/Hyperbog/RLOFT/actions/runs/39088036213) passed **68/68 CTests** plus an additional read-only source test on preserved original `RES0:15` and `RES1:336` (real palette `COLORS`, validated original SHA-256). It captured 90 and 2,992 nonzero indexed pixels respectively and logged only output hashes; no original art/captures are in artifacts. [Windows engineering ZIP](https://github.com/Hyperbog/RLOFT/actions/runs/39088036213/artifacts/12121950557) and [JUnit/evidence](https://github.com/Hyperbog/RLOFT/actions/runs/39088036213/artifacts/12121069884) expire December 17, 2026.

To inspect your own legally held original graphic, run the packaged diagnostic:

```powershell
.\rloft_original_code2_graphics_frame.exe --res "C:\Games\RLOFT\RES1" 336 1 0 "C:\Temp\portrait" "C:\Games\RLOFT\COLORS" 0 0
```

This produces owner-local `portrait.idx` and `portrait.bmp` from authentic source art. The ZIP contains only synthetic-frame examples. Original CODE.2 2FD9C unloaded resource/DOS IO is **not** wired, and R04 is unaccepted; [details](../Code2/graphics/REAL_RES_TYPE1_SOURCE_RENDER.md).

## Unloaded original CODE.2 resource-loader milestone

**Latest validated Windows package (obsolete preloading removed):** [Milestone #39090358644](https://github.com/Hyperbog/RLOFT/actions/runs/39090358644) passed **68/68 CTests** and original RES0:15/RES1:336 read-only image validation. Both indexed SHA-256 outputs were unchanged after deleting the legacy cache-prepopulation function. [Engineering ZIP](https://github.com/Hyperbog/RLOFT/actions/runs/39090358644/artifacts/12122332063) and [test evidence](https://github.com/Hyperbog/RLOFT/actions/runs/39090358644/artifacts/12122287353) expire December 17, 2026. **R04 remains unaccepted.** No proprietary image, RES archive or palette bytes are distributed.



[Windows build #39089791043](https://github.com/Hyperbog/RLOFT/actions/runs/39089791043) **passed 68/68 tests** and exercised real owner-preserved RES0/RES1 through original `2ECB4`/ `2FD9C` file-backed loading and original image-cache population. The authenticated frame hashes remain unchanged. [Windows engineering ZIP](https://github.com/Hyperbog/RLOFT/actions/runs/39089791043/artifacts/12121853934) and [test evidence](https://github.com/Hyperbog/RLOFT/actions/runs/39089791043/artifacts/12121759678), retained until December 17, 2026. Only the synthetic preview frame is packaged; original raster and palette bytes stay on the runner. That earlier milestone used a bounded host callback for `37700`; later builds replaced it with the canonical guest-stop-controlled source function. R04 is **not accepted**.

## CODE.2 compressed source and authentic RES4 column milestone

[Windows/MSVC #39092086421](https://github.com/Hyperbog/RLOFT/actions/runs/39092086421) passed **70/70 tests** and source-authenticated original `RES0:15`, `RES1:336`, `RES4:0`, `RES4:100`, `RES4:500` and `COLORS` SHA checks. The RES4 column samples are 64×128 original type-6 textures, with no visual scene/parity claim. The reconstructed `37700` stop control is guest-flag driven, `2E7F0` source resource initialization is exercised, and `338C8` original compiled column scaling runs into indexed VGA. The complete game startup remains separate; compiled original `37590` is now executed by later graphics builds.

- [Windows engineering ZIP](https://github.com/Hyperbog/RLOFT/actions/runs/39092086421/artifacts/12123107884) — source test tools, synthetic previews and SHA checksums
- [CTest and milestone evidence](https://github.com/Hyperbog/RLOFT/actions/runs/39092086421/artifacts/12122889028) — Windows diagnostics, no retail graphics

Both expire December 17, 2026. Source-loaded original RES captures were generated only in temporary CI directories, not uploaded. **R04 remains unaccepted**. Technical evidence: [RES4 and 37700 source audit](../Code2/graphics/RES4_FIRST_PERSON_SOURCE.md).

### Final RES4 32/64-column validation

[Windows milestone #39092562187](https://github.com/Hyperbog/RLOFT/actions/runs/39092562187) passed **70/70 CTests** and authenticated four shipped RES4 type-6 records: 0, 100, 500 (64×128) and 600 (32×128). Entry 600 generated 4,096 nonzero indexed pixels and a stable output SHA-256. The original 37700 recovered stop-control, source 2E7F0 initialization and the actual compiled 338C8 column scaler remained in the tested build. No original-image bytes or original texture BMPs were published. R04 remains **unaccepted**.

[Download engineering ZIP](https://github.com/Hyperbog/RLOFT/actions/runs/39092562187/artifacts/12123004192) · [Download test evidence](https://github.com/Hyperbog/RLOFT/actions/runs/39092562187/artifacts/12123262884). Both expire December 17, 2026.

## Original 37700, 37590, INIT/INFO and RES4 compositor — latest verified Windows milestone

[MSVC #39096508171](https://github.com/Hyperbog/RLOFT/actions/runs/39096508171) passed **71/71 CTests**, authentic INIT/INFO and all eleven archive source startup checks, and original RES4:0/100/500/600 sweep/compositor indexed parity with the direct sampler. Original RES0/RES1 image checks passed. This is an engineering test build using synthetic world records, **not** a playable game, full CODE.1 boot, camera-projected world scene, or R04 acceptance. Original media is not included. [Windows x64 engineering ZIP](https://github.com/Hyperbog/RLOFT/actions/runs/39096508171/artifacts/12123819892) · [CTest/evidence](https://github.com/Hyperbog/RLOFT/actions/runs/39096508171/artifacts/12125225228); both expire December 17, 2026.

## Original LEVELS.DAT → CODE.2 334C4 view-preparation milestone

[Windows/MSVC milestone #39097776940](https://github.com/Hyperbog/RLOFT/actions/runs/39097776940) passed **72/72 CTests**, plus read-only SHA-256 validation of authentic `LEVELS.DAT` and execution of **original CODE.2 `334C4`** over all 32 level headers and 115,584 packed cell words. It verifies source clears only current-view `0x0040` bit and stops at the original `32954` raycast callback; the original camera/rays/visibility/3D scene remain unexecuted.

[Download Windows engineering ZIP](https://github.com/Hyperbog/RLOFT/actions/runs/39097776940/artifacts/12125820072) · [CTest/JUnit evidence](https://github.com/Hyperbog/RLOFT/actions/runs/39097776940/artifacts/12124973924). Both expire December 17, 2026. The ZIP includes `rloft_original_code2_level_view.exe` and **no copyrighted original level media**. [Source evidence and boundary](../Code2/graphics/ORIGINAL_LEVEL_VIEW_PREP_20261018.md). **R04 remains unaccepted.**


---

## Former `Recomp/BRANCH_GUIDE.md` (historical)

# Recompilation branch — working guide

**Owner:** `recomp` branch. **Canonical source:** `Recomp/`. **Baseline:** `85327ed7f25eccbb6184f28360913402d1f5cb3d` on `code1-decompilation` before the 2026-10-17 split.

This branch deliberately retains `Code1/`, `Code2/` and common original-source research because `Recomp/CMakeLists.txt` compiles the existing reconstructed C++ directly. Do not relocate or duplicate those inputs without updating includes, build scripts, tests, and source-location contracts.

## Build and test

```powershell
cmake -S Recomp -B build/recomp -A x64
cmake --build build/recomp --config Debug --parallel 2
ctest --test-dir build/recomp -C Debug --output-on-failure
```

The Windows workflow `.github/workflows/recomp-source-link-gate.yml` runs for this branch and publishes **engineering test executables, not a playable game**. No original proprietary media belongs in Git commits or CI artifacts.

## Workstream boundaries

- **Recompilation changes:** `Recomp/` and explicitly required source/ABI integration in `Code1/`, `Code2/` on this branch.
- **Decompilation changes:** `code1-decompilation`, with entry point `Decomp/README.md`, and reconstructive sources `Code1/`, `Code2/`.
- **Player game:** `demo-v1`; do not wholesale merge this branch into the game.
- **Original files:** immutable `DnD-Classics/MASTER/`; local originals are user-supplied and excluded from source control.

For cross-branch code sharing, port bounded, verified changes with exact source attribution and rerun each workstream's tests. A successful recompilation unit test does not establish native gameplay acceptance or complete original DOS execution.


---

## Former `Recomp/LEVEL22_INTERACTIVE_PLAYTEST.md` (historical)

# Original CODE.2 Level 22 — first interactive Windows playtest

The native original-source player host already exists at `Recomp/src/original_code2_level22_window.cpp`, is compiled by the Windows CMake build, and is packaged by the Windows milestone workflow. This uses reconstructed original CODE.2 (not the enhanced demo engine).

## Launch

1. Download the package from the [latest successful Windows milestone](https://github.com/Hyperbog/RLOFT/actions/workflows/recomp-milestone-windows.yml) under **Artifacts**. Extract **the complete package**.
2. Double-click `Play-RLOFT-Level22.cmd`. The launcher finds standard GOG installation directories or asks you to choose the original game `RLOFT` folder containing `CODE.2`.
3. Supply your own unmodified GOG installation, including `game.gog` in the parent directory. A custom extracted CD resource folder can be passed using `-CdRoot`.

Controls: **W/S** move forward/back, **A/D** strafe, **J/L** or **left/right arrow keys** turn, **Esc** quit. The native host shows the live original 320×400 indexed-VGA scanout and reports source position/heading/movement counters.

Example for custom paths or non-graphical smoke check:

```powershell
powershell -ExecutionPolicy Bypass -File .\Launch-RLOFT-Level22.ps1 -GameFolder 'C:\Games\RLOFT'
powershell -ExecutionPolicy Bypass -File .\Launch-RLOFT-Level22.ps1 -GameFolder 'C:\Games\RLOFT' -CdRoot 'C:\Games\CD_Extracted\RLOFT' -Smoke
```

The original game files and saves are read-only. Original CD data extracted to a private temp directory are cleaned up on exit; none is published in the compiled package.

## Acceptance limits

The preserved-media Windows source tests verify the original keyboard loop, 16 keyboard polls, 9 actual source camera displacements, and 7 heading rotations. The source native-window smoke exercises seven inputs without opening the GUI, but **a human GUI playtest has not yet been certified**. Some game events intentionally stop when source callbacks are not connected; independent retail DOS indexed framebuffer comparison, physical DOS IRQ behavior, audio, full NPC interactions and cross-level gameplay remain open. This is a first Level22 experimental exploration build, **not a complete game**.

