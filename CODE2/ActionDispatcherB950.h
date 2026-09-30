#pragma once
#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct ActionDispatcherB950State {
 std::int32_t playerX=0, playerY=0, facing=0;
 std::uint8_t bf38=0;
 std::uint16_t f03c=0;
};
struct ActionDispatcherB950Ops {
 std::function<void(int)> begin19CCC;
 std::function<void()> finish19CF4;
 std::function<void(int)> e578;
 std::function<int(int,int)> value22E40;
 std::function<int(int)> direction9418;
 std::function<int()> select95C0;
 std::function<void(int)> burstB410;
 std::function<std::int32_t(int)> entityX;
 std::function<std::int32_t(int)> entityY;
 std::function<int(int,int,int,int)> dice1C328;
 std::function<void(int,int,int,int,int,int,int,int,int,int)> effectB02C;
 std::function<int(int,int,int,int,int,int,int,int)> effectA254;
 std::function<std::uint8_t&(int,int)> characterByte;
 std::function<std::int32_t&(int,int)> characterDword;
 std::function<std::uint8_t&(int)> characterCf5a;
 std::function<std::int32_t()> clock19CC4;
 std::function<void(std::int32_t,int,int,int)> event28110;
 std::function<void(std::int32_t)> refresh31F38;
 std::function<std::int32_t(int)> resource258D4;
};
class ActionDispatcherB950 {
public:
 static int run01(ActionDispatcherB950State&,const ActionDispatcherB950Ops&,int character);
 static int run0A(ActionDispatcherB950State&,const ActionDispatcherB950Ops&,int character);
 static int run1F(ActionDispatcherB950State&,const ActionDispatcherB950Ops&,int character);
 static int run35(ActionDispatcherB950State&,const ActionDispatcherB950Ops&,int character);
 static int runSelectedEntity(ActionDispatcherB950State&,const ActionDispatcherB950Ops&,int character);
 static int runTarget03(ActionDispatcherB950State&,const ActionDispatcherB950Ops&,int character);
 static int runState04(ActionDispatcherB950State&,const ActionDispatcherB950Ops&,int character);
 static int runState03(ActionDispatcherB950State&,const ActionDispatcherB950Ops&,int character);
 static int runStateTimed(ActionDispatcherB950State&,const ActionDispatcherB950Ops&,int character);
 static int runState80(ActionDispatcherB950State&,const ActionDispatcherB950Ops&,int character);
};
}
