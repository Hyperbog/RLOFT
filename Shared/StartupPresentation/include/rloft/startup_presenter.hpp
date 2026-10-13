#pragma once

#include <array>
#include <cstdint>
#include <vector>

#include "rloft/startup_visual_source.hpp"

namespace rloft {

struct IndexedRegion {
    int height{};
    int width{};
    std::vector<std::uint8_t> pixels;
};

// Native forensic metadata only. Does not change indexed pixels or
// resource lookup; bounds/frames correspond to original VGA units.
struct IndexedDrawAuditCall final {
    const char* operation{};
    int frame{};
    int y{};
    int x{};
    int height{};
    int width{};
    int parameter_a{};
    int parameter_b{};
};

class IndexedFramebuffer final {
public:
    static constexpr int kWidth = 320;
    static constexpr int kHeight = 400;

    IndexedFramebuffer();

    using DrawAuditCallback = void (*)(void*,const IndexedDrawAuditCall&);
    void set_draw_audit(
        DrawAuditCallback callback,void* context) noexcept {
        audit_callback_=callback;
        audit_context_=context;
    }

    void clear(std::uint8_t color = 0);
    void draw_frame(const Type1Raster& raster, int frame, int y, int x);
    void draw_frame_mirrored(const Type1Raster& raster, int frame, int y, int x);
    void draw_frame_mapped(
        const Type1Raster& raster,
        int frame,
        int y,
        int x,
        const std::array<std::uint8_t, 256>& map);
    void draw_frame_scaled_horizontal(
        const Type1Raster& raster,
        int frame,
        int y,
        int x,
        int numerator,
        int denominator,
        const std::array<std::uint8_t, 256>* map = nullptr,
        bool map_transparent = false);
    void draw(const Type1Raster& raster, int y, int x);

    [[nodiscard]] IndexedRegion capture(int y, int x, int height, int width) const;
    void restore(const IndexedRegion& region, int y, int x);
    void restore_mapped(
        const IndexedRegion& region,
        int y,
        int x,
        const std::array<std::uint8_t, 256>& map);
    void restore_scaled_horizontal(
        const IndexedRegion& region,
        int y,
        int x,
        int numerator,
        int denominator,
        const std::array<std::uint8_t, 256>* map = nullptr);
    void fill_rect(int y, int x, int height, int width, std::uint8_t color);

    [[nodiscard]] const std::vector<std::uint8_t>& pixels() const noexcept { return pixels_; }
    [[nodiscard]] std::vector<std::uint8_t>& mutable_pixels() noexcept { return pixels_; }

private:
    void audit(const IndexedDrawAuditCall& call) const {
        if(audit_callback_)audit_callback_(audit_context_,call);
    }
    std::vector<std::uint8_t> pixels_;
    DrawAuditCallback audit_callback_{};
    void* audit_context_{};
};

class StartupPresenter final {
public:
    static constexpr double kPitHz = 1193182.0 / 65536.0;

    explicit StartupPresenter(const StartupVisualSource& assets);

    [[nodiscard]] IndexedFramebuffer& framebuffer() noexcept { return framebuffer_; }
    [[nodiscard]] const IndexedFramebuffer& framebuffer() const noexcept { return framebuffer_; }

    [[nodiscard]] Palette6 palette14() const;
    [[nodiscard]] Palette6 palette9() const;
    [[nodiscard]] Palette6 palette12() const;
    [[nodiscard]] static Palette6 fade_factor(const Palette6& source, int factor);
    [[nodiscard]] static Type1Raster code2_menu_cursor(
        const Type1Raster& source);

    void first_logo();
    void second_logo();
    void third_logo();
    void title_background();
    void menu_animation_frame(int step);
    void menu_frame(int selection = -1);

    [[nodiscard]] static int menu_selection_from_y(int y) noexcept;

private:
    void draw_resource(int resource_id, int y, int x);

    const StartupVisualSource& assets_;
    IndexedFramebuffer framebuffer_;
};

}  // namespace rloft
