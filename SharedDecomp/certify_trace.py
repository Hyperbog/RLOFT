#!/usr/bin/env python3
"""Provenance-bound comparison of independent reference/host capture traces.

This checks declared capture identities and exact trace bytes. JSON manifests
are NOT cryptographic attestations that a trace came from original hardware,
DOSBox, or an independent host. No gameplay/DOS parity claim is emitted.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import re
import sys
from pathlib import Path

from workflow import EvidenceError, atomic_json, iter_jsonl, read_json, require, trace_diff

SCHEMA = "rloft-trace-capture-v1"
SHA256 = re.compile(r"[0-9a-f]{64}\Z")


def _digest_file(path: Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            h.update(block)
    return h.hexdigest()


def _nonempty(doc: dict, name: str) -> str:
    value = doc.get(name)
    require(isinstance(value, str) and bool(value.strip()),
            f"capture manifest: {name} must be nonempty text")
    return value


def _sha_field(doc: dict, name: str) -> str:
    value = _nonempty(doc, name)
    require(SHA256.fullmatch(value) is not None,
            f"capture manifest: invalid {name}")
    return value


def _check_steps(path: Path) -> int:
    """Require unambiguous contiguous logical steps, starting at zero.

    The two capture adapters must themselves agree on the meaning of a step;
    this validator cannot infer alignment from identical step numbers.
    """
    count = 0
    for frame in iter_jsonl(path):
        step = frame["step"]
        require(type(step) is int and step == count,
                f"capture trace has missing, duplicated or unordered step at {count}")
        require(any(k in frame for k in ("pc", "event", "state_id")),
                f"capture trace step {count} has no event/PC identity")
        count += 1
    require(count > 0, "empty traces cannot certify comparison")
    return count


def _check_manifest(doc: dict, path: Path, role: str) -> dict:
    require(doc.get("schema") == SCHEMA, "unrecognized trace capture schema")
    require(doc.get("role") == role, f"expected {role} capture manifest")
    trace_sha = _sha_field(doc, "trace_sha256")
    binary_sha = _sha_field(doc, "binary_sha256")
    adapter_sha = _sha_field(doc, "adapter_sha256")
    scenario = _nonempty(doc, "scenario_id")
    source = _nonempty(doc, "source_kind")
    capture = _nonempty(doc, "capture_id")
    require(_digest_file(path) == trace_sha,
            f"{role}: trace payload differs from captured SHA256")
    return {"role": role, "trace_sha256": trace_sha,
            "binary_sha256": binary_sha, "adapter_sha256": adapter_sha,
            "scenario_id": scenario, "source_kind": source,
            "capture_id": capture, "steps": _check_steps(path)}


def _safe_json(value):
    """Existing trace_diff uses a private sentinel for a missing JSON field."""
    try:
        json.dumps(value, allow_nan=False)
    except (TypeError, ValueError):
        return "<field absent>"
    return value


def certify(reference: Path, candidate: Path,
            reference_manifest: dict, candidate_manifest: dict) -> dict:
    require(reference.resolve() != candidate.resolve(),
            "reference and candidate must be separate trace inputs")
    ref = _check_manifest(reference_manifest, reference, "reference")
    host = _check_manifest(candidate_manifest, candidate, "candidate")
    require(ref["scenario_id"] == host["scenario_id"],
            "capture scenario mismatch")
    require(ref["capture_id"] != host["capture_id"],
            "same declared capture session cannot certify independent replay")
    require(ref["source_kind"] != host["source_kind"],
            "reference and candidate source kinds must differ")
    outcome = trace_diff(reference, candidate)
    difference = {k: _safe_json(v) for k, v in outcome.items()
                  if k in ("matched_steps", "first_divergence_step", "field",
                           "reference", "candidate", "reference_pc", "candidate_pc",
                           "reference_bank", "candidate_bank")}
    equal = outcome["status"] == "PASS"
    return {"schema": "rloft-trace-comparison-certificate-v1",
            "status": "EQUAL_UNATTESTED" if equal else "DIFFERENT",
            "trace_equal": equal,
            "scenario_id": ref["scenario_id"],
            "reference": ref,
            "candidate": host,
            "comparison": difference,
            "capture_origin_attested": False,
            "original_binary_authenticated": False,
            "dos_runtime_parity_verified": False,
            "native_gameplay_parity_verified": False,
            "note": ("Manifest hashes authenticate trace content only; independent "
                     "original execution and native parity require externally "
                     "verified captures and semantic step alignment.")}


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--reference", type=Path, required=True)
    parser.add_argument("--candidate", type=Path, required=True)
    parser.add_argument("--reference-manifest", type=Path, required=True)
    parser.add_argument("--candidate-manifest", type=Path, required=True)
    parser.add_argument("--report", type=Path, required=True)
    args = parser.parse_args(argv)
    try:
        result = certify(args.reference, args.candidate,
                         read_json(args.reference_manifest),
                         read_json(args.candidate_manifest))
        atomic_json(args.report, result)
        print(json.dumps({"status": result["status"],
                          "trace_equal": result["trace_equal"],
                          "dos_runtime_parity_verified": False,
                          "report": str(args.report)}, sort_keys=True))
        return 0 if result["trace_equal"] else 1
    except (EvidenceError, OSError, KeyError, TypeError, ValueError,
            json.JSONDecodeError) as error:
        print(f"TRACE CERTIFICATE ERROR: {error}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
