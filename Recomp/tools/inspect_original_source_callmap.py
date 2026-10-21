#!/usr/bin/env python3
"""Read-only C++ source call graph for the current CODE.2 execution frontier.

This analyzes the reconstructed source text, NOT runtime execution or
original DOS instruction trace. It never writes back to a source file.
"""
import argparse
import pathlib
import re


def find_closing(text: str, opening: int, left: str, right: str) -> int:
    depth = 0
    i = opening
    state = "code"
    while i < len(text):
        c = text[i]
        nxt = text[i + 1] if i + 1 < len(text) else ""
        if state == "line":
            if c == "\n":
                state = "code"
        elif state == "block":
            if c == "*" and nxt == "/":
                state = "code"
                i += 1
        elif state == "string":
            if c == "\\":
                i += 1
            elif c == '"':
                state = "code"
        elif state == "char":
            if c == "\\":
                i += 1
            elif c == "'":
                state = "code"
        elif c == "/" and nxt == "/":
            state = "line"
            i += 1
        elif c == "/" and nxt == "*":
            state = "block"
            i += 1
        elif c == '"':
            state = "string"
        elif c == "'":
            state = "char"
        elif c == left:
            depth += 1
        elif c == right:
            depth -= 1
            if depth == 0:
                return i
        i += 1
    raise ValueError(f"Unbalanced source beginning at byte {opening}")


def definition(text: str, addr: str) -> tuple[str, str, int]:
    target = re.compile(r"\bfunction_" + re.escape(addr.lower()) + r"\s*\(")
    for match in target.finditer(text):
        opened = text.find("(", match.start())
        closed = find_closing(text, opened, "(", ")")
        following = text[closed + 1 : closed + 100]
        braces = re.match(r"\s*(?:noexcept\s*)?(?:->\s*[\w:<>]+\s*)?\{", following)
        if not braces:
            continue
        body_open = closed + 1 + braces.end() - 1
        body_close = find_closing(text, body_open, "{", "}")
        signature = " ".join(text[match.start():body_open].split())
        return signature, text[body_open + 1 : body_close], match.start()
    raise ValueError(f"Missing function_{addr} compiled-source definition")


def main() -> None:
    p = argparse.ArgumentParser()
    p.add_argument("source", type=pathlib.Path)
    p.add_argument("--functions", nargs="+", default=["20440", "1fc14", "20294", "20608"])
    args = p.parse_args()
    text = args.source.read_text(encoding="utf-8")
    for addr in args.functions:
        name = addr.lower()
        signature, body, pos = definition(text, name)
        calls = re.findall(r"\bfunction_([0-9a-fA-F]+)\s*\(", body)
        print(f"CODE2_CALLMAP address={name} position={pos} source_body_chars={len(body)}")
        print(f"CODE2_CALLMAP_SIGNATURE address={name} signature={signature[:220]}")
        print(f"CODE2_CALLMAP_CALL_ORDER address={name} direct_calls={','.join(calls[:96]) or 'NONE'} count={len(calls)}")
        print(f"CODE2_CALLMAP_START address={name} source_prefix={' '.join(body[:420].split())}")


if __name__ == "__main__":
    main()
