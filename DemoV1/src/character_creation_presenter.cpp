#include "rloft/character_creation_presenter.hpp"

#include <array>
#include <cstddef>
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

void CharacterCreationPresenter::draw_text_centered(
    int y, int left, int span, const std::string& text, int font_id) {
    if (text.empty()) return;

    const auto font = assets_.res0_font(font_id);
    int width = 0;
    for (const unsigned char ch : text) {
        if (ch >= font.widths.size()) {
            throw std::runtime_error("CODE.1 text uses glyph outside RES0 font table");
        }
        width += font.widths[ch];
    }

    int x = left + (span - width) / 2;
    for (const unsigned char ch : text) {
        if (ch >= font.raster.count) {
            throw std::runtime_error("CODE.1 font raster is missing requested glyph");
        }
        framebuffer_.draw_frame(font.raster, ch, y, x);
        x += font.widths[ch];
    }
}

void CharacterCreationPresenter::draw_empty_actor_card(int y, int x) {
    // function_1A04 -> function_004C(T[0]) -> function_00EC mode 0.
    // Row 0 of the five palette-remap tables is color-equivalent to the
    // original palette, so the RES7:1A background can be drawn directly.
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

void CharacterCreationPresenter::first_screen() {
    framebuffer_.clear(0);

    // CODE.1 presentation routine 0x10A4, mode/state 0.
    draw_strip(7, 0x18, 0, 0);
    draw_strip(7, 0x19, 200, 0);
    draw_frame(7, 0x2A, 0, 5, 0);

    // Exact CODE.1 DGROUP coordinate pairs. The first coordinate is the
    // 0..399 screen row and the second is the 0..319 screen column.
    static constexpr std::array<int, 9> y{
        13, 67, 91, 91, 67, 13, 91, 91, 91
    };
    static constexpr std::array<int, 9> x{
        3, 56, 109, 162, 215, 268, 83, 136, 189
    };

    for (int i = 0; i < 9; ++i) {
        draw_frame(7, 0x1E + i, 0, y[i], x[i]);
    }

    // State 0 uses transient 095C/0960 handles for slots 2 and 3. On a fresh
    // character-generation session both handles are identical runtime-built
    // empty cards generated from T[0], RES0 font 0 and RES7:1A.
    draw_empty_actor_card(y[2], x[2]);
    draw_empty_actor_card(y[3], x[3]);
}

}  // namespace rloft
