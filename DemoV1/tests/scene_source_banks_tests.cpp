#include "rloft/scene_source_banks.hpp"

#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace {
void put16(
    std::vector<std::uint8_t>& b,
    std::size_t o,
    std::uint16_t v) {
    b[o] = static_cast<std::uint8_t>(v);
    b[o + 1U] =
        static_cast<std::uint8_t>(v >> 8U);
}
}

int main() {
    std::vector<std::uint8_t> init(0x24U, 0);
    put16(init, 0x06U, 0U);
    put16(init, 0x08U, 2169U);
    put16(init, 0x0AU, 3316U);
    put16(init, 0x0CU, 4158U);

    const auto banks =
        rloft::SceneSourceBanks::parse(init);
    assert(banks.at(0) == 0U);
    assert(banks.at(1) == 2169U);
    assert(banks.at(2) == 3316U);
    assert(banks.at(3) == 4158U);

    bool rejected = false;
    try {
        (void)banks.at(4);
    } catch (const std::out_of_range&) {
        rejected = true;
    }
    assert(rejected);

    return 0;
}
