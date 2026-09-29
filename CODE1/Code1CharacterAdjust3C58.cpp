#include "games/ravenloft/game/Code1CharacterAdjust3C58.h"

#include "games/ravenloft/game/Code1CharacterNormalize3404.h"
#include "games/ravenloft/game/Code1CharacterSheet38A4.h"

#include <cstddef>
#include <cstdint>

namespace ravenloft::game {
namespace {

void normalize(Code1CharacterAdjustState3C58& state) {
    Code1CharacterNormalizeState3404 n{};
    n.field680C = state.field680C;
    n.fields680D = state.fields680D;
    n.field6813 = state.field6813;
    n.word6816 = state.word6816;
    n.words6818 = state.words6818;
    n.bytes682A = state.bytes682A;
    n.global7118 = state.global7118;
    n.global711C = state.global711C;
    Code1CharacterNormalize3404::apply(n);
    state.field6813 = n.field6813;
    state.word6816 = n.word6816;
    state.bytes682A = n.bytes682A;
    state.global7118 = n.global7118;
    state.global711C = n.global711C;
}

void drawSheet(std::int32_t x,
               std::int32_t y,
               Code1CharacterAdjustState3C58& state,
               const ravenloft::data::TextDatabase& text,
               const Code1CharacterAdjustOps3C58& ops) {
    Code1CharacterSheetState38A4 sheet{};
    sheet.fields680D = state.fields680D;
    sheet.word6810 = state.word6810;
    sheet.field6813 = state.field6813;
    sheet.word6816 = state.word6816;
    sheet.words6818 = state.words6818;
    sheet.bytes682A = state.bytes682A;
    sheet.selectedRow7124 = state.selectedRow7124;
    sheet.control59F8 = state.control59F8;

    Code1CharacterSheet38A4::draw(x, y, sheet, text, ops.sheet);
    state.control59F8 = sheet.control59F8;
}

void redrawAndWait(Code1CharacterAdjustState3C58& state,
                   const ravenloft::data::TextDatabase& text,
                   const Code1CharacterAdjustOps3C58& ops) {
    ops.call6588(state.buffer7138);
    drawSheet(0x5B, 0x88, state, text, ops);
    ops.call55DC(state.buffer7138, 0x5B, 0x88, 0x7A, 0x30);
    while (ops.call53B4() != 0) {
    }
}

bool primaryAllowsExceptional(
    const Code1CharacterAdjustState3C58& state) noexcept {
    return state.fields680D[0] == 0u ||
           state.fields680D[0] == 1u ||
           state.fields680D[0] == 2u;
}

void increaseSelected(Code1CharacterAdjustState3C58& state,
                      std::int32_t mouseButtons) {
    const std::int32_t row = state.selectedRow7124;

    if (row < 6) {
        auto& value = state.bytes682A[static_cast<std::size_t>(row) * 2u];

        if (row == 0) {
            if ((mouseButtons & 1) != 0) {
                if (value == 0x12u) {
                    if (primaryAllowsExceptional(state)) {
                        ++state.bytes682A[12];
                    }
                } else {
                    ++value;
                }
            } else {
                value = 0x14u;
                state.bytes682A[12] = 0x64u;
            }
        } else {
            if ((mouseButtons & 1) != 0) {
                ++value;
            } else {
                value = 0x14u;
            }
        }
    } else if (row == 0x0A) {
        if ((mouseButtons & 1) != 0) {
            ++state.word6816;
        } else {
            state.word6816 = 0x03E8u;
        }
    }
}

void decreaseSelected(Code1CharacterAdjustState3C58& state,
                      std::int32_t mouseButtons) {
    const std::int32_t row = state.selectedRow7124;

    if (row < 6) {
        auto& value = state.bytes682A[static_cast<std::size_t>(row) * 2u];

        if (row == 0) {
            if ((mouseButtons & 1) != 0) {
                if (value == 0x12u && state.bytes682A[12] != 1u) {
                    --state.bytes682A[12];
                } else {
                    --value;
                }
            } else {
                value = 0u;
                state.bytes682A[12] = 0u;
            }
        } else {
            if ((mouseButtons & 1) != 0) {
                --value;
            } else {
                value = 0u;
            }
        }
    } else if (row == 0x0A) {
        if ((mouseButtons & 1) != 0) {
            --state.word6816;
        } else {
            state.word6816 = 0u;
        }
    }
}

} // namespace

void Code1CharacterAdjust3C58::run(
    Code1CharacterAdjustState3C58& state,
    const ravenloft::data::TextDatabase& text,
    const Code1CharacterAdjustOps3C58& ops) {
    // 0x3C5D..0x3CA5: transition into the adjustment page.
    state.value924 = state.value928;
    state.value92C = state.value930;
    state.value934 = state.value938;
    state.value928 = 0x2C;
    state.value938 = 0x2D;
    state.value940 = 3;
    ops.call07E4(6, 9);
    ops.call56DC();

    state.selectedRow7124 = -1;
    bool finished = false;

    while (!finished) {
        const Code1AdjustmentInput3C58 input = ops.call043C();
        const std::int32_t selection = input.selection;

        if (selection == 0) {
            finished = true;
            continue;
        }

        if (selection == 7) {
            if (state.selectedRow7124 == -1) continue;
            increaseSelected(state, input.mouseButtons4D4);
            normalize(state);
            redrawAndWait(state, text, ops);
            continue;
        }

        if (selection == 8) {
            const std::int32_t row =
                (static_cast<std::int32_t>(input.mouseX73E6) - 0x60) / 10;
            if ((row >= 0 && row < 6) || row == 0x0A) {
                state.selectedRow7124 = row;
            } else {
                state.selectedRow7124 = -1;
            }
            redrawAndWait(state, text, ops);
            continue;
        }

        if (selection == 9) {
            if (state.selectedRow7124 == -1) continue;
            decreaseSelected(state, input.mouseButtons4D4);
            normalize(state);
            redrawAndWait(state, text, ops);
            continue;
        }
    }

    // 0x3FCC..0x40A9: rebuild the two cached 122x48 sheet images while
    // preserving/restoring the original pixels underneath the scratch area.
    state.selectedRow7124 = -1;
    ops.call6588(state.buffer7138);

    const std::uintptr_t saved =
        ops.call7868(0, 0, 0x7A, 0x30, 0xFF);
    ops.call9EB0(0, 0, 0x7A, 0x30, 0xFE);
    drawSheet(0, 0, state, text, ops);

    if (state.handle954 != 0) {
        ops.call7830(state.handle954);
        state.handle954 = 0;
    }
    if (state.handle958 != 0) {
        ops.call7830(state.handle958);
        state.handle958 = 0;
    }

    state.handle958 = ops.call7868(0, 0, 0x7A, 0x30, 0xFF);
    state.handle954 = ops.call7868(0, 0, 0x7A, 0x30, 0xFF);

    ops.call67C8(0, 0, 0, saved);
    ops.call7830(saved);

    // 0x40AC..0x40F1: transition back to the normal final-sheet page.
    state.value924 = state.value928;
    state.value92C = state.value930;
    state.value934 = state.value938;
    state.value928 = 0x16;
    state.value938 = 0x2E;
    state.value940 = 0x0E;
    ops.call07E4(6, 9);
}

} // namespace ravenloft::game
