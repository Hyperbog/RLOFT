# R04 — continuous original CODE.2 startup with unseeded host DPMI heap

Canonical owner: `code1-decompilation/Recomp/`, not `native-engine`
or `demo-v1`. Source: compiled reconstructed `Code2/Code2.cpp`.

## Verified checkpoint

[Windows build #106](https://github.com/Hyperbog/RLOFT/actions/runs/38257869955)
passed **50/50 CTests** at source
`d09522858ebcc3b8ba659251f37973642252f116`.
The prior known-green baseline was 46/46 CTests (Windows #98).

**Four new source-backed integration tests** are registered in
`Recomp/CMakeLists.txt`:

| CTest option | Original-source execution boundary |
| --- | --- |
| `--through-initializers-host-dpmi` | `353A0 → 35418 → 395E2`, all four source initializers |
| `--enter-game-host-dpmi` | `39589 → 20608`, original CODE.2 game entry and audio prelude |
| `--to-sound-config-no-args-host-dpmi-heap` | argc=1 original audio allocation and the `AH=3Dh` file-open boundary |
| `--to-mouse-reset-host-dpmi-heap` | original resource/file wrappers and the `INT 33h` mouse-reset boundary |

These opt-in modes instantiate an initially empty arena-chain head at
`DGROUP+0x12624`, with **no hard-coded `0x2B800/0x2C000–0x2D500` heap**.
The historic preseeded fixture modes remain intact and are independently
checked for regression.

## Exact host-backed original-source path

1. The original compiled `353A0 → 35418` receives a bounded DOS version
   response and a test-supplied positive `INT 21h AX=FF00h` extender
   detection. It then performs original `INT 31h AX=0006h` selector-base
   lookup. A host-owned `DpmiSelectorDescriptors` supplies the live,
   checked `DS=0030h` descriptor with base `00050000h`.
   **The version, PSP, environment, and positive extender detection
   remain explicit controlled fixtures**, not original DOS observations.
2. The bounded host runtime invokes original reconstructed
   `3A395(value=1)` to enable the Watcom heap flag. This is a
   **host-initiated original-source call**, not proof that the natural DOS
   program has reached that setter before its first allocation.
3. The reconstructed original `395E2` scheduler invokes all four
   initialization callbacks, including `3A2AD` environment copying,
   `3A1B3` stream initialization, and `3968E` argument setup.
4. These allocations flow through original
   `37B42 → 39D4D → 39C7B → DPMI AX=0501h`, with real
   statefully tracked linear pages and opaque DPMI handles. Source
   `39BBC` creates arena headers; `39838` allocates/splits; source
   `398E0` links frees/coalesces. The caller-owned guest aperture is
   extended to `0x01100000` bytes without rewriting the original
   `0x2DD10` bytes of static DGROUP.
5. The original `39589 → 20608` application and audio paths run on
   the *same* guest byte image. Pointer tables, command-line arguments,
   stream records and optional argc=1 sound buffers are validated
   against **live DPMI-owned pages**, not historical fixture addresses.

The hardware-facing DOS file, interrupt-vector, mouse, PSP and audio
behaviors are still bounded service fixtures or explicit stop boundaries.
Only heap allocations/free-list changes and the reconstructed source calls
listed here have been linked to live memory services in this integration.

## Regression history

- Windows #100: 46/48; original startup remained in the plain-DOS
  branch and new source allocations could not reach DPMI provider.
- Windows #101: 46/48; positive extender and DPMI `0006h` handshake
  executed, but old assertions still assumed the fixture's fixed arena.
- [Windows #102](https://github.com/Hyperbog/RLOFT/actions/runs/38257231258):
  **48/48**, after live source-arena free-list metadata validation.
- Windows #105: 49/50; original argc=1 sound buffer worked on new
  guest pages, while its old regression expected the fixture address.
- [Windows #106](https://github.com/Hyperbog/RLOFT/actions/runs/38257869955):
  **50/50**, with live guest-page checks for the optional sound buffer.

The source `39A8A → 0503` resize and `39A2C → 0502` automatic
release were already verified separately at
[Windows #98](https://github.com/Hyperbog/RLOFT/actions/runs/38254878225);
see [prior source lifecycle audit](R04_CODE2_SOURCE_HEAP_RELOCATION_RECLAMATION_20261012.md).

## Acceptance assessment and follow-up

**R04 remains UNACCEPTED; overall recompilation: 3/12 = 25.00%.**
The new opt-in path demonstrates meaningful continuous original-source
execution without preseeded allocator nodes, but the gate's complete
DOS/4GW memory/heap/selector lifecycle is not yet demonstrated.

Unmet items:

1. Complete original CODE.1 free-list release and coalescence by wiring
   actual `DB5B/10590` beyond the current released-bit test adapter.
2. Integrate/test CODE.2 DOS fallback and `39E1C` break-extension
   branch under live services, including failure, growth, and rollback.
3. Unify DPMI selector, extent, and conventional DOS-memory ownership
   across both CODE.1 and CODE.2 startup states.
4. Compare memory-service behavior with authenticated original DOS
   observations using owner-controlled media, without adding commercial
   game assets to public CI.
5. Preserve clear distinctions among source-executed code, host-owned
   memory policies, synthetic DOS/hardware observations, and gameplay
   (still **not booted**).

**Next work remains R04. Do not promote R05 or count test registrations
as whole-game acceptance.**
