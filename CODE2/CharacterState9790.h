#pragma once
#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct CharacterState9790Ops {
  std::function<int(int,int)> querySlot;
  std::function<void(int,int,int)> updateSlot;
  std::function<int(int,int,int,int,int,int)> preserveSlot;
  std::function<void(int,int,int)> refreshPair;
  std::function<void(int,int,int,int)> playResource;
  std::function<void()> finishResource;
  std::function<void(std::uint32_t,int,int,int,int)> present;
  std::function<std::int32_t()> clock;
};
class CharacterState9790 {
public:
  static int run(std::uint8_t* characters, std::uint32_t c050, int resourceBase,
                 const CharacterState9790Ops&, int kind, int value, int mask,
                 int timerIndex, int timerScale, int duration,
                 int resourceOffset, int character);
};
}
