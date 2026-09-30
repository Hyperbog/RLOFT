#pragma once

#include <cstdint>
#include <functional>
#include <span>

namespace ravenloft::game {

struct EntitySearchState083C {
    std::span<const std::uint8_t> records14D78;
    std::int16_t count258A2 = 0;

    std::int16_t valueF03E = 0;
    std::int16_t valueF03C = 0;
    std::int16_t value255D2 = 0;
    std::int16_t value255D6 = 0;

    // DS:0x22230 is loaded as a pointer by 0x08B5.
    std::span<const std::uint8_t> pointedTable22230;
    std::span<const std::uint8_t> flags198E0;

    // Exact return-value predicates of the original helper calls.
    std::function<std::int32_t(std::uint16_t)> call2ACDC;
    std::function<std::int32_t(std::uint16_t)> call2AD18;
    std::function<std::int32_t(std::uint16_t)> call2ADE0;
};

class EntitySearch083C {
public:
    // CODE.2 0x083C..0x0930.
    static std::int32_t find083C(const EntitySearchState083C& state,
                                 std::int32_t first,
                                 std::int32_t second,
                                 std::int32_t start);

    // CODE.2 0x0934..0x09AF.
    static std::int32_t find0934(const EntitySearchState083C& state,
                                 std::int32_t first,
                                 std::int32_t second,
                                 std::int32_t start);
};

} // namespace ravenloft::game
