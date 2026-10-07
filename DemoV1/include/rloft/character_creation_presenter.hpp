#pragma once

#include "rloft/character_creation_flow.hpp"
#include "rloft/character_palette_remap.hpp"
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

    void existing_actor_decision_screen(
        const CharacterCreationFlow& flow,
        int actor,
        const CharacterStatsModel& stats);
    void final_decision_screen(
        const CharacterCreationFlow& flow,
        const CharacterStatsModel& stats);
    void name_entry_screen(
        const CharacterCreationFlow& flow,
        const CharacterStatsModel& stats,
        const std::string& name,
        bool cursor_on);

    // CODE.1 15B0: 10 dissolve frames, plus 9 selected-slot movement
    // frames when selected_slot >= 0.
    [[nodiscard]] static int transition_15b0_frame_count(
        int selected_slot) noexcept {
        return selected_slot >= 0 ? 19 : 10;
    }
    void transition_15b0_frame(
        const CharacterCreationFlow& before,
        const CharacterStatsModel* stats,
        int selected_slot,
        int target_row,
        int target_column,
        int frame_index);

    // CODE.1 24D4: nine-frame selected-slot move used by intermediate
    // class-choice stages.
    void transition_24d4_frame(
        const CharacterCreationFlow& before,
        int selected_slot,
        int target_row,
        int target_column,
        int frame_index);

    [[nodiscard]] static int transition_07e4_frame_count(
        int first_slot,
        int last_slot) noexcept {
        return (last_slot - first_slot) + 10;
    }
    void transition_07e4_frame(
        const CharacterCreationFlow& before,
        const CharacterCreationFlow& after,
        const CharacterStatsModel* before_stats,
        const CharacterStatsModel* after_stats,
        int first_slot,
        int last_slot,
        int frame_index);

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
    void draw_actor_card(const CharacterDraft& actor, int y, int x);
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
    [[nodiscard]] bool slot_active(
        const CharacterCreationFlow& flow,
        int slot) const noexcept;
    void draw_slot_content(
        const CharacterCreationFlow& flow,
        const CharacterStatsModel* stats,
        int slot,
        int y,
        int x);
    void draw_slot_content_mapped(
        const CharacterCreationFlow& flow,
        const CharacterStatsModel* stats,
        int slot,
        int y,
        int x,
        const std::array<std::uint8_t, 256>& bright_map,
        const std::array<std::uint8_t, 256>& luminance_map,
        const IndexedRegion* cached);
    void draw_slot_scaled_07e4(
        const CharacterCreationFlow& flow,
        const CharacterStatsModel* stats,
        int slot,
        int y,
        int x,
        int numerator,
        int denominator,
        const IndexedRegion* cached);

    const RavenloftBootAssets& assets_;
    CharacterPaletteRemaps remaps_;
    IndexedFramebuffer framebuffer_;
};

}  // namespace rloft
