#pragma once
#include <cstdint>
#include <functional>
#include <span>

namespace ravenloft::game {

struct TileMapBuildState1558 {
    std::int16_t sourceWidth25619 = 0;
    std::int16_t sourceHeight2561B = 0;
    std::int16_t width12756 = 0;
    std::int16_t height12754 = 0;
    std::int16_t playerX255D2 = 0;
    std::int16_t playerY255D6 = 0;
    std::uint8_t value258A8 = 0;
    std::span<const std::uint8_t> cells198E0;
    std::span<const std::uint8_t> types25622;
    std::span<std::uint8_t> output1760;
};

struct TileMapBuildOps1558 {
    std::function<std::int32_t(std::int32_t,std::int32_t)> call1E8C8;
    std::function<std::uint16_t(std::int32_t)> call1E930;
};

class TileMapBuild1558 {
public:
    // Direct reconstruction of CODE.2 0x1558..0x1830.
    static void run(TileMapBuildState1558& state,
                    const TileMapBuildOps1558& ops);
};

} // namespace ravenloft::game
