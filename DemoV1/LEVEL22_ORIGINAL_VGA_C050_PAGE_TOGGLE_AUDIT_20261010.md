# Original C050 VGA page toggle / width census — 2026-10-10

**Authority:** uploaded Strahd CODE.2, 558,689 bytes, SHA-256
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
Object-1 starts at physical `0x3AF24` and its executable extent is
`0x3A852` bytes; DGROUP object-2 begins at `0x75F24`.
The original DWORD at `C050` is initialized to `1`.

## Complete **direct literal operand** inventory

Independently inspected every occurrence of the four-byte little-endian
address `50 C0 00 00` within object-1 and reconciled its opcode
prefix against original i386 disassembly. All **339** occurrences resolve
to precisely eleven observed source opcode forms:

| Opcode / width | Count | Source action |
| --- | ---: | --- |
| `FF 35` | 115 | push DWORD |
| `0F B6 05` | 111 | read byte zero-extended |
| `A1` | 25 | read DWORD into EAX |
| `A0` | 13 | read byte into AL |
| `69 15` | 6 | imul from DWORD into EDX |
| `8B 15` | 5 | read DWORD into EDX |
| `8B 0D` | 4 | read DWORD into ECX |
| `69 05` | 2 | imul from DWORD into EAX |
| `8B 35` | 1 | read DWORD into ESI |
| `80 35 ... 01` | **56** | XOR **BYTE** C050 with 1 |
| `31 2D` | **1** | XOR **DWORD** C050 with EBP at **16221** |

There are **282 reads** and **57 directly encoded writes**.
All 56 byte-XOR immediates are precisely `1`.

## Exceptional original DWORD toggle and page-base dependency

At `160E1` the source has `mov ebp,1`, then at
`160E8` enters the graphics/event loop. One path invokes
`14218` and `1A5B0`; at `16221` it
performs `xor DWORD PTR [C050],ebp`, reads **BYTE**
`C050` at `1622A` and calls `291B0`
at `16234`. This differs **in width** from the byte XOR at
`160C0`. The original ground renderer `1F1C8`
reads the BYTE for page selection and `1F1D6` reads the
entire DWORD to compute `FDBC=0xA0000+(DWORD[C050]<<15)`.

The local EBP assignment is verified by original bytes, but
**preservation of EBP across every intervening call and optional
callback remains to be established**. Thus the DWORD XOR cannot
yet be globally collapsed into a byte toggle. Raw operand matching
does **not** exclude register-computed pointer stores, foreign
modules, selector aliasing or live DOS page values outside 0/1.

## Regression and acceptance

`tools/verify_original_ground_c050_page_toggle.py`
authenticates the entire original file, verifies all eleven
opcode-class counts, every XOR-byte immediate, the unique DWORD XOR,
13 critical source windows, and the shipped initializer.
Its binary-only mode **passed locally** against the uploaded CODE.2.
A Python-conditional source-only CTest checks documentation and
registration; the existing pinned ground-batch manifest now includes
the binary oracle as `c050_page`.

**No native gameplay code changed.** Windows Full #658 is still the
last previously accepted build (214/214 on an older SHA); the present
changes are not Windows-built or accepted against original DOS
indexed frames, palettes, VGA ES/DS mappings, IRQ or RNG.

## 2026-10-10 EBP local save/restore and dynamic callback reachability

**Original source:** authenticated CODE.2 (same SHA-256 as above),
object-1 physical base `0x3AF24`. Independently re-disassembled the
`1608C..1624A` selection loop, `14218..14308` drawing helper,
`16400..16523` draw-body slice, `1A1F4..1A21C` input polling
wrapper and `1A5B0..1A723` page-flip helper. The source oracle
now guards **five bounded slice hashes, 24 exact opcode windows, six
E8 call destinations and three cross-address branches**. A targeted
original-byte check passed locally on the supplied original.

The entry at `1608C` pushes `EBX/ESI/EDI/EBP` and at
`160E1` explicitly sets `EBP=1` for the event loop. Two
source-proven callees locally restore it:

- `14218` saves `EBP` at `1421B`, independently sets
  `EBP=0x12C20` at `1425A` for item/card drawing, then
  restores at `14304` and returns at `14308`.
- `16400` saves `EBP` at `16403`, clears it at
  `16411`, and takes cross-range exits `16431→15FA3`,
  `164BF→15FA3`, or `1651F→15F9B` on the checked
  paths. The shared epilogue at `1602C..16033` restores
  `EBP` via `pop ebp` at `1602F`. The contiguous
  `16400..16523` byte hash is a *slice*, **not a conventional
  whole-function boundary**, because the function has shared
  predecessor-address tails.

The `1A5B0..1A723` page-flip helper itself has no EBP register
assignment in the independently disassembled body and preserves only
EBX/ESI locally. It invokes several other helpers and does **not**
provide end-to-end proof that every nested callee preserves EBP.

**Important reachable indirection:** after `160E1` sets EBP=1,
`160E8→1A1F4` and `1A1F4→2F868` are exact original near calls.
At `2F86D`, when `F0C8` is nonzero, original `2F876` invokes
`call DWORD PTR [F0C8]`. Neither its installed target nor all
transitive register effects have been established. This blocks
claiming that the `16221 xor DWORD[C050],EBP` is *always*
semantically identical to `xor BYTE[C050],1`, even though its
local initialization and two important helpers strongly suggest
ordinary preservation conventions.

**Acceptance:** source-byte check passed locally; the same registered
`original_ground_c050_page_toggle_tests` and pinned proof-batch
entry now cover these checks. No CTest was added and no gameplay
source changed. Current-revision Windows Full build and matched DOS
page-state, ES/DS, framebuffer, input/IRQ acceptance remain open.

