# Ravenloft — opening-to-Level22 engineering checkpoint

**Date:** October 15–16, 2026  
**Continuous Windows game:** `Ravenloft.exe` on `demo-v1`  
**Compiled source:** `5d8a8e4a7913d93487e6e90b7483178e8976679c`  
**Build:** [Windows CI #936](https://github.com/Hyperbog/RLOFT/actions/runs/38839084872) — SUCCESS; **44/44** focused Windows CTests; verified single enhanced-game player artifact, executable SHA-256 `92895929947ad4e3b1aa5035d3194a00ad1dd8c8f475b242a8b8ee53a45f3934`. All claims here distinguish code/CI verification from actual owner playtesting.

## Source changes in this pass

1. Four original CODE.2 title actions now accept native keyboard navigation: Up/Down wrap among the four original indices, Home/End select endpoints, Enter activates once, and keys 1–4 or numpad 1–4 directly select the corresponding original option. Mouse actions and title artwork remain unchanged. [Test: `title_keyboard_tests`](tests/title_keyboard_tests.cpp).
2. F11 fullscreen now works throughout original movie playback, logo screens, title, CODE.1 character creation and Level22 rather than being restricted to exploration. Preserved the existing window/placement restore code. Additive input only.
3. GDI fallback no longer allocates a device context and bitmap at 60Hz. A size-matched cache is recreated only after a size change and released on window shutdown; the original 320×400 indexed image remains presented by a single BitBlt. This removes a known host-side source of allocation jitter but **is not** yet owner-confirmed elimination of flicker.
4. The active indexed palette is refreshed from the **current** original level ID when Save/Load or a source-backed transition replaces the level. The preserved WGL backend invalidates palette-derived textures and rebinds them without destroying the original session.
5. Source-decoded original RES6/RES10 VOC PCM now tries enumerated Windows waveOut devices when the default WinMM mapper is absent. XMIDI playback similarly tries installed MIDI devices, terminates failed-start fade waits and latches successful device start independently of short track termination. An installed MIDI synthesizer/output remains necessary. The original resource decoders, notes, events and audio timing are unchanged. The frontend-preservation gate pins all four other original frontend files and separately pins both **reviewed** WinMM backend blobs, rejecting 5 deliberate audio regressions.
6. The native save regression now roundtrips not just the active party/Level22/NPC/item/door state but a second visited mutable level with different NPC HP, event scripts, loose inventory and direct-save state, and verifies byte-identical serialization after decode. Actual file Save and title Continue already exist in source; **their disk-backed quit/relaunch success is not owner-accepted**.
7. The game integration guard now rejects 21 deliberately removed or altered source behaviors, including GPU startup, authoritative source records, original raster default, four title actions, Save, keyboard title input, fullscreen, palette switch and GDI cache.

## Independent supplied Level22 direct-file check

Supplied original `LEV022.DAT` is 6,028 bytes. Direct-file parsing gives:
- World grid: **30 columns × 15 rows**; **110 placed family-B** 22-byte records; **one NPC** and no initial loose items.
- Placed-scene source type distribution: **62 × 52**, **63 × 42**, **64 × 16**. There are **313** world-cell words carrying bit `0x0040`, the source scene-occupancy marker.
- Original fresh party pose: **(4.5,4.5), heading 270°**. Projecting the 110 object positions through an unobstructed horizontal approximation using the original `C6CC=175` focal gives **84 candidates inside the 256-pixel first-person horizontal viewport** (43 of type 62, 30 of type 63, 11 of type 64). This deliberately excludes occlusion, drawable-image resolution, vertical and world depth tests. It **does not mean 84 sprites should be on screen**. It proves the original authored tree/vegetation/scene families are not absent from the file.

Source files are present and can be decoded; if the player sees an empty or malformed Level22 field, the remaining problem is in live view preparation, visibility, SINTAB/RES3 rasterization or OpenGL composition. The original software raster (indexed presentation) remains the default. The experimental GPU polygon world is **F2 opt-in**; it must not be declared game-faithful until real screenshots and playtesting agree.

## User-visible controls after this pass

- On title: **Up/Down**, **Home/End**, **Enter**, **1–4** or **numpad 1–4**. Each dispatches the unchanged original menu choice.
- Entire application: **F11 fullscreen/windowed**.
- During gameplay only: arrows / WASD / QE; Shift-to-run; Tab automap; M or Ctrl+right-click mouse-look; **F2 original-world/experimental-3D toggle**; **F3 filtering mode**.
- Continue and in-game Save/Load use the independent native checkpoint `%LOCALAPPDATA%\\Hyperbog\\Ravenloft\\SavedGame-1.rls`; original DOS `SAVE.DAT` is never overwritten.
- Direct `Ravenloft.exe` and `Play-Ravenloft.cmd` both target the normal original opening; the CMD helper safely stages original files from the player's owned Game folder. Neither artifact bundles the original DOS media.

## Still blocking Level22 acceptance

- The owner's faulty/missing first-person scenery and perspective **have not been visually demonstrated fixed**. The source projection and map records are available; inspect actual Windows captures next, especially original RES3 scenery, original sky/ground composition, culling, SINTAB column masks and per-frame sprite extents.
- Title/intro ordering must be compared to original gameplay footage: current code executes CINE00/CINE12 before the three standalone logo panels, while the owner asks for logos before the introduction. Reordering without identifying the actual ANM content risks making fidelity worse.
- The actual player-to-Assassin combat attack dispatcher is not yet complete despite verified low-level combat hit/damage helpers. An encounter is not complete because a mouse can select a world object.
- The original 22-frame mist timeline (source CODE.2 `21F70..223DE`) is modeled but **not yet dispatched with the original 1CD48/2E510 effects in the live player**; the source path still reports transition pending. No premature fabricated Level23 level warp is permitted.
- Authentic audio and frame pacing still require field listening; fallback device enumeration cannot substitute for a Windows synthesizer that is not installed. Save→quit→Continue and active Level22 world-state restoration need actual Windows playback.
- Broader campaign expansion remains blocked until this complete sequence is accepted.

The title input, native audio/output fallback, palette correction and software-window performance change are verified as **compiled regression-tested code**. No post-Level22 gameplay acceptance was claimed.
