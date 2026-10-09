# Part 9 / Level22 — source-backed global BF44 story state

**Authority:** original CODE.2 object-2 physical base 0x75F24 and object-1 executable at 0x3AF24; SHA256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.

## Original executable writes

- Shipped object-2 initial DWORD at physical `0x75F24 + 0xBF44 = 0x81E68` is `00 00 00 00`.
- Original `E3C9..E443` handles type-9 Assassin death. After four calls to `2195C` for the original relic drops, original `E436` writes literal `BF44=1`. Nonlethal damage does not do this.
- Original `21767..2187E` performs special item pickup BF44 writes documented separately in [original pickup message audit](LEVEL22_COMPLETION_AND_PICKUP_FEEDBACK_GATE_20261009.md). A pickup with no special branch does **not** reset BF44.
- Additional CODE.2 message code writers and actual `1C21C` rendering are **not** fully integrated. This is source fidelity state plumbing, not a completed HUD ribbon renderer.

## Native integration checkpoint

`ExplorationRuntime` now owns the original global DWORD as `original_story_message_bf44_`, initialized to the real shipped zero value on fresh `initialize_gameplay_state`. Original Level22 type9 death sets value **1** only after the shared death/drop dispatcher returns; ordinary damage leaves it unchanged. The original Level22 pickup path updates it only when the source-backed message model returns a real conditional write, preserving the previous value for unmatched objects. The current public reader is read-only and returns the authoritative native value.

The newly authored pure-model `original_pickup_message_tests` and expanded `level22_encounter_tests` cover conditional pickup writes. The existing `exploration_runtime_tests` checks BF44 original initial state. The whole runtime's full native transition and actual BF44 story-message paint still require acceptance; do **not** label this 100%.

**Level22 gate:** Finish playable combat, original story message/display, item exchanges, four relic pickups and original mist presentation before starting independent Level23 gameplay. [Required slice charter](LEVEL22_COMPLETION_AND_PICKUP_FEEDBACK_GATE_20261009.md).

**Build:** no user-authorized build yet. **177 registered CTests** (Quick 19); previous verified Full 172/172 at source `fb4a0145`. Percentage engineering readiness unchanged at 49.8% until live/render/DOS acceptance.
