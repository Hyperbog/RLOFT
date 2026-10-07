#pragma once

#include <array>
#include <cstdint>
#include <vector>

#include "rloft/resource_assets.hpp"

namespace rloft {

struct IndexedRegion {
    int height{};
    int width{};
    std::vector<std::uint8_t> pixels;
};

class IndexedFramebuffer final {
public:
    static constexpr int kWidth = 320;
    static constexpr int kHeight = 400;

    IndexedFramebuffer();

    void clear(std::uint8_t color = 0);
    void draw_frame(const Type1Raster& raster, int frame, int y, int x);
    void draw_frame_mirrored(const Type1Raster& raster, int frame, int y, int x);
    void draw_frame_mapped(
        const Type1Raster& raster,
        int frame,
        int y,
        int x,
        const std::array<std::uint8_t, 256>& map);
    void draw(const Type1Raster& raster, int y, int x);

    [[nodiscard]] IndexedRegion capture(int y, int x, int height, int width) const;
    void restore(const IndexedRegion& region, int y, int x);
    void restore_mapped(
        const IndexedRegion& region,
        int y,
        int x,
        const std::array<std::uint8_t, 256>& map);
    void fill_rect(int y, int x, int height, int width, std::uint8_t color);

    [[nodiscard]] const std::vector<std::uint8_t>& pixels() const noexcept { return pixels_; }
    [[nodiscard]] std::vector<std::uint8_t>& mutable_pixels() noexcept { return pixels_; }

private:
    std::vector<std::uint8_t> pixels_;
};

class StartupPresenter final {
public:
    static constexpr double kPitHz = 1193182.0 / 65536.0;

    explicit StartupPresenter(const RavenloftBootAssets& assets);

    [[nodiscard]] IndexedFramebuffer& framebuffer() noexcept { return framebuffer_; }
    [[nodiscard]] const IndexedFramebuffer& framebuffer() const noexcept { return framebuffer_; }

    [[nodiscard]] Palette6 palette14() const;
    [[nodiscard]] Palette6 palette9() const;
    [[nodiscard]] Palette6 palette12() const;
    [[nodiscard]] static Palette6 fade_factor(const Palette6& source, int factor);

    void first_logo();
    void second_logo();
    void third_logo();
    void title_background();
    void menu_animation_frame(int step);
    void menu_frame(int selection = -1);

    [[nodiscard]] static int menu_selection_from_y(int y) noexcept;

private:
    void draw_resource(int resource_id, int y, int x);

    const RavenloftBootAssets& assets_;
    IndexedFramebuffer framebuffer_;
};

}  // namespace rloft
