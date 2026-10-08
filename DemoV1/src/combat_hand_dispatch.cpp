#include "rloft/combat_hand_dispatch.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace rloft {
namespace {

std::uint16_t word(const std::uint8_t* p) noexcept {
    return static_cast<std::uint16_t>(p[0]) |
           (static_cast<std::uint16_t>(p[1]) << 8U);
}

std::uint32_t dword(const std::uint8_t* p) noexcept {
    return static_cast<std::uint32_t>(p[0]) |
           (static_cast<std::uint32_t>(p[1]) << 8U) |
           (static_cast<std::uint32_t>(p[2]) << 16U) |
           (static_cast<std::uint32_t>(p[3]) << 24U);
}

bool special_id(std::int16_t id) noexcept {
    // CODE.2 23F50..23F96 direct object-ID special cases.
    switch (id) {
        case 0x2E:
        case 0x2F:
        case 0x41:
        case 0x57:
        case 0x59:
        case 0x5A:
            return true;
        default:
            return false;
    }
}

} // namespace

CombatHandDispatchPlan CombatHandDispatchModel::inspect(
    const PartyImage& party,
    const ObjectCatalog& objects,
    std::size_t actor,
    std::size_t hand) {
    if (actor >= PartyImage::kActorCount || hand >= 2U) {
        throw std::out_of_range("CODE.2 214B8 actor/hand index");
    }

    CombatHandDispatchPlan plan{};
    const auto base = actor * PartyImage::kActorSize;
    const auto* bytes = party.bytes.data() + base;

    // 214D9: actor status bit 15 blocks all hand paths. The separate
    // 216CD AND F240h gate applies specifically to use/attack.
    const auto status = word(bytes + 0x10U);
    if ((status & 0x8000U) != 0U) {
        plan.gate = CombatHandDispatchGate::InactiveActor;
        return plan;
    }
    if ((status & 0xF240U) != 0U) {
        plan.gate = CombatHandDispatchGate::ActorConditionBlocked;
        return plan;
    }

    // CODE.2 26037: [0x219EB + actor*0x248 + hand*4].
    // PartyImage begins at DGROUP 0x218E0, so this is +0x10B.
    // Unlike 214B8's item-swap readiness state at +0x19E, this
    // field is read by the actual action dispatcher.
    plan.original_action_state = dword(bytes + 0x10BU + hand*4U);
    if (plan.original_action_state != 0U) {
        plan.gate = CombatHandDispatchGate::ActionStateBusy;
        return plan;
    }

    // 26045: signed 16-bit item[14+hand]. An empty hand is a
    // valid original unarmed attack category 0x0A (26064).
    plan.object_id =
        static_cast<std::int16_t>(word(bytes + 0x40U +
                                       (14U+hand)*2U));
    if (plan.object_id < -1) {
        throw std::out_of_range("CODE.2 26018 invalid signed hand item");
    }
    if (plan.object_id == -1) {
        plan.category = 0x0AU;
    } else {
        const auto& item =
            objects.at(static_cast<std::size_t>(plan.object_id));
        plan.category = item.item_category;
    }

    // 26069 invokes 23F50 before normal category routing.
    // That predicate selects six hardcoded IDs or category D/F.
    plan.special_item =
        plan.object_id >= 0 &&
        (special_id(plan.object_id) ||
         plan.category == 0x0DU ||
         plan.category == 0x0FU);
    if (plan.special_item) {
        plan.route = CombatHandDispatchRoute::SpecialItem;
    } else if (plan.category >= 9U && plan.category <= 11U) {
        plan.route = CombatHandDispatchRoute::PlayerAttack;
    } else if (plan.category == 0x0EU) {
        plan.route = CombatHandDispatchRoute::CategoryE;
    } else {
        plan.route = CombatHandDispatchRoute::OtherAction;
    }
    plan.gate = CombatHandDispatchGate::AvailableForPairValidation;
    return plan;
}

} // namespace rloft
