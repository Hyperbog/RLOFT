#pragma once
#include <cstdint>
#include <functional>
#include <span>
#include <vector>

namespace ravenloft::game {

struct MapInterface3634State {
 std::span<std::uint8_t> palette759C;
 std::span<std::uint8_t> map1760;
 std::span<std::uint8_t> records5760;
 std::vector<std::uint8_t> savedRecords;
 std::vector<std::uint8_t> entityData6048;
 std::int32_t savedRecordBytes6054=0;
 std::int16_t entityCount604C=0;
 std::int16_t playerX255D2=0,playerY255D6=0;
 std::int16_t sourceHeight25619=0,sourceWidth2561B=0;
 std::int16_t mapWidth12754=0,mapHeight12756=0;
 std::int32_t viewY12740=0,viewX12744=0,mapIndex1274C=0,currentMapC6C8=0;
 std::uint8_t action5FCA=0,currentMap5FCB=0,valueC050=0,stateD680=0,value6058=0;
 std::uint16_t value9E35=0;
 std::int16_t value8600=0; std::uint8_t value8602=0;
};

struct MapInterface3634Ops {
 std::function<void(std::int32_t)> call19CCC;
 std::function<void()> call1A9F8;
 std::function<void(std::uint8_t)> call291B0;
 std::function<void()> call1558;
 std::function<void()> call1834;
 std::function<void(std::int32_t,std::int32_t,std::int32_t,std::int32_t,std::int32_t)> call139C;
 std::function<void(std::uint8_t,std::int16_t,std::uint8_t)> call1A5B0;
 std::function<void()> call22D70;
 std::function<void()> call34B0;
 std::function<void()> call22D8C;
 std::function<void(std::int32_t)> freeBlock;
 std::function<void()> tail19CF4;
};

class MapInterface3634 {
public:
 // Direct reconstruction of CODE.2 0x3634..0x3808.
 static void run(MapInterface3634State&,const MapInterface3634Ops&);
};

} // namespace ravenloft::game
