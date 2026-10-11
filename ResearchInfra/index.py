#!/usr/bin/env python3
"""Deterministic read-only research index for RLOFT's canonical reconstruction.

No inference is marked verified: parsing a C++ function or call creates only
source-derived observations. Original binaries are verified separately.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import re
import sys
from collections import Counter, deque
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SOURCE_PATHS = {"code1": "Code1/Code1.cpp", "code2": "Code2/Code2.cpp"}
ORIGINALS = {
    "code1": ("CODE.1", 277787, "fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed"),
    "code2": ("CODE.2", 558689, "853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"),
}
SCHEMA = "rloft-research-index-v1"
MASK = re.compile(r'//[^\n]*|/\*.*?\*/|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'', re.S)
# Function definitions only; an invocation alone is not evidence of definition.
DEF = re.compile(
    r"(?:^|[;}])\s*(?:\[\[nodiscard\]\]\s*)?"
    r"(?:(?:static|inline|constexpr|extern)\s+)*"
    r"(?:[A-Za-z_][\w:<>&*]*\s+){1,5}"
    r"function_([0-9a-fA-F]{2,8})\s*\(", re.M)
CALL = re.compile(r"\bfunction_([0-9a-fA-F]{2,8})\s*\(")


def digest(value: bytes) -> str:
    return hashlib.sha256(value).hexdigest()


def scrub(source: str) -> str:
    return MASK.sub(lambda m: "".join("\n" if c == "\n" else " " for c in m.group()), source)


def closing(src: str, start: int, left: str, right: str) -> int:
    depth = 0
    for i in range(start, len(src)):
        if src[i] == left:
            depth += 1
        elif src[i] == right:
            depth -= 1
            if depth == 0:
                return i
    raise ValueError("unclosed source delimiter")


def extract(source: str, module: str, path: str) -> list[dict]:
    clean = scrub(source)
    result = []
    for m in DEF.finditer(clean):
        logical = int(m.group(1), 16)
        open_paren = clean.find("(", m.start())
        close_paren = closing(clean, open_paren, "(", ")")
        suffix = clean[close_paren + 1:]
        # Supported modifiers before '{': fail closed for declarations or
        # unknown declarator forms rather than inventing function bodies.
        tail = re.match(r"\s*(?:noexcept(?:\s*\([^()]*\))?\s*)?(\{|;)", suffix)
        if not tail or tail.group(1) != "{":
            continue
        open_brace = close_paren + 1 + tail.start(1)
        close_brace = closing(clean, open_brace, "{", "}")
        start = m.start()
        # Leading delimiter from the regex is not part of the function.
        if clean[start:start + 1] in ";}":
            start += 1
        raw = source[start:close_brace + 1]
        body = clean[open_brace + 1:close_brace]
        result.append({
            "id": f"{module}:{logical:05x}",
            "module": module,
            "offset": logical,
            "source": path,
            "line_start": source.count("\n", 0, start) + 1,
            "line_end": source.count("\n", 0, close_brace) + 1,
            "sha256": digest(raw.encode("utf-8")),
            "calls": sorted({f"{module}:{int(x,16):05x}" for x in CALL.findall(body)}),
            "evidence": "source_structure_only",
        })
    # The actual CODE.2 reconstruction contains more than one source-level
    # wrapper for some original logical addresses (e.g. 29930). Retain both
    # concrete definitions, but never silently choose one on an ambiguous call.
    frequency = Counter(f["id"] for f in result)
    ordinal: dict[str, int] = {}
    for f in result:
        address = f["id"]
        if frequency[address] > 1:
            ordinal[address] = ordinal.get(address, 0) + 1
            f["id"] = f"{address}#variant{ordinal[address]}"
            f["logical_address_id"] = address
            f["address_alias_ambiguous"] = True
    return sorted(result, key=lambda v: (v["module"], v["offset"], v["id"]))


def build(root: Path) -> dict:
    entries = []
    sources = []
    for module, relative in SOURCE_PATHS.items():
        path = root / relative
        payload = path.read_bytes()  # No fallback to stale/different files.
        text = payload.decode("utf-8")
        sources.append({"module": module, "path": relative,
                        "sha256": digest(payload), "bytes": len(payload)})
        entries.extend(extract(text, module, relative))
    ids = {f["id"] for f in entries}
    ambiguous_addresses = {f["logical_address_id"] for f in entries
                           if f.get("address_alias_ambiguous")}
    edges = []
    unresolved = []
    ambiguous = []
    for f in entries:
        for target in f["calls"]:
            edge = {"caller": f["id"], "callee": target}
            if target in ambiguous_addresses:
                ambiguous.append(edge)  # Address alias: caller cannot pick an overload.
            elif target in ids:
                edges.append(edge)
            else:
                unresolved.append(edge)  # Cannot claim definition or reachability.
    return {
        "schema": SCHEMA,
        "authority": "source_structure_only_not_original_binary_or_DOS_acceptance",
        "sources": sources,
        "original_identities": [{"module": k, "name": n, "bytes": size, "sha256": sha}
                                for k, (n, size, sha) in ORIGINALS.items()],
        "functions": entries,
        "edges": sorted(edges, key=lambda x: (x["caller"], x["callee"])),
        "unresolved_calls": sorted(unresolved, key=lambda x: (x["caller"], x["callee"])),
        "ambiguous_calls": sorted(ambiguous, key=lambda x: (x["caller"], x["callee"])),
    }


def save(index: dict, path: Path) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(index, sort_keys=True, indent=2) + "\n", encoding="utf-8")


def read_index(path: Path) -> dict:
    doc = json.loads(path.read_text(encoding="utf-8"))
    if doc.get("schema") != SCHEMA:
        raise ValueError("unrecognized index schema")
    return doc


def verify_orig(root: Path, module: str) -> dict:
    name, size, expected = ORIGINALS[module]
    p = root / name
    data = p.read_bytes()
    actual = digest(data)
    if len(data) != size or actual != expected:
        raise ValueError(f"{name}: source identity mismatch; refusing original-byte claim")
    return {"module": module, "file": name, "bytes": size, "sha256": actual,
            "result": "original_identity_verified_only"}


def lookup(index: dict, key: str) -> dict:
    key = key.lower()
    funcs = {f["id"]: f for f in index["functions"]}
    if key not in funcs:
        variants = sorted(f for f in funcs if f.startswith(key + "#variant"))
        if variants:
            raise ValueError(f"{key}: ambiguous source-address overloads: {variants}")
        raise ValueError(f"{key}: no indexed function (not proof the original code lacks it)")
    incoming = sorted(e["caller"] for e in index["edges"] if e["callee"] == key)
    outgoing = sorted(e["callee"] for e in index["edges"] if e["caller"] == key)
    dangling = sorted(e["callee"] for e in index["unresolved_calls"] if e["caller"] == key)
    ambiguous = sorted(e["callee"] for e in index["ambiguous_calls"] if e["caller"] == key)
    return {"function": funcs[key], "callers": incoming,
            "callees": outgoing, "unresolved_callees": dangling,
            "ambiguous_callees": ambiguous}


def impact(index: dict, key: str, limit: int = 1000) -> dict:
    info = lookup(index, key)
    parents: dict[str, list[str]] = {}
    for edge in index["edges"]:
        parents.setdefault(edge["callee"], []).append(edge["caller"])
    seen = {key}
    queue = deque([key])
    while queue:
        for caller in sorted(parents.get(queue.popleft(), [])):
            if caller not in seen:
                seen.add(caller)
                if len(seen) > limit:
                    raise ValueError("impact limit reached: reduce scope explicitly")
                queue.append(caller)
    return {"target": key, "potentially_affected_callers": sorted(seen - {key}),
            "assessment": "static_source_calls_only_no_indirect_or_runtime_proof",
            "local": info}


def packet(index: dict, root: Path, key: str, max_chars: int = 12000) -> dict:
    info = lookup(index, key)
    f = info["function"]
    source_meta = next(s for s in index["sources"] if s["module"] == f["module"])
    raw = (root / f["source"]).read_bytes()
    if digest(raw) != source_meta["sha256"]:
        raise ValueError("source changed since indexing; rebuild index")
    lines = raw.decode("utf-8").splitlines(keepends=True)
    snippet = "".join(lines[f["line_start"] - 1:f["line_end"]])
    if len(snippet) > max_chars:
        snippet = snippet[:max_chars]
        clipped = True
    else:
        clipped = False
    return {**info, "source_excerpt": snippet, "truncated": clipped,
            "original_binary_status": "not_checked_by_source_index",
            "source_sha256": source_meta["sha256"]}


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("action", choices=["build", "query", "impact", "packet", "verify-media"])
    parser.add_argument("--root", type=Path, default=ROOT)
    parser.add_argument("--output", type=Path, default=Path("build/research/index.json"))
    parser.add_argument("--key", help="e.g. code2:353a0")
    parser.add_argument("--module", choices=sorted(ORIGINALS), help="original module for verify-media")
    args = parser.parse_args(argv)
    try:
        if args.action == "build":
            obj = build(args.root)
            save(obj, args.output)
            summary = {"index": str(args.output), "functions": len(obj["functions"]),
                       "edges": len(obj["edges"]),
                       "unresolved_calls": len(obj["unresolved_calls"]),
                       "ambiguous_calls": len(obj["ambiguous_calls"]),
                       "status": obj["authority"]}
            print(json.dumps(summary, sort_keys=True))
        elif args.action == "verify-media":
            if not args.module:
                parser.error("--module required for verify-media")
            print(json.dumps(verify_orig(args.root, args.module), sort_keys=True))
        else:
            if not args.key:
                parser.error("--key is required")
            obj = read_index(args.output)
            result = {"query": lookup, "impact": impact,
                      "packet": lambda o, k: packet(o, args.root, k)}[args.action](obj, args.key)
            print(json.dumps(result, sort_keys=True, indent=2))
        return 0
    except (OSError, ValueError, UnicodeError, KeyError) as exc:
        print(f"RLOFT_RESEARCH_ERROR: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
