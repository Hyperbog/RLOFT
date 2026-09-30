#pragma once
#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct ModalDisplay3F78State { std::uint32_t valueC050=0; std::int16_t value8600=0; std::uint8_t value8602=0; };
struct ModalDisplay3F78Ops {
 std::function<void(std::uint8_t)> call291B0;
 std::function<void(std::uint32_t,int,int,int,int)> call1C40C;
 std::function<void(int,int,int,int,int)> call30508;
 std::function<void(std::uint32_t)> call1A510;
 std::function<int()> call1A1F4,call30B40;
 std::function<void(std::uint32_t,std::int16_t,std::uint8_t)> call1A5B0;
};
class ModalDisplay3F78 { public: static void run(ModalDisplay3F78State&,const ModalDisplay3F78Ops&,int,int); };
}
