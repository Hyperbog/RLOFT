#include "CharacterTable5D90.h"

#include <cstddef>
#include <cstdint>

namespace ravenloft::game {

std::uint8_t CharacterClassCode5D90::compute(
    const std::array<std::uint8_t, 3>& classes,
    std::uint8_t result) noexcept {
    if (static_cast<std::int8_t>(classes[1]) == -1) {
        return classes[0];
    }

    if (static_cast<std::int8_t>(classes[2]) != -1) {
        if (classes[0] == 5u ||
            classes[1] == 5u ||
            classes[2] == 5u) {
            return 0x0Du;
        }

        return 0x0Eu;
    }

    if (classes[0] == 0u) {
        if (classes[1] == 5u) result = 6u;
        if (classes[1] == 3u) result = 7u;
        if (classes[1] == 4u) result = 8u;
    }

    if (classes[0] == 5u) {
        if (classes[1] == 0u) result = 6u;
        if (classes[1] == 3u) result = 9u;
        if (classes[1] == 4u) result = 0x0Cu;
    }

    if (classes[0] == 2u) {
        result = 0x0Au;
    }

    if (classes[0] == 3u) {
        if (classes[1] == 0u) result = 7u;
        if (classes[1] == 5u) result = 9u;
        if (classes[1] == 4u) result = 0x0Bu;
        if (classes[1] == 2u) result = 0x0Au;
    }

    if (classes[0] == 4u) {
        if (classes[1] == 0u) result = 8u;
        if (classes[1] == 3u) result = 0x0Bu;
        if (classes[1] == 5u) result = 0x0Cu;
    }

    return result;
}

void CharacterTable5F4C::initialize(
    CharacterTableState5F4C& state) noexcept {
    std::uint8_t incomingBl = 0;

    // 0x5F55..0x5FEC: populate the first two character records.
    for (std::size_t character = 0;
         character < 2;
         ++character) {
        const std::uint8_t code =
            CharacterClassCode5D90::compute(
                state.characters[character].classes680D,
                incomingBl);

        // EBX itself is the character index in the original loop.
        incomingBl =
            static_cast<std::uint8_t>(character + 1u);

        const std::size_t row =
            (static_cast<std::size_t>(code) * 6u +
             static_cast<std::size_t>(
                 state.characters[character].race680C)) *
            14u;

        auto& record = state.characters[character];

        // Exact WORD ordering from DS:0988 + row.
        record.words6854[0] = state.table0988[row + 2u];
        record.words6854[1] = state.table0988[row + 3u];
        record.words6854[2] = state.table0988[row + 1u];
        record.words6854[3] = state.table0988[row];

        for (std::size_t index = 0;
             index < 10;
             ++index) {
            record.words6838[index] =
                state.table0988[row + 4u + index];
        }
    }

    // 0x5FF2..0x600A. At this point EBX was 2 before the first helper call;
    // the second helper receives the first helper's result in BL.
    const std::uint8_t code0 =
        CharacterClassCode5D90::compute(
            state.characters[0].classes680D,
            2u);

    const std::uint8_t code1 =
        CharacterClassCode5D90::compute(
            state.characters[1].classes680D,
            code0);

    if (code0 != code1 ||
        state.characters[0].race680C !=
            state.characters[1].race680C) {
        return;
    }

    // 0x6019..0x604A: duplicate filtering for character two.
    for (auto& recordIndex :
         state.characters[1].words6838) {
        const std::int32_t signedIndex =
            static_cast<std::int16_t>(recordIndex);

        const std::int64_t offset =
            static_cast<std::int64_t>(signedIndex) * 65 +
            0x2F;

        // Valid shipped table entries point inside the loaded record block.
        if (offset >= 0 &&
            static_cast<std::size_t>(offset) <
                state.recordBlock7128.size() &&
            state.recordBlock7128[
                static_cast<std::size_t>(offset)] == 1u) {
            recordIndex = 0xFFFFu;
        }
    }
}

} // namespace ravenloft::game
