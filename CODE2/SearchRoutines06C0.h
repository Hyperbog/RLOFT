#pragma once

#include <cstdint>
#include <span>

namespace ravenloft::game {

struct SearchState06C0 {
    std::int16_t value255D2 = 0;
    std::int16_t value255D6 = 0;
    std::int32_t value12740 = 0;
    std::int32_t value12744 = 0;

    bool alternateTable = false;       // DS:0x5FCB
    std::span<const std::uint8_t> table6048;
    std::span<const std::uint8_t> table22230;
    std::span<const std::uint8_t> table17610;
    std::int16_t count604C = 0;
    std::int16_t count258A6 = 0;
    std::int16_t count258A4 = 0;
};

class SearchRoutines06C0 {
public:
    // CODE.2 0x06C0..0x06FC.
    static void updateBounds(SearchState06C0& state) noexcept;

    // CODE.2 0x0700..0x0791.
    static std::int32_t find0700(SearchState06C0& state,
                                 std::int32_t first,
                                 std::int32_t second,
                                 std::int32_t start) noexcept;

    // CODE.2 0x0794..0x07E1.
    static std::int32_t find0794(const SearchState06C0& state,
                                 std::int32_t first,
                                 std::int32_t second,
                                 std::int32_t start) noexcept;

    // CODE.2 0x07E4..0x0838.
    static std::int32_t find07E4(const SearchState06C0& state,
                                 std::int32_t first,
                                 std::int32_t second,
                                 std::int32_t start) noexcept;
};

} // namespace ravenloft::game
