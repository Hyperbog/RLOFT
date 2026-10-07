#include "rloft/character_creation_presenter.hpp"
#include "rloft/resource_assets.hpp"
#include "rloft/startup_presenter.hpp"

#include <cassert>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {

void put32(std::vector<std::uint8_t>& bytes,
           std::size_t offset,
           std::uint32_t value) {
    for (int i = 0; i < 4; ++i) {
        bytes[offset + i] =
            static_cast<std::uint8_t>(value >> (8 * i));
    }
}

void put16(std::vector<std::uint8_t>& bytes,
           std::size_t offset,
           std::uint16_t value) {
    bytes[offset] = static_cast<std::uint8_t>(value);
    bytes[offset + 1] = static_cast<std::uint8_t>(value >> 8U);
}

}  // namespace

int main() {
    // Synthetic one-entry RES: 1 frame, 8x1, method 0, key index 0.
    std::vector<std::uint8_t> bytes(8 + 13 + 8);
    put32(bytes, 0, 1);
    put32(bytes, 4, 8);

    std::size_t p = 8;
    bytes[p++] = 'E';
    bytes[p++] = 'H';
    bytes[p++] = 1;
    put16(bytes, p, 8);
    p += 2;
    put16(bytes, p, 1);
    p += 2;
    put16(bytes, p, 1);
    p += 2;
    bytes[p++] = 1;
    bytes[p++] = 0;
    bytes[p++] = 0;
    bytes[p++] = 0;
    for (int i = 0; i < 8; ++i) {
        bytes[p++] = static_cast<std::uint8_t>(i);
    }

    rloft::ResArchive archive(std::move(bytes));
    auto raster = archive.decode_type1(0);
    assert(raster.count == 1);
    assert(raster.width == 8);
    assert(raster.height == 1);
    assert(raster.transparent_index == 0);

    // CODE.1 767C -> DC90: a nonzero descriptor byte +7 converts an
    // N-element, one-row type-1 image into one N-row runtime image.
    {
        std::vector<std::uint8_t> transformed_bytes(8 + 13 + 24);
        put32(transformed_bytes, 0, 1);
        put32(transformed_bytes, 4, 8);

        std::size_t q = 8;
        transformed_bytes[q++] = 'E';
        transformed_bytes[q++] = 'H';
        transformed_bytes[q++] = 1;
        put16(transformed_bytes, q, 8);
        q += 2;
        put16(transformed_bytes, q, 3);
        q += 2;
        put16(transformed_bytes, q, 1);
        q += 2;
        transformed_bytes[q++] = 1;
        transformed_bytes[q++] = 0xFF;
        transformed_bytes[q++] = 0;
        transformed_bytes[q++] = 1;
        for (int i = 0; i < 24; ++i) {
            transformed_bytes[q++] =
                static_cast<std::uint8_t>(i + 1);
        }

        rloft::ResArchive transformed_archive(
            std::move(transformed_bytes));
        const auto transformed =
            transformed_archive.decode_type1(0);
        assert(transformed.count == 1);
        assert(transformed.width == 8);
        assert(transformed.height == 3);
        assert(transformed.transparent_index == 0xFF);
        assert(transformed.pixels.size() == 24);
        assert(transformed.pixels.front() == 1);
        assert(transformed.pixels.back() == 24);
    }

    rloft::IndexedFramebuffer framebuffer;
    framebuffer.clear(9);
    framebuffer.draw(raster, 10, 5);
    assert(framebuffer.pixels()[10 * 320 + 5] == 9);
    assert(framebuffer.pixels()[10 * 320 + 6] == 1);

    std::array<std::uint8_t, 256> remap{};
    for (int i = 0; i < 256; ++i) {
        remap[static_cast<std::size_t>(i)] =
            static_cast<std::uint8_t>(255 - i);
    }
    framebuffer.clear(9);
    framebuffer.draw_frame_mapped(raster, 0, 12, 7, remap);
    // Transparent source index 0 remains untouched; source index 1 maps to FE.
    assert(framebuffer.pixels()[12 * 320 + 7] == 9);
    assert(framebuffer.pixels()[12 * 320 + 8] == 0xFE);

    // CODE.1 A5C8 reverses scanline order (vertical), not columns.
    // The character-opening indicator and 10A4 flying-card sequence both
    // use this primitive.
    rloft::Type1Raster flip_test{
        1,
        2,
        2,
        0xFF,
        {1, 2,
         3, 4}
    };
    framebuffer.clear(0);
    framebuffer.draw_frame_mirrored(
        flip_test,
        0,
        20,
        10);
    assert(framebuffer.pixels()[20 * 320 + 10] == 3);
    assert(framebuffer.pixels()[20 * 320 + 11] == 4);
    assert(framebuffer.pixels()[21 * 320 + 10] == 1);
    assert(framebuffer.pixels()[21 * 320 + 11] == 2);

    rloft::Palette6 palette;
    for (auto& value : palette.rgb) value = 56;
    const auto half = rloft::StartupPresenter::fade_factor(palette, 4);
    assert(half.rgb[0] == 28);

    assert(rloft::StartupPresenter::menu_selection_from_y(285) == 0);
    assert(rloft::StartupPresenter::menu_selection_from_y(286) == 1);
    assert(rloft::StartupPresenter::menu_selection_from_y(300) == 1);
    assert(rloft::StartupPresenter::menu_selection_from_y(301) == 2);
    assert(rloft::StartupPresenter::menu_selection_from_y(318) == 3);
    assert(rloft::StartupPresenter::menu_selection_from_y(400) == -1);

    // 07E4's stagger loop contributes N+10 frames, followed by one final
    // full 064C redraw/page flip.
    assert(
        rloft::CharacterCreationPresenter::transition_07e4_frame_count(
            0, 6) == 17);
    assert(
        rloft::CharacterCreationPresenter::transition_07e4_frame_count(
            2, 4) == 13);
    assert(
        rloft::CharacterCreationPresenter::transition_07e4_frame_count(
            6, 9) == 14);

    std::cout << "presenter_tests: PASS\n";
}
