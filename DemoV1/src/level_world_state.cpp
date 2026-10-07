#include "rloft/level_world_state.hpp"

#include <cstddef>
#include <cstdint>
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

std::uint32_t fixed(std::int16_t cell, std::uint16_t fraction) noexcept {
    return (static_cast<std::uint32_t>(
                static_cast<std::uint16_t>(cell)) << 16U) |
           fraction;
}

std::uint16_t cell_index(
    int width,
    std::int16_t x,
    std::int16_t y) {
    const auto value =
        static_cast<int>(y) * width +
        static_cast<int>(x);
    if (value < 0 || value > 0xFFFF) {
        throw std::runtime_error(
            "world record cached cell exceeds 16-bit range");
    }
    return static_cast<std::uint16_t>(value);
}

LiveNpcRecord direct_npc(const std::uint8_t* p) noexcept {
    LiveNpcRecord out{};
    out.y_position_16_16 = u32(p + 0x00);
    out.x_position_16_16 = u32(p + 0x04);
    out.runtime_flags = u32(p + 0x08);
    out.cached_cell_index = u16(p + 0x0C);
    out.npc_type_id = u16(p + 0x0E);
    out.action_state_countdown = u16(p + 0x10);
    out.current_hp = s16(p + 0x12);
    out.packed_runtime_state = u16(p + 0x14);
    return out;
}

LiveObjectRecord direct_object(const std::uint8_t* p) noexcept {
    LiveObjectRecord out{};
    out.y_position_16_16 = u32(p + 0x00);
    out.x_position_16_16 = u32(p + 0x04);
    out.runtime_flags = u32(p + 0x08);
    out.cached_cell_index = u16(p + 0x0C);
    out.object_type_id = u16(p + 0x0E);
    out.state_word = u16(p + 0x10);
    out.auxiliary_word = u16(p + 0x12);
    out.packed_runtime_state = u16(p + 0x14);
    return out;
}

void require_multiple(
    std::size_t size,
    std::size_t stride,
    const char* label) {
    if (size % stride != 0U) {
        throw std::runtime_error(
            std::string(label) +
            " record bytes do not match stride");
    }
}

}  // namespace

LevelWorldState LevelWorldStateCodec::build(
    const LevelState& level,
    const NpcCatalog& npcs,
    const DiceRoller& roll) {
    LevelWorldState out{};

    if (level.storage == LevelStorage::DirectSave) {
        require_multiple(level.family_a_raw.size(), 0x16U, "family A");
        for (std::size_t at = 0;
             at < level.family_a_raw.size();
             at += 0x16U) {
            out.npcs.push_back(
                direct_npc(level.family_a_raw.data() + at));
        }

        if (level.family_b_raw.size() < 2U) {
            throw std::runtime_error(
                "direct family B lacks count word");
        }
        const auto stored_b =
            static_cast<std::size_t>(
                u16(level.family_b_raw.data()));
        const auto bbytes =
            level.family_b_raw.size() - 2U;
        require_multiple(bbytes, 0x16U, "family B");
        if (stored_b != bbytes / 0x16U) {
            throw std::runtime_error(
                "direct family B count mismatch");
        }
        for (std::size_t at = 2U;
             at < level.family_b_raw.size();
             at += 0x16U) {
            out.placed_objects.push_back(
                direct_object(level.family_b_raw.data() + at));
        }

        require_multiple(level.family_c_raw.size(), 0x16U, "family C");
        for (std::size_t at = 0;
             at < level.family_c_raw.size();
             at += 0x16U) {
            out.loose_items.push_back(
                direct_object(level.family_c_raw.data() + at));
        }
        return out;
    }

    require_multiple(level.family_a_raw.size(), 0x0BU, "family A");
    for (std::size_t at = 0;
         at < level.family_a_raw.size();
         at += 0x0BU) {
        const auto* p =
            level.family_a_raw.data() + at;
        const auto type = u16(p + 0x00);
        const auto x_cell = s16(p + 0x02);
        const auto x_fraction = u16(p + 0x04);
        const auto y_cell = s16(p + 0x06);
        const auto y_fraction = u16(p + 0x08);
        const auto compact_orientation = p[0x0A];

        const auto& def = npcs.at(type);
        LiveNpcRecord live{};
        live.y_position_16_16 =
            fixed(y_cell, y_fraction);
        live.x_position_16_16 =
            fixed(x_cell, x_fraction);
        live.runtime_flags =
            def.initial_runtime_flags;
        live.cached_cell_index =
            cell_index(level.width, x_cell, y_cell);
        live.npc_type_id = type;
        live.action_state_countdown = 0;
        live.current_hp = 0;
        live.packed_runtime_state =
            static_cast<std::uint16_t>(
                (compact_orientation & 7U) << 3U);
        live.packed_runtime_state &=
            0x3E39U;

        const auto high_flags =
            static_cast<std::uint8_t>(
                live.runtime_flags >> 24U);
        if ((high_flags & 0x30U) != 0U) {
            if (!roll) {
                throw std::logic_error(
                    "archive NPC HP initialization requires dice roller");
            }
            live.current_hp =
                static_cast<std::int16_t>(
                    roll(def.hit_dice_count, 8U));
        }
        if (type == 8U) {
            live.current_hp = 0x37;
        }
        out.npcs.push_back(live);
    }

    require_multiple(level.family_b_raw.size(), 0x0BU, "family B");
    for (std::size_t at = 0;
         at < level.family_b_raw.size();
         at += 0x0BU) {
        const auto* p =
            level.family_b_raw.data() + at;
        const auto type = u16(p + 0x00);
        const auto x_cell = s16(p + 0x02);
        const auto x_fraction = u16(p + 0x04);
        const auto y_cell = s16(p + 0x06);
        const auto y_fraction = u16(p + 0x08);
        const auto compact_orientation = p[0x0A];

        LiveObjectRecord live{};
        live.y_position_16_16 =
            fixed(y_cell, y_fraction);
        live.x_position_16_16 =
            fixed(x_cell, x_fraction);
        live.cached_cell_index =
            cell_index(level.width, x_cell, y_cell);
        live.object_type_id = type;
        live.packed_runtime_state =
            static_cast<std::uint16_t>(
                (compact_orientation & 7U) << 3U);
        live.packed_runtime_state &=
            0xFE3FU;
        out.placed_objects.push_back(live);
    }

    require_multiple(level.family_c_raw.size(), 0x0BU, "family C");
    for (std::size_t at = 0;
         at < level.family_c_raw.size();
         at += 0x0BU) {
        const auto* p =
            level.family_c_raw.data() + at;
        auto type = u16(p + 0x00);
        const auto x_cell = s16(p + 0x02);
        const auto x_fraction = u16(p + 0x04);
        const auto y_cell = s16(p + 0x06);
        const auto y_fraction = u16(p + 0x08);

        if (type == 0x54U) type = 0x58U;

        LiveObjectRecord live{};
        live.y_position_16_16 =
            fixed(y_cell, y_fraction);
        live.x_position_16_16 =
            fixed(x_cell, x_fraction);
        live.cached_cell_index =
            cell_index(level.width, x_cell, y_cell);
        live.object_type_id = type;
        live.packed_runtime_state &= 0xFE07U;
        out.loose_items.push_back(live);
    }

    return out;
}

}  // namespace rloft
