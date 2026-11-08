# Original DOSBox-X VGA parity: one-paused-frame owner capture protocol

**Purpose:** record a screenshot and a VGA savestate from the **same frozen original-game frame**. Needed because the previous original screenshot showed exploration while `1.sav` held Ravenloft's top command menu and different skeleton animation frames. The frame-complete native source renderer is independently verified but DOS indexed-frame and DAC parity are still OPEN.

**Owner actions, Windows DOSBox-X 2026.10.01 SDL2:**
1. In the running **original** DOS Ravenloft game, return to the private `SAVE09(2)` two-skeleton Level 5 camera, heading 198 degrees. Close any Ravenloft top command menu. Do not open REST/PRAY/MEMORIZE/SAVE while capturing. Do not move after positioning.
2. Use the **DOSBox-X host menu** `Main > Pause emulation`. **Not** `Pause with interrupts enabled`. Keep emulation paused for both captures.
3. Use `Capture > Take screenshot` while still paused. Record the filename of the PNG.
4. Use `Capture > Select save slot` to choose an unused slot such as **2**, then `Capture > Save state`. No game keyboard save, no Ravenloft in-game save dialog. Record the new .sav file's path.
5. Upload the **PNG and the .sav from this single pause**. Resume DOSBox-X after both were captured. If either host capture command is disabled when paused, report that fact rather than unpausing for one capture.

Do not use Windows Snipping Tool for this pair: a DOSBox-X screenshot directly from the Capture menu avoids window chrome and external scaling. Do not reuse earlier `1.sav` or mix screenshots and save states from different pauses. Host mouse/menu actions must not change the emulated game scene between captures. Original game files and private save remain on the owner's machine.

**Assistant obligations immediately after receiving the pair:**
- Verify version and save identity; parse full original saved VGA `Vga` component using `Recomp/tools/extract_dosboxx_2026_10_vga_state.py` with its exact byte-offset/version guard.
- Extract CRTC source page, all four indexed VGA memory planes, split/line compare, DAC PEL mask and 768 RGB6 palette bytes. Verify screenshot RGB colors belong to original DAC and quantify saved-selected-page RGB screenshot equality, independently of native.
- If snapshot and screenshot agree sufficiently to support a synchronized comparison, compare original indices and RGB6 DAC against accepted native `.idx/.pal`; report full mismatch count and pixel coordinates. Where same RGB occurs at multiple DAC indices, **never assert unique screenshot-derived palette indices**.
- Diagnose the 8 known original/native DAC entry mismatches 246..253 using source `COLORS` palette initialization and runtime DAC write path; correct original-source initialization only when grounded. Preserve original screenshot and save state as private media.
- If snapshot and screenshot do **not** agree, do not treat resulting native pixel differences as renderer bugs. Locate the game render/CRTC page-flip boundary or refine pause/capture protocol. Do not silently accept a DOS parity gate.

Original verified save-state `1.sav` provided earlier has DOSBox-X 2026.10.01 SDL2, CRTC start `0x8000`, stride offset `0x28`, line compare `0x1FE`, original DAC RGB6 256 entries; the previous screenshot and the saved selected page are *not* synchronized. This prior capture established original VGA extraction but **not** pixel parity.
