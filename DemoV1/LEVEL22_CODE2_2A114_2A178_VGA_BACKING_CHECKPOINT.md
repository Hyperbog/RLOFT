# CODE.2 2A114 / 2A178 — Original VGA Overlay Backing Bytes

**Checkpoint:** 2026-10-08 · **Branch:** `demo-v1`  
**Status:** New original-source-backed native plane transfer and binding, verified
in isolated C++17 builds. **Not yet a new full Windows or reference-DOS framebuffer pass.**

## Original executable and byte evidence

User-owned `CODE.2` SHA-256:
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`;
physical code-object base `0x3AF24`. Addresses here are **logical
object-1 offsets**, not bytes relative to the DOS linear VGA aperture.

Original `2A114..2A176` and `2A178..2A1E4` were independently
disassembled. **53 original instruction signatures** have been added to
`tools/verify_combat_entry_bytes.py`, raising the cumulative ledger to
**908 named checks at 900 distinct code addresses**. These are source
provenance checks, **not** executable behavior/whole-game percentage coverage.

### Exact five-parameter contract

Both routines receive
`(original_DGROUP_buffer_pointer, row, column, rows, columns)`.

They call `2AB38` using *live* `DWORD[EFED]`,
`DWORD[EFF1]`, and `DWORD[F001]` to derive the VGA
byte address and its initial plane from the starting row/column.
Then they compute

```text
physical_bytes_per_row =
    ((uint32_t(columns + (column & 3)) + 3) >> 2)
source_or_destination_row_pitch = DWORD[EFF9]
DGROUP_bytes = 4 * rows * physical_bytes_per_row
```

Arithmetic wraps through original 32-bit x86 ADD, and the
column plane offset comes from **2AB38**, not from a modern
screen rectangle normalizer. The original loops over `rows`
with `DEC/JNE` without guarding a zero initial row count.
A bounded host capture with zero rows therefore reports
**unsupported**; it is not silently normalized into a
successful zero-row DOS operation.

### 2A114: capture four VGA planes into original DGROUP bytes

Original `2A14E` loads `AX=0004h` (graphics-controller
index 04h, read-map plane 0). `2A159` executes
`OUT DX,AX` on `03CEh`; after each plane block
`2A169` increments `AH`, so the four reads use
**planes 0, 1, 2, 3**. Each plane iterates the same VGA
rectangle with original forward `REP MOVSB` and
`EFF9 - physical_bytes_per_row` source row delta.

The **original backing buffer is contiguous**:

```text
backing + 0*rows*bytes_per_row : plane 0 rows, no padding
backing + 1*rows*bytes_per_row : plane 1 rows, no padding
backing + 2*rows*bytes_per_row : plane 2 rows, no padding
backing + 3*rows*bytes_per_row : plane 3 rows, no padding
```

The VGA graphics-controller read-map index 04h remains
selected at **plane 3** when `2A114` returns. The native
`VgaPlanarMemory` now explicitly captures this register,
initially unknown until a source-backed `OUT`.

### 2A178: restore four original DGROUP plane blocks to VGA

Original `2A1B4` loads `AX=0102h` (sequencer index
02h, map-mask plane 0). Each source backing plane block
is copied in forward order into the corresponding VGA plane
through `REP MOVSB`; `SHL AH,1` yields masks
**1, 2, 4, 8**. The VGA destination row delta
remains `EFF9 - physical_bytes_per_row`.

After the four blocks, `2A1DA..2A1DC` sends the exact
port write `OUT 03C4h, AX=FF02h`. The raw sequencer
AH **0xFF** and effective **low-four-bit plane mask 0x0F**
are distinct observable facts. `VgaPlanarMemory`
now retains both instead of discarding the full port-write
byte or interpreting reserved upper bits as extra planes.

### Zero logical columns do not imply zero hardware effects

Previously reconstructed `292BC` capture skips its
underlying call when the clipped column count is zero,
but `2932C` restore may still invoke `2A178`
at **zero logical columns**.

- Starting column **3**: the original rounded physical
  width `(0+3+3)>>2 = 1` transfers **one VGA byte per row
  on each of four planes**.
- Starting column **0**: the rounded physical width is
  zero. The original `REP MOVSB` writes no pixels, but
  the four sequencer-mask port writes and final `FF02h`
  still occur.

These cases have explicit pixel- and port-state regressions.
The host additionally rejects out-of-aperture VGA addressing
or an incomplete backing byte capture **before** beginning
a partial copy; DOS itself did not necessarily reject such
malformed inputs safely.

## Native components and integration

Added:

- `include/rloft/combat_original_vga_backing.hpp`
- `src/combat_original_vga_backing.cpp`
- `tests/combat_original_vga_backing_tests.cpp`
- `include/rloft/combat_original_vga_backing_binding.hpp`
- `src/combat_original_vga_backing_binding.cpp`
- `tests/combat_original_vga_backing_binding_tests.cpp`

Also extended original `VgaPlanarMemory` with the
graphics-controller read-map selector and the raw sequencer
mask output byte, leaving the existing effective four-plane
mask semantics intact.

`CombatOriginalVgaBackingBinder` connects these native
pixel-copy routines **under the existing source-verified
`292BC/2932C` clipping** without discarding the real
`1A288/1AB20` overlay lifecycle, `2E590/2E5D4`
page selection, or `1A8C8` compositor logic. The
original `EF3F/EF41` viewport limits and `F001`
page base remain caller-owned **live state**, not
hardcoded Level 22 constants.

## Testing provenance and acceptance boundary

All **53** new code signatures correspond to instructions
in the supplied immutable CODE.2.

The **exact committed** `combat_original_vga_backing.cpp`
Git blob SHA-1 is
`45bdcc29517e8df9165631d80ed0eaaf9f075271`.
That source was compiled against a small interface-compatible
VGA/DGROUP harness and passed focused strict C++17
`-Wall -Wextra -Werror -pedantic` tests in Debug,
optimized Release (`-DNDEBUG` with assertions explicitly
enabled), and UndefinedBehaviorSanitizer modes. The tests
cover physical plane-row ordering, four-plane roundtrip,
read-map selector 3, sequencer mask 0x0F versus raw 0xFF,
zero-column cases, zero-height rejection and truncated
backing captures. This is **not** a complete compilation
of the repository's real VGA presenter or the two new
checked-in CTest source files.

The new source and both checked-in tests are registered
in DemoV1 CMake and the assertion-enabled test target
list. The last complete Windows run
[#634](https://github.com/Hyperbog/RLOFT/actions/runs/37788168281)
passed 130/130 at historical SHA
`40e3f14b1072a7a0180ea1b0b4c1cea437137631`.
Those old results **do not cover this new code**. No
private Actions build was triggered.

## Remaining integration work

The original VGA plane-byte capture and restoration
are source-backed at the component level, but real runtime
integration still needs exact `1A3B8` overlay coordinate
updates, `2AE30` hit-test and `29804` conditional
graphics using original game resources. The active native
VGA/DGROUP state must be wired to the real gameplay
renderer, then compared against reference DOS frames,
register effects and IRQ traces, including the Level 22
demo transition.

**Formal demo release acceptance: 1/5 major gates.**
Engineering completion remains **unmeasured**; these are
substantial reusable core graphics increments, not
end-to-end DOS equivalence certification.
