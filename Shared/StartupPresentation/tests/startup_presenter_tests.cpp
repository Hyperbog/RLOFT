#include "rloft/startup_presenter.hpp"
#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <vector>
namespace {
class Fixture final : public rloft::StartupVisualSource {
public:
    [[nodiscard]] rloft::Type1Raster res0_type1(int id) const override {
        if(id<94 || id>105)throw std::out_of_range("startup fixture RES0 id");
        return {1,8,1,0xFF,std::vector<std::uint8_t>(8,std::uint8_t(id))};
    }
    [[nodiscard]] rloft::Type2Font res0_font(int) const override {
        return {};
    }
    [[nodiscard]] std::string text(int) const override {
        return {};
    }
    [[nodiscard]] rloft::Palette6 colors_palette(
        int bank,int brightness=0) const override {
        if(bank<0||bank>=15||brightness!=0)
            throw std::out_of_range("startup fixture color bank");
        rloft::Palette6 result;
        result.rgb.fill(std::uint8_t(bank));
        return result;
    }
};
}
int main() {
    Fixture fixture;
    rloft::StartupPresenter p(fixture);
    const auto pixel=[&](unsigned x,unsigned y) {
        return p.framebuffer().pixels().at(std::size_t(y)*320U+x);
    };
    p.first_logo();
    assert(pixel(40,143)==94U);
    assert(pixel(39,143)==0U);
    p.second_logo();
    assert(pixel(36,138)==95U);
    // Original CODE.2 doesn't clear the preceding logo when drawing 95.
    assert(pixel(40,143)==94U);
    p.third_logo();
    assert(pixel(0,0)==96U);
    p.title_background();
    assert(pixel(0,0)==97U);
    p.menu_animation_frame(1);
    p.menu_animation_frame(14);
    // Exact terminal positions for original CODE.2 191C0 panel slots.
    assert(pixel(85,7)==98U);
    assert(pixel(0,79)==99U);
    assert(pixel(0,200)==100U);
    assert(pixel(143,341)==101U);
    p.menu_frame(2);
    assert(pixel(39,302)==104U);
    assert(pixel(39,301)==0U);
    assert(rloft::StartupPresenter::menu_selection_from_y(285)==0);
    assert(rloft::StartupPresenter::menu_selection_from_y(286)==1);
    assert(rloft::StartupPresenter::menu_selection_from_y(300)==1);
    assert(rloft::StartupPresenter::menu_selection_from_y(301)==2);
    assert(rloft::StartupPresenter::menu_selection_from_y(318)==3);
    assert(rloft::StartupPresenter::menu_selection_from_y(400)==-1);
    const auto faded=rloft::StartupPresenter::fade_factor(p.palette12(),4);
    assert(faded.rgb[0]==6U);
    bool failed=false;
    try{p.menu_animation_frame(15);}
    catch(const std::out_of_range&){failed=true;}
    assert(failed);
}
