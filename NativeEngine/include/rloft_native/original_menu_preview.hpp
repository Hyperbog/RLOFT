#pragma once

// READ-ONLY original static CODE.2 startup-menu composition from the
// existing original-source-traced StartupPresenter; NOT a running menu.
// This is an integration witness for Windows original assets, not gameplay.
#include "rloft/game_source.hpp"
#include "rloft/resource_assets.hpp"
#include "rloft/startup_presenter.hpp"
#include "rloft_native/frame_contract.hpp"

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <stdexcept>
#include <vector>

namespace rloft_native {

inline IndexedFrame make_source_menu_preview(const rloft::GameSource& source) {
    // GameSource is move-only; recreate a read-only reader for this short
    // diagnostic, retaining the caller's original install source unchanged.
    const auto& path = source.location().media_path;
    rloft::RavenloftBootAssets assets(rloft::GameSource::open(path));
    rloft::StartupPresenter presenter(assets);

    // Source evidence: CODE.2 192CC. This draws RES0 97..101 at the original
    // menu coordinates; -1 means no selected option/interactive menu state.
    presenter.menu_frame(-1);
    IndexedFrame result;
    result.pixels = presenter.framebuffer().pixels();
    result.dac6 = presenter.palette12().rgb;
    result.validate();
    return result;
}

} // namespace rloft_native
