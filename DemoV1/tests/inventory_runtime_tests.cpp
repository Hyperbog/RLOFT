#ifdef NDEBUG
#undef NDEBUG
#endif
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

    // Objects 3/4/5 are ordinary target items. Categories 0F/0D
    // are alternate CODE.2 211D8 hand/container placement branches.
    bytes[4U + 6U*0x41U + 0x2FU] = 0x0FU;
    bytes[4U + 7U*0x41U + 0x2FU] = 0x0DU;
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
    assert(state0.items[0]==-1);
    assert(state0.items[1]==5);
    assert((state0.per_slot_state_bits & 0x02U)!=0);
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
    set_item(nested_party,0U,14U,-1);
    runtime.held={5,1U};
    const auto unsupported_cursor=runtime.held;
    assert(runtime.swap_open_container_slot(
        nested_party,catalog,2U)==
           rloft::InventoryContainerStatus::Unsupported);
    assert(runtime.held.object_id==unsupported_cursor.object_id);
    assert(state0.items[2]==-1);

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
        equip_party,weight_catalog,0U)>=27U);
    assert(equip.actor_weight_half_units(
        equip_party,weight_catalog,2U)==0U);
}
