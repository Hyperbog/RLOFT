# CODE.2 Semantic Audit — Checkpoint 54

## Scope

Forward semantic inspection continued from `3968E` through historical Batch 139:

- `3968E..39751` — 196 bytes
- `39752..39836` — 229 bytes
- total newly inspected: 425 bytes

Cumulative semantic inspection is now 235,559 / 239,698 bytes = 98.2732%; 4,139 bytes remain uninspected. Next historical boundary: `39837`.

## Source identity

Immutable source remains `/mnt/data/CODE.2`, SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`. Object-1 remains SHA-256 `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Historical ownership

Batch-139 integration commit `b50507bab2cd9e9a61f51d2695d958f1a85ce7d7` owns `3968E..39836`: startup command-line duplication/argv publication plus tokenizer. `39837` was deliberately left for the next batch.

## Semantic result

No new defect is established in Batch 139.

`3968E` preserves the two-pass tokenizer contract: first pass counts command-line arguments and obtains the terminal source position, then allocation reserves the copied text plus `(argc+1)` vector slots. The vector begins with the published program-name pointer, parsed command-line entries follow, and a null pointer terminates the vector. The published argc includes argv[0]. Allocation failure clears the vector publications while preserving the computed argc publication behavior.

`39752` preserves the material distinctions of the encoded tokenizer: space/tab skipping, quoted versus unquoted token scans, quoted backslash escaping only for quote/backslash, unquoted backslash escaping for quote, in-place compaction only when the argv/output pass is active, source-token pointer publication after compaction, and terminal NUL handling.

The reconstruction remains intentionally low-level: the parser's second-pass mutation of the duplicated command-line buffer is observable and must not be replaced by a high-level token-vector abstraction during later cleanup.

## Existing repair ledger carried forward

No prior defect is closed by this checkpoint. Open/staged items still include the established repairs at `372F6`, `3736D`, `37433`, `37810`, `37D60`, `37EFA`; Batch-135 width/state/rounding defects; the substantial Batch-136 formatter-fidelity rewrite; the `3908C` incoming-carry ABI issue; and the `39589` branch-sensitive scratch-watermark defect.

## Next

Resume at `39837` using final explicit ownership rather than historical aggregate accounting.