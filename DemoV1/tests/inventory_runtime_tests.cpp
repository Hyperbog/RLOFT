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
}
