#include "CharacterNormalize3404.h"

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
constexpr std::uint32_t charBase(const CharacterNormalizeOps3404& o) noexcept {
    return mul32(o.readDword(0x04CCu), 0x248u);
}
}

void CharacterNormalize3404::apply(const CharacterNormalizeOps3404& o) {
    const std::uint32_t race = o.readByte(add32(charBase(o), 0x680Cu));

    // 0x341E..0x34D4: raw race tables, six alternating stat bytes.
    for (std::uint32_t stat = 0; stat < 6u; ++stat) {
        const std::uint32_t pair = add32(mul32(race, 12u), stat * 2u);
        const std::uint32_t modAddr = add32(0x06B4u, add32(mul32(race, 6u), stat));
        const std::int32_t mod = sx8(o.readByte(modAddr));
        const std::uint32_t valueAddr = add32(charBase(o), add32(0x682Au, stat * 2u));

        const std::uint8_t lowByte = o.readByte(add32(0x066Cu, pair));
        const std::int32_t low = s32(static_cast<std::uint32_t>(lowByte) + u32(mod));
        if (static_cast<std::int32_t>(o.readByte(valueAddr)) < low)
            o.writeByte(valueAddr, static_cast<std::uint8_t>(lowByte + o.readByte(modAddr)));

        const std::uint8_t highByte = o.readByte(add32(0x066Du, pair));
        const std::int32_t high = s32(static_cast<std::uint32_t>(highByte) + u32(mod));
        if (static_cast<std::int32_t>(o.readByte(valueAddr)) > high)
            o.writeByte(valueAddr, static_cast<std::uint8_t>(highByte + o.readByte(modAddr)));
    }

    std::uint32_t base = charBase(o);
    if (o.readByte(add32(base, 0x682Au)) == 0x12u &&
        o.readByte(add32(base, 0x6836u)) == 0u)
        o.writeByte(add32(base, 0x6836u), 1u);

    base = charBase(o);
    bool clearExtra = o.readByte(add32(base, 0x682Au)) < 0x12u;
    if (!clearExtra) {
        const std::uint8_t c0 = o.readByte(add32(base, 0x680Du));
        const std::uint8_t c1 = o.readByte(add32(base, 0x680Eu));
        const std::uint8_t c2 = o.readByte(add32(base, 0x680Fu));
        const bool allowed = c0 == 1u || c0 == 2u || c1 == 2u || c2 == 2u ||
                             c0 == 0u || c1 == 0u || c2 == 0u;
        clearExtra = !allowed;
    }
    if (clearExtra) o.writeByte(add32(charBase(o), 0x6836u), 0u);

    // 0x356C..0x35BD: raw minimum table. Signed class IDs participate in
    // address arithmetic exactly; only -1 is skipped.
    for (std::uint32_t stat = 0; stat < 6u; ++stat) {
        for (std::uint32_t slot = 0; slot < 3u; ++slot) {
            base = charBase(o);
            const std::int32_t cls = sx8(o.readByte(add32(base, 0x680Du + slot)));
            if (cls == -1) continue;
            const std::uint32_t minAddr = add32(0x07D4u,
                add32(stat * 15u, u32(cls)));
            const std::uint32_t valueAddr = add32(base, 0x682Au + stat * 2u);
            const std::uint8_t value = o.readByte(valueAddr);
            const std::uint8_t minimum = o.readByte(minAddr);
            if (value < minimum) o.writeByte(valueAddr, minimum);
        }
    }

    // 0x35BF..0x35F5: active class count.
    std::int32_t count = 1;
    base = charBase(o);
    if (sx8(o.readByte(add32(base, 0x680Eu))) != -1) count = 2;
    base = charBase(o);
    if (sx8(o.readByte(add32(base, 0x680Fu))) != -1) ++count;

    o.writeDword(0x711Cu, 0u);
    o.writeDword(0x7118u, 0x63u);

    // 0x360A..0x3680: per-class accumulation and minimum active level.
    for (std::int32_t slot = 0; slot < count; ++slot) {
        base = charBase(o);
        const std::int32_t cls = sx8(o.readByte(add32(base, 0x680Du + u32(slot))));
        std::uint32_t accumulated = 0u;
        std::int32_t i = 0;
        std::int32_t level;
        do {
            base = charBase(o);
            level = sx16(o.readWord(add32(base, 0x6818u + u32(slot * 2))));
            if (i >= level) break;
            std::int32_t increment = sx8(o.readByte(add32(0x0853u, u32(cls))));
            if (increment < 1) increment = 1;
            accumulated += u32(increment);
            ++i;
        } while (true);
        o.writeDword(0x711Cu, o.readDword(0x711Cu) + accumulated);
        if (level < s32(o.readDword(0x7118u))) o.writeDword(0x7118u, u32(level));
    }

    // 0x3682..0x36A3: maximum signed level.
    std::int32_t maxLevel = 0;
    for (std::int32_t slot = 0; slot < count; ++slot) {
        base = charBase(o);
        const std::int32_t level = sx16(o.readWord(add32(base, 0x6818u + u32(slot * 2))));
        if (level > maxLevel) maxLevel = level;
    }

    base = charBase(o);
    std::int32_t cls = sx8(o.readByte(add32(base, 0x680Du)));
    if (o.readByte(add32(base, 0x680Eu)) == 2u) cls = 2;

    // Signed IDIVs truncate toward zero. Count is 1..3 here.
    const std::int32_t average = s32(o.readDword(0x711Cu)) / count;
    o.writeDword(0x711Cu, u32(average));

    base = charBase(o);
    const std::uint32_t stat = o.readByte(add32(base, 0x682Eu));
    const std::int32_t group = cls / 3;
    const std::uint32_t bonusAddr = add32(0x04D4u,
        add32(mul32(u32(group), 0x4Cu), stat * 4u));
    std::int32_t bonus = s32(o.readDword(bonusAddr));
    if (bonus < 0) bonus = 0;
    const std::uint32_t scaledBonus = mul32(u32(bonus), u32(maxLevel));
    o.writeDword(0x711Cu, o.readDword(0x711Cu) + scaledBonus);
    o.writeDword(0x7118u, o.readDword(0x7118u) + scaledBonus);

    base = charBase(o);
    o.writeByte(add32(base, 0x6813u), 0x0Au);

    if (s32(o.readDword(0x711Cu)) < s32(o.readDword(0x7118u)))
        o.writeDword(0x711Cu, o.readDword(0x7118u));

    base = charBase(o);
    if (sx16(o.readWord(add32(base, 0x6816u))) < s32(o.readDword(0x7118u)))
        o.writeWord(add32(base, 0x6816u), static_cast<std::uint16_t>(o.readDword(0x7118u)));

    base = charBase(o);
    if (sx16(o.readWord(add32(base, 0x6816u))) > s32(o.readDword(0x711Cu)))
        o.writeWord(add32(base, 0x6816u), static_cast<std::uint16_t>(o.readDword(0x711Cu)));

    base = charBase(o);
    if (o.readWord(add32(base, 0x6816u)) == 0u)
        o.writeWord(add32(base, 0x6816u), 1u);

    base = charBase(o);
    if (o.readByte(add32(base, 0x6836u)) > 0x64u)
        o.writeByte(add32(base, 0x6836u), 0x64u);
}

} // namespace ravenloft::game
