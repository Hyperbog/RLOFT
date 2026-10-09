# Game-agnostic disassembler architecture contract

**Normative project rule — applies to every future change.**

The disassembler targets executable **formats and architectures**, never a
specific game's behavior. Supported games are examples of input data, not
switch statements or sources of hardcoded addresses. Future research targets
include the Dark Sun series, Krynn series, Pool of Radiance/Gold Box series,
Dungeon Hack, Ravenloft, Menzo, Stone Prophet and other DOS titles.

## Strict separation

- **Parser/decoder:** format- and CPU-dependent mechanics (MZ, LE/LX, COM,
  future 16-bit DOS and other formats). Detection uses binary structure,
  not title or a list of known original hashes. Unsupported input is reported
  explicitly, never guessed as a supported executable.
- **Static analyzer:** format-supported object mapping, relocations,
  reachability, indirect calls/jumps, tables, references, uncertainties and
  unknown gaps. No instructions, bounds or code/data labels may be seeded
  automatically merely because a game-specific answer is known.
- **Optional identity catalog:** separate versioned JSON with complete
  SHA-256, byte length, game ID, module and independently supported metadata.
  The catalog may label an exact original, but is not part of analyzer logic.
  A match means matching analyst-provided catalog data, not runtime proof.
- **Research manifests:** separate SHA/size-gated game-specific reference
  expectations for comparison only. An independent benchmark can report
  mismatches without changing the analyzer's guesses. Never carry Strahd
  labels into Stone, Menzo, Dark Sun, Krynn or any other executable.
- **Project data:** key analyst, inferred and evidence-backed names by full
  original hash, game identity, object location and role. Never merge labels
  by basename, sequence similarity, or assumed engine compatibility.

## Catalog behavior and privacy

The core ships with **no game identities hardcoded**. Example catalogs may
include specifically verified fingerprints as external test fixtures.
A catalog is opt-in; absent/unknown files remain unassigned, and unknown
hashes still work with the supported format parser. The user can supply a
game ID without a catalog, but it must remain analyst-declared/unverified.
Archive and internal paths are opt-in, relative metadata; no automatic local
host paths, proprietary bytes or game archives are exported.

Versioned identity catalog schema v1: `schemaVersion: 1`, `entries: []`,
each entry supplies `sha256` (64 hex), `byteLength`, `gameId` (slug),
`module`, `sourceEvidence`, plus optional display metadata.
Duplicates, malformed records, conflicting lengths and oversized files fail.

## Acceptance tests and roadmap

1. The same synthetic LE image must produce exactly identical candidate
   graphs with or without any identity catalog, regardless of its game label.
2. Two archives both named `CODE.1` must remain distinct by source bytes and
   research namespace; no names or code interpretation may transfer.
3. Unsupported DOS formats should receive raw-byte support with explicit
   format limitations; future MZ/COM/16-bit support uses new loaders, not a
   Strahd-specific fallback.
4. Known-answer manifests are compared after analysis, never used as implicit
   reachability seeds or table bounds.
5. Continue universal indirect call/jump recovery, LE metadata correctness,
   uncertainty accounting, machine-readable regression diffs and extensible
   format plugins **before** UI polish.

Windows-only delivery remains the current release target. Build and test
execution remain manual, only when the user explicitly requests them.
AI assistance is postponed until after the initial final release.

## File-format roadmap from the wider DOS game collection

P0 includes standalone 16-bit MZ executables, conservative FBOV/TPOV
container mapping, explicit headerless COM assumptions, and automatic
signature-first detection. LX/NE/driver loaders and packed executable
recognition follow as P1; AESOP and GFF/DAX bytecode/resource engines as P2.
The core parser must never special-case game titles. See
[format matrix](FORMAT_SUPPORT_MATRIX.md).
