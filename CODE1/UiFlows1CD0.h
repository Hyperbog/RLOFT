#pragma once

#include <array>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct UiFlowCharacterState {
    std::array<std::uint8_t, 2> flags4C4{};
    std::array<std::uint8_t, 2> byte680B{};
    std::array<std::uint8_t, 2> byte680C{};

    // These logical entries are physically separated by the original 0x248-byte
    // character-record stride: DS:6903/6907 + characterIndex*0x248.
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
    // Direct-binary-verified reconstruction of CODE.1 0x1CD0..0x1DD7.
    static void run(UiFlowCharacterState& state,
                    const UiFlowOps1CD0& ops);
};

class UiFlow1DD8 {
public:
    // Direct-binary-verified reconstruction of CODE.1 0x1DD8..0x1E61.
    // 0x1E62..0x1E63 is two-byte alignment padding (MOV EAX,EAX).
    static void run(UiFlowCharacterState& state,
                    const UiFlowOps1CD0& ops);
};

class UiFlow1E64 {
public:
    // Direct-binary-verified reconstruction of CODE.1 0x1E64..0x1EE0.
    // 0x1EE1..0x1EE3 is three-byte alignment padding (LEA EAX,[EAX+0]).
    static void run(UiFlowCharacterState& state,
                    const UiFlowOps1CD0& ops);
};

class SwapByte1EE4 {
public:
    // Direct-binary-verified reconstruction of CODE.1 0x1EE4..0x1EFE.
    // 0x1EFF is a NOP; 0x1F00..0x1F23 is the following routine's exact
    // nine-DWORD dispatch table, not executable fall-through.
    static void swap(std::uint8_t& a, std::uint8_t& b) noexcept;
};

} // namespace ravenloft::game
