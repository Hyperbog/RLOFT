#include "rloft/quit_confirm.hpp"

#include <stdexcept>

namespace rloft {

std::uint32_t Code2Random15::next() noexcept {
    // CODE.2 30CA2, state at DGROUP F6F4.
    state_ =
        state_ * 0x41C64E6DU +
        0x3039U;
    return (state_ >> 16U) & 0x7FFFU;
}

QuitConfirmChoice QuitConfirmModel::choice_at(
    int row,
    int column) noexcept {
    // CODE.2 07D44 reads the vertical pointer axis first and accepts both
    // C8h and E0h endpoints.
    if (row < 0xC8 || row > 0xE0) {
        return QuitConfirmChoice::None;
    }

    if (column >= 0x60 && column < 0x90) {
        return QuitConfirmChoice::Cancel;
    }
    if (column >= 0xB0 && column < 0xE0) {
        return QuitConfirmChoice::Confirm;
    }
    return QuitConfirmChoice::None;
}

std::vector<QuitConfirmTile>
QuitConfirmModel::background_tiles(
    std::uint32_t seed) {
    Code2Random15 random(seed);
    std::vector<QuitConfirmTile> out;
    out.reserve(232);

    // Interior: five 16-pixel rows by 32 eight-pixel columns.
    for (int row = 0xA0; row < 0xF0; row += 0x10) {
        for (int column = 0x20;
             column < 0x120;
             column += 8) {
            out.push_back(
                QuitConfirmTile{
                    static_cast<int>(
                        (random.next() * 4U) >> 15U),
                    row,
                    column,
                });
        }
    }

    // Top and bottom edges. CODE.2 consumes one RNG value for the top and
    // then one for the bottom at each column.
    for (int column = 0x20;
         column < 0x118;
         column += 8) {
        out.push_back(
            QuitConfirmTile{
                static_cast<int>(
                    ((random.next() * 2U) >> 15U) +
                    9U),
                0xA0,
                column,
            });
        out.push_back(
            QuitConfirmTile{
                static_cast<int>(
                    ((random.next() * 2U) >> 15U) +
                    5U),
                0xE0,
                column,
            });
    }

    // Left/right edges.
    for (int row = 0xB0;
         row < 0xE0;
         row += 0x10) {
        out.push_back(
            QuitConfirmTile{
                static_cast<int>(
                    ((random.next() * 2U) >> 15U) +
                    0x0CU),
                row,
                0x20,
            });
        out.push_back(
            QuitConfirmTile{
                static_cast<int>(
                    ((random.next() * 2U) >> 15U) +
                    0x0EU),
                row,
                0x118,
            });
    }

    // Fixed corners overwrite the randomized edge cells exactly as 07D44.
    out.push_back(QuitConfirmTile{8, 0xA0, 0x20});
    out.push_back(QuitConfirmTile{0x0B, 0xA0, 0x118});
    out.push_back(QuitConfirmTile{4, 0xE0, 0x20});
    out.push_back(QuitConfirmTile{7, 0xE0, 0x118});

    return out;
}

void QuitConfirmPresenter::draw(
    IndexedFramebuffer& framebuffer,
    std::uint32_t rng_seed,
    QuitConfirmChoice pressed) const {
    const auto base =
        assets_.init_u16(0x00);
    const auto background =
        assets_.res0_type1(
            QuitConfirmModel::background_resource_id(
                base));
    const auto buttons =
        assets_.res0_type1(
            QuitConfirmModel::button_resource_id(
                base));

    if (background.count < 16) {
        throw std::runtime_error(
            "quit confirmation background lacks required frames");
    }
    if (buttons.count < 4) {
        throw std::runtime_error(
            "quit confirmation buttons lack required frames");
    }

    for (const auto& tile :
         QuitConfirmModel::background_tiles(
             rng_seed)) {
        framebuffer.draw_frame(
            background,
            tile.frame,
            tile.row,
            tile.column);
    }

    IndexedFontPresenter::draw_centered(
        framebuffer,
        font1_,
        0xAA,
        0x20,
        0x100,
        QuitConfirmModel::prompt());

    framebuffer.draw_frame(
        buttons,
        pressed == QuitConfirmChoice::Cancel
            ? 1
            : 0,
        QuitConfirmModel::kButtonRow,
        QuitConfirmModel::kCancelColumn);
    framebuffer.draw_frame(
        buttons,
        pressed == QuitConfirmChoice::Confirm
            ? 3
            : 2,
        QuitConfirmModel::kButtonRow,
        QuitConfirmModel::kConfirmColumn);
}

}  // namespace rloft
