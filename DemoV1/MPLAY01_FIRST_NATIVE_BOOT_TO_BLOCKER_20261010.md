# M-PLAY-01 — First native boot-to-blocker playability checkpoint

**Owner:** `demo-v1` native game-engine workstream  
**Type:** diagnostic field-test milestone within the **complete faithful original-game reimplementation**; **not** a partial-game release, DOS parity certification, a separate "Phase I", or permission to modernize.  
**Established:** 2026-10-10. **Status:** latest Windows Full #660 compiled/tested/published; first ordinary user playthrough **not yet accepted**.

## 2026-10-11 field observation: Level 22 is still the prototype

**Player report (after #660):** `Trace-Normal-Launch.cmd` opens the old
Level 22 native demo rather than the original full DOS game. This is a
**valid product-scope and user-expectation blocker**, not a success result
for M-PLAY-01. No new ordinary-play progress was established.

**Code-based explanation:** the CMD calls `ravenloft_demo_v1.exe`
and enables only `--dev-trace`. That executable is the **incomplete native
C++ reconstruction**, not original `R.EXE` or the DOS `CODE.1/CODE.2`
machine code. The host uses `GameSource` to read original media
(`RES0`, `INIT`, `RES3`, `RES4`, `LEVELS.DAT`, etc.); this
**does not imply executable or full-game parity**. Original CODE.2
fresh-party initialization reaches level 22 at (4.5,4.5), facing 270.
A pre-existing writable `CHARS` suppresses the intro/creation route
and goes straight to the **same unfinished Level 22 native renderer**,
which explains the immediately familiar screen.

**Source remedy pending next authorized build:** the diagnostic wrapper
now explicitly identifies itself as an incomplete native reconstruction,
and passes `--choose-install` to require the user to select their
**intended owned original-data installation**, bypassing previously
remembered install paths. It never launches the DOS original. The new
CLI rejects pairing this option with an explicit path, and retains the
non-preview normal startup branch; source-only tests assert those
properties. The original source files, game state, and existing
renderer are unchanged. Windows compilation/execution of this new
source revision remain **unverified**, and the previously published
#660 ZIP cannot contain the fix. The original DOS parity and entire
gameplay progression gates remain **open**.

**Proper next implementation objective:** close the actual original
startup/gameplay integration and complete source-faithful Level 22
interactions, authored events, original renderer and state continuity.
Do not mistake rebranding, selecting assets, a clean CTest run,
or Level 22 resembling the original starting location for completion.
Document the first real blocker in the native engine separately from
this misleading launcher scope.

## Goal

Give the player a reproducibly identified Windows build of the **existing native engine**, run it against their **legally owned, original** Ravenloft installation, follow the **ordinary startup and character/gameplay path** as far as it works, and record the **first blocking divergence** plus all previously working interactions. Do not require completion of the whole game before collecting this evidence. Do not substitute an artificial warp, test fixture, invented event or synthetic asset for actual progression.

This checkpoint is an observation and defect-triage gate. The separate project charter's **Level 22 complete/source-faithful acceptance before independent Level 23 work** remains binding.

## Latest Windows Full candidate — accepted compiler/tests, field pending

- [Full Windows #660](FULL_WINDOWS_BUILD_CHECKPOINT_20261011_RUN660.md) compiled
  checkout `5b0b00e89bfdc783ee20c59ad1c546bfa0873311`, passed
  **245/245 CTests**, published artifact `Ravenloft-Demo-v1-Windows-x64`
  (GitHub artifact ID `11722062072`), and verified EXE SHA-256
  `f28f4165b4c4f47071536c4de4b63f12363e55efba1c10a57e5bf3c1297693de`.
- The ZIP now contains `Trace-Normal-Launch.cmd` for the **actual
  intro/creation route** with passive exploration trace. The player has
  **not** yet accepted startup, character creation, Level 22, level/story
  progression, state reloading, or original DOS-equivalent behavior.
- Use an **isolated/disposable** runtime copy: the launcher does not
  block normal game saves. Developer `Diagnose-Level22.cmd` remains a
  separate, non-progressing test.

## Previous baseline (#658, historical)


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

1. Obtain the verified [#660 Windows build](https://github.com/Hyperbog/RLOFT/actions/runs/38192487862) (compiled checkout `5b0b00e8`). Existing #658 may be used only as an explicitly older baseline, not as a test of current Git.
2. Unzip into a **new** folder. Run `ravenloft_demo_v1.exe` normally; follow its original-installation folder chooser if prompted. Optionally use `ravenloft_demo_v1.exe "C:\\path\\to\\owned-Ravenloft"` to select the media directly.
3. For normal-play tracing, the Win32 host supports `--dev-trace` **without** `--explore-level22`; this does not itself select diagnostic-party access. Record whether a trace is successfully generated. Do not assume the existing Level22 ZIP wrapper covers a normal-play session.
4. Only for an independent diagnostic run, launch bundled `Diagnose-Level22.cmd`; it uses `--explore-level22 --dev-inspect --dev-interact --dev-trace` and, after exit, packages the session beneath `%TEMP%\\RLOFT-Demo-Diagnostics`. Its world/progression results must be labeled **developer shortcut**, not ordinary gameplay.
5. Stop at the first repeatable blocking issue; submit the evidence before any unverified workaround or forced level transition.

## Packaged no-shortcut field launcher

`Trace-Normal-Launch.cmd` in the upcoming **separately authorized**
Windows artifact is an opt-in diagnostic for the **incomplete native
reimplementation**. It runs
`ravenloft_demo_v1.exe --dev-trace --choose-install` and asks which
original-data installation to read. It does **not** execute the original
DOS game. Existing `CHARS` enters the still-incomplete native Level 22
runtime; a disposable copy without `CHARS` can attempt the cold intro. It never
uses `--explore-level22`, a temporary diagnostic party or dev-interact
flags. The launcher does not write original files itself, but **normal
gameplay may write CHARS and saves**, so a disposable runtime copy is
mandatory. Do not use an original installation with unbacked saves.

After exit, a ZIP is packaged from the recording if and only if
exploration was actually reached. A failure before exploration is
reported as **no trace available**, with the game's exit status, not
as a successful progression milestone. Preserve an external screenshot
or video of such an early failure. The ZIP can then be checked using
`audit_mplay01_trace.py` below. Launcher argument forwarding is
intentionally disabled to prevent an accidental developer-shortcut test.
This is source wiring and synthetic contract validation, **not a
new Windows-tested binary or actual player acceptance**.

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
