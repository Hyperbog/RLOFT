# CODE.2 12F8F → 11AD8 → 11B63 F790 actor-pane event — October 10, 2026

**Status:** immutable original-file verified, native integration research only. This documents an additional genuine F790 event family, not another 70-Hz refresh or a change to live gameplay. The previously authorized Windows Full rebuild may still be compiling a prior checkout.

## Binary provenance and control flow

Original shipped `CODE.2` is 558,689 bytes with SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`, CODE.2 object-1 file base `0x3AF24`. Independent disassembly and direct instruction-target checks against the original confirm:

| Original site | Exact original bytes | Effect |
| --- | --- | --- |
| `12F76..` | `83 FE 45 7C 20 83 FE 60 7F 1B 83 FB 39 7C 16 83 FB 4D 7F 11` | Modal actor hit: original vertical 0x45..0x60; actor-local horizontal 0x39..0x4D |
| `12F8F` | `E8 44 EB FF FF` | Calls `11AD8` with actor index and original input flag |
| `11AE9` | `F6 44 24 18 02 74 40` | Distinguishes bit-2 input mode from the per-actor mode |
| `11B20` | `E8 43 F7 FF FF` | Source four-actor loop calls `11268` |
| `11B30` | `80 B0 90 6F 00 00 01` | Normal path toggles 6F90 actor-mode bit |
| `11B4B` | `E8 18 F7 FF FF` | Normal path redraws selected actor through `11268` |
| `11B63` | `E8 28 DC FF FF` | Both branches call original `F790` once, with original global held WORD 8600 and known BYTE 8602 |

The bit-2 branch takes selected actor's existing 6F90 value, toggles its low mode bit, propagates that value to all four actor mode contexts and repaints all four columns. The other branch toggles the selected actor and repaints that one column. Both call F790 through the **same** `11B63` source site after the repaint. F790 pending feedback `6DAC` still takes priority over ordinary held-item text and can consume the shared 30CA2 RNG according to the previously audited branch.

## Native contract comparison

The existing native `PartyPanelControlModel::modal_toggle_actor_at` already uses exactly the original source rectangle (row 0x45..0x60; actor-local column 0x39..0x4D). The Win32 input path calls `ui.toggle_party_panel(*actor)`, which currently models **only the individual-actor toggle**. This is *not* complete proof that the native input forwards DOS flag bit 2, nor that the native renderer dispatches source `11B63` F790. At present, the source event remains unimplemented in the native host.

**Necessary next integration proof:** identify the native DOS-equivalent flag source at `12E70/12F8A`; establish whether any real player input passes bit 2 and how the host should produce it. Then add the actor redraw and exactly one `11B63` source-owned F790 event using global-held arguments, and compare original DOS indexed frames, RNG/tick sequence, and both toggle modes. Do not map all native repaints to `11B63`, and do not invent a mouse modifier.

This is a bounded original-source handoff to the separate decompilation workstream as needed. There is no original source code change or unverified game functionality committed here.
