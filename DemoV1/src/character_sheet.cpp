#include "rloft/character_sheet.hpp"

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
std::int32_t s32(const std::uint8_t* p) noexcept {
    return static_cast<std::int32_t>(u32(p));
}

constexpr std::array<int, 6> kAbilityLabelText{{
    0x2F, 0x30, 0x31, 0x32, 0x33, 0x34,
}};

}  // namespace

std::size_t CharacterSheet::class_track_count() const noexcept {
    std::size_t count = 0;
    for (const auto& track : classes) {
        if (track.present()) ++count;
    }
    return count;
}

CharacterSheet CharacterSheetModel::actor(
    const PartyImage& party,
    std::size_t actor_index) {
    if (actor_index >= PartyImage::kActorCount) {
        throw std::out_of_range("party actor index");
    }

    const auto base = actor_index * PartyImage::kActorSize;
    const auto* bytes = party.bytes.data() + base;

    CharacterSheet out{};
    out.actor_index = actor_index;
    out.name = party.name(actor_index);
    out.status_word = u16(bytes + 0x010);
    out.active = (out.status_word & 0x8000U) == 0;
    out.portrait_index = bytes[0x012];
    out.sex_code = bytes[0x013];
    out.race_code = bytes[0x014];
    out.age = u16(bytes + 0x018);
    out.alignment_code = bytes[0x01A];
    out.base_armor_class =
        static_cast<std::int8_t>(bytes[0x01B]);
    out.current_hp = s16(bytes + 0x01C);
    out.max_hp = s16(bytes + 0x01E);

    for (std::size_t i = 0; i < out.classes.size(); ++i) {
        out.classes[i].class_code =
            static_cast<std::int8_t>(bytes[0x015 + i]);
        out.classes[i].level =
            s16(bytes + 0x020 + i * 2U);
        out.classes[i].progress =
            s32(bytes + 0x026 + i * 4U);
    }

    for (std::size_t i = 0; i < out.abilities.size(); ++i) {
        out.abilities[i] = CharacterAbility{
            bytes[0x032 + i * 2U],
            bytes[0x033 + i * 2U],
            kAbilityLabelText[i],
        };
    }

    out.exceptional_strength_base = bytes[0x03E];
    out.exceptional_strength_current = bytes[0x03F];

    return out;
}

}  // namespace rloft
