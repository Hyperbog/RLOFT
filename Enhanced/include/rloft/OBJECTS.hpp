#pragma once

#include "rloft/object_catalog.hpp"

#include <cstdint>
#include <vector>

namespace rloft {

// Authoritative OBJECTS.DAT codec: 4-byte LE count, then count x 0x41 records.
// Uses the existing game's ObjectDefinition; never reinterprets C++ padding.
[[nodiscard]] std::vector<ObjectDefinition> decode_objects_dat(
    const std::vector<std::uint8_t>& bytes);
[[nodiscard]] std::vector<std::uint8_t> encode_objects_dat(
    const std::vector<ObjectDefinition>& objects);

} // namespace rloft
