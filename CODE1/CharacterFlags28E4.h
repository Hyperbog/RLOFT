#pragma once

#include <array>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

class CharacterFlags28E4 {
public:
    // Fidelity-verified reconstruction of CODE.1 0x28E4..0x2990.
    // 0x2991..0x2993 is alignment LEA EAX,[EAX+0].
    static void build(std::array<std::int32_t, 6>& out,
                      const std::array<std::uint8_t, 3>& fields680D) noexcept;
};

struct CharacterFlagChoiceState2994 {
    std::array<std::uint8_t, 3> fields680D{{0xFFu,0xFFu,0xFFu}};
    std::uint8_t field6812 = 0;
    std::int32_t mode4D0 = 0;
};

struct CharacterFlagChoiceOps2994 {
    std::function<void(std::int32_t,
                       const std::array<std::int32_t, 9>&,
                       std::int32_t, std::int32_t, std::int32_t)> call1464;
    std::function<std::int32_t()> call043C;
    std::function<void(std::int32_t, std::int32_t,
                       std::int32_t, std::int32_t)> call15B0;

    // CODE.1 0x2A0B reads DWORD [EBP + EAX*4 - 0x20] with no range check.
    // EAX 1..6 addresses the six local option DWORDs. Other values address
    // the original caller/frame/stack image; the embedding layer supplies that
    // DWORD rather than letting host C++ perform an out-of-bounds access.
    std::function<std::uint32_t(std::int32_t eax)> readUncheckedFrameDword;
};

class CharacterFlagChoice2994 {
public:
    // Fidelity reconstruction of CODE.1 0x2994..0x2A60.
    static void run(CharacterFlagChoiceState2994& state,
                    const CharacterFlagChoiceOps2994& ops);
};

} // namespace ravenloft::game
