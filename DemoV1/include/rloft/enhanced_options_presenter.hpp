#pragma once

#include <cstdint>
#include <string_view>

#include "rloft/indexed_font_presenter.hpp"
#include "rloft/startup_presenter.hpp"

namespace rloft {

// Native PC in-game options. Indexed framebuffer surface; separate
// music and sampled-SFX toggles without touching original GAME.OPT.
enum class EnhancedOptionsSelection : std::uint8_t {
    None, ToggleMusic, ToggleSound, ReturnToGame
};

class EnhancedOptionsPresenter final {
public:
    static constexpr int kButtonLeft=52;
    static constexpr int kButtonRight=268;
    static constexpr int kMusicTop=152;
    static constexpr int kMusicBottom=195;
    static constexpr int kSoundTop=211;
    static constexpr int kSoundBottom=250;
    static constexpr int kBackTop=270;
    static constexpr int kBackBottom=313;

    explicit EnhancedOptionsPresenter(const Type2Font& font)
        : font_(font) {}

    [[nodiscard]] static constexpr EnhancedOptionsSelection action_at(
        int row,int column) noexcept {
        if(column<kButtonLeft || column>=kButtonRight)
            return EnhancedOptionsSelection::None;
        if(row>=kMusicTop && row<kMusicBottom)
            return EnhancedOptionsSelection::ToggleMusic;
        if(row>=kSoundTop && row<kSoundBottom)
            return EnhancedOptionsSelection::ToggleSound;
        if(row>=kBackTop && row<kBackBottom)
            return EnhancedOptionsSelection::ReturnToGame;
        return EnhancedOptionsSelection::None;
    }

    void draw(IndexedFramebuffer& fb,bool music_enabled,
              bool sound_enabled) const {
        fb.clear(0U);
        fb.fill_rect(64,32,300,256,0xB1U);
        fb.fill_rect(67,35,294,250,0U);
        // Music
        fb.fill_rect(144,kButtonLeft,59,
                     kButtonRight-kButtonLeft,0xB1U);
        fb.fill_rect(147,kButtonLeft+3,53,
                     kButtonRight-kButtonLeft-6,0U);
        // Sampled original Creative Voice SFX
        fb.fill_rect(204,kButtonLeft,56,
                     kButtonRight-kButtonLeft,0xB1U);
        fb.fill_rect(207,kButtonLeft+3,50,
                     kButtonRight-kButtonLeft-6,0U);
        // Return
        fb.fill_rect(263,kButtonLeft,57,
                     kButtonRight-kButtonLeft,0xB1U);
        fb.fill_rect(266,kButtonLeft+3,51,
                     kButtonRight-kButtonLeft-6,0U);

        IndexedFontPresenter::draw_centered(
            fb,font_,96,32,256,"ENHANCED PC OPTIONS");
        IndexedFontPresenter::draw_centered(
            fb,font_,168,kButtonLeft,kButtonRight-kButtonLeft,
            music_enabled?"MUSIC: ON":"MUSIC: OFF");
        IndexedFontPresenter::draw_centered(
            fb,font_,226,kButtonLeft,kButtonRight-kButtonLeft,
            sound_enabled?"SOUND: ON":"SOUND: OFF");
        IndexedFontPresenter::draw_centered(
            fb,font_,286,kButtonLeft,kButtonRight-kButtonLeft,
            "RETURN TO GAME");
        IndexedFontPresenter::draw_centered(
            fb,font_,335,32,256,"ESC TO RETURN");
    }
private:
    const Type2Font& font_;
};

} // namespace rloft
