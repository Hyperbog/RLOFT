#!/usr/bin/env python3
"""Read-only original-media manifest and Strahd analyzer baseline.

Run from the RLOFT repository root. No original bytes are written to outputs.
"""
from __future__ import annotations

import argparse
import csv
import hashlib
import json
import re
import subprocess
import sys
import zipfile
from collections import defaultdict
from pathlib import Path

TITLES = ("strahd", "menzo", "stone")
# Existing repository analyzers; do not alter or duplicate their parsers.
STRAHD_BASELINES = (
    ("INFO.DAT", "DatFiles/INFO.DAT/inspect_info.py", ("roundtrip=PASS",)),
    ("LEVELS.DAT", "DatFiles/LEVELS.DAT/analyze_event_scripts.py", ("levels=32", "physical_slots=1600")),
    ("NPCS.DAT", "DatFiles/NPCS.DAT/analyze_behavior_flags.py", ("records=83 record_size=0x45",)),
    ("OBJECTS.DAT", "DatFiles/OBJECTS.DAT/analyze_attack_profile.py", ("records=336", "PASS")),
)


def classify(filename: str) -> str:
    name = filename.upper()
    suffix = Path(name).suffix
    if re.fullmatch(r"RES\d+", name) or name == "T":
        return "resource archive"
    if name == "LEVELS.DAT" or re.fullmatch(r"LEV\d{3}\.DAT", name) or suffix == ".MAP":
        return "level"
    if name in {"SAVE.DAT", "CHARS"} or name.startswith("SAV") and suffix == ".DAT":
        return "save"
    if name in {"INFO.DAT", "INIT", "SOUND.CFG", "GAME.OPT", "PATCHES.INI"} or suffix in {".INI", ".CFG", ".CONF"}:
        return "config"
    if suffix in {".EXE", ".DLL", ".COM", ".BAT", ".CMD"} or name in {"CODE.1", "CODE.2"}:
        return "executable"
    if suffix in {".VOC", ".XMID", ".XMI", ".MID", ".BNK", ".AD", ".WAV", ".MUS"}:
        return "audio"
    if suffix in {".ANM", ".DFA", ".ACT", ".AVI", ".MP4"}:
        return "video"
    if suffix in {".TXT", ".NFO", ".MD", ".PDF", ".CSV"}:
        return "text"
    if suffix in {".ZIP", ".7Z", ".PAK"} or name in {"COLORS", "SINTAB"}:
        return "resource archive"
    if name in {"OBJECTS.DAT", "NPCS.DAT"} or suffix in {".DAT", ".BNK"}:
        return "resource archive"
    return "UNKNOWN"


def hash_stream(stream) -> str:
    digest = hashlib.sha256()
    for chunk in iter(lambda: stream.read(1024 * 1024), b""):
        digest.update(chunk)
    return digest.hexdigest()


def inventory(media: Path) -> list[dict]:
    result = []
    if media.is_dir():
        for path in sorted(media.rglob("*")):
            if not path.is_file():
                continue
            with path.open("rb") as stream:
                checksum = hash_stream(stream)
            rel = path.relative_to(media).as_posix()
            result.append({"filename": rel, "size": path.stat().st_size,
                           "sha256": checksum, "role": classify(path.name)})
    elif media.is_file() and zipfile.is_zipfile(media):
        with zipfile.ZipFile(media) as archive:
            for info in sorted(archive.infolist(), key=lambda item: item.filename):
                if info.is_dir():
                    continue
                with archive.open(info, "r") as stream:
                    checksum = hash_stream(stream)
                result.append({"filename": info.filename, "size": info.file_size,
                               "sha256": checksum, "role": classify(Path(info.filename).name)})
    else:
        raise ValueError("--media must be a directory or ZIP; 7z is not supported")
    return result


def comparison(manifests: list[dict]) -> dict:
    if sorted(x.get("title") for x in manifests) != sorted(TITLES):
        raise ValueError("comparison requires exactly one manifest per title")
    names = defaultdict(set)
    duplicates = defaultdict(list)
    for manifest in manifests:
        seen = defaultdict(int)
        for f in manifest["files"]:
            base = Path(f["filename"]).name.upper()
            names[base].add(manifest["title"])
            seen[base] += 1
        duplicates[manifest["title"]] = sorted(n for n, count in seen.items() if count > 1)
    by_count = {"all_three": [], "exactly_two": [], "exactly_one": []}
    for name, owners in sorted(names.items()):
        key = {3: "all_three", 2: "exactly_two", 1: "exactly_one"}[len(owners)]
        by_count[key].append({"filename": name, "titles": sorted(owners)})
    return {"match_rule": "case-insensitive basename presence, not content identity",
            "counts": {key: len(value) for key, value in by_count.items()},
            "groups": by_count, "duplicate_basenames": duplicates}


def verify_source_inventory(expected_csv: Path, files: list[dict]) -> dict:
    """Compare computed hashes to the original archive inventory, not Git hashes.

    CD_Extracted is expected to contain additional extracted files not listed
    in the source archive's outer-file inventory. They remain fully hashed.
    """
    current = {f["filename"]: f for f in files}
    checked = []
    with expected_csv.open("r", encoding="utf-8-sig", newline="") as stream:
        for source in csv.DictReader(stream):
            filename = source["relative_path"]
            existing = current.get(filename)
            item = {"filename": filename, "source_size": int(source["size_bytes"]),
                    "expected_sha256": source["sha256"].lower()}
            if existing is None:
                item["status"] = "MISSING"
            else:
                item["actual_size"] = existing["size"]
                item["actual_sha256"] = existing["sha256"]
                item["status"] = (
                    "PASS" if item["actual_size"] == item["source_size"]
                    and item["actual_sha256"] == item["expected_sha256"] else "MISMATCH"
                )
            checked.append(item)
    counts = {status: sum(x["status"] == status for x in checked)
              for status in ("PASS", "MISMATCH", "MISSING")}
    return {
        "source_inventory": expected_csv.name,
        "note": "External source-archive paths only; extracted CD members may be extra",
        "counts": counts,
        "unlisted_file_count": sum(f["filename"] not in {x["filename"] for x in checked}
                                   for f in files),
        "results": checked,
    }


def baselines(media: Path, files: list[dict], root: Path) -> list[dict]:
    results = []
    for basename, relative_script, expected in STRAHD_BASELINES:
        matches = [f for f in files if Path(f["filename"]).name.upper() == basename]
        entry = {"script": relative_script, "source_file": basename}
        script = root / relative_script
        if not script.is_file():
            entry.update(status="BLOCKED", reason="original analyzer absent from repository checkout")
        elif not media.is_dir():
            entry.update(status="BLOCKED", reason="baseline needs unpacked directory for script path argument")
        elif len(matches) != 1:
            entry.update(status="BLOCKED", reason=f"expected one {basename}; found {len(matches)}")
        else:
            path = media / matches[0]["filename"]
            command = [sys.executable, str(script), str(path)]
            process = subprocess.run(command, capture_output=True, text=True,
                                     timeout=120, check=False)
            actual = process.stdout
            passed = process.returncode == 0 and all(token in actual for token in expected)
            entry.update(status="PASS" if passed else "FAIL", command=command,
                         exit_code=process.returncode, expected=list(expected),
                         stdout=actual, stderr=process.stderr)
        results.append(entry)
    return results


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--title", choices=TITLES, required=True)
    parser.add_argument("--media", type=Path, required=True)
    parser.add_argument("--output", type=Path, help="Optional JSON output file (default stdout)")
    parser.add_argument("--baseline", action="store_true", help="Run unchanged Strahd analyzers")
    parser.add_argument("--expected-csv", type=Path, help="Compare actual SHA-256 to original source-manifest rows")
    parser.add_argument("--compare-other", type=Path, action="append", default=[],
                        help="Two other per-title JSON manifests for basename comparison")
    args = parser.parse_args()
    if args.baseline and args.title != "strahd":
        parser.error("--baseline currently applies to Strahd only")
    if args.compare_other and len(args.compare_other) != 2:
        parser.error("provide exactly two --compare-other manifests")
    media = args.media.expanduser().resolve(strict=True)
    files = inventory(media)
    result = {"schema_version": 1, "title": args.title, "media_root": str(media),
              "complete_for_provided_path_only": True, "file_count": len(files),
              "total_size": sum(row["size"] for row in files), "files": files}
    if args.expected_csv:
        result["source_inventory_audit"] = verify_source_inventory(
            args.expected_csv.expanduser().resolve(strict=True), files
        )
    if args.baseline:
        result["unchanged_strahd_baselines"] = baselines(media, files, Path(__file__).resolve().parents[1])
    if args.compare_other:
        others = [json.loads(path.read_text(encoding="utf-8")) for path in args.compare_other]
        result["comparison"] = comparison([result, *others])
    payload = json.dumps(result, indent=2, ensure_ascii=False) + "\n"
    if args.output:
        args.output.write_text(payload, encoding="utf-8")
    else:
        sys.stdout.write(payload)
    return 0 if all(x["status"] == "PASS" for x in result.get("unchanged_strahd_baselines", [])) else 1


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (OSError, ValueError, zipfile.BadZipFile, subprocess.TimeoutExpired) as exc:
        print(json.dumps({"error": str(exc)}, indent=2), file=sys.stderr)
        raise SystemExit(2)
