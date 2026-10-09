#include "rloft/inventory_runtime.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
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
    std::size_t slot,
    std::array<std::uint8_t,32>* persistent_event_flags) {
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

    // Original 117E8..117F9 protects *current* temporary spell
    // hands; a synthetic cursor spell is also outside a normal
    // ordinary-slot swap. Never mutate invalid identifiers.
    if ((current >= 0x7E && current <= 0x86) ||
        (incoming >= 0x7E && incoming <= 0x86) ||
        current < -1 || incoming < -1) {
        return InventoryOrdinarySwapStatus::Unsupported;
    }
    // 11808..11852: the source tests only the SLOT'S 0xA6 Mage
    // Spell Book. It only accepts held 0xA7..0xC9 scrolls, and
    // invokes 11630 (spell learning, capacity, duplicate checks).
    // Do not fake spell acquisition or fall through to a swap.
    if (current == 0xA6) {
        return InventoryOrdinarySwapStatus::Unsupported;
    }
    // 11857..11891: slot 0x35 is specifically the Tome of Evil
    // Artifacts and Cursed Items, not a potion. Four distinct held
    // Parchment Fragments 0x61..0x64 invoke 1C124(fragment-0x61)
    // on the 256-bit 0x22200 event vector, then 1A344(-1,0).
    // Ordinary interactions with 0x35 and holding 0xA6 DO fall
    // through to the ordinary 117A0 swap; only this pair is special.
    if (current == 0x35 && incoming >= 0x61 &&
        incoming <= 0x64) {
        if (persistent_event_flags == nullptr)
            return InventoryOrdinarySwapStatus::Unsupported;
        (void)objects.at(static_cast<std::size_t>(current));
        (void)objects.at(static_cast<std::size_t>(incoming));
        const auto flag = static_cast<unsigned>(incoming-0x61);
        // 1C124 uses low bit of its Dword-0 vector; repeated
        // fragments simply OR the already-set bit before being
        // consumed. Preserve every other story flag.
        (*persistent_event_flags)[0] = static_cast<std::uint8_t>(
            (*persistent_event_flags)[0] | (1U<<flag));
        held={-1,0U};
        return InventoryOrdinarySwapStatus::ConsumedStoryFragment;
    }
    if (current >= 0) {
        const auto& definition =
            objects.at(static_cast<std::size_t>(current));
        if (incoming != -1 && definition.item_category == 1U) {
            // CODE.2 117A0 invokes 21404 rather than overwriting an
            // occupied ordinary container. A successful automatic
            // insertion preserves the outer actor slot exactly.
            const auto result=insert_held_into_container_object(
                objects,current);
            return result==InventoryContainerStatus::Inserted
                ? InventoryOrdinarySwapStatus::InsertedInContainer
                : InventoryOrdinarySwapStatus::Unsupported;
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

InventoryQuickStashStatus InventoryRuntimeState::quick_stash_held(
    PartyImage& party,
    const ObjectCatalog& objects,
    std::size_t actor) {
    if (actor >= PartyImage::kActorCount) {
        throw std::out_of_range("CODE.2 211D8 party actor");
    }
    if (!party.active(actor)) {
        return InventoryQuickStashStatus::Unsupported;
    }
    if (held.object_id == -1) {
        return InventoryQuickStashStatus::NothingHeld;
    }
    if (held.object_id < -1 ||
        (held.object_id >= 0x7E && held.object_id <= 0x86)) {
        return InventoryQuickStashStatus::Unsupported;
    }
    const auto& definition =
        objects.at(static_cast<std::size_t>(held.object_id));
    // 21246/212F1 routes categories 0F and 0D to alternate
    // container/hand special insertion, not this ordinary fallback.
    if (definition.item_category == 0x0FU ||
        definition.item_category == 0x0DU) {
        return InventoryQuickStashStatus::Unsupported;
    }

    // 2143F..21478 scans 12 actual actor item words. On the
    // first -1 it copies the held object and calls 2A344(-1,0).
    // It does NOT call 0F238: the destination's known-bit word is
    // deliberately untouched even if the cursor had known=1.
    for (std::size_t slot=0U; slot<12U; ++slot) {
        if (party_item(party,actor,slot) == -1) {
            set_party_item(party,actor,slot,held.object_id);
            held = {-1,0U};
            return InventoryQuickStashStatus::Stashed;
        }
    }
    // 21478: the original reports full inventory with 2C21C,
    // without losing the cursor item.
    return InventoryQuickStashStatus::Full;
}

InventoryHandSwapStatus InventoryRuntimeState::swap_hand_slot(
    PartyImage& party,
    const ObjectCatalog& objects,
    std::size_t actor,
    std::size_t hand) {
    if (actor >= PartyImage::kActorCount || hand >= 2U) {
        throw std::out_of_range("CODE.2 214B8 actor/hand");
    }
    // 214CD..214DE: original only checks actor +11h bit 80h.
    if (!party.active(actor)) {
        return InventoryHandSwapStatus::Unsupported;
    }
    const auto slot = 14U + hand;
    const auto current = party_item(party, actor, slot);
    const auto incoming = held.object_id;
    if (current == -1 && incoming == -1) {
        return InventoryHandSwapStatus::NoChange;
    }

    // 2151F..2152A rejects the *current* temporary effect IDs.
    // Also guard insertion of a temporary spell object into a hand:
    // the verified normal ID/known exchange is not its action path.
    if (current < -1 || incoming < -1 ||
        (current >= 0x7E && current <= 0x86) ||
        (incoming >= 0x7E && incoming <= 0x86)) {
        return InventoryHandSwapStatus::Unsupported;
    }
    // 2155B..2158B: category 3,4,5,6,7,8 held objects cannot
    // enter this general hand swap path. A held -1 means empty.
    if (incoming >= 0) {
        const auto& definition =
            objects.at(static_cast<std::size_t>(incoming));
        const auto category = definition.item_category;
        if (category >= 3U && category <= 8U) {
            return InventoryHandSwapStatus::Unsupported;
        }
        // CODE.2 21648..216C4 selects the container's live
        // descriptor when a category-1 object enters a hand. Only
        // allow this route into an EMPTY hand; full hand-to-hand
        // container exchange and its dynamic 6FA0 menu remain
        // separate, source-bounded interactions.
        if (category == 1U && current != -1) {
            return InventoryHandSwapStatus::Unsupported;
        }
    }
    if (current >= 0) {
        const auto& definition =
            objects.at(static_cast<std::size_t>(current));
        if (definition.item_category == 1U && incoming != -1) {
            // The original open-container hand state cannot be
            // overwritten with another item without taking its
            // distinct 21648 handler. Taking it into an empty
            // cursor is an ordinary 214B8 pickup.
            return InventoryHandSwapStatus::Unsupported;
        }
    }

    const auto base = actor * PartyImage::kActorSize;
    const auto known_at = base + 0x12BU;
    const auto bit = std::uint32_t{1} << slot;
    const std::uint32_t bits =
        static_cast<std::uint32_t>(party.bytes[known_at]) |
        (static_cast<std::uint32_t>(party.bytes[known_at+1U])<<8U) |
        (static_cast<std::uint32_t>(party.bytes[known_at+2U])<<16U) |
        (static_cast<std::uint32_t>(party.bytes[known_at+3U])<<24U);
    // 215A1 reads known BEFORE changing the hand item, even if
    // that known bit is stale from a prior empty-slot swap.
    const auto previous_known =
        static_cast<std::uint8_t>((bits & bit) != 0U);
    const auto updated =
        incoming == -1
            ? bits  // 0F238 returns early for an empty slot.
            : static_cast<std::uint32_t>(
                  (bits & ~bit) |
                  (held.known ? bit : 0U));

    // Commit authoritative party item, known word and held item only
    // after all object-category validation has completed.
    set_party_item(party, actor, slot, incoming);
    for (unsigned b=0U; b<4U; ++b) {
        party.bytes[known_at+b] =
            static_cast<std::uint8_t>(updated >> (b*8U));
    }
    held = {current, previous_known};
    // The container view may no longer refer to its old hand object.
    // The caller opens the new container explicitly, after a successful
    // original hand transfer; we never retain a stale view across swaps.
    open_container.reset();
    if(incoming>=0) {
        const auto& equipped=objects.at(
            static_cast<std::size_t>(incoming));
        if(equipped.item_category==1U &&
           equipped.container_state_index>=0) {
            const auto ignored=open_hand_container(
                party,objects,actor,hand);
            (void)ignored;
        }
    }
    // 215E9 explicitly zeroes the selected hand's four-byte
    // ready deadline. Opposite hand remains unchanged.
    // DOS character base 218E0h; 21A7Eh-218E0h=019Eh.
    // Do NOT confuse this with the separate action-state dword at
    // 219EBh-218E0h=010Bh checked by the 26018 dispatcher.
    const auto deadline_at = base + 0x19EU + hand*4U;
    for (unsigned b=0U; b<4U; ++b) {
        party.bytes[deadline_at+b] = 0U;
    }
    return InventoryHandSwapStatus::Swapped;
}

std::size_t InventoryRuntimeState::original_equipment_target(
    const ObjectDefinition& definition,std::int16_t object_id,
    std::size_t hand_side) {
    if(hand_side>=2U)
        throw std::out_of_range("CODE.2 12180 equipment hand side");
    switch(definition.item_category) {
        case 3U: return 17U;  // helmet / headgear
        case 4U:
        case 8U: return 16U;  // body armor or robe
        case 5U: return 12U+hand_side;  // two ring slots
        case 6U: return 18U;  // accessory
        case 7U: return 20U;  // bracers/gauntlets
        default: break;
    }
    // CODE.2 0F168: exact original QUIVER object-ID predicate,
    // only on side A. Other categories route to normal hand A/B.
    constexpr std::array<int,13> quivers{{
        6,265,270,288,295,305,308,309,315,317,321,326,327
    }};
    if(hand_side==0U && std::find(quivers.begin(),quivers.end(),
                                  object_id)!=quivers.end()) {
        return 19U;
    }
    return 14U+hand_side;
}

InventoryEquipmentStatus InventoryRuntimeState::swap_equipment_slot(
    PartyImage& party,const ObjectCatalog& objects,
    std::size_t actor,std::size_t slot) {
    if(actor>=PartyImage::kActorCount ||
       slot<12U || slot>20U) {
        throw std::out_of_range("CODE.2 12180 equipment actor/slot");
    }
    if(!party.active(actor))return InventoryEquipmentStatus::Unsupported;
    const auto current=party_item(party,actor,slot);
    const auto incoming=held.object_id;
    if(current==-1 && incoming==-1)
        return InventoryEquipmentStatus::NoChange;
    if(current< -1 || incoming< -1 ||
       (current>=0x7E && current<=0x86) ||
       (incoming>=0x7E && incoming<=0x86)) {
        return InventoryEquipmentStatus::Unsupported;
    }
    if(current>=0)(void)objects.at(
        static_cast<std::size_t>(current));
    if(incoming>=0) {
        const auto& item=objects.at(
            static_cast<std::size_t>(incoming));
        const auto eligible_a=original_equipment_target(
            item,incoming,0U);
        const auto eligible_b=original_equipment_target(
            item,incoming,1U);
        if(slot!=eligible_a && slot!=eligible_b)
            return InventoryEquipmentStatus::WrongSlot;
        if(item.item_category==4U) {
            // Original 12180 -> 23EAC(5) + 12128 item whitelist.
            const auto base=actor*PartyImage::kActorSize;
            const auto first=party.bytes[base+0x15U];
            const auto second=party.bytes[base+0x16U];
            const auto third=party.bytes[base+0x17U];
            if(first==4U && second==0xFFU)
                return InventoryEquipmentStatus::ClassRestricted;
            const bool has_class5=
                first==5U || second==5U || third==5U;
            constexpr std::array<int,5> allowed{{
                0x0C,0x10,0x5F,0xEC,0xEE
            }};
            if(has_class5 &&
               std::find(allowed.begin(),allowed.end(),incoming)==
                   allowed.end()) {
                return InventoryEquipmentStatus::ClassRestricted;
            }
        }
        if((slot==14U || slot==15U) &&
           (item.raw_byte_35_low2&2U)!=0U) {
            const auto other=party_item(
                party,actor,29U-slot); // 14 <-> 15
            if(other!=-1)return InventoryEquipmentStatus::Unsupported;
        }
        if(slot==14U || slot==15U) {
            const auto other=party_item(party,actor,29U-slot);
            if(other>=0 &&
               (objects.at(static_cast<std::size_t>(other))
                 .raw_byte_35_low2&2U)!=0U) {
                return InventoryEquipmentStatus::Unsupported;
            }
        }
    }

    if(slot==14U || slot==15U) {
        const auto result=swap_hand_slot(
            party,objects,actor,slot-14U);
        return result==InventoryHandSwapStatus::Swapped
            ? InventoryEquipmentStatus::Swapped
            : (result==InventoryHandSwapStatus::NoChange
               ? InventoryEquipmentStatus::NoChange
               : InventoryEquipmentStatus::Unsupported);
    }

    const auto base=actor*PartyImage::kActorSize;
    const auto known_at=base+0x12BU;
    const auto mask=std::uint32_t{1}<<slot;
    std::uint32_t bits=0U;
    for(unsigned i=0U;i<4U;++i) {
        bits|=static_cast<std::uint32_t>(
            party.bytes[known_at+i])<<(8U*i);
    }
    const auto old_known=static_cast<std::uint8_t>(
        (bits&mask)!=0U);
    // CODE.2 0F238 returns without updating known bits when
    // new item is empty; preserve exact legacy stale-bit state.
    if(incoming!=-1) {
        bits=(bits&~mask)|(held.known!=0U?mask:0U);
    }
    set_party_item(party,actor,slot,incoming);
    for(unsigned i=0U;i<4U;++i) {
        party.bytes[known_at+i]=static_cast<std::uint8_t>(
            bits>>(8U*i));
    }
    held={current,old_known};
    if(open_container &&
       open_container->actor==actor &&
       open_container->object_id==current) {
        open_container.reset();
    }
    return InventoryEquipmentStatus::Swapped;
}

namespace {

// CODE.2 213AC: special-purpose descriptor subtypes 0,1,2 are
// permitted to contain the following incoming object categories.
bool source_container_type_compatible(
    std::uint8_t subtype,std::uint8_t item_category) noexcept {
    // Original 21404 / 11C3C general pouch, sack and chest
    // containers accept ordinary object categories subject to
    // separate attack-profile and nesting constraints. The first
    // two categories also reject hand-pair special items.
    if (subtype<=2U) return true;
    if (subtype==3U) return item_category==0x0DU;
    if (subtype==4U) return item_category==0x0EU;
    if (subtype==5U) return item_category==0x0CU;
    if (subtype==6U) return item_category==0x0FU;
    return false;
}

bool held_container_is_open(
    const InventoryRuntimeState& runtime,
    const PartyImage& party, const ObjectCatalog& objects) {
    if(!runtime.open_container) return false;
    const auto& opening=*runtime.open_container;
    if(opening.actor>=PartyImage::kActorCount ||
       opening.hand>=2U || !party.active(opening.actor) ||
       party_item(party,opening.actor,14U+opening.hand)!=
           opening.object_id) return false;
    if(opening.object_id<0) return false;
    const auto& entry=objects.at(
        static_cast<std::size_t>(opening.object_id));
    return entry.item_category==1U &&
        entry.container_state_index>=0 &&
        static_cast<std::size_t>(entry.container_state_index)==
            opening.descriptor_index;
}

} // namespace

std::uint32_t InventoryRuntimeState::container_weight_half_units(
    const ObjectCatalog& objects,std::size_t descriptor_index) const {
    if(descriptor_index>=containers.size())
        throw std::out_of_range("original container state index");

    std::array<bool,kRuntimeDescriptorCount> visited{};
    std::function<std::uint32_t(std::size_t)> accumulate =
        [&](std::size_t index)->std::uint32_t {
        if(index>=containers.size() || visited[index])
            throw std::runtime_error("original container cycle");
        visited[index]=true;
        std::uint32_t total=0U;
        for(const auto id:containers[index].items) {
            if(id==-1) continue;
            if(id<0) throw std::out_of_range("negative container object id");
            const auto& definition=objects.at(
                static_cast<std::size_t>(id));
            total+=definition.weight_half_units;
            if(definition.item_category==1U &&
                definition.container_state_index>=0) {
                const auto child=static_cast<std::size_t>(
                    definition.container_state_index);
                total+=accumulate(child);
            }
        }
        visited[index]=false;
        return total;
    };
    return accumulate(descriptor_index);
}

std::uint32_t InventoryRuntimeState::actor_weight_half_units(
    const PartyImage& party,const ObjectCatalog& objects,
    std::size_t actor) const {
    if(actor>=PartyImage::kActorCount)
        throw std::out_of_range("original actor weight index");
    if(!party.active(actor))return 0U;
    std::uint64_t total=0U;
    for(std::size_t slot=0U;slot<21U;++slot) {
        const auto id=party_item(party,actor,slot);
        if(id==-1)continue;
        if(id<0)throw std::out_of_range("negative character object");
        const auto& item=objects.at(static_cast<std::size_t>(id));
        total+=item.weight_half_units;
        if(item.item_category==1U &&
           item.container_state_index>=0) {
            total+=container_weight_half_units(
                objects,static_cast<std::size_t>(
                    item.container_state_index));
        }
    }
    if(total>std::numeric_limits<std::uint32_t>::max()) {
        throw std::overflow_error("original actor burden overflow");
    }
    return static_cast<std::uint32_t>(total);
}

InventoryContainerStatus InventoryRuntimeState::open_hand_container(
    const PartyImage& party,const ObjectCatalog& objects,
    std::size_t actor,std::size_t hand) {
    if(actor>=PartyImage::kActorCount || hand>=2U)
        throw std::out_of_range("original hand container actor/hand");
    open_container.reset();
    if(!party.active(actor)) return InventoryContainerStatus::Unsupported;
    const auto id=party_item(party,actor,14U+hand);
    if(id<0) return InventoryContainerStatus::NoChange;
    const auto& obj=objects.at(static_cast<std::size_t>(id));
    if(obj.item_category!=1U || obj.container_state_index<0)
        return InventoryContainerStatus::Unsupported;
    const auto index=static_cast<std::size_t>(obj.container_state_index);
    if(index>=containers.size() ||
       containers[index].slot_count==0U ||
       containers[index].items.size()!=containers[index].slot_count)
        return InventoryContainerStatus::Unsupported;
    open_container=OpenInventoryContainer{actor,hand,id,index};
    return InventoryContainerStatus::Opened;
}

InventoryContainerStatus InventoryRuntimeState::put_held_in_container_slot(
    const ObjectCatalog& objects,std::int16_t container_id,
    std::size_t slot) {
    if(container_id<0 || held.object_id<0) {
        return InventoryContainerStatus::NoChange;
    }
    const auto& owner=objects.at(
        static_cast<std::size_t>(container_id));
    if(owner.item_category!=1U || owner.container_state_index<0) {
        return InventoryContainerStatus::Unsupported;
    }
    const auto state_index=static_cast<std::size_t>(
        owner.container_state_index);
    if(state_index>=containers.size())
        return InventoryContainerStatus::Unsupported;
    auto& dest=containers[state_index];
    if(dest.slot_count==0U || dest.items.size()!=dest.slot_count) {
        return InventoryContainerStatus::Unsupported;
    }
    if(slot>=dest.items.size())
        throw std::out_of_range("original container target slot");

    const auto& next=objects.at(
        static_cast<std::size_t>(held.object_id));
    // Original CODE.2 11C3C (physical 4CC89..4CCC4) permits
    // category-1 containers INSIDE a container of a different
    // descriptor subtype. Equal subtypes are explicitly rejected.
    // For generic subtype 0/1 it rejects hand-pair class 2/3,
    // NOT hand-pair class 1 (4CC76..4CC83).
    if(!source_container_type_compatible(dest.subtype,next.item_category) ||
       (dest.subtype<=1U && next.raw_byte_35_low2>=2U)) {
        return InventoryContainerStatus::Incompatible;
    }
    const auto previous=dest.items[slot];
    if(previous < -1)
        return InventoryContainerStatus::Unsupported;

    auto object_burden=[&](std::int16_t id)->std::uint64_t {
        if(id<0)return 0U;
        const auto& definition=objects.at(static_cast<std::size_t>(id));
        std::uint64_t weight=definition.weight_half_units;
        if(definition.item_category==1U) {
            if(definition.container_state_index<0)
                throw std::runtime_error("negative nested container selector");
            weight+=container_weight_half_units(
                objects,static_cast<std::size_t>(
                    definition.container_state_index));
        }
        return weight;
    };

    if(next.item_category==1U) {
        const int nested=next.container_state_index;
        if(nested<0 || nested>=static_cast<int>(containers.size()))
            return InventoryContainerStatus::Unsupported;
        if(held.object_id==container_id ||
           containers[static_cast<std::size_t>(nested)].subtype==
               dest.subtype) {
            return InventoryContainerStatus::Incompatible;
        }
        // Prevent indirect cycles. Unlike the DOS original, never
        // trust potentially corrupted imported container contents.
        // Check the prospective child graph BEFORE modifying either
        // global container state or the held cursor item.
        std::array<bool,kRuntimeDescriptorCount> visited{};
        std::function<bool(std::int16_t)> would_cycle =
            [&](std::int16_t root)->bool {
            if(root==container_id)return true;
            if(root<0)return false;
            const auto& node=objects.at(static_cast<std::size_t>(root));
            if(node.item_category!=1U)return false;
            if(node.container_state_index<0 ||
               node.container_state_index>=
                   static_cast<int>(containers.size()))return true;
            const auto index=static_cast<std::size_t>(
                node.container_state_index);
            if(visited[index])return true;
            visited[index]=true;
            for(const auto child:containers[index].items) {
                if(child!=-1 && would_cycle(child))return true;
            }
            visited[index]=false;
            return false;
        };
        if(would_cycle(held.object_id))
            return InventoryContainerStatus::Incompatible;
    }
    // Original 11C3C 4CD57..4CE18 compares the current contents
    // weight minus the outgoing object's FULL recursive burden
    // plus incoming object's full recursive burden to capacity*2.
    const auto total=container_weight_half_units(objects,state_index);
    const auto removed=object_burden(previous);
    const auto added=object_burden(held.object_id);
    if(total<removed)
        return InventoryContainerStatus::Unsupported;
    const auto proposed=static_cast<std::uint64_t>(total)-removed+added;
    if(proposed>static_cast<std::uint64_t>(dest.capacity_pounds)*2U) {
        return InventoryContainerStatus::TooHeavy;
    }
    const auto mask=static_cast<std::uint8_t>(
        1U<<(slot&7U));
    const auto previous_known=static_cast<std::uint8_t>(
        (dest.per_slot_state_bits&mask)!=0U);
    dest.items[slot]=held.object_id;
    dest.per_slot_state_bits=static_cast<std::uint8_t>(
        (dest.per_slot_state_bits&~mask) |
        (held.known!=0U?mask:0U));
    held={previous,previous_known};
    return InventoryContainerStatus::Inserted;
}

InventoryContainerStatus InventoryRuntimeState::insert_held_into_container_object(
    const ObjectCatalog& objects,std::int16_t object_id) {
    if(held.object_id==-1)return InventoryContainerStatus::NoChange;
    if(object_id<0)return InventoryContainerStatus::Unsupported;
    const auto& owner=objects.at(
        static_cast<std::size_t>(object_id));
    if(owner.item_category!=1U || owner.container_state_index<0)
        return InventoryContainerStatus::Unsupported;
    const auto index=static_cast<std::size_t>(
        owner.container_state_index);
    if(index>=containers.size())
        return InventoryContainerStatus::Unsupported;
    auto& dest=containers[index];
    if(dest.slot_count==0U || dest.items.size()!=dest.slot_count)
        return InventoryContainerStatus::Unsupported;
    for(std::size_t slot=0;slot<dest.items.size();++slot) {
        if(dest.items[slot]==-1) {
            return put_held_in_container_slot(
                objects,object_id,slot);
        }
    }
    return InventoryContainerStatus::Full;
}

InventoryContainerStatus InventoryRuntimeState::insert_held_into_open_container(
    const PartyImage& party,const ObjectCatalog& objects) {
    if(!held_container_is_open(*this,party,objects))
        return InventoryContainerStatus::Unsupported;
    return insert_held_into_container_object(
        objects,open_container->object_id);
}

InventoryContainerStatus InventoryRuntimeState::take_open_container_slot(
    const PartyImage& party,const ObjectCatalog& objects,
    std::size_t slot) {
    if(!held_container_is_open(*this,party,objects))
        return InventoryContainerStatus::Unsupported;
    auto& dest=containers[open_container->descriptor_index];
    if(slot>=dest.items.size())
        throw std::out_of_range("original container slot index");
    if(held.object_id!=-1)
        return InventoryContainerStatus::Unsupported;
    const auto id=dest.items[slot];
    if(id==-1)return InventoryContainerStatus::NoChange;
    if(id<0)return InventoryContainerStatus::Unsupported;
    (void)objects.at(static_cast<std::size_t>(id));
    const auto mask=static_cast<std::uint8_t>(1U<<(slot&7U));
    const auto known=static_cast<std::uint8_t>(
        (dest.per_slot_state_bits&mask)!=0U);
    dest.items[slot]=-1;
    dest.per_slot_state_bits=static_cast<std::uint8_t>(
        dest.per_slot_state_bits&~mask);
    held={id,known};
    return InventoryContainerStatus::Taken;
}

InventoryContainerStatus InventoryRuntimeState::swap_open_container_slot(
    const PartyImage& party,const ObjectCatalog& objects,
    std::size_t slot) {
    if(!held_container_is_open(*this,party,objects))
        return InventoryContainerStatus::Unsupported;
    const auto& opened=*open_container;
    if(slot>=containers[opened.descriptor_index].items.size())
        throw std::out_of_range("original open container grid slot");
    if(held.object_id==-1)
        return take_open_container_slot(party,objects,slot);
    return put_held_in_container_slot(
        objects,opened.object_id,slot);
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
