# Canonical recompilation: actual CODE.2 main -> DOS far callback boundary

**Scope:** `code1-decompilation:Recomp/` exclusively, not `demo-v1` and not `native-engine`.  
**Date:** 2026-10-11 (America/New_York)  
**State:** Original-source integration authored; Windows CI execution must be checked against the exact checkout before acceptance.

## Delivered source integration

The *compiled original reconstructed CODE.2* is now wired through:

```
353A0  entry stub                    actual Code2.cpp
  -> 35418 DOS startup               actual Code2.cpp
  -> 395E2 four callback scheduler   actual Code2.cpp
       -> 3A16E stack init           actual Code2.cpp
       -> 3A2AD environment          actual Code2.cpp
       -> 3A1B3 original streams     actual Code2.cpp
       -> 3968E argv                 actual Code2.cpp
  -> 39589 post initializer          actual Code2.cpp
  -> 20608 actual game main          NEW EXECUTION, real Code2.cpp
       -> 352EE getenv lookup        actual Code2.cpp
          -> 39526 case comparison    actual Code2.cpp
       -> 35349 atoi parsing         actual Code2.cpp
       -> original DGROUP game state writes
       -> 20440 main setup           NEW EXECUTION, real Code2.cpp
          -> 2F2EC audio/resource    NEW EXECUTION, real Code2.cpp
             -> original EF53/EF3E/EF68/EF3C/F0C0 writes
             -> 375FE far callback   NEW EXECUTION, real Code2.cpp
                -> original extender-mode vector selection (0..255)
                -> actual DOS interrupt-vector installation   EXPLICIT STOP
```

Previously, `rloft_code2_startup_gate` stopped at 20608 **before
executing it**. In the new opt-in mode, the original application runs,
mutates its original-sized DGROUP, and enters the actual original
audio/resource setup, now including the reconstructed **375FE interrupt
vector choice**. The first unimplemented boundary is the real DOS
interrupt-vector installation callback called *inside* 375FE. Original
hardware interrupt installation has **not** been invented or silently
skipped.

## Three source-execution CTests

All reuse the original source compiler unit, the existing four-initializer
guest arena fixture and actual runtime scheduler; **none** substitutes a
new startup/game engine:

| CTest mode | DOS environment | Important state verified |
| --- | --- | --- |
| `--enter-game` | original baseline `no87=1` only | source defaults: position (4,4), heading 270 |
| `--enter-game-config` | original four CODE.2 game variable names with values 0,7,5,359 | native original 352EE/35349 lookups; position (7,5), heading 359 |
| `--enter-game-signed-wrap` | 22, -1, 65536, -90 | source atoi signed / 16-bit truncation / DWORD wrapping |

The tests verify:
- Original four callback init order, original original-argv/PSP startup path,
  CODE.2 environment list construction, streams, allocator metadata and
  pre-existing assertions.
- Exact CODE.2 20608 clearing of the `255E0..25610`, `22200..2220C`
  and `248E4/248E8` game-state fields.
- Original 16-bit sentinel/position stores at `255D0..255D6`, raw heading
  at `C658` under default, configured and signed-wrap cases.
- Audio/resource state initialized by **real 2F2EC** at
  `EF53`, `EF3E`, `EF68`, `EF3C` and `F0C0`.
- Actual 352EE getenv and 35349 atoi call counts and real far callback
  address/selector `2F2E0` + incoming CS, explicitly halted at original
  far install routine `375FE`.

The synthetic CTest fixture uses source-identical initial DOS environment
keys and a **restricted** ASCII ctype table for numbers. It makes no claim
that these guest-only fixtures reproduce Watcom, DOS, BIOS hardware, real
audio drivers, or on-screen game operation.

## Complete 375FE extender-mode branch matrix

A fourth new CTest invokes the **actual compiled canonical**
`code2::function_375fe` for each of the **256 possible 8-bit extender
mode values**. The encoded branch chooses vector **6**, selector
**0x24**, handler **0x37642** only for modes **2 through 6**; all other
mode values request vector **0x24**, selector **0**, handler
**0x37642**. The test observes the callback without installing a DOS
interrupt, and fails if the original source call count or selected
vector/selector/handler differs. In the authentic source-integrated game
path, that callback instead stops immediately before changing the host
DOS/IVT vectors. This new test asserts all branches of the original
8-bit mode selector, not game startup parity.

## Actual original CODE.2 argc/argv and DOS-mode correction (authoritative)

Windows canonical build **#22**, commit `4c10427ed1d73e7d2c9ad1b96db9e3e1f939ed0d`,
passed **15/15 CTests**. Earlier builds #20/#21 failed because the
integrator incorrectly assumed `2F2EC` always called checked allocator
`3104C`. The actual original CODE.2 branch depends on
**`[105FC] = argc`**, populated by the original Watcom `3968E`
argv initializer:

- **argc = 1, no extra DOS argument:** original `2F2EC` executes
  `3104C -> 37B34 -> 37B42 -> 39838` to allocate its 0x100-byte
  audio state and then `30D30 -> 379D0` to clear it.
- **argc = 2, numeric DOS argument:** original `2F2EC` takes
  `3968E`'s argv[1], invokes **real `35349` decimal conversion**,
  and uses that external DOS4GW device descriptor pointer directly.
  It does NOT execute checked allocation or memset on this branch.
  The supplied fixture address `186496` (0x2D880) is **test-only
  external DOS memory**, not a claim about the real original game.

Original `375FE` accesses a *different* variable, byte
**DGROUP 12662h**, written by the actual DOS runtime as the
extender-vector mode. Byte-for-byte original disassembly:
`37607: A3 F2 05 01 00` saves old far offset to **105F2h**,
`3760C: 66 89 15 F6 05 01 00` saves its selector to **105F6h**,
and `3761E: A0 62 26 01 00` reads the DOS mode from **12662h**.
The previous canonical `function_375fe` silently discarded the saved
far pointer. This was a **real decompilation semantic bug** and
has been corrected in `Code2/Code2.cpp`, with a full 256-value
source-execution matrix checking both selection and DGROUP writes.

Original DOS PSP fixture entry is now genuinely parsed through
`353A0 -> 35418 -> 3968E`, tested with zero or one numeric
argument, then passed into original `20608 -> 20440 -> 2F2EC`.
Two configured-environment alternatives yield **four distinct original
audio startup scenarios**. Every scenario uses the same compiled original
sources, and the old original source-linked tests are preserved.

### Newest execution boundary: original DOS AH=3Dh

Subsequent integration executes the complete original source route
`312A0 -> 37E77 -> 37E99`, including the original
`mode=0x200` read-open and zero access/share mask. The bridge now
stops at the *actual external DOS AH=3Dh open service*, rather than
stopping before the original `37E77` file routine. In the
`312A0` wrapper original x86 pushes only `(path, 0x200)`;
`37E77` reads an additional inherited stack slot. In read-only mode
`0x200`, the source `37E99` does not consume the extra
create-only parameter; a test-only sentinel is passed explicitly rather
than fabricating an original value. No on-disk original media or real
DOS handles are accessed in CI. A real file-service/guest handle bridge
is the next missing requirement.

Independent
`verify_original_game_entry_oracle.py` now checks **17 complete
CODE.2 code spans**, exact original `375FE` side-effect instruction
windows and `sound.cfg` data-name bytes against the original
558,689-byte file; missing owner media is never silently considered a
passing DOS comparison.

## Beyond interrupt installation: canonical audio setup to sound.cfg open

Two additional genuine-source integration paths, `--to-sound-config` and
`--to-sound-config-configured`, extend the same original
`353A0 -> 20608 -> 2F2EC` execution chain:

```
2F2EC -> 375FE  original far-vector/selector mode selection
       -> 30BB4  original vector 9/8/0 backup/install
       -> 290D0 -> 30CC4  original game RNG seed write
       -> 3104C -> 37B34 -> 37B42 -> 39838  checked allocation
       -> 30D30 -> 379D0  original checked memory fill
       -> 312A0 -> 37E77  original sound.cfg DOS file open: STOP
```

**Only the external DOS interrupt table is a bounded test fixture.** The
three vector entries 9, 8 and 0 start with distinct sentinel
offset+selector pairs. The CTests check original `30BB4` stores the
*actual prior* IVT values in DGROUP `F576/F57A`, `F582/F586`,
`F57C/F580`, then installs the three genuine code handlers `308F8`,
`30708` and `30B54`, and sets `F575`. The prior 375FE far vector
is captured in the same bounded test-only IVT. **No host DOS/Windows
interrupts are changed.**

The assertions verify original `290D0->30CC4` seeding;
`3104C/37B34` checked allocation through the actual reconstructed
arena splitter, its ten-byte header and `12345678h` magic;
original `30D30/379D0` zero fill; and source
`312A0->37E77` opening argument `(sound.cfg,0x200)`. At that
last original DOS I/O service, the test **stops fail-closed** rather than
claiming to have opened real sound hardware files or entered gameplay.

Two new CTests:
`canonical_code2_source_runtime_to_sound_cfg_open` and
`canonical_code2_configured_runtime_to_sound_cfg_open`.
New Windows build / original DOS parity are separate gates.

## Independent original CODE.2 binary oracle

`Recomp/tools/verify_original_game_entry_oracle.py` requires the untouched
original CODE.2 (size 558,689, SHA-256
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`).
The oracle authenticates five complete relevant instruction spans and all
four original data-object game configuration names plus original digit class.

Five span SHA-256 digests, determined independently from owner-original
CODE.2, are pinned for `20608..207D0`, `20440..205B3`,
`2F2EC..2F464`, `352EE..3539F` and `375FE..3769F`.
Every one of these **five original byte windows and four original strings**
was separately verified in the original-media environment. This is
original **binary provenance**, not Windows CTest parity.

```powershell
python Recomp/tools/verify_original_game_entry_oracle.py "C:\RLOFT\CODE.2"
cmake -S Recomp -B build/recomp -A x64
cmake --build build/recomp --config Debug --parallel 2
ctest --test-dir build/recomp -C Debug --output-on-failure
.\build\recomp\Debug\rloft_code2_startup_gate.exe --enter-game "C:\RLOFT\CODE.2"
```

## Acceptance boundaries / next real engineering work

The 375FE far callback is a DOS/4GW interrupt/selector installation.
A faithful implementation must reconcile the real DOS vector-install
service, selector lifetimes, IRQ drivers and guest interrupt callback before
advancing the *real* 2F2EC initializer. The encoded 375FE switch itself
is now executed by the canonical source and exhaustively checked. The next major integration stage is
then original graphics/sound resource setup and the real 1FC14 application
event loop. In particular, **this checkpoint does not boot the whole game,
play the intro, use the user's CHARS/SAVE, or establish original DOS runtime
equivalence**. Hardware stubs must remain fail-closed; DO NOT replace them
with synthetic successful responses to pass a test. Native-engine work is
a different branch and not included here.
