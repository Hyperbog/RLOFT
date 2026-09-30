#pragma once
#include <cstdint>
#include <functional>
#include <span>
namespace ravenloft::game {
struct PackedHit5698State {
 std::span<std::uint8_t> buffers; // six pointer-backed row buffers flattened by adapter
 std::span<const std::int32_t> x63F0,y63F4,counts6420;
 std::uint32_t c050=0;
};
struct PackedHit5698Ops {
 std::function<void(std::uint8_t)> call291B0;
 std::function<void(int,int,int,int)> call296C0,call3060C;
 std::function<int(int,int)> call2F29C;
 std::function<void(int,int,int,int,int)> call2E438,call30630,call1A8C8;
 std::function<int(int)> call2E6F0;
 std::function<void(int,int)> call1C368;
};
class PackedHit5698 { public:
 static int apply(PackedHit5698State&,const PackedHit5698Ops&,int mode,int mouseX,int mouseY,int selected,int baseX,int baseY,std::uint8_t*limit);
};
}
