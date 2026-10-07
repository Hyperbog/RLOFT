#pragma once

#include "rloft/resource_assets.hpp"
#include "rloft/startup_presenter.hpp"

namespace rloft {

class CharacterCreationPresenter final {
public:
    static constexpr int kOpeningFrameCount = 70;

    explicit CharacterCreationPresenter(const RavenloftBootAssets& assets);

    [[nodiscard]] IndexedFramebuffer& framebuffer() noexcept { return framebuffer_; }
    [[nodiscard]] const IndexedFramebuffer& framebuffer() const noexcept { return framebuffer_; }
    [[nodiscard]] Palette6 palette() const;

    // CODE.1 0x45FC..0x4976. Frames 0..69 correspond to the two
    // (10-frame + 25-frame) groups exactly as the original loops execute.
    void opening_frame(int frame_index);

    // Final resting frame of CODE.1 state 0, before the first input event.
    void first_screen();

private:
    void draw_frame(int archive, int resource, int frame, int y, int x);
    void draw_frame_mirrored(
        int archive, int resource, int frame, int y, int x);
    void draw_strip(int archive, int resource, int y, int x);
    void draw_opening_base();
    void draw_opening_moving_object(int position, int delta);

    const RavenloftBootAssets& assets_;
    IndexedFramebuffer framebuffer_;
};

}  // namespace rloft
