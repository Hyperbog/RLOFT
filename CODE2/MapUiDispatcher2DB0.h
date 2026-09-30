#pragma once
#include <array>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct MapUiDispatcher2DB0State {
 std::int32_t viewY12740=0,viewX12744=0,mapIndex1274C=0,currentMapC6C8=0;
 std::int16_t mapWidth12754=0,mapHeight12756=0,playerX255D2=0,playerY255D6=0;
 std::uint8_t action5FCA=0,enabled5FD9=0,currentMap5FCB=0,valueC050=0;
 // 16 records: action id,left,top,right,bottom. id==-1 terminates.
 std::array<std::array<std::int16_t,5>,16> hit{};
};

struct MapUiDispatcher2DB0Ops {
 std::function<void(std::uint8_t)> call291B0;
 std::function<void(std::int32_t,std::int32_t,std::int32_t,std::int32_t,std::int32_t)> call139C;
 std::function<void(std::uint8_t)> call1A510;
 std::function<std::int32_t(std::int32_t)> call2AE8;
 std::function<void()> call207C;
 std::function<void()> call21D8;
 std::function<void(std::int32_t,std::int32_t,std::int32_t)> call26E8;
 std::function<void(std::int32_t)> call2A34;
 std::function<void()> call1F8FC;
 std::function<void()> call1AE30;
 std::function<void(std::int32_t)> drawAction;
};

class MapUiDispatcher2DB0 {
public:
 // Reconstruction of CODE.2 jump table 0x2D70..0x2DAF and dispatcher
 // 0x2DB0..0x34AE.
 static void run(MapUiDispatcher2DB0State&,const MapUiDispatcher2DB0Ops&,
                 std::int32_t x,std::int32_t y,std::uint8_t flags);
};

} // namespace ravenloft::game
