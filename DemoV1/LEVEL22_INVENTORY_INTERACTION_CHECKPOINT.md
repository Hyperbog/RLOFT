# Level 22 ordinary inventory interaction checkpoint

## Scope and status

Resumed the normal Level 22 gameplay-integration path after the optional direct walkaround milestone. This slice connects the original 12-slot inventory mouse grid to real party+cursor item state, allowing a player who has picked up a loose object to place it into a free ordinary slot and return to the world with an empty cursor. **It does not reimplement combat, equipment/hand action logic, nested-container insertion, spell use or save files.**

### Original executable evidence

The immutable supplied CODE.2 program (SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`) and [PreservationArchive Chapter 07](../PreservationArchive/07_INPUT_UI_MENU_AND_INVENTORY_ARCHITECTURE.md) establish:

- `12E70`: the full-party inventory modal divides the 320-wide logical screen into four 80-pixel actor columns. Ordinary slots are the rectangular 3x4 grid with tile anchors `row = 0x129 + (slot/4)*0x22` and actor-local `column = 5 + (slot%4)*0x12`. Actual drawn tiles are 32 rows × 16 columns, with two-pixel gutters that are not item slots.
- `117A0` begins by computing actor record stride `0x248`, extracting the signed item ID from `0x21920 + actor*0x248 + 2*slot`, extracting the slot-known bit from the four-byte bitfield at `0x21A0B + actor*0x248`, and reading global held ID/known at `0x8600/0x8602`. The effect-placeholder range `0x7E..0x86` is explicitly rejected before ordinary swap.
- The direct replacement path at `11933..11A4D` writes the held item into the actor item array, calls the per-slot known-bit updater (`1F238`), and installs the old slot object and old known state into the cursor via `2A5B0`. Only this identity+known transfer is modeled in the current native ordinary-grid action.
- The original has special object/side-effect branches (including `0x35` and `0xA6`, and `21404` for occupied containers). These **are not generic swaps**; their corresponding native interactions are explicitly marked unsupported until the relevant rule/controller is reconstructed. Equipment slots `12..20`, temporary effects, nested containers and spell-hand actions are likewise out of scope.

### Committed native changes

- `InventoryRuntimeState::swap_ordinary_slot` validates actor/slot, actor status, object IDs and special-object/container cases before touching memory. It atomically exchanges the character's object ID, its correct bit in the 32-bit `+0x12B` known-bitfield and the global held ID/known byte. It returns `NoChange`, `Swapped` or `Unsupported`.
- `PartyPanelControlModel::modal_ordinary_slot_at` resolves exact active 32×16 tiles independently of graphics, including all four 80-pixel actor lanes. It rejects the two-pixel gaps, screen margins and out-of-panel coordinates.
- The Windows `PartyPanels` modal now handles the toggle first, then ordinary inventory tiles for active actors in inventory (not character-sheet) mode. On a successful change it invalidates the last rendered world-action frame. A native title-bar diagnostic reports the held item ID and identified status; unsupported category-specific interactions are reported as pending rather than silently changing persistent state.
- Existing loose-item pickup remains guarded while another item is held; the user can now empty the cursor by opening the full party panel and placing an ordinary object into an eligible slot. Normal intro/gameplay and opt-in walkaround share this behavior.

### Regression coverage

1. Every ordinary tile's first/last pixel across all four actors; two-pixel horizontal/vertical gutters and modal bounds.
2. Slot → cursor pickup carries identified state; cursor → empty slot stashes the object; cursor ↔ occupied slot moves both known bits.
3. Exact empty/empty no-op without clobbering a pre-existing stale bit; inactive actors, invalid slots, temporary spell-hand objects and held-over-container interactions preserve state.
4. Assassin's original four death-drop object IDs → three sequential pickups and ordinary-slot stashes → fourth pickup reports the exact Level 22 mist predicate. This is a deterministic model integration test, not an enabled combat/transition sequence.

### Acceptance boundaries and next priority

- [x] Original bytecode and archival crosswalk for ordinary 12-slot swap.
- [x] Native swap and original tile hit geometry.
- [x] Win32 modal input routing and held-item feedback.
- [x] Synthetic core/integration regression tests added.
- [ ] Fresh Windows CI compilation and all tests pass after these changes.
- [ ] Original-game resource playtest of pickup → inventory stash → world return, including cursor graphic and menu-mode sequencing.

**Inventory interaction subcheckpoint: 67% (4/6 gates)** prior to build and playtest verification.

**Overall faithful-demo acceptance: 20% (1/5 major release gates)**. This percentage does not advance until the broader inventory, original combat, main wall renderer and story-transition acceptance conditions close. The previously completed initial walkaround milestone remains 100% within its own deliberately bounded scope.

Next implementation targets: original `214B8` live HUD hand use versus swap; `117A0` container auto-insertion; original combat hit/THAC0/damage controller before enabling Assassin viewport attacks; original 22-frame transition and Level 23 handoff. Re-run full Windows build before treating new executable as tested.
