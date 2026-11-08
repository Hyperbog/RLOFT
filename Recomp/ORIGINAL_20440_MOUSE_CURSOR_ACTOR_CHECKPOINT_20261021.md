# Part 25 — Original 20440 cursor rendering and Level22 actor record checkpoint

**Branch:** `Recomp`. **Validated code/CI commit:** [`23b5ea971550bff3f1cc6700da1a1a7b8923a96e`](https://github.com/Hyperbog/RLOFT/commit/23b5ea971550bff3f1cc6700da1a1a7b8923a96e).
**Mandatory real-media Windows validation:** [run #39711347125](https://github.com/Hyperbog/RLOFT/actions/runs/39711347125) — **PASS: 85/85 CTests, original-media source assertions, and 2 retained keyboard-poll iterations**.
This is a source-engineering milestone, **not** a playable-game or DOS screenshot-parity assertion.

## Verified original source continuation, same guest and stack

After original `20440` palette output/presentation and retained `2ECB4→2FD9C→2F184` RES0:59 image source:

- `1A0B0` **original compiled source executes** the mouse-present branch with actual `2E340=-1` low AX mouse flag, `33D38` Y range 0..399 and `33D18` X range 0..638. `33C90` centers the mouse through INT33 fn4, and `33CB0` samples INT33 fn3. Each hardware service suspends the retained guest thread and obtains an explicitly selected **virtual host reply**; no claim is made about actual physical mouse movement. Source `1A064 → 33A7C` registers the INT33 fn0C callback via a separate host acknowledgement. Original guest cursor coordinates are first=200, second=160, in the original non-obvious `33CB0` layout.
- Source `2F29C→2ECB4→2FD9C` independently loads original **RES0:62**, `EH` type1, LZSS method3, 11 frames × 32 rows × 2 width units; `30CD4` copies **0x1614 original image bytes** to `0x880D`, independently checked byte-for-byte against the allocated decoded source image.
- `1A288` **original compiled source** executes `2E590` page push → `292BC→2A114` **four-plane VGA mouse background capture** → `2AE30` hit check → `29804→29414→2A380` **visible cursor sprite draw into retained `OriginalVgaHost` planar aperture** → `2E5D4` page pop. Source trace confirms exactly one sprite draw, frame 0, (x=200,y=160), and balanced three total source page pushes/pops. `2A380` uses separate native VGA aperture read/write callbacks, never accidentally writing the graphic only to regular guest RAM.
- Original `181B4` Level22 archive source still loads original 30×15, 450-cell map and additionally executes `19CCC` guest level-state stack push, `19CF4` stack pop, and `0CCF4` terminal-party condition check. Source event and actor vectors remain guest-native, not host-fabricated.
- Independent **original archive actor oracle** consumes original packed Level22 object column lists and 11-byte actor records, checking all five source field locations for every actor in `0x14D80`, `0x22238`, and `0x17618`. It confirms **moving=1, primary=110, alternate=0, total=111, indexed column objects=10**. No synthetic actor placement.
- The original `1FC14→30B40` loop again completes **two original no-key replies** on the retained source call stack and same guest address.

## Required acceptance markers

```text
ORIGINAL_CODE2_20440_MOUSE_CURSOR_SOURCE_PASS source=1A0B0_1A288_2F29C_2FD9C_292BC_2A114_29804_2A380 archive=RES0_62 frames=11 image_copy=1614_SOURCE_BYTES_MATCH mouse_state=INT33_EXPLICIT_VIRTUAL_HOST vga=ORIGINAL_PLANES original_guest=PRESERVED
ORIGINAL_CODE2_181B4_LEVEL22_ACTOR_RECORDS_PARITY_PASS moving=1 primary=110 alternate=0 total=111 indexed_column_objects=10 original_source=181B4 actor_records=BYTE_MAPPED_FROM_AUTHENTIC_LEVELS_DAT actor_AI=NOT_CONNECTED
ORIGINAL_CODE2_1FC14_TWO_SOURCE_LOOP_ITERATIONS_PASS original_keyboard_poll=30B40 no_key_host_replies=2 next_service=11 guest_memory=PRESERVED game_rendering=NOT_CONNECTED
ORIGINAL_CODE2_20440_CURSOR_SOURCE_ACCEPTED res=62 original_1A0B0_1A288=EXECUTED draw=ORIGINAL_VGA
ORIGINAL_CODE2_181B4_ACTOR_SOURCE_ACCEPTED authentic_actor_records=111 actor_AI=NOT_CONNECTED
```

These markers are **mandatory Windows CI assertions** and must not appear only in an optional best-effort test.

## Explicit remaining original-source dependencies — do not claim complete

1. `181B4→1F8E0→1F8FC`: authentic Level22 switch requests a **changed-scene repaint**; the full original 1F8FC presentation pipeline (resource pages, panel captures, character HUD, palette and composer callbacks) is **not connected to the retained runtime**. The source `call_1f8e0` remains null, clearly isolated; a first attempt deliberately stopped at `ORIGINAL_181B4_1F8FC_SCENE_COMPOSITOR_NOT_CONNECTED`, never treat this as passed.
2. Actor lifecycles and events: original `181B4→171FC` old-level cleanup and `17A68` post-load actor transformations/quest state need real `Application31Boundary` callbacks. AI/motion `20850`, collision, timers, sound, interaction/combat/spells and transitions across all levels require continuous source execution. Accurate actor **records** alone are not AI.
3. `1A288` source sprite draw works for the authenticated initial original cursor. Clipped cursor drawing (`2A6B0`), cursor movement/restore across frames, source pointer capture beyond the initial host reply and actual native physical mouse event delivery are not yet certified. Host model uses a deterministic bounded VGA adapter, not retail analog VGA.
4. Frame rendering/input: original `1FC14` no-key source polls are real, but live original world raster/compositor and meaningful keyboard/mouse-driven game updates are **not yet connected**. Never call the current native image playable or mark R04 accepted.
5. Original media, decoded pictures, original saves and screenshots stay read-only/private and are not committed to the public `Recomp` branch.

**Next priority:** wire original `1F8FC` scene/panel pipeline to this retained guest and VGA, then original `17A68` actor post-load callbacks with preserved original quest state, then original `1FC14` world updates and genuinely interactive frame rendering.
