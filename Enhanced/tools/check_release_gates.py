#!/usr/bin/env python3
"""Read-only enhanced native PC port Alpha/Beta/Final release validator.

Checks schema, proof *presence*, stage order, honest statuses, and active
enhanced-port policy. It does not run Windows builds or the game.
Original DOS captures remain optional research, not mandatory release proof.
Any external evidence link still needs human inspection.
"""
from __future__ import annotations

import argparse
import copy
import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
DEFAULT = ROOT / "DemoV1" / "RELEASE_GATES.json"
TYPES = {
    "source_oracle", "windows_ci", "native_field",
    "dos_comparison", "package_audit", "owner_acceptance"
}
CODES = ("alpha", "beta", "final")
FULL_SHA = re.compile(r"^[0-9a-f]{40}$")


def require(ok: bool, reason: str) -> None:
    if not ok:
        raise ValueError(reason)


def validate(data: dict) -> list[tuple[str, int, int]]:
    require(isinstance(data, dict) and data.get("schema_version") == 1,
            "unknown release gate schema")
    require(data.get("owner_branch") == "demo-v1",
            "gate source ownership must remain demo-v1")
    stages = data.get("stages")
    require(isinstance(stages, list) and
            [s.get("id") for s in stages] == list(CODES),
            "Alpha -> Beta -> Final ordering required")
    observed = data.get("observed_windows_ci")
    require(isinstance(observed, dict) and
            FULL_SHA.fullmatch(observed.get("source_sha", "")) is not None and
            isinstance(observed.get("run_number"), int) and
            observed["run_number"] > 0 and
            isinstance(observed.get("ctests_passed"), int) and
            observed["ctests_passed"] > 0 and
            observed["ctests_passed"] == observed.get("ctests_total") and
            observed.get("qualifies_as_original_dos_parity") is False,
            "Windows CI provenance cannot be called DOS parity")
    require(isinstance(data.get("release_policy"), str) and
            data["release_policy"] ==
            "DemoV1/ENHANCED_PC_PORT_CHARTER.md",
            "enhanced PC port charter must remain release authority")
    require(data.get("current_classification") in
            {"pre-alpha", "alpha", "beta", "final"},
            "unknown declared release classification")
    seen = set()
    all_ready: dict[str, bool] = {}
    summary: list[tuple[str, int, int]] = []
    for stage in stages:
        stage_id = stage["id"]
        gates = stage.get("gates")
        require(isinstance(gates, list) and len(gates) >= 8,
                f"{stage_id}: expected substantive release gates")
        met = 0
        for g in gates:
            code = g.get("id")
            require(isinstance(code, str) and
                    code.startswith(stage_id.upper() + "-") and
                    code not in seen, "non-unique or misplaced gate")
            seen.add(code)
            require(isinstance(g.get("name"), str) and g["name"] and
                    isinstance(g.get("pass_when"), str) and
                    len(g["pass_when"]) >= 50,
                    f"{code}: release gate needs falsifiable acceptance")
            required = g.get("requires")
            require(isinstance(required, list) and
                    len(required) == len(set(required)) and
                    required and set(required) <= TYPES,
                    f"{code}: unknown/duplicate proof requirements")
            require("dos_comparison" not in required,
                    f"{code}: DOS parity cannot be a mandatory enhanced-port release gate")

            status = g.get("status")
            require(status in {"open", "met"}, f"{code}: invalid status")
            evidence = g.get("evidence")
            require(isinstance(evidence, list),
                    f"{code}: evidence must be an explicit list")
            accepted_types = set()
            for e in evidence:
                require(isinstance(e, dict) and e.get("type") in TYPES and
                        isinstance(e.get("ref"), str) and len(e["ref"]) >= 12,
                        f"{code}: malformed evidence reference")
                require(FULL_SHA.fullmatch(e.get("source_sha", "")) is not None,
                        f"{code}: every acceptance claim pins exact source SHA")
                accepted_types.add(e["type"])
            if status == "met":
                require(set(required) <= accepted_types,
                        f"{code}: cannot pass without ALL required proof types")
                require(not g.get("blocked_by"),
                        f"{code}: passed gate retains unresolved blockers")
                met += 1
            else:
                require(isinstance(g.get("blocked_by"), list) and
                        bool(g["blocked_by"]) and
                        all(isinstance(b, str) and len(b) >= 20
                            for b in g["blocked_by"]),
                        f"{code}: open gate needs concrete blocker")
        all_ready[stage_id] = met == len(gates)
        summary.append((stage_id, met, len(gates)))
    require(all_ready["alpha"] or
            not (all_ready["beta"] or all_ready["final"]),
            "Beta/Final cannot be complete before Alpha")
    require(all_ready["beta"] or not all_ready["final"],
            "Final cannot be complete before Beta")
    achieved = ("final" if all_ready["final"] else
                "beta" if all_ready["beta"] else
                "alpha" if all_ready["alpha"] else "pre-alpha")
    require(data["current_classification"] == achieved,
            "declared classification exceeds or contradicts accepted gates")
    return summary


def self_test(original: dict) -> None:
    # A structurally valid manifest is not a gameplay acceptance claim.
    summary = validate(original)
    assert summary and summary[0][0] == "alpha"
    failures = []

    bad = copy.deepcopy(original)
    bad["stages"][0]["gates"][0]["status"] = "met"
    failures.append(bad)  # cannot pass without required Windows evidence

    bad = copy.deepcopy(original)
    bad["stages"][0]["gates"][0]["blocked_by"] = []
    failures.append(bad)  # cannot hide blocker

    bad = copy.deepcopy(original)
    bad["stages"][1]["gates"][0]["id"] = original["stages"][0]["gates"][0]["id"]
    failures.append(bad)  # duplicate or cross-stage ID

    bad = copy.deepcopy(original)
    bad["current_classification"] = "final"
    failures.append(bad)  # cannot claim final on CI alone

    bad = copy.deepcopy(original)
    bad["observed_windows_ci"]["qualifies_as_original_dos_parity"] = True
    failures.append(bad)  # CI != reference-DOS capture

    bad = copy.deepcopy(original)
    bad["stages"] = list(reversed(bad["stages"]))
    failures.append(bad)  # stage order

    bad = copy.deepcopy(original)
    bad["stages"][0]["gates"][0]["requires"] = ["dos_comparison"]
    failures.append(bad)  # retired DOS parity requirement must stay optional

    bad = copy.deepcopy(original)
    bad["release_policy"] = "DemoV1/ORIGINAL_EXECUTION_FIDELITY_CHARTER.md"
    failures.append(bad)  # cannot revert to old project goal

    bad = copy.deepcopy(original)
    bad["stages"][0]["gates"][0]["requires"] = []
    failures.append(bad)  # proof obligation removed

    for number, mutation in enumerate(failures, start=1):
        try:
            validate(mutation)
        except ValueError:
            continue
        raise AssertionError(f"release gate negative test {number} was accepted")
    print(f"RELEASE_GATES_SOURCE_PASS {len(failures)} regression mutations rejected")


def main() -> int:
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--manifest", type=Path, default=DEFAULT)
    p.add_argument("--self-test", action="store_true")
    args = p.parse_args()
    try:
        data = json.loads(args.manifest.read_text(encoding="utf-8"))
        summary = validate(data)
        if args.self_test:
            self_test(data)
        else:
            for stage, accepted, total in summary:
                print(f"{stage.upper()}: {accepted}/{total} release gates accepted")
            print(f"CURRENT: {data['current_classification']}")
            print("Verification: structural only; external evidence needs review")
    except (ValueError, OSError, AssertionError) as exc:
        p.exit(1, f"RELEASE_GATES_REJECTED: {exc}\n")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
