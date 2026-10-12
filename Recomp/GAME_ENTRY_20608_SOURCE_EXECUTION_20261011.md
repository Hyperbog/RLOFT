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
             -> 375FE far callback   EXPLICIT STOP, DOS boundary
```

Previously, `rloft_code2_startup_gate` stopped at 20608 **before
executing it**. In the new opt-in mode, the original application runs,
mutates its original-sized DGROUP, and enters the actual original
audio/resource setup through the first unimplemented 375FE DOS/4GW far
callback installation. Original hardware interrupt installation has **not**
been invented, silently skipped, or called through a null boundary.

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
A faithful implementation must reconcile original 375FE semantics,
selector lifetimes, driver IRQ services and the interrupt callback before
advancing the *real* 2F2EC initializer. The next major integration stage is
then original graphics/sound resource setup and the real 1FC14 application
event loop. In particular, **this checkpoint does not boot the whole game,
play the intro, use the user's CHARS/SAVE, or establish original DOS runtime
equivalence**. Hardware stubs must remain fail-closed; DO NOT replace them
with synthetic successful responses to pass a test. Native-engine work is
a different branch and not included here.
