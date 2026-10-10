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
