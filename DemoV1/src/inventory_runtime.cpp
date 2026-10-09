#include "rloft/inventory_runtime.hpp"

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
    // A container must not be nested into itself (or recursively).
    // Nested category-1 item support requires further CODE.2 11C3C
    // rule proof. Do not corrupt global descriptor ownership.
    if(next.item_category==1U ||
       !source_container_type_compatible(
           dest.subtype,next.item_category) ||
       (dest.subtype<=1U && next.raw_byte_35_low2!=0U)) {
        return InventoryContainerStatus::Incompatible;
    }
    const auto previous=dest.items[slot];
    if(previous < -1)
        return InventoryContainerStatus::Unsupported;
    const auto total=container_weight_half_units(objects,state_index);
    const auto removed=previous>=0
        ? objects.at(static_cast<std::size_t>(previous)).weight_half_units
        : 0U;
    const auto proposed=static_cast<std::int64_t>(total)-
        static_cast<std::int64_t>(removed)+
        static_cast<std::int64_t>(next.weight_half_units);
    if(proposed>static_cast<std::int64_t>(dest.capacity_pounds)*2) {
        return InventoryContainerStatus::TooHeavy;
    }
    if(proposed<0) return InventoryContainerStatus::Unsupported;
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
