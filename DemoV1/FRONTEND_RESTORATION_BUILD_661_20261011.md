# Original front-end restoration — Windows Full #661

**Branch:** `demo-v1`  
**Owner direction:** retire player-facing standalone Level 22 demo; prioritize the original intro, DreamForge/SSI logos/title, cinematics and character creation.  
**CI run:** [#661](https://github.com/Hyperbog/RLOFT/actions/runs/38194351208) — **success**, 2026-10-12 UTC.  
**Exact checked-out source:** `66046988dd66f54105c83bbf02aeb368973033eb`.  
**CTest:** **246/246 passed**, including `original_front_end_showcase_contract_tests` and the historical trace-launch source test.

## Visible user-facing change

The Windows artifact is now **`Ravenloft-Original-Intro-Windows-x64`**, artifact ID `11722379681`.

The ZIP contains:
- `ravenloft_demo_v1.exe` — Windows native executable
- `Play-Original-Intro.cmd` — **intended double-click launch path**
- `Audit-Starting-Equipment.cmd` and `rloft_starting_equipment_probe.exe` — character creation research
- `build-source-sha.txt` and `build-verification-mode.txt`

The ZIP **does not package** `Explore-Level22.cmd`, `Capture-Level22.cmd`,
`Diagnose-Level22.cmd`, `Probe-Level22-Ground.cmd`, or
`Trace-Normal-Launch.cmd`. Original LEVELS/LEV022 game media, its archived
source audits, and the CODE.2 gameplay runtime remain preserved for eventual
faithful complete-game reconstruction; original game levels are not deleted.

**Opt-in `--front-end-only --choose-install` mode** requires choosing an
owned original-game-data installation and forces the existing intro/title
flow even when a CHARS file is present. This override is confined to the
front-end showcase. On successful character-creation completion, the in-memory
CODE.1 transfer image is generated but the host stops **before writing CHARS
or entering Level 22**; it shows a clear completion/incomplete-runtime message.
Old menu options 0 and 1 show an explicit not-yet-implemented notice in the
showcase, rather than appearing unresponsive. Normal unflagged CODE.2
CHARS-present semantics remain unchanged.

## Independently checked artifact identity

- ZIP: **641,134 bytes**; SHA-256
  `42f300273accec02d3966761c0b1795f024eab76e389ccfd35d2950cce98f734`
- ZIP integrity: no broken member (`zipfile.testzip()` returned `None`)
- `ravenloft_demo_v1.exe`: **951,808 bytes**; SHA-256
  `70ceab7a0bed6dd919b04842a742ec90cd258aed1235288020442dd626bca73e`
- Committed `DemoV1/Windows/ravenloft_demo_v1.exe.sha256` agrees.
- `build-source-sha.txt` matches the exact CI checkout;
  `build-verification-mode.txt` is `full`.

## Limits and next work

This is a **Windows build and synthetic/source-contract success**, not a
verified on-screen player field session, a fully functioning intro/title
comparison to original DOS, or complete campaign playability. Do not claim
the old Level 22 prototype is the delivered game. The next faithful game
workstream is complete original startup/title-menu option behavior and
connected CODE.1/CODE.2 gameplay, with source and original DOS evidence.
The owner authorized automated Windows Full CI on relevant `demo-v1` pushes.
