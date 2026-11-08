#include "rloft/quit_confirm.hpp"

#include <cassert>

int main() {
    rloft::Code2Random15 random(1);
    assert(random.next() == 16838U);
    assert(random.next() == 5758U);
    assert(random.next() == 10113U);

    assert(
        rloft::QuitConfirmModel::choice_at(
            0xC8, 0x60) ==
        rloft::QuitConfirmChoice::Cancel);
    assert(
        rloft::QuitConfirmModel::choice_at(
            0xE0, 0x8F) ==
        rloft::QuitConfirmChoice::Cancel);
    assert(
        rloft::QuitConfirmModel::choice_at(
            0xC8, 0xB0) ==
        rloft::QuitConfirmChoice::Confirm);
    assert(
        rloft::QuitConfirmModel::choice_at(
            0xE0, 0xDF) ==
        rloft::QuitConfirmChoice::Confirm);
    assert(
        rloft::QuitConfirmModel::choice_at(
            0xC7, 0x60) ==
        rloft::QuitConfirmChoice::None);
    assert(
        rloft::QuitConfirmModel::choice_at(
            0xE1, 0xB0) ==
        rloft::QuitConfirmChoice::None);
    assert(
        rloft::QuitConfirmModel::choice_at(
            0xD0, 0x90) ==
        rloft::QuitConfirmChoice::None);

    const auto tiles =
        rloft::QuitConfirmModel::background_tiles(1);
    assert(tiles.size() == 232U);

    // First two interior choices use the exact 30CA2 sequence.
    assert(tiles[0].row == 0xA0);
    assert(tiles[0].column == 0x20);
    assert(tiles[0].frame == 2);
    assert(tiles[1].column == 0x28);
    assert(tiles[1].frame == 0);

    // Fixed corner overwrite calls are the final four draws.
    const auto n = tiles.size();
    assert(tiles[n - 4].frame == 8);
    assert(tiles[n - 4].row == 0xA0);
    assert(tiles[n - 4].column == 0x20);
    assert(tiles[n - 3].frame == 0x0B);
    assert(tiles[n - 3].column == 0x118);
    assert(tiles[n - 2].frame == 4);
    assert(tiles[n - 2].row == 0xE0);
    assert(tiles[n - 1].frame == 7);
    assert(tiles[n - 1].row == 0xE0);
    assert(tiles[n - 1].column == 0x118);

    assert(
        rloft::QuitConfirmModel::prompt() ==
        "do you wish to quit?");
}
