#pragma once
#include <cstdint>
#include <functional>
#include <span>

namespace ravenloft::game {

struct MapExportFile2A34State {
    std::span<std::uint8_t> nameTemplate5FDA;
};

struct MapExportFile2A34Ops {
    std::function<void(std::span<const std::uint8_t>)> call31380;
    std::function<std::int32_t(std::span<const std::uint8_t>)> call312C8;
    std::function<void(std::int32_t)> call312DC;
    std::function<std::int32_t(std::span<const std::uint8_t>)> call312B4;
    std::function<void()> call2E6D0;
    std::function<void(std::int32_t,std::int32_t,std::int32_t)> call26E8;
};

class MapExportFile2A34 {
public:
    // Direct reconstruction of CODE.2 0x2A34..0x2AE4.
    static void run(MapExportFile2A34State&,const MapExportFile2A34Ops&,
                    std::int32_t number);
};

} // namespace ravenloft::game
