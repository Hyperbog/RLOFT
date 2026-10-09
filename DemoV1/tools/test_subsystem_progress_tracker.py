"""Validate per-subsystem evidence scores and committed scorecard totals.

This script uses only Python's standard library; CTest can invoke it
in a user-authorized manual run. It is deliberately independent of
Windows builds and never mutates any repository file.
"""

from __future__ import annotations

import csv
from collections import defaultdict
from decimal import Decimal, ROUND_HALF_UP
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SCORES = ROOT / "PROGRESS_SUBSYSTEMS.csv"
REPORT = ROOT / "PROGRESS_SUBSYSTEM_REPORT_20261009.md"
WEIGHTS = (10, 20, 20, 10, 40)
MARKS = ("source", "implementation", "live", "regression", "dos_acceptance")
GATES = (
    "Source mapping",
    "Native implementation",
    "Live runtime wiring",
    "Authored regression coverage",
    "Current-head DOS acceptance",
)
EXPECTED_COLUMNS = (
    "system",
    "subsystem",
    *MARKS,
    "evidence",
    "next_gate",
)


def pct(number: Decimal) -> str:
    return str(number.quantize(Decimal("0.1"), rounding=ROUND_HALF_UP)) + "%"


def main() -> None:
    report = REPORT.read_text(encoding="utf-8")
    with SCORES.open(newline="", encoding="utf-8") as stream:
        source = csv.DictReader(stream)
        assert tuple(source.fieldnames or ()) == EXPECTED_COLUMNS, (
            "Subsystem CSV schema changed without updating score rules"
        )
        data = list(source)

    assert len(data) >= 100, "Entire level development scope not represented"
    by_system: dict[str, list[tuple[dict[str, str], Decimal]]] = defaultdict(list)
    seen: set[tuple[str, str]] = set()
    component_sums = [0, 0, 0, 0, 0]
    for row in data:
        identity = (row["system"].strip(), row["subsystem"].strip())
        assert identity[0] and identity[1], "Empty system/subsystem label"
        assert identity not in seen, f"Duplicate subsystem: {identity}"
        seen.add(identity)
        assert row["evidence"].strip() and row["next_gate"].strip(), (
            f"Missing evidence or next acceptance gate: {identity}"
        )
        marks = []
        for idx, key in enumerate(MARKS):
            raw = row[key].strip()
            assert raw in {"0", "10", "20"}, (
                f"Non-categorical {key} score for {identity}: {raw!r}"
            )
            value = int(raw)
            marks.append(value)
            component_sums[idx] += value

        readiness = sum(
            Decimal(value * weight) / Decimal(20)
            for value, weight in zip(marks, WEIGHTS)
        )
        assert readiness <= Decimal(100)
        if marks[-1] == 0:
            assert readiness <= Decimal(60), (
                "Unaccepted DOS behavior scored too highly"
            )
        by_system[identity[0]].append((row, readiness))
        subtotal_marker = (
            f"| {identity[1]} | **{pct(readiness)}** | "
            f'{"\u002f".join(str(x) for x in marks)} |'
        )
        assert subtotal_marker in report, (
            f"CSV versus detailed report drift: {identity}"
        )

    assert len(by_system) >= 15, "Missing major engine system"
    total = Decimal(0)
    for system, subsystem_rows in by_system.items():
        count = len(subsystem_rows)
        subtotal = sum((s for _, s in subsystem_rows), Decimal(0))
        total += subtotal
        summary_marker = (
            f"| **{system}** | **{pct(subtotal / Decimal(count))}** "
            f"| {count} |"
        )
        assert summary_marker in report, (
            f"System rollup differs from CSV: {system}"
        )

    readiness = total / Decimal(len(data))
    assert f"**{pct(readiness)}**" in report, "Overall weighted readiness drift"
    for idx, gate in enumerate(GATES):
        gate_fraction = (
            Decimal(component_sums[idx])
            / Decimal(len(data) * 20)
            * Decimal(100)
        )
        contribution = gate_fraction * Decimal(WEIGHTS[idx]) / Decimal(100)
        marker = f"| {gate} | {pct(gate_fraction)} | {pct(contribution)} |"
        assert marker in report, f"Component gate summary drift: {gate}"

    assert (
        f"**{len(data)} named subsystems across {len(by_system)} major systems."
        in report
    ), "Headline baseline row count differs from CSV"
    print(
        f"Progress tracker valid: {len(by_system)} systems / "
        f"{len(data)} subsystems / weighted readiness {pct(readiness)}"
    )


if __name__ == "__main__":
    main()
