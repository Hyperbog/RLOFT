#pragma once

#include <array>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct UiFlowCharacterState {
    std::array<std::uint8_t, 2> flags4C4{};
    std::array<std::uint8_t, 2> byte680B{};
    std::array<std::uint8_t, 2> byte680C{};
    std::array<std::uint32_t, 2> dword6903{};
    std::array<std::uint32_t, 2> dword6907{};
    std::int32_t selectedCharacter4CC = -1;
    std::int32_t mode4D0 = 0;
    std::uintptr_t handle95C = 0;
    std::uintptr_t handle960 = 0;
};

struct UiFlowOps1CD0 {
    std::function<void()> call1A04;
    std::function<void(std::int32_t,
                       const std::array<std::int32_t, 9>&,
                       std::int32_t, std::int32_t, std::int32_t)> call1464;
    std::function<std::int32_t()> call043C;
    std::function<void()> call1BD4;
    std::function<void(std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> call15B0;
    std::function<void(std::uintptr_t)> call7830;
};

class UiFlow1CD0 {
public:
    // Direct reconstruction of CODE.1 0x1CD0..0x1DD7.
    static void run(UiFlowCharacterState& state,
                    const UiFlowOps1CD0& ops);
};

class UiFlow1DD8 {
public:
    // Direct reconstruction of CODE.1 0x1DD8..0x1E61.
    static void run(UiFlowCharacterState& state,
                    const UiFlowOps1CD0& ops);
};

class UiFlow1E64 {
public:
    // Direct reconstruction of CODE.1 0x1E64..0x1EE0.
    static void run(UiFlowCharacterState& state,
                    const UiFlowOps1CD0& ops);
};

class SwapByte1EE4 {
public:
    // Direct reconstruction of CODE.1 0x1EE4..0x1EFE.
    static void swap(std::uint8_t& a, std::uint8_t& b) noexcept;
};

} // namespace ravenloft::game
