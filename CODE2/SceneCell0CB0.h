#pragma once

#include <cstdint>
#include <functional>
#include <span>

namespace ravenloft::game {

struct SceneCellState0CB0 {
    std::span<const std::uint8_t> map1760;
    std::int16_t width12756 = 0;
    bool alternateTable = false; // DS:0x5FCB

    // Raw 0x16-stride record storage used by the four search families.
    std::span<const std::uint8_t> table6048;
    std::span<const std::uint8_t> table14D78;
    std::span<const std::uint8_t> table22230;

    std::int16_t value255D0 = 0;
    std::int16_t value255D2 = 0;
    std::uint16_t value255D4 = 0;
    std::int16_t value255D6 = 0;

    const void* drawContext = nullptr; // DS:0x258DC + 0x34
};

struct SceneCellOps0CB0 {
    std::function<std::int32_t(std::int32_t,std::int32_t,std::int32_t)> find0700;
    std::function<std::int32_t(std::int32_t,std::int32_t,std::int32_t)> find0794;
    std::function<std::int32_t(std::int32_t,std::int32_t,std::int32_t)> find07E4;
    std::function<std::int32_t(std::int32_t,std::int32_t,std::int32_t)> find083C;
    std::function<std::int32_t(std::int32_t,std::int32_t,std::int32_t)> find0934;

    std::function<void(std::int32_t,std::int32_t,std::int32_t,
                       const void*,std::int32_t)> call2975C;
    std::function<void(std::int32_t,std::int32_t)> call0BFC;
};

class SceneCell0CB0 {
public:
    // Direct reconstruction of CODE.2 0x0CB0..0x10C2.
    static void run(const SceneCellState0CB0& state,
                    const SceneCellOps0CB0& ops,
                    std::int32_t cellX,
                    std::int32_t cellY,
                    std::int32_t screenY,
                    std::int32_t screenX);
};

} // namespace ravenloft::game
