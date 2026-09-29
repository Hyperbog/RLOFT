#pragma once

#include "games/ravenloft/data/WorkLevel.h"
#include "games/ravenloft/game/ObjectOwnershipF390F4B0.h"
#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>

namespace ravenloft::game {

struct GroundObjectPickupContext21704 {
    std::function<bool(int)> ownsObjectF4B0;
    std::function<int(int)> query1C1DC;
    std::function<void(int)> setEventStateBF44;
};

struct GroundObjectPickupResult21704 {
    bool picked=false;
    std::uint16_t objectId=0xFFFFu;
    std::uint8_t heldState=0;
};

// CODE.2 0x21704 opening-relevant world-object pickup. It transfers the
// selected runtime object's id/state into DS:8600/8602 through the 0x1A344
// state boundary, compacts the 22-byte runtime array, and preserves the
// original object-id BF44 side effects.
class GroundObjectPickup21704 {
public:
    static GroundObjectPickupResult21704 execute(std::size_t objectIndex,
                                                  std::vector<data::RuntimePlacement22>& runtimeObjects,
                                                  HeldObjectState1A344& held,
                                                  const GroundObjectPickupContext21704& context={});
};

} // namespace ravenloft::game
