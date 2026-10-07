#pragma once

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
          res0_base_(assets.init_u16(0x00)) {}

    void draw(
        IndexedFramebuffer& framebuffer,
        const ExplorationSessionState& session,
        const RendererTables& tables,
        const AutomapScreenState& state,
        bool annotation_overlay_enabled = false) const;

private:
    void draw_frame(
        IndexedFramebuffer& framebuffer,
        int local_resource,
        int frame,
        int row,
        int column) const;

    const RavenloftBootAssets& assets_;
    AutomapViewportPresenter viewport_;
    std::uint16_t res0_base_{};
};

}  // namespace rloft
