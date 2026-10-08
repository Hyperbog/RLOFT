# Part 14 — Core executable loader and disassembly implementation

## Components

| Package | Purpose | Current status |
|---|---|---|
| `DosDis.Core/BinaryArtifact` | Bounded original-byte read-only snapshot and SHA-256 | Implemented |
| `DosDis.Core/LeImage` | Bounded MZ→LE scanning, object/page decoding, file↔object offsets | Implemented for ordinary stored pages |
| `DosDis.Core/Disassembly` | iced-x86 disassembly at a supplied 32-bit object offset | Implemented linear decode only |
| `DosDis.Core/FileIntelligence` | Evidence-tagged known-fingerprint and extender-string facts | Implemented, limited database |
| `DosDis.Cli` | JSON inspect / disasm commands | Implemented |
| `DosDis.Smoke` | Synthetic fixtures and optional original-binary tests | Implemented |

## Address-space contract

- A file offset is the physical index in the original complete file.
- `ModuleOrigin` is the position of the embedded MZ; `HeaderOffset` is the LE signature in the same physical file.
- Object offsets are `LE object N + displacement`, **not** file offsets or unqualified process linear addresses.
- For an ordinary LE page record, the three-byte big-endian page ordinal is 1-based. With ordinal `p`, original file offset = `ModuleOrigin + DataPagesOffset + (p-1)*PageSize + PageOffset`.
- Each object's `FirstPage`/`PageCount` selects the corresponding entries in the executable's global page map. Unmapped/zero-fill virtual tails do not receive fictional source offsets.
- Relocations do not rewrite stored bytes. A future relocated-image type will carry a separate mapping and fixup provenance.
- The interface uses `ToFileOffset(object, offset) → null` to distinguish non-file-backed addresses.

## Error policy

- Truncated headers, offset overflow, invalid object/page geometry: fail, no partial pretend success.
- Unsupported page flags or ambiguous LE candidates: explicit error, no chosen fallback.
- Disassembly of arbitrary addresses is *requested linear decoding*, not a verified executable classification.
- Exact DOS version, compiler revision and ambiguous historical claims remain `Unknown`.

## Near-term acceptance work

1. Compile/test under .NET 10 on Linux and Windows in CI; fix all warnings/errors.
2. Verify real CODE.1/CODE.2 object page maps and first instruction vectors from authorized originals.
3. Add full LE fixup table and relocation decoding with independently reconstructed RLOFT witnesses.
4. Add a typed distinction between physical, object, segmented, and runtime addresses, followed by recursive control-flow traversal.
5. Add versioned SQLite persistence for user annotations, provenance, and candidate-versus-verified classifications.
6. Add a dedicated 16-bit MZ/.COM loader rather than applying 32-bit LE assumptions.
7. Implement WPF interface and File Intelligence pane after loader correctness gates.

## Reference sources

- IBM/OS2 LX/LE object format: https://openwatcom.org/ftp/devel/docs/lxomf.pdf
- Open Watcom source: https://github.com/open-watcom/open-watcom-v2
- iced-x86 .NET examples: https://github.com/icedland/iced/tree/master/src/csharp
- RLOFT object map: ../../../../Runtime/CODE_OBJECT_MAP.md
- RLOFT original source audits: ../../../../Code1/FINAL_BINARY_AUDIT.md and ../../../../Code2/FINAL_CERTIFICATION.md
