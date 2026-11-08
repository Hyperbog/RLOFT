#include "rloft/exploration_hud_presenter.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

int main() {
    {
        const auto layout =
            rloft::ExplorationHudPresenter::base_layout(
                15, 0);

        assert(layout.upper_resource_id == 16);
        assert(layout.lower_resource_id == 17);
        assert(layout.upper_y == 0);
        assert(layout.lower_y == 200);
        assert(layout.x == 0);
    }

    {
        const auto layout =
            rloft::ExplorationHudPresenter::base_layout(
                15, 1);

        assert(layout.upper_resource_id == 19);
        assert(layout.lower_resource_id == 20);
    }

    {
        constexpr std::size_t base =
            0x75F24U + 0xCF70U;
        std::vector<std::uint8_t> code2(
            base + 6U * 256U, 0);

        for (std::size_t state = 0;
             state < 6;
             ++state) {
            for (std::size_t i = 0;
                 i < 256;
                 ++i) {
                code2[
                    base +
                    state * 256U +
                    i] =
                    static_cast<std::uint8_t>(
                        i + state);
            }
        }

        const auto maps =
            rloft::ExplorationHudPresenter::
                portrait_maps_from_code2(
                    code2);
        assert(maps[0][0] == 0);
        assert(maps[2][100] == 102);
        assert(maps[5][255] == 4);
    }

    {
        rloft::IndexedFramebuffer framebuffer;
        framebuffer.clear(0);
        rloft::ExplorationHudPresenter::draw_outline(
            framebuffer,
            10, 20, 5, 4, 7);

        const auto& pixels =
            framebuffer.pixels();
        auto at = [&](int row, int col) {
            return pixels[
                static_cast<std::size_t>(row) *
                    320U +
                static_cast<std::size_t>(col)];
        };

        assert(at(10, 20) == 7);
        assert(at(10, 23) == 7);
        assert(at(14, 20) == 7);
        assert(at(14, 23) == 7);
        assert(at(12, 20) == 7);
        assert(at(12, 23) == 7);
        assert(at(12, 21) == 0);
        assert(at(12, 22) == 0);
    }

    bool rejected = false;
    try {
        (void)rloft::ExplorationHudPresenter::base_layout(
            15, -1);
    } catch (const std::out_of_range&) {
        rejected = true;
    }
    assert(rejected);

    rejected = false;
    try {
        std::vector<std::uint8_t> short_code2(10);
        (void)rloft::ExplorationHudPresenter::
            portrait_maps_from_code2(
                short_code2);
    } catch (const std::runtime_error&) {
        rejected = true;
    }
    assert(rejected);
}
