#pragma once

#include "rloft/resource_assets.hpp"
#include "rloft/startup_presenter.hpp"

namespace rloft {

class CharacterCreationPresenter final {
public:
    explicit CharacterCreationPresenter(const RavenloftBootAssets& assets);

    [[nodiscard]] IndexedFramebuffer& framebuffer() noexcept { return framebuffer_; }
    [[nodiscard]] const IndexedFramebuffer& framebuffer() const noexcept { return framebuffer_; }
    [[nodiscard]] Palette6 palette() const;

    // Final resting frame of CODE.1 state 0, before the first input event.
    void first_screen();

private:
    void draw_frame(int archive, int resource, int frame, int y, int x);
    void draw_strip(int archive, int resource, int y, int x);

    const RavenloftBootAssets& assets_;
    IndexedFramebuffer framebuffer_;
};

}  // namespace rloft
