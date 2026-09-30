#pragma once
#include <array>
#include <cstdint>
#include <functional>
#include <span>
namespace ravenloft::game {
struct StatusExpiry871CState {
 std::span<std::uint8_t> characters;
 std::array<std::uint8_t,4> refreshFlags{};
 std::int32_t globalFD08=0;
 std::int16_t globalF03C=0,globalF03E=0,global6DAE=0;
};
struct StatusExpiry871COps {
 std::function<void(int)> call31F38;
 std::function<void(int,int)> call1202C;
 std::function<void(int,int)> notify;
};
class StatusExpiry871C { public: static void apply(StatusExpiry871CState&,const StatusExpiry871COps&,int character,int status); };
}
