# Original RES4 forest-ground selection — source-backed candidate audit

**Branch:** `demo-v1` · **Status:** original RES4 candidate inspector,
unit test, standalone Windows diagnostic program and artifact launcher
committed. **Current HEAD remains unbuilt.** GitHub Actions is manual-only.

## Context and immutable input

The original assets are preserved unchanged on the
[`DnD-Classics` branch](https://github.com/Hyperbog/RLOFT/tree/DnD-Classics/MASTER),
including original
[`RES4`](https://github.com/Hyperbog/RLOFT/blob/DnD-Classics/MASTER/Ravenloft%20-%20Strahd%27s%20Possession/RLOFT/RES4)
in the actual Strahd's Possession game directory.

The latest source disassembly establishes:

- Original `CODE.2 1F388..1F39D` takes signed level-header
  `+7=55` and **adds mutable `258F8`** before calling
  `2F29C` with resource-bank 4.
- `2F29C` accepts the resource only when the loaded resource
  type byte is exactly 1 and returns its decoded-body pointer.
- `34A74 34AA7..34AAF` adds **two bytes** to this decoded
  body before generating 64×64 texture samples from `34350`.
- The original 128×6-byte FF58 perspective generator, FE15
  shading and original 80/48 VGA raster are implemented in the
  preceding [ground pipeline checkpoint](
  ORIGINAL_CODE2_34A74_TEXTURED_GROUND_PIPELINE_20261008.md).
- The value 55 is therefore a **relative selector**, not a
  certified absolute RES4 ID. Do not set `RES4[55]` as live
  texture without establishing `258F8`.

Although the repository connection can inspect original RES4
metadata and SHA, its UTF-8 file decoding cannot return raw
binary bytes. The Win32 build's pre-existing
`GameSource::read("RES4")` has no such limitation: it reads the
user's original installed game or GOG CD image directly.

## New original-resource audit

`OriginalGroundRes4Audit::inspect` uses the existing
`ResArchive` index table and original EH type-1 decoder
to enumerate **all** genuinely decodable `count=1`,
`64×64`, `4096`-byte indexed candidate tiles from the
**original RES4**, reporting:

- Archive entry count, type-1 record count, malformed type-1 count.
- Absolute index, EH-1 entry byte size, original compression
  (raw `0` or LZSS `3`) and transform flags.
- Original transparency-index field and **FNV-1a 32-bit**
  decoded-pixel checksum. FNV is diagnostic only, **not**
  a cryptographic integrity proof.
- No resource is silently guessed from matching dimensions:
  multiple candidate entries are all reported unselected.

`absolute_index(relative, verified_base, count)` preserves
the original relative-plus-base selection, rejects overflow/
out-of-range entries and **requires a proven runtime base
provided by the caller**.

`verified_tile_source` reuses the exact decoded original
4096 pixel bytes, checking dimensions before allowing them
into the separate 34A74 raster core. A two-byte address spacer
in the host buffer corresponds to CODE.2's `C040+2`
pointer adjustment and is **never sampled or treated as
original art**. No texture content is fabricated.

## New manual Windows tool in the artifact

The build now additionally compiles
`rloft_res4_ground_probe.exe` and packages both it and
`Probe-Level22-Ground.cmd` with the existing manual
Windows demo artifact.

After manually dispatching a new `demo-v1` Actions run
and extracting the resulting Windows artifact into a
fresh folder:

```bat
Probe-Level22-Ground.cmd
```

It automatically discovers an installed original
Ravenloft source using the same `GameSource` resolver as
the native game. Alternatively, pass a quoted explicit
original game path or `game.gog`:

```bat
Probe-Level22-Ground.cmd "C:\Path\To\RLOFT"
```

The launcher prints all candidate lines and writes the
same report to `%TEMP%\RLOFT-Level22-RES4-Ground-<number>.txt`.
Only text inventory/count/checksum information is emitted;
**the original RES4 archive, tile bytes, party/save state
and DOS executable are not exported or modified**.

Please preserve that report for source comparison against
a future `DGROUP:258F8` original fresh-start trace.
The reporter intentionally prints
`mutable_CODE2_base=258F8 (not yet resolved)`; this
string is an address, not a verified numeric base value.

## Regression and release boundary

Registered `original_ground_res4_audit_tests` builds
synthetic EH type-1 resources in an authentic RES offset
table. It checks a raw 64×64 source, an independently
LZSS-coded equivalent, non-ground 8×1 source, malformed
type-1 source, identical decoded sample fingerprints,
bounds-safe original relative selector resolution and
an actual one-row 34A74 indexed output from the decoded
texels. **No copyrighted source is embedded.**

Expected Windows CTest registrations: **164**, if
Python3 is present (previous source expected 163).
The last real successful manual Windows build remains
[#640](https://github.com/Hyperbog/RLOFT/actions/runs/37872583683)
with 160/160 tests at an older SHA. None of the current
ground code, tooling or regressions has passed MSVC CI yet.

**The live preview STILL draws flat foreground**; attaching
a source shape solely because its dimensions match would
not establish correct original runtime resource selection,
texture row-span `FF50`, dynamic `C026/C027` mode, or
original indexed-pixel equivalence.

## Next source-backed development

1. Build the current branch **manually**, Linux unchecked.
2. Run the RES4 probe on the installed originals; inspect
   candidates without assuming relative ID 55 is absolute.
3. Recover the fresh-game original numeric value of
   `DGROUP:258F8` from its runtime initialization path
   (CODE.2 16E90 and callers), with a DOS trace if necessary.
4. Resolve the original `RES4[55+base]` entry and compare
   the decoded 4096 indexed pixels to the verified candidate.
5. Reconstruct live `FF50` upper/lower clipping, mutable
   `C026/C027`, upper textured path and post-34A74 draw
   order; only then replace flat fill in live presentation.
6. Compare an aligned DOS freshly-created-party forest
   framebuffer and original palette to the new native pixels.

Formal demo acceptance remains **1/5 gates (20%)**;
whole-game completion is not a meaningful percentage yet.
