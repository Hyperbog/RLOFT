#pragma once

#include <array>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct InterruptUpdateState50C0 {
    std::int16_t word73EC = 0;
    std::int32_t value97C = 0;

    std::int16_t mouseX73E6 = 0;
    std::int16_t mouseY73E8 = 0;

    std::int16_t value71D8 = 0;
    std::int16_t value71D6 = 0;
    std::int16_t value71D4 = 0;

    std::array<std::int16_t, 4> words2401{};
    std::array<std::int16_t, 4> saved7188{};

    // Contiguous DWORDs DS:7178, 717C, 7180, 7184.
    std::array<std::int32_t, 4> rect7178{};
};

struct InterruptUpdateOps50C0 {
    std::function<std::int32_t()> call84E0;
    std::function<void(std::array<std::int32_t, 4>&)> call6C44;
    std::function<void(std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> call6BC0;
    std::function<void()> call5780;
    std::function<void()> call57DC;
};

class InterruptUpdate50C0 {
public:
    // Direct reconstruction of CODE.1 0x50C0..0x522E.
    // CODE.1 0x522F is alignment.
    //
    // The only direct caller is the interrupt path at CODE.1 0x6104.
    static void run(
        std::int32_t unusedEax,
        std::int32_t valueEbx,
        std::int32_t valueEcx,
        std::int32_t valueEdx,
        const std::array<std::int16_t, 2>* pointerEsi,
        InterruptUpdateState50C0& state,
        const InterruptUpdateOps50C0& ops);
};

} // namespace ravenloft::game
