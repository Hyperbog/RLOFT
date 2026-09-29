#include "games/ravenloft/game/CellInteractionSoundSystem.h"
#include <array>
#include <cstdlib>

namespace ravenloft::game {
namespace {
struct SoundMapEntry { std::uint16_t wallDefinition; std::uint16_t soundIndex; };

// CODE.2 data table DS:D7A0, 34 (wall-definition,sound-index) pairs.
constexpr std::array<SoundMapEntry,34> kSoundMap{{
    {405,14},{32,15},{156,15},{281,16},{304,16},{46,17},{60,17},{181,17},
    {183,17},{235,17},{240,17},{460,17},{290,17},{403,17},{450,17},{452,17},
    {454,17},{456,17},{458,17},{308,18},{359,18},{418,18},{36,19},{131,19},
    {135,19},{207,19},{265,19},{267,19},{319,19},{401,19},{185,21},{187,21},
    {213,21},{253,21}
}};

std::uint16_t soundIndexFor(std::uint16_t wallDefinition) {
    for (const auto& e : kSoundMap)
        if (e.wallDefinition == wallDefinition) return e.soundIndex;
    return 14; // CODE.2 0x20AC8 default.
}
}

std::optional<CellInteractionSoundEvent> CellInteractionSoundSystem::select(
    const CellInteractionRecord& record,
    int partyCellX,
    int partyCellY,
    std::uint16_t wallDefinitionId,
    std::uint32_t soundResourceBase) {

    const int dx = std::abs(static_cast<int>(record.cellX) - partyCellX);
    const int dy = std::abs(static_cast<int>(record.cellY) - partyCellY);
    const int distance = dx > dy ? dx : dy; // 0x20A86..0x20A8F.

    // 0x20A92 computes 0x40 - distance*8 and skips the sound path at <=0.
    const int attenuationBase = 0x40 - distance * 8;
    if (attenuationBase <= 0) return std::nullopt;

    // Only terminal frame states take the 0x28110 queued-sound path.
    // Intermediate frames call the 0x28164 no-op stub in this executable.
    if (record.frame != 0 && record.frame != 8) return std::nullopt;

    const auto u = static_cast<std::uint16_t>(attenuationBase);
    const auto volume = static_cast<std::uint8_t>((static_cast<std::uint32_t>(u) * 255u + 32u) >> 6);
    const auto soundIndex = soundIndexFor(wallDefinitionId);

    return CellInteractionSoundEvent{
        soundResourceBase + soundIndex,
        volume,
        0,
        static_cast<std::uint8_t>(distance)
    };
}

} // namespace ravenloft::game
