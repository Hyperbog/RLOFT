#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "rloft/startup_presenter.hpp"

namespace rloft {

struct SurfaceFillTable final {
    // CODE.2 33DE0/33E28 index FE94+EBX with EBX=128..1.
    std::array<std::uint8_t, 129> subtract{};

    [[nodiscard]] static SurfaceFillTable from_code2(
        const std::vector<std::uint8_t>& code2);
};

class SurfaceFillModel final {
public:
    static constexpr int kColumn = 0x20;
    static constexpr int kWidth = 0x100;
    static constexpr int kRows = 0x80;
    static constexpr int kUpperRow = 4;
    static constexpr int kLowerRow = 0x103;

    // Portable chunky equivalents of CODE.2 33E28 and 33DE0.
    static void draw_upper(
        IndexedFramebuffer& framebuffer,
        const SurfaceFillTable& table,
        std::uint8_t base_color);

    static void draw_lower(
        IndexedFramebuffer& framebuffer,
        const SurfaceFillTable& table,
        std::uint8_t base_color);

private:
    static void draw(
        IndexedFramebuffer& framebuffer,
        const SurfaceFillTable& table,
        int first_row,
        int row_step,
        std::uint8_t base_color);
};

}  // namespace rloft
