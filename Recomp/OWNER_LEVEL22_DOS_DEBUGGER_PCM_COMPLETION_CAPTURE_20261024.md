# Part 29: owner-only retail DOS Level 22 framebuffer / sound completion evidence

**Date:** 2026-10-24. **No parity claim yet.** All original executables, DOSBox-X screenshots/states, debugger memory, captured WAV, original VOC, and source-indexed buffers are **private**. They must never be committed, uploaded as CI artifacts or substituted with reconstructed source bytes. The original game assets in `DnD-Classics/MASTER` do not contain the synchronous emulator state requested here.

## Original DOS frame — required capture pair

1. Run the **unmodified retail DOS** game using **DOSBox-X 2026.10.01 (SDL2)**, the only version the existing VGA save-state reader accepts. Replay the selected Level 22 save and independently log **every consumed 16-bit `30B40` key word**, including zero/idle polls, from the start of the test. Count actual **game `1FC14/1A510` source cycles**, not emulator presentation frames.
2. Stop at the original `30B40` keyboard boundary immediately after a fully completed `1FC14` gameplay cycle: its preceding `1A510` page swap, `1F494` HUD and `28168/1F120` post-scene processing must have returned. The DOSBox-X debugger offers `BPM <seg>:<off>` for protected-mode breakpoints, and `MEMDUMPBIN <seg>:<off> <bytecount> <filename>` to extract guest bytes. First establish the original **runtime loaded CODE.2 object base and DS selector**; do not interpret reconstructed logical offsets as DOS runtime linear addresses. Collect at least **`0x255D8` bytes beginning at protected-mode DS offset 0**, using a debugger memory dump from this exact pause.
3. Keep the original game CPU and display frozen while taking the DOSBox-X **Capture → Take screenshot** and **Capture → Save state**. Use the emulator capture menu, not a Windows desktop crop. The established [same-pause protocol](DOSBOXX_ORIGINAL_SAME_PAUSE_CAPTURE_PROTOCOL.md) documents this. **Some DOSBox-X builds hang if you save while already paused**: if that happens, do not silently unpause for one half of the pair. A running-state save and later screenshot are *not* accepted as synchronized. Prior owner snapshot `1.sav` and screenshot were from different scene instants and are not parity evidence.
4. Keep a binary `keys.le16` containing those independently observed keyboard words, little-endian `u16` per poll. The trace must cover exactly the original source input sequence whose tick is being compared; do not reconstruct it from native logs. Record the actual original source-tick count and branch `1FC14_1A510`. The 320×400 displayed page is retrieved from the saved VGA CRTC, **not** inferred from a screenshot or whichever source page was drawn last.

DOSBox-X exposes screenshot and savestate actions and a protected-mode debugger. See [DOSBox-X official controls](https://dosbox-x.com/wiki/Home.html) and [debugger reference](https://github.com/joncampbell123/dosbox-x/blob/master/README.debugger). An authenticated retail input debugger/screenshot pair has **not** been supplied in this session.

## Extract, bind to original debugger RAM, then compare

From a local checkout with the exact matched project source and Pillow installed:

```powershell
python Recomp/tools/check_dosboxx_2026_10_frame_pair.py `
  --state C:\Private\level22.sav --screenshot C:\Private\level22.png `
  --report C:\Private\dosbox-frame.json `
  --export-dos-index C:\Private\dos.idx `
  --export-dos-palette C:\Private\dos.pal

python Recomp/tools/create_original_dos_debugger_observation.py `
  --retail-code2 C:\Games\RLOFT\CODE.2 `
  --dgroup-dump C:\Private\original-ds0.bin `
  --dos-index C:\Private\dos.idx --dos-palette C:\Private\dos.pal `
  --dosbox-state-report C:\Private\dosbox-frame.json `
  --keyboard-trace C:\Private\keys.le16 `
  --source-tick 5 --frame-boundary 1FC14_1A510 `
  --out C:\Private\original-observation.json

python Recomp/tools/verify_synchronized_dos_frame_pair.py `
  --dos-index C:\Private\dos.idx --dos-palette C:\Private\dos.pal `
  --native-index C:\Private\level22_source_tick_5.idx `
  --native-palette C:\Private\level22_source_tick_5.pal `
  --dos-observation C:\Private\original-observation.json `
  --native-observation C:\Private\level22_source_tick_5.json `
  --dosbox-state-report C:\Private\dosbox-frame.json `
  --report C:\Private\indexed-parity-report.json
```

The second tool reads level at `C6C8`, original camera at `255D0/255D4`, heading at `C658` **from dumped retail DOS data-segment bytes** and authenticates the exact uploaded original `CODE.2` SHA-256. It refuses missing screenshot-state agreement. It does **not** authenticate whether a user-supplied dump was really obtained at the claimed CPU breakpoint: preserve the original debugger breakpoint log/CPU state for an independent review. Never overwrite original inputs with the recompiled camera or CRTC. The third tool rejects camera, frame boundary, source tick, keyboard-trace, CRTC, DAC and index mismatches **before** a parity assertion.

## Real sound output and completion: distinct claims

`Recomp/src/host_original_voc_waveout.cpp` now exposes an **explicit opt-in** Windows device test for an owner-extracted Creative Voice `.voc`. For block type 1 (8-bit unsigned mono PCM, no compression) with optional type-2 continuation, it decodes the signed-off format and uses native **WinMM waveOutOpen / waveOutWrite**; reports completion **only when WinMM marks that submitted buffer `WHDR_DONE`**. Wrong VOC blocks/codecs, missing hardware, unsuccessful device open, failed write, or missing completion are errors, not synthetic acknowledgements.

```powershell
# Safe parser/format inspection only, no sound:
build\recomp\Debug\rloft_host_original_voc_waveout.exe --voc C:\Private\sample.voc
# Explicit output on the owner's actual configured Windows audio device:
build\recomp\Debug\rloft_host_original_voc_waveout.exe --voc C:\Private\sample.voc --play
```

**Important distinction:** WinMM sample-buffer completion is a *real modern OS driver* completion **for this standalone test**, not original Miles AIL/32 completion, Sound Blaster IRQ/DMA or CODE.2 `367F9/367EF/3680D` runtime ABI. The retained original gameplay session still fails closed with pending uninitialized AIL, without tampering with `EE40` or fabricating a finished sound. Truly authentic AIL output/completion requires original DOS/AIL initialized and queried in its own hardware/emulator session, or an independently validated native adapter with vendor completion semantics. No original AIL completion observation is available yet.

## Camera displacement frontier

Real stdin `S/D/W` has already executed original source `30B40` three times; it changed **`C000/C004` movement accumulators** but did not alter `255D0/255D4/C658`. The guest now logs **separate** `camera_displaced` and `heading_rotated` flags and exposes `RLOFT_RECOMP_REQUIRE_CONSOLE_REAL_POSE=1`, which **fails closed** unless source-owned pose actually changes.

The Windows original-media workflow additionally runs an opt-in 60-poll **diagnostic** with host-delivered sustained movement followed by idle polls. This is not a DOS key-held IRQ9 waveform; it measures whether authentic original source functions eventually update camera values without host writes. An unsupported source branch remains a diagnostic failure, **not** a camera-displacement acceptance.


## 2026-10-24 extended original-media observation

The diagnostic [Windows run #40078918700](https://github.com/Hyperbog/RLOFT/actions/runs/40078918700) passed the other required original-media tests, including source audio, five private frame captures, and three short live host-keyboard pulses. Its **separately non-gating, 60-poll movement search** returned only **42 original source input events** within the 95-second diagnostic limit and was terminated by its safety timeout. Of the 42 completed events, **zero** changed camera X/Y and **zero** changed heading, even though the guest's `C000` speed saturated at unsigned `65376` (signed `-160`). These 42 observations are genuine original-code guest state, not fabricated movement. The remaining 20 intended events were **not executed** and cannot be counted. The positive-camera acceptance remains OPEN. This result implicates an unconnected or untriggered original world-position integration path, or missing real-time input cadence; it does not prove which one.

The strict native terminal option `RLOFT_RECOMP_REQUIRE_CONSOLE_REAL_POSE=1` now refuses success without original source changing camera X/Y or heading. Do **not** set this flag merely to obtain a convenient passing synthetic test.

### Never promote from these probes

Source VGA/private indexed capture ≠ synchronized original DOS framebuffer. Original `28168/27F24` synthetic callback tests ≠ initialized retail AIL. Real WinMM `WHDR_DONE` ≠ original AIL completion. Source movement accumulator ≠ camera displacement. Whole-game acceptance gates remain unchanged.
