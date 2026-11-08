#include "rloft/NPCS.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

namespace rloft {
namespace {
constexpr std::size_t kNameBytes = 20U;
constexpr std::size_t kHeaderBytes = 4U;
constexpr std::size_t kRecordBytes = NpcDefinition::kSerializedSize;

std::uint16_t read16(const std::uint8_t* p) noexcept {
    return static_cast<std::uint16_t>(
        static_cast<unsigned>(p[0]) | (static_cast<unsigned>(p[1]) << 8U));
}
std::uint32_t read32(const std::uint8_t* p) noexcept {
    return static_cast<std::uint32_t>(p[0]) |
           (static_cast<std::uint32_t>(p[1]) << 8U) |
           (static_cast<std::uint32_t>(p[2]) << 16U) |
           (static_cast<std::uint32_t>(p[3]) << 24U);
}
std::int8_t signed8(std::uint8_t x) noexcept {
    const int value = x < 128 ? static_cast<int>(x) : static_cast<int>(x) - 256;
    return static_cast<std::int8_t>(value);
}
void write16(std::uint8_t* p, std::uint16_t value) noexcept {
    p[0] = static_cast<std::uint8_t>(value);
    p[1] = static_cast<std::uint8_t>(value >> 8U);
}
void write32(std::uint8_t* p, std::uint32_t value) noexcept {
    for (unsigned i = 0; i < 4U; ++i) {
        p[i] = static_cast<std::uint8_t>(value >> (8U * i));
    }
}
} // namespace

std::vector<NpcDefinition> decode_npcs_dat(
    const std::vector<std::uint8_t>& bytes) {
    if (bytes.size() < kHeaderBytes) {
        throw std::runtime_error("NPCS.DAT has no 4-byte count");
    }
    const auto count = static_cast<std::size_t>(read32(bytes.data()));
    if ((bytes.size() - kHeaderBytes) % kRecordBytes != 0U ||
        count != (bytes.size() - kHeaderBytes) / kRecordBytes) {
        throw std::runtime_error("NPCS.DAT size disagrees with record count");
    }
    std::vector<NpcDefinition> result;
    result.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
        const auto* r = bytes.data() + kHeaderBytes + i * kRecordBytes;
        NpcDefinition n{};
        std::copy_n(r, kNameBytes, n.original_name_bytes.begin());
        const auto end = std::find(r, r + kNameBytes, std::uint8_t{0});
        if (end == r + kNameBytes) {
            throw std::runtime_error("NPCS.DAT name lacks 20-byte NUL terminator");
        }
        n.name.assign(reinterpret_cast<const char*>(r),
                      static_cast<std::size_t>(end - r));
        n.local_shape_base = read16(r + 0x14U);
        n.collision_half_extent = read16(r + 0x16U);
        n.scene_frame_stride = r[0x18U];
        n.animation_cycle_divisor = r[0x19U];
        n.animation_span = r[0x1AU];
        n.scene_influence_step = r[0x1BU];
        n.damage_reaction_animation_flag = r[0x1CU];
        n.hit_dice_count = r[0x1DU];
        n.melee_damage_dice = r[0x1EU];
        n.melee_damage_sides = r[0x1FU];
        n.melee_damage_bonus = r[0x20U];
        n.armor_class = signed8(r[0x21U]);
        n.thac0 = r[0x22U];
        n.experience_award = read16(r + 0x23U);
        n.action_delay = read16(r + 0x25U);
        n.render_scale_lookup_bias = read16(r + 0x27U);
        for (std::size_t j = 0; j < n.directional_offset.size(); ++j) {
            n.directional_offset[j] = read16(r + 0x29U + j * 2U);
        }
        n.behavior_flags = read32(r + 0x31U);
        n.initial_runtime_flags = read32(r + 0x35U);
        n.behavior_flags_39 = r[0x39U];
        n.behavior_flags_3a = r[0x3AU];
        n.unresolved_3b = r[0x3BU];
        n.unresolved_3c = r[0x3CU];
        n.ambient_sound_id = read16(r + 0x3DU);
        n.attack_sound_id = read16(r + 0x3FU);
        n.damage_sound_id = read16(r + 0x41U);
        n.death_sound_id = read16(r + 0x43U);
        result.push_back(std::move(n));
    }
    return result;
}

std::vector<std::uint8_t> encode_npcs_dat(
    const std::vector<NpcDefinition>& definitions) {
    constexpr auto max32 = std::numeric_limits<std::uint32_t>::max();
    if (definitions.size() > max32 ||
        definitions.size() > (std::vector<std::uint8_t>().max_size() -
                              kHeaderBytes) / kRecordBytes) {
        throw std::invalid_argument("NPCS.DAT record count too large");
    }
    std::vector<std::uint8_t> out(
        kHeaderBytes + definitions.size() * kRecordBytes, 0);
    write32(out.data(), static_cast<std::uint32_t>(definitions.size()));
    for (std::size_t i = 0; i < definitions.size(); ++i) {
        const auto& n = definitions[i];
        if (n.name.size() >= kNameBytes ||
            n.name.find('\0') != std::string::npos) {
            throw std::invalid_argument("NPCS.DAT name needs an in-field NUL");
        }
        auto* r = out.data() + kHeaderBytes + i * kRecordBytes;
        std::copy(n.original_name_bytes.begin(), n.original_name_bytes.end(), r);
        std::copy(n.name.begin(), n.name.end(), r);
        r[n.name.size()] = 0;
        write16(r + 0x14U, n.local_shape_base);
        write16(r + 0x16U, n.collision_half_extent);
        r[0x18U] = n.scene_frame_stride;
        r[0x19U] = n.animation_cycle_divisor;
        r[0x1AU] = n.animation_span;
        r[0x1BU] = n.scene_influence_step;
        r[0x1CU] = n.damage_reaction_animation_flag;
        r[0x1DU] = n.hit_dice_count;
        r[0x1EU] = n.melee_damage_dice;
        r[0x1FU] = n.melee_damage_sides;
        r[0x20U] = n.melee_damage_bonus;
        r[0x21U] = static_cast<std::uint8_t>(n.armor_class);
        r[0x22U] = n.thac0;
        write16(r + 0x23U, n.experience_award);
        write16(r + 0x25U, n.action_delay);
        write16(r + 0x27U, n.render_scale_lookup_bias);
        for (std::size_t j = 0; j < n.directional_offset.size(); ++j) {
            write16(r + 0x29U + j * 2U, n.directional_offset[j]);
        }
        write32(r + 0x31U, n.behavior_flags);
        write32(r + 0x35U, n.initial_runtime_flags);
        r[0x39U] = n.behavior_flags_39;
        r[0x3AU] = n.behavior_flags_3a;
        r[0x3BU] = n.unresolved_3b;
        r[0x3CU] = n.unresolved_3c;
        write16(r + 0x3DU, n.ambient_sound_id);
        write16(r + 0x3FU, n.attack_sound_id);
        write16(r + 0x41U, n.damage_sound_id);
        write16(r + 0x43U, n.death_sound_id);
    }
    return out;
}

} // namespace rloft
