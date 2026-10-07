#pragma once

#include "rloft/character_creation_flow.hpp"
#include "rloft/character_stats.hpp"
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

    // Resting frame after CODE.1's presentation/slot animations have settled.
    // stats is required for mode 6.
    void screen(
        const CharacterCreationFlow& flow,
        const CharacterStatsModel* stats = nullptr,
        int selected_stat = -1,
        bool stats_editor = false);
    void screen(CharacterCreationState state);

    // Compatibility name for state 0.
    void first_screen();

private:
    void draw_frame(int archive, int resource, int frame, int y, int x);
    void draw_frame_mirrored(
        int archive, int resource, int frame, int y, int x);
    void draw_strip(int archive, int resource, int y, int x);
    void draw_opening_base();
    void draw_opening_moving_object(int position, int delta);
    void draw_text(
        int y, int x, const std::string& text, int font_id, bool selected = false);
    void draw_text_right(
        int y, int right, const std::string& text, int font_id, bool selected = false);
    void draw_text_centered(
        int y, int left, int span, const std::string& text, int font_id);
    void draw_empty_actor_card(int y, int x);
    void draw_text_box(int record, int y, int x);
    void draw_persistent_summary(const CharacterDraft& draft, int stage);
    void draw_stats_panel(
        const CharacterDraft& draft,
        const CharacterStatsModel& stats,
        int y,
        int x,
        int selected_stat);
    void draw_controller_base(
        CharacterCreationState state, const std::string& label);

    const RavenloftBootAssets& assets_;
    IndexedFramebuffer framebuffer_;
};

}  // namespace rloft
