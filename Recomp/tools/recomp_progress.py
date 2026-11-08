#!/usr/bin/env python3
"""Conservative, reproducible acceptance-milestone percentage for canonical Recomp.

This is NOT an estimate of playable-game completion. It reports a fixed set of
coarse gates, not lines of code, percentage of original instructions or CI pass
rate. It refuses to turn test-only fixtures into completed DOS/runtime parity.
"""
from __future__ import annotations
import argparse
import json
from pathlib import Path

ACCEPTANCE_FILE = Path(__file__).resolve().parent.parent / "ACCEPTANCE_GATES.json"

def calculate(raw: dict) -> dict:
    if raw.get("schema") != "rloft.recomp.major-acceptance-gates.v1":
        raise ValueError("unknown acceptance-gate schema")
    gates = raw.get("gates")
    if not isinstance(gates,list) or len(gates) != raw.get("total") or not gates:
        raise ValueError("invalid fixed major acceptance-gate denominator")
    if len(gates)!=12:
        raise ValueError("any change to the 12-gate rubric requires an explicitly revised definition")
    ids = [g.get("id") for g in gates]
    if ids != [f"R{i:02d}" for i in range(1,13)]:
        raise ValueError("major acceptance-gate identity/order changed")
    for g in gates:
        if type(g.get("accepted")) is not bool:
            raise ValueError("gate status must be true or false")
        if not isinstance(g.get("scope"),str) or len(g["scope"]) < 35:
            raise ValueError("gate scope missing")
        if not isinstance(g.get("limitation"),str) or not g["limitation"]:
            raise ValueError("each gate needs an explicit limitation")
        if g["accepted"] and (not isinstance(g.get("evidence"),str) or
                              "Recomp/" not in g["evidence"]):
            raise ValueError("accepted gates require canonical project evidence")
    passed = sum(g["accepted"] for g in gates)
    return {
        "schema":"rloft.recomp.progress.v1",
        "basis":"12 fixed major acceptance milestones; not playable-game percent",
        "accepted":passed,"total":len(gates),
        "percent":round(100*passed/len(gates),2),
        "pending":[g["id"] for g in gates if not g["accepted"]],
        "accepted_ids":[g["id"] for g in gates if g["accepted"]],
        "game_booted":False,
        "original_dos_parity":False,
        "notes":"Status is manually evidence-certified; no synthetic fixture or CI test alone passes a whole-game gate."
    }

def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--manifest",type=Path,default=ACCEPTANCE_FILE)
    parser.add_argument("--json",action="store_true")
    args=parser.parse_args(argv)
    report=calculate(json.loads(args.manifest.read_text(encoding="utf-8")))
    if args.json:
        print(json.dumps(report,indent=2))
    else:
        print(f"RECOMP_ACCEPTED_MILESTONES={report['accepted']}/{report['total']}")
        print(f"RECOMP_MILESTONE_COMPLETION={report['percent']:.2f}%")
        print("RECOMP_PERCENT_BASIS=MAJOR_ACCEPTANCE_GATES_NOT_GAMEPLAY")
        print("GAME_BOOTED=NO")
        print("ORIGINAL_DOS_PARITY=NO")
    return 0

if __name__=="__main__":
    raise SystemExit(main())
