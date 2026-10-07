#include "rloft/character_creation_presenter.hpp"

#include <array>
#include <cstddef>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace rloft {
namespace {

constexpr std::array<int, 9> kSlotRow{
    13, 67, 91, 91, 67, 13, 91, 91, 91
};
constexpr std::array<int, 9> kSlotColumn{
    3, 56, 109, 162, 215, 268, 83, 136, 189
};

int div8_after_plus4(int value) noexcept {
    const auto wrapped =
        static_cast<std::int32_t>(
            static_cast<std::uint32_t>(value) + 4U);
    return wrapped / 8;
}

void advance_indicator(int& value, int& step) {
    int sum = value + step;
    if (sum == 0x18) {
        sum = 0x14;
        step = -2;
    }
    if (sum == -4) {
        sum = 0;
        step = 2;
    }
    value = sum;
}

}  // namespace

CharacterCreationPresenter::CharacterCreationPresenter(
    const RavenloftBootAssets& assets)
    : assets_(assets),
      remaps_(
          CharacterPaletteRemaps::build(
              assets.colors_palette(8))) {}

Palette6 CharacterCreationPresenter::palette() const {
    // CODE.1 0x4978 calls C254(buffer, 0, 8, "COLORS", 256)
    // before its opening presentation and seven-state character controller.
    return assets_.colors_palette(8);
}

void CharacterCreationPresenter::draw_frame(
    int archive, int resource, int frame, int y, int x) {
    framebuffer_.draw_frame(assets_.type1(archive, resource), frame, y, x);
}

void CharacterCreationPresenter::draw_frame_mirrored(
    int archive, int resource, int frame, int y, int x) {
    framebuffer_.draw_frame_mirrored(
        assets_.type1(archive, resource), frame, y, x);
}

void CharacterCreationPresenter::draw_strip(
    int archive, int resource, int y, int x) {
    framebuffer_.draw(assets_.type1(archive, resource), y, x);
}

void CharacterCreationPresenter::draw_opening_base() {
    framebuffer_.clear(0);

    // CODE.1 45FC:
    // 86C4(0,0,1B,7), 86C4(C8,0,1C,7).
    draw_strip(7, 0x1B, 0, 0);
    draw_strip(7, 0x1C, 200, 0);
}

void CharacterCreationPresenter::draw_opening_moving_object(
    int position, int delta) {
    // CODE.1 4520. A5C8 temporarily mirrors the resource in the two
    // positive-direction cases, then immediately restores the source.
    if (position == 0x16) {
        draw_frame_mirrored(
            7, 0x3D, 0, 0xE5, position - delta);
    } else if (position == -2) {
        draw_frame(7, 0x3D, 0, 0xE5, position - delta);
    } else if (delta > 0) {
        draw_frame_mirrored(7, 0x3C, 0, 0xE5, position);
    } else {
        draw_frame(7, 0x3C, 0, 0xE5, position);
    }

    draw_frame(7, 0x3B, 0, 0xE5, 0);
}

void CharacterCreationPresenter::opening_frame(int frame_index) {
    if (frame_index < 0 || frame_index >= kOpeningFrameCount) {
        throw std::out_of_range("CODE.1 opening frame index");
    }

    draw_opening_base();

    // 7868(E5,0,AA,50,FF): capture the panel before 4520 paints into it.
    const auto panel = framebuffer_.capture(0xE5, 0, 0xAA, 0x50);

    // The setup page contains the indicator and RES7:31. CODE.1 then copies
    // that page, switches draw pages and restores only the panel region before
    // taking the first central capture. In a single host framebuffer this is
    // equivalent to the following sequence.
    draw_opening_moving_object(0, 2);
    draw_frame(7, 0x31, 0, 0x15F, 0x74);
    framebuffer_.restore(panel, 0xE5, 0);

    // Initial 7868(96,8C,E2,68,FF).
    auto central = framebuffer_.capture(0x96, 0x8C, 0xE2, 0x68);

    static constexpr std::array<int, 10> sequence{
        0x36, 0x37, 0x38, 0x34, 0x35,
        0x36, 0x37, 0x38, 0x39, 0x3A
    };

    int value = 0;
    int step = 2;
    int logical_frame = 0;

    for (int group = 0; group < 2; ++group) {
        // First loop: 10 frames, each sequence image held for two frames.
        for (int frame = 0; frame < 10; ++frame) {
            framebuffer_.restore(central, 0x96, 0x8C);
            draw_frame(
                7,
                sequence[static_cast<std::size_t>(group) * 5U +
                         static_cast<std::size_t>(frame / 2)],
                0,
                0x96,
                0x8C);

            framebuffer_.restore(panel, 0xE5, 0);
            draw_opening_moving_object(value, step);

            if (logical_frame == frame_index) return;
            ++logical_frame;
            advance_indicator(value, step);
        }

        // Second loop: 25 frames. The central graphic remains constant while
        // the panel indicator continues its encoded paired-counter motion.
        for (int frame = 0; frame < 25; ++frame) {
            framebuffer_.restore(central, 0x96, 0x8C);
            draw_frame(
                7,
                group == 0 ? 0x32 : 0x33,
                0,
                0x15F,
                group == 0 ? 0x95 : 0xB2);

            framebuffer_.restore(panel, 0xE5, 0);
            draw_opening_moving_object(value, step);

            if (logical_frame == frame_index) return;
            ++logical_frame;
            advance_indicator(value, step);
        }

        // CODE.1 frees the E2-row capture and recaptures D8 rows from the
        // just-rendered page. Group 1 therefore inherits group 0's final
        // central artwork instead of starting from the original baseline.
        central = framebuffer_.capture(0x96, 0x8C, 0xD8, 0x68);
    }

    throw std::logic_error("opening frame simulation did not reach requested frame");
}

void CharacterCreationPresenter::draw_text(
    int y,
    int x,
    const std::string& text,
    int font_id,
    bool selected) {
    if (text.empty()) return;

    const auto font = assets_.res0_font(font_id);
    auto& dst = framebuffer_.mutable_pixels();

    for (const unsigned char ch : text) {
        if (ch >= font.widths.size() || ch >= font.raster.count) {
            throw std::runtime_error(
                "CODE.1 text uses glyph outside RES0 font resource");
        }

        if (!selected) {
            framebuffer_.draw_frame(font.raster, ch, y, x);
        } else {
            // function_38A4 changes 59F8[8] from 8 to 5Ah and routes font
            // blits through that remap while the selected row is drawn.
            const auto frame_stride =
                static_cast<std::size_t>(font.raster.width) *
                font.raster.height;
            const auto frame_at =
                static_cast<std::size_t>(ch) * frame_stride;

            for (int sy = 0; sy < font.raster.height; ++sy) {
                const int dy = y + sy;
                if (dy < 0 || dy >= IndexedFramebuffer::kHeight) continue;

                for (int sx = 0; sx < font.raster.width; ++sx) {
                    const int dx = x + sx;
                    if (dx < 0 || dx >= IndexedFramebuffer::kWidth) continue;

                    auto value = font.raster.pixels[
                        frame_at +
                        static_cast<std::size_t>(sy) * font.raster.width +
                        sx];
                    if (font.raster.transparent_index != 0xFF &&
                        value == font.raster.transparent_index) {
                        continue;
                    }
                    if (value == 8) value = 0x5A;

                    dst[static_cast<std::size_t>(dy) *
                            IndexedFramebuffer::kWidth +
                        dx] = value;
                }
            }
        }

        x += font.widths[ch];
    }
}

void CharacterCreationPresenter::draw_text_right(
    int y,
    int right,
    const std::string& text,
    int font_id,
    bool selected) {
    const auto font = assets_.res0_font(font_id);
    int width = 0;
    for (const unsigned char ch : text) {
        if (ch >= font.widths.size()) {
            throw std::runtime_error(
                "CODE.1 text uses glyph outside RES0 font table");
        }
        width += font.widths[ch];
    }
    draw_text(y, right - width, text, font_id, selected);
}

void CharacterCreationPresenter::draw_text_centered(
    int y, int left, int span, const std::string& text, int font_id) {
    if (text.empty()) return;

    const auto font = assets_.res0_font(font_id);
    int width = 0;
    for (const unsigned char ch : text) {
        if (ch >= font.widths.size()) {
            throw std::runtime_error(
                "CODE.1 text uses glyph outside RES0 font table");
        }
        width += font.widths[ch];
    }

    draw_text(y, left + (span - width) / 2, text, font_id);
}

void CharacterCreationPresenter::draw_empty_actor_card(int y, int x) {
    // function_1A04 -> function_004C(T[0]) -> function_00EC mode 0.
    draw_frame(7, 0x1A, 0, y, x);

    const auto label = assets_.text(0);
    std::vector<std::string> words;
    std::size_t start = 0;
    while (start < label.size()) {
        const auto space = label.find(' ', start);
        if (space == std::string::npos) {
            words.emplace_back(label.substr(start));
            break;
        }
        words.emplace_back(label.substr(start, space - start));
        start = space + 1;
    }

    int row = y + 0x7A / 2 - static_cast<int>(words.size()) * 5;
    for (const auto& word : words) {
        draw_text_centered(row, x, 0x30, word, 0);
        row += 10;
    }
}

void CharacterCreationPresenter::draw_actor_card(
    const CharacterDraft& actor, int y, int x) {
    if (actor.sex < 0 || actor.face < 0) {
        throw std::logic_error(
            "completed actor card is missing sex or portrait");
    }

    // function_1A04 present-actor branch.
    draw_frame(7, 0x14, 0, y, x);
    draw_frame(
        1,
        336 + actor.sex * 23 + actor.face,
        0,
        y + 0x15,
        x);
    draw_text_centered(y + 2, x, 0x30, actor.name, 0);
}

void CharacterCreationPresenter::draw_text_box(int record, int y, int x) {
    draw_frame(7, 0x1A, 0, y, x);

    const auto label = assets_.text(record);
    std::vector<std::string> words;
    std::size_t start = 0;
    while (start < label.size()) {
        while (start < label.size() && label[start] == ' ') ++start;
        if (start >= label.size()) break;
        const auto space = label.find(' ', start);
        if (space == std::string::npos) {
            words.emplace_back(label.substr(start));
            break;
        }
        words.emplace_back(label.substr(start, space - start));
        start = space + 1;
    }

    int row = y + 0x7A / 2 - static_cast<int>(words.size()) * 5;
    for (const auto& word : words) {
        draw_text_centered(row, x, 0x30, word, 0);
        row += 10;
    }
}

void CharacterCreationPresenter::draw_persistent_summary(
    const CharacterDraft& draft, int stage) {
    // Exact function_04B0 staged painter.
    if (stage >= 1 && draft.sex >= 0) {
        draw_frame(7, draft.sex + 0x0C, 0, 0x114, 3);
    }

    if (stage >= 2 && draft.race >= 0) {
        draw_frame(7, draft.race, 0, 0x114, 0x38);
    }

    if (stage >= 3) {
        for (int i = 2; i >= 0; --i) {
            const int cls = draft.classes[static_cast<std::size_t>(i)];
            if (cls < 0) continue;
            draw_frame(
                7,
                cls + 6,
                0,
                0x114 - i * 0x14,
                0x6D - i);
        }
    }

    if (stage >= 4 && draft.option >= 0) {
        draw_frame(7, draft.option + 0x0E, 0, 0x114, 0xA2);
    }

    if (stage >= 5 && draft.sex >= 0 && draft.face >= 0) {
        draw_frame(7, 0x14, 0, 0x114, 0xD7);
        draw_frame(
            1,
            336 + draft.sex * 23 + draft.face,
            0,
            0x129,
            0xD7);
    }
}

void CharacterCreationPresenter::draw_stats_panel(
    const CharacterDraft& draft,
    const CharacterStatsModel& stats_model,
    int y,
    int x,
    int selected_stat) {
    draw_frame(7, 0x14, 0, y, x);

    const auto& stats = stats_model.value();

    auto two_digits = [](int value) {
        std::ostringstream out;
        out << std::setfill('0') << std::setw(2) << value;
        return out.str();
    };

    int row = y + 4;
    for (int i = 0; i < 6; ++i) {
        const bool selected = selected_stat == i;
        draw_text(row, x + 1, assets_.text(0x2F + i), 0, selected);

        std::string value;
        if (i == 0 && stats.exceptional_strength != 0) {
            value =
                two_digits(stats.abilities[0]) + "/" +
                two_digits(stats.exceptional_strength % 100);
        } else {
            value = two_digits(stats.abilities[static_cast<std::size_t>(i)]);
        }

        draw_text_right(row, x + 0x2F, value, 0, selected);
        row += 10;
    }

    row += 10;
    draw_text(row, x + 1, assets_.text(0x35), 0);
    draw_text_right(
        row, x + 0x2F, std::to_string(stats.base_6810), 0);

    row += 10;
    draw_text(row, x + 1, assets_.text(0x36), 0);
    draw_text_right(
        row,
        x + 0x2F,
        std::to_string(stats_model.secondary_display_value()),
        0);

    row += 10;
    const bool multiclass = draft.classes[1] != -1;
    draw_text(
        row,
        x + 1,
        assets_.text(multiclass ? 0x38 : 0x37),
        0);

    std::ostringstream levels;
    levels << stats.levels[0];
    if (draft.classes[1] != -1) levels << '/' << stats.levels[1];
    if (draft.classes[2] != -1) levels << '/' << stats.levels[2];
    draw_text_right(row, x + 0x2F, levels.str(), 0);

    row += 10;
    const bool hp_selected = selected_stat == 10;
    draw_text(row, x + 1, assets_.text(0x39), 0, hp_selected);
    draw_text_right(
        row,
        x + 0x2F,
        std::to_string(stats.hit_points),
        0,
        hp_selected);
}

void CharacterCreationPresenter::draw_controller_base(
    CharacterCreationState state, const std::string& label) {
    framebuffer_.clear(0);

    if (state == CharacterCreationState::ActorChoice) {
        draw_strip(7, 0x18, 0, 0);
    }
    draw_strip(7, 0x19, 200, 0);
    draw_frame(7, 0x2A, 0, 5, 0);

    if (!label.empty()) {
        draw_text_centered(5, 0, 0x140, label, 1);
    }

    static constexpr std::array<int, 9> row{
        13, 67, 91, 91, 67, 13, 91, 91, 91
    };
    static constexpr std::array<int, 9> column{
        3, 56, 109, 162, 215, 268, 83, 136, 189
    };
    for (int i = 0; i < 9; ++i) {
        draw_frame(7, 0x1E + i, 0, row[i], column[i]);
    }
}

void CharacterCreationPresenter::draw_controller_entry_base(
    const CharacterCreationFlow& flow) {
    std::string label;
    int stage = 0;

    switch (flow.state()) {
        case CharacterCreationState::ActorChoice:
            break;
        case CharacterCreationState::SexChoice:
            label = assets_.text(0x26);
            break;
        case CharacterCreationState::RaceChoice:
            label = assets_.text(0x27);
            stage = 1;
            break;
        case CharacterCreationState::ClassChoice:
            label = assets_.text(0x28);
            stage = 3;
            break;
        case CharacterCreationState::OptionChoice:
            label = assets_.text(0x29);
            stage = 3;
            break;
        case CharacterCreationState::PortraitChoice:
            label = assets_.text(0x2A);
            stage = 4;
            break;
        case CharacterCreationState::StatsReview: {
            const auto offset = assets_.code1_s32(0x04B8);
            if (offset < 0) {
                throw std::runtime_error(
                    "CODE.1 mode-6 title pointer is negative");
            }
            label = assets_.code1_cstring(
                static_cast<std::size_t>(offset));
            stage = 5;
            break;
        }
    }

    // function_10A4's two initial page passes: controller chrome, all nine
    // slot backgrounds, title, and the 04B0 persistent summary only.
    draw_controller_base(flow.state(), label);
    if (stage != 0) {
        draw_persistent_summary(flow.draft(), stage);
    }
}

void CharacterCreationPresenter::draw_controller_text_boxes(
    const CharacterCreationFlow& flow) {
    switch (flow.state()) {
        case CharacterCreationState::ActorChoice:
            draw_text_box(
                flow.both_present() ? 1 : 2,
                0x114,
                0x10C);
            return;
        case CharacterCreationState::SexChoice:
            draw_text_box(8, 0x114, 0x10C);
            return;
        case CharacterCreationState::RaceChoice:
            draw_text_box(9, 0x114, 0x10C);
            return;
        case CharacterCreationState::ClassChoice:
            if (flow.class_stage() == 0) {
                draw_text_box(0x0A, 0x114, 0x10C);
            } else {
                draw_text_box(0x0B, 0x114, 0x10C);
                draw_text_box(3, 0x114, 0x0D7);
            }
            return;
        case CharacterCreationState::OptionChoice:
            draw_text_box(0x0C, 0x114, 0x10C);
            return;
        case CharacterCreationState::PortraitChoice:
            draw_text_box(0x0D, 0x114, 0x10C);
            draw_text_box(4, 0x114, 0x0D7);
            return;
        case CharacterCreationState::StatsReview:
            draw_text_box(0x0E, 0x114, 0x10C);
            return;
    }
}

void CharacterCreationPresenter::draw_final_decision_base(
    const CharacterCreationFlow& flow,
    const CharacterStatsModel& stats) {
    if (flow.state() != CharacterCreationState::StatsReview) {
        throw std::logic_error(
            "final decision base requires CODE.1 mode 6 draft");
    }
    draw_controller_base(
        CharacterCreationState::StatsReview,
        assets_.text(0x2D));
    draw_persistent_summary(flow.draft(), 5);
    draw_stats_panel(
        flow.draft(),
        stats,
        0x114,
        0x10C,
        -1);
}

void CharacterCreationPresenter::draw_name_entry_base(
    const CharacterCreationFlow& flow,
    const CharacterStatsModel& stats) {
    if (flow.state() != CharacterCreationState::StatsReview) {
        throw std::logic_error(
            "name entry base requires CODE.1 mode 6 draft");
    }
    draw_controller_base(
        CharacterCreationState::StatsReview,
        assets_.text(0x2E));
    draw_persistent_summary(flow.draft(), 5);
    draw_stats_panel(
        flow.draft(),
        stats,
        0x114,
        0x10C,
        -1);
}

void CharacterCreationPresenter::draw_10a4_ornament(
    int slot,
    int phase) {
    if (slot < 0 || slot >= 9 ||
        phase < 0 || phase >= 12) {
        throw std::out_of_range(
            "CODE.1 10A4 ornament frame");
    }

    static constexpr std::array<int, 12> sequence{
        0x15, 0x27, 0x28, 0x29, 0x28, 0x27,
        0x15, 0x27, 0x28, 0x29, 0x28, 0x27
    };
    static constexpr std::array<bool, 12> reversed{
        false, false, false, false, true, true,
        false, false, false, false, true, true
    };

    const auto raster =
        assets_.type1(
            7,
            sequence[static_cast<std::size_t>(phase)]);

    const int target_column =
        kSlotColumn[static_cast<std::size_t>(slot)] + 0x18;
    const int target_row =
        kSlotRow[static_cast<std::size_t>(slot)] + 0x3D;

    const int anchor_column =
        0xA0 +
        ((target_column - 0xA0) * (phase + 1) + 6) /
            12;
    const int anchor_row =
        -0x38 +
        ((target_row - (-0x38)) * (phase + 1) + 6) /
            12;

    const int row = anchor_row - raster.height / 2;
    const int column = anchor_column - raster.width * 2;

    if (reversed[static_cast<std::size_t>(phase)]) {
        // A5C8 is the same temporary in-place mirror used by 4520.
        framebuffer_.draw_frame_mirrored(
            raster,
            0,
            row,
            column);
    } else {
        framebuffer_.draw_frame(
            raster,
            0,
            row,
            column);
    }
}

std::pair<int, int>
CharacterCreationPresenter::controller_entry_range(
    CharacterCreationState state) const noexcept {
    switch (state) {
        case CharacterCreationState::ActorChoice:
        case CharacterCreationState::SexChoice:
            return {2, 4};
        case CharacterCreationState::StatsReview:
            return {6, 9};
        case CharacterCreationState::RaceChoice:
        case CharacterCreationState::ClassChoice:
        case CharacterCreationState::OptionChoice:
        case CharacterCreationState::PortraitChoice:
            return {0, 6};
    }
    return {0, 0};
}

bool CharacterCreationPresenter::slot_active(
    const CharacterCreationFlow& flow,
    int slot) const noexcept {
    if (slot < 0 || slot >= 9) return false;

    switch (flow.state()) {
        case CharacterCreationState::ActorChoice:
        case CharacterCreationState::SexChoice:
            return slot == 2 || slot == 3;

        case CharacterCreationState::RaceChoice:
        case CharacterCreationState::ClassChoice:
        case CharacterCreationState::OptionChoice:
        case CharacterCreationState::PortraitChoice:
            return slot < 6;

        case CharacterCreationState::StatsReview:
            return slot >= 6 && slot <= 8;
    }
    return false;
}

void CharacterCreationPresenter::draw_slot_content(
    const CharacterCreationFlow& flow,
    const CharacterStatsModel* stats,
    int slot,
    int y,
    int x) {
    const auto state = flow.state();

    if (!slot_active(flow, slot)) return;

    switch (state) {
        case CharacterCreationState::ActorChoice: {
            const int actor = slot - 2;
            if (flow.actor_present(actor)) {
                draw_actor_card(flow.actor(actor), y, x);
            } else {
                draw_empty_actor_card(y, x);
            }
            return;
        }

        case CharacterCreationState::SexChoice:
            draw_frame(7, 0x0C + slot - 2, 0, y, x);
            return;

        case CharacterCreationState::RaceChoice:
            draw_frame(7, slot, 0, y, x);
            return;

        case CharacterCreationState::ClassChoice: {
            const auto available = flow.available_choices();
            draw_frame(
                7,
                available[static_cast<std::size_t>(slot)]
                    ? 6 + slot
                    : 0x15,
                0,
                y,
                x);
            return;
        }

        case CharacterCreationState::OptionChoice: {
            const auto available = flow.available_choices();
            draw_frame(
                7,
                available[static_cast<std::size_t>(slot)]
                    ? 0x0E + slot
                    : 0x15,
                0,
                y,
                x);
            return;
        }

        case CharacterCreationState::PortraitChoice: {
            const auto available = flow.available_choices();
            if (!available[static_cast<std::size_t>(slot)]) {
                draw_frame(7, 0x15, 0, y, x);
                return;
            }
            draw_frame(7, 0x14, 0, y, x);
            draw_frame(
                1,
                336 +
                    flow.draft().sex * 23 +
                    flow.portrait_page() +
                    slot,
                0,
                y + 0x15,
                x);
            return;
        }

        case CharacterCreationState::StatsReview:
            if (slot == 6) {
                draw_frame(7, 0x16, 0, y, x);
                return;
            }
            if (slot == 7) {
                if (!stats) {
                    throw std::logic_error(
                        "stats slot requires generated stats");
                }
                draw_stats_panel(
                    flow.draft(), *stats, y, x, -1);
                return;
            }
            if (slot == 8) {
                draw_frame(7, 0x2E, 0, y, x);
                return;
            }
            return;
    }
}

void CharacterCreationPresenter::draw_slot_content_mapped(
    const CharacterCreationFlow& flow,
    const CharacterStatsModel* stats,
    int slot,
    int y,
    int x,
    const std::array<std::uint8_t, 256>& bright_map,
    const std::array<std::uint8_t, 256>& luminance_map,
    const IndexedRegion* cached) {
    if (!slot_active(flow, slot)) return;

    const auto state = flow.state();

    // CODE.1 mode 0 actor cards and mode 6 slot 7 are cached images and pass
    // through 6824 rather than being redrawn from their component resources.
    if (state == CharacterCreationState::ActorChoice ||
        (state == CharacterCreationState::StatsReview && slot == 7)) {
        if (!cached) {
            throw std::logic_error(
                "CODE.1 cached slot transition lacks captured region");
        }
        framebuffer_.restore_mapped(
            *cached, y, x, bright_map);
        return;
    }

    if (state == CharacterCreationState::PortraitChoice) {
        const auto available = flow.available_choices();
        if (!available[static_cast<std::size_t>(slot)]) {
            framebuffer_.draw_frame_mapped(
                assets_.type1(7, 0x15),
                0,
                y,
                x,
                bright_map);
            return;
        }

        framebuffer_.draw_frame_mapped(
            assets_.type1(7, 0x14),
            0,
            y,
            x,
            bright_map);
        framebuffer_.draw_frame_mapped(
            assets_.type1(
                1,
                336 +
                    flow.draft().sex * 23 +
                    flow.portrait_page() +
                    slot),
            0,
            y + 0x15,
            x,
            luminance_map);
        return;
    }

    int resource = -1;
    switch (state) {
        case CharacterCreationState::SexChoice:
            resource = 0x0C + slot - 2;
            break;
        case CharacterCreationState::RaceChoice:
            resource = slot;
            break;
        case CharacterCreationState::ClassChoice: {
            const auto available = flow.available_choices();
            resource =
                available[static_cast<std::size_t>(slot)]
                    ? 6 + slot
                    : 0x15;
            break;
        }
        case CharacterCreationState::OptionChoice: {
            const auto available = flow.available_choices();
            resource =
                available[static_cast<std::size_t>(slot)]
                    ? 0x0E + slot
                    : 0x15;
            break;
        }
        case CharacterCreationState::StatsReview:
            resource = slot == 6 ? 0x16 : 0x2E;
            break;
        case CharacterCreationState::ActorChoice:
        case CharacterCreationState::PortraitChoice:
            break;
    }

    if (resource < 0) {
        throw std::logic_error(
            "unsupported mapped character slot resource");
    }

    framebuffer_.draw_frame_mapped(
        assets_.type1(7, resource),
        0,
        y,
        x,
        bright_map);
}

void CharacterCreationPresenter::transition_15b0_frame(
    const CharacterCreationFlow& before,
    const CharacterStatsModel* stats,
    const CharacterCreationFlow* after,
    int selected_slot,
    int target_row,
    int target_column,
    int final_stage,
    int frame_index) {
    const int count =
        transition_15b0_frame_count(selected_slot);
    if (frame_index < 0 || frame_index >= count) {
        throw std::out_of_range(
            "CODE.1 15B0 transition frame index");
    }
    if (selected_slot >= 0 &&
        !slot_active(before, selected_slot)) {
        throw std::logic_error(
            "CODE.1 15B0 selected slot is not active");
    }

    // Reconstruct each displayed page from the settled source state. This is
    // equivalent to the original two-page scratch-buffer sequence because
    // each iteration restores the affected rectangle before drawing.
    screen(
        before,
        stats,
        -1,
        false,
        before.state() == CharacterCreationState::PortraitChoice
            ? selected_slot
            : -1);

    std::array<IndexedRegion, 9> cached{};
    std::array<bool, 9> has_cached{};

    // 1DD8 decrements 04D0 *before* calling 15B0 on Back. The old sex-choice
    // IDs are still installed, but 15B0 sees mode 0 and therefore draws the
    // cached actor cards from 095C/0960. Represent that deliberately mixed
    // state without replacing the surrounding sex-choice title/page.
    const bool sex_back_to_actor =
        selected_slot < 0 &&
        before.state() == CharacterCreationState::SexChoice &&
        after &&
        after->state() == CharacterCreationState::ActorChoice;

    if (sex_back_to_actor) {
        const auto old_page =
            framebuffer_.capture(
                0,
                0,
                IndexedFramebuffer::kHeight,
                IndexedFramebuffer::kWidth);

        screen(*after, nullptr);
        for (int slot : {2, 3}) {
            cached[static_cast<std::size_t>(slot)] =
                framebuffer_.capture(
                    kSlotRow[static_cast<std::size_t>(slot)],
                    kSlotColumn[static_cast<std::size_t>(slot)],
                    0x7A,
                    0x30);
            has_cached[static_cast<std::size_t>(slot)] = true;
        }

        framebuffer_.restore(old_page, 0, 0);
    } else {
        for (int slot = 0; slot < 9; ++slot) {
            if (!slot_active(before, slot)) continue;
            if (before.state() == CharacterCreationState::ActorChoice ||
                (before.state() == CharacterCreationState::StatsReview &&
                 slot == 7)) {
                cached[static_cast<std::size_t>(slot)] =
                    framebuffer_.capture(
                        kSlotRow[static_cast<std::size_t>(slot)],
                        kSlotColumn[static_cast<std::size_t>(slot)],
                        0x7A,
                        0x30);
                has_cached[static_cast<std::size_t>(slot)] = true;
            }
        }
    }

    if (frame_index < 5) {
        const auto& bright =
            remaps_.bright[static_cast<std::size_t>(frame_index)];
        const auto& lum =
            remaps_.luminance[static_cast<std::size_t>(frame_index)];
        const auto& dissolve_flow =
            sex_back_to_actor ? *after : before;
        const auto* dissolve_stats =
            sex_back_to_actor ? nullptr : stats;

        for (int slot = 0; slot < 9; ++slot) {
            if (!slot_active(before, slot) ||
                slot == selected_slot) {
                continue;
            }

            draw_slot_content_mapped(
                dissolve_flow,
                dissolve_stats,
                slot,
                kSlotRow[static_cast<std::size_t>(slot)],
                kSlotColumn[static_cast<std::size_t>(slot)],
                bright,
                lum,
                has_cached[static_cast<std::size_t>(slot)]
                    ? &cached[static_cast<std::size_t>(slot)]
                    : nullptr);
        }
        return;
    }

    if (frame_index < 10) {
        const int map_row = 9 - frame_index;
        const auto& bright =
            remaps_.bright[static_cast<std::size_t>(map_row)];

        for (int slot = 0; slot < 9; ++slot) {
            if (!slot_active(before, slot) ||
                slot == selected_slot) {
                continue;
            }

            framebuffer_.draw_frame_mapped(
                assets_.type1(7, 0x1E + slot),
                0,
                kSlotRow[static_cast<std::size_t>(slot)],
                kSlotColumn[static_cast<std::size_t>(slot)],
                bright);
        }
        return;
    }

    if (selected_slot < 0) {
        throw std::logic_error(
            "dissolve-only 15B0 transition has no movement frames");
    }
    if (final_stage < 1 || final_stage > 5) {
        throw std::logic_error(
            "selected CODE.1 15B0 requires final 04B0 stage 1..5");
    }

    if (frame_index == 19) {
        // After the nine movement frames, 15B0 redraws RES7:19 over the
        // lower page, reapplies 04B0(final_mode), and calls 5454 for one
        // final full-page presentation before the next controller starts.
        transition_15b0_frame(
            before,
            stats,
            after,
            selected_slot,
            target_row,
            target_column,
            final_stage,
            18);
        if (!after) {
            throw std::logic_error(
                "selected CODE.1 15B0 final frame lacks post-selection state");
        }
        draw_strip(7, 0x19, 200, 0);

        // function_266C calls 15B0 before function_1F24 normalizes the
        // selected class tuple. CharacterCreationFlow normalizes as part of
        // apply_event(), so reconstruct that one pre-normalization summary
        // frame from the source class stage and selected slot.
        if (before.state() == CharacterCreationState::ClassChoice &&
            after->state() == CharacterCreationState::OptionChoice &&
            selected_slot >= 0) {
            auto transition_draft = before.draft();
            const int stage = before.class_stage();
            if (stage < 0 || stage >= 3) {
                throw std::logic_error(
                    "CODE.1 final class transition has invalid class stage");
            }
            transition_draft.classes[
                static_cast<std::size_t>(stage)] = selected_slot;
            draw_persistent_summary(
                transition_draft,
                final_stage);
        } else {
            draw_persistent_summary(
                after->draft(),
                final_stage);
        }
        return;
    }

    // End of the second loop: all active slots except the selected choice are
    // their ordinary slot backgrounds.
    for (int slot = 0; slot < 9; ++slot) {
        if (!slot_active(before, slot)) continue;
        if (slot != selected_slot) {
            draw_frame(
                7,
                0x1E + slot,
                0,
                kSlotRow[static_cast<std::size_t>(slot)],
                kSlotColumn[static_cast<std::size_t>(slot)]);
        }
    }

    const int start_row =
        kSlotRow[static_cast<std::size_t>(selected_slot)];
    const int start_column =
        kSlotColumn[static_cast<std::size_t>(selected_slot)];

    // The selected slot's source rectangle is restored from scratch before
    // every movement frame.
    draw_frame(
        7,
        0x1E + selected_slot,
        0,
        start_row,
        start_column);

    const int step = frame_index - 10;
    const auto row_product = static_cast<std::int32_t>(
        static_cast<std::uint32_t>(target_row - start_row) *
        static_cast<std::uint32_t>(step));
    const auto column_product = static_cast<std::int32_t>(
        static_cast<std::uint32_t>(target_column - start_column) *
        static_cast<std::uint32_t>(step));

    const int row =
        start_row + div8_after_plus4(row_product);
    const int column =
        start_column + div8_after_plus4(column_product);

    draw_slot_content(
        before,
        stats,
        selected_slot,
        row,
        column);
}

void CharacterCreationPresenter::transition_24d4_frame(
    const CharacterCreationFlow& before,
    int selected_slot,
    int target_row,
    int target_column,
    int frame_index) {
    if (frame_index < 0 || frame_index > 8) {
        throw std::out_of_range(
            "CODE.1 24D4 transition frame index");
    }
    if (!slot_active(before, selected_slot)) {
        throw std::logic_error(
            "CODE.1 24D4 selected slot is not active");
    }

    screen(before, nullptr);

    const int start_row =
        kSlotRow[static_cast<std::size_t>(selected_slot)];
    const int start_column =
        kSlotColumn[static_cast<std::size_t>(selected_slot)];

    draw_frame(
        7,
        0x1E + selected_slot,
        0,
        start_row,
        start_column);

    const int dr =
        static_cast<std::int32_t>(
            static_cast<std::uint32_t>(target_row) -
            static_cast<std::uint32_t>(start_row)) /
        8;
    const int dc =
        static_cast<std::int32_t>(
            static_cast<std::uint32_t>(target_column) -
            static_cast<std::uint32_t>(start_column)) /
        8;

    const int row =
        static_cast<std::int32_t>(
            static_cast<std::uint32_t>(start_row) +
            static_cast<std::uint32_t>(dr * frame_index));
    const int column =
        static_cast<std::int32_t>(
            static_cast<std::uint32_t>(start_column) +
            static_cast<std::uint32_t>(dc * frame_index));

    draw_slot_content(
        before,
        nullptr,
        selected_slot,
        row,
        column);
}

void CharacterCreationPresenter::draw_slot_scaled_07e4(
    const CharacterCreationFlow& flow,
    const CharacterStatsModel* stats,
    int slot,
    int y,
    int x,
    int numerator,
    int denominator,
    const IndexedRegion* cached) {
    if (!slot_active(flow, slot)) return;

    const auto state = flow.state();

    // 07E4 special cases use captured images for actor cards and the live
    // stats panel, matching its 0954/0958 paths.
    if (state == CharacterCreationState::ActorChoice ||
        (state == CharacterCreationState::StatsReview && slot == 7)) {
        if (!cached) {
            throw std::logic_error(
                "CODE.1 07E4 cached slot lacks captured region");
        }
        framebuffer_.restore_scaled_horizontal(
            *cached,
            y,
            x,
            numerator,
            denominator);
        return;
    }

    if (state == CharacterCreationState::PortraitChoice) {
        const auto available = flow.available_choices();
        if (!available[static_cast<std::size_t>(slot)]) {
            framebuffer_.draw_frame_scaled_horizontal(
                assets_.type1(7, 0x15),
                0,
                y,
                x,
                numerator,
                denominator);
            return;
        }

        framebuffer_.draw_frame_scaled_horizontal(
            assets_.type1(7, 0x14),
            0,
            y,
            x,
            numerator,
            denominator);

        // b16_range -> CACC uses the table installed by 4B18/CBD4. CACC
        // translates every byte, including the encoded transparent key.
        framebuffer_.draw_frame_scaled_horizontal(
            assets_.type1(
                1,
                336 +
                    flow.draft().sex * 23 +
                    flow.portrait_page() +
                    slot),
            0,
            y + 0x15,
            x,
            numerator,
            denominator,
            &remaps_.luminance[0],
            true);
        return;
    }

    int resource = -1;
    switch (state) {
        case CharacterCreationState::SexChoice:
            resource = 0x0C + slot - 2;
            break;
        case CharacterCreationState::RaceChoice:
            resource = slot;
            break;
        case CharacterCreationState::ClassChoice: {
            const auto available = flow.available_choices();
            resource =
                available[static_cast<std::size_t>(slot)]
                    ? 6 + slot
                    : 0x15;
            break;
        }
        case CharacterCreationState::OptionChoice: {
            const auto available = flow.available_choices();
            resource =
                available[static_cast<std::size_t>(slot)]
                    ? 0x0E + slot
                    : 0x15;
            break;
        }
        case CharacterCreationState::StatsReview:
            resource = slot == 6 ? 0x16 : 0x2E;
            break;
        case CharacterCreationState::ActorChoice:
        case CharacterCreationState::PortraitChoice:
            break;
    }

    if (resource < 0) {
        throw std::logic_error(
            "unsupported CODE.1 07E4 slot resource");
    }

    framebuffer_.draw_frame_scaled_horizontal(
        assets_.type1(7, resource),
        0,
        y,
        x,
        numerator,
        denominator);
}

void CharacterCreationPresenter::transition_07e4_frame(
    const CharacterCreationFlow& before,
    const CharacterCreationFlow& after,
    const CharacterStatsModel* before_stats,
    const CharacterStatsModel* after_stats,
    int first_slot,
    int last_slot,
    int frame_index) {
    if (before.state() != after.state()) {
        throw std::logic_error(
            "CODE.1 07E4 only replaces slots within one controller mode");
    }
    if (first_slot < 0 ||
        first_slot >= last_slot ||
        last_slot > 9) {
        throw std::out_of_range(
            "CODE.1 07E4 slot range");
    }

    const int count =
        transition_07e4_frame_count(first_slot, last_slot);
    if (frame_index < 0 || frame_index >= count) {
        throw std::out_of_range(
            "CODE.1 07E4 transition frame index");
    }

    // After the stagger loop terminates, 07E4 calls 064C once and flips that
    // complete page. Preserve it as a distinct presentation frame.
    if (frame_index == count - 1) {
        screen(after, after_stats);
        return;
    }

    std::array<IndexedRegion, 9> old_region{};
    std::array<IndexedRegion, 9> new_region{};

    screen(before, before_stats);
    for (int slot = first_slot; slot < last_slot; ++slot) {
        old_region[static_cast<std::size_t>(slot)] =
            framebuffer_.capture(
                kSlotRow[static_cast<std::size_t>(slot)],
                kSlotColumn[static_cast<std::size_t>(slot)],
                0x7A,
                0x30);
    }

    // 07E4's optional 0940/0948 records are already the new records while
    // the slot flip runs, so the destination screen is the correct base.
    screen(after, after_stats);
    for (int slot = first_slot; slot < last_slot; ++slot) {
        new_region[static_cast<std::size_t>(slot)] =
            framebuffer_.capture(
                kSlotRow[static_cast<std::size_t>(slot)],
                kSlotColumn[static_cast<std::size_t>(slot)],
                0x7A,
                0x30);
    }

    // function_266C has an intentional asymmetric update when advancing to
    // class substage 1/2. Enabled choices are written to both 08F4 and 08F8
    // before 07E4, so they are already visible (old == new); only choices
    // becoming disabled retain the previous old ID and animate to RES7:15.
    if (before.state() == CharacterCreationState::ClassChoice &&
        after.state() == CharacterCreationState::ClassChoice &&
        after.class_stage() > 0 &&
        after.class_stage() != before.class_stage()) {
        const auto available = after.available_choices();
        for (int slot = first_slot; slot < last_slot && slot < 6; ++slot) {
            if (available[static_cast<std::size_t>(slot)]) {
                old_region[static_cast<std::size_t>(slot)] =
                    new_region[static_cast<std::size_t>(slot)];
            }
        }
    }

    // Runtime 63F8 is the first byte of 4A0C row 4:
    // 5FF8 + 4*100h. It lives in generated BSS, not stored CODE.1 data.
    const auto seam_color = remaps_.bright[4][0];

    for (int slot = first_slot; slot < last_slot; ++slot) {
        const auto index = static_cast<std::size_t>(slot);
        const auto& old_pixels = old_region[index].pixels;
        const auto& new_pixels = new_region[index].pixels;

        // This is the visible equivalent of 07E4's old_id == new_id fast
        // path, and also covers special capture handles with identical pixels.
        if (old_pixels == new_pixels) {
            continue;
        }

        const int row = kSlotRow[index];
        const int column = kSlotColumn[index];
        const int delay = slot - first_slot;
        const int k =
            frame_index < delay
                ? 0
                : frame_index - delay + 1;

        if (k == 0) {
            framebuffer_.restore(old_region[index], row, column);
            continue;
        }

        draw_frame(7, 0x1E + slot, 0, row, column);

        if (k < 6) {
            const int q = 6 - k;
            draw_slot_scaled_07e4(
                before,
                before_stats,
                slot,
                row,
                column + 4 * k,
                q,
                10,
                &old_region[index]);
            continue;
        }

        if (k == 6) {
            // A05C(x,y+18h,x+79h,y+18h,color): one vertical center seam.
            framebuffer_.fill_rect(
                row,
                column + 0x18,
                0x7A,
                1,
                seam_color);
            continue;
        }

        if (k < 12) {
            const int q = 12 - k;
            draw_slot_scaled_07e4(
                after,
                after_stats,
                slot,
                row,
                column + 4 * q,
                q,
                6,
                &new_region[index]);
            continue;
        }

        framebuffer_.restore(new_region[index], row, column);
    }
}

int CharacterCreationPresenter::controller_entry_frame_count(
    const CharacterCreationFlow& flow) const noexcept {
    int active = 0;
    for (int slot = 0; slot < 9; ++slot) {
        if (slot_active(flow, slot)) ++active;
    }
    const auto range = controller_entry_range(flow.state());
    return 2 + active * 14 +
           transition_07e4_frame_count(
               range.first,
               range.second);
}

void CharacterCreationPresenter::controller_entry_frame(
    const CharacterCreationFlow& flow,
    const CharacterStatsModel* stats,
    int frame_index) {
    if (flow.state() == CharacterCreationState::StatsReview &&
        !stats) {
        throw std::logic_error(
            "CODE.1 mode-6 entry requires generated stats");
    }

    std::array<int, 9> active_slots{};
    int active_count = 0;
    for (int slot = 0; slot < 9; ++slot) {
        if (slot_active(flow, slot)) {
            active_slots[static_cast<std::size_t>(active_count++)] =
                slot;
        }
    }

    const int ten_a4_frames = 2 + active_count * 14;
    const auto range = controller_entry_range(flow.state());
    const int seven_e4_frames =
        transition_07e4_frame_count(
            range.first,
            range.second);
    const int total =
        ten_a4_frames + seven_e4_frames;

    if (frame_index < 0 || frame_index >= total) {
        throw std::out_of_range(
            "CODE.1 controller entry frame index");
    }

    if (frame_index < ten_a4_frames) {
        draw_controller_entry_base(flow);

        if (frame_index < 2) {
            return;
        }

        const int local = frame_index - 2;
        const int active_index = local / 14;
        const int phase = local % 14;

        for (int i = 0; i < active_index; ++i) {
            const int slot =
                active_slots[static_cast<std::size_t>(i)];
            draw_frame(
                7,
                0x15,
                0,
                kSlotRow[static_cast<std::size_t>(slot)],
                kSlotColumn[static_cast<std::size_t>(slot)]);
        }

        const int slot =
            active_slots[
                static_cast<std::size_t>(active_index)];

        if (phase >= 12) {
            draw_frame(
                7,
                0x15,
                0,
                kSlotRow[static_cast<std::size_t>(slot)],
                kSlotColumn[static_cast<std::size_t>(slot)]);
            return;
        }

        draw_10a4_ornament(slot, phase);
        return;
    }

    const int phase = frame_index - ten_a4_frames;
    if (phase == seven_e4_frames - 1) {
        screen(flow, stats);
        return;
    }

    // 10A4 has now settled every active slot to RES7:15h. 1464 then calls
    // 07E4 to flip those placeholders into the real controller resources.
    draw_controller_entry_base(flow);
    for (int slot = 0; slot < 9; ++slot) {
        if (!slot_active(flow, slot)) continue;
        draw_frame(
            7,
            0x15,
            0,
            kSlotRow[static_cast<std::size_t>(slot)],
            kSlotColumn[static_cast<std::size_t>(slot)]);
    }
    draw_controller_text_boxes(flow);

    std::array<IndexedRegion, 9> old_region{};
    std::array<IndexedRegion, 9> new_region{};

    for (int slot = range.first; slot < range.second; ++slot) {
        if (!slot_active(flow, slot)) continue;
        old_region[static_cast<std::size_t>(slot)] =
            framebuffer_.capture(
                kSlotRow[static_cast<std::size_t>(slot)],
                kSlotColumn[static_cast<std::size_t>(slot)],
                0x7A,
                0x30);
    }

    screen(flow, stats);
    for (int slot = range.first; slot < range.second; ++slot) {
        if (!slot_active(flow, slot)) continue;
        new_region[static_cast<std::size_t>(slot)] =
            framebuffer_.capture(
                kSlotRow[static_cast<std::size_t>(slot)],
                kSlotColumn[static_cast<std::size_t>(slot)],
                0x7A,
                0x30);
    }

    // Rebuild the visible page that 07E4 starts from.
    draw_controller_entry_base(flow);
    for (int slot = 0; slot < 9; ++slot) {
        if (!slot_active(flow, slot)) continue;
        draw_frame(
            7,
            0x15,
            0,
            kSlotRow[static_cast<std::size_t>(slot)],
            kSlotColumn[static_cast<std::size_t>(slot)]);
    }
    draw_controller_text_boxes(flow);

    const auto seam_color = remaps_.bright[4][0];

    for (int slot = range.first; slot < range.second; ++slot) {
        if (!slot_active(flow, slot)) continue;

        const auto index = static_cast<std::size_t>(slot);
        if (old_region[index].pixels ==
            new_region[index].pixels) {
            continue;
        }

        const int row = kSlotRow[index];
        const int column = kSlotColumn[index];
        const int delay = slot - range.first;
        const int k =
            phase < delay
                ? 0
                : phase - delay + 1;

        if (k == 0) {
            framebuffer_.restore(
                old_region[index],
                row,
                column);
            continue;
        }

        draw_frame(7, 0x1E + slot, 0, row, column);

        if (k < 6) {
            const int q = 6 - k;
            framebuffer_.restore_scaled_horizontal(
                old_region[index],
                row,
                column + 4 * k,
                q,
                10);
            continue;
        }

        if (k == 6) {
            framebuffer_.fill_rect(
                row,
                column + 0x18,
                0x7A,
                1,
                seam_color);
            continue;
        }

        if (k < 12) {
            const int q = 12 - k;
            draw_slot_scaled_07e4(
                flow,
                stats,
                slot,
                row,
                column + 4 * q,
                q,
                6,
                &new_region[index]);
            continue;
        }

        framebuffer_.restore(
            new_region[index],
            row,
            column);
    }
}

void CharacterCreationPresenter::screen(
    const CharacterCreationFlow& flow,
    const CharacterStatsModel* stats,
    int selected_stat,
    bool stats_editor,
    int hovered_choice) {
    static constexpr std::array<int, 9> row{
        13, 67, 91, 91, 67, 13, 91, 91, 91
    };
    static constexpr std::array<int, 9> column{
        3, 56, 109, 162, 215, 268, 83, 136, 189
    };

    const auto state = flow.state();
    const auto& draft = flow.draft();

    switch (state) {
        case CharacterCreationState::ActorChoice:
            draw_controller_base(state, {});
            if (flow.actor_present(0)) {
                draw_actor_card(flow.actor(0), row[2], column[2]);
            } else {
                draw_empty_actor_card(row[2], column[2]);
            }
            if (flow.actor_present(1)) {
                draw_actor_card(flow.actor(1), row[3], column[3]);
            } else {
                draw_empty_actor_card(row[3], column[3]);
            }
            draw_text_box(flow.both_present() ? 1 : 2, 0x114, 0x10C);
            return;

        case CharacterCreationState::SexChoice:
            draw_controller_base(state, assets_.text(0x26));
            draw_frame(7, 0x0C, 0, row[2], column[2]);
            draw_frame(7, 0x0D, 0, row[3], column[3]);
            draw_text_box(8, 0x114, 0x10C);
            return;

        case CharacterCreationState::RaceChoice:
            draw_controller_base(state, assets_.text(0x27));
            draw_persistent_summary(draft, 1);
            for (int i = 0; i < 6; ++i) {
                draw_frame(7, i, 0, row[i], column[i]);
            }
            draw_text_box(9, 0x114, 0x10C);
            return;

        case CharacterCreationState::ClassChoice: {
            draw_controller_base(state, assets_.text(0x28));
            draw_persistent_summary(draft, 3);

            const auto available = flow.available_choices();
            for (int i = 0; i < 6; ++i) {
                draw_frame(
                    7,
                    available[static_cast<std::size_t>(i)] ? 6 + i : 0x15,
                    0,
                    row[i],
                    column[i]);
            }

            if (flow.class_stage() == 0) {
                draw_text_box(0x0A, 0x114, 0x10C);
            } else {
                draw_text_box(0x0B, 0x114, 0x10C);
                draw_text_box(3, 0x114, 0x0D7);
            }
            return;
        }

        case CharacterCreationState::OptionChoice: {
            draw_controller_base(state, assets_.text(0x29));
            draw_persistent_summary(draft, 3);
            const auto available = flow.available_choices();
            for (int i = 0; i < 6; ++i) {
                draw_frame(
                    7,
                    available[static_cast<std::size_t>(i)]
                        ? 0x0E + i
                        : 0x15,
                    0,
                    row[i],
                    column[i]);
            }
            draw_text_box(0x0C, 0x114, 0x10C);
            return;
        }

        case CharacterCreationState::PortraitChoice: {
            draw_controller_base(state, assets_.text(0x2A));
            draw_persistent_summary(draft, 4);

            const auto available = flow.available_choices();
            for (int i = 0; i < 6; ++i) {
                if (!available[static_cast<std::size_t>(i)]) {
                    draw_frame(7, 0x15, 0, row[i], column[i]);
                    continue;
                }

                draw_frame(7, 0x14, 0, row[i], column[i]);
                const auto portrait =
                    assets_.type1(
                        1,
                        336 +
                            draft.sex * 23 +
                            flow.portrait_page() +
                            i);
                if (i == hovered_choice) {
                    framebuffer_.draw_frame(
                        portrait,
                        0,
                        row[i] + 0x15,
                        column[i]);
                } else {
                    // 064C/2A64 render the resting portrait through the
                    // 4B18 luminance table; hover redraws it raw.
                    framebuffer_.draw_frame_mapped(
                        portrait,
                        0,
                        row[i] + 0x15,
                        column[i],
                        remaps_.luminance[0]);
                }
            }

            draw_text_box(0x0D, 0x114, 0x10C);
            draw_text_box(4, 0x114, 0x0D7);
            return;
        }

        case CharacterCreationState::StatsReview: {
            if (!stats) {
                throw std::logic_error(
                    "CODE.1 mode 6 requires generated character stats");
            }

            std::string label;
            if (stats_editor) {
                label = assets_.text(0x2C);
            } else {
                const auto offset = assets_.code1_s32(0x04B8);
                if (offset < 0) {
                    throw std::runtime_error(
                        "CODE.1 mode-6 title pointer is negative");
                }
                label = assets_.code1_cstring(
                    static_cast<std::size_t>(offset));
            }

            draw_controller_base(state, label);
            draw_persistent_summary(draft, 5);

            draw_frame(
                7, stats_editor ? 0x2C : 0x16, 0, row[6], column[6]);
            draw_stats_panel(
                draft,
                *stats,
                row[7],
                column[7],
                selected_stat);
            draw_frame(
                7, stats_editor ? 0x2D : 0x2E, 0, row[8], column[8]);

            draw_text_box(
                stats_editor ? 3 : 0x0E,
                0x114,
                0x10C);
            return;
        }
    }

    throw std::logic_error("unsupported character controller state");
}

void CharacterCreationPresenter::existing_actor_transition_frame(
    const CharacterCreationFlow& before,
    const CharacterCreationFlow& after,
    int actor,
    const CharacterStatsModel& stats,
    bool entering,
    int frame_index) {
    const int flip_frames =
        transition_07e4_frame_count(0, 6);
    const int total =
        existing_actor_transition_frame_count(entering);
    if (frame_index < 0 || frame_index >= total) {
        throw std::out_of_range(
            "CODE.1 1BD4 transition frame index");
    }

    if (entering && frame_index == flip_frames) {
        // 1BD4 draws stage 6 only after the 07E4 slot flip has completed.
        existing_actor_decision_screen(
            before,
            actor,
            stats);
        return;
    }

    if (frame_index == flip_frames - 1) {
        if (entering) {
            screen(before);
            draw_frame(
                7, 0x2F, 0,
                kSlotRow[2], kSlotColumn[2]);
            draw_frame(
                7, 0x30, 0,
                kSlotRow[3], kSlotColumn[3]);
        } else {
            screen(after);
        }
        return;
    }

    std::array<IndexedRegion, 2> old_region{};
    std::array<IndexedRegion, 2> new_region{};

    if (entering) {
        screen(before);
        for (int n = 0; n < 2; ++n) {
            const int slot = 2 + n;
            old_region[static_cast<std::size_t>(n)] =
                framebuffer_.capture(
                    kSlotRow[static_cast<std::size_t>(slot)],
                    kSlotColumn[static_cast<std::size_t>(slot)],
                    0x7A,
                    0x30);
        }

        draw_frame(
            7, 0x2F, 0,
            kSlotRow[2], kSlotColumn[2]);
        draw_frame(
            7, 0x30, 0,
            kSlotRow[3], kSlotColumn[3]);
        for (int n = 0; n < 2; ++n) {
            const int slot = 2 + n;
            new_region[static_cast<std::size_t>(n)] =
                framebuffer_.capture(
                    kSlotRow[static_cast<std::size_t>(slot)],
                    kSlotColumn[static_cast<std::size_t>(slot)],
                    0x7A,
                    0x30);
        }

        // During the flip, mode 0's ordinary lower text record remains
        // active. Stage 6 is painted only after the flip.
        screen(before);
    } else {
        existing_actor_decision_screen(
            before,
            actor,
            stats);
        for (int n = 0; n < 2; ++n) {
            const int slot = 2 + n;
            old_region[static_cast<std::size_t>(n)] =
                framebuffer_.capture(
                    kSlotRow[static_cast<std::size_t>(slot)],
                    kSlotColumn[static_cast<std::size_t>(slot)],
                    0x7A,
                    0x30);
        }

        screen(after);
        for (int n = 0; n < 2; ++n) {
            const int slot = 2 + n;
            new_region[static_cast<std::size_t>(n)] =
                framebuffer_.capture(
                    kSlotRow[static_cast<std::size_t>(slot)],
                    kSlotColumn[static_cast<std::size_t>(slot)],
                    0x7A,
                    0x30);
        }

        // 07E4 phase 0/1 redraws the actor-choice lower record, replacing
        // the temporary stage-6 panel as the buttons flip back.
        screen(after);
    }

    const auto seam_color = remaps_.bright[4][0];

    for (int n = 0; n < 2; ++n) {
        const int slot = 2 + n;
        const auto index = static_cast<std::size_t>(n);
        const int delay = slot;  // first=0 in 1BD4's 07E4(0,6).
        const int k =
            frame_index < delay
                ? 0
                : frame_index - delay + 1;
        const int row =
            kSlotRow[static_cast<std::size_t>(slot)];
        const int column =
            kSlotColumn[static_cast<std::size_t>(slot)];

        if (k == 0) {
            framebuffer_.restore(
                old_region[index],
                row,
                column);
            continue;
        }

        draw_frame(7, 0x1E + slot, 0, row, column);

        if (k < 6) {
            const int q = 6 - k;
            framebuffer_.restore_scaled_horizontal(
                old_region[index],
                row,
                column + 4 * k,
                q,
                10);
            continue;
        }

        if (k == 6) {
            framebuffer_.fill_rect(
                row,
                column + 0x18,
                0x7A,
                1,
                seam_color);
            continue;
        }

        if (k < 12) {
            const int q = 12 - k;
            framebuffer_.restore_scaled_horizontal(
                new_region[index],
                row,
                column + 4 * q,
                q,
                6);
            continue;
        }

        framebuffer_.restore(
            new_region[index],
            row,
            column);
    }
}

void CharacterCreationPresenter::existing_actor_decision_screen(
    const CharacterCreationFlow& flow,
    int actor,
    const CharacterStatsModel& stats) {
    if (flow.state() != CharacterCreationState::ActorChoice ||
        !flow.actor_present(actor)) {
        throw std::logic_error(
            "existing-actor decision requires a populated mode-0 slot");
    }

    static constexpr std::array<int, 9> row{
        13, 67, 91, 91, 67, 13, 91, 91, 91
    };
    static constexpr std::array<int, 9> column{
        3, 56, 109, 162, 215, 268, 83, 136, 189
    };

    // function_1BD4 leaves the mode-0 title blank, substitutes resources
    // 2F/30 into slots 2/3, and invokes function_04B0(stage 6).
    draw_controller_base(CharacterCreationState::ActorChoice, {});
    draw_frame(7, 0x2F, 0, row[2], column[2]);
    draw_frame(7, 0x30, 0, row[3], column[3]);

    const auto& draft = flow.actor(actor);
    draw_persistent_summary(draft, 5);
    draw_stats_panel(draft, stats, 0x114, 0x10C, -1);
}

void CharacterCreationPresenter::stats_editor_transition_frame(
    const CharacterCreationFlow& flow,
    const CharacterStatsModel& stats,
    bool entering,
    int frame_index) {
    const int flip_frames =
        transition_07e4_frame_count(6, 9);
    const int total =
        stats_editor_transition_frame_count(entering);
    if (frame_index < 0 || frame_index >= total) {
        throw std::out_of_range(
            "CODE.1 3C58 transition frame index");
    }

    if (!entering && frame_index == flip_frames) {
        // function_40FC restores the ordinary mode-6 title immediately after
        // 3C58 returns and copies that title region to the visible page.
        screen(flow, &stats, -1, false);
        return;
    }

    if (frame_index == flip_frames - 1) {
        // 07E4's final 064C redraw still carries the editor title when
        // leaving 3C58; entering already uses the editor title.
        screen(flow, &stats, -1, entering ? true : true);
        if (!entering) {
            draw_frame(
                7, 0x16, 0,
                kSlotRow[6], kSlotColumn[6]);
            draw_stats_panel(
                flow.draft(), stats,
                kSlotRow[7], kSlotColumn[7], -1);
            draw_frame(
                7, 0x2E, 0,
                kSlotRow[8], kSlotColumn[8]);
            draw_text_box(0x0E, 0x114, 0x10C);
        }
        return;
    }

    std::array<IndexedRegion, 3> old_region{};
    std::array<IndexedRegion, 3> new_region{};

    screen(
        flow,
        &stats,
        -1,
        entering ? false : true);
    for (int slot = 6; slot < 9; ++slot) {
        old_region[static_cast<std::size_t>(slot - 6)] =
            framebuffer_.capture(
                kSlotRow[static_cast<std::size_t>(slot)],
                kSlotColumn[static_cast<std::size_t>(slot)],
                0x7A,
                0x30);
    }

    screen(
        flow,
        &stats,
        -1,
        entering ? true : false);
    for (int slot = 6; slot < 9; ++slot) {
        new_region[static_cast<std::size_t>(slot - 6)] =
            framebuffer_.capture(
                kSlotRow[static_cast<std::size_t>(slot)],
                kSlotColumn[static_cast<std::size_t>(slot)],
                0x7A,
                0x30);
    }

    // Entering: function_40FC has already painted T[2C] before 3C58 starts.
    // Leaving: T[2C] remains until 3C58 returns, but 07E4 phase 0/1 paints
    // the ordinary mode-6 lower text record.
    screen(flow, &stats, -1, true);
    if (!entering) {
        draw_text_box(0x0E, 0x114, 0x10C);
    }

    const auto seam_color = remaps_.bright[4][0];
    const int phase = frame_index;

    for (int slot = 6; slot < 9; ++slot) {
        const auto index =
            static_cast<std::size_t>(slot - 6);
        if (old_region[index].pixels ==
            new_region[index].pixels) {
            continue;
        }

        const int delay = slot - 6;
        const int k =
            phase < delay
                ? 0
                : phase - delay + 1;
        const int row =
            kSlotRow[static_cast<std::size_t>(slot)];
        const int column =
            kSlotColumn[static_cast<std::size_t>(slot)];

        if (k == 0) {
            framebuffer_.restore(
                old_region[index], row, column);
            continue;
        }

        draw_frame(7, 0x1E + slot, 0, row, column);

        if (k < 6) {
            const int q = 6 - k;
            framebuffer_.restore_scaled_horizontal(
                old_region[index],
                row,
                column + 4 * k,
                q,
                10);
        } else if (k == 6) {
            framebuffer_.fill_rect(
                row,
                column + 0x18,
                0x7A,
                1,
                seam_color);
        } else if (k < 12) {
            const int q = 12 - k;
            framebuffer_.restore_scaled_horizontal(
                new_region[index],
                row,
                column + 4 * q,
                q,
                6);
        } else {
            framebuffer_.restore(
                new_region[index], row, column);
        }
    }
}

void CharacterCreationPresenter::final_decision_entry_frame(
    const CharacterCreationFlow& flow,
    const CharacterStatsModel& stats,
    int frame_index) {
    const int total = final_decision_entry_frame_count();
    if (frame_index < 0 || frame_index >= total) {
        throw std::out_of_range(
            "CODE.1 3138 entry frame index");
    }

    constexpr std::array<int, 2> active{{2, 3}};
    constexpr int ten_a4_frames = 2 + 2 * 14;
    const int flip_frames =
        transition_07e4_frame_count(0, 6);

    if (frame_index < ten_a4_frames) {
        draw_final_decision_base(flow, stats);
        if (frame_index < 2) return;

        const int local = frame_index - 2;
        const int active_index = local / 14;
        const int phase = local % 14;

        for (int i = 0; i < active_index; ++i) {
            const int slot = active[static_cast<std::size_t>(i)];
            draw_frame(
                7, 0x15, 0,
                kSlotRow[static_cast<std::size_t>(slot)],
                kSlotColumn[static_cast<std::size_t>(slot)]);
        }

        const int slot =
            active[static_cast<std::size_t>(active_index)];
        if (phase >= 12) {
            draw_frame(
                7, 0x15, 0,
                kSlotRow[static_cast<std::size_t>(slot)],
                kSlotColumn[static_cast<std::size_t>(slot)]);
        } else {
            draw_10a4_ornament(slot, phase);
        }
        return;
    }

    const int phase = frame_index - ten_a4_frames;
    if (phase == flip_frames - 1) {
        final_decision_screen(flow, stats);
        return;
    }

    draw_final_decision_base(flow, stats);
    for (const int slot : active) {
        draw_frame(
            7, 0x15, 0,
            kSlotRow[static_cast<std::size_t>(slot)],
            kSlotColumn[static_cast<std::size_t>(slot)]);
    }

    std::array<IndexedRegion, 2> old_region{};
    std::array<IndexedRegion, 2> new_region{};
    for (int i = 0; i < 2; ++i) {
        const int slot = active[static_cast<std::size_t>(i)];
        old_region[static_cast<std::size_t>(i)] =
            framebuffer_.capture(
                kSlotRow[static_cast<std::size_t>(slot)],
                kSlotColumn[static_cast<std::size_t>(slot)],
                0x7A,
                0x30);
    }

    final_decision_screen(flow, stats);
    for (int i = 0; i < 2; ++i) {
        const int slot = active[static_cast<std::size_t>(i)];
        new_region[static_cast<std::size_t>(i)] =
            framebuffer_.capture(
                kSlotRow[static_cast<std::size_t>(slot)],
                kSlotColumn[static_cast<std::size_t>(slot)],
                0x7A,
                0x30);
    }

    draw_final_decision_base(flow, stats);
    for (const int slot : active) {
        draw_frame(
            7, 0x15, 0,
            kSlotRow[static_cast<std::size_t>(slot)],
            kSlotColumn[static_cast<std::size_t>(slot)]);
    }

    const auto seam_color = remaps_.bright[4][0];
    for (int i = 0; i < 2; ++i) {
        const int slot = active[static_cast<std::size_t>(i)];
        const auto index = static_cast<std::size_t>(i);
        const int delay = slot;  // 3138's temporary mode 7 uses 07E4(0,6).
        const int k =
            phase < delay
                ? 0
                : phase - delay + 1;
        const int row =
            kSlotRow[static_cast<std::size_t>(slot)];
        const int column =
            kSlotColumn[static_cast<std::size_t>(slot)];

        if (k == 0) {
            framebuffer_.restore(
                old_region[index], row, column);
            continue;
        }

        draw_frame(7, 0x1E + slot, 0, row, column);
        if (k < 6) {
            const int q = 6 - k;
            framebuffer_.restore_scaled_horizontal(
                old_region[index],
                row,
                column + 4 * k,
                q,
                10);
        } else if (k == 6) {
            framebuffer_.fill_rect(
                row,
                column + 0x18,
                0x7A,
                1,
                seam_color);
        } else if (k < 12) {
            const int q = 12 - k;
            framebuffer_.restore_scaled_horizontal(
                new_region[index],
                row,
                column + 4 * q,
                q,
                6);
        } else {
            framebuffer_.restore(
                new_region[index], row, column);
        }
    }
}

void CharacterCreationPresenter::final_decision_dissolve_frame(
    const CharacterCreationFlow& flow,
    const CharacterStatsModel& stats,
    int frame_index) {
    if (frame_index < 0 || frame_index >= 10) {
        throw std::out_of_range(
            "CODE.1 3138 15B0 frame index");
    }

    final_decision_screen(flow, stats);

    if (frame_index < 5) {
        const auto& map =
            remaps_.bright[
                static_cast<std::size_t>(frame_index)];
        for (int slot : {2, 3}) {
            const auto region = framebuffer_.capture(
                kSlotRow[static_cast<std::size_t>(slot)],
                kSlotColumn[static_cast<std::size_t>(slot)],
                0x7A,
                0x30);
            framebuffer_.restore_mapped(
                region,
                kSlotRow[static_cast<std::size_t>(slot)],
                kSlotColumn[static_cast<std::size_t>(slot)],
                map);
        }
        return;
    }

    const auto& map =
        remaps_.bright[
            static_cast<std::size_t>(9 - frame_index)];
    for (int slot : {2, 3}) {
        framebuffer_.draw_frame_mapped(
            assets_.type1(7, 0x1E + slot),
            0,
            kSlotRow[static_cast<std::size_t>(slot)],
            kSlotColumn[static_cast<std::size_t>(slot)],
            map);
    }
}

void CharacterCreationPresenter::name_entry_entry_frame(
    const CharacterCreationFlow& flow,
    const CharacterStatsModel& stats,
    int frame_index) {
    const int total = name_entry_entry_frame_count();
    if (frame_index < 0 || frame_index >= total) {
        throw std::out_of_range(
            "CODE.1 3020 entry frame index");
    }

    constexpr int ten_a4_frames = 2 + 14;
    const int flip_frames =
        transition_07e4_frame_count(6, 9);

    if (frame_index < ten_a4_frames) {
        draw_name_entry_base(flow, stats);
        if (frame_index < 2) return;

        const int phase = (frame_index - 2) % 14;
        if (phase >= 12) {
            draw_frame(
                7, 0x15, 0,
                kSlotRow[7], kSlotColumn[7]);
        } else {
            draw_10a4_ornament(7, phase);
        }
        return;
    }

    const int phase = frame_index - ten_a4_frames;
    if (phase >= flip_frames) {
        // 3020 explicitly draws RES7:14 and flips once more after 1464.
        name_entry_screen(flow, stats, {}, false);
        return;
    }

    if (phase == flip_frames - 1) {
        name_entry_screen(flow, stats, {}, false);
        return;
    }

    draw_name_entry_base(flow, stats);
    draw_frame(
        7, 0x15, 0,
        kSlotRow[7], kSlotColumn[7]);
    const auto old_region = framebuffer_.capture(
        kSlotRow[7], kSlotColumn[7], 0x7A, 0x30);

    name_entry_screen(flow, stats, {}, false);
    const auto new_region = framebuffer_.capture(
        kSlotRow[7], kSlotColumn[7], 0x7A, 0x30);

    draw_name_entry_base(flow, stats);
    draw_frame(
        7, 0x15, 0,
        kSlotRow[7], kSlotColumn[7]);

    // 3020 runs under mode 6, so 07E4 covers slots 6..8. Slot 7 is delayed
    // one phase even though slots 6/8 have new_id == -1.
    const int k =
        phase < 1
            ? 0
            : phase;
    const int row = kSlotRow[7];
    const int column = kSlotColumn[7];
    const auto seam_color = remaps_.bright[4][0];

    if (k == 0) {
        framebuffer_.restore(old_region, row, column);
    } else {
        draw_frame(7, 0x1E + 7, 0, row, column);
        if (k < 6) {
            const int q = 6 - k;
            framebuffer_.restore_scaled_horizontal(
                old_region,
                row,
                column + 4 * k,
                q,
                10);
        } else if (k == 6) {
            framebuffer_.fill_rect(
                row,
                column + 0x18,
                0x7A,
                1,
                seam_color);
        } else if (k < 12) {
            const int q = 12 - k;
            framebuffer_.restore_scaled_horizontal(
                new_region,
                row,
                column + 4 * q,
                q,
                6);
        } else {
            framebuffer_.restore(new_region, row, column);
        }
    }
}

void CharacterCreationPresenter::name_entry_dissolve_frame(
    const CharacterCreationFlow& flow,
    const CharacterStatsModel& stats,
    const std::string& name,
    int frame_index) {
    if (frame_index < 0 || frame_index >= 10) {
        throw std::out_of_range(
            "CODE.1 3020 15B0 frame index");
    }

    name_entry_screen(flow, stats, name, false);

    if (frame_index < 5) {
        const auto region = framebuffer_.capture(
            kSlotRow[7], kSlotColumn[7], 0x7A, 0x30);
        framebuffer_.restore_mapped(
            region,
            kSlotRow[7],
            kSlotColumn[7],
            remaps_.bright[
                static_cast<std::size_t>(frame_index)]);
        return;
    }

    framebuffer_.draw_frame_mapped(
        assets_.type1(7, 0x1E + 7),
        0,
        kSlotRow[7],
        kSlotColumn[7],
        remaps_.bright[
            static_cast<std::size_t>(9 - frame_index)]);
}

void CharacterCreationPresenter::final_decision_screen(
    const CharacterCreationFlow& flow,
    const CharacterStatsModel& stats) {
    if (flow.state() != CharacterCreationState::StatsReview) {
        throw std::logic_error(
            "final decision is only valid from CODE.1 mode 6");
    }

    static constexpr std::array<int, 9> row{
        13, 67, 91, 91, 67, 13, 91, 91, 91
    };
    static constexpr std::array<int, 9> column{
        3, 56, 109, 162, 215, 268, 83, 136, 189
    };

    // function_3138 temporarily presents mode 7 with arg=6:
    // persistent summary through the stats panel, plus RES7:2F/30.
    draw_controller_base(
        CharacterCreationState::StatsReview,
        assets_.text(0x2D));
    draw_persistent_summary(flow.draft(), 5);
    draw_stats_panel(
        flow.draft(), stats, 0x114, 0x10C, -1);

    draw_frame(7, 0x2F, 0, row[2], column[2]);
    draw_frame(7, 0x30, 0, row[3], column[3]);
}

void CharacterCreationPresenter::name_entry_screen(
    const CharacterCreationFlow& flow,
    const CharacterStatsModel& stats,
    const std::string& name,
    bool cursor_on) {
    if (flow.state() != CharacterCreationState::StatsReview) {
        throw std::logic_error(
            "name entry is only valid from CODE.1 mode 6");
    }

    // function_3020: mode-6 staged summary, T[46] title, and the center
    // RES7:14 card at 5Bh/88h.
    draw_controller_base(
        CharacterCreationState::StatsReview,
        assets_.text(0x2E));
    draw_persistent_summary(flow.draft(), 5);
    draw_stats_panel(
        flow.draft(), stats, 0x114, 0x10C, -1);

    draw_frame(7, 0x14, 0, 0x5B, 0x88);
    draw_text_centered(0x93, 0x88, 0x30, name, 0);

    if (cursor_on) {
        const auto font = assets_.res0_font(0);
        int width = 0;
        for (const unsigned char ch : name) {
            if (ch >= font.widths.size()) {
                throw std::runtime_error(
                    "name-entry glyph exceeds CODE.1 font table");
            }
            width += font.widths[ch];
        }

        const int start = 0x88 + (0x30 - width) / 2;
        framebuffer_.fill_rect(
            0x93,
            start + width,
            0x0A,
            1,
            0x5F);
    }
}

void CharacterCreationPresenter::screen(CharacterCreationState state) {
    CharacterCreationFlow flow;
    if (state != CharacterCreationState::ActorChoice) {
        throw std::logic_error(
            "state-only character screen is valid only for actor choice");
    }
    screen(flow);
}

void CharacterCreationPresenter::first_screen() {
    screen(CharacterCreationState::ActorChoice);
}

}  // namespace rloft
