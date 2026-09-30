#pragma once
#include <cstdint>
#include <functional>
#include <span>
#include <vector>

namespace ravenloft::game {

struct MapLoad2AE8State {
 std::span<std::uint8_t> map1760;
 std::span<std::uint8_t> records5760;
 std::vector<std::uint8_t> savedRecords;
 std::vector<std::uint8_t> entityData6048;
 std::int32_t savedRecordBytes6054=0;
 std::int16_t entityCount604C=0;
 std::int16_t width12754=0,height12756=0;
 std::int32_t currentMapC6C8=0;
};

struct MapLoad2AE8Ops {
 std::function<std::size_t()> call1C08;
 std::function<void(std::int32_t)> call310B0;
 std::function<void(std::span<std::uint8_t>,std::uint8_t)> fill;
 std::function<void(std::int32_t)> call1558;
 std::function<std::int32_t(std::int32_t)> openNumbered;
 std::function<void(std::int32_t,std::int32_t,std::int32_t)> call3131C;
 std::function<void(std::uint8_t*,std::int32_t,std::int32_t)> call312EC;
 std::function<void()> call2E6D0;
};

class MapLoad2AE8 {
public:
 // Direct reconstruction of CODE.2 0x2AE8..0x2D6E.
 static std::int32_t run(MapLoad2AE8State&,const MapLoad2AE8Ops&,std::int32_t mapNumber);
};

} // namespace ravenloft::game
