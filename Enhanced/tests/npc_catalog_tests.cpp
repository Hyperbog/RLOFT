#include "rloft/npc_catalog.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace {
void put16(std::vector<std::uint8_t>& b, std::size_t o, std::uint16_t v) {
    b[o] = static_cast<std::uint8_t>(v);
    b[o + 1] = static_cast<std::uint8_t>(v >> 8U);
}
void put32(std::vector<std::uint8_t>& b, std::size_t o, std::uint32_t v) {
    for (int i = 0; i < 4; ++i) {
        b[o + static_cast<std::size_t>(i)] =
            static_cast<std::uint8_t>(v >> (i * 8));
    }
}
}

int main() {
    std::vector<std::uint8_t> bytes(4U + 0x45U, 0);
    put32(bytes, 0, 1);
    const auto r = 4U;
    const char* name = "ASSASSIN";
    for (std::size_t i = 0; name[i]; ++i) {
        bytes[r + i] = static_cast<std::uint8_t>(name[i]);
    }
    put16(bytes, r + 0x14, 123);
    put16(bytes, r + 0x16, 0x8000);
    bytes[r + 0x18] = 4;
    bytes[r + 0x19] = 5;
    bytes[r + 0x1A] = 6;
    bytes[r + 0x1D] = 4;
    bytes[r + 0x1E] = 1;
    bytes[r + 0x1F] = 8;
    bytes[r + 0x20] = 2;
    bytes[r + 0x21] = static_cast<std::uint8_t>(-3);
    bytes[r + 0x22] = 15;
    put16(bytes, r + 0x23, 975);
    put16(bytes, r + 0x25, 7);
    put16(bytes, r + 0x27, 2);
    for (std::size_t i = 0; i < 4; ++i) {
        put16(bytes, r + 0x29 + i * 2U, static_cast<std::uint16_t>(10 + i));
    }
    put32(bytes, r + 0x31, 0x12345678);
    put32(bytes, r + 0x35, 0x10000000);
    bytes[r + 0x39] = 0x21;
    bytes[r + 0x3A] = 0x16;
    put16(bytes, r + 0x3D, 20);
    put16(bytes, r + 0x3F, 21);
    put16(bytes, r + 0x41, 22);
    put16(bytes, r + 0x43, 23);

    const auto cat = rloft::NpcCatalog::parse(bytes);
    const auto& n = cat.at(0);
    assert(cat.size() == 1);
    assert(n.name == "ASSASSIN");
    assert(n.local_shape_base == 123);
    assert(n.collision_half_extent == 0x8000);
    assert(n.hit_dice_count == 4);
    assert(n.melee_damage_dice == 1);
    assert(n.melee_damage_sides == 8);
    assert(n.melee_damage_bonus == 2);
    assert(n.armor_class == -3);
    assert(n.thac0 == 15);
    assert(n.experience_award == 975);
    assert(n.action_delay == 7);
    assert(n.render_scale_lookup_bias == 2);
    assert(n.directional_offset[3] == 13);
    assert(n.behavior_flags == 0x12345678);
    assert(n.initial_runtime_flags == 0x10000000);
    assert(n.behavior_flags_39 == 0x21);
    assert(n.behavior_flags_3a == 0x16);
    assert(n.ambient_sound_id == 20);
    assert(n.attack_sound_id == 21);
    assert(n.damage_sound_id == 22);
    assert(n.death_sound_id == 23);

    bool rejected = false;
    try { (void)cat.at(1); }
    catch (const std::out_of_range&) { rejected = true; }
    assert(rejected);
}
