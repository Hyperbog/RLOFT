# P0 game-specific source provenance and identity namespaces

Status: source-only, no compilation, no test execution.

A CODE.1 basename is not an artifact identity. SourceProvenance contains a
complete SHA-256, byte length (nullable only when the source artifact was not
provided), game ID, module label, format, identity evidence status, research
namespace, and opt-in relative archive/internal paths.

Known Strahd CODE.1 and CODE.2 complete hash fingerprints assign verified
game identity and prevent a conflicting CLI game claim. Other games require
an explicit analyst-supplied game ID and remain "analyst-declared-game-unverified"
until an independent source manifest is confirmed. Unknown games default to
"unassigned", NOT to Strahd. Namespaces are always game-id/source-sha256,
so two CODE.1 files from Stone and Menzo cannot share research identity.

CLI usage:

```powershell
DosDis.Cli.exe report CODE.1 --out menzo-report.json --game-id menzo --module CODE.1 --source-archive Menzoberranzan.zip --internal-path Menzoberranzan/CODE.1
```

Archive and internal paths are optional analyst declarations and never
inferred from local absolute filesystem paths. The original bytes are not
embedded by default. Graph-only report API calls retain a truthful fallback
source-only/unassigned status. The old 'source.sha256' field is retained for
reader compatibility while the new 'provenance' object gives the full context.


## Persistent named symbols (project schema V2)

SQLite `.ddproj` V1 research projects migrate in-place to schema V2. Original
analysis snapshots and legacy `annotations` rows remain intact. New
`source_identities` rows bind one complete SHA-256 to one game/module label;
`named_symbols` uses a composite game ID + original hash + object/offset +
name role key. A name can exist independently as `analyst`, `inferred`
or `verified` (the latter requires an explicit evidence ID; this does not
mean the application has independently validated the claim).
Source identity conflicts and simultaneous revisions are rejected. No
automatic Strahd-to-Stone or Menzo symbol copying. Existing legacy labels
are **not** promoted into the new verified tier.

The schema migration and symbol isolation have synthetic source tests but
**the tests have not been run**. UI editing for the new tiers remains future
work. Back up an important V1 project before opening it in a V2 executable.
