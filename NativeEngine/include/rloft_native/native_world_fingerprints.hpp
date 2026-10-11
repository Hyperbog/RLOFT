#pragma once
#include "rloft_native/native_world_payload.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace rloft_native {
// Re-encodes the retained typed world payload, verifies every source section
// byte-for-byte against read-only original input, and emits hashes only.
// No gameplay / DOS code is executed and no original bytes are emitted.
[[nodiscard]] std::string native_world_payload_fingerprints_json(
    const NativeWorldPayload& decoded,
    const std::vector<std::uint8_t>& original_levels_dat);
}
