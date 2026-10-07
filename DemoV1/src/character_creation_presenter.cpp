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

void CharacterCreationPresenter::screen(
    const CharacterCreationFlow& flow,
    const CharacterStatsModel* stats,
    int selected_stat,
    bool stats_editor) {
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
            draw_empty_actor_card(row[2], column[2]);
            draw_empty_actor_card(row[3], column[3]);
            draw_text_box(2, 0x114, 0x10C);
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
                draw_frame(
                    1,
                    336 + draft.sex * 23 + flow.portrait_page() + i,
                    0,
                    row[i] + 0x15,
                    column[i]);
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
