# CODE.2 original indirect callback table resolution — 2026-10-11

The original CODE.2 runtime has two original indirect `CALL EAX` sites: `3961F` for the ascending initializer table and `3966F` for termination callbacks. These are **static indirect-call target candidate sets**, not evidence of observed DOS execution or immutable runtime callback table state.

## Full-file authenticated original observation

Original CODE.2: 558,689 bytes, SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.

- `395E2..3962C`: startup initializer dispatcher SHA-256 `4372327a1ecb72c421622e01ebc4cab18320202d60e5afc3a8aef6c39268e410`, indirect `FF D0` at `3961F`.
- `3962D..3967B`: shutdown dispatcher SHA-256 `ddbb043ce125bac62303954e01d5fa2f1d74667c9c588acecf089c18dfc2bbfe`, indirect `FF D0` at `3966F`.
- Four six-byte original initialization records at `DGROUP 12720..12737`: flags `00`, priority `20`, targets `3968E,3A1B3,3A2AD,3A16E`. With the independently verified equal-priority `JA` tie rule, callback execution starts from the last target.
- **Termination record at `DGROUP 12738..1273D`**: `00 20 28 A2 03 00` → flags=0, priority=0x20, callback target **`3A228`**. Critically, only **five bytes** `00 20 28 A2 03` are physically backed in the original file. The sixth byte is virtual LE object **zero fill** at DGROUP offset `1273D`. The original file physically ends at data backing size **0x1273D** and has virtual data size **0x2DD10**. Treating the missing physical byte as a missing record would give the wrong control-flow graph.
- The canonical CODE.2 source independently contains `function_3a228` (stream close and exit callback), matching the original address.

`ResearchInfra/code2_callback_tables.py` authenticates the complete original file, validates LE code object construction, original scheduler span hashes, exact indirect call instructions and both table layouts, then emits metadata-only JSON with per-record physical/zero-fill provenance. Tests verify the partial source backing without any game media. No executable bytes are redistributed.

Example:

```sh
python3 ResearchInfra/code2_callback_tables.py --code2 /private/CODE.2 --out build/research/code2_callbacks.json
python3 -m unittest discover -s ResearchInfra -p 'test_*.py' -v
```

The two indirect calls have **one known original static callback-table configuration**. Actual targets can change if the game mutates the table, runtime priority flags or memory mappings. Therefore the output intentionally retains `runtime_target_proven=false`; it does not mark the original DOS call sites fully resolved in a global CFG.

[Canonical post-initializer executable source integration](../Recomp/INITIALIZER_20608_INTEGRATION_20261011.md) separately executed the four source-owned initializer callbacks with bounded test fixtures and stopped at game entry `20608`. The shutdown `3A228` callback has not yet been executed as original DOS or native gameplay.
