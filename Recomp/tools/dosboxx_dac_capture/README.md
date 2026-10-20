# Part 24: DOSBox-X 2026.10.01 retail palette-port capture

## IMPORTANT: save-state freeze workaround (preferred on affected DOSBox-X)

Some DOSBox-X sessions become unresponsive when **Pause emulation** is invoked
*before* opening **Save state**. The original tagged
`src/misc/savestates.cpp` contains a remark dialog on the save path (when
`saveremark=true`), so avoid mixing a halted emulation/menu with that dialog.
Do not represent this observed hang as proven to originate at that call site.

**Do not rebuild the capture emulator to try this:**

1. Run the original retail game using the instrumented DOSBox-X recorder.
   Stay in a stable first-person scene. Do **not** pause.
2. Before saving, in **Capture → Save/load options**, enable
   **No remark when saving state** if possible. Alternatively use the
   DOSBox-X config setting `[dosbox] saveremark=false` on the next launch.
3. Press **F11 + S** on Windows **while the game is running**.
   Wait until the save finishes. No prior Pause emulation or modal
   Save-state interaction is needed.
4. If you can safely pause *after* the save, do so, but **do not touch
   Save state again while paused**. Otherwise leave the game running.
   Copy the newly generated DOSBox-X `.sav` and the instrumented
   `active-dac-*.jsonl` (or `active-dac.jsonl`) separately to a
   private folder. Preserve the originals.
5. Use the independent *running-save analyzer* below; **do not use**
   the strict `--owner-confirms-paused-copy` verifier. A post-save log
   copy may contain palette writes that occurred **after** the save.

```powershell
python "C:\RLOFT-source\Recomp\tools\dosboxx_dac_capture\analyze_running_dac_capture.py" --raw-jsonl "C:\RLOFT-DAC-private\copied-events.jsonl" --state "C:\RLOFT-DAC-private\copied-original.sav" --report "C:\RLOFT-DAC-private\running-save-analysis.json"
```

The original DOSBox-X save implementation uses the `save` subdirectory
of the emulator working directory by default, naming save slots
`1.sav`, `2.sav`, etc.; custom save directories and `savefile`
configuration may override this. Find the **newest .sav** created at
the moment you pressed F11+S, rather than guessing a path or uploading
the Ravenloft in-game SAVE.DAT.

The analyzer authenticates hook format, refuses truncated/overflowed
logs, preserves raw guest CS/EIP hints, reconstructs RGB6 writes, and
reports the write history for palette entries 246..253. If an optional
same-run SAV is supplied it compares the final observed DAC against
the snapshot, but reports `UNSYNCHRONIZED` even for exact equality.
No same-instant I/O save marker exists in the v1 capture hook; original
CODE.2 caller and full pixel parity remain **NOT VERIFIED**.

If F11+S also hangs, **stop trying save states**; the running DAC
JSONL from a genuine gameplay session still gives independent port-write
observations. Supply that file alone to analyze original palette writers.
Any matching SAV can be collected later.

---

**Owner branch:** `Recomp`. **Purpose:** independent original-game VGA DAC I/O
provenance. **Status:** patch/hook/trace converter tested on a pinned upstream
source file and synthetic captures; **actual DOSBox-X Windows executable and
retail Ravenloft run remain untested**.

## Version and hook boundary

- DOSBox-X Git tag: `dosbox-x-v2026.10.01`
- Upstream commit: `b6abbd5980a885f5f310a4088c59a8688d1b116c`
- Upstream `src/hardware/vga_dac.cpp` blob: `509d42fbead67a56594e49cec55549bc6fcc6d7c`
- Original CODE.2: SHA256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.

The pinned patch adds **two** hook calls to the actual upstream
`write_p3c8` and `write_p3c9` VGA DAC write handlers. The hook captures
the post-lockout accepted palette index, and post-DAC6-mask accepted color
byte. `REP OUTSB` passes through the normal VGA byte handlers, so its
individual transferred bytes are captured in original sequence.

Each line is flushed, and the local recorder is entirely disabled if
`RLOFT_DAC_CAPTURE_JSONL` is unset. A hard 200,000-event cap emits an
overflow marker that the converter rejects; **never** silently validate
truncated history. The instrumented version is experimental; default
DOSBox-X has no such capture option.

CPU `SegValue(cs)` / `reg_eip` are captured as *raw emulator-register
hints*, not as confidently normalized original instruction addresses.
Depending on DOSBox-X CPU core, these registers may point past an I/O
instruction at handler dispatch time.

The fixed hook covers the original VGA handlers in this tagged source.
It does not claim coverage of every substitute SVGA handler, DOSBox-X
backend, or operating system. A saved VGA palette does **not** contain
historical port-write events.

## Windows build (one-time source patch)

Use **Developer PowerShell for Visual Studio 2022** with Python 3 and
Git installed, Windows SDK, and the upstream DOSBox-X build prerequisites
(including DirectX SDK where needed). The project supplies a source-checking
build helper; this operation builds a new emulator, **not** a game release.

```powershell
git clone --branch Recomp https://github.com/Hyperbog/RLOFT.git C:\RLOFT-source
& "C:\RLOFT-source\Recomp\tools\dosboxx_dac_capture\build_windows_capture.ps1" -RloftRoot "C:\RLOFT-source" -DosboxSourceRoot "C:\dosbox-x-RLOFT"
```

The helper refuses any different original upstream `vga_dac.cpp` blob.
The hook cannot be installed by editing an existing retail DOSBox-X EXE.
Output is expected at
`C:\dosbox-x-RLOFT\bin\x64\Release SDL2\dosbox-x.exe`; this exact
Windows build has **not** yet been exercised in CI or on the owner's PC.
The instrumented build uses DOSBox-X GPL-2.0-or-later source and is intended
for local research. Do not confuse its output with an accepted native
Ravenloft game.

## Same-pause original runtime capture

1. Create a private folder `C:\RLOFT-DAC-private`. Keep it off Git.
2. In a *fresh* PowerShell process, before launching the instrumented
   DOSBox-X, set:

   ```powershell
   $env:RLOFT_DAC_CAPTURE_JSONL = "C:\RLOFT-DAC-private\active-capture.jsonl"
   & "C:\dosbox-x-RLOFT\bin\x64\Release SDL2\dosbox-x.exe"
   ```

3. Run the original unmodified retail Ravenloft from the owner's game
   install. Do not start tracing an unrelated program and claim its writes
   belong to CODE.2. From startup/load, reach the intended saved Level-5
   camera; preferably stop early before 200,000 port events.
4. Use **Main > Pause emulation** (not pause with interrupts enabled).
   While *still paused*, use DOSBox-X **Capture > Save state** to a new
   slot. Do **not** open the game's own save/menu UI.
5. While *still paused*, copy `active-capture.jsonl` to
   `frozen-capture.jsonl`, and copy the newly created `.sav` from the
   host capture directory to the same private folder. Do not resume
   emulation between copies. If host capture commands cannot execute
   while paused, record that limitation rather than claiming parity.
6. Stop this DOSBox-X instance while the original guest is still
   paused. A changed/emulator-post-resume event stream is not the same
   paired capture. Preserve original raw JSONL and SAV independently
   for auditing.

The saved state must identify `DOSBox-X 2026.10.01 (SDL2)` and
`Program_Name=CODE`. The original color byte values remain private;
the recorder itself creates no synthetic/expected palette events.

## Validate against saved original DAC

```powershell
python "C:\RLOFT-source\Recomp\tools\dosboxx_dac_capture\convert_capture.py" --raw-jsonl "C:\RLOFT-DAC-private\frozen-capture.jsonl" --state "C:\RLOFT-DAC-private\2.sav" --code2 "C:\Games\RLOFT\CODE.2" --capture-id "original-level5-capture-1" --owner-confirms-paused-copy --output "C:\RLOFT-DAC-private\independent-DOS-DAC.json"
```

If the original game wrote all 768 DAC channels *during the recorded
window*, and the original hardware SAV palette agrees byte-for-byte,
the output is `DAC_RGB6_EQUAL_DECLARED_CAPTURE_UNATTESTED`.
The converter also writes a `.receipt.json`; both remain private.
Missing channels, 8-bit mode, wrong SVGA dispatch, overflow, an incomplete
JSONL write, or differing frame-phase DAC cause explicit failure.

The 246–253 bank-3 patch in the native renderer remains explicitly
**provisional**. Even an exact DAC replay does **not** alone prove the
individual original CODE.2 caller that installed that slice, nor original
versus native pixel parity. Original CPU code ancestry requires a separate,
independently verified EIP/CS mapping to the loaded CODE.2 LE object.

## Regression and CI

- [Hook patch check + synthetic C++ replay](https://github.com/Hyperbog/RLOFT/actions/workflows/recomp-dosboxx-dac-hook.yml)
- `test_capture.py`: independent C++11 compilation, opt-in/off,
  sequence checks, parser negatives, synthetic palette replay.
- `apply_dosboxx_dac_capture.py`: SHA-pinned DOSBox-X source,
  one-time-only two-site insertion, reject modified source.
- `convert_capture.py`: strict versioned raw capture parser and
  same-pause SAV/palette matching, no original bytes in public commits.

Keep all user-supplied original game binaries, VGA snapshots, RGB6
palettes, DOSBox-X raw event streams and reconstructed indexed frames
outside published CI artifacts and public git commits.

## Automated private capture handoff (Windows PowerShell)

The full Windows SDL2 build is attempted by
[the source-pinned GitHub Actions workflow](https://github.com/Hyperbog/RLOFT/actions/workflows/recomp-dosboxx-windows-build.yml).
A workflow artifact is only available **if that actual full-emulator job succeeds**.
The Windows host must still run the original retail game locally; the CI
build does not fabricate original DOS events or original saved-state files.

From a local `Recomp` checkout, launch the instrumented executable from the
extracted build bundle into a previously empty private directory:

```powershell
& "C:\RLOFT-source\Recomp\tools\dosboxx_dac_capture\start_original_capture.ps1" -DosboxExe "C:\RLOFT-DAC-build\dosbox-x.exe" -PrivateFolder "C:\RLOFT-DAC-private"
```

Inside the instrumented DOSBox-X, launch the original game, reach the matching
saved Level-5 camera, and **pause emulation without running interrupts**. Use
DOSBox-X's emulator *Save state* UI to create a fresh `.sav`. Do **not**
unpause between saving and the following command (run it in PowerShell in
parallel, outside DOSBox-X):

```powershell
& "C:\RLOFT-source\Recomp\tools\dosboxx_dac_capture\freeze_original_capture.ps1" -RloftRoot "C:\RLOFT-source" -PrivateFolder "C:\RLOFT-DAC-private" -SavedState "C:\Path\To\New\2.sav" -OriginalCode2 "C:\Games\RLOFT\CODE.2" -CaptureId "L5-camera1"
```

This copies frozen raw DAC JSONL and original VGA SAV into the private folder,
checks that the SAV did not change during copying, and calls the strict
`convert_capture.py` validator. It refuses to overwrite existing evidence.
Your confirmation that the emulator remained paused is **not** external proof
of synchronization: the receipt retains `UNATTESTED` status. If the validator
fails, preserve both original files locally for diagnosis; never manufacture
port events from a saved DAC to make them pass.


## Easy game startup: one-click original Ravenloft launcher

**DOSBox-X by itself does not automatically launch Ravenloft.** The captured
port log will otherwise contain emulator BIOS palette writes, not the game's
runtime palette history. For example, a startup-only log with all event
`guest_cs_hint=61440` (`F000h`) is consistent with BIOS startup.

The Windows capture bundle now includes two files alongside
`dosbox-x.exe`:

- `Play-Ravenloft-With-DAC.cmd`: double-click this; it runs the PowerShell
  launcher with the correct Windows UI mode.
- `Launch-Ravenloft-DAC.ps1`: folder picker, source-game validation,
  private log path, and DOSBox-X command-line auto-start of `RLOFT.BAT`.

Double-click `Play-Ravenloft-With-DAC.cmd`, then select **the folder that
actually contains `R.EXE`, `RLOFT.BAT`, `CODE.1`, `CODE.2` and `COLORS`**.
The corrected launcher also verifies and mounts the original GOG disc image
`game.gog` located in that game's **parent installation directory** as DOS drive
`D:` with `imgmount ... -t iso -fs iso`. For the user's example installation,
this is `D:\GOG Games\Ravenloft - Strahd's Possession\game.gog`.
For the immutable historical repository layout that is the final nested
`MASTER\Ravenloft - Strahd's Possession\RLOFT` directory, not the outer
`MASTER` directory. Use a writable owner-owned installed copy of the game.

The launcher mounts the selected folder as DOS drive `C:`, mounts the GOG
CD image as DOS drive `D:`, switches to `C:`, then runs `R.EXE`
**directly**. The shipped `RLOFT.BAT` ends with `EXIT`; directly running
`R.EXE` preserves startup error messages. It creates
a fresh timestamped trace in Windows Documents under
`RLOFT-DAC-private\active-dac-YYYYMMDDTHHMMSSfffZ.jsonl`.
The file is not made public or committed. The launcher does not modify or
synthesize the original game binaries or port values.

Manual equivalent, from an existing DOSBox-X `Z:\>` shell (substitute your
actual owner game installation path):

```dos
mount c "D:\GOG Games\Ravenloft - Strahd's Possession\RLOFT"
imgmount d "D:\GOG Games\Ravenloft - Strahd's Possession\game.gog" -t iso -fs iso
c:
dir d:\rloft
r.exe
```

If the game does not launch, confirm that `dir` lists `R.EXE` and
`RLOFT.BAT`. If it does and `rloft` fails, preserve the exact error
screen instead of assuming DOSBox-X is recording original gameplay.
The scripted launcher is source-built and has not yet been GUI-tested on
an owner's Windows machine; its DOSBox-X `-c` commands use the upstream
supported command-line option.
