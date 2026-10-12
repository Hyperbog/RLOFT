# R04: original CODE.1 startup mode and constructor-owned heap

**Date:** 2026-10-12. **Branch:** `code1-decompilation`.
**Whole-game milestone:** R04 — DOS/4GW memory, heap growth and DPMI
selector lifecycle. **Not accepted.**

## Summary: source startup before the first game entry

The prior R04 witness `--extender-host-resize-to-source-heap` reused an
original CODE.1 extender-resized DGROUP but **wrote runtime mode 1
directly** and explicitly invoked `10AA9`. The underlying
Phar Lap `CCDE` startup instead derives the memory mode from the
DOS-version service's `BL` ASCII minor subtype. The legacy witness is
retained as a deliberately forced *mode-1* DPMI path, not recast as
natural startup.

The added source-backed modes execute the authentic reconstructed
sequence:

```
CC1C  original DOS version and initial DGROUP/stack establishment
CCDE  original Phar Lap 4-KiB-unit AH=4Ah DS-break resize
CD97  original PSP/environment extraction; publishes DGROUP 531Eh mode=2
FCE8  original ordered three-entry guest constructor dispatcher
  10E81  original five fixed stdio stream registrations
  10438  original stack selector capture
  FD6E   original empty-PSP-tail argv initialization via FE21 scanner
FC98  original 10D1C heap-minimum setter; original 508C game-entry boundary
```

The `10E81` constructor allocates **five** stream-list nodes and
`FD6E` allocates one argv backing block. For the opt-in
`--extender-host-resize-to-cd97-constructors` case, their host
service adapter invokes the **original C++ reconstructed
`104E0` first-fit and `10AA9→10DD7` growth** in the
same live guest image. The first constructor allocation causes
a real tracked 4-KiB DS-break extension of the previously
`0xA000` CODE.1 data object to `0xB000`; no DPMI `0501h`,
host malloc-backed guest arena or fabricated free-list nodes are used
as guest-allocation substitutes.

**Boundaries:** Original DOS AH=30h and Phar Lap AH=4Ah receipts are
controlled host services. PSP and environment are bounded, empty
test fixtures. Runtime call dispatch selects source routines, but
the host owns their callback wiring for checked guest linear
memory, and the DAc9/DAd9 allocation veneer is not yet a naturally
traversed source caller. The `FC98→508C` game-entry call is **still
a host stop boundary**, not running character creation. This is
meaningful original-source startup execution, not an original-DOS trace.

## Byte-grounded original CODE.1 comparison correction

The owner's original `CODE.1` SHA-256
`fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed`
was parsed as two original LE objects without repackaging game media.
Independent GNU `objdump` x86 inspection of source code-object
`10AA9..10C45` established:

```asm
10B84  03 35 F0 52 00 00     ADD ESI, DWORD PTR [52F0h]   ; desired end
10B97  56                    PUSH ESI
10B98  E8 3A 02 00 00        CALL 10DD7h
10BA9  89 C2                 MOV EDX, EAX                ; previous end
10BB4  39 C6                 CMP ESI, EAX                ; desired vs previous
10BB6  0F 86 ...             JBE 10ABAh
10BBC  29 C6                 SUB ESI, EAX                ; new usable extent
```

The previous reconstructed C++ compared
`size <= old` and subtracted `old` from the *increment*.
The byte-grounded correction tests `desired <= old` and computes
`desired-old`. The newly added `10AA9` natural mode-2 test first
failed on [Windows #137](https://github.com/Hyperbog/RLOFT/actions/runs/38321913158)
(61/62) and passed after the fix on
[Windows #138](https://github.com/Hyperbog/RLOFT/actions/runs/38322030744)
(**62/62**). The CODE.1 original binary itself was never modified.

## Original object-2 static runtime state

An independent authenticated LE object decode verified these original
runtime bytes (guest offsets, not arbitrary test-only choices):

| Offset | Source value | Role |
| --- | --- | --- |
| `55CCh` | `00000001h` | Initial heap-enable state |
| `55D0h` | `00000010h` | Before the source `10D1C` `8000h` default |
| `55E0h` | state 0 / priority `20h` / function `FD6E` | argv constructor |
| `55E6h` | state 0 / priority `20h` / function `10438` | stack-selector constructor |
| `55ECh` | state 0 / priority `20h` / function `10E81` | stdio constructor |
| `539C..5404h` | flags 1, 2, 2, 3, 2 | Five initialized fixed stream slots |
| `541Eh` | flag 0 | Stream-sentinel termination |
| `30C0h` | `000030D4h` | Initial circular heap sentinel |
| `30C8h` | `FFFFFFFFh` | Initial max/free-list state |
| `30D8h` / `30DCh` | `000030D4h` | Circular sentinel neighbor links |

The original `CD97` routine publishes `[531Eh]=2` using the
real source return registers. The test's DOS-version callback now
provides **`BL='2'` (`32h`)** alongside `AX=4458h`, rather
than retaining the unrelated `BL='R'` bytes from the entry probe.

For tests with an optional original CODE.1 file, the authentic static
heap roots/runtime record bytes are **verified but not overwritten**.
The asset-free CTest explicitly sets only these independently
authenticated bytes. No copyrighted original bytes are committed.

### Diagnostic regression and recovery

The first complete original `10E81` source constructor test hung
in original reconstructed `DB5B→10590` before the first source
allocation returned; a **12-second CTest timeout** was added so this
cannot stall a public Windows build. Instrumentation proved original
`10DD7` resized correctly to 11 pages (0xB000), and the entry to
`10590` looped with an **asset-free fixture whose four CODE.1
circular-list static roots were accidentally zero**. The
authenticated original data object already contains their correct
values. Initializing those pinned original values *before* source
`FCE8` constructor execution restored the original free-list
insertion behavior. It was a fixture error, not a reason to modify
original `10590` semantics.

[Windows build #146](https://github.com/Hyperbog/RLOFT/actions/runs/38323174797)
passed **63/63 CTests** with all three source constructor bodies
enabled, real guest heap ownership, correct priority dispatch,
and argv/stdio metadata checks. Temporary debug messages were
subsequently removed; the 12-second guard is retained.

## Remaining R04 acceptance blockers

The full whole-game recompilation score remains **3/12 = 25.00%**.
The new CODE.1 modes reduce prior limitations substantially but do
**not** establish any of:

1. Real original DOS/4GW selector privileges/limits, address aliases,
   DPMI alloc/resize/free receipts, or matching runtime error behavior,
   independently captured on the owner-original DOS game.
2. Naturally executed CODE.1 `DAD9/DAC9` allocation veneers and all
   allocator caller chains, as opposed to source core entrypoints
   exposed by the host service adapter.
3. Full original `508C` gameplay/character-creation entry after
   `FC98`, beyond the explicit bounded call.
4. All supported startup environment/PSP and command-tail variants;
   this source-backed regression uses a controlled empty-tail fixture.

Do not mark R04 accepted until the original DOS/4GW memory oracle
exists and the specified whole-runtime selector/heap contract is
verified. Continue R04 before R05; preservation of truthful milestone
status is required.
