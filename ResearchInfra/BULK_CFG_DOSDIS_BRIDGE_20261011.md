# Research Infrastructure: bulk original LE survey and DOSDis evidence bridge — 2026-10-11

**Owner:** `ResearchInfra/` on `code1-decompilation`. These are additional deterministic **research tools**, not new end-user DOSDis interfaces, native game code or original DOS execution.

## What was implemented

1. `bulk_cfg.py` uses the canonical reconstructed C++ source index as a set of **source-address candidates**. It verifies the exact original SHA-256 of both owner-local CODE.1/CODE.2, loads the original LE code objects, deduplicates overloaded source address variants, partitions sample windows by the next source address and a strict per-window 160-byte default cap, then invokes the separately maintained GNU binutils x86-32 original-code decoder and candidate CFG builder. The 2,500-label and 500,000-byte hard limits **fail closed**, never silently truncate. Invalid/out-of-object source labels and decoding conflicts are preserved as diagnostics. The output has hashes and metadata, not the bytes of the game. **The source index is not proof of function starts or DOS reachability.**
2. `dosdis_bridge.py` understands the actual `DosDis.Core.AnalysisReportExport.BuildJson` **`dosdis-analysis-report`, schema version 1**, with `FlowAddress(Object,Offset)`, `FlowInstruction(Address,Length,Assembly)` and `FlowEdge(Source,Destination,Kind)`. This is a read-only developer integration, not a second DOSDis application. It independently authenticates original media before accepting a DOSDis report, requires the full executable's SHA-256 to match the export's `source.sha256`, validates original object-relative offsets, unique instruction starts and x86 instruction lengths (1–15), and compares independent original GNU decoded window lengths/optional original-byte representations, plus directly observed `call`, `branch-taken`, `jump` targets. Unobserved candidate instructions are **unknown**, not classified as errors. Disagreements are **review observations**, not proven analyzer bugs.
3. `test_bulk_cfg.py` and `test_dosdis_bridge.py` test source-overload handling, bounded truncation failures, bad original-media rejection, exact JSON schema, SHA rejection, malformed object/offset rejection, invalid source claims and duplicate instruction refusal.

## Independent local original-media survey (not a CI/gameplay acceptance)

Using the owner-supplied authentic CODE.1/CODE.2 in the current private task container, a **stride-five reproducible sample of 310 distinct address names mentioned by canonical C++** was decoded in original LE code objects. Unlike the earlier **15 SHA-pinned source-certified witnesses**, these addresses are **only source mentions**, not independently verified function boundaries or call-target validation; the output does not label them as certified entries. Each was bounded by a 160-byte limit and the next selected candidate.

| Measurement (candidate-only) | CODE.1 | CODE.2 | Total |
| --- | ---: | ---: | ---: |
| Selected source-mention seeds | 113 | 197 | **310** |
| SHA-backed original-code bytes sampled | 16,853 | 31,258 | **48,111** |
| Linear decoder instruction candidates | 6,309 | 10,356 | **16,665** |
| Direct-edge static traversal instruction candidates | 2,998 | 6,548 | **9,546** |
| Candidate basic blocks | 696 | 1,532 | **2,228** |
| Conflicting decoder/target observations | 14 | 58 | **72** |
| Unresolved indirect-edge sites | 6 | 9 | **15** |
| Decode subprocess failures | 0 | 0 | **0** |

The report is available as a task-local JSON artifact `RLOFT_original_310_source_mention_survey.json`, intentionally **not committed** to the source repository. Counts are candidate static-sweep diagnostics on a sample: they are *not* original function coverage percentages, executed instruction counts, certified code/data partition or correct disassembly across the two code objects. The 72 conflicts and 15 indirect sites remain unresolved.

### Original identity

- CODE.1: 277,787 bytes; SHA-256 `fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed`
- CODE.2: 558,689 bytes; SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`

## CI verification

[Linux source-only run #38184342710](https://github.com/Hyperbog/RLOFT/actions/runs/38184342710) passed at commit `d69cea83fea8ac56336967f5e7df41beaac9aa2f`: **29 Research Infrastructure tests**, **65 SharedDecomp tests**, plus regenerated canonical source index: 1,560 function records / 961 statically resolvable source-call edges.

**No user-original CODE assets are shipped to the Linux CI runner.** The real original LE survey above was local, not CI-verified. The DOSDis adapter's synthetic schema and negative cases are tested, but **a fresh exact-version DOSDis Windows export has not yet been supplied and compared**. Therefore the number of actual independent DOSDis-vs-binutils agreements or disagreements is **not known** and must not be invented.

## Commands (from canonical source checkout)

```sh
# Optional original files are installed privately under /path/to/originals
python3 ResearchInfra/index.py build --root . --output build/research/index.json
python3 ResearchInfra/bulk_cfg.py --original-root /path/to/originals --source-root . --index build/research/index.json --out build/research/bulk_original_candidate_survey.json

# Exports can come from an authorized DOSDis desktop's original-file report.
# CLI here is a developer-only evidence adapter, not another released app:
python3 ResearchInfra/dosdis_bridge.py --module code2 --original-root /path/to/originals --dosdis /private/Code2_dosdis_report.json --out build/research/dosdis_vs_gnu_original.json
python3 -m unittest discover -s ResearchInfra -p 'test_*.py' -v
```

Both optional commands refuse wrong-version original game media. Original executable content must not be committed. Disassembler outputs require an independently identified source SHA and exact export version; do not reuse 2026-10-10 early screenshots/JSON snapshots as evidence of the current DOSDis application.

## Next high-leverage work

- Reconcile independently authenticated DOSDis original instruction/edge export against GNU original witness windows, including `0x130A0` orphan candidate and `0xF8CC` guarded original switch without promoting runtime reachability.
- Partition and independently verify the **72 conflict observations** from the 310-sample survey; use original CODE runtime semantics, relocation-target tables and DOSDis protected-data regions rather than blindly forcing graph closure.
- Import known source-backed CODE.1 padding/table regions and CODE.2 protected switch table ranges into the survey to reduce false decoding.
- Continue canonical DOS runtime initialization `3A2AD`/allocator `37B42`, resolving indirect callback targets without substituting synthetic successful services.
