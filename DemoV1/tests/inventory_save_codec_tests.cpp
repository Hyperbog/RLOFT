#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/inventory_save_codec.hpp"

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace {

void put16(std::vector<std::uint8_t>& dst,
           std::size_t at,std::uint16_t value) {
    dst[at]=static_cast<std::uint8_t>(value);
    dst[at+1U]=static_cast<std::uint8_t>(value>>8U);
}
void put32(std::vector<std::uint8_t>& dst,
           std::size_t at,std::uint32_t value) {
    for(unsigned i=0;i<4U;++i)
        dst[at+i]=static_cast<std::uint8_t>(value>>(i*8U));
}
std::uint16_t get16(const std::vector<std::uint8_t>& b,
                    std::size_t at) {
    return static_cast<std::uint16_t>(b[at]) |
        (static_cast<std::uint16_t>(b[at+1U])<<8U);
}

// Descriptor slot-count bytes copied from original CODE.2 object-2
// DGROUP:7330, at physical 0x7D254 + 10*descriptor.
// State 62 is zero-initialized, NOT persisted by original SAVE.
constexpr std::array<std::uint8_t,63> kOriginalSlotCounts{{
    4,8,8,8,6,6,8,6,8,8,4,8,8,6,8,4,8,4,8,8,8,
    8,8,8,6,8,8,4,6,8,8,8,8,8,8,8,8,8,4,8,8,8,
    4,8,8,6,6,8,8,8,4,8,8,8,6,8,4,6,8,8,6,8,0
}};

rloft::InventoryRuntimeState original_template() {
    rloft::InventoryRuntimeState state{};
    for(std::size_t i=0;i<63U;++i) {
        state.containers[i].slot_count=kOriginalSlotCounts[i];
        state.containers[i].items.assign(kOriginalSlotCounts[i],-1);
        state.containers[i].subtype=static_cast<std::uint8_t>(i%7U);
    }
    // This test sentinel must survive import but never be written
    // into the 62-record original on-disk format.
    state.containers[62].per_slot_state_bits=0xD3U;
    return state;
}

std::vector<std::uint8_t> source_fixture(std::uint32_t effects) {
    using C=rloft::InventorySaveCodec;
    std::vector<std::uint8_t> b(
        C::kZeroTransientSaveBytes+
            static_cast<std::size_t>(effects)*C::kTransientRecordBytes);
    for(std::size_t i=0;i<b.size();++i)
        b[i]=static_cast<std::uint8_t>((i*37U+19U)&0xFFU);
    put32(b,C::kTransientCountAt,effects);
    // The live-only bridge deliberately requires world consistency.
    // Match CODE.2 fresh Level22 (4.5,4.5), facing 270, time 14:00.
    put32(b,0x52U,22U);
    put32(b,0x56U,0x00048000U);
    put32(b,0x5AU,0x00048000U);
    put32(b,0x5EU,270U);
    put32(b,0x70U,0x000E0000U);
    // The source's four inactive 6FA0 modal contexts.
    for(std::size_t i=b.size()-16U;i<b.size();++i)b[i]=0xFFU;
    put16(b,0x74U,0xFFFFU);
    b[0x76U]=0U;
    const auto bank=0x9A7U+
        static_cast<std::size_t>(effects)*0x16U;
    std::size_t at=bank;
    for(std::size_t i=0;i<62U;++i) {
        for(std::size_t s=0;s<kOriginalSlotCounts[i];++s) {
            put16(b,at,s==0U
                ? static_cast<std::uint16_t>(i==61U?57U:i)
                : 0xFFFFU);
            at+=2U;
        }
        b[at++]=static_cast<std::uint8_t>(i&0xFFU);
    }
    assert(at==bank+0x3AAU);
    return b;
}

template<class Action>
bool fails(Action&& action) {
    try { action(); }
    catch(const std::runtime_error&) { return true; }
    return false;
}

void round_trip(std::uint32_t effects) {
    using C=rloft::InventorySaveCodec;
    const auto original=source_fixture(effects);
    const auto template_state=original_template();
    auto saved=C::decode(original,template_state);

    assert(saved.preserved_save_bytes==original);
    assert(saved.transient_effect_count==effects);
    assert(saved.container_bank_offset==
        0x9A7U+static_cast<std::size_t>(effects)*0x16U);
    assert(saved.inventory.held.object_id==-1);
    assert(saved.inventory.held.known==0U);
    assert(saved.inventory.containers[0].items[0]==0);
    assert(saved.inventory.containers[61].items[0]==57);
    assert(saved.inventory.containers[61].per_slot_state_bits==61U);
    assert(saved.inventory.containers[62].per_slot_state_bits==0xD3U);
    assert(saved.inventory.containers[62].slot_count==0U);
    assert(!saved.inventory.open_container.has_value());
    assert(!saved.inventory.mage_scribe_feedback.has_value());

    // Unchanged decode -> encode must reproduce EVERY source
    // byte, including packed effects, reserved actor regions,
    // charged-item counters, all 10 roster records, phase bytes.
    assert(C::encode(saved)==original);

    // Change known flags, Mage spellbook low nibble, held cursor,
    // four original fragment story bits, a nested container, and
    // the late quiver-shaped source descriptor 61.
    saved.party.bytes[0U*0x248U+0x6AU]=0U;
    saved.party.bytes[2U*0x248U+0x12BU]=0x05U;
    saved.persistent_event_flags[0U]=0x0FU;
    saved.persistent_event_flags[0xBAU/8U] |= 0x04U;
    saved.inventory.held={201,1U};
    saved.inventory.containers[0].items[1]=0x70;
    saved.inventory.containers[0].per_slot_state_bits=0x02U;
    saved.inventory.containers[61].items[0]=-1;
    saved.inventory.containers[61].per_slot_state_bits=0x80U;
    saved.inventory.containers[62].per_slot_state_bits=0x22U;
    saved.container_ui_contexts[1]={{1U,2U,3U,4U}};

    const auto encoded=C::encode(saved);
    assert(encoded.size()==original.size());
    assert(encoded[0x32U]==0x0FU);
    assert(encoded[0x32U+0xBAU/8U]==
        static_cast<std::uint8_t>(
            original[0x32U+0xBAU/8U]|0x04U));
    assert(encoded[0x77U+0x6AU]==0U);
    assert(encoded[0x77U+2U*0x248U+0x12BU]==0x05U);
    assert(get16(encoded,0x74U)==201U);
    assert(encoded[0x76U]==1U);
    const auto bank=saved.container_bank_offset;
    assert(get16(encoded,bank+2U)==0x70U);
    assert(encoded[bank+8U]==0x02U);
    const auto quiver=bank+0x3AAU-17U;
    assert(get16(encoded,quiver)==0xFFFFU);
    assert(encoded[quiver+16U]==0x80U);
    for(std::size_t i=0;i<4U;++i)
        assert(encoded[encoded.size()-16U+4U+i]==i+1U);

    // Source bytes not authored by the inventory codec must
    // not change, even at nonzero counted transient effects.
    for(std::size_t i=0;i<original.size();++i) {
        const bool owned=
            (i>=0x32U && i<0x52U) ||
            (i>=0x74U && i<=0x76U) ||
            (i>=0x77U && i<0x77U+C::kPartyBytes) ||
            (i>=bank && i<bank+C::kPersistentContainerBytes) ||
            (i>=original.size()-16U);
        if(!owned)assert(encoded[i]==original[i]);
    }

    // Decode the modified original bytes a second time; metadata
    // remains CODE.2-sourced and authoring changes survive.
    const auto restored=C::decode(encoded,template_state);
    assert(restored.party.bytes==saved.party.bytes);
    assert(restored.persistent_event_flags==
           saved.persistent_event_flags);
    assert(restored.inventory.held.object_id==201);
    assert(restored.inventory.held.known==1U);
    assert(restored.inventory.containers[0].items[1]==0x70);
    assert(restored.inventory.containers[61].items[0]==-1);
    assert(restored.inventory.containers[62].per_slot_state_bits==
           0xD3U); // unpersisted descriptor 62 stays on CODE.2
    assert(restored.container_ui_contexts==
           saved.container_ui_contexts);
    assert(C::encode(restored)==encoded);
}

} // namespace

int main() {
    // Original quick-start SAVE: 0x2448, no transient records.
    round_trip(0U);
    // Count moves ALL middle/trailing sections by 0x16*N.
    round_trip(3U);
    round_trip(rloft::InventorySaveCodec::kMaxTransientCount);

    using C=rloft::InventorySaveCodec;
    const auto baseline=original_template();
    auto valid=source_fixture(0U);

    auto too_short=valid;
    too_short.pop_back();
    assert(fails([&]{
        (void)C::decode(too_short,baseline);
    }));
    auto invalid_count=valid;
    put32(invalid_count,C::kTransientCountAt,0x97U);
    assert(fails([&]{
        (void)C::decode(invalid_count,baseline);
    }));
    auto malformed=baseline;
    malformed.containers[61].items.pop_back();
    assert(fails([&]{
        (void)C::decode(valid,malformed);
    }));
    auto nonoriginal=baseline;
    nonoriginal.containers[1].slot_count=9U;
    nonoriginal.containers[1].items.push_back(-1);
    assert(fails([&]{
        (void)C::decode(valid,nonoriginal);
    }));
    auto loaded=C::decode(valid,baseline);
    loaded.transient_effect_count=1U;
    assert(fails([&]{
        (void)C::encode(loaded);
    }));
    loaded.transient_effect_count=0U;
    loaded.inventory.containers[0].items.clear();
    assert(fails([&]{
        (void)C::encode(loaded);
    }));

    // Native session bridge: only previously verified inventory
    // components may be applied over a CURRENT, matching world.
    // Both copies of character state (SAVE fixture and live
    // runtime session) must remain internally consistent.
    auto original_save=source_fixture(0U);
    const auto snapshot=C::decode(original_save,baseline);
    rloft::ExplorationSessionState session{};
    session.level_id=22U;
    session.coordinate_a=0x00048000U;
    session.coordinate_b=0x00048000U;
    session.facing=270U;
    session.time_of_day_16_16_hours=0x000E0000U;
    session.party.bytes.fill(0xAAU);
    session.persistent_event_flags.fill(0x55U);
    auto inventory=baseline;
    inventory.held={150,1U};
    C::apply_to_matching_session(snapshot,session,inventory);
    assert(session.party.bytes==snapshot.party.bytes);
    assert(session.persistent_event_flags==
        snapshot.persistent_event_flags);
    assert(inventory.held.object_id==-1);
    assert(inventory.held.known==0U);
    assert(inventory.containers[61].items[0]==57);
    assert(inventory.containers[62].per_slot_state_bits==0xD3U);

    // After live inventory actions, projection must collect
    // CURRENT party/flags/containers/held, not stale imported
    // snapshot fields. The original independent world bytes are
    // preserved exactly from the source SAVE.
    session.party.bytes[0x6AU]=0U; // learned Mage spell
    session.party.bytes[0x12BU]=0x04U; // item known bits
    session.persistent_event_flags[0U]=0x0FU;
    inventory.held={0x61,1U};
    inventory.containers[0].items[1]=0x62;
    inventory.containers[0].per_slot_state_bits=0x02U;
    const auto from_live=C::capture_matching_session(
        snapshot,session,inventory);
    const auto emitted=C::encode(from_live);
    assert(emitted[0x77U+0x6AU]==0U);
    assert(emitted[0x77U+0x12BU]==0x04U);
    assert(emitted[0x32U]==0x0FU);
    assert(get16(emitted,0x74U)==0x61U);
    assert(emitted[0x76U]==1U);
    assert(get16(emitted,0x9A7U+2U)==0x62U);
    assert(C::decode(emitted,baseline).party.bytes==
        session.party.bytes);
    for(std::size_t i=0;i<emitted.size();++i) {
        const bool inventory_owned=
            (i>=0x32U && i<0x52U) ||
            (i>=0x74U && i<=0x76U) ||
            (i>=0x77U && i<0x77U+C::kPartyBytes) ||
            (i>=0x9A7U && i<0x9A7U+C::kPersistentContainerBytes) ||
            (i>=emitted.size()-16U);
        if(!inventory_owned)assert(emitted[i]==original_save[i]);
    }

    // Fail closed: an import for a DIFFERENT level/pose/time
    // never changes either live character or held cursor state.
    const auto party_before=session.party.bytes;
    const auto event_before=session.persistent_event_flags;
    const auto held_before=inventory.held;
    const auto containers_before=inventory.containers[0].items;
    auto wrong_pose=session;
    wrong_pose.coordinate_a+=0x10000U;
    assert(fails([&]{
        C::apply_to_matching_session(snapshot,wrong_pose,inventory);
    }));
    assert(wrong_pose.party.bytes==party_before);
    assert(wrong_pose.persistent_event_flags==event_before);
    assert(inventory.held.object_id==held_before.object_id);
    assert(inventory.containers[0].items==containers_before);
    assert(fails([&]{
        (void)C::capture_matching_session(snapshot,wrong_pose,inventory);
    }));
    for(int changed=0;changed<3;++changed) {
        auto wrong=session;
        if(changed==0)wrong.level_id+=1U;
        if(changed==1)wrong.facing+=90U;
        if(changed==2)wrong.time_of_day_16_16_hours+=0x10000U;
        assert(fails([&]{
            C::apply_to_matching_session(snapshot,wrong,inventory);
        }));
        assert(wrong.party.bytes==party_before);
        assert(inventory.held.object_id==held_before.object_id);
        assert(fails([&]{
            (void)C::capture_matching_session(snapshot,wrong,inventory);
        }));
    }

    auto moving=C::decode(source_fixture(3U),baseline);
    assert(fails([&]{
        C::apply_to_matching_session(moving,session,inventory);
    }));
    assert(session.party.bytes==party_before);
    auto active_ui=snapshot;
    active_ui.container_ui_contexts[0][0]=2U;
    assert(fails([&]{
        C::apply_to_matching_session(active_ui,session,inventory);
    }));
    assert(fails([&]{
        (void)C::capture_matching_session(active_ui,session,inventory);
    }));
    inventory.open_container=rloft::OpenInventoryContainer{};
    assert(fails([&]{
        C::apply_to_matching_session(snapshot,session,inventory);
    }));
    assert(session.party.bytes==party_before);
    assert(inventory.held.object_id==held_before.object_id);
    assert(fails([&]{
        (void)C::capture_matching_session(snapshot,session,inventory);
    }));
    inventory.open_container.reset();
    // Standard zero-effect, unchanged-world roundtrip remains
    // allowed after closing the transient hand-container UI.
    assert(C::encode(C::capture_matching_session(
        snapshot,session,inventory))==emitted);
}
