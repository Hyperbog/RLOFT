#pragma once
#include "rloft_native/native_party_start.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace rloft_native {

// Source-exact CODE.1 0x248 character fields and immutable 65-byte
// OBJECTS.DAT definitions. Inspector only: item use, equipping, pickup,
// and mutable CODE.2 inventory are NOT implemented here.
struct NativeInventorySlot {
    unsigned slot{};
    std::int16_t object_id{-1};
    std::string source_name;
    std::uint8_t source_category{};
    std::uint16_t absolute_res1_ui_id{};
    bool identified{};
    bool occupied{};
};
struct NativePartyActorInventory {
    std::string actor_name;
    std::array<NativeInventorySlot,21> slots{};
    std::uint32_t original_known_mask{};
    std::size_t occupied_count{};
};
struct NativePartyInventory {
    std::array<NativePartyActorInventory,2> actors{};
    std::size_t object_definitions{};
    std::size_t occupied_total{};
    bool live_inventory_emulation_verified{false};
};

// CODE.1-created CHARS is authoritative: 21 little-endian signed item IDs
// at actor+0x40, 32-bit known mask at +0x12B. Original category/name/UI
// base are decoded from 336 counted immutable OBJECTS.DAT records.
// Absent items are -1; do not coerce them to object zero.
[[nodiscard]] NativePartyInventory inspect_native_party_inventory(
    const NativePartyStart& party,
    const std::vector<std::uint8_t>& objects_dat);

[[nodiscard]] const char* native_inventory_slot_label(unsigned slot);

} // namespace rloft_native
