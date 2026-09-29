#pragma once
#include "games/ravenloft/data/SaveGame.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct CharacterDamageContextCD44 {
    // CODE.2 0x19CC4 combat/update clock used for incoming-damage display expiry.
    std::function<std::uint32_t()> currentTick19CC4;
    // CODE.2 0xAD24(characterIndex), invoked on character death.
    std::function<void(std::size_t)> onDeathAD24;
    // Original DS:CF5A four-byte per-character dirty/refresh array.
    std::array<std::uint8_t,4>* cf5aCharacterFlags = nullptr;
    // Original paired-character globals DS:D681[2] and DS:D8A8[2].
    std::array<std::uint8_t,2>* d681PairActive = nullptr;
    std::array<std::int32_t,2>* d8a8PairCharacter = nullptr;
    // CODE.2 0x14A60(0x578) and 0x1C1DC(0x46), used by the death-state branch.
    std::function<int(int)> query14A60;
    std::function<int(int)> query1C1DC;
    // Original DS:BF44 state written to 0x6A by one death branch.
    std::int32_t* bf44 = nullptr;
    // CODE.2 0xCBC8 party-defeat path, called when all four status words match 0xD040.
    std::function<void()> allPartyDownCBC8;
};

struct CharacterDamageResultCD44 {
    int originalDamage = 0;
    int remainingDamage = 0;
    bool died = false;
    bool allPartyDown = false;
};

// CODE.2 0xCD44..0xCEE3.
class CharacterDamageCD44 {
public:
    static CharacterDamageResultCD44 apply(std::size_t characterIndex,
                                           int damage,
                                           std::array<data::CharacterRecord,4>& party,
                                           const CharacterDamageContextCD44& context);
};

} // namespace ravenloft::game
