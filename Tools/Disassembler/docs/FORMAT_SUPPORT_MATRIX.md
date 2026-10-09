# Universal format support matrix and collection-derived regression order

The collection observations below are **reported header observations**,
not independently reverified by this source-only checkpoint.

| Format family | Example from collection | Implementation status | Correct next gate |
| --- | --- | --- | --- |
| DOS MZ `.EXE` | Hillsfar `MAIN.EXE`; Dark Sun `DSUN.EXE` | Original-byte-backed x86-16 CFG, non-destructive MZ relocation simulation, exact-fixup far candidate annotations, bounded cross-segment review | Real-file validation, dynamic far/indirect flow, alias uncertainty and DOS runtime load semantics |
| MZ-bound LE | `CODE.1`, `CODE.2` | Validated object/page mapping and x86-32 candidates in source | Fixup coverage, indirect-flow completeness |
| FBOV | Pool of Radiance `GAME.OVR` | Envelope, 8-byte variant paired resident directory, opaque fixup audit, explicit source-bound region x86-16 CFG | Confirm original layout, runtime segment and relocation encodings; 16-byte variant remains envelope-only |
| TPOV | Secret of the Silver Blades `GAME.OVR` | Envelope, paired resident MZ directory, opaque fixup audit, explicit source-bound region x86-16 CFG | Verify real files, fixup encoding and runtime overlay mapping |
| Headerless COM | Pools of Darkness `SOUND.COM` | Explicit 16-bit origin, requested decode, and bounded same-segment candidate CFG; never auto-detected from filename | Real DOS PSP/runtime assumptions, interrupt/driver and callback analysis |
| Headerless/structured DRV | Pools of Darkness `VGA.DRV` | Unknown/headerless by default | Driver calling/initialization conventions and detection |
| MZ-bound LX | `A32ADLIB.DLL`, other AIL/32 drivers | Signature recognized only | LX object/page/rebase/entry mapping |
| NE / PE | Other MZ-bound binaries | Signature recognized only | Dedicated NE/PE metadata loaders and host-platform distinction |
| AESOP/16 | Dungeon Hack `HACK.RES` | Resource signature recognized only | Container records + separate AESOP VM decoder |
| GFFI | Dark Sun `RESOURCE.GFF` | Resource signature recognized only | GFF directory, data references, scripts |
| DAX / Gold Box events | Gold Box resources | Unknown | Data container and interpreter-specific event decoder |
| Unlimited Adventures data | `.DAT`, `.TLB`, `.GLB` | Unknown | Proven resource/event schema, never automatic x86 |

## Source-driven priority

**P0:** reliable 16-bit MZ load semantics and seeded CFG; FBOV/TPOV
overlay parsing; explicit COM mode; signature detection and archive identity.
Continue universal indirect CALL/JMP analysis and unknown-gap reporting
in parallel.

**P1:** LX and NE loader details, DOS driver interfaces, packed and embedded
executable identification, entry point/rebase reconstruction.

**P2:** AESOP/16 interpreter disassembly, GFF/DAX and other data/bytecode
engines; read-only archive and disk-image inventory.

This list expresses **format support**, not hardcoded per-game behavior.
Many games share formats; one game may supply multiple executable and VM
container types. Game-specific SHA-256 known-answer fixtures must remain
external to the decoding engine.

## First original-file regression set, once manual testing is authorized

1. Hillsfar `MAIN.EXE` — standalone DOS MZ metadata/16-bit decode.
2. Pool of Radiance `START.EXE` — DOS MZ loader and relocation mapping.
3. Pool of Radiance `GAME.OVR` — FBOV container map and segment mapping.
4. Dark Sun `DSUN.EXE` — DOS MZ structural differences.
5. Menzo `A32ADLIB.DLL` — MZ-bound LX audio interface/loader.
6. Secret of the Silver Blades `GAME.OVR` — TPOV overlay variant.
7. Pools of Darkness `SOUND.COM` and `VGA.DRV` — explicit headerless contexts.
8. Dungeon Hack `HACK.RES` — AESOP VM instruction format.
9. Dark Sun `RESOURCE.GFF` — GFF resource mapping.

Exclude bundled modern Windows DOSBox executables, installers, uninstallers,
SDL libraries and GOG configuration helpers from the original-game benchmark;
they can be cataloged separately as supporting software.

**Source checkpoint only:** no Windows build, no original files processed and
no regression suite executed here.

## Explicit headerless COM research mode

`com` and `com-disasm` CLI modes require the analyst to opt into a
single-segment 16-bit load layout. The conventional default origin is
`0x100`, explicitly overridable with `--origin`; `--offset` selects a
requested instruction address. No PSP bytes are inferred and no DOS runtime
segment is invented. A conflicting recognized MZ/LX/FBOV/TPOV/resource
signature is rejected even in explicit COM mode. An ordinary `inspect`
operation still reports such a file as `unknown-headerless` unless its
structure is independently identified.

MZ and COM use the same `X86RealModeDecoder` on bounded original bytes.
This is **linear, analyst-requested candidate decoding**; a 16-bit recursive
CFG and runtime relocation emulation remain future work.

## Paired overlay recovery caveat

`overlay-map GAME.OVR --resident START.EXE` is a general-purpose, paired
Turbo Pascal overlay directory check, **not** a loader for every file
with an OVR extension. It requires a validated source MZ and an exact,
unique resident descriptor chain covering the overlay payload, with at
least one checked procedure trap stub. The 16-byte FBOV variant uses a
different manager and remains envelope-only. Stored code/fixup extents
are not evidence of runtime-segment assignments or fully resolved calls.

## Universal 16-bit recursive CFG (source-only)

CLI `mz-analyze`, `com-analyze`, and `overlay-analyze` now share a
single analyzer. They use original file offsets and a **declared segment
window**; only reachable near branches/calls within that validated window
receive candidate destination edges. Far/indirect transfers, DOS interrupts,
unbacked destinations, rejected overlaps and instruction-budget truncation
receive separate evidence/diagnostic statuses. The original bytes, code/data
uncertainty and independent file identities remain intact.

`overlay-fixups GAME.OVR --resident START.EXE` returns source-authenticated,
separate **code span and opaque fixup span fingerprints**, never guessed
relocation values. It first requires a complete resident directory.
The paired overlay code-map analysis still requires a user- or research-
provided, source-hash-bound region manifest with assumed segment/IP;
the runtime loader's final segment values remain unknown.

**All new code and fixtures remain unbuilt and unexecuted.**

## MZ segment relocations and cross-segment candidates

P0 now includes deterministic modeling of MZ fixup additions to a private
copy of the stored DOS load module. The analyst-supplied load segment is a
hypothesis, not a measured runtime location. Full SHA-256 and original file
offsets are preserved; repeated or overlapping fixups remain visible.

The 16-bit analyzer can identify relocation-supported unprefixed immediate
far CALL/JMP **candidates**, then perform independent bounded source-backed
traversals at those module-relative CS:IP addresses. It does not claim
verification of those branches, merge segment aliases, decode opaque OVR
fixup blobs, or reconstruct far-indirect calls.

No original Dark Sun, Gold Box, Dungeon Hack, or other game receives special
decoder logic. No Windows compilation or original-file replay has occurred.

## Generic x86-16 indirect CALL/JMP candidate recovery (P0 source)

Explicit-CS displacement-only near memory pointers now produce
source-backed **candidate** destinations, not proven runtime edges. Far
pointers require exact MZ selector fixups and a user-supplied load-base
simulation. Indexed CS indirect CALL/JMP sites can be paired with optional,
SHA-256-bound manifests specifying table storage, entry count and evidence.
Complete source backing of all table entries is enforced; runtime index
bounds, data mutation and code reachability are **not** established.

Other 16-bit addressing forms remain unresolved; the app does not
special-case Gold Box, Dark Sun, Krynn, Pools, Ravenloft or any title.
A separate bounded dataflow pass remains necessary for automatic indexed
table recovery, and authentic executable/overlay regression remains pending.
