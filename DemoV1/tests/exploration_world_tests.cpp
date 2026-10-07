#include "rloft/exploration_world.hpp"

#include <cassert>
#include <cstdint>

namespace {

void put16(std::array<std::uint8_t, 0x2A4>& b,
           std::size_t o,
           std::uint16_t v) {
    b[o] = static_cast<std::uint8_t>(v);
    b[o + 1] = static_cast<std::uint8_t>(v >> 8U);
}

rloft::LevelState make_level() {
    rloft::LevelState level{};
    level.width = 5;
    level.height = 5;
    level.environment_profile = 0;
    level.cell_words.assign(25, 0);
    level.automap_cache.assign(25, 0xFF);

    // Cell code 1 is the special 0x0093 primary-resource shortcut.
    put16(level.header, 0x00A + 2, 0x0093);
    return level;
}

}  // namespace

int main() {
    using rloft::CellPosition;
    using rloft::ExplorationWorldModel;

    auto level = make_level();

    rloft::ExplorationSessionState session{};
    session.coordinate_a = 0x00048000U;
    session.coordinate_b = 0x00038000U;
    assert((ExplorationWorldModel::party_cell(session) == CellPosition{4, 3}));

    ExplorationWorldModel::mark_visible_and_explored(level, {2, 2});
    assert((ExplorationWorldModel::cell_word(level, {2, 2}) & 0x0440U) == 0x0440U);
    ExplorationWorldModel::clear_current_visibility(level);
    assert((ExplorationWorldModel::cell_word(level, {2, 2}) & 0x0040U) == 0);
    assert((ExplorationWorldModel::cell_word(level, {2, 2}) & 0x0400U) != 0);

    // Explored empty/floor.
    assert(ExplorationWorldModel::classify_automap_cell(level, {2, 2}) == 0x2E);

    // Unexplored code-1 still follows the certified 0x0093 shortcut.
    level.cell_words[ExplorationWorldModel::cell_index(level, {0, 0})] = 0x0001;
    assert(ExplorationWorldModel::classify_automap_cell(level, {0, 0}) == 0x2E);

    // An unexplored ordinary wall adjacent to an explored raw-code-zero cell
    // becomes boundary class 1.
    level.cell_words[ExplorationWorldModel::cell_index(level, {1, 2})] = 0x0002;
    assert(ExplorationWorldModel::classify_automap_cell(level, {1, 2}) == 0x01);

    // Explored hidden/secret topology.
    level.cell_words[ExplorationWorldModel::cell_index(level, {3, 2})] =
        0x0400U | 0x0800U | 0x0002U;
    assert(ExplorationWorldModel::classify_automap_cell(level, {3, 2}) == 0x05);

    // Special center with no record is inset class 3.
    level.cell_words[ExplorationWorldModel::cell_index(level, {3, 3})] =
        0x0400U | 0x0080U | 0x0002U;
    assert(ExplorationWorldModel::classify_automap_cell(level, {3, 3}) == 0x03);

    // Any record changes that path; no dynamic group-2 match yields class 4.
    rloft::CellControlRecord static_record{};
    static_record.raw[0] = 8;
    static_record.raw[4] = 3;
    static_record.raw[5] = 3;
    level.group1.push_back(static_record);
    assert(ExplorationWorldModel::classify_automap_cell(level, {3, 3}) == 0x04);

    // Real group-2 state 0 is open/passable and maps to class 2.
    rloft::CellControlRecord dynamic_record{};
    dynamic_record.raw[0] = 1;
    dynamic_record.raw[4] = 3;
    dynamic_record.raw[5] = 3;
    dynamic_record.raw[6] = 0;
    level.group2.push_back(dynamic_record);
    assert(ExplorationWorldModel::classify_automap_cell(level, {3, 3}) == 0x02);

    // Endpoint 8 returns 0x8000 and therefore class 4.
    level.group2.back().raw[6] = 8;
    assert(ExplorationWorldModel::classify_automap_cell(level, {3, 3}) == 0x04);

    // Party cell override uses environment-specific floor class.
    assert(ExplorationWorldModel::classify_automap_cell(
               level, {4, 4}, CellPosition{4, 4}) == 0x2E);
    level.environment_profile = 0xFF;
    assert(ExplorationWorldModel::classify_automap_cell(
               level, {4, 4}, CellPosition{4, 4}) == 0x2F);

    ExplorationWorldModel::rebuild_automap_cache(level, CellPosition{4, 4});
    assert(level.automap_cache.size() == 25);
    assert(level.automap_cache[
               ExplorationWorldModel::cell_index(level, {4, 4})] == 0x2F);
}
