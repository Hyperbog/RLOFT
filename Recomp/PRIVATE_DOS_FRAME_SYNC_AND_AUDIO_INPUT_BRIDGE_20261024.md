# Part 29 — private DOS indexed-frame synchronization and audio/input source bridge

**Branch:** `Recomp` (2026-10-24). **Ownership:** no retail executable, RES image, screenshot, DOSBox state, indexed framebuffer, RGB6 bank or raw VGA plane belongs in Git, CI artifacts or published build ZIPs.

## New preserved-original interfaces

The already verified retained `1FC14` loop performs `1F1C8` scene rendering, original `1A510` CRTC/page handoff, `1F494` HUD, `28168` sound service and `1F120` sound/state, then returns to original `30B40` keyboard polling. The last accepted checkpoint proved five post-scene cycles, not five independent complete `1F8FC` compositor presentations.

### Owner-private native framebuffer captures

Set `RLOFT_RECOMP_PRIVATE_1FC14_FRAME_DIR` to a private output directory on the same machine as the preserved originals. **No files are written by default.** For every authenticated `1FC14` source cycle, the next `30B40` poll records the actually selected VGA scanout *after* post-scene HUD/audio processing:

- `level22_source_tick_N.idx`: 128,000 top-down, row-major, indexed `320×400` bytes.
- `level22_source_tick_N.pal`: 768 original DAC RGB6 bytes in 256 RGB triples.
- `level22_source_tick_N.json`: `rloft-vga-observation-v1` receipt with original guest level (22), `1FC14_1A510` source boundary, raw 16.16 X/Y camera position (`255D0/255D4`), original heading (`C658`), selected CRTC start, source-tick index, SHA-256 of **little-endian 16-bit consumed keyboard codes**, and hashes of both private files. It explicitly says `original_dos_executed=false` and `input_origin=HOST_30B40_EXPLICIT_REPLY`.

The capture code only runs after scene/page/HUD/audio cycle witnesses pass. It checks actual RGB6 range and scanout dimensions; a capture failure fails closed. It neither changes the guest camera nor produces an imitation reference DOS image.

[Windows original-media #40074872545](https://github.com/Hyperbog/RLOFT/actions/runs/40074872545) **PASSED** a required, private-only five-frame capture test (five 128,000-byte index buffers and five 768-byte palettes; original hashes equal their JSON receipts). The temporary raw files are destroyed before the CI job exits and are outside published artifact paths. This is **recompiled-source capture validation**, not DOS-vs-native parity.

### Strict original DOS synchronization gate

The preexisting `check_dosboxx_2026_10_frame_pair.py` accepts **only** the independently tested DOSBox-X `2026.10.01 (SDL2)` VGA save-state serializer, identifies the exact CRTC-selected page and RGB6 table, and exports owner-private `.idx/.pal` only when the **separate actual emulator screenshot matches all 128,000 saved-state RGB pixels**. No screenshots or save states were provided in this checkpoint; original screenshot synchronization has **not** been measured here.

The new `verify_synchronized_dos_frame_pair.py` rechecks that DOSBox-X receipt and actual original file hashes, then requires separate `rloft-vga-observation-v1` receipts for the original DOS debugger **and** recompiled source. It refuses mismatched level, `1F8FC_1A5B0` vs `1FC14_1A510` presentation boundary, raw camera X/Y, heading, CRTC, source tick and keyboard trace. After those prerequisites it compares all indexed bytes and all RGB6 bytes. Its report **always** says `original_dos_execution_independently_attested=false` and `retail_dos_parity_certified=false`: receipt matching is necessary but not sufficient for independent debugger provenance.

To produce a real independent DOS observation, halt DOSBox-X at the corresponding original post-scene `30B40` boundary on the same original input sequence, take screenshot and save state at the exact same displayed instant, and independently record logical level (`C6C8`), camera (`255D0/255D4`), heading (`C658`), CRTC page and full 16-bit source keyboard sequence. Do **not** fill an original receipt by copying reconstructed values. Frame tick number must describe the same source cycle, not a guessed wall-clock interval.

Example owner-local sequence after collecting independent original DOS evidence:

```shell
python Recomp/tools/check_dosboxx_2026_10_frame_pair.py \
  --state PRIVATE_DOS_STATE.sav --screenshot PRIVATE_DOS_SCREEN.png \
  --report PRIVATE_DOS_STATE_REPORT.json \
  --export-dos-index PRIVATE_DOS.idx \
  --export-dos-palette PRIVATE_DOS.pal

python Recomp/tools/verify_synchronized_dos_frame_pair.py \
  --dos-index PRIVATE_DOS.idx --dos-palette PRIVATE_DOS.pal \
  --native-index PRIVATE_NATIVE.idx --native-palette PRIVATE_NATIVE.pal \
  --dos-observation PRIVATE_DOS_OBSERVATION.json \
  --native-observation PRIVATE_NATIVE.json \
  --dosbox-state-report PRIVATE_DOS_STATE_REPORT.json \
  --report PRIVATE_COMPARISON_RECEIPT.json
```

The private original DOS observation must be debugger-sourced in the exact schema. None was created, inferred, or committed here.

### Actual original keyboard host service, not simulated IRQ9

The `--console-input` option to `rloft_code1_code2_staged_boot` is opt-in. Following the existing bounded original-media startup and virtual DOS mouse/device fixtures, it leaves the **same** `CODE.2` guest worker suspended at original `1FC14 → 30B40` and reads human terminal command lines:

| Terminal input + Enter | Original scan word |
| --- | --- |
| W | `0x148` forward |
| A | `0x14B` left turn |
| S | `0x150` backward |
| D | `0x14D` right turn |
| empty line / Space | `0` no key |
| Q | stop host console loop, no game key |

Each actual host input resumes the original `30B40` service and requires a return to the next source keyboard poll on the same guest. No input is fabricated by the console loop. This is **terminal-translated input**, not a native game-window controller, DOS `IRQ9`, DOS key repeat, or game playability certificate. The upstream boot remains an explicit original-media engineering test.

### Audio: real original software queue, not AIL playback

Original CODE.2 binary `28110` and reconstructed `function_28110(Machine&,request,aux,channel,priority)` show the two 12-byte request slots `EE48/EE54`, enabled by `EE40`. A pending record is replaced only according to its **signed** priority comparison. The source `1F120` callback now invokes that compiled original `28110` with all four unchanged source arguments, checking channel 0/1 and guest bounds. It neither synthesizes waveforms nor claims a present AIL hardware backend.

The independent media-free original-source CTest `canonical_code2_original_28110_sampled_sound_queue` covers disabled vs enabled requests, both channels, replacement/rejection and signed priority. [Canonical #40074782322](https://github.com/Hyperbog/RLOFT/actions/runs/40074782322) passed **87/87 CTests**, including this test, and **12/12 synthetic synchronization-gate tests**.

Active AIL playback, driver completion/status and real device timing remain unavailable; source `28AE0/288FC/28F58/29008` still fails closed if `EE78` indicates an unconnected active audio driver. Neither source queue servicing nor reaching `1F120` proves audible output.

## Acceptance ledger

- Source scene returns: **5**; original `1A510` page handoffs: **5**; bounded `1FC14` post-scene cycles: **5**.
- Independent one-shot complete `1F8FC` compositor frame: **1**, not part of the five cycle counts.
- Privately captured completed `1FC14` original-source VGA frames: **5**, hashes checked, **none exported in CI**.
- Independently matched original DOS Level 22 indexed capture pairs: **0**.
- Original sampled-SFX queue mechanics: **compiled and source-tested**; active sound hardware: **not connected**.
- Live terminal text input into real original source poll: **compiled and exposed**, but physical/IRQ9 parity: **not certified**.
- Original DOS hardware DAC RGB6 palette parity: **previously verified**; full frame indexed parity: **open**.
- Coarse whole-game acceptance R04/R07/R08/R09/R12: **unchanged**.

## Next source frontier

Gather a real synchronized DOSBox-X screenshot/save-state/debugger receipt for Level 22 and run the fail-closed pair comparison against the matching private native source tick. Next, map `28168 → 27F24` status/completion callbacks to the selected active AIL interface (without inventing playback), and validate a genuine console/keyboard OS event trace and source state deltas in the retained guest. Do not promote sound requests, terminal key translation, or owner-private captures to complete retail parity.
