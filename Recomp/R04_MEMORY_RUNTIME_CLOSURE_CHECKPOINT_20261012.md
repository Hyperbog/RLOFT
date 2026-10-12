# R04 focused acceptance checkpoint — memory and original heap services (2026-10-12)

Canonical owner: `code1-decompilation/Recomp/`. This is **not** `demo-v1`
or `native-engine`. Original reconstructed `Code1/Code1.cpp` and
`Code2/Code2.cpp` were not modified.

## Subsequent independently verified original CODE.2 closure

Windows [#98](https://github.com/Hyperbog/RLOFT/actions/runs/38254878225)
passed **46/46**, including original CODE.2 `39A8A` host-DPMI
`0503` relocation and `39A2C` source-driven `0502` reclamation,
with no seeded arena in the test. Original byte disassembly at
`39A3C..39A45` exposed a corrected arena-size comparison in the
reconstructed `Code2/Code2.cpp`: failing Windows #97 now passes.
See [the exact byte-level correction and source test](R04_CODE2_SOURCE_HEAP_RELOCATION_RECLAMATION_20261012.md).
**The standalone lifecycle gate has passed; continuous startup and
full DOS/4GW memory acceptance have not.**

## Decision

**R04 remains NOT ACCEPTED.** The fixed whole-game score is
**3/12 = 25.00%**. A passing source-backed heap witness is not proof that
the original DOS/4GW runtime supports full startup, genuine selector/heap
semantics, or interactive gameplay.

## Newly executed original CODE.1 paths

`src/code1_original_heap_growth_gate.cpp` invokes the **compiled original
reconstructed source**, using real host-owned protected-mode DPMI blocks:

- `10AA9 → 109C1 → 1069B`: starts from *zero* seeded heap arenas,
  allocates via stateful `INT31 0501`, stores the actual opaque handle
  and guest linear address in the original arena ownership header,
  and initializes the original arena/free-block structure.
- `109C1` excessive-size request: original source returns failure,
  with no guest heap head published and no host allocation committed.
- `107CB`: original reclaim decision and DPMI `0502` release callback
  return the owned arena to the provider; the handle is invalid afterward.
- `1083F`: with an explicitly labeled *terminal allocated-block test
  precondition*, original source calls stateful DPMI `0503` to relocate
  an arena, retires the old handle/linear mapping and rebuilds the
  relocated arena's metadata. The provider enforces guest-page bounds.

**Limitation:** `DB5B` has not been integrated as the original free-list
adapter. The tests model only its released-bit contract; the forced
terminal allocated-bit precondition is not naturally reached gameplay.
This is NOT complete original CODE.1 heap free/coalesce/growth parity.

`src/code1_original_dos_fallback_gate.cpp` covers a separate actual
CODE.1 source path when DPMI `0501` fails or is absent:

- original `10AA9 → 109C1` calls DOS `AH=48h` for a probe and
  requested block, frees the probe with `AH=49h`, and builds the
  DOS-owned heap arena;
- original `107CB` reclaims that arena via DOS `AH=49h`;
- the new `src/dos_paragraph_memory.hpp` supplies bounded host-owned
  guest paragraphs and MCB spacing; independent tests cover
  `AH=48h/49h/4Ah`, in-place growth, shrink, reuse, invalid blocks,
  OOM and guest-address boundaries.

**Limitation:** the source boundary named `segment_base_0006` still
uses an explicitly limited physical `segment × 16` alias when called
from this DOS fallback. A true protected-mode selector descriptor/base
provider for DPMI `0006` remains REQUIRED before R04 acceptance.

## Source-required extender-specific break semantics

The actual CODE.1 `CC1C → CCDE` protected-mode startup path performs
`INT21h AH=4Ah` with `ES=DS=30h` and **BX in 4 KiB units**, after
rounding and shifting the high-water mark by 12. This is *not* the
standard DOS `AH=4Ah` paragraph contract.

- `src/extender_data_break.hpp` implements a separately checked,
  host-owned guest-DGROUP resize that preserves source bytes and rejects
  wrong selectors, truncation of the original 0x9E90-byte static image,
  over-budget requests and invalid state.
- `src/code1_startup_gate.cpp --extender-host-resize` executes the real
  compiled `CCDE` to the `AH=4Ah` callback, completes the source
  call using a real resized host vector, and verifies its published
  0xA000-byte break and subsequent register state.
- This is a bounded **Phar Lap branch**, not implementation or proof
  of the game's DOS/4GW extender's complete DGROUP program break.

## Latest verified CI

- [Windows #75](https://github.com/Hyperbog/RLOFT/actions/runs/38245071874):
  **41/41 CTests passed**, including original CODE.1 DPMI
  allocate/grow/resize/free, DOS-memory fallback, and DOS paragraph
  provider.
- [Windows #81](https://github.com/Hyperbog/RLOFT/actions/runs/38245588733):
  **43/43 CTests passed** at source SHA
  `875617f4b6a47dcbb48bc209eeb0abfa2d2b4075`, including the
  original CODE.1 `CCDE` source-backed 4 KiB host resize and separate
  extender memory contract. No commercial media is bundled.
  **Passing tests do not satisfy the remaining R04 conditions.**

Build commands (asset-free Windows CI):

```powershell
cmake -S Recomp -B build/recomp -A x64
cmake --build build/recomp --config Debug --parallel 2
ctest --test-dir build/recomp -C Debug --output-on-failure
```

The owner-supplied originals were authenticated separately in this
project; `CODE.1` SHA-256:
`fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed`;
`CODE.2` SHA-256:
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
None of the original commercial media is included in Actions.

## Unmet acceptance conditions (do not flip R04 yet)

1. Integrate live original **CODE.2 `39C7B/39D4D/39BBC/39A8A/39A2C`**
   memory lifecycles into **continuous game-entry and initializer source**
   without the bounded synthetic `0x2B800/0x2C000..0x2D500` arena.
   The isolated source `39A8A` DPMI 0503 and `39A2C` DPMI 0502
   lifecycles are now verified in Windows #98; DOS fallback / alternate
   `39E1C` heap growth and original DOS parity still need work.
2. Implement selector base/limit/descriptor state for source-required
   **DPMI `0006h`**, complete DOS/4GW and extender-specific memory
   ownership and 48/49/4A callback ABI, and unify memory mappings.
3. Replace the remaining CODE.1 free-list release adapters and
   CODE.2 startup allocator fixtures with authentic original-source
   creation, split, coalesce, grow, shrink, invalidation and failure
   rollback end-to-end.
4. Integrate those services with both original compiled startup paths
   and authenticated LE media without relying on test-only allocation
   success receipts.
5. Obtain independent evidence of original DOS memory-service behavior
   before asserting parity. Until then explicitly label host policies
   and all synthetic branches as *adapters*, not DOS observations.

**Next checkpoint must continue R04, not begin R05-R12.**
