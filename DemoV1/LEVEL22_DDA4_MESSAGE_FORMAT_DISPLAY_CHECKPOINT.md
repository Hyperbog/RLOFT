# CODE.2 DDA4 level-up message and IRQ display lifecycle

**Date:** 2026-10-08. **Branch:** `demo-v1`.  
**Status:** immutable original-byte source audit, bounded read-only C++ models
and CMake-registered regressions. No live DOS/Windows message state is
mutated, and no full Windows EXE / repository CTest has run.

## Exact DDA4 and 3210F string source

The supplied CODE.2 has SHA-256
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
The object-1 executable starts at physical file offset `0x3AF24`;
DGROUP object-2 starts at `0x75F24`.

At original logical `DDA4:DF54..DF75`, the game passes the
current actor's 0x248-stride character-record pointer as the
single string parameter to `3210F`, with:

- format source: **DWORD DGROUP +67E4 = 0x548**
- original format bytes at DGROUP +548: `%s gains a level.` followed by NUL
- destination: **DGROUP +759C**
- next original call: `1C21C(destination+759C)`.

The format-pointer DWORD SHA-256 is
`a6246ee40149d8701c64240f8fa77d429aac33cc9c1de8acac31e9a595e12cc1`.
The 18-byte format string SHA-256 is
`7a8e30053ba3aa8b830030d93008eb38f8f458da54c2cc275305bc15e5d658d5`.
Both original data spans are independently checked in the
versioned combat-byte verifier.

The original `3210F` wrapper passes the variadic arguments to
generic `385B0` with original byte-emitter callback `320F8`;
`3210F:32139` writes the final NUL at destination+emitted_count.
The full generic formatter supports more than this single message;
it is **not** claimed to be wholly reconstructed here.

`CombatLevelMessageModel::inspect` implements **this verified
single literal format only**, retaining the original character
name's raw DOS bytes and the trailing destination buffer bytes.
It rejects nonterminated character names within their original
16-byte record span, unsupported source text/pointer, out-of-bounds
actor indexes or insufficient captured output storage instead of
pretending DOS safely truncates memory writes.

The accompanying focused regression verifies actual `3210F`
output bytes, NUL and destination preservation, its next
`1C21C` copied message and IRQ deadline, and
high-byte non-UTF-8 DOS character bytes.

## Exact 1C258, 1C2C4, 1C2CC..1C326 lifecycle

The original `1C2C4` first invokes `1C258` **before** it
reads any IRQ time. If `BYTE[CF59]` is nonzero, `1C258`
performs these operations in strict order:

1. `291B0(BYTE[C050])`;
2. `2939C(0,0x106,0x20,DWORD[2222C])`;
3. `3060C(0x107,0x22,DGROUP:14580,1)`;
4. `1A8C8(DWORD[C050],0x106,0x20,0x0C,0x100)`;
5. **only after that compositor returns**, set `BYTE[CF59]=0`.

There is no pending-text draw if the flag is zero.

After `1C258` returns, `1C2C4` calls `30700` at `1C2CC`.
`1C2D1..1C2D7` compares the observed unsigned IRQ DWORD
against `DWORD[CF54]`, and jumps over the erase stage on
`IRQ_tick <= deadline`. If `IRQ_tick > deadline`, execute:

1. `291B0(BYTE[C050])`;
2. `2939C(0,0x106,0x20,DWORD[2222C])`;
3. store `DWORD[CF54]=0xFFFFFFFF`;
4. `1A8C8(DWORD[C050],0x106,0x20,0x0C,0x100)`.

The sentinel store is **before** the second compositor call.
Original 1C2C4 does **not** require the pending flag to be
nonzero before testing expiry. A queued message can be drawn
and erased in **the same call** if the tick read after drawing
already exceeds the deadline. The original unsigned comparison
is **not** wrap-safe elapsed-time subtraction.

The new `CombatIrqDisplayModel::inspect` exposes only an
ordered **call/write trace** and projected flags/deadline.
It does not paint pixels or implement the external services
`291B0/2939C/3060C/1A8C8`. Future execution must re-read
the underlying `C050` and `2222C` at each original call;
their values are **not** frozen by this read-only plan.
Snapshot `CF59` is captured at 1C258 entry; `CF54` and
IRQ tick are captured later, at the 1C2D1 comparison.
Never sample this IRQ time ahead of presentation.

## Source and verification

New C++ files:

- `include/rloft/combat_level_message.hpp`
- `src/combat_level_message.cpp`
- `tests/combat_level_message_tests.cpp`
- `include/rloft/combat_irq_display.hpp`
- `src/combat_irq_display.cpp`
- `tests/combat_irq_display_tests.cpp`

CMake adds both implementations to `rloft_boot_flow` and
registers `combat_level_message_tests` and
`combat_irq_display_tests` with CTest.

The original-file checker gained **30 exact x86 instruction
signatures** and **two separate SHA-256 data anchors**, all
directly verified against uploaded CODE.2. Its cumulative
ledger now has **487 named x86 byte checks at 479 distinct
logical instruction addresses**, one direct Strength data
anchor, **nine** independently hashed DGROUP spans and
**one** hashed CS data span.

The exact two **committed implementation sources and headers**
were independently copied to a local minimal compatible
PartyImage harness. Their Git blob SHA identities matched
GitHub. The source passed `-std=c++17 -Wall -Wextra -Werror
-pedantic` in Debug, optimized `-DNDEBUG` Release with
assertions enabled, and UndefinedBehaviorSanitizer against
the supplied original CODE.2. A local test exercised
formatting, bounds, non-ASCII character bytes, same-step
presentation and expiry, sentinel and unsigned clock wrap.
**This is not yet a full compile of the two checked-in test
files or a consolidated repository/Windows build.**

## Remaining integration gates

1. Map actual `291B0/2939C/3060C/1A8C8` renderer
   services to the native framebuffer in correct order and
   compare reference-DOS pixels/frames.
2. Connect `3210F` output, `1C21C` IRQ enqueue and
   `1C2C4` display lifecycle to shared original globals
   while preserving the **post-render** IRQ observation.
3. Only then stage real DDA4 progression/XP/level writes:
   original 50DC, optional 3EA4/3DE4 spell-state update,
   exact message creation/queue, DC7C RNG-consuming HP,
   and final DBD4 attack-target update.
4. Execute a deliberate full CMake/CTest/Windows build and
   reference DOS behavior comparison before claiming a new
   playable-combat or 100%-faithful release.

**Overall faithful demo acceptance remains 20% (1/5).**
GitHub Actions remains manual-only; no private runner minutes spent.


## Later actual Windows build and live-callback bridge checkpoint

The [2026-10-08 build and IRQ/VGA checkpoint](WINDOWS_BUILD_AND_IRQ_RENDERER_CHECKPOINT_20261008.md)
records **a real Windows MSVC build and 130/130 CTest PASS**
at exact source SHA `40e3f14b1072a7a0180ea1b0b4c1cea437137631`,
with a refreshed Windows executable committed and artifact uploaded.
Three MSVC test-source compilation errors and one original-status
fixture mismatch were corrected to reach the green run.

Subsequent source **added after the passing Windows binary** is
`CombatIrqDisplayRuntimeModel::update`, which actually invokes
the previous read-only `1C258/1C2C4` renderer services in their
original order through callbacks. The original independent IRQ
clock is still supplied by the host, never invented. This remains
**non-pixel-integrated source** until 291B0/2939C/3060C/1A8C8
are bound to the real native renderer. New source and CTest
have only strict isolated C++17 validation; do not apply the
earlier Windows PASS to those unbuilt new files.
