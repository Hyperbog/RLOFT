#!/usr/bin/env python3
"""Generate factual GitHub branch/CI/PR observations, never gameplay acceptance."""
from __future__ import annotations

import json
import os
from datetime import datetime, timezone
from pathlib import Path
from urllib.parse import quote, urlencode
from urllib.request import Request, urlopen

REPOSITORY = "Hyperbog/RLOFT"
WORKSTREAMS = (
    ("Decompilation", ("Decomp",)),
    ("Original-source recompilation", ("Recomp",)),
    ("DOS disassembler", ("DosDis",)),
    ("Ravenloft asset extractor", ("Ravenloft-Asset-Extractor",)),
    ("Original media", ("DnD-Classics",)),
)

def api(path: str, token: str, repo: str = REPOSITORY) -> object:
    url = f"https://api.github.com/repos/{repo}" + ("/" + path if path else "")
    headers = {
        "Accept": "application/vnd.github+json",
        "X-GitHub-Api-Version": "2022-11-28",
        "User-Agent": "rloft-documentation-health",
    }
    if token:
        headers["Authorization"] = f"Bearer {token}"
    with urlopen(Request(url, headers=headers), timeout=25) as response:
        return json.load(response)

def markdown(value: object) -> str:
    return str(value).replace("|", "\\|").replace("\r", " ").replace("\n", " ")

def collect(token: str, repo: str = REPOSITORY) -> dict:
    info = api("", token, repo)
    branches = api("branches?per_page=100", token, repo)
    prs = api("pulls?state=open&per_page=100", token, repo)
    if not isinstance(branches, list) or not isinstance(prs, list):
        raise ValueError("Unexpected GitHub branch/PR response")
    by_name = {x["name"]: x["commit"]["sha"] for x in branches}
    items = []
    for label, alternatives in WORKSTREAMS:
        current = next((name for name in alternatives if name in by_name), None)
        if current is None:
            raise ValueError(f"Required workstream branch missing: {label}")
        query = urlencode({"branch": current, "status": "completed", "per_page": 1})
        workflows = api("actions/runs?" + query, token, repo)
        runs = workflows.get("workflow_runs", [])
        items.append({
            "label": label, "branch": current,
            "sha": by_name[current], "last_run": runs[0] if runs else None,
        })
    known = {item["branch"] for item in items}
    return {
        "default_branch": info["default_branch"],
        "items": items,
        "additional": sorted(name for name in by_name if name not in known),
        "prs": [{"number": p["number"], "title": p["title"],
                 "head": p["head"]["ref"], "base": p["base"]["ref"],
                 "url": p["html_url"]} for p in prs],
    }

def render(data: dict, generated_at: str) -> str:
    lines = [
        "# RLOFT — live GitHub workstream status",
        "",
        f"**Observed:** {generated_at} UTC. **Repository default:** "
        f"`{markdown(data['default_branch'])}`.",
        "",
        "> **Automatically generated.** These are GitHub branch heads and the most "
        "recent *completed* Actions run associated with each branch, not proof of "
        "gameplay, original DOS parity, sound, visuals, or user acceptance. "
        "A run may be a documentation or tools job rather than a game build. "
        "For verified engineering boundaries, use [CURRENT_STATUS.md](CURRENT_STATUS.md).",
        "",
        "| Workstream | Branch | Current HEAD | Most recent completed Actions run |",
        "| --- | --- | --- | --- |",
    ]
    for item in data["items"]:
        branch, sha, run = item["branch"], item["sha"], item["last_run"]
        run_text = "No completed run found"
        if run:
            outcome = markdown(run.get("conclusion") or "unknown")
            rsha = str(run.get("head_sha") or "")
            target = f"[#{run['id']}]({run['html_url']})"
            run_text = f"{target}: {outcome} (source `{rsha[:10]}`)"
        branch_url = f"https://github.com/{REPOSITORY}/tree/{quote(branch, safe='')}"
        commit_url = f"https://github.com/{REPOSITORY}/commit/{sha}"
        lines.append(f"| {markdown(item['label'])} | [`{branch}`]({branch_url}) "
                     f"| [`{sha[:10]}`]({commit_url}) | {run_text} |")
    lines.extend(["", "## Open pull requests", ""])
    if data["prs"]:
        for pr in data["prs"]:
            lines.append(f"- [#{pr['number']}]({pr['url']}) "
                         f"{markdown(pr['title'])} "
                         f"(`{markdown(pr['head'])}` → `{markdown(pr['base'])}`)")
    else:
        lines.append("No open pull requests at observation time.")
    lines.extend(["", "## Additional branches", ""])
    lines.append(", ".join(f"`{markdown(x)}`" for x in data["additional"])
                 if data["additional"] else "None.")
    lines.extend([
        "", "## Provenance and limitations", "",
        "- Source: authenticated GitHub REST branch, Actions run, and pull-request APIs.",
        "- Current branch HEAD is independent of the commit compiled by a previous run.",
        "- A successful build/CTest does not establish a functioning game.",
        "- Original media and private machine-code reports are never read or published.",
        "- Generator: [docs/tools/update_live_status.py](tools/update_live_status.py).",
        "",
    ])
    return "\n".join(lines)

def main() -> None:
    token = os.getenv("GH_TOKEN") or os.getenv("GITHUB_TOKEN")
    if not token:
        raise SystemExit("GH_TOKEN or GITHUB_TOKEN is required; old report unchanged")
    output = Path("docs/LIVE_STATUS.md")
    rendered = render(collect(token),
                      datetime.now(timezone.utc).strftime("%Y-%m-%d %H:%M:%S"))
    output.parent.mkdir(parents=True, exist_ok=True)
    if not output.exists() or output.read_text(encoding="utf-8") != rendered:
        tmp = output.with_suffix(".md.tmp")
        tmp.write_text(rendered, encoding="utf-8")
        tmp.replace(output)
    print(f"Validated GitHub status for {len(WORKSTREAMS)} workstreams → {output}")

if __name__ == "__main__":
    main()
