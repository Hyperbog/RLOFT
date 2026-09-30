#pragma once
#include <array>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct DeviceProbe2354Ops {
    // Original call: 0x30D52(0x17, &block, &block).
    std::function<void(std::int32_t, std::array<std::uint8_t,0x1C>&)> call30D52;
};

class DeviceProbe2354 {
public:
    // Direct reconstruction of CODE.2 0x2354..0x23B5.
    static std::int32_t run(const DeviceProbe2354Ops& ops);
};

} // namespace ravenloft::game
