# D&D Classics: Original Game Collection Import

This importer prepares the 13 existing `MASTER/<game>/` folders on the **private** `DnD-Classics` branch of `Hyperbog/RLOFT` for reverse-engineering and archival research.

The current environment can read the uploaded source archives and write repository text through a GitHub integration, but it cannot transmit the original binary payloads to GitHub. The bundled Python importer performs the full import on your own authenticated computer using normal Git. The 100-file browser limit does not apply to Git commits; the importer nevertheless batches up to **80 original files per commit**.

## Requirements

- Python 3.10 or later
- Git and Git LFS (`git lfs install`)
- A local clone of `https://github.com/Hyperbog/RLOFT.git`, checked out on branch `DnD-Classics`
- The six unmodified archive files from this project's upload session in one directory
- Roughly 3 GB of available working space, plus Git/Git LFS history and cache capacity
- Permission to store and share these proprietary game files in the chosen private repository

## Usage

Place the following originals together in a local directory (for example `D:\DnD-Archives`):

```text
D&D Games 1.zip
D&D Games 2.zip
Menzoberranzan.zip
Ravenloft - Stone Prophet.zip.001
Ravenloft - Stone Prophet.zip.002
Ravenloft - Stone Prophet.zip.003
```

Clone and prepare the destination:

```powershell
git clone https://github.com/Hyperbog/RLOFT.git
cd RLOFT
git switch DnD-Classics
git lfs install
```

Use the importer (in the cloned repo as `Tools/CollectionImport/upload_collection.py`):

```powershell
# Read-only validation and directory preview
python Tools/CollectionImport/upload_collection.py --source-dir "D:\DnD-Archives" --repo-root .

# Import to MASTER, commit in 80-file batches, then push to the private branch
python Tools/CollectionImport/upload_collection.py --source-dir "D:\DnD-Archives" --repo-root . --apply --push
```

**Safety controls:** The script verifies all six source archive SHA-256 values; rejects ZIP path traversal and cross-game paths; refuses to overwrite existing files; preserves original file bytes and relative directories; recognizes the three Stone Prophet archive segments; uses Git LFS for files >= 90 MiB; writes game-local `.source-manifest.json` records; requires a clean checkout of the correct branch and an `origin` pointing to `Hyperbog/RLOFT`; and never changes the source archives. It also checks ZIP CRCs during extraction.

**Operational caveats:** GitHub LFS can incur quota/storage charges. Check available LFS capacity before pushing. The importer is deliberately single-run; if interrupted mid-run, inspect the worktree and completed commits before resuming. The script does not upload Strahd's separate research files. GitHub's API may expose only LFS pointers to downstream tools; the original bytes may still need an authenticated `git lfs pull` for local analysis.

## Scope

The complete verified collection contains **1,533 regular files** across **13 game folders**. Four large files require Git LFS: `MASTER/Dark Sun 2/game.gog`, `MASTER/Menzoberranzan/game.gog`, `MASTER/Ravenloft - Stone Prophet/gog.game`, and `MASTER/Ravenloft - Stone Prophet/gog.zip`.

The resulting private-repository contents are *not* a substitute for the original ZIP collection and its archived checksums. Retain those originals separately for preservation.
