# Turbo Pascal resident overlay directory recovery (source-only)

`DosOverlayDirectoryRecovery.Recover` consumes an independently validated
standalone DOS MZ **resident** executable and a separate TPOV or 8-byte-FBOV
overlay envelope. The overlay source alone does not contain enough evidence
to reconstruct its resident dispatch descriptors.

It scans the resident load-module bytes for the overlay-manager trap marker,
checks each 32-byte resident descriptor's overlay file offset, code and fixup
sizes, procedure count and referenced five-byte trap stubs, then requires
exactly one descriptor at **every successive overlay extent** from the
header to the end of the file. Missing, duplicate or mis-sized extents reject
the entire map. Mismatched executable/overlay source SHA-256 values are
retained separately; no file names or game IDs influence the result.

The returned offsets describe original **stored bytes**: resident descriptor,
overlay code, and overlay fixup blob. Fixup contents, runtime segment
assignment, procedure semantic identities, and executable reachability
remain **unverified**, and no automatic branch seeds are produced.
The distinct 16-byte FBOV variant is deliberately excluded because it
has a different overlay manager/directory contract.

The generic approach follows public descriptions of resident descriptor
recovery, independently adapted for the DOSDis codebase:
https://github.com/gg-sl-oss/binary-comp
See also the contrasting Borland FBOV variant:
https://github.com/NationalSecurityAgency/ghidra/issues/5543

Synthetic fixture source cases cover full chain acceptance and missing,
duplicate, truncated, and mis-typed descriptors. They are **not executed**.
No original game binaries were present on the DnD-Classics repository branch
for this specific source-only development step.

## CLI availability

```powershell
DosDis.Cli.exe overlay GAME.OVR
DosDis.Cli.exe overlay-map GAME.OVR --resident START.EXE
DosDis.Cli.exe overlay-disasm GAME.OVR --regions source-bound-map.json --region candidate-1 --count 32
```

The `overlay-map` response includes both source identities and candidate
stored unit extents. It does not export original bytes, automatically decode
overlay code, or execute an overlay manager. Paired resident MZs may be
compressed or use an unsupported overlay manager; such cases explicitly fail
rather than fabricating a directory. The runtime segment base is unknown.
