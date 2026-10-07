#pragma once

#include <cstdint>

#include "rloft/resource_assets.hpp"
#include "rloft/startup_presenter.hpp"

namespace rloft {

struct ExplorationHudBaseLayout final {
    int upper_resource_id{};
    int lower_resource_id{};
    int upper_y{};
    int lower_y{};
    int x{};
};

class ExplorationHudPresenter final {
public:
    explicit ExplorationHudPresenter(
        const RavenloftBootAssets& assets)
        : assets_(assets) {}

    // CODE.2 1F8FC: resource base 258DC comes from INIT +00. The active
    // presentation mode selects a three-resource group; +1 is drawn at
    // y=0,x=0 and +2 at y=200,x=0.
    [[nodiscard]] static ExplorationHudBaseLayout base_layout(
        std::uint16_t res0_raster_base,
        int mode);

    void draw_base(
        IndexedFramebuffer& framebuffer,
        int mode) const;

private:
    const RavenloftBootAssets& assets_;
};

}  // namespace rloft
