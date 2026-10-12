# Optional REA analysis — Ravenloft asset extraction tools

This workstream uses the **single canonical shared analysis adapter**:
[ResearchInfra REA guide](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/ResearchInfra/REA.md)
and [Python CLI](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/ResearchInfra/rea_bridge.py).
The adapter is **not copied** to this branch, and no reverse-engineering
engine, MCP server or Node/npm dependency is installed into this application's
build. Keep the canonical branch available in a second checkout and run its
script with absolute paths to this project's own artifacts.

```sh
python3 /canonical/RLOFT/ResearchInfra/rea_bridge.py plan --project extractor-wpf-fluent --target /owned/extractor/publish/Extractor.exe --format auto --out /canonical/RLOFT/.rloft-private/rea/extractor.plan.json
```

The `plan` operation is offline, read-only and SHA-bound. An optional
`run --plan /canonical/RLOFT/.rloft-private/rea/....plan.json
--out /canonical/RLOFT/.rloft-private/rea/...inspect.json`
invokes a previously installed `rea` and appropriately configured Ghidra;
it is not run in this project's public CI. All derived original-game bytes
and decompiler observations remain private and ignored. For local storage
in this branch instead of canonical checkout, use `--checkout <branch-root>`
and an `--out` beneath its ignored `.rloft-private/`.

Use REA for authorized independent binary inspection of the extractor and original executable routines when relevant to asset formats. REA's native Ghidra provider does not decompile managed .NET code like native x86; consult REA's separate managed static inspector when examining WPF/.NET assemblies. Decoded original asset formats remain subject to the original Resources/ evidence and exact corpus tests.

Project progress gates are **not** advanced merely by REA analysis.
Original DOS parity and playable-game evidence remain independent.
