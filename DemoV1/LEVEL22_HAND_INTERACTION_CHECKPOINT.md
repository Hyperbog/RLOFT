# Level 22 live HUD hand and actor-panel interactions

Status: original CODE.2 semantics cross-checked, bounded native implementation + synthetic tests committed, Windows CI fully passed (102/102), original-game interactive acceptance pending.

## Original DOS evidence

The immutable original `CODE.2` SHA-256 is `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`. The native model is constrained by original object-1 disassembly, not inferred UI conventions:

- `229CC..22CE0` scans the original DGROUP `D684` table of 21 inclusive row/column rectangles plus sentinel. Region 0 is world viewport `row 4..259; col 32..287`. Four actor groups of lanes 0..3 follow. Every actor has two hand tiles (lanes 0/2), spell/actor selection (lane 1) and inventory panel (lane 3).
- `214B8`: argument `actor`, `hand=0/1`, `pointer_flags`. Before action, `actor.status & 0x8000` rejects unavailable actors. **Flag `0x08`** requests a held-cursor/hand-object swap; **flag `0x01`** uses/attacks with readiness/target restrictions via `24478/26018`.
- `214B8:21517` locates `item[14 + hand]` by actor stride `0x248`; current pseudo-effect object IDs `0x7E..0x86` are not movable. `21556..2158B` rejects incoming category codes `3,4,5,6,7,8` for this general hand action. Other hand/equipment/container state can require additional dedicated side effects.
- `215A1..215E2` captures old known bit using `0F218`, installs the cursor-held ID in the hand, and calls `0F238` with the new known state. The original **leaves a stale identified bit** when the new slot is empty because `0F238` returns early on `item[slot] == -1`. `215E9` clears the selected hand's readiness deadline `actor+0x18E + hand*4`; the opposite hand's deadline is untouched. The outer handler refreshes sprites and optional active-container submenu.
- `211D8:21212` branches on pointer flag `0x08` in the actor panel when an item is held. Special incoming categories `0x0F` and `0x0D` enter dedicated container/hand placement branches. In the ordinary fallback `2143F..21478` the original scans slots `0..11` and puts the held object into the first `-1` slot. It resets the global cursor to `(-1,known=0)` using `2A344` **without calling `0F238`**. The destination slot's existing known bit is not changed, even when the held item's known flag was 1. A full 12-slot grid invokes the original inventory-full UI and keeps the cursor item.

### Exact original HUD hand hit rectangles

| Actor | Hand 0 (row; col) | Hand 1 (row; col) |
| --- | --- | --- |
| 0 | 276..307; 4..19 | 276..307; 37..52 |
| 1 | 276..307; 57..72 | 276..307; 90..105 |
| 2 | 276..307; 215..230 | 276..307; 248..263 |
| 3 | 276..307; 268..283 | 276..307; 301..316 |

The immediate adjacent spaces are actor spell/control lanes or gutters, **not** hand hit rectangles. These values were checked directly against the original object-2 bytes at `CODE.2[0x75F24 + 0xD684:]`.

## Committed native implementation

- `PartyPanelControlModel::gameplay_hand_at(row,col)` resolves exactly one of the eight original hand rectangles or returns no hand.
- `InventoryRuntimeState::swap_hand_slot(party,objects,actor,hand)` validates the actor, hand and supported IDs/categories before touching state. It atomically transfers canonical `item[14+hand]`, four-byte `+0x12B` item-known bits and held item+known. It preserves `0F238`'s stale-bit early return when empty and clears exactly that hand's four-byte readiness deadline.
- `InventoryRuntimeState::quick_stash_held(party,objects,actor)` emulates the generic original `211D8` first-empty ordinary `item[0..11]` path. It never overwrites occupied slots, preserves an item when full, and leaves `+0x12B` known bits unchanged as in original `2143F`.
- The Win32 gameplay viewport uses a **provisional native right-click adapter** to request these `0x08` item-movement operations: right-click hand tile → hand exchange; right-click inventory actor panel → ordinary quick-stash. The exact DOS driver/click/hold-time mapping to queue flags is not yet verified and must not be claimed identical. Left-click hand actions are not enabled until original `214B8` flag `0x01` attack/use orchestration exists.
- The special category-1 container/active-submenu paths, effect placeholders, and category-specific equipment handling are **rejected rather than guessed**. These are bounded implementation differences, not original interaction equivalence.
- Successful state mutations invalidate cached world hit-test frames and update the native window caption. The original held-item cursor artwork, hand sound, readiness re-paint and actual per-frame visual validation remain to be completed.

## Regression and acceptance

- [x] Original opcode and 21-region table audit, with correct actor/hand/status/known offsets.
- [x] Actual native hand and actor-panel input routing with safe non-combat interactions.
- [x] Regression tests for exact hit rectangles, known-bit swap, independent hand readiness, pseudo-item/actor/category exclusion, quick-stash first-free ordering, full-inventory loss prevention, category 0F/0D separation.
- [x] Fresh Windows x64 MSVC build, CODE.2 syntax gate, all **102/102** CTests, artifact upload and Git EXE publication verified in [GitHub Actions run #631](https://github.com/Hyperbog/RLOFT/actions/runs/37719712911). The added hand geometry, inventory state and four-relic fast-stash integration tests passed. Published EXE SHA-256 `fe79c73de9e6560fdcd967854d817093bf03130141a21f02303fd0853e469f56`.
- [ ] Installed-original Windows playtest of hand exchange, direct actor-panel stash, cursor feedback, and return to Level 22 world (plus compare DOS button mapping).

**Bounded hand/actor-panel checkpoint: 80% (4/5 gates)** after successful Windows CI. Only real installed-game interaction and original DOS input-trace confirmation remain in the final combined playtest gate. **Overall faithful-demo release acceptance: 20% (1/5 major release gates)**; the initial environmental walkaround remains separately closed.

The next main-gameplay dependency is original `214B8` flag `0x01` → `24478/26018` → `E5F0` weapon/THAC0/dice/NPC gate/feedback/cooldown, followed by Assassin death, relics and mist/Level 23 handoff. No artificial damage roll or scripted kill has been enabled.
