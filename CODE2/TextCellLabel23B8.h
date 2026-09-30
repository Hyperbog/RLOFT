#pragma once
#include <cstddef>
#include <cstdint>
#include <span>

namespace ravenloft::game {

struct TextCellLabel23B8State {
    std::span<const std::uint8_t> records5760;
};

class TextCellLabel23B8 {
public:
    // Direct reconstruction of CODE.2 0x23B8..0x2472.
    // Returns 1 when a record occupies (column,row). The two-byte label is
    // always written and advances A..Z, then AA..AZ, BA... as records pass.
    static std::int32_t run(const TextCellLabel23B8State& state,
                            std::int32_t column,
                            std::int32_t row,
                            std::uint8_t* labelOut);
};

} // namespace ravenloft::game
