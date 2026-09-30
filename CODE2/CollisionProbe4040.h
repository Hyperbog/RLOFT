#pragma once
#include <cstdint>
#include <functional>
#include <span>
namespace ravenloft::game {
struct CollisionProbe4040State {
 std::int32_t posX255D0=0,posY255D4=0; std::int16_t height25619=0;
 std::span<const std::uint16_t> mapFlags198E0;
};
struct CollisionProbe4040Ops {
 std::function<void(int,int,std::int32_t&,std::int32_t&)> call2DF2C;
 std::function<std::uint16_t(int)> call1E930;
 std::function<int(std::int32_t,std::int32_t,int)> call2C8E4;
};
class CollisionProbe4040 { public: static int run(const CollisionProbe4040State&,const CollisionProbe4040Ops&); };
}
