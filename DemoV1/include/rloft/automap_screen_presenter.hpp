#pragma once

#include <array>
#include <cstdint>
#include <vector>
#include "rloft/indexed_font_presenter.hpp"
#include "rloft/original_random.hpp"
#include "rloft/automap_annotation_presenter.hpp"
#include "rloft/automap_screen_state.hpp"
#include "rloft/automap_viewport_presenter.hpp"
#include "rloft/renderer_tables.hpp"
#include "rloft/resource_assets.hpp"
#include "rloft/startup_presenter.hpp"

namespace rloft {

class AutomapScreenPresenter final {
public:
    explicit AutomapScreenPresenter(
        const RavenloftBootAssets& assets)
        : assets_(assets),
          viewport_(assets),
          annotations_(assets),
          res0_base_(assets.init_u16(0x00)),
          legend_font_(assets.res0_font(1)),
          legend_frames_(legend_frames_from_code2(
              assets.source().read("CODE.2"))) {}

    // Recovered from actual CODE.2 DGROUP:5FCC used in routine
    // 01834. No guessed icon IDs or hardcoded English legend text.
    [[nodiscard]] static std::array<std::uint8_t,13>
    legend_frames_from_code2(const std::vector<std::uint8_t>& code2);
    [[nodiscard]] static bool legend_entry_visible(
        std::size_t index,std::uint8_t party_mode_258a8) noexcept;

    // CODE.2 01884..018E6: seeded 30CA2 selects one of four
    // authentic RES0+35h tiles for each 8x16 sidebar block.
    static constexpr std::size_t kSidebarFillTileCount=250U;
    [[nodiscard]] static std::array<std::uint8_t,kSidebarFillTileCount>
    sidebar_filler_frames(std::uint32_t original_irq_seed) noexcept;

    // CODE.2 01957..01A4E: 38 top/bottom pairs and 23 side
    // triples, all authored RES0+35 frames 5..19.
    static constexpr std::size_t kBorderFillTileCount=145U;
    [[nodiscard]] static std::array<std::uint8_t,kBorderFillTileCount>
    border_filler_frames(std::uint32_t original_irq_seed) noexcept;

    void draw(
        IndexedFramebuffer& framebuffer,
        const ExplorationSessionState& session,
        const RendererTables& tables,
        const AutomapScreenState& state,
        bool annotation_overlay_enabled = false,
        std::uint32_t original_irq_seed = 1U) const;

private:
    void draw_frame(
        IndexedFramebuffer& framebuffer,
        int local_resource,
        int frame,
        int row,
        int column) const;

    void draw_original_legend(
        IndexedFramebuffer& framebuffer,
        std::uint8_t party_mode_258a8) const;

    const RavenloftBootAssets& assets_;
    AutomapViewportPresenter viewport_;
    AutomapAnnotationPresenter annotations_;
    std::uint16_t res0_base_{};
    Type2Font legend_font_{};
    std::array<std::uint8_t,13> legend_frames_{};
};

}  // namespace rloft
