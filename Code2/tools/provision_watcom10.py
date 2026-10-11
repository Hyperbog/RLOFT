#!/usr/bin/env python3
"""Verify and optionally safely extract a locally supplied Watcom 10.0a archive.

Does NOT download, execute, trust, or publish the compiler. Only a pinned
archive/version is accepted. Windows-friendly, Python standard library only.
"""
from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path, PurePosixPath
import re
import tarfile

ARCHIVE_NAME = 'wcc10.0a.tar.gz'
ARCHIVE_SIZE = 9_247_235
ARCHIVE_SHA256 = '920a9674d1b10f227ae65171c187212b06d7fc38b263313922615c4f5e9b24a6'
SOURCE_URL = 'https://github.com/decompme/compilers/releases/download/compilers/wcc10.0a.tar.gz'
MAX_FILES = 8192
MAX_UNPACKED = 512 * 1024 * 1024


class ProvisionError(ValueError):
    pass


def verify_archive(path: Path) -> dict:
    if not path.is_file():
        raise ProvisionError('compiler archive must be a local regular file')
    if path.stat().st_size != ARCHIVE_SIZE:
        raise ProvisionError('Watcom 10.0a archive size mismatch')
    with path.open('rb') as stream:
        digest = hashlib.file_digest(stream, 'sha256').hexdigest()
    if digest != ARCHIVE_SHA256:
        raise ProvisionError('Watcom 10.0a archive SHA-256 mismatch')
    return {'version': 'Watcom C 10.0a', 'size': ARCHIVE_SIZE,
            'sha256': digest, 'source': SOURCE_URL,
            'compiler_executed': False, 'watcom_match_verified': False}


def safe_members(tf: tarfile.TarFile) -> list[tarfile.TarInfo]:
    members = tf.getmembers()
    if len(members) > MAX_FILES:
        raise ProvisionError('archive member count exceeds safe extraction cap')
    total = 0
    seen: set[str] = set()
    for member in members:
        name = member.name
        # Never allow OS-specific drive paths, backslash escapes, or links.
        p = PurePosixPath(name)
        parts = p.parts
        if (not name or name.startswith(('/', '\\')) or '\\' in name
                or ':' in name or any(x in ('.', '..') for x in parts)
                or any(re.search(r'[<>|?*\x00-\x1f]', x) for x in parts)
                or not parts):
            raise ProvisionError(f'unsafe archive member path: {name!r}')
        key = '/'.join(parts).casefold()
        if key in seen:
            raise ProvisionError('duplicate or case-colliding archive path')
        seen.add(key)
        if not (member.isdir() or member.isfile()):
            raise ProvisionError('links and special archive members are not allowed')
        if member.isfile():
            total += member.size
            if member.size < 0 or total > MAX_UNPACKED:
                raise ProvisionError('archive exceeds unpacked size cap')
    return members


def extract_verified(path: Path, destination: Path, *, verified: dict) -> dict:
    if verified.get('sha256') != ARCHIVE_SHA256:
        raise ProvisionError('archive must be authenticated before extraction')
    if destination.exists():
        raise FileExistsError('refusing to overwrite existing compiler directory')
    with tarfile.open(path, 'r:gz') as tf:
        entries = safe_members(tf)
        # Nothing is created until the complete member table is vetted.
        destination.mkdir(parents=True, exist_ok=False)
        for entry in entries:
            out = destination.joinpath(*PurePosixPath(entry.name).parts)
            if entry.isdir():
                out.mkdir(parents=True, exist_ok=True)
                continue
            out.parent.mkdir(parents=True, exist_ok=True)
            inp = tf.extractfile(entry)
            if inp is None:
                raise ProvisionError('archive file has no readable payload')
            with inp, out.open('xb') as dest:
                remaining = entry.size
                while remaining:
                    chunk = inp.read(min(65536, remaining))
                    if not chunk:
                        raise ProvisionError('truncated archive data stream')
                    dest.write(chunk)
                    remaining -= len(chunk)
    return {**verified, 'extracted_directory': str(destination),
            'members': len(entries), 'compiler_executed': False}


def main() -> None:
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('archive', type=Path, help=f'Locally downloaded {ARCHIVE_NAME}')
    p.add_argument('--extract-to', type=Path, help='New, private target directory')
    p.add_argument('--report', type=Path, help='Metadata-only JSON report')
    args = p.parse_args()
    verified = verify_archive(args.archive)
    report = (extract_verified(args.archive, args.extract_to, verified=verified)
              if args.extract_to else verified)
    out = json.dumps(report, indent=2, sort_keys=True) + '\n'
    if args.report:
        args.report.write_text(out, encoding='utf-8')
    else:
        print(out, end='')


if __name__ == '__main__':
    main()
