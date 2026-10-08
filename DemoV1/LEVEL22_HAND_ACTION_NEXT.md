# Level 22 next implementation — live HUD hand inputs

## Original-byte evidence, not a native implementation claim

This research note identifies the next hand/inventory action boundary after the newly connected 12-slot party-modal exchange and the `C80C` accuracy model.

The original CODE.2 object-2 region array at physical file `0x75F24 + DGROUP 0xD684` has 21 active 8-byte hit rectangles plus one `(-1,-1,-1,-1)` terminator. Region 0 covers `row 4..259, column 32..287` (256×256 viewport); 1..16 are four lanes per actor, where `actor=(region-1)/4` and `lane=(region-1)&3`.

The eight live **hand** rectangles from the original shipped bytes are:

| Actor | Hand 0 (lane 0) row; column | Hand 1 (lane 2) row; column |
| --- | --- | --- |
| 0 | 276..307; 4..19 | 276..307; 37..52 |
| 1 | 276..307; 57..72 | 276..307; 90..105 |
| 2 | 276..307; 215..230 | 276..307; 248..263 |
| 3 | 276..307; 268..283 | 276..307; 301..316 |

The intervening lane 1 is spell/actor selection; lane 3 is the full-party inventory panel. This is **not** the same grid geometry as the 12 ordinary inventory tiles in the full-screen modal.

The original `214B8` wrapper, directly re-disassembled:

- `214D9` rejects unavailable/dead actor status before processing flags.
- `214E4` tests **pointer flags bit 3 (`0x08`)** and takes the **item swap path**. It reads actor `item[14+hand]` at `0x2193C + actor*0x248 + hand*2`.
- `2151F..2152A` rejects temporary spell-hand object IDs `0x7E..0x86`.
- `21530..2158B` reads global held ID and known bit; currently held objects with equipment categories 3,4,5,6,7,8 cannot be placed through the generic hand-swap path. This is *category-directed legality*, not free drag/drop.
- `215A1..215E2` queries the old hand's identified state, installs the held object and updates the slot-known bit, then triggers redraw. `215E9` clears the hand's readiness countdown. If the hand becomes empty, `0F238` has the same verified stale-known-bit early exit described in the inventory checkpoint.
- `216C6` tests **pointer flags bit 0 (`0x01`)** for the **use/attack path**. It checks actor status and the readiness gate, invokes `24478`, then `26018`; it does not directly call the Level 22 Assassin damage function with a made-up damage value.

## Implementation status update

The separate [live hand and HUD actor-panel checkpoint](LEVEL22_HAND_INTERACTION_CHECKPOINT.md) now implements the original inclusive hand rectangles, bounded `214B8` identity/known exchange and selected-hand deadline clear, and `211D8` quick stash into first free ordinary inventory slot. Both are connected to a **provisional native right-click adapter**; original DOS queue timing/button semantics still require a trace. The actual `214B8` flag-`01h` use/attack branch remains unwired. The original-value hand geometry and state mutation regressions were added to the Windows test suite.

## Next original-byte audit: hand use/attack dispatch

A further direct disassembly of CODE.2 `26018..260ED` establishes the next controller boundary:

- `2601F..26027`: load actor/hand; process/close prior action context through `19CCC`.
- `2602E..2603F`: inspect a **separate** four-byte per-hand action state at live address `0x219EB + actor*0x248 + hand*4`. If nonzero, return early at `273DF`, without running attack code. This is distinct from the `0x21A7E + actor*0x248 + hand*4` ready-deadline dword reset by the item-swap branch `214B8`.
- `26045..26069`: fetch actual hand object `item[14+hand]` and OBJECTS category `+0x2F`; unarmed hand (`-1`) is explicitly classified as category `0x0A`, not as an invalid catalog read.
- `260B2..260D6`: categories `9,10,11` dispatch into the original `E5F0` player attack orchestrator. Category `0x0E` has a separate hard-coded object-ID dispatch. All remaining category/effect behavior continues deeper in `26018`.
- `214B8:216C6` enters this dispatcher only after actor condition mask `0xF240` and `24478(actor,hand)` approval. `24478` inspects both hand objects and status flags, not just the clicked weapon's presence.

These are **newly bounded control-flow findings**, not implemented playable attacks. The existing native `C80C` hit arithmetic cannot be wired directly to the mouse: preserve the two distinct per-hand state dwords, classification of the unarmed hand, special items, and original `E5F0` effect ordering first.

## Safe integration sequence

1. Add a separate native hit model for exact above four actor hand-pair rectangles, keeping lane 1/3 priority and cursor/inventory region separation.
2. Confirm the original keyboard/mouse input event-to-bit mapping in `229CC` / the pointer queue, then route the `0x08` swap gesture only after the exact category and handedness checks are reconstructed. **Do not assume which Win32 mouse button owns `0x08` solely from a UI convention.**
3. Reuse `InventoryRuntimeState` authoritative item/known pair, preserve hand readiness at actor `+0x18E + 4*hand` (live `0x21A7E + 4*hand`), and re-render inventory/HUD graphics. Keep special equipment/container cases pending.
4. Connect the *distinct* `0x01` action route to `C80C` accuracy only after the original `E5F0` attack dispatcher, `DB94` target selection, damage dice, `C910` category gate and cooldown/feedback are implemented.
5. Replay original-game input and indexed screenshots before labeling a working combat step.

**Completion:** bounded live hand/actor-panel milestone **60% (3/5 acceptance gates)** before CI; overall faithful demo **20%**.
