#pragma once
#include "rloft_native/native_party_start.hpp"
#include "rloft_native/native_world_runtime.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace rloft_native {

// NativeEngine host-only Level22 session checkpoint. Not DOS SAVE.DAT,
// LEVnnn.DAT, a modified original CHARS, or a recovered quest/actor VM.
// Keep original source game files untouched.
struct NativeSessionProfile {
    NativeCharsImage chars{};
    std::string original_world_sha256;
    unsigned level{22};
    double column{4.5};
    double row{4.5};
    double heading_radians{};
    double pitch_radians{};
    std::vector<std::uint8_t> discovered_cells;
};

[[nodiscard]] std::vector<std::uint8_t> encode_native_session_profile(
    const NativeSessionProfile& profile,const NativeWorldRuntime& world);

// Checks file integrity, exact immutable original-world identity,
// source CHARS validity, map bounds, ordinary-cell collision and every
// disclosed bit before returning the state. No changes to world on error.
[[nodiscard]] NativeSessionProfile decode_native_session_profile(
    const std::vector<std::uint8_t>& encoded,
    const NativeWorldRuntime& world);

} // namespace rloft_native
