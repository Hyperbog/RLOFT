# Part 29 — New Windows RLOFT source-boot and native visual preview

Updated 2026-10-20.

## New original-supervisor correction

The authenticated original 12,368-byte `R.EXE` (SHA-256
`a6d5874dc91fb068a3ed0a07290547c310ee7f4f50be4d8176c4788f251631b0`)
is a **16-bit DOS supervisor**, not the game's direct.CODE.1-to-CODE.2
CPU jump. The original supervisor initializes `WORK` and `AUTOMAPS`,
runs **CODE.2 first**, and interprets process-level exit codes.

The exact verified transition protocol has been implemented as a
deterministic host-side `original_r_exe_supervisor.hpp` state machine,
with standalone acceptance regression `tests/original_r_exe_supervisor_test.cpp`:

| Child process result | Supervisor outcome |
|---|---|
| Start | Spawn/wait CODE.2 |
| CODE.2 status 10 | Spawn/wait CODE.1 |
| CODE.1 status 10 | Spawn/wait CODE.2 again |
| CODE.2 status 12 | Delete CHARS; restart at CODE.2 |
| Other terminal child status | Delete CHARS; stop |

The native staged-source checkpoint now **verifies R.EXE SHA** and calls
CODE.2 original INIT/INFO/11 RES and palette routines first, in one
CODE.2 source session. It does **not** pretend to have received CODE.2
exit status 10: without that exit, the R.EXE supervisor STOPS at the
unimplemented child-process return boundary. Independently it can run
the CODE.1 original startup/constructors as a bounded diagnostic; that
test is not falsely presented as an original R.EXE CODE.1 child.

**Neither dynamic DOS/4GW child spawning nor CHARS process transfer,
gameplay event-loop input, native real-time renderer and live gameplay
is implemented by this checkpoint.**

## Fresh user-visible Windows preview

The Windows engineering milestone ZIP now includes:

- `Play-RLOFT-Recomp-Preview.cmd` — user double-clicks this
- `Launch-RLOFT-Recomp-Preview.ps1` — uses the user's confirmed original
  GOG installation directory, detects saves and writes private diagnostics
- `rloft_gog_cd_extract.exe` — reads `game.gog` **without modifying
  it**, locally extracts **only RES9 and RES10** from standard 2048 ISO
  or raw 2352-byte sector layouts, and deletes scratch copies after the
  staged startup completes
- `rloft_code1_code2_staged_boot.exe` — R.EXE-authenticated
  CODE.2-first source startup and independent CODE.1 diagnostic
- `rloft_original_supervisor_test.exe` — original source status protocol
- `rloft_original_code2_world_frame.exe` — reconstructed original
  320x400 indexed saved-world BMP generator

The optional image preview automatically searches for original
**Ravenloft SAVE.DAT + LEV005.DAT**, preferring the owner's GOG
`cloud_saves/RLOFT/SAVE09` if present. It lets the user choose a folder
if no original Level-5 pair is found. The visual preview uses the
authentic read-only installed `RES0`, `RES1`, `RES3`,
`RES4`, `COLORS`, `INIT`, `SINTAB`, `NPCS.DAT`,
`OBJECTS.DAT` and `CODE.2` assets to render a native
saved-world snapshot. It opens the resulting `.bmp` using the
owner's Windows default image viewer.

The image is a **saved-state native renderer output**, not a booted
gameplay screen and not proven pixel-identical to DOSBox-X. It is
not an animated/interactive demo.

All output and diagnostic logs remain private in
`Documents/RLOFT-Recomp-Preview/<unique-UTC-run>/`.
No game assets, audio, fonts or save states are included in the binary
artifact or committed. Original CD extraction is temporary and local.
An ISO image of unsupported layout causes a clearly labeled diagnostic
and independent source tests can still run.

## Acceptance gates

- C++17 original-supervisor status10/status12/first-child tests.
- Synthetic ISO9660 fixture: 2048-byte sectors, raw Mode1 2352-byte
  sectors, exact extracted test bytes, overwrite refusal, corrupt image
  rejection. No proprietary disc image is used by unit tests.
- Windows PowerShell AST syntax check for the one-click launcher.
- Original immutable DnD-Classics R.EXE SHA and supervisor protocol,
  CODE.1 + CODE.2 source startup, eleven real original RES archives,
  original selected bank3 palette read and 769 DAC I/O adapter writes.
- Windows build packages all original-engineering EXEs and one-click
  CMD/PS1 launchers in fresh GitHub Actions artifact.
- The source-level renderer and palette bank3 tests remain separate
  from active native game loop parity.

**Outstanding:** actual supervisor DOS child process handoff and
preserved CHARS 0x920-byte module serialization, original CODE.2
runtime loop/input and no-save live world rendering. The native visual
preview is for inspecting existing renderer progress, not confirming
that the original application boots interactively.
