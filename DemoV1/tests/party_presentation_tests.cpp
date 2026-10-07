#include "rloft/party_presentation.hpp"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace {

void put16(std::vector<std::uint8_t>& b,
           std::size_t o,
           std::uint16_t v) {
    b[o] = static_cast<std::uint8_t>(v);
    b[o + 1U] = static_cast<std::uint8_t>(v >> 8U);
}

void put32(std::vector<std::uint8_t>& b,
           std::size_t o,
           std::uint32_t v) {
    for (int i = 0; i < 4; ++i) {
        b[o + static_cast<std::size_t>(i)] =
            static_cast<std::uint8_t>(v >> (i * 8));
    }
}

}  // namespace

int main() {
    std::vector<std::uint8_t> bytes(rloft::PartyImage::kSize, 0xFF);
    std::fill(bytes.begin(), bytes.begin() + 16, 0);
    bytes[0] = 'A';
    bytes[1] = 'N';
    bytes[2] = 'N';

    put16(bytes, 0x010, 0x0100);  // active + possessed portrait bank
    bytes[0x012] = 2;
    bytes[0x013] = 1;
    bytes[0x01B] = 0xFF;          // signed AC -1
    put16(bytes, 0x01C, 12);
    put16(bytes, 0x01E, 20);

    for (std::size_t slot = 0;
         slot < rloft::PartyActorPresentation::kInventorySlotCount;
         ++slot) {
        put16(bytes, 0x040 + slot * 2U, 0xFFFF);
    }
    put16(bytes, 0x040 + 0U * 2U, 5);
    put16(bytes, 0x040 + 12U * 2U, 7);
    put32(bytes, 0x12B, (1U << 0) | (1U << 12));

    const auto party = rloft::PartyImage::from_bytes(bytes);
    const auto actor = rloft::PartyPresentationModel::actor(party, 0);

    assert(actor.name == "ANN");
    assert(actor.active);
    assert(actor.status_word == 0x0100);
    assert(actor.portrait_index == 2);
    assert(actor.sex_code == 1);
    assert(actor.portrait_local_index() == 71);
    assert(actor.base_armor_class == -1);
    assert(actor.current_hp == 12);
    assert(actor.max_hp == 20);
    assert(actor.items[0] == 5);
    assert(actor.items[1] == -1);
    assert(actor.items[12] == 7);
    assert(actor.item_known(0));
    assert(actor.item_known(12));
    assert(!actor.item_known(1));

    assert(rloft::PartyActorPresentation::slot_role(0) ==
           rloft::InventorySlotRole::Ordinary);
    assert(rloft::PartyActorPresentation::slot_role(11) ==
           rloft::InventorySlotRole::Ordinary);
    assert(rloft::PartyActorPresentation::slot_role(12) ==
           rloft::InventorySlotRole::RingA);
    assert(rloft::PartyActorPresentation::slot_role(14) ==
           rloft::InventorySlotRole::HandA);
    assert(rloft::PartyActorPresentation::slot_role(16) ==
           rloft::InventorySlotRole::Body);
    assert(rloft::PartyActorPresentation::slot_role(19) ==
           rloft::InventorySlotRole::Quiver);
    assert(rloft::PartyActorPresentation::slot_role(20) ==
           rloft::InventorySlotRole::BracerGauntlet);

    bool rejected = false;
    try {
        (void)actor.item_known(21);
    } catch (const std::out_of_range&) {
        rejected = true;
    }
    assert(rejected);

    rejected = false;
    try {
        (void)rloft::PartyActorPresentation::slot_role(21);
    } catch (const std::out_of_range&) {
        rejected = true;
    }
    assert(rejected);
}
