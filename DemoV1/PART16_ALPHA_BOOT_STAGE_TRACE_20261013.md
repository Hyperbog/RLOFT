# Part 16 — first-blocker boot-stage diagnostic for Alpha

**Owner branch:** `demo-v1`. **Scope:** opt-in native
`--dev-trace` diagnostics for the opening of the original game.
No gameplay logic or original assets are introduced or changed.

Previously diagnostics were initialized only AFTER
`ExplorationRuntime::load_fresh`, gameplay state setup and
all exploration presenters. A failure in normal CINE00/CINE12,
three logos, animated title, action dispatch, CODE.1 creation,
or CHARS disk handoff left **no diagnostic session** for the
isolated Alpha field-test package.

On `--dev-trace`, the Win32 host now starts a separate
`DevSessionTrace` after creating the window, before intro
video playback. Its SHA-pinned source manifest and event log
are under `%TEMP%\\RLOFT-Demo-Diagnostics`; the new
`latest-boot-session.txt` pointer names that specific run.
Stage markers record resource launch/result, presentation and
character transfer without saving character names or game
files. Normal gameplay without `--dev-trace` has no IO,
simulated input or additional game behavior.

The trace finishes at the verified transition into
`run_exploration`. That runtime retains its separate
frame/palette/draw diagnostic session as before. The
protected-copy PowerShell runner now packs **both** sessions
when present, so an early cinematic or CODE.1 crash still
leaves a reviewable startup ZIP without misrepresenting the
incomplete exploration renderer as playable.

`alpha_boot_stage_trace_contract_tests` validates source
stage ordering, three ordinary/preview/CHARS-existing handoffs,
trace pointer separation and the two packaged artifacts,
and rejects four missing-stage/pointer mutations.

The original DOS executable, native Windows manual
interaction, source parity, player acceptance and all ten
Alpha gates remain separate, open validation requirements.
