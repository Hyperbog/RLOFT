# Phase 1: Menzo / Stone Prophet RES9 wordcode VM

**Status: the candidate serialized wordcode grammar now decodes all 31/31 Menzo and 48/48 Stone programs with exhaustive direct-branch boundary validation. Original opcode/gameplay effects and runtime-conditional execution are only partially reconstructed; this is NOT a finished behavioral VM.** The authenticated original `CODE.2` binaries and preserved `RES9` files control these findings. See [cross-title diff](../../CROSS_TITLE_DIFF.md), [Strahd VM](WORDCODE_VM.md), [opcode probe](../../Tools/phase1_res9_opcode_probe.py), [handler map](../../Tools/phase1_res9_handler_map.py) and [arity test](../../Tools/phase1_res9_arity_correlate.py).

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
| `0x05` | `0x19BF4` | `0x1E2F8` | Caller delegates to party-slot helper, which consumes **one** word (0x18AC0 / 0x1C6A4) |
| `0x06` | `0x19BFF` | `0x1E303` | Reads one 16-bit word; advances IP one |
| `0x07` | `0x19BA9` | `0x1E2A8` | Returns code 1 without a direct operand |
| `0x08` | `0x19D48` | `0x1E3CB` | **Two words**: condition and target; Stone case branches to IP from second word or skips past it |
| `0x2E` | Not dispatched | `0x1EA59` | **Three words**: condition value, slot selector, conditional branch target. Helper `0x1D594` explicitly recognizes `999` as an empty-value test |
| `0x0E` | `0x19D3F` | `0x1E3C2` | Returns code 2 without a direct operand |
| `0x0F` | `0x19F19` | `0x1E67A` | Reads three words; advances IP three |
| `0x1B` | `0x1A32A` | `0x1E97C` | Returns code 3 without a direct operand |
| `0x2C` | `0x1A3B2` | `0x1EA36` | Sets a state flag without a direct operand |
| `0x2D..0x37` | No cases | Eleven original handler entries | **Stone-specific**; full operand and effect semantics unresolved |

A shared **archive/parser envelope** is justified, but a common opcode numbering, operand grammar, or behavioral interpreter is **not** certified.

## Reproducibility and negative controls

The unchanged Strahd operand grammar fails to parse **31/31 Menzo** and **48/48 Stone** RES9 programs. Subtracting two from encoded opcodes while keeping the Strahd arities still fails **31/31** and **48/48**. Reconstructed title-specific operand rules now consume **31/31 Menzo** and **48/48 Stone** programs linearly, with **no failures**. This establishes a complete candidate serialized grammar for the preserved corpora, corroborated by original CODE.2 instruction-pointer readsites; it does **not** prove unobserved instruction variants or complete gameplay semantics.

Original Stone `RES9:574` differs between installed and extracted CD sources by five 16-bit words at program-word indices **246, 261, 268, 273, 313**. Neither the affected opcodes nor the active media version is established.

## Instruction-proven variable-width cases

Menzo encoded opcode `0x2A` dispatches to `0x1A351`, which calls helper `0x19534`. The original helper consumes one 16-bit selector and up to four condition/value pairs; condition word `0x21` terminates the pair loop. Stone encoded opcode `0x37` dispatches to `0x1EA4E`, calling helper `0x1D77C`. The Stone helper consumes one 16-bit count followed by exactly that many 16-bit values. These are original-machine-code-backed physical rules; their game semantics remain unidentified.

These two cases supplied the first variable operand rules. Later CODE.2 tracing additionally established opcode `0x13`/`0x1A` choice tables, opcode `0x05` template-index consumption and Stone's one-word opcode `0x08`. These checks, together with the corrected two-word Stone opcode `0x08`, now produce 31/31 Menzo and 48/48 Stone candidate linear parses. The candidate grammar is **not** shipped as a unified runtime VM.

## Reconstructed conditional choice table (current audit)

**VERIFIED physical reads from authenticated CODE.2:** Both encoded opcode `0x13` and `0x1A` invoke the same title-specific choice helper, Menzo `0x19610` / Stone `0x1D118`. Each initially reads two `u16 LE` program words, followed on the choice-table path by zero to four two-word condition/value pairs. A condition value **`0x21` for Menzo** or **`0x26` for Stone** ends the loop. The paths' read-count can depend on runtime choice state; the static candidate parser models the complete serialized table, not a guaranteed execution trace.

The encoded opcode `0x05` call at Menzo `0x19BF4` / Stone `0x1E2F8` reaches a party-slot helper at `0x18AC0` / `0x1C6A4`. That callee reads a **single u16 LE program word** from the runtime word stream, advances the IP by one, and uses the word to select a 0x41C-byte character template. The old zero-operand assumption was wrong. Stone `0x08` at CODE.2 `0x1E3CB` reads the condition first, then **either** fetches the second word as the absolute target at `0x1E3F4..0x1E401` **or** skips that word via the shared `0x1EB7A` increment. Therefore the **serialized footprint is two words** regardless of the conditional path; the callee `0x15200` does not itself advance the RES9 IP.

## Stone last four: CODE.2-backed two-word conditional opcode `0x08`

The initial parser incorrectly assigned a **single** word to Stone opcode `0x08` because only the condition word is fetched before the callee. This misclassified the **second word (the target)** as a new opcode and caused all four remaining failures. Both executor routes are now traced:

- `0x1E3CB..0x1E3EE` reads condition, calls `0x15200` and selects a branch.
- If the condition succeeds, `0x1E3F4..0x1E401` loads the next 16-bit word into the program IP (absolute **word** index).
- If the condition fails, `0x1EB7A` increments IP to skip that target word.

The four formerly failing records are now decoded, and both their branch destinations and fallthrough indexes are aligned to instruction starts.

| Original Stone RES9 record | Opcode word index | Condition word | Absolute target word | Fallthrough word |
| --- | ---: | ---: | ---: | ---: |
| `568` | 138 | 183 | 143 | 141 |
| `575` | 59 | 159 | 483 | 62 |
| `585` | 32 | 199 | 146 | 35 |
| `595` | 135 | 214 | 141 | 138 |

**VERIFIED:** Four repaired records, both control-flow routes at CODE.2, and 48/48 complete serialized programs. The original UI/gameplay behavior invoked by those branches remains outside this proof.

## Stone's `999` is not a target: original empty-value matcher

The prior parser had treated the destination word of opcode `0x08` as another opcode. That shifted the parsing of `RES9:615..624` and produced **ten spurious out-of-range targets 999**. Correct parsing, verified in all ten original records, is:

| Word index | Parsed purpose | Value |
| --- | --- | ---: |
| 14 | encoded opcode | `0x08` |
| 15 | condition operand | 187 |
| 16 | branch target | 33 |
| 17 | encoded opcode | `0x2E` |
| 18 | *condition value* operand | **999** |
| 19 | indexed-slot selector | **21** |
| 20 | actual conditional branch target | **27** |

At CODE.2 `0x1EA59..0x1EAA0`, Stone opcode `0x2E` reads three operands. The first two are passed to helper `0x1D594`. If the helper reports true, the **third** operand is assigned to the program IP; otherwise the handler increments IP by three.

The original helper itself proves the sentinel:

- `0x1D59B` checks whether the slot selector is `0x15` (**21**).
- `0x1D5A1` compares the first operand to `0x3E7` (**999**).
- For `(999,21)`, `0x1D5A9..0x1D5B0` checks whether a signed 16-bit field at data `0x1F714` equals **−1**.
- For other slot selectors, `0x1D5D4..0x1D62C` scans up to four `0x41C`-byte party-character records, comparing a selected 16-bit field with **−1** when the first operand equals **999**, or with the supplied literal otherwise.

**VERIFIED:** `999` is a deliberately implemented **empty-value matching sentinel**, not a jump address and not an archive defect. Target **27** is a valid instruction boundary in all ten records. Higher-level inventory/character UI meaning is not asserted without further caller analysis.

## Corpus-wide validation (current verified checkpoint)

```sh
python Tools/phase1_res9_handler_map.py --root preservation/MASTER --repo-root . --json audit/res9-handlers.json
python Tools/phase1_res9_arity_correlate.py --root preservation/MASTER --repo-root . --json audit/res9-arity.json
python Tools/phase1_res9_control_flow.py --root preservation/MASTER --repo-root . --json audit/res9-targets.json
python Tools/phase1_res9_stone_four.py --root preservation/MASTER --repo-root . --json audit/stone-four-and-999.json
```

| Measure | Menzo | Stone Prophet |
| --- | ---: | ---: |
| Original RES9 programs | 31 | 48 |
| Completely linearly decoded | **31** | **48** |
| Checked direct targets landing on instruction starts | **876** | **1,473** |
| Checked direct-target boundary failures | **0** | **0** |
| Timer operands encoded as `0xFFFF` (semantics not certified) | **45** | **24** |

This is **2,349 / 2,349 inspected direct program-word branch targets** landing on decoded starts. The additional **69** `0xFFFF` timer fields are intentionally classified separately because their runtime sentinel semantics have not been demonstrated. Exact CODE.2 opcode, archive record and target-count witnesses are pinned by the CI regression scripts; only derived metadata are published.

**Still UNKNOWN:** complete per-opcode effects in the 43/54-case interaction interpreters, actual conditional runtime paths (not executed), `0xFFFF` timer behavior in Menzo/Stone, and Stone `RES9:574` installed-vs-CD media precedence and the meaning of its five changed words. These are separate Phase 1 tasks. No original media, saves or native gameplay implementation were changed.
