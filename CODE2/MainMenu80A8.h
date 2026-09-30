#pragma once
#include <array>
#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct MainMenu80A8State {
 std::uint32_t c050=0, resource258DC=0, c034=0, surface=0;
 std::int32_t c6c8=0; std::int16_t a6c2=0, value8600=0, value1457c=0;
 std::uint8_t a79d=0,d680=0,value8602=0,cf58=0,flag6460=0;
 std::array<std::uint32_t,8> labels60F0{};
};
struct MainMenu80A8Ops {
 std::function<std::int16_t()> mouseX9E31,mouseY9E33;
 std::function<int()> input1A1F4;
 std::function<void(int)> call19CCC,call171FC,call205B4,call3634,message803C,free30040;
 std::function<void()> flip1A9F8,call461C,call5C84,call5D34,call1F8FC,load7B94,save79E0,config6548,config5DD0,call22D70,call19CF4;
 std::function<void(std::uint8_t)> call291B0;
 std::function<std::uint32_t(int,int,int,int,int)> alloc30070;
 std::function<void(int,int,int,int,int)> draw2975C,draw30508,present1A8C8;
 std::function<void(int,int)> call1A344;
 std::function<void(std::uint32_t,int,int,int,int)> call325B0;
 std::function<void(std::uint32_t,int,int,int,int,int)> call322EC;
 std::function<void(std::uint32_t,int,int,int)> call2939C;
 std::function<int(int)> confirm7D44;
};
class MainMenu80A8 { public: static void run(MainMenu80A8State&,const MainMenu80A8Ops&); };
}
