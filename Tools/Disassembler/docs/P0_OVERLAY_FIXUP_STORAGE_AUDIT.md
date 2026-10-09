# Opaque overlay fixup storage audit

**Source implementation only: Windows compilation and execution not performed.**

`DosOverlayFixupAudit.Analyze` consumes an original-source-backed overlay
envelope and a complete, unique resident-directory map. It enforces exact
overlay source hash, format, header and payload bounds; each unit's ordered
physical code/fixup file offsets; and total payload coverage.

The audit produces separately SHA-256-fingerprinted **stored code** and
**stored opaque fixup** spans for each unit. These fingerprints describe
original bytes, not relocated memory. It exposes zero-length spans explicitly
with null fingerprints; neither an original byte stream nor a decryption/
relocation value is emitted. Source EXE and OVR SHA-256 values remain distinct.

No interpretation of 16-bit fixup records, relocated selector values,
overlay loader state, or code reachability is attempted. A fixup blob is
**not an x86 instruction stream** and must not be fed to the CFG analyzer.
Once the actual overlay relocation records are acquired and their semantics
documented, a separate format-specific decoder can consume these spans
while preserving the exact original source and the opaque archival audit.

## CLI

```powershell
DosDis.Cli.exe overlay-fixups GAME.OVR --resident START.EXE
```

This command first validates the paired Turbo Pascal-style directory and
then emits per-unit original-source code and fixup SHA-256 digests,
file offsets and byte lengths. It does not decode or execute bytes. No
original game files are embedded in the report.
