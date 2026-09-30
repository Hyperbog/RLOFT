#pragma once
#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct DosDiskReset7378State { std::uint8_t status12B70=0; };
struct DosDiskReset7378Ops { std::function<void(void*,int,int)> call33971; std::function<void(std::uint8_t*)> dosInt21Ah13; };
class DosDiskReset7378 { public: static void run(DosDiskReset7378State&,const DosDiskReset7378Ops&); };
}
