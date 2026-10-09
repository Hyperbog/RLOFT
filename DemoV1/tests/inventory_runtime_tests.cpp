#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/inventory_runtime.hpp"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
int g_inventory_test_phase=0; // diagnostic only; no gameplay mutation

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

    // Objects 3/4/5 are ordinary target items. Categories 0F/0D
    // are alternate CODE.2 211D8 hand/container placement branches.
    bytes[4U + 6U*0x41U + 0x2FU] = 0x0FU;
    bytes[4U + 7U*0x41U + 0x2FU] = 0x0DU;
    return rloft::ObjectCatalog::parse(bytes);
}

}  // namespace

int main() {
    // Source-backed regression test: make an uncaught C++ failure
    // actionable on MSVC where 0xC0000409 otherwise has no line.
    std::set_terminate([]() noexcept {
        std::fprintf(stderr,
            "inventory_runtime_tests terminated in section %d\n",
            g_inventory_test_phase);
        if(const auto ex=std::current_exception()) {
            try { std::rethrow_exception(ex); }
            catch(const std::exception& e) {
                std::fprintf(stderr,"C++ exception: %s\n",e.what());
            } catch(...) {
                std::fprintf(stderr,"Nonstandard exception\n");
            }
        }
        std::fflush(stderr);
        std::abort();
    });
    auto catalog = objects();
    rloft::InventoryRuntimeState runtime{};
    runtime.held = {-1, 0};

    auto& state0 = runtime.containers[0];
    state0.slot_count = 4;
    // CODE.2 descriptor geometry is required to reselect a
    // source-open actor grid. The old synthetic fixture left
    // columns=0, a malformed non-renderable descriptor.
    state0.ui_columns = 2;
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
    // Original 0F238 returns early for a newly empty slot. Its
    // identified bit remains stale although the item moved away.
    assert(party.bytes[known_at]==0x0AU);

    // Insert the identified item into another empty ordinary slot;
    // transfer its bit, without changing unrelated identified slots.
    const auto inserted=runtime.swap_ordinary_slot(
        party,catalog,0U,4U);
    assert(inserted==rloft::InventoryOrdinarySwapStatus::Swapped);
    assert(get_item(party,0,4U)==5);
    assert(runtime.held.object_id==-1);
    assert(runtime.held.known==0U);
    assert(party.bytes[known_at]==0x1AU);

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

    // CODE.2 117A0 -> 21404 automatically inserts an item dragged
    // onto a filled ordinary container without replacing the bag.
    set_item(party,0,9U,2);
    runtime.held={5,1U};
    const auto before_container=party.bytes;
    assert(runtime.swap_ordinary_slot(party,catalog,0U,9U)==
           rloft::InventoryOrdinarySwapStatus::InsertedInContainer);
    assert(party.bytes==before_container);
    assert(state0.items[0]==5);
    assert(state0.items[1]==-1);
    assert((state0.per_slot_state_bits & 0x01U)!=0);
    assert(runtime.held.object_id==-1);
    assert(runtime.held.known==0U);

    // CODE.2 214B8 original HUD-hand swap, independent of the
    // 12 ordinary inventory tiles. Populate both hand deadlines to
    // establish which original 4-byte field gets reset.
    set_item(party,0,14U,3);
    set_item(party,0,15U,4);
    const auto dead0 = 0x19EU;
    const auto dead1 = 0x1A2U;
    // The 26018 attack-action state words at +10B/+10F are
    // independent. The item swap must NOT clear either of them.
    const auto action0 = 0x10BU;
    const auto action1 = 0x10FU;
    for(std::size_t b=0U;b<4U;++b) {
        party.bytes[dead0+b]=0xABU;
        party.bytes[dead1+b]=0xCDU;
        party.bytes[action0+b]=0x5AU;
        party.bytes[action1+b]=0x6BU;
    }
    // Hand A contains identified item3; held unidentified item5.
    party.bytes[known_at+1U]=0x40U;
    runtime.held={5,0U};
    assert(runtime.swap_hand_slot(party,catalog,0U,0U)==
           rloft::InventoryHandSwapStatus::Swapped);
    assert(get_item(party,0,14U)==5);
    assert(get_item(party,0,15U)==4);
    assert(runtime.held.object_id==3);
    assert(runtime.held.known==1U);
    assert((party.bytes[known_at+1U] & 0x40U)==0U);
    for(std::size_t b=0U;b<4U;++b) {
        assert(party.bytes[dead0+b]==0U);
        assert(party.bytes[dead1+b]==0xCDU);
        assert(party.bytes[action0+b]==0x5AU);
        assert(party.bytes[action1+b]==0x6BU);
    }

    // Take from hand B to an empty cursor: 0F238 returns early
    // and preserves the prior hand known bit in the now empty slot.
    runtime.held={-1,0U};
    party.bytes[known_at+1U] |= 0x80U;
    assert(runtime.swap_hand_slot(party,catalog,0U,1U)==
           rloft::InventoryHandSwapStatus::Swapped);
    assert(get_item(party,0,15U)==-1);
    assert(runtime.held.object_id==4);
    assert(runtime.held.known==1U);
    assert((party.bytes[known_at+1U] & 0x80U)!=0U);
    for(std::size_t b=0U;b<4U;++b) {
        assert(party.bytes[dead1+b]==0U);
        assert(party.bytes[action0+b]==0x5AU);
        assert(party.bytes[action1+b]==0x6BU);
    }

    // Hand swap refuses equipment types 3..8, category-1
    // container menu state, and temporary spell-hand pseudo-items.
    auto unmovable = objects();
    runtime.held={2,1U};
    const auto unchanged=party.bytes;
    assert(runtime.swap_hand_slot(party,unmovable,0U,0U)==
           rloft::InventoryHandSwapStatus::Unsupported);
    assert(party.bytes==unchanged);
    assert(runtime.held.object_id==2);

    set_item(party,0,15U,0x7EU);
    runtime.held={5,1U};
    const auto pseudo=party.bytes;
    assert(runtime.swap_hand_slot(party,catalog,0U,1U)==
           rloft::InventoryHandSwapStatus::Unsupported);
    assert(party.bytes==pseudo);
    assert(runtime.held.object_id==5);

    bool hand_out_of_range=false;
    try {
        (void)runtime.swap_hand_slot(party,catalog,0U,2U);
    } catch(const std::out_of_range&) {
        hand_out_of_range=true;
    }
    assert(hand_out_of_range);

    // Inactive actor and invalid slots cannot mutate an unrelated
    // party member's inventory, held item, or identification bitset.
    assert(runtime.swap_ordinary_slot(party,catalog,2U,1U)==
           rloft::InventoryOrdinarySwapStatus::Unsupported);
    bool rejected=false;
    try {
        (void)runtime.swap_ordinary_slot(party,catalog,0U,12U);
    } catch (const std::out_of_range&) { rejected=true; }
    assert(rejected);

    // CODE.2 211D8 short path: held item -> first free ordinary
    // slot in the clicked actor panel, no full modal required.
    // This direct insertion intentionally does NOT update the
    // item-known bitfield; it copies only the object ID.
    rloft::PartyImage quick_party{};
    quick_party.bytes.fill(0xFFU);
    quick_party.bytes[0x10U]=0U;
    quick_party.bytes[0x11U]=0U;
    set_item(quick_party,0U,0U,3);
    quick_party.bytes[0x12BU]=0x02U; // stale known bit at empty slot1
    runtime.held={4,0U};
    assert(runtime.quick_stash_held(quick_party,catalog,0U)==
           rloft::InventoryQuickStashStatus::Stashed);
    assert(get_item(quick_party,0U,1U)==4);
    assert(quick_party.bytes[0x12BU]==0x02U);
    assert(runtime.held.object_id==-1);
    assert(runtime.held.known==0U);

    // First occupied free slot is skipped; verified 12-slot scan
    // chooses slot 2 on the next eligible insertion.
    runtime.held={5,1U};
    assert(runtime.quick_stash_held(quick_party,catalog,0U)==
           rloft::InventoryQuickStashStatus::Stashed);
    assert(get_item(quick_party,0U,2U)==5);
    assert((quick_party.bytes[0x12BU] & 0x04U)==0U);
    assert(runtime.held.object_id==-1);

    assert(runtime.quick_stash_held(quick_party,catalog,0U)==
           rloft::InventoryQuickStashStatus::NothingHeld);

    // The original inventory-full branch must preserve the held item.
    for(std::size_t slot=0U; slot<12U; ++slot)
        set_item(quick_party,0U,slot,3);
    const auto full_party=quick_party.bytes;
    runtime.held={4,1U};
    assert(runtime.quick_stash_held(quick_party,catalog,0U)==
           rloft::InventoryQuickStashStatus::Full);
    assert(quick_party.bytes==full_party);
    assert(runtime.held.object_id==4);
    assert(runtime.held.known==1U);

    // Category0F and 0D take alternate original paths. Do not
    // coerce either into an ordinary stash.
    set_item(quick_party,0U,10U,-1);
    for(const auto special : {6,7}) {
        runtime.held={static_cast<std::int16_t>(special),1U};
        const auto prior=quick_party.bytes;
        assert(runtime.quick_stash_held(quick_party,catalog,0U)==
               rloft::InventoryQuickStashStatus::Unsupported);
        assert(quick_party.bytes==prior);
        assert(runtime.held.object_id==special);
    }
    assert(runtime.quick_stash_held(quick_party,catalog,2U)==
           rloft::InventoryQuickStashStatus::Unsupported);

    // Full original category-1 inventory:
    // placing a bag in an empty hand opens its live descriptor; taking
    // items from its UI, reinserting, and exchanging preserve IDs,
    // known bits and unrelated party fields.
    auto nested_party=party;
    set_item(nested_party,0U,14U,-1);
    set_item(nested_party,0U,15U,-1);
    state0.items={3,-1,-1,-1};
    state0.per_slot_state_bits=0x01U;
    state0.capacity_pounds=8;
    state0.subtype=0;
    runtime.open_container.reset();
    runtime.held={2,1U};
    assert(runtime.swap_hand_slot(nested_party,catalog,0U,0U)==
           rloft::InventoryHandSwapStatus::Swapped);
    assert(get_item(nested_party,0U,14U)==2);
    assert(runtime.open_container.has_value());
    assert(runtime.open_container->descriptor_index==0U);
    // Original CODE.2 21693 initializes the independent actor
    // context directly to subtype/index/phase3/object-lowbyte.
    assert(runtime.party_container_ui_contexts[0U].raw==
        (std::array<std::uint8_t,4>{{0U,0U,3U,2U}}));
    assert(runtime.held.object_id==-1);
    assert(runtime.swap_open_container_slot(
        nested_party,catalog,0U)==
           rloft::InventoryContainerStatus::Taken);
    assert(state0.items[0]==-1);
    assert(runtime.held.object_id==3);
    assert(runtime.held.known==1U);
    assert(runtime.swap_open_container_slot(
        nested_party,catalog,1U)==
           rloft::InventoryContainerStatus::Inserted);
    assert(state0.items[1]==3);
    assert((state0.per_slot_state_bits&0x02U)!=0U);
    assert(runtime.held.object_id==-1);

    // Non-empty cursor exchanges with one individual container slot;
    // both previous identified flags and inventory are maintained.
    runtime.held={4,0U};
    assert(runtime.swap_open_container_slot(
        nested_party,catalog,1U)==
           rloft::InventoryContainerStatus::Inserted);
    assert(state0.items[1]==4);
    assert((state0.per_slot_state_bits&0x02U)==0U);
    assert(runtime.held.object_id==3);
    assert(runtime.held.known==1U);
    assert(runtime.container_weight_half_units(catalog,0U)==0U);

    // An already occupied container hand can be picked up when the
    // cursor is empty. Its open view closes with the transferred bag.
    runtime.held={-1,0U};
    assert(runtime.swap_hand_slot(nested_party,catalog,0U,0U)==
           rloft::InventoryHandSwapStatus::Swapped);
    assert(get_item(nested_party,0U,14U)==-1);
    assert(runtime.held.object_id==2);
    assert(!runtime.open_container.has_value());
    // CODE.2 21648 transfer clears ALL 4 bytes; do not
    // conflate with the animated 110B4 post-close state.
    assert(runtime.party_container_ui_contexts[0U].raw==
        (std::array<std::uint8_t,4>{{0xFFU,0xFFU,0xFFU,0xFFU}}));
    assert(runtime.open_hand_container(
        nested_party,catalog,0U,0U)==
           rloft::InventoryContainerStatus::NoChange);

    // Original specialized containers use exact item-category gates.
    const auto specialized=objects();
    state0.subtype=4; // Ivory Scroll Case: category 0x0E only
    runtime.held={3,0U}; // ordinary category 0x00
    assert(runtime.insert_held_into_container_object(
        specialized,2)==rloft::InventoryContainerStatus::Incompatible);
    assert(runtime.held.object_id==3);
    state0.subtype=0;
    runtime.held={-1,0U};

    // Invalid descriptor, lost equipment and unsupported source
    // special-state must leave cursor and party bytes untouched.
    set_item(nested_party,0U,14U,2);
    assert(runtime.open_hand_container(
        nested_party,catalog,0U,0U)==
           rloft::InventoryContainerStatus::Opened);
    assert(runtime.party_container_ui_contexts[0U].fully_open());
    // The 21648 direct replacement explicitly clears all four,
    // unlike the 110B4 animated close.
    auto transfer_probe=runtime;
    transfer_probe.reset_actor_context_21648(0U);
    assert(transfer_probe.party_container_ui_contexts[0U].raw==
        (std::array<std::uint8_t,4>{{
            0xFFU,0xFFU,0xFFU,0xFFU}}));
    assert(!transfer_probe.open_container);
    // The shared descriptor's first slot was emptied by the
    // earlier nested transfer. Populate a real item before the
    // subsequent click asserts source 11C3C status Taken.
    state0.items[0]=3;
    state0.per_slot_state_bits=static_cast<std::uint8_t>(
        state0.per_slot_state_bits|0x01U);
    g_inventory_test_phase=445;
    // Original 6FA0 has four independent actors. Opening a
    // second active actor's hand container must not close or
    // rewrite actor 0's phase3 view, and its slot clicks must
    // select the clicked actor without modifying the other.
    const auto source_actor0=runtime.party_container_ui_contexts[0U].raw;
    const auto actor1_base=rloft::PartyImage::kActorSize;
    nested_party.bytes[actor1_base+0x10U]=0U;
    nested_party.bytes[actor1_base+0x11U]=0U;
    set_item(nested_party,1U,14U,2);
    set_item(nested_party,1U,15U,-1);
    assert(runtime.open_hand_container(
        nested_party,catalog,1U,0U)==
           rloft::InventoryContainerStatus::Opened);
    assert(runtime.open_container && runtime.open_container->actor==1U);
    assert(runtime.party_container_ui_contexts[0U].raw==source_actor0);
    assert(runtime.party_container_ui_contexts[1U].fully_open());
    assert(runtime.select_container_actor(
        nested_party,catalog,0U)==
           rloft::InventoryContainerStatus::Opened);
    assert(runtime.open_container->actor==0U);
    assert(runtime.party_container_ui_contexts[1U].fully_open());
    assert(runtime.swap_actor_container_slot(
        nested_party,catalog,1U,0U)==
           rloft::InventoryContainerStatus::Taken);
    assert(runtime.open_container->actor==1U);
    assert(runtime.party_container_ui_contexts[0U].raw==source_actor0);
    assert(runtime.party_container_ui_contexts[1U].fully_open());
    g_inventory_test_phase=472;
    // An invalid actor selection must leave the previously
    // selected native interaction context untouched.
    const auto selection_before=runtime.open_container->actor;
    assert(runtime.select_container_actor(
        nested_party,catalog,2U)==
           rloft::InventoryContainerStatus::Unsupported);
    assert(runtime.open_container->actor==selection_before);
    runtime.close_container();
    assert(runtime.party_container_ui_contexts[1U].inactive());
    assert(runtime.party_container_ui_contexts[0U].fully_open());
    assert(runtime.select_container_actor(
        nested_party,catalog,0U)==
           rloft::InventoryContainerStatus::Opened);
    set_item(nested_party,1U,14U,-1);
    g_inventory_test_phase=486;
    // The original actor0 container's test fixture was modified
    // by the shared state-index 0 nested take above; reset its
    // cursor/contents before the existing single-actor checks.
    runtime.held={-1,0U};
    state0.items={3,-1,-1,-1};
    state0.per_slot_state_bits=0x01U;
    runtime.close_container();
    assert(runtime.party_container_ui_contexts[0U].inactive());
    assert(runtime.party_container_ui_contexts[1U].inactive());
    assert(runtime.open_hand_container(
        nested_party,catalog,0U,0U)==
           rloft::InventoryContainerStatus::Opened);
    runtime.close_container();
    assert(runtime.party_container_ui_contexts[0U].raw==
        (std::array<std::uint8_t,4>{{0xFFU,0xFFU,0U,0xFFU}}));
    assert(!runtime.open_container);
    assert(runtime.open_hand_container(
        nested_party,catalog,0U,0U)==
           rloft::InventoryContainerStatus::Opened);
    // Failed attempts to open a different empty/non-container
    // hand must not discard a valid 6FA0 view belonging to the
    // actor's other hand.
    const auto raw_before_invalid=
        runtime.party_container_ui_contexts[0U].raw;
    assert(runtime.open_hand_container(
        nested_party,catalog,0U,1U)==
           rloft::InventoryContainerStatus::NoChange);
    assert(runtime.open_container.has_value());
    assert(runtime.open_container->hand==0U);
    assert(runtime.party_container_ui_contexts[0U].raw==
           raw_before_invalid);
    set_item(nested_party,0U,14U,-1);
    runtime.held={5,1U};
    const auto unsupported_cursor=runtime.held;
    assert(runtime.swap_open_container_slot(
        nested_party,catalog,2U)==
           rloft::InventoryContainerStatus::Unsupported);
    assert(runtime.held.object_id==unsupported_cursor.object_id);
    assert(state0.items[2]==-1);

    g_inventory_test_phase=526;
    // Original CODE.2 12180 all nine equipment locations:
    // keep category routing separate from the source-rendered
    // click geometry. Every branch below is on original party
    // item[12..20], not an invented UI-only equipment array.
    auto equipment_bytes=std::vector<std::uint8_t>(
        4U+40U*0x41U,0U);
    put32(equipment_bytes,0U,40U);
    const auto set_category=[&](std::size_t id,std::uint8_t value) {
        equipment_bytes[4U+id*0x41U+0x2FU]=value;
    };
    set_category(10U,3U); // head
    set_category(11U,4U); // armor
    set_category(12U,5U); // ring
    set_category(13U,6U); // accessory
    set_category(14U,7U); // bracer
    set_category(15U,8U); // robe
    set_category(16U,9U); // weapon
    set_category(17U,9U); // two-handed weapon
    equipment_bytes[4U+17U*0x41U+0x35U]=2U;
    set_category(18U,1U); // quiver class
    equipment_bytes[4U+18U*0x41U+0x31U]=1U;
    const auto equipped_catalog=
        rloft::ObjectCatalog::parse(equipment_bytes);
    for(const auto spec:{
        std::array<int,3>{10,17,17},
        std::array<int,3>{11,16,16},
        std::array<int,3>{12,12,13},
        std::array<int,3>{13,18,18},
        std::array<int,3>{14,20,20},
        std::array<int,3>{15,16,16},
        std::array<int,3>{16,14,15}}) {
        const auto& definition=equipped_catalog.at(
            static_cast<std::size_t>(spec[0]));
        assert(rloft::InventoryRuntimeState::
            original_equipment_target(definition,
                static_cast<std::int16_t>(spec[0]),0U)==
            static_cast<std::size_t>(spec[1]));
        assert(rloft::InventoryRuntimeState::
            original_equipment_target(definition,
                static_cast<std::int16_t>(spec[0]),1U)==
            static_cast<std::size_t>(spec[2]));
    }

    auto equip_party=party;
    equip_party.bytes[0x15U]=0U; // Fighter
    equip_party.bytes[0x16U]=0xFFU;
    equip_party.bytes[0x17U]=0xFFU;
    for(std::size_t slot=12U;slot<21U;++slot)
        set_item(equip_party,0U,slot,-1);
    rloft::InventoryRuntimeState equip{};
    equip.held={12,1U};
    assert(equip.swap_equipment_slot(
        equip_party,equipped_catalog,0U,12U)==
        rloft::InventoryEquipmentStatus::Swapped);
    assert(get_item(equip_party,0U,12U)==12);
    assert((equip_party.bytes[0x12BU+1U]&0x10U)!=0U);
    assert(equip.held.object_id==-1);
    equip.held={12,0U};
    assert(equip.swap_equipment_slot(
        equip_party,equipped_catalog,0U,13U)==
        rloft::InventoryEquipmentStatus::Swapped);
    assert(get_item(equip_party,0U,13U)==12);
    assert((equip_party.bytes[0x12BU+1U]&0x20U)==0U);
    assert(equip.held.object_id==-1);
    assert(equip.swap_equipment_slot(
        equip_party,equipped_catalog,0U,12U)==
        rloft::InventoryEquipmentStatus::Swapped);
    assert(equip.held.object_id==12);
    assert(equip.held.known==1U);
    equip.held={11,0U};
    assert(equip.swap_equipment_slot(
        equip_party,equipped_catalog,0U,17U)==
        rloft::InventoryEquipmentStatus::WrongSlot);
    assert(get_item(equip_party,0U,17U)==-1);
    assert(equip.swap_equipment_slot(
        equip_party,equipped_catalog,0U,16U)==
        rloft::InventoryEquipmentStatus::Swapped);
    assert(get_item(equip_party,0U,16U)==11);

    g_inventory_test_phase=605;
    // CODE.2 12128: a single class-code-4 actor cannot equip
    // category-4 body armor; a class-5 actor can only use the
    // five source-whitelisted armor object IDs.
    set_item(equip_party,0U,16U,-1);
    equip_party.bytes[0x15U]=4U;
    equip.held={11,0U};
    assert(equip.swap_equipment_slot(
        equip_party,equipped_catalog,0U,16U)==
        rloft::InventoryEquipmentStatus::ClassRestricted);
    assert(equip.held.object_id==11);
    equip_party.bytes[0x15U]=0U;
    equip_party.bytes[0x16U]=5U;
    assert(equip.swap_equipment_slot(
        equip_party,equipped_catalog,0U,16U)==
        rloft::InventoryEquipmentStatus::ClassRestricted);
    // The original source whitelist includes item ID 0x0C (12).
    // Re-tag the synthetic object as body armor to test that branch.
    set_category(12U,4U);
    const auto whitelisted=rloft::ObjectCatalog::parse(
        equipment_bytes);
    equip.held={12,0U};
    assert(equip.swap_equipment_slot(
        equip_party,whitelisted,0U,16U)==
        rloft::InventoryEquipmentStatus::Swapped);
    assert(get_item(equip_party,0U,16U)==12);

    // Source raw item +35h low 2 bits encode a paired-hand class;
    // paired-hand-exclusive class 2 cannot coexist with offhand item.
    equip_party.bytes[0x15U]=0U;
    equip_party.bytes[0x16U]=0xFFU;
    set_item(equip_party,0U,15U,16);
    equip.held={17,0U};
    assert(equip.swap_equipment_slot(
        equip_party,equipped_catalog,0U,14U)==
        rloft::InventoryEquipmentStatus::Unsupported);
    assert(get_item(equip_party,0U,14U)==-1);
    set_item(equip_party,0U,15U,-1);
    assert(equip.swap_equipment_slot(
        equip_party,equipped_catalog,0U,14U)==
        rloft::InventoryEquipmentStatus::Swapped);
    assert(get_item(equip_party,0U,14U)==17);
    assert(equip.held.object_id==-1);

    // Burden is the 21 source equipment items + nested descriptor
    // contents, all measured in OBJECTS half-pound units.
    auto weight_bytes=equipment_bytes;
    weight_bytes[4U+16U*0x41U+0x30U]=5U;
    weight_bytes[4U+18U*0x41U+0x30U]=3U;
    weight_bytes[4U+10U*0x41U+0x30U]=9U;
    const auto weight_catalog=rloft::ObjectCatalog::parse(weight_bytes);
    set_item(equip_party,0U,0U,16);
    set_item(equip_party,0U,1U,10);
    set_item(equip_party,0U,2U,18);
    equip.containers[1U].slot_count=4U;
    equip.containers[1U].ui_columns=2U;
    equip.containers[1U].capacity_pounds=8U;
    equip.containers[1U].items={16,-1,-1,-1};
    assert(equip.actor_weight_half_units(
        equip_party,weight_catalog,0U)==22U);
    assert(equip.actor_weight_half_units(
        equip_party,weight_catalog,2U)==0U);

    g_inventory_test_phase=667;
    // Original CODE.2 11C3C 4CC89..4CCC4: category-1 nested
    // containers are admissible with DIFFERENT descriptor subtypes.
    // Source 4CC76..4CC83 rejects generic pouch/sack items only
    // for raw hand-pair flags 2/3; class 1 is still accepted.
    std::vector<std::uint8_t> nested_bytes(4U+16U*65U,0U);
    put32(nested_bytes,0U,16U);
    const auto nested_object=[&](std::size_t id,int descriptor,
                                 std::uint8_t half_weight) {
        const auto at=4U+id*65U;
        nested_bytes[at+0x2FU]=1U;
        nested_bytes[at+0x30U]=half_weight;
        nested_bytes[at+0x31U]=static_cast<std::uint8_t>(descriptor);
    };
    nested_object(2U,0,1U); // outer pouch, source state0
    nested_object(8U,1,2U); // different-subtype sack, state1
    nested_object(9U,2,2U); // same-subtype pouch, state2
    nested_bytes[4U+10U*65U+0x30U]=5U;
    nested_bytes[4U+10U*65U+0x35U]=1U; // hand-pair class1 accepted
    nested_bytes[4U+11U*65U+0x30U]=3U;
    nested_bytes[4U+11U*65U+0x35U]=2U; // class2 prohibited
    const auto nested_catalog=
        rloft::ObjectCatalog::parse(nested_bytes);
    rloft::InventoryRuntimeState nested{};
    for(std::size_t i=0U;i<3U;++i) {
        nested.containers[i].slot_count=4U;
        nested.containers[i].ui_columns=2U;
        nested.containers[i].capacity_pounds=20U;
        nested.containers[i].items={-1,-1,-1,-1};
    }
    nested.containers[0U].subtype=0U;
    nested.containers[1U].subtype=1U;
    nested.containers[2U].subtype=0U;
    nested.containers[1U].items[0U]=10;
    nested.held={8,1U};
    assert(nested.insert_held_into_container_object(
        nested_catalog,2)==
        rloft::InventoryContainerStatus::Inserted);
    assert(nested.containers[0U].items[0U]==8);
    assert(nested.containers[0U].per_slot_state_bits==1U);
    assert(nested.container_weight_half_units(nested_catalog,0U)==7U);
    assert(nested.held.object_id==-1);

    // Attempting to insert the outer pouch into its nested child
    // would create a cycle. Both cursor and source slots must survive.
    nested.held={2,0U};
    assert(nested.insert_held_into_container_object(
        nested_catalog,8)==
        rloft::InventoryContainerStatus::Incompatible);
    assert(nested.held.object_id==2);
    assert(nested.containers[0U].items[0U]==8);
    assert(nested.containers[1U].items[0U]==10);

    // Distinct instances but equal source subtype are not nestable.
    nested.held={9,0U};
    assert(nested.put_held_in_container_slot(
        nested_catalog,2,1U)==
        rloft::InventoryContainerStatus::Incompatible);
    assert(nested.containers[0U].items[1U]==-1);

    g_inventory_test_phase=726;
    // Original CODE.2 4CD85..4CE18 subtracts the FULL recursive
    // outgoing burden, then adds the FULL incoming item's burden.
    // An existing nested bag with weight 2+5=7 half-pounds can be
    // swapped for item10 weight5 at capacity3lb (6 half-pounds).
    nested.containers[0U].capacity_pounds=3U;
    nested.held={10,0U};
    assert(nested.put_held_in_container_slot(
        nested_catalog,2,0U)==
        rloft::InventoryContainerStatus::Inserted);
    assert(nested.containers[0U].items[0U]==10);
    assert(nested.held.object_id==8);
    assert(nested.held.known==1U);
    assert(nested.container_weight_half_units(nested_catalog,0U)==5U);

    g_inventory_test_phase=740;
    // Original inventory 117A0: the object in the backpack
    // slot is 0x35 TOME OF EVIL ARTIFACTS AND CURSED ITEMS.
    // Held 0x61..0x64 are four different PARCHMENT FRAGMENT
    // objects. 11866 calls 1C124(index0..3), which ORs exactly
    // one bit into original DGROUP 22200 persistent event vector.
    // The original 1A344(-1,0) clears the hand/cursor. The Tome,
    // its known bit and all other story flags stay untouched.
    std::vector<std::uint8_t> special_bytes(4U+202U*65U,0U);
    put32(special_bytes,0U,202U);
    const auto special_catalog=
        rloft::ObjectCatalog::parse(special_bytes);
    rloft::InventoryRuntimeState special_inventory{};
    rloft::PartyImage special_party{};
    special_party.bytes.fill(0xFFU);
    special_party.bytes[0x10U]=0U;
    special_party.bytes[0x11U]=0U;
    set_item(special_party,0U,0U,0x35);
    special_party.bytes[0x12BU]=0xA5U;
    std::array<std::uint8_t,32> story_flags{};
    story_flags.fill(0x80U);
    const auto initial_party=special_party.bytes;
    for(int fragment=0x61; fragment<=0x64; ++fragment) {
        special_inventory.held={
            static_cast<std::int16_t>(fragment),1U};
        assert(special_inventory.swap_ordinary_slot(
            special_party,special_catalog,0U,0U,&story_flags)==
            rloft::InventoryOrdinarySwapStatus::ConsumedStoryFragment);
        assert(special_inventory.held.object_id==-1);
        assert(special_inventory.held.known==0U);
        assert(special_party.bytes==initial_party);
        assert(story_flags[0U]==
            static_cast<std::uint8_t>(
                0x80U|((1U<<(fragment-0x61+1))-1U)));
        for(std::size_t i=1U;i<story_flags.size();++i)
            assert(story_flags[i]==0x80U);
    }
    // Applying an already-recorded fragment is idempotent on
    // the story flags, but still consumes the held fragment.
    special_inventory.held={0x61,0U};
    assert(special_inventory.swap_ordinary_slot(
        special_party,special_catalog,0U,0U,&story_flags)==
        rloft::InventoryOrdinarySwapStatus::ConsumedStoryFragment);
    assert(story_flags[0U]==0x8FU);
    assert(special_party.bytes==initial_party);

    // No attached original 0x22200 vector means the runtime
    // cannot safely discard a quest fragment.
    special_inventory.held={0x64,1U};
    const auto before_story=story_flags;
    assert(special_inventory.swap_ordinary_slot(
        special_party,special_catalog,0U,0U)==
        rloft::InventoryOrdinarySwapStatus::Unsupported);
    assert(special_inventory.held.object_id==0x64);
    assert(story_flags==before_story);
    assert(special_party.bytes==initial_party);

    // The original checks CURRENT slot == 0x35. Holding a Tome
    // is not itself the special event: the Tome can be put in
    // a free ordinary slot and taken back through 117A0.
    special_inventory.held={-1,0U};
    assert(special_inventory.swap_ordinary_slot(
        special_party,special_catalog,0U,0U,&story_flags)==
        rloft::InventoryOrdinarySwapStatus::Swapped);
    assert(special_inventory.held.object_id==0x35);
    assert(get_item(special_party,0U,0U)==-1);
    assert(special_inventory.swap_ordinary_slot(
        special_party,special_catalog,0U,0U,&story_flags)==
        rloft::InventoryOrdinarySwapStatus::Swapped);
    assert(get_item(special_party,0U,0U)==0x35);
    assert(special_inventory.held.object_id==-1);

    // The special fragment IDs do not automatically fire when
    // the destination is an unrelated ordinary object.
    set_item(special_party,0U,1U,3);
    special_inventory.held={0x61,1U};
    const auto flags_prior_to_normal_swap=story_flags;
    assert(special_inventory.swap_ordinary_slot(
        special_party,special_catalog,0U,1U,&story_flags)==
        rloft::InventoryOrdinarySwapStatus::Swapped);
    assert(get_item(special_party,0U,1U)==0x61);
    assert(special_inventory.held.object_id==3);
    assert(story_flags==flags_prior_to_normal_swap);

    g_inventory_test_phase=823;
    // CODE.2 11808..11852: a Mage Spell Book in the source
    // slot 0xA6 delegates held Mage scrolls 0xA7..0xC9 to
    // 11630, checking spell class/level and capacity. That
    // branch must never become a generic destructive swap
    // until full spell-list mutation is reconstructed.
    set_item(special_party,0U,2U,0xA6);
    for(const int held_id : {-1,0xA7,0xC9,0xCA}) {
        special_inventory.held={
            static_cast<std::int16_t>(held_id),1U};
        const auto unchanged=special_party.bytes;
        assert(special_inventory.swap_ordinary_slot(
            special_party,special_catalog,0U,2U,&story_flags)==
            rloft::InventoryOrdinarySwapStatus::Unsupported);
        assert(special_party.bytes==unchanged);
        assert(special_inventory.held.object_id==held_id);
        assert(story_flags==flags_prior_to_normal_swap);
    }
    // Conversely, holding the book and clicking an ordinary
    // empty cell is NOT the special 11808 source branch.
    set_item(special_party,0U,2U,-1);
    special_inventory.held={0xA6,0U};
    assert(special_inventory.swap_ordinary_slot(
        special_party,special_catalog,0U,2U,&story_flags)==
        rloft::InventoryOrdinarySwapStatus::Swapped);
    assert(get_item(special_party,0U,2U)==0xA6);
    assert(special_inventory.held.object_id==-1);

    g_inventory_test_phase=850;
    // Original CODE.2 11630 scroll table is in object-1
    // at physical 3AF24+EEB0, NOT object-2 at 75F24+EEB0.
    // Six 2-DWORD [last,first] pairs cover all 35 Mage
    // scrolls, with bank sizes 7,6,10,4,2,6.
    constexpr std::size_t source_at=0x3AF24U+0xEEB0U;
    constexpr std::size_t cap_at=0x75F24U+0x6153U;
    constexpr std::size_t text_ptr_at=0x75F24U+0x6D9CU;
    std::vector<std::uint8_t> code2_spell(text_ptr_at+12U,0U);
    // These are the original CODE.2 object-2 pointer offsets and
    // exact message bytes recovered independently from the binary.
    put32(code2_spell,text_ptr_at,0x5A4U);
    put32(code2_spell,text_ptr_at+4U,0x5D4U);
    put32(code2_spell,text_ptr_at+8U,0x5E4U);
    const auto original_msg=[&](std::size_t offset,
                                 const std::string& value) {
        const auto first=0x75F24U+offset;
        for(std::size_t i=0;i<value.size();++i)
            code2_spell[first+i]=static_cast<std::uint8_t>(value[i]);
        code2_spell[first+value.size()]=0U;
    };
    original_msg(0x5A4U,
        "you do not understand this spell to scribe it.");
    original_msg(0x5D4U," SPELL SCRIBED.");
    original_msg(0x5E4U,
        "that spell is already in your spell book.");
    const std::array<std::uint32_t,12> ranges{{
        173,167,179,174,189,180,193,190,195,194,201,196
    }};
    for(std::size_t i=0;i<ranges.size();++i)
        put32(code2_spell,source_at+i*4U,ranges[i]);
    const std::array<std::uint8_t,21> capacities{{
        9,6,6,6,6,6,6,6,6,6,7,7,7,9,9,11,11,14,18,18,18
    }};
    for(std::size_t i=0;i<capacities.size();++i)
        code2_spell[cap_at+i]=capacities[i];
    const auto scribe_source=
        rloft::MageScrollScribeSource::from_code2(code2_spell);
    assert(scribe_source.id_ranges==ranges);
    assert(scribe_source.intelligence_capacities==capacities);
    assert(scribe_source.cannot_understand_text==
        "you do not understand this spell to scribe it.");
    assert(scribe_source.success_suffix==" SPELL SCRIBED.");
    assert(scribe_source.already_known_text==
        "that spell is already in your spell book.");
    // A zero-filled/incorrect object-1 range block cannot be
    // interpreted as a source-certified Mage spell table.
    code2_spell[source_at]=0U;
    bool invalid_source=false;
    try {
        (void)rloft::MageScrollScribeSource::from_code2(
            code2_spell);
    } catch(const std::runtime_error&) {
        invalid_source=true;
    }
    assert(invalid_source);

    // Use a complete synthetic OBJECTS ID corpus with original
    // Mage book (0xA6) and scrolls (0xA7..0xC9). All spell
    // matrix cells start 0xFF: low nibble 0xF means unavailable.
    rloft::InventoryRuntimeState scribe_inventory{};
    scribe_inventory.mage_scroll_source=scribe_source;
    rloft::PartyImage scribe_party{};
    scribe_party.bytes.fill(0xFFU);
    scribe_party.bytes[0x10U]=0U;
    scribe_party.bytes[0x11U]=0U;
    scribe_party.bytes[0x39U]=12U; // CODE.2 6153[12]=7
    set_item(scribe_party,0U,0U,0xA6);
    std::array<std::uint8_t,32> scribe_flags{};
    scribe_flags.fill(0x10U);
    const auto scribe_original=scribe_party.bytes;

    // 11630 sets story flag 0xBA BEFORE capacity and duplicate
    // checks. A duplicate returns false and keeps the scroll.
    scribe_party.bytes[0x6AU]=0U; // Armor already known
    scribe_inventory.held={0xA7,1U};
    assert(scribe_inventory.swap_ordinary_slot(
        scribe_party,special_catalog,0U,0U,&scribe_flags)==
        rloft::InventoryOrdinarySwapStatus::MageScrollRejected);
    assert(scribe_flags[0xBAU/8U]==0x14U);
    assert(scribe_party.bytes[0x6AU]==0U);
    assert(scribe_inventory.held.object_id==0xA7);
    assert(scribe_inventory.held.known==1U);
    assert(scribe_inventory.mage_scribe_feedback.has_value());
    const auto known_feedback=*scribe_inventory.mage_scribe_feedback;
    assert(known_feedback.kind==
        rloft::MageScribeFeedbackKind::AlreadyKnown);
    assert(known_feedback.message("")==
        "that spell is already in your spell book.");
    // No observed 30700 tick: NEVER substitute host time.
    assert(!known_feedback.deadline_6da8.has_value());
    assert(!scribe_inventory.expire_scribe_feedback_13184(0xFFFFFFFFU));
    scribe_party.bytes[0x6AU]=0xFFU;

    // 116CD requires the target low nibble to be 0xF.
    // A success zeroes the ENTIRE matrix byte (including
    // any high nibble), does not remove the book and consumes
    // held scroll and known byte. Party known bits persist.
    scribe_inventory.held={0xA7,1U};
    assert(scribe_inventory.swap_ordinary_slot(
        scribe_party,special_catalog,0U,0U,&scribe_flags,100U)==
        rloft::InventoryOrdinarySwapStatus::LearnedMageSpell);
    assert(scribe_inventory.mage_scribe_feedback.has_value());
    const auto scribbled=*scribe_inventory.mage_scribe_feedback;
    assert(scribbled.kind==rloft::MageScribeFeedbackKind::Scribed);
    assert(scribbled.t_spell_name_index==0x42U);
    assert(scribbled.message("ARMOR")=="ARMOR SPELL SCRIBED.");
    assert(scribbled.pending_6dac==1U);
    assert(scribbled.deadline_6da8==154U);
    assert(!scribe_inventory.expire_scribe_feedback_13184(154U));
    assert(scribe_inventory.mage_scribe_feedback.has_value());
    assert(scribe_inventory.expire_scribe_feedback_13184(155U));
    assert(!scribe_inventory.mage_scribe_feedback.has_value());
    assert(!scribe_inventory.expire_scribe_feedback_13184(156U));
    assert(scribe_party.bytes[0x6AU]==0U);
    assert(get_item(scribe_party,0U,0U)==0xA6);
    assert(scribe_party.bytes[0x12BU]==scribe_original[0x12BU]);
    assert(scribe_inventory.held.object_id==-1);
    assert(scribe_inventory.held.known==0U);
    for(std::size_t i=0;i<scribe_flags.size();++i)
        assert(scribe_flags[i]==(i==0xBAU/8U?0x14U:0x10U));

    // Different source range banks index distinct 10-byte levels.
    for(const auto mapping : {
            std::array<int,3>{174,1,0},
            std::array<int,3>{189,2,9},
            std::array<int,3>{194,4,0},
            std::array<int,3>{201,5,5}}) {
        const auto at=0x6AU+static_cast<std::size_t>(
            mapping[1]*10+mapping[2]);
        assert(scribe_party.bytes[at]==0xFFU);
        scribe_inventory.held={
            static_cast<std::int16_t>(mapping[0]),0U};
        assert(scribe_inventory.swap_ordinary_slot(
            scribe_party,special_catalog,0U,0U,&scribe_flags)==
            rloft::InventoryOrdinarySwapStatus::LearnedMageSpell);
        assert(scribe_party.bytes[at]==0U);
        assert(scribe_inventory.held.object_id==-1);
    }

    // Capacity at actor+0x39 indexes the original 6153
    // 21-entry byte table. Exactly six known spells at INT3
    // prevents the seventh; the 0xBA flag still commits.
    scribe_party.bytes[0x39U]=3U;
    for(std::size_t i=0;i<6U;++i)
        scribe_party.bytes[0x6AU+i]=0U;
    scribe_party.bytes[0x6AU+6U]=0xFFU;
    scribe_inventory.held={173,1U};
    scribe_flags[0xBAU/8U]=0U;
    assert(scribe_inventory.swap_ordinary_slot(
        scribe_party,special_catalog,0U,0U,&scribe_flags)==
        rloft::InventoryOrdinarySwapStatus::MageScrollRejected);
    assert(scribe_flags[0xBAU/8U]==0x04U);
    assert(scribe_party.bytes[0x6AU+6U]==0xFFU);
    assert(scribe_inventory.held.object_id==173);
    assert(scribe_inventory.mage_scribe_feedback.has_value());
    assert(scribe_inventory.mage_scribe_feedback->kind==
        rloft::MageScribeFeedbackKind::CapacityReached);
    assert(scribe_inventory.mage_scribe_feedback->message("")==
        "you do not understand this spell to scribe it.");

    // DOS 32-bit addition is NOT wrap-corrected; 13184 uses
    // unsigned comparison, even across the DWORD boundary.
    scribe_inventory.held={173,1U};
    assert(scribe_inventory.swap_ordinary_slot(
        scribe_party,special_catalog,0U,0U,&scribe_flags,
        0xFFFFFFF0U)==
        rloft::InventoryOrdinarySwapStatus::MageScrollRejected);
    assert(scribe_inventory.mage_scribe_feedback->deadline_6da8==
        0x26U);
    assert(scribe_inventory.expire_scribe_feedback_13184(
        0xFFFFFFF1U));
    assert(!scribe_inventory.mage_scribe_feedback.has_value());
    scribe_inventory.reset_scribe_feedback_13118();
    assert(!scribe_inventory.mage_scribe_feedback.has_value());

    // Without persistent 0x22200 state or without authentic
    // CODE.2 tables, no scroll/matrix/held mutation is safe.
    const auto before_no_flag=scribe_party.bytes;
    assert(scribe_inventory.swap_ordinary_slot(
        scribe_party,special_catalog,0U,0U)==
        rloft::InventoryOrdinarySwapStatus::Unsupported);
    assert(scribe_party.bytes==before_no_flag);
    scribe_inventory.mage_scroll_source.reset();
    assert(scribe_inventory.swap_ordinary_slot(
        scribe_party,special_catalog,0U,0U,&scribe_flags)==
        rloft::InventoryOrdinarySwapStatus::Unsupported);
    assert(scribe_party.bytes==before_no_flag);

    g_inventory_test_phase=1038;
    // CODE.2 raw WORD +35 low2 class1 is allowed, class2 rejected.
    nested.containers[0U].capacity_pounds=20U;
    nested.held={10,0U};
    assert(nested.put_held_in_container_slot(
        nested_catalog,2,1U)==
        rloft::InventoryContainerStatus::Inserted);
    nested.held={11,0U};
    assert(nested.put_held_in_container_slot(
        nested_catalog,2,2U)==
        rloft::InventoryContainerStatus::Incompatible);
    assert(nested.containers[0U].items[2U]==-1);
    assert(nested.held.object_id==11);
}
