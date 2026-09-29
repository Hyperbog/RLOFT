#pragma once

#include <cstdint>
#include <string>

namespace ravenloft::game {

class RuntimeDecimal88D8 {
public:
    // Observable reconstruction of CODE.1 0x88D8..0x88F6: signed integer to
    // base-10 text using the original shared conversion-buffer semantics.
    static std::string convert(std::int32_t value);
};

} // namespace ravenloft::game
