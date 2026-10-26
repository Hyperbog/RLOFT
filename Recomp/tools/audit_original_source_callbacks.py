#!/usr/bin/env python3
"""Static, fail-closed inventory of ORIGINAL CODE.2 source callback wiring.

Not a dynamic/reachable whole-game call graph: C++ indirect calls,
native device callbacks and source-machine branches need runtime evidence.
Use with the original immutable reconstructed Code2.cpp at HEAD.
"""
from __future__ import annotations
import argparse
import json
import re
from pathlib import Path

FUNCTION = re.compile(r"\bfunction_([0-9a-fA-F]{3,6})\s*\(")
CALLBACK = re.compile(r"\bcall_([0-9a-fA-F]{3,6})(?:_[A-Za-z0-9_]+)?\s*=")
SOURCE_STOPS = re.compile(r'\bstop\(\s*"([^"]+)"\s*\)')
UNCLOSED = ("_PENDING", "_NOT_CONNECTED", "_UNSUPPORTED",
            "_MISSING", "_INCOMPLETE", "_UNVERIFIED", "_REQUIRES_SOURCE")
GUEST_PATHS = (
    "Recomp/src/code2_startup_gate.cpp",
    "Recomp/src/original_code2_live_full_frame.inc",
    "Recomp/src/original_code2_live_level_transition.inc",
    "Recomp/src/original_code2_live_07938_restore.inc",
    "Recomp/src/original_code2_1f494_bridge.inc",
)
def line_number(s: str, pos: int) -> int:
    return s.count("\n", 0, pos) + 1

def matching_cpp_delimiter(source: str, opened: int,
                           opening: str, closing: str) -> int | None:
    """Match a C++ delimiter, skipping comments and ordinary string literals."""
    depth = 0
    i = opened
    n = len(source)
    while i < n:
        if source.startswith("//", i):
            end = source.find("\n", i + 2)
            i = n if end == -1 else end + 1
            continue
        if source.startswith("/*", i):
            end = source.find("*/", i + 2)
            if end == -1:
                return None
            i = end + 2
            continue
        c = source[i]
        if c in ("'", '"'):
            quote = c
            i += 1
            while i < n:
                if source[i] == "\\":
                    i += 2
                elif source[i] == quote:
                    i += 1
                    break
                else:
                    i += 1
            continue
        if c == opening:
            depth += 1
        elif c == closing:
            depth -= 1
            if depth == 0:
                return i
        i += 1
    return None

def canonical_direct_call_graph(source: str) -> list[dict]:
    """Lexically reconstruct direct calls from CODE2 functions.

    Indirect call_XXX edges remain distinct ABI dependencies, never
    silently treated as completed or resolved direct function calls.
    """
    graph: dict[str, dict] = {}
    for match in FUNCTION.finditer(source):
        name = match.group(1).lower()
        end_params = matching_cpp_delimiter(source, match.end() - 1, "(", ")")
        if end_params is None:
            continue
        tail = re.match(r"\s*(?:noexcept\s*)?\{", source[end_params + 1:])
        if tail is None:
            continue
        opening = end_params + 1 + tail.end() - 1
        closing = matching_cpp_delimiter(source, opening, "{", "}")
        if closing is None:
            continue
        body = source[opening + 1:closing]
        calls = sorted({m.group(1).lower() for m in FUNCTION.finditer(body)})
        callbacks = sorted(set(re.findall(
            r"\bcall_([0-9a-fA-F]{3,6})(?:_[A-Za-z0-9_]+)?\b", body)))
        record = graph.setdefault(name, {"source_address": name,
                                         "direct_callees": [],
                                         "callback_abi_addresses": []})
        record["direct_callees"] = sorted(
            set(record["direct_callees"]).union(calls))
        record["callback_abi_addresses"] = sorted(
            set(record["callback_abi_addresses"]).union(x.lower() for x in callbacks))
    return [graph[k] for k in sorted(graph)]

def catalog(root: Path) -> dict:
    root = root.resolve()
    canonical = root / "Code2/Code2.cpp"
    if not canonical.is_file():
        raise ValueError("canonical reconstructed Code2.cpp absent")
    original = canonical.read_text(encoding="utf-8")
    originals = sorted(set(m.group(1).lower() for m in FUNCTION.finditer(original)))
    if not originals:
        raise ValueError("canonical source contained zero function tokens")
    graph = canonical_direct_call_graph(original)
    if not graph:
        raise ValueError("canonical source contained no recognizable function bodies")
    sources = []
    wired = []
    unresolved = []
    explicit_null = []
    referenced = set()
    for rel in GUEST_PATHS:
        p = root / rel
        if not p.is_file():
            raise ValueError(f"missing retained source guest callback owner: {rel}")
        s = p.read_text(encoding="utf-8")
        local_ref = sorted(set(m.group(1).lower() for m in FUNCTION.finditer(s)))
        referenced.update(local_ref)
        sources.append({"file": rel, "bytes": p.stat().st_size,
                        "source_functions_referenced": local_ref})
        for m in CALLBACK.finditer(s):
            # A statement's RHS may wrap across lines; stop before the
            # next semicolon. We do not claim pointer reachability.
            semi = s.find(";", m.end())
            rhs = s[m.end():semi] if semi >= 0 else ""
            if len(rhs) > 1000:
                rhs = rhs[:1000]
            record = {"file": rel, "line": line_number(s, m.start()),
                      "source_address": m.group(1).lower(),
                      "expression": " ".join(rhs.split())}
            wired.append(record)
            if "nullptr" in rhs:
                explicit_null.append(record)
        for m in SOURCE_STOPS.finditer(s):
            stop = m.group(1)
            if any(part in stop for part in UNCLOSED):
                unresolved.append({"file": rel,
                                   "line": line_number(s, m.start()),
                                   "stop": stop})
    return {
        "schema": "rloft.recomp.static-original-source-callback-census.v1",
        "basis": "literal CODE.2 function tokens, guest callback assignments and fail-closed source guards; no dynamic-reachability claim",
        "canonical_function_address_tokens": len(originals),
        "canonical_function_bodies": len(graph),
        "canonical_direct_call_edges": sum(len(x["direct_callees"]) for x in graph),
        "canonical_callback_abi_occurrences": sum(len(x["callback_abi_addresses"]) for x in graph),
        "canonical_direct_call_graph": graph,
        "live_guest_source_function_references": len(referenced),
        "live_guest_callback_wirings": len(wired),
        "live_guest_unique_callback_addresses": len(set(w["source_address"] for w in wired)),
        "conditional_or_explicit_null_wirings": len(explicit_null),
        "unresolved_guard_sites": len(unresolved),
        "unresolved_unique_guard_names": len(set(x["stop"] for x in unresolved)),
        "owners": sources,
        "wiring": sorted(wired,key=lambda x:(x["file"],x["line"])),
        "conditional_or_explicit_null": sorted(explicit_null,key=lambda x:(x["file"],x["line"])),
        "fail_closed_guard_sites": sorted(unresolved,key=lambda x:(x["file"],x["line"])),
    }

def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path,
        default=Path(__file__).resolve().parents[2])
    parser.add_argument("--out", type=Path)
    args = parser.parse_args()
    try:
        obj = catalog(args.root)
    except (OSError, ValueError, UnicodeError) as exc:
        print(f"ORIGINAL_SOURCE_CALLBACK_CENSUS_ERROR={exc}")
        return 1
    if args.out:
        args.out.parent.mkdir(parents=True,exist_ok=True)
        args.out.write_text(json.dumps(obj,indent=2)+"\n",encoding="utf-8")
    print("ORIGINAL_SOURCE_CALLBACK_CENSUS"
          f" canonical_function_tokens={obj['canonical_function_address_tokens']}"
          f" source_function_bodies={obj['canonical_function_bodies']}"
          f" source_direct_edges={obj['canonical_direct_call_edges']}"
          f" source_callback_abi_mentions={obj['canonical_callback_abi_occurrences']}"
          f" live_source_function_refs={obj['live_guest_source_function_references']}"
          f" callback_wirings={obj['live_guest_callback_wirings']}"
          f" unique_callback_addresses={obj['live_guest_unique_callback_addresses']}"
          f" nullable_wirings={obj['conditional_or_explicit_null_wirings']}"
          f" unresolved_guard_sites={obj['unresolved_guard_sites']}"
          " dynamic_callgraph=NOT_CERTIFIED")
    for row in obj["fail_closed_guard_sites"]:
        print(f"ORIGINAL_SOURCE_UNRESOLVED file={row['file']} line={row['line']} guard={row['stop']}")
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
