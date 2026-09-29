#pragma once

#include <cstdint>

namespace ravenloft::game {

class StateStep44E4 {
public:
    // Direct reconstruction of CODE.1 0x44E4..0x451E.
    static void apply(std::int32_t& value,
                      std::int32_t& delta) noexcept;
};

} // namespace ravenloft::game
