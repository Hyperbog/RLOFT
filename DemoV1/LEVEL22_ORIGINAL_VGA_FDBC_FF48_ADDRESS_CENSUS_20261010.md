# Original CODE.2 VGA page-base FDBC and texture-offset FF48 address census — 2026-10-10

**Original authority:** supplied Strahd `CODE.2`, SHA256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`, 558,689 bytes, object-1 physical `0x3AF24` and exact code size `0x3A852`. This is an **exhaustive four-byte literal operand search only**, not a proof of indirect alias freedom, original protected-mode ES selector base, or every possible runtime `C050` page state.

## Verified object-1 direct address references

Scanning all original object-1 code bytes for the absolute DWORD addresses, and checking the preceding instruction opcode for each occurrence, yields exactly:

| Address | Opcode/operand positions | Source action |
|---|---|---|
| `FDBC` VGA page base | `1F1E7` (`A3`) | **Sole direct-address store**, original renderer `1F1E6` |
| `FDBC` | `273FA`, `33DF5`, `33E3D` (each `8B 3D`) | Read into `EDI` for scene/compositing, lower flat fill and upper flat fill |
| `FDBC` | `3420D` (`03 1D`) | Add page base to a separate indexed drawing destination |
| `FDBC` | `34A9E` (`03 05`) | Add page base to `FF48` for original `34A74` textured raster |
| `FF48` texture offset | `1F345`, `1F3B6` (each `C7 05`) | Store upper `0x148` and lower `0x50F8` texture origin |
| `FF48` | `34A98` (`A1`) | Read texture offset before adding `FDBC` |

Thus original `FDBC` has **six direct references, one store and five reads**; `FF48` has **three direct references, two stores and one read**. The exact `1F1D6/1F1E6` page formula is `FDBC = 0xA0000 + (DWORD[C050]<<15)`. The shipped original DGROUP DWORD `C050` equals **1**, and `34A97` computes `FF40=FF48+FDBC`. All these source windows passed against the uploaded original binary.

## Why this matters and what remains open

The complete direct-reference census establishes that **none of the known original flat-fill or textured-raster helpers directly overwrites the page-base global**. It strengthens the previous [raster, VGA page and 80/81 source-clipping audit](LEVEL22_ORIGINAL_34A74_UNROLLED_PLANAR_RASTER_AUDIT_20261010.md). Source offsets for valid page 0/1 remain bounded to planar VGA aperture memory. It does **not** prove runtime `DWORD[C050]` stays within 0/1, that `FDBC` cannot be written via register-computed addresses, or that protected-mode `ES:[EDI]` is mapped to the same base as those linear values. Original `F0C8` indirect callbacks and the upper overlay may still alter other source state.

The added `tools/verify_original_ground_vga_page_refs.py` independently enforces the source file identity, **all nine** direct address operand positions, nine opcode prefixes, five instruction-aligned setup windows, and original `C050` initializer. It is registered as a source-only Python CTest `original_ground_vga_page_base_reference_tests` with no original private asset dependency in CI. No live renderer branch or indexed pixel has changed. Current Windows Full #658 remains the last accepted build, **not** validation of the current source. Independent original DOS framebuffer, palette, IRQ/RNG, ES selector and full `C050` runtime-state acceptance remain pending.
