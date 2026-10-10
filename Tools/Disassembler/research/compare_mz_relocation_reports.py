#!/usr/bin/env python3
"""Compare separately produced Python and DOSDis MZ relocation metadata.

SOURCE-ONLY until next owner-authorized execution. Both reports must refer
to one independently pinned original SHA256 and one explicitly assumed DOS
load segment. Equality is not proof of executed code or a real DOS allocation.
No source bytes, relocation words, private filenames or report contents are
copied into the result. Never commit original proprietary reports.
"""
from __future__ import annotations

import argparse
import json
import re
from pathlib import Path

MAX_REPORT_BYTES = 2 * 1024 * 1024
SHA256_PATTERN = re.compile(r"[0-9a-fA-F]{64}\Z")


class InvalidReport(ValueError):
    pass


def _unique_properties(pairs: list[tuple[str, object]]) -> dict:
    props: dict[str, object] = {}
    seen: set[str] = set()
    for name, value in pairs:
        normalized = name.casefold()
        if normalized in seen:
            raise InvalidReport("duplicate or case-colliding report fields")
        seen.add(normalized)
        props[name] = value
    return props


def _reject_nonstandard_number(token: str) -> object:
    raise InvalidReport("nonfinite or nonstandard report number")


def read_report(path: Path) -> dict:
    try:
        size = path.stat().st_size
        if not (1 <= size <= MAX_REPORT_BYTES):
            raise InvalidReport("report size exceeds bounded comparison scope")
        raw = path.read_bytes()
        if len(raw) != size:
            raise InvalidReport("report changed during read")
        result = json.loads(
            raw.decode("utf-8"),
            object_pairs_hook=_unique_properties,
            parse_constant=_reject_nonstandard_number,
        )
    except (OSError, ValueError, UnicodeError) as exc:
        if isinstance(exc, InvalidReport):
            raise
        raise InvalidReport("report is unreadable or invalid JSON") from exc
    if not isinstance(result, dict):
        raise InvalidReport("report must be a JSON object")
    return result


def _object(parent: dict, name: str) -> dict:
    result = parent.get(name)
    if not isinstance(result, dict):
        raise InvalidReport("missing expected structured report metadata")
    return result


def _sha256(value: object) -> str:
    if not isinstance(value, str) or SHA256_PATTERN.fullmatch(value) is None:
        raise InvalidReport("invalid or missing source/image SHA256")
    return value.lower()


def _uint(value: object, maximum: int) -> int:
    if type(value) is not int or not 0 <= value <= maximum:
        raise InvalidReport("invalid or missing bounded integer field")
    return value


def _flag(value: object) -> bool:
    if type(value) is not bool:
        raise InvalidReport("invalid or missing boolean field")
    return value


def compare(independent: dict, candidate: dict, expected_source_sha: str) -> dict:
    """Compare report facts, never use either result to derive the other."""
    expected = _sha256(expected_source_sha)
    if independent.get("evidence") != "read-only-original-source-preflight":
        raise InvalidReport("not a recognized independent preflight record")
    if independent.get("g2_accuracy_status") != "not-certified" or (
        independent.get("independent_decoder_or_execution") is not False
    ):
        raise InvalidReport("independent report makes unsupported certification claim")
    originals = independent.get("originals")
    if not isinstance(originals, list) or len(originals) != 1:
        raise InvalidReport("expected one unambiguous pinned original source")
    original = _object({"selected": originals[0]}, "selected")
    if _sha256(original.get("original_sha256")) != expected:
        raise InvalidReport("independent report belongs to a foreign original source")
    geometry = _object(original, "geometry")
    if geometry.get("format") != "outer-DOS-MZ-stored-image-only":
        raise InvalidReport("independent result is not an original MZ load image")
    simulation = _object(independent, "assumed_mz_relocation_simulation")
    if simulation.get("status") != "simulated-relocations;assumed-segment;not-runtime-proof":
        raise InvalidReport("independent relocation model status is unsupported")
    if not isinstance(candidate.get("AnalysisStatus"), str) or not candidate[
        "AnalysisStatus"
    ].startswith("simulated-MZ-relocations;"):
        raise InvalidReport("DOSDis report is not an assumed-base MZ simulation")
    if _sha256(candidate.get("SourceSha256")) != expected:
        raise InvalidReport("DOSDis report belongs to a foreign original source")

    # Keys refer to visibly distinct JSON formats on purpose. Do not
    # silently accept absent optional facts or cross-title relabeling.
    independent_facts = {
        "assumed_load_segment": _uint(simulation.get("assumed_load_segment"), 65535),
        "original_file_backed_image_bytes": _uint(
            simulation.get("original_file_backed_image_bytes"), MAX_REPORT_BYTES * 64
        ),
        "relocation_write_count": _uint(
            simulation.get("relocation_write_count"), 65535
        ),
        "overlapping_relocation_writes": _flag(
            simulation.get("overlapping_relocation_writes")
        ),
        "simulated_file_backed_image_sha256": _sha256(
            simulation.get("simulated_file_backed_image_sha256")
        ),
    }
    candidate_facts = {
        "assumed_load_segment": _uint(candidate.get("AssumedLoadSegment"), 65535),
        "original_file_backed_image_bytes": _uint(
            candidate.get("OriginalFileBackedBytes"), MAX_REPORT_BYTES * 64
        ),
        "relocation_write_count": _uint(candidate.get("RelocationWriteCount"), 65535),
        "overlapping_relocation_writes": _flag(
            candidate.get("OverlappingRelocationWrites")
        ),
        "simulated_file_backed_image_sha256": _sha256(
            candidate.get("SimulatedFileBackedImageSha256")
        ),
    }
    if (
        independent_facts["original_file_backed_image_bytes"]
        != _uint(geometry.get("file_backed_load_bytes"), MAX_REPORT_BYTES * 64)
        or independent_facts["relocation_write_count"]
        != _uint(geometry.get("relocation_count"), 65535)
        or independent_facts["overlapping_relocation_writes"]
        != (_uint(geometry.get("relocation_overlaps"), 65535) > 0)
    ):
        raise InvalidReport("independent geometry and simulated metadata conflict")
    mismatches = sorted(
        field for field in independent_facts
        if independent_facts[field] != candidate_facts[field]
    )
    return {
        "schema_version": 1,
        "scope": "independent-vs-dosdis-assumed-MZ-relocation-metadata",
        "source_sha256": expected,
        "independent_evidence_status": "two-implementations;no-review;not-runtime-proof",
        "status": "metadata-agreement-not-certification" if not mismatches
                  else "metadata-disagreement",
        "matching_fields": len(independent_facts) - len(mismatches),
        "compared_fields": len(independent_facts),
        "discrepancy_fields": mismatches,
        "g2_accuracy_gate": "pending",
    }


def main() -> int:
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--independent", type=Path, required=True)
    p.add_argument("--dosdis", type=Path, required=True)
    p.add_argument("--expected-source-sha256", required=True)
    args = p.parse_args()
    try:
        result = compare(
            read_report(args.independent),
            read_report(args.dosdis),
            args.expected_source_sha256,
        )
    except InvalidReport as exc:
        p.error(str(exc))  # only constant error messages, no private paths/bytes
    print(json.dumps(result, indent=2, sort_keys=True))
    return 0 if result["status"] == "metadata-agreement-not-certification" else 1


if __name__ == "__main__":
    raise SystemExit(main())
