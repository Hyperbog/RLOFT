#pragma once

// Read-only, SOURCE-BACKED diagnostic image. Reuses the existing verified
// RES0 EH type-1 raster decoder and original 15-bank COLORS palette.
// The resulting composition is NOT the original DOS title/menu framebuffer.
#include "rloft/game_source.hpp"
#include "rloft/resource_assets.hpp"
#include "rloft_native/frame_contract.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace rloft_native {

struct SourceCursorPreview final {
    IndexedFrame frame;
    int original_res0_entry{};
    int raster_width{};
    int raster_height{};
};

// CODE.2 1A0B0 and CODE.1 52AC both choose RES0[INIT[0] + 0x2F]
// for the default cursor, verified by the inherited faithful demo source.
// Palette bank 12 is the startup title/menu palette, not a new palette.
inline SourceCursorPreview make_source_cursor_preview(
    const rloft::GameSource& source) {
    const auto init = source.read("INIT");
    if (init.size() < 2)
        throw std::runtime_error("original INIT file cannot identify RES0 cursor");
    const auto resource_id = static_cast<int>(init[0]) +
                             (static_cast<int>(init[1]) << 8) + 0x2F;
    const auto original_colors = source.read("COLORS");
    constexpr std::size_t kBankSize = 768;
    constexpr std::size_t kPaletteIndex = 12;
    if (original_colors.size() != 1U + 15U * kBankSize ||
        original_colors[0] != 15U)
        throw std::runtime_error("original COLORS bank header/length mismatch");
    const rloft::ResArchive res0(source.read("RES0"));
    const auto cursor = res0.decode_type1(resource_id);
    if (cursor.count <= 0 || cursor.width <= 0 || cursor.height <= 0 ||
        cursor.width > static_cast<int>(IndexedFrame::width) ||
        cursor.height > static_cast<int>(IndexedFrame::height))
        throw std::runtime_error("original cursor dimensions are unsupported");
    const auto per_frame = static_cast<std::size_t>(cursor.width) *
                           static_cast<std::size_t>(cursor.height);
    if (cursor.pixels.size() < per_frame)
        throw std::runtime_error("original RES0 cursor pixel body is short");

    SourceCursorPreview output;
    output.original_res0_entry = resource_id;
    output.raster_width = cursor.width;
    output.raster_height = cursor.height;
    output.frame.pixels.assign(IndexedFrame::width * IndexedFrame::height, 0U);
    std::copy_n(original_colors.begin() + 1U + kPaletteIndex * kBankSize,
                kBankSize, output.frame.dac6.begin());
    // A centered cursor on palette index 0 is a diagnostic composition only,
    // not the original mouse position, title scene, or gameplay viewport.
    const unsigned x_start = (IndexedFrame::width -
                               static_cast<unsigned>(cursor.width)) / 2;
    const unsigned y_start = (IndexedFrame::height -
                               static_cast<unsigned>(cursor.height)) / 2;
    for (int y = 0; y < cursor.height; ++y) {
        for (int x = 0; x < cursor.width; ++x) {
            const auto index = static_cast<std::size_t>(y) *
                                   static_cast<std::size_t>(cursor.width) +
                               static_cast<std::size_t>(x);
            const auto color = cursor.pixels[index];
            if (color != cursor.transparent_index) {
                output.frame.pixels[
                    (y_start + static_cast<unsigned>(y)) * IndexedFrame::width +
                    x_start + static_cast<unsigned>(x)] = color;
            }
        }
    }
    output.frame.validate();
    return output;
}

} // namespace rloft_native
