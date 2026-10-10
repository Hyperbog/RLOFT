# Original FF50 direct-address operand audit — 2026-10-10

**Immutable source authority:** user-provided Strahd `CODE.2`, 558,689 bytes, SHA256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`; object-1 physical range `0x3AF24..0x75F24`. This is a **complete scan of literal operand bytes**, not proof about every possible pointer-based/indirect writer.

Original `CODE.2` contains **exactly seven** little-endian absolute references to DWORD `FF50` (`50 FF 00 00`). Each original opcode prefix was independently checked and disassembled:

| Instruction | Address operand | Opcode prefix | Original purpose |
|---|---|---|---|
| `1F29D` | `1F29F` | `89 0D` | write calculated upper FF50 |
| `1F2A5` | `1F2A7` | `C7 05` | write 128 open-field upper FF50 |
| `1F30E` | `1F310` | `83 3D` | compare FF50 with zero |
| `1F377` | `1F379` | `03 1D` | **reload** current FF50 into lower calculation |
| `1F37F` | `1F380` | `A3` | write calculated lower FF50 |
| `34AB9` | `34ABB` | `8B 0D` | read FF50 for first raster phase |
| `34B47` | `34B49` | `8B 0D` | read FF50 for paired raster phase |

There are **three** direct stores, **one** direct comparison, and **three** direct reads. No *direct literal-FF50-address operand* occurs in the original upper callbacks beginning `29B40`, `314B0` and `33E28`; the original `34A74` raster has two read operands. Thus the existing source projection `lower_ff50` is consistent with the direct accesses, and `1F377` genuinely reads mutable FF50 rather than a cached host value.

**Limit:** this cannot exclude indirect register/pointer aliases, nested callback callgraph effects, runtime overlays or stores through computed addresses. The C++ source's lower FF50 remains **conditional on the upper callback path leaving mutable FF50 unchanged**. Do not use this audit to claim partial-depth rendering, change actual indexed pixels, or mark original DOS framebuffer/timing parity accepted.

A reproducible independent source tool `DemoV1/tools/verify_original_ground_ff50_direct_references.py` verifies the exact original file hash, all seven operand offsets and opcode prefixes. Command:

```sh
python DemoV1/tools/verify_original_ground_ff50_direct_references.py --original-code2 /path/to/CODE.2
```

This was locally independently validated against the mounted original. A default no-private-file Python CTest checks project wiring. The next proof gate is the callgraph/indirect-alias audit for the upper callbacks, full `C026/C027` state, `34DE8` ordering and original DOS pixels. CI remains user-authorized manual-only and the last certified Windows Full #658 was 214/214 on older source.
