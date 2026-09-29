#pragma once
#include <array>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct PlayerCollisionState2D004 {
    std::int32_t partyXFixed = 0;       // DS:255D0
    std::int32_t partyYFixed = 0;       // DS:255D4
    std::int32_t savedXFixed = 0;       // DS:2B634
    std::int32_t savedYFixed = 0;       // DS:2B630
    std::int32_t facingDegrees = 0;     // DS:C658
    std::uint16_t currentLevel = 0;     // DS:C6C8
    std::uint32_t stateBF44 = 0;        // DS:BF44
};

struct PlayerCollisionTrigger2D004Callbacks {
    // DS:25622[cellCode] lookup. Kept explicit because this table is runtime
    // level state populated elsewhere by the original loader.
    std::function<std::int16_t(std::uint8_t cellCode)> mappedCellObjectId;
    std::function<bool(std::uint16_t)> queryFlag1C1DC;

    // Exact external call boundary at CODE.2 0x2CEB8.
    std::function<void()> sequence2CEB8;
    std::function<void(std::uint16_t level)> loadLevel181B4;
    std::function<void(std::int32_t facing)> setFacing1C0C0;

    // False-flag trap path call sequence.
    std::function<void()> refresh1F1C8;
    std::function<void()> refresh1EA04;
    std::function<void()> refresh1EA78;
    std::function<void(int a,int b,std::int32_t c,int d)> draw2E510;
    std::function<std::int32_t()> resourceBase258EC;
    std::function<std::uint16_t(std::size_t partyIndex)> partyStatusWord;
    std::function<int()> roll1d2;
    std::function<void(std::size_t partyIndex,int damage)> damageCharacterCD44;
};

class PlayerCollisionTrigger2D004 {
public:
    // Direct reconstruction of CODE.2 0x2D004..0x2D232.
    static int evaluate(std::uint8_t cellCode,
                        PlayerCollisionState2D004& state,
                        const PlayerCollisionTrigger2D004Callbacks& cb);
};

} // namespace ravenloft::game
