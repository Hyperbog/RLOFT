# Optional REA analysis — NativeEngine independent native executable

This workstream uses the **single canonical shared analysis adapter**:
[ResearchInfra REA guide](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/ResearchInfra/REA.md)
and [Python CLI](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/ResearchInfra/rea_bridge.py).
The adapter is **not copied** to this branch, and no reverse-engineering
engine, MCP server or Node/npm dependency is installed into this application's
build. Keep the canonical branch available in a second checkout and run its
script with absolute paths to this project's own artifacts.

```sh
python3 /canonical/RLOFT/ResearchInfra/rea_bridge.py plan --project native-engine --target /owned/NativeEngine/build/game.exe --format auto --out /canonical/RLOFT/.rloft-private/rea/native.plan.json
```

The `plan` operation is offline, read-only and SHA-bound. An optional
`run --plan /canonical/RLOFT/.rloft-private/rea/....plan.json
--out /canonical/RLOFT/.rloft-private/rea/...inspect.json`
invokes a previously installed `rea` and appropriately configured Ghidra;
it is not run in this project's public CI. All derived original-game bytes
and decompiler observations remain private and ignored. For local storage
in this branch instead of canonical checkout, use `--checkout <branch-root>`
and an `--out` beneath its ignored `.rloft-private/`.

REA may inspect local native-engine objects/EXEs and help cross-check bounded original routines, but is not a substitute for user-original asset or framebuffer/behavior oracles.

Project progress gates are **not** advanced merely by REA analysis.
Original DOS parity and playable-game evidence remain independent.
