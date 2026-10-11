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

## 2026-10-10 Original C050 scripted event-poller boundary

**Source:** original Strahd `CODE.2`, SHA-256
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`;
object-1 logical offsets below. After original `160E1` assigns
`EBP=1`, the selection loop calls `16034` at both
`160F1` and `16105`. This is **not just a fixed drawing helper**.
It scans five 12-byte timer/event records at `14024+0xC*i`
(`EBX=0,0xC,..,0x30`, stop at `0x3C`).
For active entries whose original tick `30700` exceeds the
per-entry deadline `1402C+EBX`, it temporarily assigns the source
event cursor `7FEC` from `14024+EBX`, repeatedly calls
`15534` until its result is nonzero, restores `7FEC` and
updates the next deadline using `14028+EBX`.

**Independently proven indirect dispatch:** source `15534` checks
`BYTE[8101]`, reads a **16-bit** script opcode from
`WORD[13020+2*7FEC]`, increments `7FEC`, and performs an
unsigned `cmp ax,0x29`. Opcodes `0..41` dispatch through
`1556D: jmp DWORD PTR CS:[0x1548C+EAX*4]`; larger opcodes
take the `1558E` fallback. The original table at
`1548C..15533` contains **42 unique DWORD targets**; every
source-stored entry points within `15534..15E37`.
Example entries: opcode 0 `155D0`, 27 `1553F`,
41 `156F2`. These values are verified **in original file
bytes**, without assuming how the original DOS loader handles
relocations or writable executable pages.

The `16034..16089` timer wrapper directly saves/restores
`EBX` and `ESI`; its decoded bounded body does not itself
write `EBP`. The `15534..15574` dispatcher likewise contains
no direct `EBP` manipulation, but it jumps into handlers that
make further calls. This **adds a second concrete transitive
uncertainty** beyond the installed `F0C8` graphics callback:
the complete event handler and runtime-callback register effects
are **not closed**. In particular, it remains unjustified to
replace `16221 xor DWORD[C050],EBP` with a byte-only toggle.

**Regression:** Extended the already pinned
`verify_original_ground_c050_page_toggle.py` oracle with
four SHA-pinned original code/table slices (including the
42-entry table), 17 precise instruction windows, nine near-E8
destinations and every source target's range/uniqueness.
A separate direct original-byte check passed locally.
The broad `15534..15E37` hash is a **bounded code slice**,
not a claim that it is one function or exhausts transitive
effects. No new CTest registration or native gameplay change.

**Open:** handler and callback transitive side effects,
protected-mode ES/DS and any original runtime C050 state,
the current Windows Full build, and original DOS parity.

## 2026-10-10 Part 12 full 42-handler local call-edges and opcode-17 page flip

**Authority:** independently decoded original Strahd CODE.2 object-1 bytes,
SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`,
original script dispatcher table `1548C..15533` and all 42 entries
within original bounded handler slice `15534..15E37`. This is a
**source-bounded reachability and direct-effect proof**, not proof
of all transitive callees, installed pointers, or DOS segment aliasing.

The entire `15534..15E37` region was instruction-decoded from the
authenticated machine-code bytes. **All 48 direct E8 calls** from
this region were enumerated: they target **36 distinct routines**,
including three recursive calls back to `15534`. The source slice
contains **no directly decoded EBP register references**, one
indirect **CS jump-table dispatch** at `1556D`, and no directly
encoded call to the setter `2E74C`. All 48 byte-level E8
displacements are now independently asserted in the pinned C050
regression. A full decoded-slice SHA already authenticates the
source instructions; this does **not** establish callee ABI or
absence of computed memory aliases.

The handler region contains **eight** raw literal DWORD
operands matching address `C050` and **zero** matching graphics
callback `F0C8` or event callback `F0CC`. This is a
four-byte literal census only; address computation through
registers or external routines is still open.

**Script opcode 17** (original jump-table target `15C78`)
is a concrete VGA page state writer. It first reads BYTE `C050`,
calls `291B0` and `15320`, then chooses a branch on
`BYTE[8102]`. On the zero branch, it pushes the **full DWORD
C050** at `15C96`, calls `1A510` at `15C9C`, **XORs only
BYTE[C050] with 1 at 15CA4**, and calls `1A9F8`
at `15CAB`. On the other branch it passes the original
DWORD page and coordinates to `1A8C8` at `15CC6`
without executing the direct `15CA4` flip. Preserve
this byte-versus-DWORD distinction and the branch-specific
order in any native script implementation.

Original `1A8C8..1A9F7` is **304 bytes** of graphics work,
whole-body SHA-256
`8f3572ad75c478d56df8d82236281a79e701a8ff566d4820a4d3f3b15f91be14`.
It saves EBP at `1A8CB`, uses `1A8F5 movsx ebp,WORD[9E31]`
internally, and restores EBP in **both** source epilogues:
`1A9D0..1A9D4` and `1A9F3..1A9F7`. Local restoration
is proven; its `2A078/1A288/30CD4/1AB20` callees and
other external effects are still unverified.

**Regression:** the existing Python source/binary C050 oracle
now checks all 48 near-call targets, the opcode 17 table entry,
two additional complete SHA-256 code spans, 16 critical
instruction windows and exact literal-address counts.
The new assertions passed locally against the uploaded original
CODE.2. There is **no new CTest registration or native gameplay
change**. Windows Full on current HEAD, original-DOS page/tick
behavior, external callback installation and ES/DS alias safety
remain **not accepted**.

## 2026-10-10 Opcode-17 VGA display-page helper closure

**Original authority:** authenticated shipped CODE.2 object-1,
SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
Independent original disassembly and the local binary-only oracle
confirm **11 bounded complete helper bodies** (SHA-256 including
RET), **20 exact E8 destinations**, **44 byte windows**, and no
literal operand of `C050`, `F0C8` or `F0CC` in those bodies.
Direct-address absence does not exclude register-computed addresses,
transitive calls or callbacks.

The **zero-mode script opcode 17** takes
`15C9C→1A510`, then XORs BYTE[C050] with 1 at `15CA4`,
and calls `15CAB→1A9F8`. The original `1A510..1A5AE`
(159 bytes) has four direct E8 calls (to `1A288`,
`291C8` twice, and `1AB20`), with no direct EBP
register modification. `1A9F8..1AAC9` (210 bytes)
has eight direct E8 calls (`2E590`, `2A020`, `30CD4`,
`1AACC`, `2E5D4`) across its two source branches.
It also has no direct EBP-register modification. Their callees
remain independent acceptance subjects.

**Two distinct source display/draw page selectors**:
`291B0` calls `29FEC→2AB6C` to update the original
`F001` drawing framebuffer page, then stores its selected
BYTE in `EF4D` at `291C2`.
In contrast `291C8` calls `29FFC` and stores the selected
BYTE in `EF4E` at `291DA` (display page).
`29FFC` calls `2ABF4`, which masks interrupts using
`CLI` at `2ABF6`, writes two CRTC registers via
`0x3D4` at `2ABFF/2AC05`, reenables interrupts using
`STI` at `2AC07`, then calls `2ABC4`.
The latter polls port `0x3DA` until vertical-retrace bit
`0x08` passes the two source loops. These are **actual
original hardware instructions**, not a suggestion to
emulate CRTC timing as a host sleep.

The original page-copy helper `2A020..2A074` uses
`ENTER` and `LEAVE` to save/restore its EBP frame, and
copies `rep movsb DS:[ESI]→ES:[EDI]` after VGA sequencer
register setup. **ES is not set within this helper**, so
its runtime ES selector alias must still be proven.
The separate original memory mover `30CD4..30D25`
takes overlap-safe forward/backward branches, saves ES,
temporarily does `mov ES,DS` at `30CF9/30D14`,
and restores ES using `pop ES` at `30D0A/30D22`.
The early equal-source/destination return at `30D23`
does not change ES. This closes the **local ES save/restore
boundary for 30CD4 only**, not for the VGA raster generally.

The original page-selection stack helpers `2E590`
and `2E5D4` manage `F0D0` and call `291B0` to
restore drawing-page selection. They must not be
confused with the *different* mutable callback
pointers `F0C8` and `F0CC`.

**Evidence and limits:** Eleven full SHA witnesses,
20 E8 checks, 44 opcode windows, original RET boundaries
and bounded literal operand census passed a targeted local
verification against the supplied CODE.2. The existing
`original_ground_c050_page_toggle_tests` source-only CTest
registration now covers this additional source oracle;
no new CTest or native gameplay change. Deeper callees
`1A288/1AB20/1AACC/2F580/29FFC→2ABF4` should
be tracked by their actual source body (display chain
bounded here; other transitive paths remain open).
Installed `F0C8` targets, full `EBP` caller preservation,
VGA ES/DS mapping, native indexed palette/tick output
and original-DOS parity remain unresolved.

