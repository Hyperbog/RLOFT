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

// Source OBJECTS.DAT 0x41-byte records used to authenticate
// active 6FA0 context ownership. Object 2 and object 258 share
// low byte 0x02, so this also tests the original 6FA3 truncation.
rloft::ObjectCatalog source_objects() {
    constexpr std::size_t count=260U;
    std::vector<std::uint8_t> bytes(4U+count*0x41U,0U);
    put32(bytes,0U,static_cast<std::uint32_t>(count));
    for(const auto id:{2U,258U}) {
        const auto at=4U+id*0x41U;
        bytes[at+0x2FU]=1U; // category: container
        bytes[at+0x31U]=0U; // descriptor state 0
    }
    return rloft::ObjectCatalog::parse(bytes);
}
void set_hand_in_save(std::vector<std::uint8_t>& save,
                      std::size_t actor,std::size_t hand,
                      std::int16_t object_id) {
    const auto at=0x77U+actor*0x248U+
                  0x40U+(14U+hand)*2U;
    put16(save,at,static_cast<std::uint16_t>(object_id));
}
void set_ui_in_save(std::vector<std::uint8_t>& save,
                    std::size_t actor,
                    const std::array<std::uint8_t,4>& bytes) {
    const auto at=save.size()-16U+4U*actor;
    for(std::size_t i=0U;i<4U;++i)save[at+i]=bytes[i];
}

rloft::InventoryRuntimeState original_template() {
    rloft::InventoryRuntimeState state{};
    for(std::size_t i=0;i<63U;++i) {
        state.containers[i].slot_count=kOriginalSlotCounts[i];
        // Source CODE.2 7330 families: 4/6/8 slots map 2/3/4
        // columns. The active-view decoder requires usable
        // original presentation geometry, not just item count.
        state.containers[i].ui_columns=
            kOriginalSlotCounts[i]==4U ? 2U :
            kOriginalSlotCounts[i]==6U ? 3U :
            kOriginalSlotCounts[i]==8U ? 4U : 0U;
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
    // Export must use the CURRENT live 6FA0 array, not a stale
    // actor context copied into the preserved snapshot. This is
    // intentionally different from LOAD, which consumes the
    // serialized context stored in the imported snapshot.
    assert(C::encode(C::capture_matching_session(
        active_ui,session,inventory))==emitted);
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

    // Direct reconstructed 10FAC and 110B4 phase machine.
    // No invented host-time delays: each explicit advance models
    // precisely one original 10BBC/1A724 presentation phase.
    rloft::PartyContainerUiContext context{};
    assert(context.inactive());
    assert(!context.active());
    context.begin_open_10fac(4U,7U,258);
    assert(context.raw==
        (std::array<std::uint8_t,4>{{4U,7U,0U,2U}}));
    for(std::uint8_t phase=1U;phase<=3U;++phase) {
        assert(context.advance_open_phase_10fac());
        assert(context.raw[2]==phase);
        assert(context.active());
    }
    assert(context.fully_open());
    assert(!context.advance_open_phase_10fac());
    assert(context.raw[2]==3U);
    for(int phase=2;phase>=0;--phase) {
        assert(context.advance_close_phase_110b4());
        assert(context.raw[0]==4U);
        assert(context.raw[1]==7U);
        assert(context.raw[2]==static_cast<std::uint8_t>(phase));
        assert(context.raw[3]==2U);
    }
    assert(context.advance_close_phase_110b4());
    assert(context.raw==
        (std::array<std::uint8_t,4>{{
            0xFFU,0xFFU,0U,0xFFU}}));
    assert(context.inactive());
    assert(!context.advance_close_phase_110b4());
    context.mark_open_21693(2U,1U,329);
    assert(context.raw==
        (std::array<std::uint8_t,4>{{2U,1U,3U,73U}}));
    assert(context.fully_open());

    // Source 110B4 CLOSE DOES NOT write FF FF FF FF.
    // It steps 3,2,1,0, then only bytes +0/+1/+3
    // are reset to FF. The native bridge must restore this
    // actually reachable inactive 6FA0 record without requiring
    // any object-catalogue/hand association.
    auto closed_original=source_fixture(0U);
    set_ui_in_save(closed_original,0U,{{0xFFU,0xFFU,0U,0xFFU}});
    const auto closed=C::decode(closed_original,baseline);
    assert(closed.inventory.party_container_ui_contexts[0].inactive());
    assert(!closed.inventory.party_container_ui_contexts[0].active());
    assert(C::encode(closed)==closed_original);
    auto closed_inv=baseline;
    C::apply_to_matching_session(closed,session,closed_inv);
    assert(!closed_inv.open_container);
    assert(closed_inv.party_container_ui_contexts[0].raw==
        (std::array<std::uint8_t,4>{{0xFFU,0xFFU,0U,0xFFU}}));
    assert(C::encode(C::capture_matching_session(
        closed,session,closed_inv))==closed_original);

    // DOS 21693 creates source 6FA0 {subtype,index,phase3,
    // LOW8(object ID)}. An active UI is a true saved state:
    // accept only a single unambiguous hand owning a
    // category-1 item whose descriptor/subtype match.
    const auto objs=source_objects();
    auto open_original=source_fixture(0U);
    // Status word: actor 0 active (no 0x8000 empty sentinel).
    open_original[0x77U+0x10U]=0U;
    open_original[0x77U+0x11U]=0U;
    set_hand_in_save(open_original,0U,0U,2);
    set_hand_in_save(open_original,0U,1U,-1);
    set_ui_in_save(open_original,0U,{{0U,0U,3U,2U}});
    auto opened=C::decode(open_original,baseline);
    assert(C::encode(opened)==open_original);
    rloft::ExplorationSessionState clean{};
    clean.level_id=22U;
    clean.coordinate_a=0x48000U;
    clean.coordinate_b=0x48000U;
    clean.facing=270U;
    clean.time_of_day_16_16_hours=0xE0000U;
    auto live=baseline;
    assert(fails([&]{
        C::apply_to_matching_session(opened,clean,live);
    })); // Cannot guess hand without OBJECTS metadata.
    C::apply_to_matching_session(opened,clean,live,&objs);
    assert(live.open_container.has_value());
    assert(live.open_container->actor==0U);
    assert(live.open_container->hand==0U);
    assert(live.open_container->object_id==2);
    assert(live.open_container->descriptor_index==0U);
    assert(live.party_container_ui_contexts[0].raw==
        (std::array<std::uint8_t,4>{{0U,0U,3U,2U}}));
    assert(C::encode(C::capture_matching_session(
        opened,clean,live,&objs))==open_original);
    assert(fails([&]{
        (void)C::capture_matching_session(opened,clean,live);
    })); // An active hand needs source OBJECTS validation.

    // Closing a native fully-open view generates 110B4's
    // exact inactive bytes and leaves saved container contents.
    live.close_container();
    assert(!live.open_container.has_value());
    assert(live.party_container_ui_contexts[0].raw==
        (std::array<std::uint8_t,4>{{0xFFU,0xFFU,0U,0xFFU}}));
    auto closed_after_open=C::encode(
        C::capture_matching_session(opened,clean,live));
    assert(closed_after_open.size()==open_original.size());
    assert(closed_after_open[closed_after_open.size()-16U]==0xFFU);
    assert(closed_after_open[closed_after_open.size()-14U]==0U);
    assert(C::encode(C::decode(closed_after_open,baseline))==
        closed_after_open);

    // Original 6FA3 is only one byte, even though party item
    // IDs occupy a 16-bit signed slot. With OBJECTS available
    // an ID 258 may legally own context object byte 2.
    auto high_id=open_original;
    set_hand_in_save(high_id,0U,0U,258);
    const auto high_snapshot=C::decode(high_id,baseline);
    live=baseline;
    C::apply_to_matching_session(high_snapshot,clean,live,&objs);
    assert(live.open_container->object_id==258);
    assert(C::encode(C::capture_matching_session(
        high_snapshot,clean,live,&objs))==high_id);

    // No arbitrary hand choice when both candidates share the
    // same low byte + category + descriptor. Reject atomically.
    auto both_hands=high_id;
    set_hand_in_save(both_hands,0U,1U,2);
    auto ambiguous=C::decode(both_hands,baseline);
    live.close_container();
    const auto live_before=clean.party.bytes;
    const auto held_pre=live.held.object_id;
    assert(fails([&]{
        C::apply_to_matching_session(ambiguous,clean,live,&objs);
    }));
    assert(clean.party.bytes==live_before);
    assert(live.held.object_id==held_pre);

    // An interrupted original opening or closing animation is
    // source-preserved by raw codec, but not falsely presented
    // as an already-open graphical native view.
    for(const auto phase:{0U,1U,2U}) {
        auto partial=open_original;
        set_ui_in_save(partial,0U,
            {{0U,0U,static_cast<std::uint8_t>(phase),2U}});
        const auto original_partial=C::decode(partial,baseline);
        assert(C::encode(original_partial)==partial);
        live=baseline;
        assert(fails([&]{
            C::apply_to_matching_session(
                original_partial,clean,live,&objs);
        }));
    }

    // The one-view native modal cannot currently display two
    // independently open actor contexts. Preserve their bytes,
    // but reject live application until multi-lane renderer exists.
    auto two_actor_views=open_original;
    two_actor_views[0x77U+0x248U+0x10U]=0U;
    two_actor_views[0x77U+0x248U+0x11U]=0U;
    set_hand_in_save(two_actor_views,1U,0U,2);
    set_ui_in_save(two_actor_views,1U,{{0U,0U,3U,2U}});
    const auto many=C::decode(two_actor_views,baseline);
    assert(C::encode(many)==two_actor_views);
    live=baseline;
    assert(fails([&]{
        C::apply_to_matching_session(many,clean,live,&objs);
    }));

    // Reject mismatch between persisted subtype/index and the
    // original CODE.2 descriptor without silently repairing SAVE.
    auto bad_subtype=open_original;
    set_ui_in_save(bad_subtype,0U,{{1U,0U,3U,2U}});
    assert(fails([&]{
        live=baseline;
        C::apply_to_matching_session(
            C::decode(bad_subtype,baseline),clean,live,&objs);
    }));
    auto bad_index=open_original;
    set_ui_in_save(bad_index,0U,{{0U,62U,3U,2U}});
    assert(fails([&]{
        live=baseline;
        C::apply_to_matching_session(
            C::decode(bad_index,baseline),clean,live,&objs);
    }));
    auto no_hand=open_original;
    set_hand_in_save(no_hand,0U,0U,-1);
    assert(fails([&]{
        live=baseline;
        C::apply_to_matching_session(
            C::decode(no_hand,baseline),clean,live,&objs);
    }));
}
