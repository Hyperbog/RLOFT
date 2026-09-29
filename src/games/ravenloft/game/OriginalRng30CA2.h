#pragma once
#include <cstdint>
#include <functional>

namespace ravenloft::game {

// Direct reconstruction of CODE.2 0x30C9C/0x30CA2/0x30CC4 plus the DOS
// gameplay seeder at 0x290D0 and BIOS tick reader boundary 0x311E0.
class OriginalRng30CA2 {
public:
    explicit OriginalRng30CA2(std::uint32_t state=1) : state_(state) {}
    std::uint16_t next15();
    void seed(std::uint32_t state) { state_=state; } // 0x30CC4
    std::uint32_t state() const { return state_; }

    // 0x290D0: only the low WORD of BX is retained. If it is zero the routine
    // rereads BIOS 0x046C and again takes only the low WORD.
    std::uint16_t seedFromBiosLowWord(std::uint32_t firstBiosTick,
                                      const std::function<std::uint32_t()>& rereadBiosTick = {});
private:
    std::uint32_t state_=1;
};

} // namespace ravenloft::game
