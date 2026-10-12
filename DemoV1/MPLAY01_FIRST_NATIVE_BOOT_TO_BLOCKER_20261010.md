# M-PLAY-01 — First native boot-to-blocker playability checkpoint

**Owner:** `demo-v1` native game-engine workstream  
**Type:** diagnostic field-test milestone within the **complete faithful original-game reimplementation**; **not** a partial-game release, DOS parity certification, a separate "Phase I", or permission to modernize.  
**Established:** 2026-10-10. **Status:** defined; latest-source executable and first user playthrough **not yet accepted**.

## Goal

Give the player a reproducibly identified Windows build of the **existing native engine**, run it against their **legally owned, original** Ravenloft installation, follow the **ordinary startup and character/gameplay path** as far as it works, and record the **first blocking divergence** plus all previously working interactions. Do not require completion of the whole game before collecting this evidence. Do not substitute an artificial warp, test fixture, invented event or synthetic asset for actual progression.

This checkpoint is an observation and defect-triage gate. The separate project charter's **Level 22 complete/source-faithful acceptance before independent Level 23 work** remains binding.

## Existing baseline (not a current-SHA build)

- Last independently accepted native Windows Full [#658](https://github.com/Hyperbog/RLOFT/actions/runs/38085278540) compiled source checkout `ba3dc29e42d93e87a147b4b909c52167dbd3b362`, passed **214/214** CTests, published `Ravenloft-Demo-v1-Windows-x64`, and verified original `RES4[655]` terrain in 64/64 subsequent **native** open-field captures.
- Later source work has expanded the source/test catalog without another accepted current-checkout Windows Full run. Treat #658 as an **older baseline**, not a binary containing the latest source or proof of end-to-end gameplay.
- The existing Win32 host normally attempts an owned installation via saved path/autodiscovery, otherwise prompts for the folder. It accepts one optional installation path. It checks a writable `CHARS`: a present party goes directly toward exploration; a missing party enters the intro/title/character-creation flow, whose completed `CHARS` transitions to Level 22. This is **source wiring**, not a confirmed field playthrough.
- The existing `--explore-level22` / `Diagnose-Level22.cmd` launch is **opt-in developmental access** with diagnostic party fallback; it must be reported separately from ordinary gameplay progression.

## Entry and acceptance checks

| Stage | Player action | Evidence to record; do **not** assume passing |
| --- | --- | --- |
| 0 — provenance | Extract fresh Windows build, record exact embedded Git SHA and EXE/ZIP hashes; point at owned original installation | Build CI run/checkout, original installation or patch version, startup errors, whether game asset discovery succeeds |
| 1 — cold start | Use an isolated copy of the original installation/runtime state **without `CHARS`**, launch `ravenloft_demo_v1.exe` with no dev flags | Intro movies/sound, title screen, menu, input, character creation, whether CHARS is written successfully |
| 2 — transition | Complete ordinary creation and enter gameplay without developer flags | Whether CODE.1 → CHARS → CODE.2 → actual Level 22 occurs; first interactive frame |
| 3 — play-until-blocked | Attempt original movement, turning, collision, HUD/menu, automap, inventory, equipment, encounters, interaction, combat, pickups, and authored level/story transition **where reachable** | For each action mark **working / incorrect / crashes / not reached** and the first reproducible blocker; do not fake unavailable encounters |
| 4 — return/load | With separate disposable runtime files, exit and relaunch using generated `CHARS` or supported saves | Startup/restore behavior and retained party/world state; differentiate original save support from synthetic test state |
| 5 — diagnostics | Separately run `Diagnose-Level22.cmd` from the same build and owned media; press **F12** where the defect occurs, exit normally | Diagnostic ZIP path and relevant screenshots/short video; note that the diagnostic shortcut **does not count** as normal-play progress |

**Protect original saves:** do not delete or overwrite the real installation's `CHARS`, `SAVE.DAT`, or other writable state. Use a separate disposable installation/runtime copy or back up every mutable file before running. Do not commit or redistribute proprietary assets, CHARS/saves, or personal paths.

## How to run the candidate

1. Obtain the most recent **user-authorized** verified Windows build artifact on the `demo-v1` source SHA. Existing #658 may be used **right now** as an explicitly older baseline for preliminary testing, not as a test of current Git.
2. Unzip into a **new** folder. Run `ravenloft_demo_v1.exe` normally; follow its original-installation folder chooser if prompted. Optionally use `ravenloft_demo_v1.exe "C:\\path\\to\\owned-Ravenloft"` to select the media directly.
3. For normal-play tracing, the Win32 host supports `--dev-trace` **without** `--explore-level22`; this does not itself select diagnostic-party access. Record whether a trace is successfully generated. Do not assume the existing Level22 ZIP wrapper covers a normal-play session.
4. Only for an independent diagnostic run, launch bundled `Diagnose-Level22.cmd`; it uses `--explore-level22 --dev-inspect --dev-interact --dev-trace` and, after exit, packages the session beneath `%TEMP%\\RLOFT-Demo-Diagnostics`. Its world/progression results must be labeled **developer shortcut**, not ordinary gameplay.
5. Stop at the first repeatable blocking issue; submit the evidence before any unverified workaround or forced level transition.

## Offline integrity verification of the native trace

The read-only [diagnostic ZIP/folder validator](tools/audit_mplay01_trace.py)
cross-checks source revision, manifest format and recorder limits, sequential
events and finish counts, frame/marker metadata, exact 320x400 indexed bytes,
6-bit palette bytes, both recorded FNV-1a 64-bit hashes, and existence of
draw-order/marker-recent evidence. ZIP members are inspected **without extraction**
and rejected on path traversal, duplicate paths, unsafe symlinks or oversized
declared content. A session lacking `session.finish` is reported as interrupted,
not passed. A wrong revision, corrupted frame or contradictory count fails closed.

```sh
python DemoV1/tools/audit_mplay01_trace.py "path/to/session.zip" --expected-sha FULL_40_HEX_BUILD_SHA
python DemoV1/tools/test_mplay01_trace_audit.py
```

Use the **compiled executable's** embedded SHA/packaged build-source-sha.txt,
not today's branch HEAD, when supplying `--expected-sha`. Reports contain
only aggregate screen/error-stage counts and last native frame location; no
game saves, captured keystrokes or installation paths are emitted. This check
is **not a normal-play acceptance**, even if the diagnostic was taken during a
normal launch with `--dev-trace`; provenance says native preview, not DOS.
M-PLAY-01 still requires a genuine no-shortcut field attempt and the actual
first blocking defect. Authored offline tests do not constitute a newly built
Windows binary.

## First playthrough report (copy for each tested binary)

```text
Milestone: M-PLAY-01
Test type: NORMAL COLD START / NORMAL CHARS PRESENT / LEVEL22 DEV DIAGNOSTIC
Native branch and SHA:
Windows Actions run / artifact / executable SHA-256:
Original owned media/patch version (no copyrighted uploads):
Runtime copy isolated and backed up: yes / no
Launch mode and command (omit private filesystem paths):
Last successfully observed phase:
Farthest ordinary reachable location/story action:
Actions confirmed working:
First blocking failure and exact reproduction:
Expected original behavior / actual native behavior:
Crash, hang, wrong pixels/sound, incorrect rules, or missing gameplay:
Screenshots/video/diagnostic archive:
Can continue without cheating or developer shortcut? yes / no
Next original-code source offsets or resource records to audit:
```

## Milestone exit gate

This **testing milestone** is complete when:

- [ ] At least one build is traceable to a **specific** `demo-v1` checkout; source compiler and test results are reported separately from Windows field behavior.
- [ ] The player makes one real ordinary launch attempt against legally owned media, recording whether cold start, character flow and Level 22 occur.
- [ ] The farthest *ordinary* reachable action, earliest blocking failure, and at least one reproducible trace or screenshot are recorded, including the case where startup itself fails.
- [ ] Failures are filed and prioritized: **P0** cannot boot or immediately crashes; **P1** blocks ordinary progression; **P2** correctness, render/audio/input mismatch without total blockage. A genuine DOS-equivalence mismatch retains an independent acceptance gate.
- [ ] The next source-verified native correction is selected from the observed blocker, rather than assuming the rest is playable.

This milestone **does not require** a full Level 22 playthrough or imply full original-game completion. Windows Full GitHub Actions is **manual, per-run user-authorized**; creating this tracking document/issue does not dispatch it. No live gameplay change, readiness-score increase or original-DOS-parity claim occurs by declaring the milestone.
