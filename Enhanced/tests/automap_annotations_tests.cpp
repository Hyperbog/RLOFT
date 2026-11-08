#include "rloft/automap_annotations.hpp"

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace {
void put16(std::vector<std::uint8_t>& b,
           std::size_t o,
           std::uint16_t v) {
    b[o] = static_cast<std::uint8_t>(v);
    b[o + 1U] = static_cast<std::uint8_t>(v >> 8U);
}
}  // namespace

int main() {
    using rloft::AutomapAnnotationModel;
    using rloft::CellPosition;

    std::vector<std::uint8_t> blob(17, 0);
    std::size_t p = 0;
    put16(blob, p + 0, 16);
    put16(blob, p + 2, 10);
    put16(blob, p + 4, 3);
    p += 6;
    blob[p++] = 'O'; blob[p++] = 'N'; blob[p++] = 'E';

    put16(blob, p + 0, static_cast<std::uint16_t>(-9));
    put16(blob, p + 2, static_cast<std::uint16_t>(-6));
    put16(blob, p + 4, 2);
    p += 6;
    blob[p++] = 'T'; blob[p++] = 'W';

    const auto annotations = AutomapAnnotationModel::parse(blob);
    assert(annotations.size() == 2);
    assert((annotations[0].map_cell() == CellPosition{2, 2}));
    assert((annotations[1].map_cell() == CellPosition{-1, -1}));

    assert((AutomapAnnotationModel::label_for_index(0).bytes() ==
            std::array<char, 2>{'A', ' '}));
    assert((AutomapAnnotationModel::label_for_index(25).bytes() ==
            std::array<char, 2>{'Z', ' '}));
    assert((AutomapAnnotationModel::label_for_index(26).bytes() ==
            std::array<char, 2>{'A', 'A'}));
    assert((AutomapAnnotationModel::label_for_index(27).bytes() ==
            std::array<char, 2>{'B', 'A'}));
    assert((AutomapAnnotationModel::label_for_index(51).bytes() ==
            std::array<char, 2>{'Z', 'A'}));
    assert((AutomapAnnotationModel::label_for_index(52).bytes() ==
            std::array<char, 2>{'A', 'B'}));

    const auto label = AutomapAnnotationModel::label_at(
        annotations, {2, 2});
    assert(label.has_value());
    assert((label->bytes() == std::array<char, 2>{'A', ' '}));
    assert(!AutomapAnnotationModel::label_at(
        annotations, {3, 3}).has_value());

    bool rejected = false;
    try {
        (void)AutomapAnnotationModel::parse({0, 0, 0});
    } catch (const std::runtime_error&) {
        rejected = true;
    }
    assert(rejected);
}
