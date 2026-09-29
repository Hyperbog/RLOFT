#include "games/ravenloft/game/Code1StateStep44E4.h"

namespace ravenloft::game {

void Code1StateStep44E4::apply(std::int32_t& value,
                               std::int32_t& delta) noexcept {
    value += delta;

    if (value == 0x18) {
        value = 0x14;
        delta = -2;
    }

    if (value == -4) {
        value = 0;
        delta = 2;
    }
}

} // namespace ravenloft::game
