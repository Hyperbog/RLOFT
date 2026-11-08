#include "rloft/automap_annotation_presenter.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace rloft {
namespace {

std::size_t glyph_index(std::uint8_t value) {
    if (value >= 0x80U) {
        throw std::runtime_error(
            "automap annotation contains non-font byte");
    }
    return static_cast<std::size_t>(value);
}

}  // namespace

int AutomapAnnotationPresenter::bounded_extent(
    const AutomapAnnotation& annotation,
    const Type2Font& font) {
    int total = 0;
    for (const auto value : annotation.payload) {
        total += font.widths[glyph_index(value)];
    }
    return total;
}

bool AutomapAnnotationPresenter::visible(
    const AutomapAnnotation& annotation,
    const Type2Font& font,
    int viewport_column,
    int viewport_row) {
    const int row0 = viewport_column * 8;
    const int column0 = viewport_row * 5;

    // The raster may enter the viewport from ABOVE or LEFT; testing the
    // text origin alone drops labels that should be partially visible.
    // Likewise a label whose end leaves the right edge must not vanish.
    // Match the real tile/marker bounds, rather than the old 0x189/0xEF
    // annotation-only rectangle which painted over the chrome.
    const int row_min = row0 + kClipRowMin;
    const int row_max = row0 + kClipRowMax;
    const int column_min = column0 + kClipColumnMin;
    const int column_max = column0 + kClipColumnMax;
    int vertical_span = font.raster.height;
    for (const auto value : annotation.payload) {
        if (value == 0x0DU) vertical_span += 0x0C;
    }
    const int rx = static_cast<int>(annotation.raw_x);
    const int ry = static_cast<int>(annotation.raw_y);
    const int text_end = ry + bounded_extent(annotation, font);
    return rx <= row_max && rx + vertical_span > row_min &&
           ry <= column_max && text_end > column_min;
}

void AutomapAnnotationPresenter::draw_glyph_clipped(
    IndexedFramebuffer& framebuffer,
    const Type2Font& font,
    std::uint8_t glyph,
    int row,
    int column) {
    const auto index = glyph_index(glyph);
    const auto& raster = font.raster;
    if (static_cast<int>(index) >= raster.count ||
        raster.width <= 0 ||
        raster.height <= 0) {
        throw std::runtime_error(
            "automap annotation glyph exceeds font raster");
    }

    const auto stride =
        static_cast<std::size_t>(raster.width) *
        static_cast<std::size_t>(raster.height);
    const auto frame_at = index * stride;
    if (frame_at > raster.pixels.size() ||
        stride > raster.pixels.size() - frame_at) {
        throw std::runtime_error(
            "automap annotation font raster is truncated");
    }

    auto& dst = framebuffer.mutable_pixels();
    for (int sy = 0; sy < raster.height; ++sy) {
        const int dy = row + sy;
        if (dy < kClipRowMin || dy > kClipRowMax ||
            dy < 0 || dy >= IndexedFramebuffer::kHeight) {
            continue;
        }
        for (int sx = 0; sx < raster.width; ++sx) {
            const int dx = column + sx;
            if (dx < kClipColumnMin || dx > kClipColumnMax ||
                dx < 0 || dx >= IndexedFramebuffer::kWidth) {
                continue;
            }

            const auto value = raster.pixels[
                frame_at +
                static_cast<std::size_t>(sy) *
                    static_cast<std::size_t>(raster.width) +
                static_cast<std::size_t>(sx)];
            if (raster.transparent_index != 0xFFU &&
                value == raster.transparent_index) {
                continue;
            }

            dst[
                static_cast<std::size_t>(dy) *
                    IndexedFramebuffer::kWidth +
                static_cast<std::size_t>(dx)] = value;
        }
    }
}

void AutomapAnnotationPresenter::draw_with_font(
    IndexedFramebuffer& framebuffer,
    const std::vector<AutomapAnnotation>& annotations,
    int viewport_column,
    int viewport_row,
    const Type2Font& font) {
    const int row0 = viewport_column * 8;
    const int column0 = viewport_row * 5;

    for (const auto& annotation : annotations) {
        if (!visible(
                annotation, font,
                viewport_column, viewport_row)) {
            continue;
        }

        int draw_row =
            static_cast<int>(annotation.raw_x) - row0;
        int draw_column =
            static_cast<int>(annotation.raw_y) - column0;
        const int reset_column = draw_column;

        for (const auto value : annotation.payload) {
            if (value == 0x0DU) {
                draw_row += 0x0C;
                draw_column = reset_column;
                continue;
            }

            draw_glyph_clipped(
                framebuffer, font,
                value, draw_row, draw_column);
            draw_column +=
                font.widths[glyph_index(value)];
        }
    }
}

void AutomapAnnotationPresenter::draw(
    IndexedFramebuffer& framebuffer,
    const std::vector<AutomapAnnotation>& annotations,
    int viewport_column,
    int viewport_row) const {
    draw_with_font(
        framebuffer, annotations,
        viewport_column, viewport_row,
        font_);
}

}  // namespace rloft
