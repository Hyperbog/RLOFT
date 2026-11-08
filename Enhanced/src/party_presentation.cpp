#include "rloft/party_presentation.hpp"

#include <stdexcept>

namespace rloft {
namespace {

std::uint16_t u16(const std::uint8_t* p) noexcept {
    return static_cast<std::uint16_t>(p[0]) |
           (static_cast<std::uint16_t>(p[1]) << 8U);
}

std::int16_t s16(const std::uint8_t* p) noexcept {
    return static_cast<std::int16_t>(u16(p));
}

std::uint32_t u32(const std::uint8_t* p) noexcept {
    return static_cast<std::uint32_t>(p[0]) |
           (static_cast<std::uint32_t>(p[1]) << 8U) |
           (static_cast<std::uint32_t>(p[2]) << 16U) |
           (static_cast<std::uint32_t>(p[3]) << 24U);
}

}  // namespace

bool PartyActorPresentation::item_known(std::size_t slot) const {
    if (slot >= kInventorySlotCount) {
        throw std::out_of_range("party inventory slot");
    }
    return (item_known_bits & (1U << slot)) != 0;
}

InventorySlotRole PartyActorPresentation::slot_role(std::size_t slot) {
    if (slot < kOrdinaryInventoryCount) {
        return InventorySlotRole::Ordinary;
    }
    switch (slot) {
        case 12: return InventorySlotRole::RingA;
        case 13: return InventorySlotRole::RingB;
        case 14: return InventorySlotRole::HandA;
        case 15: return InventorySlotRole::HandB;
        case 16: return InventorySlotRole::Body;
        case 17: return InventorySlotRole::Head;
        case 18: return InventorySlotRole::Accessory;
        case 19: return InventorySlotRole::Quiver;
        case 20: return InventorySlotRole::BracerGauntlet;
        default:
            throw std::out_of_range("party inventory slot");
    }
}

int PartyActorPresentation::portrait_local_index() const noexcept {
    int local =
        static_cast<int>(
            static_cast<std::int8_t>(portrait_index)) +
        static_cast<int>(sex_code) * 23;
    if ((status_word & 0x0100U) != 0) {
        local += 46;
    }
    return local;
}

std::int32_t PartyActorPresentation::effective_hp() const noexcept {
    const auto raw =
        static_cast<std::uint32_t>(
            static_cast<std::int32_t>(current_hp)) +
        static_cast<std::uint32_t>(hp_modifier_113) +
        static_cast<std::uint32_t>(hp_modifier_1ea);
    return static_cast<std::int32_t>(raw);
}

PartyActorPresentation PartyPresentationModel::actor(
    const PartyImage& party,
    std::size_t actor_index) {
    if (actor_index >= PartyImage::kActorCount) {
        throw std::out_of_range("party actor index");
    }

    const auto base = actor_index * PartyImage::kActorSize;
    const auto* bytes = party.bytes.data() + base;

    PartyActorPresentation out{};
    out.actor_index = actor_index;
    out.name = party.name(actor_index);
    out.status_word = u16(bytes + 0x010);
    out.active = (out.status_word & 0x8000U) == 0;
    out.portrait_index = bytes[0x012];
    out.sex_code = bytes[0x013];
    out.race_code = bytes[0x014];
    out.base_armor_class =
        static_cast<std::int8_t>(bytes[0x01B]);
    out.current_hp = s16(bytes + 0x01C);
    out.max_hp = s16(bytes + 0x01E);
    out.hp_modifier_113 =
        static_cast<std::int32_t>(u32(bytes + 0x113));
    out.condition_effect_flags = u32(bytes + 0x132);
    out.hp_modifier_1ea =
        static_cast<std::int32_t>(u32(bytes + 0x1EA));

    for (std::size_t slot = 0; slot < out.items.size(); ++slot) {
        out.items[slot] =
            s16(bytes + 0x040 + slot * 2U);
    }
    out.item_known_bits = u32(bytes + 0x12B);
    return out;
}

}  // namespace rloft
