# Original executable disassembly lookup cache

This tool accelerates read-only reverse engineering of the original *Ravenloft:
Strahd's Possession* executables. It **does not** change the native demo, alter
game mechanics, or waive the original-byte fidelity checks.

## Rebuild from a legally installed original copy

GNU binutils `objdump` and Python 3.9+ are required. From the repository root:

```sh
python3 Tools/original_disassembly_cache.py \
  --code1 /path/to/CODE.1 \
  --code2 /path/to/CODE.2 \
  --out .rloft-disassembly-cache
```

Outputs are locally generated and gitignored:

- `CODE1_OBJECT1_DISASSEMBLY.txt` and `CODE2_OBJECT1_DISASSEMBLY.txt` —
  complete **linear-sweep** object-1 x86 disassembly, suitable for searching.
- `CODE1_OBJECT1_ADDRESS_INDEX.tsv` and
  `CODE2_OBJECT1_ADDRESS_INDEX.tsv` — tab-separated logical addresses,
  physical original-file offsets, exact encoded bytes and candidate mnemonics.

Original binary identities are mandatory; incorrect size or SHA-256 causes
the tool to abort **before producing any output**. Every byte reported by
`objdump` is checked against the extracted immutable executable-object
bytes. The index must span the complete object exactly.

| Original | Full-file SHA-256 | Object-1 file base | Object-1 length |
| --- | --- | --- | --- |
| `CODE.1` | `fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed` | `0x2C724` | `0x11302` |
| `CODE.2` | `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314` | `0x3AF24` | `0x3A852` |

## Required interpretation discipline

**Linear disassembly is not a certified instruction boundary map.**
Disassembling consecutively from object start may misdecode embedded data,
jump tables, relocated entry points, branches, shared tails or deliberately
overlapping regions. An apparent mnemonic in this file is a **lead**, not
evidence of original behavior.

For source-level changes, inspect the exact bytes at the true entry point,
reconstruct control-flow reachability and call/stack semantics, and consult
the certified `Code1/Code1.cpp`, `Code2/Code2.cpp`, preservation
documentation and targeted original-byte regression checks. Preserve original
widths, RNG sequence, separate clocks, animation dwell, queued input,
interruptibility, nonstandard quirks and game-world state transitions.
Compare original DOS timing and frame traces before any 100%-fidelity claim.

The existing `DemoV1/tools/verify_combat_entry_bytes.py` maintains a
separate **verified instruction-signature ledger**; generated linear output
must never be counted as proof of those verified anchors.

Original copyrighted bytes must not be added to the public repository,
including reversible full disassembly text. Keep the generated cache locally,
regenerate from the owned game copy when necessary, and commit only the
generator, research findings, and non-proprietary verification metadata.

The tool is not wired to GitHub Actions and requires no CI minutes.
