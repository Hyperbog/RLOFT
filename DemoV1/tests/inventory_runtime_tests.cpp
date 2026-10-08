#include "rloft/inventory_runtime.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

void put16(
    std::vector<std::uint8_t>& b,
    std::size_t o,
    std::uint16_t v) {
    b[o] = static_cast<std::uint8_t>(v);
    b[o + 1U] =
        static_cast<std::uint8_t>(v >> 8U);
}
void put32(
    std::vector<std::uint8_t>& b,
    std::size_t o,
    std::uint32_t v) {
    for (int i = 0; i < 4; ++i) {
        b[o + static_cast<std::size_t>(i)] =
            static_cast<std::uint8_t>(
                v >> (i * 8));
    }
}
void set_item(
    rloft::PartyImage& party,
    std::size_t actor,
    std::size_t slot,
    std::int16_t value) {
    const auto at =
        actor * rloft::PartyImage::kActorSize +
        0x40U + slot * 2U;
    const auto raw =
        static_cast<std::uint16_t>(value);
    party.bytes[at] =
        static_cast<std::uint8_t>(raw);
    party.bytes[at + 1U] =
        static_cast<std::uint8_t>(raw >> 8U);
}
std::int16_t get_item(
    const rloft::PartyImage& party,
    std::size_t actor,
    std::size_t slot) {
    const auto at =
        actor * rloft::PartyImage::kActorSize +
        0x40U + slot * 2U;
    return static_cast<std::int16_t>(
        static_cast<std::uint16_t>(
            party.bytes[at] |
            (static_cast<std::uint16_t>(
                 party.bytes[at + 1U]) << 8U)));
}

rloft::ObjectCatalog objects() {
    std::vector<std::uint8_t> bytes(
        4U + 10U * 0x41U, 0);
    put32(bytes, 0, 10);

    // Object 2 is a category-1 container using state index 0.
    const auto c = 4U + 2U * 0x41U;
    bytes[c + 0x2F] = 1;
    bytes[c + 0x31] = 0;

    // Objects 3/4/5 are ordinary target items.
    return rloft::ObjectCatalog::parse(bytes);
}

}  // namespace

int main() {
    auto catalog = objects();
    rloft::InventoryRuntimeState runtime{};
    runtime.held = {-1, 0};

    auto& state0 = runtime.containers[0];
    state0.slot_count = 4;
    state0.items = {{3, -1, -1, -1}};
    state0.per_slot_state_bits = 1;

    rloft::PartyImage party{};
    party.bytes.fill(0xFF);

    // Actor 0 active; remaining records stay empty/inactive.
    party.bytes[0x10] = 0;
    party.bytes[0x11] = 0;
    set_item(party, 0, 0, 2);

    assert(runtime.has_object(
        party, catalog, 2));
    assert(runtime.has_object(
        party, catalog, 3));
    assert(!runtime.has_object(
        party, catalog, 4));

    // Held-item search precedes party search.
    runtime.held = {4, 1};
    assert(runtime.has_object(
        party, catalog, 4));
    runtime.remove_object(
        party, catalog, 4);
    assert(runtime.held.object_id == -1);
    assert(runtime.held.known == 0);

    // Nested removal clears the container per-slot state bit.
    runtime.remove_object(
        party, catalog, 3);
    assert(state0.items[0] == -1);
    assert(state0.per_slot_state_bits == 0);

    // Direct party removal clears the slot but intentionally does not touch
    // the stale actor item-known bit.
    set_item(party, 0, 1, 5);
    const auto known_at = 0x12BU;
    party.bytes[known_at] = 0x02;
    runtime.remove_object(
        party, catalog, 5);
    assert(get_item(party, 0, 1) == -1);
    assert((party.bytes[known_at] & 0x02U) != 0U);

    // CODE.2 117A0 ordinary-grid exchange: identified state follows
    // the object from the actor's 32-bit +12B bitset onto the held cursor.
    // Actor 0 slot 3 occupied and identified, cursor empty.
    set_item(party,0,3,5);
    party.bytes[known_at]=0x0AU; // slots 1 and 3 known
    runtime.held={-1,0U};
    const auto taken=runtime.swap_ordinary_slot(
        party,catalog,0U,3U);
    assert(taken==rloft::InventoryOrdinarySwapStatus::Swapped);
    assert(get_item(party,0,3U)==-1);
    assert(runtime.held.object_id==5);
    assert(runtime.held.known==1);
    assert(party.bytes[known_at]==0x02U);

    // Insert the identified item into another empty ordinary slot;
    // transfer its bit, without changing unrelated identified slots.
    const auto inserted=runtime.swap_ordinary_slot(
        party,catalog,0U,4U);
    assert(inserted==rloft::InventoryOrdinarySwapStatus::Swapped);
    assert(get_item(party,0,4U)==5);
    assert(runtime.held.object_id==-1);
    assert(runtime.held.known==0U);
    assert(party.bytes[known_at]==0x12U);

    // Swapping two distinct objects also swaps both known states,
    // rather than leaving identification attached to the character slot.
    set_item(party,0,6U,4);
    runtime.held={3,0U};
    party.bytes[known_at]=0x52U; // slot6 known, slot4 known, slot1 known
    const auto exchanged=runtime.swap_ordinary_slot(
        party,catalog,0U,6U);
    assert(exchanged==rloft::InventoryOrdinarySwapStatus::Swapped);
    assert(get_item(party,0,6U)==3);
    assert(runtime.held.object_id==4);
    assert(runtime.held.known==1U);
    assert(party.bytes[known_at]==0x12U);

    // Empty->empty must be a true no-op even if a historical orphaned
    // slot-known bit survives an older 0F540 removal.
    const auto before_party=party.bytes;
    const auto before_held=runtime.held;
    runtime.held={-1,0U};
    assert(runtime.swap_ordinary_slot(party,catalog,0U,8U)==
           rloft::InventoryOrdinarySwapStatus::NoChange);
    assert(party.bytes==before_party);
    assert(runtime.held.object_id==-1);
    runtime.held=before_held;

    // The original does not transfer spell-hand pseudo-objects.
    set_item(party,0,7U,0x7EU);
    const auto before_protected=party.bytes;
    const auto held_protected=runtime.held;
    assert(runtime.swap_ordinary_slot(party,catalog,0U,7U)==
           rloft::InventoryOrdinarySwapStatus::Unsupported);
    assert(party.bytes==before_protected);
    assert(runtime.held.object_id==held_protected.object_id);
    assert(runtime.held.known==held_protected.known);

    // A held item over a category-1 container invokes original
    // 21404 insertion, which is not yet implemented: do not swap it.
    set_item(party,0,9U,2);
    runtime.held={5,1U};
    const auto before_container=party.bytes;
    assert(runtime.swap_ordinary_slot(party,catalog,0U,9U)==
           rloft::InventoryOrdinarySwapStatus::Unsupported);
    assert(party.bytes==before_container);
    assert(runtime.held.object_id==5);
    assert(runtime.held.known==1U);

    // Inactive actor and invalid slots cannot mutate an unrelated
    // party member's inventory, held item, or identification bitset.
    assert(runtime.swap_ordinary_slot(party,catalog,2U,1U)==
           rloft::InventoryOrdinarySwapStatus::Unsupported);
    bool rejected=false;
    try {
        (void)runtime.swap_ordinary_slot(party,catalog,0U,12U);
    } catch (const std::out_of_range&) { rejected=true; }
    assert(rejected);
}
