#pragma once
#include <cstdint>
#include <functional>
#include <span>
#include <vector>

namespace ravenloft::game {

struct MapExport26E8State {
    std::int16_t width12754=0, height12756=0;
    std::int16_t value2561B=0;
    std::span<const std::uint8_t> map1760;
    std::span<const std::uint8_t> tileChars6011;
    std::uint8_t fallback6030=' ';
    std::uint8_t value5F60=0, value5F61=0;
};

struct MapExport26E8Ops {
    std::function<std::int32_t()> call2354;
    std::function<void()> call2E6D0;
    std::function<std::int32_t(std::int32_t,std::int32_t,std::int32_t)> call09B0;
    std::function<std::int32_t(std::int32_t,std::int32_t,std::int32_t)> call0700;
    std::function<std::uint16_t(std::int32_t)> entityType22246;
    std::function<std::int32_t(std::int32_t,std::int32_t,std::uint8_t*)> call23B8;
    std::function<void(std::uint8_t,std::int32_t)> call31107;
    std::function<void(const std::uint8_t*,std::int32_t,std::int32_t)> call31304;
    std::function<void(std::int32_t,std::int32_t,std::int32_t)> call2474;
    std::function<void(std::int32_t)> call31491;
};

class MapExport26E8 {
public:
 static void run(const MapExport26E8State&,const MapExport26E8Ops&,
                 std::int32_t arg0,std::int32_t output,std::int32_t alternateMode);
};

} // namespace ravenloft::game
