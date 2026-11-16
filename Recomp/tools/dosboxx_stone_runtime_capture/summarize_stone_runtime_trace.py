#!/usr/bin/env python3
"""Validate owner-private original Stone runtime JSONL without game media.

A syntactically valid trace is not a DOS-session, encounter, or crash proof.
"""
import argparse
import collections
import hashlib
import json
from pathlib import Path

HOOK = "dosboxx-stone-original-v1"
SOURCE = "00e225ec8f141a8fdb72ff0547fe93b2ea3aabdb4ec3b59d9261a5268de9203e"
AFFECTED = (4, 24, 27)
KINDS = {
    "code2_signature_verified", "event_buffer_bound", "event_baseline",
    "invalid_event_buffer", "event_buffer_changed",
    "event_buffer_opcode13_count", "affected_event_commands_present",
    "event_spawn_attempt", "constructor_entry",
    "alternate_constructor_entry", "authored_live_type",
    "restored_live_type", "idiv_zero_before_execution",
    "idiv_nonzero_sample", "data_segment_changed",
}

def verify(path):
    raw=Path(path).read_bytes()
    lines=raw.splitlines()
    if len(lines)<2:
        raise ValueError("Trace has no event rows; original CODE.2 was not observed")
    try:
        header=json.loads(lines[0])
    except (json.JSONDecodeError,UnicodeDecodeError) as e:
        raise ValueError("Invalid trace header") from e
    if (header.get("hook")!=HOOK or
        header.get("stone_code2_sha256")!=SOURCE or
        header.get("core")!="normal"):
        raise ValueError("Trace version, code signature, or core mismatch")
    counts=collections.Counter()
    target_spawns=collections.Counter()
    target_restores=collections.Counter()
    target_ctors=collections.Counter()
    division_zero_actors=collections.Counter()
    prev=0
    seen_signature=False
    baseline=0
    mutations=0
    for seq,line in enumerate(lines[1:]):
        try:
            row=json.loads(line)
        except (json.JSONDecodeError,UnicodeDecodeError) as e:
            raise ValueError(f"Invalid or truncated event line {seq+2}") from e
        if row.get("overflow"):
            raise ValueError("Capture overflow: cannot certify complete trace")
        if row.get("seq")!=seq:
            raise ValueError(f"Capture sequence gap at event {seq}")
        if row.get("kind") not in KINDS:
            raise ValueError(f"Unknown capture event kind at row {seq+2}")
        if not all(isinstance(row.get(f),int) and 0<=row[f]<=0xffffffff
                   for f in ("cs","eip","x","y","z","w","prior_eip")):
            raise ValueError(f"Invalid original register/value at row {seq+2}")
        kind=row["kind"]
        counts[kind]+=1
        if kind=="code2_signature_verified":
            seen_signature=True
        elif not seen_signature:
            raise ValueError("Events preceding CODE.2 identity witness")
        if kind=="event_spawn_attempt" and row["x"] in AFFECTED:
            target_spawns[row["x"]]+=1
        elif kind=="constructor_entry" and row["x"] in AFFECTED:
            target_ctors[row["x"]]+=1
        elif kind=="restored_live_type" and row["x"] in AFFECTED:
            target_restores[row["x"]]+=1
        elif kind=="idiv_zero_before_execution":
            division_zero_actors[row["y"]]+=1
        if kind=="event_baseline":
            baseline+=1
        elif kind=="event_buffer_changed":
            mutations+=1
    if not seen_signature:
        raise ValueError("No original Stone CODE.2 signature observed")
    return {
        "trace_file_sha256":hashlib.sha256(raw).hexdigest(),
        "hook":HOOK,
        "code2_machine_signature_observed":True,
        "guest_original_gameplay_end_to_end_confirmed":False,
        "raw_trace_events":sum(counts.values()),
        "event_kinds":dict(sorted(counts.items())),
        "event_buffer_baselines":baseline,
        "event_buffer_mutation_observations":mutations,
        "affected_event_spawn_dispatch_count":dict(sorted(target_spawns.items())),
        "affected_constructor_entry_count":dict(sorted(target_ctors.items())),
        "affected_restored_actor_type_count":dict(sorted(target_restores.items())),
        "idiv_zero_before_execution_actor_type_hints":
            dict(sorted(division_zero_actors.items())),
        "idiv_zero_pre_execution_observed":
            counts["idiv_zero_before_execution"]>0,
        "original_divide_exception_or_crash_observed":False,
        "interpretation":
           "All findings are observed original-code instruction entry and memory "
           "samples. A zero divisor before IDIV is a verified hazard condition, "
           "not a captured CPU exception. Missing rows do not prove unreachability.",
        "coverage_limit":
           "Only CPU normal core is hooked, with the exact CODE.2 signature. "
           "Event mutations are sampled at checkpoints unless "
           "RLOFT_STONE_TRACE_EVERY_INSTRUCTION=1 was set. "
           "Other code segments and emulator cores are unobserved.",
    }

def main():
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--trace",type=Path,required=True)
    ap.add_argument("--report",type=Path,required=True)
    args=ap.parse_args()
    result=verify(args.trace)
    if args.report.exists():
        raise ValueError("Refusing to overwrite existing analysis report")
    args.report.parent.mkdir(parents=True,exist_ok=True)
    args.report.write_text(json.dumps(result,indent=2)+"\n",encoding="utf-8")
    print("STONE_ORIGINAL_TRACE_VALIDATED",result["raw_trace_events"],
          "zero_idiv",result["idiv_zero_pre_execution_observed"],
          "target_dispatch",result["affected_event_spawn_dispatch_count"],
          "full_gameplay_certified=NO")

if __name__=="__main__":
    main()
