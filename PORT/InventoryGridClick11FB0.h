#pragma once

#include "games/ravenloft/game/InventorySlotExchange117A0.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>

namespace ravenloft::game {

struct InventoryGridClickContext11FB0 {
    // DS:6F90..6F9C. A nonzero entry switches that character panel away from
    // the ordinary inventory grid, so 0x11FB0 refuses this path.
    std::array<std::uint32_t,4> panelMode6F90{};
    InventorySlotExchangeContext117A0 exchangeContext{};
};

struct InventoryGridClickResult11FB0 {
    bool handled=false;          // nonzero EAX returned to the outer UI dispatcher
    bool exchangeHandled=false;  // opening-relevant 0x117A0 empty-slot exchange ran
    std::size_t characterIndex=0;
    std::size_t slotIndex=0;
};

// Opening inventory-grid click path from CODE.2 0x12E70/0x12F9B..0x12FC8
// through 0x11FB0..0x12029 and into the already reconstructed 0x117A0
// exchange. Coordinates are the original interface framebuffer coordinates.
class InventoryGridClick11FB0 {
public:
    static constexpr std::int32_t kPanelHeight=0x50; // 80
    static constexpr std::int32_t kLeft=0x129;       // 297
    static constexpr std::int32_t kRight=0x18C;      // 396, inclusive
    static constexpr std::int32_t kTop=0x05;
    static constexpr std::int32_t kBottom=0x4A;      // 74, inclusive
    static constexpr std::int32_t kColumnPitch=0x22; // 34
    static constexpr std::int32_t kRowPitch=0x12;    // 18

    // Exact positive-coordinate slot arithmetic reached after the caller's
    // rectangle checks. Returns no slot outside that caller-owned rectangle.
    static std::optional<std::size_t> mapSlot(std::int32_t x,std::int32_t localY);

    static InventoryGridClickResult11FB0 execute(
        std::int32_t screenX,
        std::int32_t screenY,
        std::uint32_t clickFlags,
        HeldObjectState1A344& held,
        std::array<data::CharacterRecord,4>& party,
        const InventoryGridClickContext11FB0& context={});
};

} // namespace ravenloft::game
