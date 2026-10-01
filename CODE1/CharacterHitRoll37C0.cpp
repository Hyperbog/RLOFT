#include "CharacterHitRoll37C0.h"

#include <bit>
#include <cstdint>

namespace ravenloft::game {
namespace {
constexpr std::uint32_t add32(std::uint32_t a, std::uint32_t b) noexcept { return a + b; }
constexpr std::uint32_t mul32(std::uint32_t a, std::uint32_t b) noexcept { return a * b; }
constexpr std::int32_t s32(std::uint32_t v) noexcept { return std::bit_cast<std::int32_t>(v); }
constexpr std::uint32_t u32(std::int32_t v) noexcept { return std::bit_cast<std::uint32_t>(v); }
constexpr std::int32_t sx8(std::uint8_t v) noexcept { return static_cast<std::int8_t>(v); }
constexpr std::int32_t sx16(std::uint16_t v) noexcept { return static_cast<std::int16_t>(v); }
constexpr std::uint32_t base(const CharacterHitRollOps37C0& o) noexcept {
    return mul32(o.readDword(0x04CCu), 0x248u);
}
}

void CharacterHitRoll37C0::initialize(const CharacterHitRollOps37C0& o) {
    std::int32_t count = 1;
    if (sx8(o.readByte(add32(base(o), 0x680Eu))) != -1) count = 2;
    if (sx8(o.readByte(add32(base(o), 0x680Fu))) != -1) ++count;

    std::uint32_t total = 0u;
    std::int32_t lastClass = 0;
    std::uint32_t characterBase = base(o);
    for (std::int32_t slot = 0; slot < count; ++slot) {
        characterBase = base(o); // 0x3837 supplies ECX for the iteration.
        lastClass = sx8(o.readByte(add32(characterBase, 0x680Du + u32(slot))));
        const std::int32_t sides = sx8(o.readByte(add32(0x0853u, u32(lastClass))));
        const std::int32_t diceCount = sx16(o.readWord(
            add32(characterBase, 0x6818u + u32(slot * 2))));
        total += o.call0010(u32(diceCount), u32(sides), 0u, 0u);
    }

    const std::int32_t average = s32(total) / count;
    std::uint32_t result = u32(average);

    const std::uint32_t stat = o.readByte(add32(characterBase, 0x682Eu));
    const std::int32_t group = lastClass / 3;
    const std::uint32_t bonusAddr = add32(0x04D4u,
        add32(mul32(u32(group), 0x4Cu), stat * 4u));
    result += o.readDword(bonusAddr);

    // 0x3873 stores only BX through ECX, which still contains the character
    // base loaded by the final 0x3837 loop test.
    o.writeWord(add32(characterBase, 0x6816u), static_cast<std::uint16_t>(result));
}

} // namespace ravenloft::game
