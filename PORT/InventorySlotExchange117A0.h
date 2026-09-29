#pragma once

#include "games/ravenloft/data/SaveGame.h"
#include "games/ravenloft/game/ObjectOwnershipF390F4B0.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct InventorySlotExchangeContext117A0 {
    // CODE.2 0x11953 calls F238 immediately after writing the held object into
    // the selected character slot. The reconstructed routine performs F238's
    // bit mutation itself; this callback preserves the original call boundary.
    std::function<void(std::size_t,std::size_t,int)> slotBoundaryF238;

    // Presentation-only F790 boundary. It receives the object/state that was
    // in the slot before the exchange.
    std::function<void(int,std::uint8_t)> presentationF790;

    // Logical 1A5B0 held-object boundary. The reconstructed routine updates
    // HeldObjectState1A344 itself, then exposes the original call boundary.
    std::function<void(int,std::uint8_t)> heldBoundary1A5B0;
};

struct InventorySlotExchangeResult117A0 {
    bool handled=false;
    int oldSlotObject=-1;
    std::uint8_t oldSlotState=0;
};

// Opening-demo path through CODE.2 0x117A0. This is the exact ordinary
// held-object -> empty inventory-slot exchange used by the opening quest-item
// stow flow. The special existing-slot branches (0x7E..0x86, 0xA6, 0x35) and
// container-insertion branch at 0x118C1 are deliberately not claimed here;
// they are unreachable when the alpha selector chooses an empty carried slot.
class InventorySlotExchange117A0 {
public:
    static InventorySlotExchangeResult117A0 executeOpeningEmptySlot(
        std::size_t characterIndex,
        std::size_t slotIndex,
        int allowContainerInsert,
        HeldObjectState1A344& held,
        std::array<data::CharacterRecord,4>& party,
        const InventorySlotExchangeContext117A0& context={});
};

} // namespace ravenloft::game
