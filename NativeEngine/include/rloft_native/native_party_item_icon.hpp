#pragma once

#include "rloft_native/native_assets.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace rloft_native {

// Immutable original RES1 object UI art. This selects resource frame 0 as
// an explicitly PROVISIONAL native inventory preview; original CODE.2
// animation/callsite palette switches have not been reconstructed.
struct NativePartyItemIcon {
    unsigned original_res1_id{};
    std::uint16_t width{},height{};
    std::uint16_t original_frame_count{};
    std::uint8_t original_transparent_index{};
    std::string compressed_source_sha256;
    std::vector<std::uint8_t> rgba; // row-major, top-down GL_RGBA
    bool source_is_intentionally_empty{};
    bool original_ui_frame_and_palette_parity_verified{false};
};

// Caller supplies the source-aware active 6-bit palette, e.g. the
// independently sourced Level22 world bank 4. Empty archive records
// remain absent; they must not be substituted with a fabricated icon.
[[nodiscard]] NativePartyItemIcon decode_native_party_item_icon(
    const NativeResArchive& source_res1,
    unsigned absolute_res1_id,
    const NativePalette6& scene_palette);

// Traverses exactly the native object UI namespace and decodes every
// nonempty original EH type1 source through the same gameplay path.
struct NativePartyItemIconCorpus {
    std::size_t examined{};
    std::size_t nonempty{};
    std::size_t zero_length{};
    std::size_t decoded_pixels{};
    bool original_ui_frame_palette_parity_verified{false};
};
[[nodiscard]] NativePartyItemIconCorpus audit_native_party_item_icon_corpus(
    const NativeResArchive& source_res1,
    const NativePalette6& scene_palette);

} // namespace rloft_native
