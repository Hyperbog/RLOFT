#include "Code1CharacterFinalize40FC.h"

#include "Code1CharacterInit3884.h"
#include "Code1CharacterSheet38A4.h"

#include <array>
#include <cstddef>

namespace ravenloft::game {
namespace {

Code1CharacterSheetState38A4 makeSheetState(
    const Code1CharacterFinalizeState40FC& state) {
    Code1CharacterSheetState38A4 sheet{};
    sheet.fields680D = state.character.fields680D;
    sheet.word6810 = state.character.word6810;
    sheet.field6813 = state.character.field6813;
    sheet.word6816 = state.character.word6816;
    sheet.words6818 = state.character.words6818;
    sheet.bytes682A = state.character.bytes682A;
    sheet.selectedRow7124 = state.selectedRow7124;
    sheet.control59F8 = state.control59F8;
    return sheet;
}

void drawCharacterSheet(Code1CharacterFinalizeState40FC& state,
                        const Code1TextDatabase& text,
                        const Code1CharacterFinalizeOps40FC& ops) {
    auto sheet = makeSheetState(state);
    Code1CharacterSheet38A4::draw(0, 0, sheet, text, ops.sheet);
    state.control59F8 = sheet.control59F8;
}

std::uintptr_t rebuildSheetImage(
    Code1CharacterFinalizeState40FC& state,
    const Code1TextDatabase& text,
    const Code1CharacterFinalizeOps40FC& ops) {
    ops.call6588(state.buffer7138);
    const std::uintptr_t saved =
        ops.call7868(0, 0, 0x7A, 0x30, 0xFF);

    ops.call9EB0(0, 0, 0x7A, 0x30, 0xFE);
    drawCharacterSheet(state, text, ops);

    const std::uintptr_t result =
        ops.call7868(0, 0, 0x7A, 0x30, 0xFF);

    ops.call67C8(0, 0, 0, saved);
    ops.call7830(saved);
    return result;
}

void drawFinalizePage(Code1CharacterFinalizeState40FC& state,
                      const Code1CharacterFinalizeOps40FC& ops) {
    ops.call6588(state.buffer7138);
    ops.call67C8(0, 0x5B, 0x88, state.handle958);
    ops.call6C60(0, 0x5B, 0xBD, 0x2E, 7);
    ops.call53E4(state.buffer7138);
    state.buffer7138 ^= 1u;
}

void redrawBothPages(Code1CharacterFinalizeState40FC& state,
                     const Code1CharacterFinalizeOps40FC& ops) {
    drawFinalizePage(state, ops);
    drawFinalizePage(state, ops);
}

void showAdjustmentBanner(Code1CharacterFinalizeState40FC& state,
                          const Code1CharacterFinalizeOps40FC& ops) {
    ops.call6588(state.buffer7138);
    ops.call6C60(0, 5, 0, 0x2A, 7);
    ops.call7D80(5, 0, 0x140, state.text7140.c_str(), 1);

    ops.call5780();
    ops.callA3E8(static_cast<std::uint8_t>(state.buffer7138 ^ 1u),
                 5, 0, 5, 0, 0x14, 0x140);
    ops.call57DC();
}

} // namespace

void Code1CharacterFinalize40FC::run(
    Code1CharacterFinalizeState40FC& state,
    Code1RuntimeRng618A& rng,
    const Code1TextDatabase& text,
    const Code1CharacterFinalizeOps40FC& ops) {
    // 0x4100..0x410B: seed the original CODE.1 RNG from the original timer.
    rng.seed(ops.call7F50());

    // 0x410E.
    Code1CharacterInit3884::initialize(state.character, rng);

    // 0x4113.
    state.selectedRow7124 = -1;

    // 0x411D..0x4194.
    state.handle958 = rebuildSheetImage(state, text, ops);

    // 0x4197..0x41B6.
    const std::array<std::int32_t, 9> slots{
        -1, -1, -1, -1, -1, -1, 0x16, 0x02, 0x2E
    };
    ops.call1464(5, slots, 0x0E, -1, 0x2B);

    // 0x41B9..0x4254.
    redrawBothPages(state, ops);

    for (;;) {
        // 0x425B..0x4264.
        const std::int32_t selection = ops.call043C();

        if (selection == 0) {
            // 0x4266..0x4281.
            ops.call15B0(-1, -1, -1, -1);
            --state.mode4D0;
            break;
        }

        if (selection == 7) {
            // 0x428F..0x4360: reroll/rebuild and return to the same page.
            if (state.handle954 != 0) {
                ops.call7830(state.handle954);
            }
            state.handle954 = state.handle958;

            Code1CharacterInit3884::initialize(state.character, rng);
            state.handle958 = rebuildSheetImage(state, text, ops);

            state.value924 = state.value928;
            state.value934 = state.value938;
            state.value92C = state.value930;
            state.value930 ^= 1;

            ops.call07E4(7, 8);
            redrawBothPages(state, ops);
            continue;
        }

        if (selection == 8) {
            // 0x4365..0x438A.
            ops.call15B0(-1, -1, -1, -1);
            ops.call3138();
            ++state.mode4D0;
            break;
        }

        if (selection == 9) {
            // 0x4398..0x4480: show record 0x2C, enter the adjustment loop,
            // restore DS:04B8 text, then repaint the banner.
            state.text7140 = text.at(0x2C);
            showAdjustmentBanner(state, ops);

            ops.call3C58();

            state.text7140 = state.string4B8;
            showAdjustmentBanner(state, ops);

            redrawBothPages(state, ops);
            continue;
        }
    }

    // 0x4499..0x44DA.
    if (state.handle954 != 0) {
        ops.call7830(state.handle954);
        state.handle954 = 0;
    }
    if (state.handle958 != 0) {
        ops.call7830(state.handle958);
        state.handle958 = 0;
    }
}

} // namespace ravenloft::game
