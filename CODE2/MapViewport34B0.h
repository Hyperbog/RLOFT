#pragma once
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct MapViewport34B0State {
 std::int32_t viewY12740=0,viewX12744=0;
 std::int16_t mapWidth12754=0,mapHeight12756=0;
 std::uint8_t action5FCA=0,valueC050=0;
};
struct MapViewport34B0Ops {
 std::function<void(std::uint8_t)> call291B0;
 std::function<void(std::int32_t,std::int32_t,std::int32_t,std::int32_t,std::int32_t)> call139C;
 std::function<void(std::uint8_t)> call1A510;
};
class MapViewport34B0 {
public:
 // Direct reconstruction of CODE.2 0x34B0..0x3630.
 static void run(MapViewport34B0State&,const MapViewport34B0Ops&);
};

} // namespace ravenloft::game
