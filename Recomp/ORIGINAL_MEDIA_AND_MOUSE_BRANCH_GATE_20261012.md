# Canonical source recompilation — original media + CODE.2 mouse branch gate

**Checkpoint:** 2026-10-12  
**Owner branch:** `code1-decompilation`; **not** `demo-v1` or `native-engine`  
**Windows build:** [#32](https://github.com/Hyperbog/RLOFT/actions/runs/38202643128),
checkout `18a71e37fb5f0ec44ecb7486f754f7678095ef17`;
**22/22 registered CTests passed**. All complete canonical CODE.1/CODE.2
translation units compiled and linked; CTest #18 and #19 executed the
new, reconstructed mouse-branch pathways.

## Follow-on canonical recompilation proof — Windows #34

The source-backed mouse integration did not stop at the first DPMI
wrapper address. A further source change executes the **actual reconstructed**
`30D80 → 30D52` DPMI 0100h allocation request, including original
paragraph-count arithmetic and the **actual `37A73`** six-selector
snapshot in ES,CS,SS,DS,FS,GS order. The only test-owned callback
intercepts the subsequent `33A7C` external `INT 31h` boundary and
rejects any request other than `INT 31h / AX=0100h / BX=5 paragraphs`.
No DOS memory block, segment, selector, return carry, or success value
is fabricated. The mouse-present test now stops at
`STOP_BEFORE_DOS_INT31_0100_ALLOCATION`.

Likewise the no-device branch now executes canonical `311E8`, which
rewrites embedded dollar signs, appends its temporary DOS dollar
terminator, and reaches the original `INT 21h / AH=09` output call.
The test-owned output boundary checks that the original source has
prepared the expected text before stopping at
`STOP_BEFORE_DOS_INT21_AH09_MOUSE_WARNING`, without manufacturing
DOS output or altering the original string-mutation routine.

**Windows build [#34](https://github.com/Hyperbog/RLOFT/actions/runs/38202963930)**
completed successfully on exact source commit
`f7f94d8c38153707fde6eacc2474556442c50ec4`, with **22/22 CTests
passed**, including both expanded original-mouse branches and offline
14-path original-media identity/boundary contracts. This supersedes
the #32 source-execution boundary, not its preserved historical results.
The host test environment and original-DOS game parity are still
unaccepted and original media were not present in Actions.

## What actually executes

The canonical `Recomp/src/code2_startup_gate.cpp` continues to enter the
compiled source at original CODE.2 `353A0`, perform actual `35418`
startup, original priority dispatch `395E2`, four initializers, original
application `20608`, source environment/atoi conversion, original
`20440`, audio setup `2F2EC`, virtualized original 375FE far
callback registration, 30BB4 hardware-vector chain, 290D0/30CC4 RNG
initialization, original 37B42/39838 heap splitting, original 37E77
file descriptor open, 312EC/38106 59-byte read and 312DC/380F8 close,
and reaches the real `33B44` mouse reset routine. The guest environment
and emulator-of-DOS-service callbacks are deliberately bounded
**test-only** adaptations, not a real DOS/4GW host and not observed
original execution.

## New source execution beyond the former mouse boundary

Original file: immutable `CODE.2`, 558,689 bytes, SHA-256
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.

Verified directly against the original source object at file offset
`0x3AF24` + logical `0x33B44`:

- `33B58: MOV EAX,0` then `33B5D: INT 33h`: mouse reset.
- `33B6B..33B80` stores the low 16 bits of returned EAX/EBX into
  `DGROUP:2B7A2` and `2B7A4`, zeroes `2B7A6`.
- `33B86` branches on the *stored* availability word, not an invented
  generic Win32 state.
- On an explicitly simulated **present** response `AX=FFFFh,BX=3`,
  the real source follows `33B94` to `30D80(40h,FD14h,FD18h)`.
  The test stops *at* this DOS memory acquisition boundary. It does not
  synthesize a successful memory result.
- On an explicitly simulated **absent** response `AX=0,BX=0`,
  the real source follows `33C2E` to text output
  `311E8(15C8h)`. It stops at that DOS warning dependency; no
  fake warning output, successful fallback, or game continuation is
  claimed.
- Both CTests check that the original compiled source wrote the
  expected `2B7A2/2B7A4/2B7A6` memory state before stopping, in
  addition to the previous DOS vector, arena, audio file and game-entry
  checks.

The new executable probes are
`--mouse-present-to-memory` and `--mouse-absent-to-warning`.
Without these explicitly named fixtures, `--to-mouse-reset` still
halts **before INT 33h**; the faithful default is unchanged.

## Authenticated original files at the C++ execution gate

Prior testing generated a distinctive 59-byte synthetic sound-data
sequence. That is retained **only as the clearly labeled default
synthetic test fixture**.

The original media can instead be supplied explicitly:

```powershell
.\build\recomp\Debug\rloft_code2_startup_gate.exe --to-mouse-reset "C:\RLOFT\CODE.2" "C:\RLOFT\SOUND.CFG"
```

This now feeds the *real 59 file bytes* into the compiled CODE.2
37E77/38106/380F8 source execution. Unlike the old synthetic input,
the source-controlled bytes are not fabricated by the gate. The C++
host checks file length and the executed source verifies every byte
was transported intact. Because direct use of the executable alone
does not independently authenticate the 59-byte file's hash, use the
matrix below to enforce original-file identity.

```powershell
python Recomp/tools/original_media_matrix.py `
  --exe build/recomp/Debug/rloft_code2_startup_gate.exe `
  --code2 "C:\RLOFT\CODE.2" `
  --sound-cfg "C:\RLOFT\SOUND.CFG" `
  --report build/recomp/original-media-matrix.json
```

The Python matrix requires the canonical supplied-file SHA-256:
- CODE.2 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`
- SOUND.CFG `b373f84879c0dfe524009526989a3b6562f518a9a64280b4ee282673cb0474cc`

All **14 executable source paths** must return their exact independently
specified first unsupported service boundary; each records its status,
sound input provenance, source-entry confirmation and explicit
`GAME_BOOTED=NO`. The JSON report preserves the supplied file
identities, statuses, stop points and the falsity of any claim of DOS
parity. The matrix may be run without SOUND.CFG, but then it honestly
labels the sound contents as `SYNTHETIC_FIXTURE`.

## Evidence and blockers

- **Windows proof:** 22/22 synthetic, source-execution and offline
  tests passed on exact checkout, including five adversarial Python
  matrix contracts. This proves compilation, executable memory
  branching and deterministic test behavior. Neither original media
  was present on GitHub's Windows runner.
- **Original CODE.2 opcode proof:** the initial `33B44` machine-code
  branch and call sequence were independently re-disassembled from
  the original uploaded CODE.2.
- **Not proved:** real DOS/4GW 30D80 allocation, INT33 device
  response, real DOS 311E8 warning output, full video/input/IRQ/heap
  behavior, original DOS dynamic trace equivalence, character
  creation execution from canonical CODE.1 startup, or full playable
  CODE.2 state. No test fixture must be advertised as a completed
  original game port.
- **Next true runtime integration:** reconstruct the actual DOS
  protected-memory provider for `30D80` and the text/terminal
  adapter for `311E8` from canonical source and matched original
  binaries, then preserve game-entry progression to authentic
  resource/display initialization. Do **not** route this work
  through a Level22 demo or a native engine.
