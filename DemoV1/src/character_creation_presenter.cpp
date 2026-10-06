#include "rloft/character_creation_presenter.hpp"

#include <array>

namespace rloft {

CharacterCreationPresenter::CharacterCreationPresenter(
    const RavenloftBootAssets& assets)
    : assets_(assets) {}

Palette6 CharacterCreationPresenter::palette() const {
    // CODE.1 0x4978 calls C254(buffer, 0, 8, "COLORS", 256)
    // before its opening presentation and seven-state character controller.
    return assets_.colors_palette(8);
}

void CharacterCreationPresenter::draw_frame(
    int archive, int resource, int frame, int y, int x) {
    framebuffer_.draw_frame(assets_.type1(archive, resource), frame, y, x);
}

void CharacterCreationPresenter::draw_strip(
    int archive, int resource, int y, int x) {
    framebuffer_.draw(assets_.type1(archive, resource), y, x);
}

void CharacterCreationPresenter::first_screen() {
    framebuffer_.clear(0);

    // CODE.1 presentation routine 0x10A4, state 0.
    //
    // 86C4 vertically emits every frame of these two RES7 resources.
    draw_strip(7, 0x18, 0, 0);
    draw_strip(7, 0x19, 200, 0);

    // 6C60(frame=0, y=5, x=0, resource=0x2A, archive=7).
    draw_frame(7, 0x2A, 0, 5, 0);

    // Exact CODE.1 DGROUP tables at 0x087C / 0x0880.
    static constexpr std::array<int, 9> y{
        13, 67, 91, 91, 67, 13, 91, 91, 91
    };
    static constexpr std::array<int, 9> x{
        3, 56, 109, 162, 215, 268, 83, 136, 189
    };

    // Base controls: RES7:30..38.
    for (int i = 0; i < 9; ++i) {
        draw_frame(7, 0x1E + i, 0, y[i], x[i]);
    }

    // State-0 0x1464 selector vector:
    // {-1,-1,1,1,-1,-1,-1,-1,-1}.
    // Only slots 2 and 3 have an additional graphic at rest.
    draw_frame(7, 1, 0, y[2], x[2]);
    draw_frame(7, 1, 0, y[3], x[3]);
}

}  // namespace rloft
