#pragma once

#include <cstdint>
#include <string_view>

#include "rloft/indexed_font_presenter.hpp"
#include "rloft/startup_presenter.hpp"

namespace rloft {

// Native PC in-game options surface. Unlike a Windows MessageBox this
// draws inside the same 320x400 indexed game framebuffer and remains
// compatible with normal window scaling and the current game palette.
enum class EnhancedOptionsSelection : std::uint8_t {
    None, ToggleMusic, ReturnToGame
};

class EnhancedOptionsPresenter final {
public:
    static constexpr int kButtonLeft=52;
    static constexpr int kButtonRight=268;
    static constexpr int kMusicTop=152;
    static constexpr int kMusicBottom=195;
    static constexpr int kBackTop=218;
    static constexpr int kBackBottom=261;

    explicit EnhancedOptionsPresenter(const Type2Font& font)
        : font_(font) {}

    [[nodiscard]] static constexpr EnhancedOptionsSelection action_at(
        int row,int column) noexcept {
        if(column<kButtonLeft || column>=kButtonRight)
            return EnhancedOptionsSelection::None;
        if(row>=kMusicTop && row<kMusicBottom)
            return EnhancedOptionsSelection::ToggleMusic;
        if(row>=kBackTop && row<kBackBottom)
            return EnhancedOptionsSelection::ReturnToGame;
        return EnhancedOptionsSelection::None;
    }

    void draw(IndexedFramebuffer& fb,bool music_enabled) const {
        // Full opaque repaint avoids stale world/automap pixels behind
        // unloaded transparent RES UI chrome.
        fb.clear(0U);
        fb.fill_rect(64,32,246,256,0xB1U);
        fb.fill_rect(67,35,240,250,0U);
        fb.fill_rect(144,kButtonLeft,59,
                     kButtonRight-kButtonLeft,0xB1U);
        fb.fill_rect(147,kButtonLeft+3,53,
                     kButtonRight-kButtonLeft-6,0U);
        fb.fill_rect(210,kButtonLeft,59,
                     kButtonRight-kButtonLeft,0xB1U);
        fb.fill_rect(213,kButtonLeft+3,53,
                     kButtonRight-kButtonLeft-6,0U);

        IndexedFontPresenter::draw_centered(
            fb,font_,96,32,256,"ENHANCED PC OPTIONS");
        IndexedFontPresenter::draw_centered(
            fb,font_,168,kButtonLeft,kButtonRight-kButtonLeft,
            music_enabled?"MUSIC: ON":"MUSIC: OFF");
        IndexedFontPresenter::draw_centered(
            fb,font_,235,kButtonLeft,kButtonRight-kButtonLeft,
            "RETURN TO GAME");
        IndexedFontPresenter::draw_centered(
            fb,font_,287,32,256,"ESC TO RETURN");
    }
private:
    const Type2Font& font_;
};

} // namespace rloft
