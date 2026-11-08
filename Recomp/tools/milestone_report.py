#!/usr/bin/env python3
"""Evidence-labelled Windows Recomp milestone manifest. Never self-certifies a gate."""
from __future__ import annotations

import argparse
import json
import re
import xml.etree.ElementTree as ET
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
GATES = ROOT / "ACCEPTANCE_GATES.json"


def report_for(milestone: str, sha: str, junit: Path | None = None, build_result: str = "not_executed") -> dict:
    gates = json.loads(GATES.read_text(encoding="utf-8"))
    if gates.get("schema") != "rloft.recomp.major-acceptance-gates.v1":
        raise ValueError("Unknown major gate schema")
    rows = gates["gates"]
    if len(rows) != 12 or [r["id"] for r in rows] != [f"R{i:02}" for i in range(1, 13)]:
        raise ValueError("Invalid R01-R12 gate set")
    if milestone == "next":
        chosen = next((row for row in rows if not row["accepted"]), rows[-1])
    else:
        chosen = next((row for row in rows if row["id"] == milestone), None)
        if chosen is None:
            raise ValueError("Unknown milestone: " + milestone)
    if not re.fullmatch(r"[a-fA-F0-9]{40}", sha):
        raise ValueError("Expected exact 40-hex source commit")
    tests = {"status": "not_executed", "total": 0, "failures": 0, "errors": 0, "skipped": 0}
    if junit and junit.is_file():
        root = ET.parse(junit).getroot()
        suites = [root] if root.tag == "testsuite" else list(root.findall("testsuite"))
        if not suites:
            raise ValueError("CTest JUnit has no testsuite")
        tests = {"status": "recorded"}
        for key in ("total", "failures", "errors", "skipped"):
            name = "tests" if key == "total" else key
            tests[key] = sum(int(s.attrib.get(name, 0)) for s in suites)
    return {
        "schema": "rloft.recomp.windows-milestone.v1",
        "milestone_id": chosen["id"],
        "milestone_scope": chosen["scope"],
        "milestone_accepted_by_source_evidence": chosen["accepted"],
        "milestone_limitation": chosen["limitation"],
        "source_sha": sha.lower(),
        "windows_ci_build_state": build_result,
        "ctest": tests,
        "artifact_type": "engineering-test-executables-not-playable-game",
        "original_media_bundled": False,
        "dos_runtime_parity_certified": False,
        "player_usable_game_certified": False,
        "interpretation": "A Windows build or CTest pass is not sufficient to accept a gate; manually record independent original-game evidence.",
    }


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--milestone", default="next")
    parser.add_argument("--sha", required=True)
    parser.add_argument("--junit", type=Path)
    parser.add_argument("--build-result", default="not_executed")
    parser.add_argument("--out", type=Path, required=True)
    args = parser.parse_args()
    obj = report_for(args.milestone, args.sha, args.junit, args.build_result)
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(obj, indent=2, ensure_ascii=True) + "\n", encoding="utf-8")
    print(f"Recomp milestone {obj['milestone_id']}: accepted={obj['milestone_accepted_by_source_evidence']}; Windows={args.build_result}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
