# Native reconstruction — continuous improvement and repeatable research

**Owner:** `demo-v1/DemoV1/` · **Scope:** process/tooling only. This supplements, but never overrides, the [original-execution fidelity charter](ORIGINAL_EXECUTION_FIDELITY_CHARTER.md) and [integration contract](CONTINUOUS_GAME_INTEGRATION_CONTRACT_20261009.md). No gameplay behavior is authorized by this page.

## Start each research batch with evidence, not recollection

1. Fetch **live** `demo-v1` HEAD and read the latest native README, integration contract, relevant original-byte audit, and CODE.1/CODE.2 continuity. Record the head and original-file identities. Use [the central index](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/docs/INDEX.md) only to find the owning documents.
2. Run the **read-only baseline tool** on a local checkout:

   ```sh
   python DemoV1/tools/research_preflight.py --repo-root . \
      --original-code2 /path/to/CODE.2 --original-sintab /path/to/SINTAB \
      --expected-head <PINNED_HEAD_SHA> --json
   ```

   Other optional flags are `--original-code1` and `--require-clean`. The script detects missing continuity documents, duplicate CMake test names, a stale Git HEAD and mismatched original-file hashes. Its CTest number is a **source declaration count**, not a green Windows test report. The original binaries stay off Git.
3. Establish **one bounded uncertainty**: the original routine/data record, exact original version, caller/callee boundary, unverified assumptions, native consumer and minimum proof required. Search the existing cached decomp/disassembler indexes and targeted proof scripts **before** running a fresh disassembly. Do not replicate already proven analysis.
4. Reuse one small original-byte oracle for the affected routine: whole bounded body/RET where independently identified, exact opcode windows and branch/call targets, boundary cases and mutation effects. Distinguish literal byte scans from actual instruction decoding and avoid claiming indirect call or segment-alias closure without runtime evidence.
5. Run the smallest **local targeted test first**, then broader portable tests when justified. Stop after two corrective retries, report the exact failing assertion and isolate the blocker. No test is a substitute for matched DOS state, raster, IRQ, RNG or input comparisons.
6. Commit a narrow source-verified batch with its regression, original provenance, open dependencies and acceptance category. Check live HEAD again immediately before updating a branch. If it advanced, re-read changed paths and reconcile, rather than overwriting another workstream. Avoid merging divergent branches wholesale.
7. Keep **four independent labels** in the handoff: original-byte/source proof; locally executed regression; exact-sha Windows Full build/CTest; original-DOS execution acceptance. Source registrations do not count as passing tests. Native engineering readiness is not whole-game completeness. Windows Full CI remains **manual, per-run owner-authorized**.

## Learning loop and throughput metric

At the end of each batch, add a concise note to the owning subsystem audit or integration contract with these fields: **original question**, **new verified fact**, **reusable test/tool**, **remaining blocker**, **source SHA**, **local test result**, and **Windows/DOS gates**. On the next batch, first check whether the reusable asset eliminates a repeated manual step. Prefer measurable throughput gains over broader unsupported claims.

| Bottleneck | Durable improvement | Measurable outcome |
| --- | --- | --- |
| Stale branch while parallel chats commit | Pinned HEAD preflight and compare-before-update | Fewer lost or conflicting edits |
| Repeat checks of original identities | Chunked SHA-256 verifier for supplied original versions | One deterministic check per run |
| Confusion over source vs CTest acceptance | Source declaration count plus explicit untested gates | No registry count reported as Windows success |
| Redoing disassembly and lookups | Reuse original offset indexes and machine-byte oracles | Fewer repeated source scans |
| Lost handoff detail | Persist proof and unresolved blockers in owning Git files | Next chat can resume from exact source evidence |

**Initial process-check baseline:** `test_research_preflight.py` passed six local unit tests during authoring. This validates the new preflight script's isolated behavior only; it is **not** a current-source C++ build, Windows Full run or original DOS parity certification. Future iterations should record actual elapsed time, duplicate-work avoided and regression failures when such measurements exist, rather than inventing percentages.

**Model limitation:** The assistant's underlying model weights do not self-train on each project step. Reliability gains here come from durable source evidence, explicit corrections, tested automation and reproducible handoffs, rather than claims of automatic model retraining or unattended work.
