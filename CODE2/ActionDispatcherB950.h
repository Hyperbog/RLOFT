#pragma once
#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct ActionDispatcherB950State {
 std::int32_t playerX=0, playerY=0, facing=0;
 std::uint8_t bf38=0;
 std::uint16_t f03c=0;
 std::uint8_t cf5a[4]{};
 std::uint8_t d681=0,d682=0;
 std::uint32_t saved12ba0=0,saved12ba4=0;
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
 std::function<void(int)> directA794;
 std::function<int(int,int,int,int,int,int,int)> state9790;
 std::function<void(int)> refresh1F494;
 std::function<void(int,int,int)> panel9B4C;
 std::function<void(int,int,int)> f238;
 std::function<std::uint8_t&(int,int)> entityByte;
 std::function<std::uint16_t(int)> entityType;
 std::function<std::uint16_t&(int,int)> entityWord;
 std::function<std::uint16_t(int,int)> metadataWord;
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
 static int runTarget42(ActionDispatcherB950State&,const ActionDispatcherB950Ops&,int character);
 static int runTarget06(ActionDispatcherB950State&,const ActionDispatcherB950Ops&,int character);
 static int runTarget25(ActionDispatcherB950State&,const ActionDispatcherB950Ops&,int character);
 static int runTarget18(ActionDispatcherB950State&,const ActionDispatcherB950Ops&,int character);
 static int runTarget45(ActionDispatcherB950State&,const ActionDispatcherB950Ops&,int character);
 static int runTarget05(ActionDispatcherB950State&,const ActionDispatcherB950Ops&,int character);
 static int runTarget30(ActionDispatcherB950State&,const ActionDispatcherB950Ops&,int character);
 static int runA794(ActionDispatcherB950State&,const ActionDispatcherB950Ops&,int character);
 static int runTarget44(ActionDispatcherB950State&,const ActionDispatcherB950Ops&,int character);
 static int runTarget02(ActionDispatcherB950State&,const ActionDispatcherB950Ops&,int character);
 static int runUiStateBE28(ActionDispatcherB950State&,const ActionDispatcherB950Ops&,int character);
 static int runTarget51(ActionDispatcherB950State&,const ActionDispatcherB950Ops&,int character);
 static int runTarget6E(ActionDispatcherB950State&,const ActionDispatcherB950Ops&,int character);
 static int runTarget78(ActionDispatcherB950State&,const ActionDispatcherB950Ops&,int character);
 static int runTarget64(ActionDispatcherB950State&,const ActionDispatcherB950Ops&,int character);
 static int runTarget71(ActionDispatcherB950State&,const ActionDispatcherB950Ops&,int character);
 static int runTarget68(ActionDispatcherB950State&,const ActionDispatcherB950Ops&,int character);
 static int runTarget82(ActionDispatcherB950State&,const ActionDispatcherB950Ops&,int character);
 static int runState7E(ActionDispatcherB950State&,const ActionDispatcherB950Ops&,int character);
 static int runState7F(ActionDispatcherB950State&,const ActionDispatcherB950Ops&,int character);
 static int runState80_9790(ActionDispatcherB950State&,const ActionDispatcherB950Ops&,int character);
 static int runState81(ActionDispatcherB950State&,const ActionDispatcherB950Ops&,int character);
 static int runState82_9790(ActionDispatcherB950State&,const ActionDispatcherB950Ops&,int character);
 static int runState83(ActionDispatcherB950State&,const ActionDispatcherB950Ops&,int character);
 static int runState84(ActionDispatcherB950State&,const ActionDispatcherB950Ops&,int character);
 static int runState85(ActionDispatcherB950State&,const ActionDispatcherB950Ops&,int character);
 static int runState86(ActionDispatcherB950State&,const ActionDispatcherB950Ops&,int character);
 static int runSelectedFlag80(ActionDispatcherB950State&,const ActionDispatcherB950Ops&,int character);
 static int runSelectedTimed08(ActionDispatcherB950State&,const ActionDispatcherB950Ops&,int character);
};
}
