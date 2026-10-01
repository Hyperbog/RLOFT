#include "CharacterProgress3208.h"

#include <array>
#include <cstddef>
#include <cstdint>

namespace ravenloft::game {
namespace {
constexpr std::array<std::array<std::uint16_t, 20>, 6> kProgress057C{{
    {{0,2,4,8,16,32,64,125,250,500,750,1000,1250,1500,1750,2000,2250,2500,2750,3000}},
    {{0,2,4,9,18,36,75,150,300,600,900,1200,1500,1800,2100,2400,2700,3000,3300,3600}},
    {{0,2,4,9,18,36,75,150,300,600,900,1200,1500,1800,2100,2400,2700,3000,3300,3600}},
    {{0,1,3,6,13,27,55,110,225,450,675,900,1125,1350,1575,1800,2025,2055,2475,2700}},
    {{0,2,5,10,20,40,60,90,135,250,375,750,1125,1500,1875,2250,2625,3000,3375,3750}},
    {{0,1,2,5,10,20,40,70,110,160,220,440,660,880,1100,1320,1540,1760,1980,2200}},
}};
}

void CharacterProgress3208::run(const CharacterProgressOps3208& o) {
    const std::uint32_t selected = o.readDword(0x04CCu);
    const std::uint32_t base = selected * 0x248u;

    std::int32_t count = 1;
    if (static_cast<std::int8_t>(o.readByte(base + 0x680Eu)) != -1) count = 2;
    if (static_cast<std::int8_t>(o.readByte(base + 0x680Fu)) != -1) ++count;

    for (std::int32_t slot = 0; slot < count; ++slot) {
        // 0x3256..0x3273: signed IDIV, then a DWORD store.
        const std::int32_t share = 0x88BC / count;
        o.writeDword(base + 0x681Eu + static_cast<std::uint32_t>(slot) * 4u,
                     static_cast<std::uint32_t>(share));

        std::int32_t level = 0;
        for (;;) {
            // MOVSX class byte; the signed value participates directly in the
            // address calculation. No class/table bounds check exists.
            const std::int32_t cls = static_cast<std::int8_t>(
                o.readByte(base + 0x680Du + static_cast<std::uint32_t>(slot)));
            const std::uint32_t tableAddress =
                0x057Cu + static_cast<std::uint32_t>(cls * 40 + level * 2);
            const std::uint16_t threshold = o.readWord(tableAddress);

            // Original reloads the stored DWORD and performs unsigned DIV 1000.
            const std::uint32_t value = o.readDword(
                base + 0x681Eu + static_cast<std::uint32_t>(slot) * 4u);
            if (value / 1000u < threshold) break;
            ++level;
            if (level >= 20) break;
        }
        o.writeWord(base + 0x6818u + static_cast<std::uint32_t>(slot) * 2u,
                    static_cast<std::uint16_t>(level));
    }
}

void CharacterProgress3208::initialize(CharacterProgressState3208& state) noexcept {
    std::int32_t count = 1;
    if (static_cast<std::int8_t>(state.fields680D[1]) != -1) count = 2;
    if (static_cast<std::int8_t>(state.fields680D[2]) != -1) ++count;
    for (std::int32_t slot = 0; slot < count; ++slot) {
        const auto i = static_cast<std::size_t>(slot);
        state.dwords681E[i] = static_cast<std::uint32_t>(0x88BC / count);
        const std::int32_t cls = static_cast<std::int8_t>(state.fields680D[i]);
        std::uint16_t level = 0;
        // Valid structured states use the six table rows. Raw/invalid states
        // must use run(), which preserves adjacent-memory reads.
        if (cls >= 0 && cls < 6) {
            while (level < 20u && state.dwords681E[i] / 1000u >=
                   kProgress057C[static_cast<std::size_t>(cls)][level]) ++level;
        }
        state.words6818[i] = level;
    }
}

} // namespace ravenloft::game
