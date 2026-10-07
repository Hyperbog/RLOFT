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

int PartyActorPresentation::portrait_local_index() const noexcept {
    int local =
        static_cast<int>(portrait_index) +
        static_cast<int>(sex_code) * 23;
    if ((status_word & 0x0100U) != 0) {
        local += 46;
    }
    return local;
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
    out.base_armor_class =
        static_cast<std::int8_t>(bytes[0x01B]);
    out.current_hp = s16(bytes + 0x01C);
    out.max_hp = s16(bytes + 0x01E);

    for (std::size_t slot = 0; slot < out.items.size(); ++slot) {
        out.items[slot] =
            s16(bytes + 0x040 + slot * 2U);
    }
    out.item_known_bits = u32(bytes + 0x12B);
    return out;
}

}  // namespace rloft
