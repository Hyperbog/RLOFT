# Phase 1: Menzo / Stone Prophet RES9 wordcode VM

**Status: PARTIAL; NOT A CERTIFIED FULL OPCODE/OPERAND CODEC.** The authenticated original `CODE.2` binaries and preserved `RES9` files control these findings. See [cross-title diff](../../CROSS_TITLE_DIFF.md), [Strahd VM](WORDCODE_VM.md), [opcode probe](../../Tools/phase1_res9_opcode_probe.py), [handler map](../../Tools/phase1_res9_handler_map.py) and [arity test](../../Tools/phase1_res9_arity_correlate.py).

## Authenticated dispatcher witnesses

| Field | Menzo | Stone |
| --- | --- | --- |
| CODE.2 SHA-256 | `4a4fa725dddea00cb62ff44f44c48999cf6ed4c899b16c21826e8141ec7fda96` | `00e225ec8f141a8fdb72ff0547fe93b2ea3aabdb4ec3b59d9261a5268de9203e` |
| Opcode fetch | `0x19B2F` | `0x1E1FB` |
| Subtract two | `0x19B37` | `0x1E203` |
| Upper guard | `0x19B40`, limit `0x2A` | `0x1E20C`, limit `0x35` |
| Jump table | `0x19A6C` | `0x1E10C` |
| Indexed branch | `0x19B4D` | `0x1E220` |
| Cases | **43** | **54** |
| Encoded opcodes | `0x02..0x2C` | `0x02..0x37` |
| Word stream in LE data object | `0x1D950` | `0x2EC30` |
| Program-word IP global | `0x8C0C` | `0x1599C` |
| Raw RES9 programs | **31**, two-bank | **48**, one-bank |

**VERIFIED:** The original interpreter reads a little-endian 16-bit opcode, subtracts two, bounds-checks the result and indexes a table of 32-bit code-object offsets. Every listed case target is a decoded original instruction start. The exact 43 and 54 case-handler maps are exported by the automated audit; case indices are *not* encoded opcode values.

## Verified examples of opcode operand consumption

| Encoded opcode | Menzo case entry | Stone case entry | Physical CODE.2 observation |
| --- | --- | --- | --- |
| `0x02` | `0x19BB2` | `0x1E2B1` | Calls distinct interaction helpers `0x18850` / `0x1C4E4`; helper-level IP updates differ and the full effective footprint is unresolved |
| `0x03` | `0x19BBD` | `0x1E2BC` | Reads two 16-bit words; advances IP two |
| `0x04` | `0x1A3B9` | `0x1EC3B` | Default/no-op return; no direct operand |
| `0x05` | `0x19BF4` | `0x1E2F8` | Calls title-specific helper; caller does not advance wordcode IP |
| `0x06` | `0x19BFF` | `0x1E303` | Reads one 16-bit word; advances IP one |
| `0x07` | `0x19BA9` | `0x1E2A8` | Returns code 1 without a direct operand |
| `0x0E` | `0x19D3F` | `0x1E3C2` | Returns code 2 without a direct operand |
| `0x0F` | `0x19F19` | `0x1E67A` | Reads three words; advances IP three |
| `0x1B` | `0x1A32A` | `0x1E97C` | Returns code 3 without a direct operand |
| `0x2C` | `0x1A3B2` | `0x1EA36` | Sets a state flag without a direct operand |
| `0x2D..0x37` | No cases | Eleven original handler entries | **Stone-specific**; full operand and effect semantics unresolved |

A shared **archive/parser envelope** is justified, but a common opcode numbering, operand grammar, or behavioral interpreter is **not** certified.

## Reproducibility and negative controls

The unchanged Strahd operand grammar fails to parse **31/31 Menzo** and **48/48 Stone** RES9 programs. Subtracting two from encoded opcodes while keeping the Strahd arities still fails **31/31** and **48/48**. A provisional width map derived from the original handler readsites completely consumes **5/31** Menzo and **20/48** Stone programs. The other failures and their program IPs are retained in JSON. Those partial successes are *correlation*, not certified field-level disassembly: branch targets and semantics remain to be proved.

Original Stone `RES9:574` differs between installed and extracted CD sources by five 16-bit words at program-word indices **246, 261, 268, 273, 313**. Neither the affected opcodes nor the active media version is established.

**Next:** trace full IP updates through interaction helper callees, test the remaining program corpus and validate all control-flow targets. No gameplay code or commercial original bytes were changed.

