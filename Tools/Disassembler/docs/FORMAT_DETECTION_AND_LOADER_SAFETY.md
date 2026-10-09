# DOS format detection and loader safety

This module is **signature first** and deliberately separate from decoded
instruction validity. Recognized front-of-file markers: MZ, FBOV, TPOV,
AESOP/16, GFFI. A structurally plausible MZ `e_lfanew` pointer can identify
LE/LX/NE/PE signatures, but a matching two-byte marker is not a validated
loader map. Existing LE loader success carries stronger, validated evidence
and takes precedence. File extensions play no role in this classification.

Unknown or headerless data must remain raw until an analyst explicitly
supplies COM/driver loader semantics including CPU mode, segment base,
entry offset and relocation assumptions. No flat x86 fallback for FBOV/TPOV,
LX, AESOP or GFF. No game-specific signatures or hardcoded filenames exist
in the detector.

Targets for future independent fixtures: MZ/EXE, FBOV and TPOV Gold Box
overlays, explicit COM and DRV, LX sound drivers, AESOP VM resources,
and GFF containers. This source checkpoint is not a parser implementation
for the latter formats, nor an independently executed runtime test.

## Manual headerless selection

A source identified only by a `.COM` or `.DRV` extension remains unknown.
The analyst can explicitly choose the `com` / `com-disasm` CLI mode and
set the 16-bit origin. The source must fit the declared segment without
wrapping; recognized executable/container signatures cannot be overridden
silently. This is **not** a general driver loader or runtime emulator.

## Nested MZ-bound module detection

Some DOS protected-mode applications include an outer DOS MZ launcher
followed by a second, independently bounded MZ with its own
`e_lfanew` and LE/LX/NE/PE signature. DOSDis now scans for that **generic
structural relationship**. Each hit retains the original physical
extended-header location and is labeled `le-validation-required`
or the corresponding format-loader requirement. Multiple hits are
classified as ambiguous; no arbitrary module is selected.

The standalone `DosMzImage` loader refuses these recognized
MZ-bound extended modules instead of silently decoding the outer
launcher as the complete executable. The outer DOS stub may still
contain genuine 16-bit code, but it requires an explicitly scoped
DOS-stub analysis mode separate from protected-mode module analysis.
No game name, source hash or filename enters signature detection.

The independently archived Strahd CODE.1 and CODE.2 original byte
samples both expose a nested MZ at physical `0x25D24`, whose
`e_lfanew=0x1EB8` points to LE header `0x27BDC`. These examples are
**external regression facts only**, not special loader rules.

The original collection branch can be indexed by the GitHub connector.
This environment's current connector will not return arbitrary
binary file bytes (non-UTF-8 decoding restriction), and the container
does not have network DNS access. This limits the independent original
file audits currently possible here. No claim that those files are
absent from GitHub is implied. Raw original binaries are not committed
to the source branch.
