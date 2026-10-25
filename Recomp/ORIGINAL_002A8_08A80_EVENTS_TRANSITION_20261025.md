# Part 33 — original CODE.2 script moving records and same-level events

**Branch:** `Recomp`; no original copyrighted media added to the repository.

## Original source branches connected

- Original `002A8` script opcode **3**, guarded for **type-0 through type-8 moving records**, runs the compiled original `08A80` constructor against the retained guest. Original `0E578` clears the source owner-state flags, `30CA2` advances the original random generator, and any sound request is passed to the original `28110` queue. The `0x248EC` moving-record array and `0xC038` count are mutated **by CODE.2**, never by host code.
- Preflight validates all instructions in the descriptor before executing any of them; invalid actor types, off-map coordinates, overflowing record counts, and unimplemented script callbacks are still rejected. Original `08A80` types **0–7** are excluded because they can require original `22E90` collision repositioning and source redraw callbacks.
- An in-world portal targeting the **already-active Level 22** now follows the real `181B4` equal-level return instead of incorrectly re-running the one-shot boot loader. This path performs no new file reads and leaves the level's script table, object list, and tiles intact.

- Original \`002A8\` nested opcode **11** executes the latched return branches for source \`000B8\` (event bit \`0x3D\`) and \`001FC\` (bit \`0x46\`) through compiled source. All other early handlers require UI/selection callback work and remain guarded.

## Cross-level frontier (not accepted)

A transition to another level is more than a change to `C6C8`. Source `181B4` first invokes original `171FC` persistence of the departing level, then closes old graphic/actor/script allocations, reads the destination `LEVxxx.DAT` or `LEVELS.DAT`, performs `17A68` NPC transformations, and refreshes display/music mode. The retained live loader remains boot-scoped to Level 22 and has one-shot scene-state counter assertions. Executing those callbacks against an unverified live world risks irreversible mid-level guest mutation.

**Next:** supply source-authentic writable private level-save/file callbacks for `171FC`, prove fresh `181B4` target-level file load and object/script-table allocation, and re-enter `1FC14` after a live world replacement. Verify this against owner-held Level 22/23 retail data; no claim of full cross-level playability until it passes.

## Evidence scope

The new compiled source tests exercise synthetic guest state only. Existing Windows preserved-media checks still assert live Level 22 movement, scene output, and keyboard polls. Human gameplay, independent DOS synchronized indexed frame parity, original AIL audio and cross-level transitions remain open.
