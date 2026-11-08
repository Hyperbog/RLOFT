#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace rloft {

// INIT scene-shape descriptor: seven bytes on disk, little endian.
struct InitSceneDescriptor {
    std::uint16_t local_shape_base{};
    std::uint16_t spatial_extent{};
    std::uint8_t shape_stride{};
    std::uint8_t variant_count{};
    std::uint8_t scene_influence_step{};
};

struct EnvironmentPlacement {
    std::int16_t resource_selector{};
    std::int16_t azimuth_degrees{};
    std::int16_t draw_width_pixels{};
    std::int16_t screen_y{};

    [[nodiscard]] bool terminator() const noexcept {
        return resource_selector == 0 && azimuth_degrees == 0 &&
               draw_width_pixels == 0 && screen_y == 0;
    }
};

struct EnvironmentProfile {
    std::uint16_t res8_base{};
    std::uint16_t stored_record_count{};
    std::vector<EnvironmentPlacement> placements;
};

struct InitFile {
    // The first 18 little-endian words are resource-bank bases/boundaries.
    // Preserve the original numeric values; see Resources/MISC/INIT/FORMAT.md.
    std::array<std::uint16_t, 18> header_words{};
    std::vector<InitSceneDescriptor> class1_descriptors;
    std::vector<InitSceneDescriptor> class3_descriptors;
    std::vector<EnvironmentProfile> environment_profiles;
};

// Lossless representation of the entire original INIT grammar; no hardcoded
// descriptor counts or offsets. Throws std::runtime_error on malformed input.
[[nodiscard]] InitFile parse_init(const std::vector<std::uint8_t>& bytes);
[[nodiscard]] std::vector<std::uint8_t> encode_init(const InitFile& file);

// Historical renderer compatibility: parse the environment section at a known
// byte offset. Existing renderer inputs use 0x0545 for the shipped INIT build;
// parse_init instead derives its offset from serialized descriptor counts.
[[nodiscard]] std::vector<EnvironmentProfile> decode_init_environment_profiles_at(
    const std::vector<std::uint8_t>& bytes, std::size_t offset);

} // namespace rloft
