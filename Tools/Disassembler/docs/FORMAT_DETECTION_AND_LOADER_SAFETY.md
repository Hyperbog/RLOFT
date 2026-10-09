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
