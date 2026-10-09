# Level 22 completion gate and original pickup feedback audit

## Required vertical-slice ordering

**The entire original Level 22 must be playable, source-faithful and accepted before any independent Level 23 gameplay work begins.** The Level22 development/testbed is the real game's initial slice, connected to the original boot, character creation and actual CODE.2 gameplay. Source study of the *Level22 exit destination* is permitted solely to validate the Level22 ending. It does **not** authorize Level23 AI, levels, assets, interactions or campaign feature implementation before Level22 is finished.

The original 22-frame mist sequence is part of Level22's **ending**. Its original frame-20 call to `181B4(23)` and frame-21 display references must ultimately be reproduced faithfully. That unavoidable handoff is a boundary condition of Level22 completion—not an authorization to work ahead on Level23 gameplay.

**Gate 1 — Level22:** original startup → CODE.1 creation → party load → real movement/collision/view/palette/HUD/automap → player combat through original hit, damage, timing and NPC reaction → Level22 Assassin death and four original drops → source-equivalent pickup/cursor/equipment → all four relic predicates → actual effect/camera staging → 22 original mist frames, exact frame-20 loader call → original three relic removals → return/teardown. Preserve original world/party/global/timing/IRQ/RNG and validate under original DOS comparison. Basic combat/usable items and original menus/click regions must be playable rather than stubbed developer commands. Continue accepting/regressing all 122 subsystems only as applicable to this slice.

**Gate 2 — future original levels:** only after Gate 1 is confirmed playable and source-faithful. Reuse the same production engine for each additional level in original story order; no redesign/modding/QoL until the full original-game code recreation is complete.

## Verified CODE.2 21704 picked-item message selection

Disassembled original file `CODE.2` SHA256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`, object-1 physical base `0x3AF24`.

After original `21704` invokes `1A344` to place the picked world object into the hand, and compacts family C, it makes conditional assignments to the **original global `BF44`**. These are assignments, not immediately a replacement UI widget or cinematic.

| Source offset | Condition | Assign BF44 |
|---|---|---:|
| 21767..21789 | ID 61h..64h and original `F4B0(35h)` Tome possession | 0Fh |
| 21789..2179F | ID 49h | 25h |
| 2179F..217B7 | ID 9Eh | 36h |
| 217B7..217CD | ID 5Fh | 37h |
| 217CD..217E5 | ID 8Bh | 38h |
| 217E5..2180F | original `F2B4(id)` true, **except** 5Fh; `F2B4` returns true only for 10h and 5Fh | 39h for ID 10h |
| 2180F..2182E | ID A7h..C9h | 3Ah |
| 2182E..21844 | ID 77h | 3Ch |
| 21844..2185A | ID 75h | 29h |
| 2185A..2187E | ID 74h and original event bit 51h via `1C1DC` | 2Fh |

**Original quirk:** when no branch matches, BF44 is unchanged. Do not reset to zero or synthesize a new generic message. In particular, the Level22 relic pickups 87h/88h/E9h do not modify BF44 here; 74h modifies it only when event flag 51h is already set.

### Implementation

`OriginalPickupMessageModel::assignment_21704` returns an optional original BF44 **write**, preserving the absent/write distinction. `Level22EncounterModel::pickup_loose_item` reports the source write from the same real picked object after live cursor/family-C mutation; `ExplorationRuntime` forwards the *existing* authoritative `session_.persistent_event_flags` bit 51h. No synthetic user-facing text is introduced.

`original_pickup_message_tests` covers all ID cases, range endpoints and conditional flags. Existing `level22_encounter_tests` now checks Holy Symbol pickup with bit 51h set/clear. Production BF44 UI ribbon presenter and original source sound/clock remain pending—the result does **not** claim it renders original graphics.

**CTest registration:** 177 total / 19 Quick, none of these newly authored tests run. Historical Windows Full #647 172/172 at old source `fb4a0145`. Completion percentages should not increase merely for a model without DOS/runtime acceptance.
