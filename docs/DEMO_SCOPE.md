# Vertical slice target

Primary target: **Forgotten Realms Forest**, the opening gameplay area containing the ASSASSIN encounter (LEVELS.DAT level 22).

Demo endpoint: complete the connected Mist transition after the assassin-object sequence. The transition may preserve the executable-proven destination metadata for Svalich Woods, but **Svalich Woods is not playable in this demo**.

The slice must eventually include the original:
- level geometry and world state
- scenery, NPC and object placements
- movement/collision and interactions
- assassin AI/combat/death behavior and resulting story state/items
- UI, inventory and party state required by the sequence
- music, sound and voice used in the slice
- cinematics and transitions invoked by the original
- save/load state needed to preserve the slice

No hand-placed corrections from the old viewer are permitted.

## Demo party

The proof-of-concept boots with the shipped **Quick Start** party from `SAVE.DAT` rather than invented test characters:

- EHLIANNA — Elf Fighter/Mage
- ANVIL DAWN — Dwarf Fighter/Cleric

Their original 0x248-byte character records are loaded directly, preserving portrait indices, stats, XP/levels, carried items and all still-unexposed record fields verbatim. This gives the inventory/character/UI work authentic state from the first runnable build. Full CODE.1 character creation remains a later full-port milestone, not a requirement for this vertical slice.
