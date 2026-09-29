#pragma once

#include <cstddef>
#include <cstdint>

namespace ravenloft::game {

class RuntimeMemset84A0 {
public:
    // Observable C/C++ reconstruction of CODE.1 0x84A0..0x84C1.
    // The original expands the low byte across EDX and delegates the bulk fill
    // to CODE.1 0xDE70, then returns the original destination pointer.
    static void* set(void* destination,
                     std::uint8_t value,
                     std::size_t count) noexcept;
};

} // namespace ravenloft::game
