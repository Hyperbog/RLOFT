#pragma once
#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct ActionDispatcherB950State {
 std::int32_t playerX=0, playerY=0;
 std::uint8_t bf38=0;
};
struct ActionDispatcherB950Ops {
 std::function<void(int)> begin19CCC;
 std::function<void()> finish19CF4;
 std::function<void(int)> e578;
 std::function<int(int,int)> value22E40;
 std::function<int(int,int,int,int)> dice1C328;
 std::function<void(int,int,int,int,int,int,int,int,int,int)> effectB02C;
};
class ActionDispatcherB950 {
public:
 static int run01(ActionDispatcherB950State&,const ActionDispatcherB950Ops&,int character);
};
}
