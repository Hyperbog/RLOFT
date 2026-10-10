# Level22 F790: deterministic feedback loop, original-code pattern recognition and bounded agent workflow
**Date:** 2026-10-10. **Branch:** `demo-v1`. **Authority:** original user-supplied CODE.2 and original source game assets. Disassembler JSON is explicitly candidate-unverified.

## 1. New automatic development feedback mechanism

The committed `DemoV1/tools/run_f790_source_feedback_loop.py` is a deterministic, fail-closed **on-demand** orchestrator. It runs eight existing dedicated source-verification programs as subprocesses and reports each check's scope, status, output and failure code. It can also run seven or eight original-data checks when supplied exact private CODE.2 / OBJECTS.DAT / Code2.json files. It produces machine-readable JSON via `--json` or `--write-json <path>`, so an agent can consume failures and select the next narrowly justified patch.

Typical invocations from the checked-out repository root:

```bash
python DemoV1/tools/run_f790_source_feedback_loop.py
python DemoV1/tools/run_f790_source_feedback_loop.py \
  --code2 /path/to/CODE.2 --objects /path/to/OBJECTS.DAT \
  --candidate-report /path/to/Code2.json --json
```

The coordinator runs the following contracts:
- Original inventory F790 cadence and 17 direct callers.
- Original 112EC/2932C planar saved-background source signatures.
- Original scribe success/rejection/deadline-expiry event windows.
- Original F790 item-category dispatch.
- Original F790 item-record branch arithmetic.
- Original LE static strings, container descriptors and T decoder contracts.
- Original F790 runtime event/held-argument ownership.
- Original CODE.2 vs candidate-disassembler F790 function-parent coverage.

**Safety properties:** the runner makes no edits, no network calls, no GitHub API requests, no Windows or Linux builds, no game RNG calls and no DOS parity claim. It exits nonzero if any contract fails. Default source-only checks need no proprietary original binaries in CI. The optional original-file mode validates fingerprints and original instruction bytes without uploading or committing game files.

This is an *interactive/on-demand* agentic workflow, not an asynchronous background service or a surprise GitHub Actions build. The user's manual-build policy remains unchanged.

## 2. Binary-to-candidate pattern recognition

The original executable was scanned directly for E8-rel32 calls into logical CODE.2 `F790`; **17 exact machine-code callers** exist. A separate, independent analysis of user-supplied `Code2.json` (66,321 candidate instructions, 833 function candidates; report declares `analysisStatus=candidate-unverified` and omits instruction bytes) associated **16/17 callsites** with candidate function parents. Source-backed candidate groups:

| Candidate function entry | Original direct F790 E8 callsites |
| --- | --- |
| `112EC` | `1134C` |
| `11630` | `1173B`, `1178E` |
| `117A0` | `1184A`, `11882`, `118FF`, `119FC` |
| `11AD8` | `11B63` |
| `11C3C` | `11F47` |
| `12724` | `127EC`, `128A3` |
| `12988` | `12AFA`, `12BA4` |
| `12C20` | `12D50`, `12DFD` |
| `13118` | `131BB` |
| **Not represented by candidate block/function ownership** | **`130DE`** |

`130DE` contains a genuine original `E8` directly targeting `F790`, with the previously verified source argument pair `push 0; push ebx`. Its absence from the candidate function-parent report is an **analyzer coverage gap**, not proof the original code is unused, nor permission to fabricate native event behavior. Do not use a candidate function parent's descriptive name as proof of its full execution semantics. The new `analyze_original_f790_candidate_coverage.py` checks expected patterns in source-only mode and performs the byte-accurate vs candidate-block differential when private original files are supplied; optional `--json` returns `17 total, 16 candidate matched, 130DE missing`.

The differential locally **passed** against immutable original CODE.2 and user-provided Code2.json. An explicit gap like this is actionable feedback for the separate disassembler development thread.

## 3. Original source feedback event state machine

Independent exact CODE.2 byte windows at `116F5,11717,1176A,13184,131AA` and rel32 calls `1173B,1178E,131BB` prove:
- Successful mage scroll scribe: `6DAC=1`, `6DA8=30700()+0x36`, pending `F790` at `1173B`.
- Rejected scribe (capacity or already known): same pending/deadline side effects, pending `F790` at `1178E`.
- Expiry: only while pending and strictly **unsigned `30700()>6DA8`**; clear `6DA8` and `6DAC` *before* ordinary-held `F790` at `131BB`. Equal ticks and unsigned wrap do **not** count as expiry.

New `OriginalF790FeedbackEdges` maps the **existing native InventoryOrdinarySwapStatus** (not a second status enumeration) into exactly these three original E8 sites, and rejects inconsistent kind, deadline or pending state. `OriginalF790FeedbackSourceSnapshot` owns exact `759C` feedback message plus item ID and known flag at the original caller boundary; a success obtains its original spell name from `OriginalTTextArchive`, whereas rejection uses the native source-proven message without adding a name. The after-expiry event records empty pending text. It is not safe to store `string_view` across moves; the model constructs a new view only for synchronous dispatch.

Two new C++ source-event regressions cover both rejected outcomes, success, consumed held scrolls, snapshot ownership after mutable inventory changes, equality, unsigned wrap, expiry edge and 1,000 non-event iterations. The new Python contract revalidates five original windows and three calls and guards against premature native host linkage. **These models are isolated**: they do NOT run in `win32_main.cpp`.

## 4. Truth-in-status gates

Four new CTests have been registered since 202: feedback edge state regression, feedback snapshot regression, Python original-code feedback contract, and Python candidate function coverage differential. **206 registered CTests with Python available.** The optional-original Python windows (**5/5** and **3/3** calls) and Code2.json parent differential (**16/17**) were executed locally and passed. The new C++ tests are registered **but not built or executed on Windows**. The on-demand aggregate runner itself is checked in but not treated as a CTest or evidence of a completed Windows run.

Last completed manually authorized Windows Full remains #651 (**182/182 passing on an earlier commit**). The newest live plain-text F790 improvement and source-only feedback models have not passed a current Windows compile. Original-DOS visual, UI event cadence and shared RNG parity remain unaccepted; engineering readiness and gameplay percentage unchanged.

Next agent steps: use the callsite family pattern table to trace the outstanding 14 original E8 sites and their exact source triggering conditions; instrument source event logs without touching the 70-Hz render loop; attach the existing shared gameplay RNG only when event ownership is proved; then request a manual Windows build and user-assisted DOS/native image/timing comparison. Level22 must remain the sole gameplay target before independent Level23. No speculative modernization.
