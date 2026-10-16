#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/enhanced_options_presenter.hpp"
#include <cassert>

int main() {
    using rloft::EnhancedOptionsPresenter;
    using rloft::EnhancedOptionsSelection;
    assert(EnhancedOptionsPresenter::action_at(152,52)==
           EnhancedOptionsSelection::ToggleMusic);
    assert(EnhancedOptionsPresenter::action_at(194,267)==
           EnhancedOptionsSelection::ToggleMusic);
    assert(EnhancedOptionsPresenter::action_at(195,52)==
           EnhancedOptionsSelection::None);
    assert(EnhancedOptionsPresenter::action_at(211,52)==
           EnhancedOptionsSelection::ToggleSound);
    assert(EnhancedOptionsPresenter::action_at(249,267)==
           EnhancedOptionsSelection::ToggleSound);
    assert(EnhancedOptionsPresenter::action_at(250,52)==
           EnhancedOptionsSelection::None);
    assert(EnhancedOptionsPresenter::action_at(270,52)==
           EnhancedOptionsSelection::ReturnToGame);
    assert(EnhancedOptionsPresenter::action_at(312,267)==
           EnhancedOptionsSelection::ReturnToGame);
    assert(EnhancedOptionsPresenter::action_at(313,267)==
           EnhancedOptionsSelection::None);
    assert(EnhancedOptionsPresenter::action_at(170,51)==
           EnhancedOptionsSelection::None);
    assert(EnhancedOptionsPresenter::action_at(230,268)==
           EnhancedOptionsSelection::None);

    rloft::Type2Font font{};
    font.widths.fill(3U);
    font.raster.width=3;
    font.raster.height=7;
    font.raster.count=128;
    font.raster.transparent_index=0;
    font.raster.pixels.assign(128U*3U*7U,255U);
    rloft::IndexedFramebuffer screen{};
    screen.clear(7);
    EnhancedOptionsPresenter panel(font);
    panel.draw(screen,true,true);
    const auto at=[&](int row,int column){
        return screen.pixels()[
            static_cast<std::size_t>(row)*
            rloft::IndexedFramebuffer::kWidth+
            static_cast<std::size_t>(column)];
    };
    assert(at(0,0)==0U);
    assert(at(64,32)==0xB1U);
    assert(at(67,35)==0U);
    assert(at(170,52)==0xB1U);
    assert(at(230,52)==0xB1U);
    assert(at(290,52)==0xB1U);
    assert(at(175,56)==0U || at(175,56)==255U);
    screen.clear(7);
    panel.draw(screen,false,false);
    assert(at(0,0)==0U);
    assert(at(64,32)==0xB1U);
}
