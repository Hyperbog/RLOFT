#include "rloft/startup_presenter.hpp"

#include <algorithm>
#include <array>
#include <cstdlib>
#include <stdexcept>

namespace rloft {

IndexedFramebuffer::IndexedFramebuffer()
    : pixels_(static_cast<std::size_t>(kWidth) * kHeight, 0) {}

void IndexedFramebuffer::clear(std::uint8_t color) {
    std::fill(pixels_.begin(), pixels_.end(), color);
}

void IndexedFramebuffer::draw_frame(
    const Type1Raster& raster, int frame, int y, int x) {
    if (frame < 0 || frame >= raster.count) {
        throw std::out_of_range("type-1 frame index");
    }

    const auto frame_stride =
        static_cast<std::size_t>(raster.width) * raster.height;
    const auto frame_at = static_cast<std::size_t>(frame) * frame_stride;
    for (int sy = 0; sy < raster.height; ++sy) {
        const int dy = y + sy;
        if (dy < 0 || dy >= kHeight) continue;
        for (int sx = 0; sx < raster.width; ++sx) {
            const int dx = x + sx;
            if (dx < 0 || dx >= kWidth) continue;
            const auto value = raster.pixels[
                frame_at + static_cast<std::size_t>(sy) * raster.width + sx];
            if (raster.transparent_index != 0xFF &&
                value == raster.transparent_index) {
                continue;
            }
            pixels_[static_cast<std::size_t>(dy) * kWidth + dx] = value;
        }
    }
}

void IndexedFramebuffer::draw_frame_mirrored(
    const Type1Raster& raster, int frame, int y, int x) {
    if (frame < 0 || frame >= raster.count) {
        throw std::out_of_range("type-1 frame index");
    }

    const auto frame_stride =
        static_cast<std::size_t>(raster.width) * raster.height;
    const auto frame_at = static_cast<std::size_t>(frame) * frame_stride;
    for (int sy = 0; sy < raster.height; ++sy) {
        const int dy = y + sy;
        if (dy < 0 || dy >= kHeight) continue;
        for (int sx = 0; sx < raster.width; ++sx) {
            const int dx = x + sx;
            if (dx < 0 || dx >= kWidth) continue;
            const int source_x = raster.width - 1 - sx;
            const auto value = raster.pixels[
                frame_at + static_cast<std::size_t>(sy) * raster.width +
                source_x];
            if (raster.transparent_index != 0xFF &&
                value == raster.transparent_index) {
                continue;
            }
            pixels_[static_cast<std::size_t>(dy) * kWidth + dx] = value;
        }
    }
}

void IndexedFramebuffer::draw_frame_mapped(
    const Type1Raster& raster,
    int frame,
    int y,
    int x,
    const std::array<std::uint8_t, 256>& map) {
    if (frame < 0 || frame >= raster.count) {
        throw std::out_of_range("type-1 mapped frame index");
    }

    const auto frame_stride =
        static_cast<std::size_t>(raster.width) * raster.height;
    const auto frame_at = static_cast<std::size_t>(frame) * frame_stride;
    for (int sy = 0; sy < raster.height; ++sy) {
        const int dy = y + sy;
        if (dy < 0 || dy >= kHeight) continue;
        for (int sx = 0; sx < raster.width; ++sx) {
            const int dx = x + sx;
            if (dx < 0 || dx >= kWidth) continue;

            const auto value = raster.pixels[
                frame_at +
                static_cast<std::size_t>(sy) * raster.width +
                sx];
            if (raster.transparent_index != 0xFF &&
                value == raster.transparent_index) {
                continue;
            }

            pixels_[static_cast<std::size_t>(dy) * kWidth + dx] =
                map[value];
        }
    }
}

IndexedRegion IndexedFramebuffer::capture(
    int y, int x, int height, int width) const {
    if (height < 0 || width < 0) {
        throw std::invalid_argument("negative indexed capture extent");
    }

    IndexedRegion region;
    region.height = height;
    region.width = width;
    region.pixels.assign(
        static_cast<std::size_t>(height) * static_cast<std::size_t>(width), 0);

    for (int row = 0; row < height; ++row) {
        const int sy = y + row;
        if (sy < 0 || sy >= kHeight) continue;
        for (int col = 0; col < width; ++col) {
            const int sx = x + col;
            if (sx < 0 || sx >= kWidth) continue;
            region.pixels[
                static_cast<std::size_t>(row) * width + col] =
                pixels_[static_cast<std::size_t>(sy) * kWidth + sx];
        }
    }
    return region;
}

void IndexedFramebuffer::restore(
    const IndexedRegion& region, int y, int x) {
    if (region.height < 0 || region.width < 0 ||
        region.pixels.size() !=
            static_cast<std::size_t>(region.height) *
                static_cast<std::size_t>(region.width)) {
        throw std::invalid_argument("invalid indexed region");
    }

    for (int row = 0; row < region.height; ++row) {
        const int dy = y + row;
        if (dy < 0 || dy >= kHeight) continue;
        for (int col = 0; col < region.width; ++col) {
            const int dx = x + col;
            if (dx < 0 || dx >= kWidth) continue;
            pixels_[static_cast<std::size_t>(dy) * kWidth + dx] =
                region.pixels[
                    static_cast<std::size_t>(row) * region.width + col];
        }
    }
}

void IndexedFramebuffer::fill_rect(
    int y, int x, int height, int width, std::uint8_t color) {
    if (height <= 0 || width <= 0) return;
    for (int row = 0; row < height; ++row) {
        const int dy = y + row;
        if (dy < 0 || dy >= kHeight) continue;
        for (int col = 0; col < width; ++col) {
            const int dx = x + col;
            if (dx < 0 || dx >= kWidth) continue;
            pixels_[static_cast<std::size_t>(dy) * kWidth + dx] = color;
        }
    }
}

void IndexedFramebuffer::draw(const Type1Raster& raster, int y, int x) {
    for (int frame = 0; frame < raster.count; ++frame) {
        draw_frame(raster, frame, y + frame * raster.height, x);
    }
}

StartupPresenter::StartupPresenter(const RavenloftBootAssets& assets)
    : assets_(assets) {}

Palette6 StartupPresenter::palette14() const { return assets_.colors_palette(14); }
Palette6 StartupPresenter::palette9() const { return assets_.colors_palette(9); }
Palette6 StartupPresenter::palette12() const { return assets_.colors_palette(12); }

Palette6 StartupPresenter::fade_factor(const Palette6& source, int factor) {
    factor = std::clamp(factor, 0, 8);
    Palette6 out;
    for (std::size_t i = 0; i < source.rgb.size(); ++i) {
        out.rgb[i] = static_cast<std::uint8_t>(
            (static_cast<unsigned>(source.rgb[i]) * factor) >> 3U);
    }
    return out;
}

void StartupPresenter::draw_resource(int resource_id, int y, int x) {
    framebuffer_.draw(assets_.res0_type1(resource_id), y, x);
}

void StartupPresenter::first_logo() {
    framebuffer_.clear(0);
    // CODE.2 19A1A..19A30:
    // RES0 base 15 + 0x4F = RES0:94, coordinates y=0x8F, x=0x28.
    draw_resource(94, 143, 40);
}

void StartupPresenter::second_logo() {
    // The original fades the palette to black but does not clear the page.
    draw_resource(95, 138, 36);
}

void StartupPresenter::third_logo() {
    draw_resource(96, 0, 0);
}

void StartupPresenter::title_background() {
    draw_resource(97, 0, 0);
}

void StartupPresenter::menu_animation_frame(int step) {
    if (step < 1 || step > 14) {
        throw std::out_of_range("menu animation step must be 1..14");
    }

    // Exact CODE.2 data at DGROUP 0x8210 / 0x8230 / 0x8250,
    // consumed by routine 0x191C0.
    static constexpr std::array<int, 4> start_y{-70, 79, 400, 341};
    static constexpr std::array<int, 4> start_x{85, -240, 0, 320};
    static constexpr std::array<int, 4> target_y{7, 79, 200, 341};
    static constexpr std::array<int, 4> target_x{85, 0, 0, 143};
    static constexpr std::array<int, 4> dir_y{1, 0, -1, 0};
    static constexpr std::array<int, 4> dir_x{0, 1, 0, -1};

    framebuffer_.clear(0);
    draw_resource(97, 0, 0);

    for (int i = 0; i < 4; ++i) {
        const int dy = std::abs(start_y[i] - target_y[i]);
        const int dx = std::abs(start_x[i] - target_x[i]);
        const int y = start_y[i] + dir_y[i] * ((dy * step + 7) / 14);
        const int x = start_x[i] + dir_x[i] * ((dx * step + 7) / 14);
        draw_resource(98 + i, y, x);
    }
}

void StartupPresenter::menu_frame(int selection) {
    framebuffer_.clear(0);

    // CODE.2 0x192CC static menu composition.
    draw_resource(97, 0, 0);
    draw_resource(98, 7, 85);
    draw_resource(99, 79, 0);
    draw_resource(100, 200, 0);
    draw_resource(101, 341, 143);

    if (selection >= 0 && selection < 4) {
        // CODE.2 DGROUP 0x8290 table: y, x, vertical selection ceiling.
        static constexpr std::array<int, 4> y{270, 287, 302, 319};
        static constexpr std::array<int, 4> x{33, 47, 39, 64};
        draw_resource(102 + selection, y[selection], x[selection]);
    }
}

int StartupPresenter::menu_selection_from_y(int y) noexcept {
    static constexpr std::array<int, 4> max_y{285, 300, 317, 399};
    if (y < 0) return -1;
    for (int i = 0; i < 4; ++i) {
        if (y <= max_y[i]) return i;
    }
    return -1;
}

}  // namespace rloft
