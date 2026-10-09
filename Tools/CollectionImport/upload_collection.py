#!/usr/bin/env python3
"""Import the audited D&D game archives into RLOFT/DnD-Classics/MASTER.

Read-only toward source archives. Git writes occur only with --apply.
Requires Python 3.10+, Git, and Git LFS for large files.
"""
from __future__ import annotations

import argparse
import bisect
import contextlib
import datetime as dt
import hashlib
import io
import json
import os
from pathlib import Path, PurePosixPath
import shutil
import subprocess
import sys
import zipfile

BRANCH = "DnD-Classics"
REPOSITORY = "Hyperbog/RLOFT"
ARCHIVES = {
    "D&D Games 1.zip": "5c345624038de91ca3590850dbb70607c3c60d20641cacbba5a861be172111ef",
    "D&D Games 2.zip": "5ab6c306be598cd267f349914cc4554964640bcd3600c77de24d24906c3393e4",
    "Menzoberranzan.zip": "b98a60e7e743669ed37f03b14be1d83b1ab01de7a20f5056469051a4d7c5df1e",
    "Ravenloft - Stone Prophet.zip.001": "17e2b3c8456b930e231ad03933c3a1c66863ae96d3972068321fc2584121426a",
    "Ravenloft - Stone Prophet.zip.002": "dfcf23d19ec1a215b350883fccd0db1425f604ea077b1ca9e04d965b02467132",
    "Ravenloft - Stone Prophet.zip.003": "26bc49ef03190b00c68543a7b7e98b1a07d8192affb276056a06841be23e06ee",
}
GAMES_BY_ARCHIVE = {
    "D&D Games 1.zip": (
        "Curse of the Azure Bonds", "Dark Sun", "Dark Sun 2", "Dungeon Hack",
        "Gateway to the Savage Frontier", "Hillsfar", "Pool of Radiance",
    ),
    "D&D Games 2.zip": (
        "Pools of Darkness", "Secret of the Silver Blades",
        "Treasures of the Savage Frontier", "Unlimited Adventures",
    ),
    "Menzoberranzan.zip": ("Menzoberranzan",),
    "Ravenloft - Stone Prophet.zip.001": ("Ravenloft - Stone Prophet",),
}

class ConcatReader(io.RawIOBase):
    """Seekable read-only view of consecutively split archive volumes."""
    def __init__(self, paths):
        self._files = [p.open("rb") for p in paths]
        self._ends = [0]
        for p in paths:
            self._ends.append(self._ends[-1] + p.stat().st_size)
        self._position = 0

    def readable(self):
        return True

    def seekable(self):
        return True

    def tell(self):
        return self._position

    def seek(self, offset, whence=os.SEEK_SET):
        if whence == os.SEEK_SET:
            position = offset
        elif whence == os.SEEK_CUR:
            position = self._position + offset
        elif whence == os.SEEK_END:
            position = self._ends[-1] + offset
        else:
            raise ValueError("Invalid seek mode")
        if position < 0:
            raise ValueError("Negative seek")
        self._position = position
        return position

    def read(self, length=-1):
        if length is None or length < 0:
            length = self._ends[-1] - self._position
        remaining = max(0, min(length, self._ends[-1] - self._position))
        chunks = []
        while remaining:
            index = bisect.bisect_right(self._ends, self._position) - 1
            size = min(remaining, self._ends[index + 1] - self._position)
            reader = self._files[index]
            reader.seek(self._position - self._ends[index])
            piece = reader.read(size)
            if not piece:
                raise EOFError("Archive segment unexpectedly ended")
            chunks.append(piece)
            self._position += len(piece)
            remaining -= len(piece)
        return b"".join(chunks)

    def close(self):
        if not self.closed:
            for reader in self._files:
                reader.close()
        super().close()


def run(*args, cwd=None, capture=True):
    r = subprocess.run(args, cwd=cwd, check=True, text=True, capture_output=capture)
    return r.stdout.strip() if capture else ""


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as fp:
        while block := fp.read(2**20):
            digest.update(block)
    return digest.hexdigest()


def safe_member(name: str, allowed: set[str]) -> PurePosixPath:
    """Reject traversal, alternate separators, cross-game paths and Git metadata."""
    if "\\" in name or "\x00" in name or name.startswith("/"):
        raise ValueError(f"Unsafe ZIP name: {name!r}")
    parts = PurePosixPath(name).parts
    if len(parts) < 2 or any(p in ("", ".", "..", ".git") for p in parts):
        raise ValueError(f"Unsafe ZIP path: {name!r}")
    if parts[0] not in allowed:
        raise ValueError(f"Archive has unexpected top-level folder: {name!r}")
    return PurePosixPath(*parts)


def open_source(paths: list[Path]):
    if len(paths) == 1:
        return paths[0].open("rb")
    return ConcatReader(paths)


def check_repository(repo: Path):
    if not (repo / ".git").exists():
        raise ValueError("--repo-root must point to a Git clone (not a downloaded ZIP)")
    if run("git", "branch", "--show-current", cwd=repo) != BRANCH:
        raise ValueError(f"Check out {BRANCH} before importing")
    if run("git", "status", "--porcelain", cwd=repo):
        raise ValueError("Git worktree must be clean before --apply")
    remote = run("git", "remote", "get-url", "origin", cwd=repo).lower()
    if "hyperbog/rloft" not in remote:
        raise ValueError(f"Unexpected Git origin: {remote!r}")
    for game in {g for games in GAMES_BY_ARCHIVE.values() for g in games}:
        if not (repo / "MASTER" / game / "README.md").is_file():
            raise ValueError(f"Missing expected folder: MASTER/{game}")


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--source-dir", required=True, type=Path, help="Directory holding all six original ZIP/ZIP-part uploads")
    p.add_argument("--repo-root", required=True, type=Path, help="Your local clone of Hyperbog/RLOFT")
    p.add_argument("--apply", action="store_true", help="Extract to MASTER and make commits (default: inspect only)")
    p.add_argument("--push", action="store_true", help="Push completed commits to origin/DnD-Classics; requires --apply")
    p.add_argument("--batch-size", type=int, default=80, help="Maximum imported regular files per commit (1 to 100)")
    p.add_argument("--lfs-threshold-mb", type=int, default=90, help="Use Git LFS for files of at least this many MiB")
    args = p.parse_args()
    if not 1 <= args.batch_size <= 100:
        p.error("--batch-size must be between 1 and 100")
    if args.push and not args.apply:
        p.error("--push requires --apply")
    source = args.source_dir.expanduser().resolve()
    repo = args.repo_root.expanduser().resolve()
    if args.apply:
        check_repository(repo)
    for filename, expected in ARCHIVES.items():
        path = source / filename
        if not path.is_file():
            raise FileNotFoundError(f"Missing source archive: {path}")
        actual = sha256(path)
        if actual != expected:
            raise ValueError(f"SHA-256 mismatch for {filename}: {actual} (expected {expected})")
        print(f"Verified archive: {filename} ({path.stat().st_size:,} bytes)", flush=True)

    sources = [
        ("D&D Games 1.zip", [source / "D&D Games 1.zip"]),
        ("D&D Games 2.zip", [source / "D&D Games 2.zip"]),
        ("Menzoberranzan.zip", [source / "Menzoberranzan.zip"]),
        ("Ravenloft - Stone Prophet.zip.001", [source / f"Ravenloft - Stone Prophet.zip.{j:03}" for j in range(1, 4)]),
    ]
    entries = []
    seen = set()
    high_files = []
    for identifier, parts in sources:
        with open_source(parts) as handle, zipfile.ZipFile(handle) as z:
            for info in z.infolist():
                if info.is_dir():
                    continue
                path = safe_member(info.filename, set(GAMES_BY_ARCHIVE[identifier]))
                key = str(path).casefold()
                if key in seen:
                    raise ValueError(f"Duplicate/case-colliding ZIP entry: {path}")
                seen.add(key)
                if info.is_dir():
                    continue
                if info.flag_bits & 1:
                    raise ValueError(f"Encrypted ZIP member unsupported: {path}")
                if (info.external_attr >> 16) & 0o170000 == 0o120000:
                    raise ValueError(f"Symbolic link ZIP member unsupported: {path}")
                entries.append((identifier, str(path), info.file_size))
                if info.file_size >= args.lfs_threshold_mb * 1024 * 1024:
                    high_files.append(str(path))
    from collections import Counter
    counts = Counter(name.split("/", 1)[0] for _, name, _ in entries)
    print(f"Found {len(entries):,} regular files across {len(counts)} games; {len(high_files)} require Git LFS")
    for game in sorted(counts):
        print(f"  {game}: {counts[game]} files")
    print("Large files:", *(f"MASTER/{name}" for name in high_files), sep="\n  ")
    if not args.apply:
        print("Dry run complete. No source or repository files changed.")
        return

    if high_files:
        try:
            run("git", "lfs", "version", cwd=repo)
        except (subprocess.CalledProcessError, FileNotFoundError):
            raise RuntimeError("Git LFS is required. Install git-lfs and run 'git lfs install' first.")
        run("git", "lfs", "install", "--local", cwd=repo, capture=False)
        for path in high_files:
            run("git", "lfs", "track", "--filename", "MASTER/" + path, cwd=repo)
            attr = run("git", "check-attr", "filter", "--", "MASTER/" + path, cwd=repo)
            if not attr.endswith(": lfs"):
                raise RuntimeError(f"Git LFS tracking failed: {path}: {attr}")
    pending = []
    commit_number = 0
    per_game = {game: [] for game in counts}

    def commit_batch(force=False):
        nonlocal commit_number
        if not pending and not force:
            return
        changed = list(pending)
        pending.clear()
        if force and not changed:
            return
        run("git", "add", "--", *changed, cwd=repo)
        if commit_number == 0 and (repo / ".gitattributes").exists():
            run("git", "add", "--", ".gitattributes", cwd=repo)
        run("git", "commit", "-m", f"Import D&D Classics game files (batch {commit_number + 1})", cwd=repo, capture=False)
        commit_number += 1
        print(f"Committed batch {commit_number}, {len(changed)} import files", flush=True)

    for identifier, parts in sources:
        with open_source(parts) as handle, zipfile.ZipFile(handle) as z:
            for info in z.infolist():
                if info.is_dir():
                    continue
                rel = safe_member(info.filename, set(GAMES_BY_ARCHIVE[identifier]))
                path = repo / "MASTER" / Path(*rel.parts)
                if path.exists():
                    raise FileExistsError(f"Refusing to replace an existing destination file: {path}")
                path.parent.mkdir(parents=True, exist_ok=True)
                temp = path.with_name(path.name + ".dnimport-partial")
                digest = hashlib.sha256()
                try:
                    with z.open(info) as src, temp.open("xb") as dst:
                        while block := src.read(2**20):
                            digest.update(block)
                            dst.write(block)
                    os.replace(temp, path)
                finally:
                    temp.unlink(missing_ok=True)
                game = rel.parts[0]
                per_game[game].append({
                    "path": str(PurePosixPath(*rel.parts[1:])),
                    "size": info.file_size,
                    "crc32": f"{info.CRC:08x}",
                    "sha256": digest.hexdigest(),
                    "source_archive": identifier,
                })
                pending.append(str(PurePosixPath("MASTER") / rel))
                if len(pending) >= args.batch_size:
                    commit_batch()

    for game, files in sorted(per_game.items()):
        path = repo / "MASTER" / game / ".source-manifest.json"
        if path.exists():
            raise FileExistsError(f"Manifest already exists: {path}")
        data = {"game": game, "source_sha256": ARCHIVES,
                "source_note": "All source archives SHA-256 verified before import. Filenames retain archive-relative paths.",
                "files": files}
        path.write_text(json.dumps(data, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
        pending.append(str(PurePosixPath("MASTER") / game / ".source-manifest.json"))
        if len(pending) >= args.batch_size:
            commit_batch()
    commit_batch()
    print(f"Imported {len(entries):,} original files into {len(counts)} game folders in {commit_number} commits.")
    if args.push:
        run("git", "push", "origin", BRANCH, cwd=repo, capture=False)
        print(f"Pushed {BRANCH} successfully.")
    else:
        print(f"Commits are local only. Review, then run: git push origin {BRANCH}")


if __name__ == "__main__":
    try:
        main()
    except (OSError, ValueError, zipfile.BadZipFile, subprocess.CalledProcessError, RuntimeError) as exc:
        print(f"ERROR: {exc}", file=sys.stderr)
        sys.exit(1)
