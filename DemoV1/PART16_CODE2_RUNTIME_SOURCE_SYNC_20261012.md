# Part 16 — CODE.2 original runtime source synchronization

**Owner:** `demo-v1`; **base branch source:** `Code2/Code2.cpp` blob
`b6d4fb44bda916f892b65d84cf4ec3005affdc88`.
**Target:** already byte-audited canonical `code1-decompilation` blob
`239a63468bf2132ce71f3e5e24debc8c33beaa11`.

## Bounded source comparison

Exactly **three source regions** differ between these complete 1.3 MB
translation units (besides the resulting line-number shifts). Synchronize
the complete exact canonical Git blob, not an unreviewed branch merge.

| Original source | Original x86 witness | Synced source result |
| --- | --- | --- |
| `375FE / 37607 / 3760C` | `A3 F2 05 01 00`, `66 89 15 F6 05 01 00` | Save previous interrupt-24 handler far offset and 16-bit selector to guest DGROUP `105F2/105F6` before registering the replacement |
| `398E0 / 39924..3999D` | `39940: LEA EDI,[EBX+20]`; `39943: CMP ESI,[EBX+24]` | Restore the sentinel-as-successor semantics for an address-ordered circular free list, allowing the adjacent backward-coalescing case |
| `39A2C / 39A39..39A45` | `MOV EAX,[EBX+24]; MOV EAX,[EAX]; MOV EDX,[EBX]; ADD EAX,2C; CMP EAX,EDX` | Compare the descriptor-derived bound with **the DWORD stored at the node**, rather than the node's pointer value |

## Provenance and regression

- Authenticated owner-original `CODE.2` is **558,689 bytes** with SHA-256
  `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- The original LE object-1 code maps original logical byte offset `X`
  to physical file byte `0x3AF24+X` for these authenticated ranges.
- Independent local original bytes were disassembled as x86-32 and
  confirmed the seven guarded instruction windows, including both
  interrupt-save stores and descriptor dereferencing. Full original
  file bytes are never committed.
- CMake registers `original_code2_runtime_sync_contract_tests`.
  `tools/test_code2_runtime_source_sync.py` checks source and
  deliberately mutated known-bad variants without any original
  game files. With `--original /private/CODE.2` it additionally
  requires the full original SHA/length and exact machine-code windows.
- Its free-list successor test is a **source algorithm model only**:
  it does not execute the C++ allocator, prove malformed-list behavior,
  or establish actual DOS runtime or end-to-end heap parity.

## Scope and limits

This source sync does **not** claim that DemoV1 calls each canonical
CODE.2 routine in ordinary gameplay, nor that original DOS service
or heap behavior matches at runtime. The restored front-end preview
still stops before writing CHARS or entering original Level 22.
Actual program startup/menu selection, CODE.1-to-CODE.2 transfer,
indexed framebuffer, IRQ/timing and full-game progression remain
distinct implementation/acceptance gates.

The source audit must be accompanied by an exact-checkout Windows
Full build before the source can be declared compiled/tested on
Windows. Registered CTests and independently captured original-DOS
behavior are different evidence grades.
