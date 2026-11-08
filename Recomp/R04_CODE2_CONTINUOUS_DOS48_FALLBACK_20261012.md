# R04: original CODE.2 continuous DOS 48h fallback to game entry, audio, and mouse

**Branch:** `code1-decompilation`. **Milestone:** R04 —
DOS/4GW memory and DPMI selector lifecycle (**not accepted**).
**Verified:** 2026-10-12.

## Checkpoint source and passing CI

[Windows canonical build #122](https://github.com/Hyperbog/RLOFT/actions/runs/38287503152)
compiled the original complete reconstructed CODE.1/CODE.2 and passed
**55/55 CTests** at source
`2dacfa6caaa3d18856612d1d961f9091cf7f1046`.
Its predecessor [#120](https://github.com/Hyperbog/RLOFT/actions/runs/38287259752)
passed **53/53**, including both continuous initializer/game-entry
DOS-fallback cases. Earlier queued runs #118/#119/#121 were cancelled
by newer pushes; they are not counted as validation.

Four **additional asset-free CODE.2 source integration tests**:

| Test | Original source boundary |
| --- | --- |
| `canonical_code2_startup_initializers_unseeded_dos48_fallback` | Original `353A0 → 35418 → 395E2` four original startup initializers |
| `canonical_code2_startup_game_entry_unseeded_dos48_fallback` | Original `39589 → 20608` application/game-entry source |
| `canonical_code2_startup_no_args_sound_unseeded_dos48_fallback` | Original argc=1 `2F2EC` audio buffer allocation and `SOUND.CFG` file-open boundary |
| `canonical_code2_startup_mouse_reset_unseeded_dos48_fallback` | Original `37E77/37E99/38106/380F8` file wrappers then the original mouse reset `INT 33h` stop |

All previously verified **DPMI 0501** unseeded-heap startup tests remain
passing as separate controls. No fixture heap was seeded in the four
new modes. The original source owns the arena head `[12624h]`, arena
linkage and `39BBC/39838/398E0` split, free and coalescing state.

## Source-backed DOS fallback behavior

In the new, **explicitly simulated** extender environment, original
`35418` observes `INT 21h AX=FF00h` positive and queries a tracked
DGROUP descriptor via source `INT 31h AX=0006h`. The host then
deliberately rejects a real `0501h` DPMI allocation request.
Consequently original compiled `37B42 → 39D4D → 39C7B` itself
selects the **DOS conventional-memory fallback**, asking for a probe
allocation and a full-sized allocation via DOS `AH=48h`, releasing
the probe via `AH=49h`. The host does **not** fabricate an arena
receipt; it creates two tracked paragraph allocations, releases one
and translates the remaining selector into a bounded guest aperture.

The host `DosParagraphMemory` maps DOS physical segment `×16`
into a separate guest address band rooted at `0x300000`,
preventing it from overwriting CODE.2's original static LE object-2
DGROUP. Both the provider and canonical compiled reconstructed source
see the **same guest bytes**. Every heap pointer validation checks
live ownership through `DosParagraphMemory.translate`, including
original environment strings, stream nodes, argument arrays and
the optional sound buffer.

The test confirms the original call paths, the denied DPMI receipt,
**two** DOS allocation requests, **one** probe release, exactly **one**
remaining live DOS selector, **zero** live DPMI 0501 allocations,
and a source-initialized circular free list with 8 or 9 allocations
depending on the audio path. A stop-boundary check uses the preserved
`Session` explicitly rather than dereferencing a cleared callback
global.

The alternate original `39E1C` DS-break-growth branch and
`39A2C` complete-heap DOS `49h` reclamation have already been
covered in isolated source-backed regression tests at
[Windows #113](https://github.com/Hyperbog/RLOFT/actions/runs/38260777313).
This checkpoint **does not claim** those alternate lifetimes execute
in the *same* continuous startup path.

## Independently verified CODE.1 free-list closure

The separately added original CODE.1 `DB5B → 10590` source tests
were confirmed by [Windows build #117](https://github.com/Hyperbog/RLOFT/actions/runs/38271837071)
(**51/51**). Both original `10AA9/109C1` DPMI and DOS fallback
now exercise source `104E0` block split, `DB5B/10590` return,
free-list coalescence, and original `107CB` reclamation.
The previous test-owned free-bit adapter is removed. See
[`R04_CODE1_DB5B_SOURCE_FREE_LIST_20261012.md`](R04_CODE1_DB5B_SOURCE_FREE_LIST_20261012.md).
The CODE.1 first-runtime setup still has bounded test-owned original
DGROUP root state; this does not establish complete original
CODE.1 character creation or original DOS loader behavior.

## Gate decision — R04 still open

**3/12 acceptance gates = 25.00%. No accepted gate changed.**

The live host-memory services are consistent enough for the listed
reconstructed source cases. What remains:

1. Reproduce the alternative original CODE.2 `39E1C`
   extender/DS-break branch from its **actual startup trigger**
   and continue into game entry; isolated `39E1C` tests are not
   evidence of a source-selected original startup transition.
2. Integrate shared DOS/DPMI selector issuance, base/limit lookup,
   relocation and release ownership consistently across CODE.1
   and CODE.2 full runtime boundaries, not just isolated tests.
3. Complete CODE.1 startup/handoff memory initialization without
   test-owned DGROUP root setup and separate media/loader fixtures.
4. Independently observe original DOS/4GW memory and selector
   behavior on owner-original media and compare with host traces.
   Successful CTests and static original-byte disassembly are not
   actual DOS observation.

The original MZ/LE CODE.1 and CODE.2 media remain unchanged;
no retail game bytes were committed to public CI. Continue R04,
not R05–R12, until memory runtime acceptance is justified.
