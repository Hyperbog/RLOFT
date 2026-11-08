#include "rloft/npc_catalog.hpp"

#include <cstddef>
#include <stdexcept>
#include <utility>

namespace rloft {
namespace {

std::uint16_t u16(const std::uint8_t* p) noexcept {
    return static_cast<std::uint16_t>(p[0]) |
           (static_cast<std::uint16_t>(p[1]) << 8U);
}
std::uint32_t u32(const std::uint8_t* p) noexcept {
    return static_cast<std::uint32_t>(p[0]) |
           (static_cast<std::uint32_t>(p[1]) << 8U) |
           (static_cast<std::uint32_t>(p[2]) << 16U) |
           (static_cast<std::uint32_t>(p[3]) << 24U);
}
std::string fixed_name(const std::uint8_t* p) {
    std::size_t n = 0;
    while (n < 20U && p[n] != 0) ++n;
    return std::string(reinterpret_cast<const char*>(p), n);
}

}  // namespace

NpcCatalog NpcCatalog::parse(const std::vector<std::uint8_t>& bytes) {
    if (bytes.size() < 4U) {
        throw std::runtime_error("NPCS.DAT is truncated");
    }
    const auto count = static_cast<std::size_t>(u32(bytes.data()));
    const auto expected = 4U + count * NpcDefinition::kSerializedSize;
    if (bytes.size() != expected) {
        throw std::runtime_error(
            "NPCS.DAT size does not match counted 0x45 records");
    }

    NpcCatalog out{};
    out.npcs_.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
        const auto* rec =
            bytes.data() + 4U + i * NpcDefinition::kSerializedSize;
        NpcDefinition npc{};
        npc.name = fixed_name(rec);
        npc.local_shape_base = u16(rec + 0x14);
        npc.collision_half_extent = u16(rec + 0x16);
        npc.scene_frame_stride = rec[0x18];
        npc.animation_cycle_divisor = rec[0x19];
        npc.animation_span = rec[0x1A];
        npc.scene_influence_step = rec[0x1B];
        npc.damage_reaction_animation_flag = rec[0x1C];
        npc.hit_dice_count = rec[0x1D];
        npc.melee_damage_dice = rec[0x1E];
        npc.melee_damage_sides = rec[0x1F];
        npc.melee_damage_bonus = rec[0x20];
        npc.armor_class = static_cast<std::int8_t>(rec[0x21]);
        npc.thac0 = rec[0x22];
        npc.experience_award = u16(rec + 0x23);
        npc.action_delay = u16(rec + 0x25);
        npc.render_scale_lookup_bias = u16(rec + 0x27);
        for (std::size_t d = 0; d < 4U; ++d) {
            npc.directional_offset[d] = u16(rec + 0x29 + d * 2U);
        }
        npc.behavior_flags = u32(rec + 0x31);
        npc.initial_runtime_flags = u32(rec + 0x35);
        npc.behavior_flags_39 = rec[0x39];
        npc.behavior_flags_3a = rec[0x3A];
        npc.ambient_sound_id = u16(rec + 0x3D);
        npc.attack_sound_id = u16(rec + 0x3F);
        npc.damage_sound_id = u16(rec + 0x41);
        npc.death_sound_id = u16(rec + 0x43);
        out.npcs_.push_back(std::move(npc));
    }
    return out;
}

NpcCatalog NpcCatalog::load(const GameSource& source) {
    return parse(source.read("NPCS.DAT"));
}

const NpcDefinition& NpcCatalog::at(std::size_t npc_id) const {
    if (npc_id >= npcs_.size()) throw std::out_of_range("NPCS.DAT npc id");
    return npcs_[npc_id];
}

}  // namespace rloft
