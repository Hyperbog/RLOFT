#include "games/ravenloft/game/Code1RuntimeMemset84A0.h"

namespace ravenloft::game {

void* Code1RuntimeMemset84A0::set(void* destination,
                                  std::uint8_t value,
                                  std::size_t count) noexcept {
    auto* out = static_cast<std::uint8_t*>(destination);
    for (std::size_t i = 0; i < count; ++i) {
        out[i] = value;
    }
    return destination;
}

} // namespace ravenloft::game
