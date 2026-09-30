#pragma once
#include <array>
#include <cstdint>
#include <functional>
#include <span>
namespace ravenloft::game {
struct LimitBuilder5BDCState { std::array<std::uint8_t,8> limits6208{}; std::span<const std::uint8_t> objects; std::span<const std::uint8_t> table6160,table613F; };
struct LimitBuilder5BDCOps { std::function<void(std::uint8_t*,const std::uint8_t*,int)> copy30CD4; };
class LimitBuilder5BDC { public: static void build(LimitBuilder5BDCState&,const LimitBuilder5BDCOps&,int mode,int object,int level,std::span<const std::uint8_t> table); };
}
