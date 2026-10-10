# Feedback for the Build Game Disassembler thread — CODE.1 / CODE.2 JSON
**Date:** 2026-10-10. **Status:** reproducible findings for the *user-uploaded exports*, not an assertion about the newest build of our separate disassembler.

The user confirmed Code1.json and Code2.json were produced by our own DOSDis disassembler. Both snapshots identify original game hashes and mark analysis as candidate-unverified, with instructionBytesIncluded=false. CODE.2 export timestamp: 2026-10-10 07:09:13 UTC; CODE.1: 07:07:57 UTC. Original binaries remain authoritative.

## Finding A: an original CODE.2 source island absent from candidate analysis

Original CODE.2 size 558,689; SHA-256 853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314; object-1 base 0x3AF24.

- Original **routine 1:0x130A0–0x13117**, 120 bytes, follows RET at 0x1309E and NOP at 0x1309F, and ends in RET immediately before 0x13118.
- Source call **1:0x130DE** (physical CODE.2 file offset 0x4E002) contains E8 AD C6 FF FF and targets F790. It passes the **selected item ID, known=0**, rather than the global held cursor. Before the call the routine compares raw signed WORD 9E33 against the previous *clamped* value and an upper bound at 258E8.
- **Code2.json does not decode any instruction/block in that routine.** Its classifiedRanges correctly records **[0x1309F,0x13118)** as 121 unknown-file-backed bytes (the 120-byte routine plus a one-byte NOP). This is *not* missing from the underlying byte coverage ledger.
- Default gapAnalysis.MinimumGapBytes=256 filters the 121-byte region out of the ranked Gaps list. The report contains 587 unknown-file-backed spans totaling 21,471 bytes; **51 spans from 64 to 255 bytes** are excluded by the default threshold. Only 18 gaps >=256 bytes are ranked.
- **Important reachability caveat:** a raw scan found *no direct E8 rel32 call or E9 rel32 jump targeting entry 0x130A0*. An indirect entry or other control path is unproven. Therefore this is a **candidate coverage and analyst-seed question, NOT proof of a disassembler bug, reachable gameplay or an instruction to decode all unknown spans**.

For the disassembler thread: try an optional analyst seed at 1:0x130A0 (retaining the normal LE entry) and inspect whether the separate routine and 130DE→F790 call decode consistently with original bytes. Do not elevate it to proved-executed merely because it decodes. Search for *supported* indirect reachability. Use a separate short-gap export with --min-gap-bytes 64 to surface this and analogous regions without changing the default report.

## Finding B: original guarded ten-way jump table has unproven candidate bound

Original CODE.2 object-1 source instructions:

~~~
0xF8BB  mov   al, [edi+eax+0x2F]
0xF8BF  sub   al, 2         ; 8-bit subtract/wrap
0xF8C1  cmp   al, 9
0xF8C3  ja    0xFB3D       ; unsigned out-of-range default
0xF8C9  movzx eax, al
0xF8CC  jmp   cs:[eax*4+0xF768]
~~~

Source table at 1:0xF768 (file offset 0x4A68C): ten DWORD targets **FA85×3, FB3D×4, F8D4×3**. The user-uploaded report correctly identifies the source 0xF8CC, the table 0xF768, four-byte entries, and all ten targets, yet tags it **index-bound-unproven**. All 31 CODE.2 jump table candidates and all five CODE.1 table candidates in these snapshots have that flag.

For the disassembler thread: re-export on its **latest code**, then check whether current guarded-domain recovery tracks the 8-bit SUB→CMP→unsigned JA→MOVZX chain. If already corrected, this is simply an older report. Otherwise use a **generic x86-32 LE synthetic fixture** proving indices 0..9, rejecting wrapping/out-of-range categories, and retaining the correct default edge. The current conservative annotation is not an incorrect target; it is incomplete proof of index bounds.

## Finding C: short-gap visibility, not a correctness bug

Independent reconciliation of classifiedRanges and gapAnalysis metadata:

| User-uploaded report | Unknown bytes | Total unknown spans | 64–255-byte unknown spans hidden by 256 threshold | Listed gaps ≥256 |
| --- | ---: | ---: | ---: | ---: |
| CODE.2 | 21,471 | 587 | 51 | 18 |
| CODE.1 | 15,253 | 231 | 37 | 17 |

For forensic sessions, run a second report with --min-gap-bytes 64 (and a sufficient --max-gaps) rather than treating the ranked long-gap list as complete. Optionally surface the omitted short-gap count in the GUI.

## Positive regression baselines — important

I independently checked every exported direct near FlowControl=Call instruction against the immutable original five-byte E8 rel32 encoding and the report's printed target:

- **CODE.2: 4,640 / 4,640 match.**
- **CODE.1: 1,205 / 1,205 match.**
- **Zero duplicate or overlapping instruction starts** in either report, with all lengths in x86 1–15 byte bounds.
- Unknown-span counts and total unknown bytes agree with the corresponding gap ledger in both reports.

Thus the selected routine omission is **not evidence of a general near-call decoder or exporter corruption**. Do not damage the working near-call path while improving conservative reachability.

## Recommended handoff and repro

This feedback was generated from original user files in /mnt/data, and relates to the CODE.2 LE 32-bit analyzer. In the other disassembler chat, ask it to compare these **exact addresses** with its current Build Game Disassembler branch, then add *source-agnostic* LE regression fixtures. Do not embed copyrighted CODE.1/CODE.2 or full disassembly in Git.

Relevant existing DOSDis implementation areas: Tools/Disassembler/src/DosDis.Core/ControlFlowAnalysis.cs (recursive seed discovery), ExecutableGapAnalysis.cs (ranked thresholds) and the candidate jump-table recovery/abstract-value pipeline. Existing disassembler documentation already describes optional --seed and --include-entry, and the gap analyzer supports --min-gap-bytes.

Original CODE.2 130A0 source follow-on test: DemoV1/tools/verify_original_f790_selected_poll.py. Original F790/candidate function differential: DemoV1/tools/analyze_original_f790_candidate_coverage.py. The latter now emits a structured 120-byte missing source island record.

**Do not change the game's F790 timing, native RNG, or completion percentage based solely on a candidate report. No Windows builds have been dispatched.**
