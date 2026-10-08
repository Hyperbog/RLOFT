#include "rloft/inventory_runtime.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace rloft {
namespace {

constexpr std::size_t kCode2Object2FileStart = 0x75F24U;
constexpr std::size_t kContainerDescriptorsAt = 0x7330U;
constexpr std::size_t kContainerDescriptorSize = 10U;

std::uint16_t u16(const std::uint8_t* p) noexcept {
    return static_cast<std::uint16_t>(p[0]) |
           (static_cast<std::uint16_t>(p[1]) << 8U);
}
std::uint32_t u32(const std::uint8_t* p) noexcept {
    return static_cast<std::uint32_t>(p[0]) |
           (static_cast<std::uint32_t>(p[1]) << 8U) |
           (static_cast<std::uint32_t>(p[2]) << 16U) |
           (static_cast<std::uint32_t>(p[3]) << 24U);
}
std::int16_t s16(const std::uint8_t* p) noexcept {
    return static_cast<std::int16_t>(u16(p));
}

std::int16_t party_item(
    const PartyImage& party,
    std::size_t actor,
    std::size_t slot) noexcept {
    const auto at =
        actor * PartyImage::kActorSize +
        0x40U + slot * 2U;
    return s16(party.bytes.data() + at);
}

void set_party_item(
    PartyImage& party,
    std::size_t actor,
    std::size_t slot,
    std::int16_t item) noexcept {
    const auto at =
        actor * PartyImage::kActorSize +
        0x40U + slot * 2U;
    const auto value =
        static_cast<std::uint16_t>(item);
    party.bytes[at] =
        static_cast<std::uint8_t>(value);
    party.bytes[at + 1U] =
        static_cast<std::uint8_t>(value >> 8U);
}

bool contains_recursive(
    const InventoryRuntimeState& runtime,
    const ObjectCatalog& objects,
    std::int16_t root,
    std::int16_t target) {
    if (root == target) return true;
    if (root < 0) return false;

    const auto& definition =
        objects.at(static_cast<std::size_t>(root));
    if (definition.item_category != 1U) {
        return false;
    }

    const int state_index =
        definition.container_state_index;
    if (state_index < 0 ||
        state_index >=
            static_cast<int>(
                InventoryRuntimeState::kRuntimeDescriptorCount)) {
        return false;
    }

    const auto& container =
        runtime.containers[
            static_cast<std::size_t>(state_index)];
    for (const auto child : container.items) {
        if (child == -1) continue;
        if (contains_recursive(
                runtime,
                objects,
                child,
                target)) {
            return true;
        }
    }
    return false;
}

bool remove_recursive(
    InventoryRuntimeState& runtime,
    const ObjectCatalog& objects,
    std::int16_t root,
    std::int16_t target) {
    if (root < 0) return false;

    const auto& definition =
        objects.at(static_cast<std::size_t>(root));
    if (definition.item_category != 1U) {
        return false;
    }

    const int state_index =
        definition.container_state_index;
    if (state_index < 0 ||
        state_index >=
            static_cast<int>(
                InventoryRuntimeState::kRuntimeDescriptorCount)) {
        return false;
    }

    auto& container =
        runtime.containers[
            static_cast<std::size_t>(state_index)];

    for (std::size_t i = 0;
         i < container.items.size();
         ++i) {
        const auto child = container.items[i];
        if (child == -1) continue;

        if (child == target) {
            container.items[i] = -1;

            // CODE.2 0F290 uses an 8-bit descriptor bitset. Shifts are
            // machine-masked; supplied container sizes are <= 8.
            const auto mask =
                static_cast<std::uint8_t>(
                    1U << (i & 7U));
            container.per_slot_state_bits =
                static_cast<std::uint8_t>(
                    container.per_slot_state_bits &
                    static_cast<std::uint8_t>(~mask));
            return true;
        }

        if (remove_recursive(
                runtime,
                objects,
                child,
                target)) {
            return true;
        }
    }

    return false;
}

}  // namespace

InventoryRuntimeState InventoryRuntimeState::fresh(
    const GameSource& source) {
    const auto code2 = source.read("CODE.2");
    if (code2.size() < kCode2Object2FileStart) {
        throw std::runtime_error(
            "CODE.2 is truncated before data object");
    }

    InventoryRuntimeState out{};
    out.held = {-1, 0};

    for (std::size_t i = 0;
         i < kRuntimeDescriptorCount;
         ++i) {
        const auto desc_at =
            kCode2Object2FileStart +
            kContainerDescriptorsAt +
            i * kContainerDescriptorSize;
        if (desc_at + kContainerDescriptorSize >
            code2.size()) {
            throw std::runtime_error(
                "CODE.2 container descriptor exceeds file");
        }

        const auto* d = code2.data() + desc_at;
        auto& container = out.containers[i];
        container.slot_count = d[0];
        container.ui_columns = d[1];
        container.capacity_pounds = d[2];
        container.ui_x_offset = d[3];
        container.subtype = d[4];
        container.per_slot_state_bits = d[9];

        const auto contents_dgroup =
            static_cast<std::size_t>(u32(d + 5));
        if (container.slot_count == 0U) {
            if (contents_dgroup != 0U) {
                throw std::runtime_error(
                    "zero-slot CODE.2 container has contents pointer");
            }
            continue;
        }

        const auto contents_at =
            kCode2Object2FileStart +
            contents_dgroup;
        const auto bytes =
            static_cast<std::size_t>(
                container.slot_count) * 2U;
        if (contents_at + bytes > code2.size()) {
            throw std::runtime_error(
                "CODE.2 container contents exceed file");
        }

        container.items.reserve(
            container.slot_count);
        for (std::size_t slot = 0;
             slot < container.slot_count;
             ++slot) {
            container.items.push_back(
                s16(code2.data() +
                    contents_at + slot * 2U));
        }
    }

    return out;
}

InventoryOrdinarySwapStatus InventoryRuntimeState::swap_ordinary_slot(
    PartyImage& party,
    const ObjectCatalog& objects,
    std::size_t actor,
    std::size_t slot) {
    if (actor >= PartyImage::kActorCount || slot >= 12U) {
        throw std::out_of_range("CODE.2 ordinary inventory actor/slot");
    }
    if (!party.active(actor)) {
        return InventoryOrdinarySwapStatus::Unsupported;
    }

    const auto current = party_item(party,actor,slot);
    const auto incoming = held.object_id;
    if (current == -1 && incoming == -1) {
        return InventoryOrdinarySwapStatus::NoChange;
    }

    // The original 117A0 calls context-dependent 21404 for a held
    // item over a container, and has special paths for the 0x35
    // potion and 0xA6 spell-item families. Do not silently turn
    // those interactions into an ordinary exchange.
    if ((current >= 0x7E && current <= 0x86) ||
        (incoming >= 0x7E && incoming <= 0x86) ||
        current == 0x35 || current == 0xA6 ||
        incoming == 0x35 || incoming == 0xA6 ||
        (current < -1) || (incoming < -1)) {
        return InventoryOrdinarySwapStatus::Unsupported;
    }
    if (current >= 0) {
        const auto& definition =
            objects.at(static_cast<std::size_t>(current));
        if (incoming != -1 && definition.item_category == 1U) {
            return InventoryOrdinarySwapStatus::Unsupported;
        }
    }
    if (incoming >= 0) {
        (void)objects.at(static_cast<std::size_t>(incoming));
    }

    const auto base = actor * PartyImage::kActorSize;
    const auto bit = std::uint32_t{1} << slot;
    const auto known_at = base + 0x12BU;
    const std::uint32_t before =
        static_cast<std::uint32_t>(party.bytes[known_at]) |
        (static_cast<std::uint32_t>(party.bytes[known_at+1U]) << 8U) |
        (static_cast<std::uint32_t>(party.bytes[known_at+2U]) << 16U) |
        (static_cast<std::uint32_t>(party.bytes[known_at+3U]) << 24U);
    const auto old_known =
        static_cast<std::uint8_t>((before & bit) != 0U);
    // CODE.2 1194C writes the new object before calling 0F238.
    // Original 0F238 immediately returns when that slot is -1, so
    // taking an item leaves its old known bit stale in the empty slot.
    const auto updated =
        incoming == -1
            ? before
            : static_cast<std::uint32_t>(
                  (before & ~bit) |
                  (held.known != 0U ? bit : 0U));

    // All model preflight occurs before the first write. Commit the
    // authoritative 21-item party record, four-byte known bitset and
    // held cursor identity together. Do not synthesize 117A0's
    // separate renderer refresh/sound effects in this core routine.
    set_party_item(party,actor,slot,incoming);
    for (unsigned b=0U;b<4U;++b) {
        party.bytes[known_at+b] =
            static_cast<std::uint8_t>(updated >> (8U*b));
    }
    held = {current,old_known};
    return InventoryOrdinarySwapStatus::Swapped;
}

bool InventoryRuntimeState::has_object(
    const PartyImage& party,
    const ObjectCatalog& objects,
    std::int16_t target) const {
    if (held.object_id != -1 &&
        contains_recursive(
            *this,
            objects,
            held.object_id,
            target)) {
        return true;
    }

    for (std::size_t actor = 0;
         actor < PartyImage::kActorCount;
         ++actor) {
        if (!party.active(actor)) continue;

        for (std::size_t slot = 0;
             slot < 21U;
             ++slot) {
            const auto item =
                party_item(party, actor, slot);
            if (item == -1) continue;
            if (contains_recursive(
                    *this,
                    objects,
                    item,
                    target)) {
                return true;
            }
        }
    }
    return false;
}

void InventoryRuntimeState::remove_object(
    PartyImage& party,
    const ObjectCatalog& objects,
    std::int16_t target) {
    if (held.object_id != -1) {
        if (held.object_id == target) {
            held = {-1, 0};
            return;
        }
        if (remove_recursive(
                *this,
                objects,
                held.object_id,
                target)) {
            return;
        }
    }

    for (std::size_t actor = 0;
         actor < PartyImage::kActorCount;
         ++actor) {
        if (!party.active(actor)) continue;

        for (std::size_t slot = 0;
             slot < 21U;
             ++slot) {
            const auto item =
                party_item(party, actor, slot);
            if (item == -1) continue;

            if (item == target) {
                set_party_item(
                    party,
                    actor,
                    slot,
                    -1);
                // CODE.2 then calls 0F238(actor, slot, 0). Because the
                // slot is already -1, that routine returns before changing
                // the actor item-known bitfield. Preserve that ordering.
                break;
            }

            if (remove_recursive(
                    *this,
                    objects,
                    item,
                    target)) {
                return;
            }
        }
    }
}

}  // namespace rloft
