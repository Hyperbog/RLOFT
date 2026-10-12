# R04: CODE.2 original-source DPMI resize and arena reclamation

Canonical branch: `code1-decompilation`. This is **original CODE.1/CODE.2 recompilation**, not `demo-v1` or `native-engine`.

## Original-byte fidelity correction

An exact instruction comparison against owner-provided
`CODE.2` (558,689 bytes; SHA-256
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`)
identified an incorrect C++ expression in the previously reconstructed
`CODE.2:39A2C` empty-arena reclamation predicate.

The original LE code object's raw x86 bytes at `39A3C..39A45` are:

```asm
39A3C  8B 00       mov eax, dword ptr [eax]   ; first free-block size
39A3E  8B 13       mov edx, dword ptr [ebx]   ; current arena size
39A40  83 C0 2C    add eax, 0x2C
39A43  39 D0       cmp eax, edx
39A45  75 3A       jne 39A81
```

The old reconstruction tested
`load32(free_block)+0x2C != node` (**pointer**).
The original bytes unequivocally test
`load32(free_block)+0x2C != load32(node)` (**arena size**).
The latter is now committed in
[`Code2/Code2.cpp`](../Code2/Code2.cpp). This change corrects the
reconstructed original semantics rather than altering the source game.

## New source-backed lifecycle test

Expanded `Recomp/src/code2_original_dpmi_heap_gate.cpp`, using the same
guest-byte mapping as the real compiled source. It creates a new,
independent source heap with **zero preseeded arenas** and executes:

1. Original `37B42 → 39D4D → 39C7B → 39BBC → 39838` allocates a
   4 KiB DPMI `0501h` arena and a real source-owned user block.
2. Original `39A8A` unlinks the sole allocated arena and calls the
   host DPMI `0503h` resize service, receiving a relocated 12 KiB
   allocation and a **new opaque handle**. Original `39BBC`
   reconstructs its free list. Original `398E0` releases the
   surplus split tail via the source callback.
3. The test verifies that original user bytes survive relocation in
   the same physical guest aperture that host DPMI exposes, and that
   the retired old address and handle cannot translate.
4. Original `37C37 → 398E0` frees and coalesces the remaining user
   block. Original `39A2C` observes its complete free block,
   removes the arena, and invokes host DPMI `0502h` to release
   the **new** handle. The test requires an empty arena chain and
   zero committed provider allocations.

Callbacks still supply the bounded guest/host dispatch of the DPMI
functions and locate the owning arena for source `398E0`. They do
**not** invent successful allocation/resize/free receipts, directly
seed free-list nodes, manually retire source-owned handles, or copy
guest-byte snapshots after allocation.

## Verification

- [Windows #97](https://github.com/Hyperbog/RLOFT/actions/runs/38254715407)
  compiled all source but **failed 1/46**: the new test reproducibly
  caught the original-source `39A2C` predicate mismatch.
- [Windows #98](https://github.com/Hyperbog/RLOFT/actions/runs/38254878225)
  passed **46/46** after the byte-grounded original-source correction.
  All existing tests remained passing.

## Acceptance decision

**R04 remains NOT ACCEPTED.** The completed tests cover original
`39A8A` DPMI `0503` relocation and original `39A2C` DPMI `0502`
automatic reclamation under bounded stateful host services.
They **do not** prove the DOS/4GW extender's full selector/heap
lifecycles, original DOS execution parity, CODE.1 free-list completeness
or continuous CODE.2 startup that does not preseed a synthetic arena.

Next: opt-in host-backed heap integration directly into
`Recomp/src/code2_startup_gate.cpp` (original `353A0 → 395E2/39589`
initializers through `20608` game entry), preserving existing
fixture-only tests as explicit differential controls. Independent
owner-original DOS memory traces remain required before claiming
emulation parity. Until R04's fixed gate scope is met,
**3/12 = 25.00%** is unchanged.
