#pragma once
#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct PackedOverlay5984State { std::uint8_t visible6460=0; std::uint32_t c050=0; std::int16_t v8600=0; std::uint8_t v8602=0; };
struct PackedOverlay5984Ops {
 std::function<void(int,int,int,int,int)> call325B0;
 std::function<void(std::uint8_t)> call291B0;
 std::function<void(int,int,int,int)> call2E510;
 std::function<void(std::uint32_t)> call1A510;
 std::function<void(int)> call29234;
 std::function<void(int,int,int,int,int,void*)> call32468;
 std::function<void(std::uint32_t,int,int)> call1A5B0;
};
class PackedOverlay5984 { public: static void show(PackedOverlay5984State&,const PackedOverlay5984Ops&,int,int); static void hide(PackedOverlay5984State&,const PackedOverlay5984Ops&,int); };
}
