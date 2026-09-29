#include "CharacterTable5F4C.h"

#include <cstddef>
#include <cstdint>

namespace ravenloft::game {
namespace {

std::int16_t readWord(
    std::span<const std::uint8_t> bytes,
    std::size_t offset) noexcept {
    return static_cast<std::int16_t>(
        static_cast<std::uint16_t>(bytes[offset]) |
        (static_cast<std::uint16_t>(bytes[offset + 1u]) << 8u));
}

} // namespace

void CharacterTable5F4C::apply(
    std::array<CharacterTableRecord5F4C, 2>& characters,
    std::span<const std::uint8_t> table0988,
    std::span<const std::uint8_t> recordBlock7128) noexcept {
    // 0x5F55..0x5FEC: populate both character records.
    for (std::size_t character = 0; character < 2; ++character) {
        const std::uint8_t classId =
            CharacterClassId5D90::classify(
                characters[character].fields680D);

        const std::size_t base =
            static_cast<std::size_t>(classId) * 0xA8u +
            static_cast<std::size_t>(
                characters[character].field680C) * 0x1Cu;

        // The first four table WORDs are written in the original non-linear
        // destination order.
        characters[character].word6854 =
            readWord(table0988, base + 4u);
        characters[character].word6856 =
            readWord(table0988, base + 6u);
        characters[character].word6858 =
            readWord(table0988, base + 2u);
        characters[character].word685A =
            readWord(table0988, base);

        for (std::size_t slot = 0; slot < 10; ++slot) {
            characters[character].words6838[slot] =
                readWord(
                    table0988,
                    base + 8u + slot * 2u);
        }
    }

    // 0x5FF2..0x604A: only when both characters have the same class-ID and
    // the same 680C byte does character 1 receive the record-block filtering.
    const std::uint8_t class0 =
        CharacterClassId5D90::classify(
            characters[0].fields680D);
    const std::uint8_t class1 =
        CharacterClassId5D90::classify(
            characters[1].fields680D);

    if (class0 != class1 ||
        characters[0].field680C != characters[1].field680C) {
        return;
    }

    for (std::size_t slot = 0; slot < 10; ++slot) {
        const std::int32_t recordIndex =
            characters[1].words6838[slot];

        // Original address arithmetic:
        // recordBlock7128 + recordIndex * 65 + 0x2F.
        const std::size_t offset =
            static_cast<std::size_t>(
                recordIndex * 65 + 0x2F);

        if (recordBlock7128[offset] == 1u) {
            characters[1].words6838[slot] = -1;
        }
    }
}

} // namespace ravenloft::game
