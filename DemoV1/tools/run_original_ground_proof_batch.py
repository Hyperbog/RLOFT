#!/usr/bin/env python3
"""Run pinned Level 22 ground-source oracles without weakening them.

SOURCE_ONLY_PASS is not original binary evidence. ORIGINAL_PASS requires
identity-verified user CODE.2 and SINTAB and successful checker scripts.
No network, binary uploads, gameplay effects, or automatic Windows CI.
"""
from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path
import subprocess
import sys
import time

ROOT = Path(__file__).resolve().parents[2]
CODE2_ID = (558689, "853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314")
SINTAB_ID = (333757, "19b6bc15f1be616bb8ff49dc9029995a1a0413fae06addc9fd41ddef761d0e87")
# Explicit versioned manifest. Newly added proof scripts require review and
# opt-in here; silent globbing omissions cannot masquerade as full coverage.
CHECKS = (
    ("ff50_direct", "verify_original_ground_ff50_direct_references.py", False),
    ("ff50_span", "verify_original_ground_ff50_source_span.py", True),
    ("mode_options", "verify_original_ground_mode_option_bytes.py", False),
    ("nearest_22220", "verify_original_ground_nearest_22220.py", False),
    ("planar_raster", "verify_original_ground_raster_unrolled_planes.py", False),
    ("sentinel_32f4c", "verify_original_ground_sentinel_fill.py", False),
    ("source_projection", "verify_original_ground_source_projection.py", True),
    ("surface_dispatch", "verify_original_ground_surface_dispatch.py", False),
    ("upper_callbacks", "verify_original_ground_upper_callback_boundary.py", False),
    ("c050_page", "verify_original_ground_c050_page_toggle.py", False),
    ("vga_page_refs", "verify_original_ground_vga_page_refs.py", False),
)


def validate_source(path: Path, identity: tuple[int, str], label: str) -> None:
    if not path.is_file():
        raise ValueError(f"{label}: original input missing or not a file")
    if path.stat().st_size != identity[0]:
        raise ValueError(f"{label}: incorrect original file size")
    h = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b""):
            h.update(chunk)
    if h.hexdigest() != identity[1]:
        raise ValueError(f"{label}: wrong SHA-256 or patch version")


def run_check(name: str, filename: str, needs_sintab: bool,
              *, original: bool, code2: Path | None, sintab: Path | None,
              timeout: float) -> dict[str, object]:
    path = ROOT / "DemoV1" / "tools" / filename
    command = [sys.executable, str(path)]
    if original:
        assert code2 is not None and sintab is not None
        command.extend(("--original-code2", str(code2)))
        if needs_sintab:
            command.extend(("--original-sintab", str(sintab)))
    start = time.monotonic()
    try:
        process = subprocess.run(command, cwd=ROOT, capture_output=True,
                                 text=True, timeout=timeout, check=False)
        status = ("ORIGINAL_PASS" if original else "SOURCE_ONLY_PASS") if process.returncode == 0 else "FAIL"
        return {"name": name, "status": status, "exit_code": process.returncode,
                "seconds": round(time.monotonic() - start, 3),
                "stdout": process.stdout[-1200:], "stderr": process.stderr[-2000:]}
    except subprocess.TimeoutExpired:
        return {"name": name, "status": "FAIL", "exit_code": None,
                "seconds": round(time.monotonic() - start, 3),
                "stdout": "", "stderr": "checker timed out"}
    except OSError as exc:
        return {"name": name, "status": "FAIL", "exit_code": None,
                "seconds": round(time.monotonic() - start, 3),
                "stdout": "", "stderr": f"checker launch failed: {exc}"}


def main() -> int:
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--source-only", action="store_true",
                   help="run native/source contract checks WITHOUT original byte proof")
    p.add_argument("--original-code2", type=Path)
    p.add_argument("--original-sintab", type=Path)
    p.add_argument("--only", action="append", metavar="CHECK",
                   help="repeat to select exact names; report indicates partial coverage")
    p.add_argument("--list", action="store_true",
                   help="list pinned check names, without running tests")
    p.add_argument("--json-out", type=Path,
                   help="optional local machine-readable result")
    p.add_argument("--timeout", type=float, default=45.0)
    args = p.parse_args()
    if args.list:
        for name, filename, requires_sintab in CHECKS:
            print(f"{name}\t{filename}\tSINTAB={int(requires_sintab)}")
        return 0
    if args.timeout <= 0:
        p.error("--timeout must be positive")
    if args.source_only:
        if args.original_code2 or args.original_sintab:
            p.error("--source-only cannot be combined with original binaries")
    elif not (args.original_code2 and args.original_sintab):
        p.error("provide BOTH --original-code2 and --original-sintab, or explicitly use --source-only")
    names = {check[0] for check in CHECKS}
    chosen = set(args.only) if args.only else names
    unknown = chosen - names
    if unknown:
        p.error("unknown --only: " + ", ".join(sorted(unknown)))
    selected = [check for check in CHECKS if check[0] in chosen]
    original = not args.source_only
    if original:
        try:
            validate_source(args.original_code2, CODE2_ID, "CODE.2")
            validate_source(args.original_sintab, SINTAB_ID, "SINTAB")
        except (ValueError, OSError) as exc:
            print(f"INPUT_REJECTED: {exc}", file=sys.stderr)
            return 2
    results = [run_check(*check, original=original,
                         code2=args.original_code2, sintab=args.original_sintab,
                         timeout=args.timeout) for check in selected]
    failed = sum(item["status"] == "FAIL" for item in results)
    report = {"schema": "rloft-ground-proof-batch-v1",
              "mode": "original-and-source" if original else "source-only",
              "scope": "all-pinned" if len(selected) == len(CHECKS) else "subset",
              "selected": len(selected), "catalog": len(CHECKS), "failed": failed,
              "original_sha256": {"CODE.2": CODE2_ID[1], "SINTAB": SINTAB_ID[1]} if original else None,
              "results": results}
    for item in results:
        print(f"{item['status']:16s} {item['name']:22s} {item['seconds']:7.3f}s")
        if item["status"] == "FAIL":
            print(str(item["stderr"]).strip()[-600:], file=sys.stderr)
    print(f"SUMMARY mode={report['mode']} coverage={report['scope']} "
          f"passed={len(selected)-failed}/{len(selected)} failed={failed}")
    if args.json_out:
        args.json_out.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
