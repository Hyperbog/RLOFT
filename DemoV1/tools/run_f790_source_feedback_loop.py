"""On-demand deterministic F790 source feedback loop; never builds or edits.

Run from a checkout:
  python DemoV1/tools/run_f790_source_feedback_loop.py
  python DemoV1/tools/run_f790_source_feedback_loop.py --code2 CODE.2 --objects OBJECTS.DAT --candidate-report Code2.json --json

Each check independently reports PASS/FAIL. Original binaries are optional,
and neither source-only CTest checks nor the report manufacture parity.
No GitHub API, network, timers, build jobs, or game-state mutation.
"""
from __future__ import annotations
import argparse
import json
from pathlib import Path
import subprocess
import sys

TOOLS=Path(__file__).resolve().parent
SOURCE_SCRIPTS=(
    "test_original_inventory_f790_cadence.py",
    "verify_original_f790_background_page.py",
    "verify_original_f790_feedback_edges.py",
    "verify_original_f790_item_description_dispatch.py",
    "verify_original_f790_description_source.py",
    "verify_original_f790_static_resources.py",
    "verify_original_f790_runtime_event.py",
    "analyze_original_f790_candidate_coverage.py",
    "verify_original_f790_selected_poll.py",
)
# Real-source runners selected by the original script's own strict CLI.
def original_cases(code2:Path,objects:Path|None,report:Path|None):
    basic=("--original-code2",str(code2),"--binary-only")
    cases=[
        ("test_original_inventory_f790_cadence.py",basic),
        ("verify_original_f790_background_page.py",basic),
        ("verify_original_f790_feedback_edges.py",basic),
        ("verify_original_f790_item_description_dispatch.py",basic),
        ("verify_original_f790_runtime_event.py",("--original-code2",str(code2))),
        ("verify_original_f790_description_source.py",basic+(
            ("--original-objects",str(objects)) if objects else ())),
        ("verify_original_f790_static_resources.py",("--code2",str(code2))+(
            ("--objects",str(objects)) if objects else ())),
    ]
    cases.append(("verify_original_f790_selected_poll.py",basic))
    if report is not None:
        cases.append(("analyze_original_f790_candidate_coverage.py",
                      ("--original-code2",str(code2),"--code2-report",
                       str(report),"--binary-only")))
    return cases

def run(name:str,scope:str,extra:tuple[str,...]):
    location=TOOLS/name
    if not location.is_file():
        return {"name":name,"scope":scope,"passed":False,
                "message":"missing expected checked-in contract"}
    command=(sys.executable,str(location),*extra)
    try:
        result=subprocess.run(command,stdin=subprocess.DEVNULL,
                              capture_output=True,text=True,timeout=90,
                              check=False,cwd=TOOLS.parent.parent)
        return {"name":name,"scope":scope,"passed":result.returncode==0,
                "message":(result.stdout.strip() if result.returncode==0
                           else (result.stderr.strip() or result.stdout.strip())),
                "exit_code":result.returncode}
    except (OSError,subprocess.TimeoutExpired) as exc:
        return {"name":name,"scope":scope,"passed":False,"message":str(exc)}

def main()->int:
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument("--code2",type=Path,help="private original CODE.2")
    p.add_argument("--objects",type=Path,help="private original OBJECTS.DAT")
    p.add_argument("--candidate-report",type=Path,
                   help="candidate-unverified Code2.json cross-check")
    p.add_argument("--json",action="store_true",help="machine-readable output")
    p.add_argument("--write-json",type=Path,
                   help="write this audit result as a JSON artifact")
    args=p.parse_args()
    if (args.objects or args.candidate_report) and not args.code2:
        p.error("--objects and --candidate-report require --code2")
    results=[run(name,"source-only",()) for name in SOURCE_SCRIPTS]
    if args.code2:
        for name,extra in original_cases(args.code2,args.objects,
                                          args.candidate_report):
            results.append(run(name,"immutable-original",extra))
    count=sum(bool(entry["passed"]) for entry in results)
    audit={"schema":"rloft-f790-source-feedback-loop-v1",
           "checks":results,"passed":count,"failed":len(results)-count,
           "manual_windows_build_triggered":False,
           "original_dos_pixel_acceptance":"not-established"}
    payload=json.dumps(audit,indent=2,sort_keys=True)
    if args.write_json:
        args.write_json.write_text(payload+"\n",encoding="utf-8")
    if args.json:
        print(payload)
    else:
        for entry in results:
            print(("PASS" if entry["passed"] else "FAIL")+" "+
                  entry["scope"]+" "+entry["name"]+" "+entry["message"])
        print(f"F790 source feedback: {count}/{len(results)} checks passed; "
              "Windows build NOT run; DOS parity NOT established")
    return 0 if count==len(results) else 1

if __name__=="__main__":
    sys.exit(main())
