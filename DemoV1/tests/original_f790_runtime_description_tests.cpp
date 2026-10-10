#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/original_f790_runtime_event.hpp"
#include "rloft/original_f790_ordinary_slot_event_plan.hpp"
#include "rloft/original_f790_modal_entry.hpp"
#include "rloft/original_f790_ordinary_slot_dispatch.hpp"

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
void put32(std::vector<std::uint8_t>& v,std::size_t at,std::uint32_t n) {
    for(int i=0;i<4;++i)
        v[at+static_cast<std::size_t>(i)]=
          static_cast<std::uint8_t>(n>>(8*i));
}
}

int main() {
    using Static=rloft::OriginalF790StaticResources;
    using Gateway=rloft::OriginalF790RuntimeDescription;
    std::vector<std::uint8_t> c(Static::kCode2FileBytes,0U);
    c[0x27BDC]='L';c[0x27BDD]='E';
    put32(c,0x27BDC+0xC4+0x0C,1U);
    put32(c,0x27BDC+0xC4+0x10,59U);
    put32(c,0x27BDC+0xDC+0x0C,60U);
    put32(c,0x27BDC+0xDC+0x10,19U);
    const auto ds=[&](std::size_t off,const std::string& str) {
        const auto p=Static::kObject2DiskBase+off;
        for(std::size_t i=0;i<str.size();++i)
            c[p+i]=static_cast<std::uint8_t>(str[i]);
    };
    ds(0x598,"; CHARGES: ");ds(0x610," +");ds(0x614,"; ");
    ds(0x618," ");ds(0x61C,"-");ds(0x620,"/");ds(0x624,".5");
    put32(c,Static::kObject2DiskBase+0x6D98,0x598U);
    c[Static::kObject2DiskBase+0x7330]=4U;
    c[Static::kObject2DiskBase+0x7332]=8U;
    c[Static::kObject2DiskBase+0x7330+20U*10U+2U]=0x7fU;
    c[Static::kObject2DiskBase+0x67EC]=8U;
    const Static original_code2(c);

    std::vector<std::uint8_t> ts(4U+0x43U*4U,0U);
    put32(ts,0U,0x43U);
    const auto empty=ts.size();
    ts.push_back(0U);ts.push_back(0U);
    for(std::size_t i=0;i<0x43U;++i)
        put32(ts,4U+4U*i,static_cast<std::uint32_t>(empty));
    const auto t=[&](std::size_t index,const std::string& label) {
        put32(ts,4U+4U*index,static_cast<std::uint32_t>(ts.size()));
        ts.push_back(static_cast<std::uint8_t>(label.size()));
        ts.push_back(0U);
        for(char ch:label)ts.push_back(static_cast<std::uint8_t>(ch)^0xDCU);
    };
    t(0x3dU,"WEIGHT:");
    t(0x3eU,"DAMAGE:");
    t(0x40U,"A.C. BONUS:");
    t(0x41U,"CAPACITY:");
    t(0x42U,"FIREBALL"); // synthetic original-index 11630 source name
    const rloft::OriginalTTextArchive original_t(ts);

    constexpr std::size_t count=90U;
    std::vector<std::uint8_t> original_objects(4U+count*0x41U,0U);
    put32(original_objects,0U,static_cast<std::uint32_t>(count));
    const auto object=[&](std::size_t id,const char* name,
                          std::uint8_t category,std::uint8_t weight,
                          std::uint8_t subtype) {
        const auto p=4U+id*0x41U;
        std::size_t j=0;
        while(name[j]!=0) {
            original_objects[p+j]=static_cast<std::uint8_t>(name[j]);
            ++j;
        }
        original_objects[p+0x2FU]=category;
        original_objects[p+0x30U]=weight;
        original_objects[p+0x31U]=subtype;
    };
    object(0,"POUCH",1U,2U,0U);
    object(1,"GEM",0U,5U,0U);
    object(2,"ARMOR",2U,3U,0U);
    object(3,"SWORD",9U,4U,0U);
    object(4,"ROCK",0U,9U,0U);
    object(74,"SPECIAL",0U,1U,0U);
    object(80,"BAG",1U,4U,20U);
    original_objects[4U+2U*0x41U+0x34U]=0xfeU;
    original_objects[4U+2U*0x41U+0x37U]=3U;
    const auto weapon=4U+3U*0x41U;
    original_objects[weapon+0x38U]=1U;
    original_objects[weapon+0x39U]=6U;
    original_objects[weapon+0x3AU]=1U;
    original_objects[weapon+0x3BU]=1U;
    original_objects[weapon+0x3CU]=6U;
    original_objects[weapon+0x3DU]=0U;
    const auto parsed=rloft::ObjectCatalog::parse(original_objects);
    rloft::InventoryRuntimeState inventory{};
    inventory.containers[0].slot_count=2U;
    inventory.containers[0].items={1,-1};
    inventory.containers[20].slot_count=8U;
    inventory.containers[20].items.assign(8U,4);

    const auto describe=[&](int id,bool known=false) {
        return Gateway::compose(original_objects,parsed,inventory,id,known,
                                original_code2,original_t);
    };
    assert(describe(0)=="POUCH; WEIGHT:3.5; CAPACITY: 8");
    inventory.containers[0].items[0]=4;
    assert(describe(0)=="POUCH; WEIGHT:5.5; CAPACITY: 8");
    inventory.containers[0].items[0]=-1;
    assert(describe(0)=="POUCH; WEIGHT:1; CAPACITY: 8");

    // Even with heavy mutable contents Bag of Holding uses the exact
    // original F5F8 subtype20 exception (object80 weight field).
    assert(describe(80)=="BAG; WEIGHT:4");
    inventory.containers[20].items.assign(8U,-1);
    assert(describe(80)=="BAG; WEIGHT:4");
    assert(describe(2,true)=="ARMOR +3; A.C. BONUS: 2; ");
    assert(describe(3)=="SWORD; DAMAGE: 2-7/1-6");
    assert(describe(74,true)=="SPECIAL; WEIGHT:0.5; CHARGES: 8");
    assert(describe(-1).empty());

    // F7B4 priorizes pending 759C even when the held-object argument
    // is invalid. The original two local and one selected-ID F790
    // callers must not be rewritten using inventory.held.
    const auto source_message=[&](
        rloft::OriginalF790SourceArguments args,
        const std::vector<std::uint8_t>& raw) {
        return rloft::OriginalF790RuntimeEvent::resolve_text_for_source_call(
            args,raw,parsed,inventory,original_code2,original_t);
    };
    inventory.held={1,1U}; // intentionally different source caller

    // All 17 original E8 callers are represented and use the right
    // argument source, rather than copying the GUI cursor blindly.
    using Origin=rloft::OriginalF790ArgumentOrigin;
    const auto& original_calls=rloft::OriginalF790CallerCatalog::kDirectCalls;
    assert(original_calls.size()==17U);
    std::array<int,4> ownership_counts{};
    for(std::size_t i=0;i<original_calls.size();++i) {
        const auto& call=original_calls[i];
        assert(i==0U || original_calls[i-1U].e8_offset<call.e8_offset);
        const auto maybe=rloft::OriginalF790CallerCatalog::arguments_at(
            call.e8_offset);
        assert(maybe && *maybe==call.argument_origin);
        const auto args=rloft::OriginalF790RuntimeEvent::arguments_for_direct_call(
            call.e8_offset,inventory,74,true,false,{});
        switch(call.argument_origin) {
        case Origin::GlobalHeld8600_8602:
            ++ownership_counts[0];
            assert(args.item_id==1 && args.known);
            break;
        case Origin::ExplicitEmpty:
            ++ownership_counts[1];
            assert(args.item_id==-1 && !args.known);
            break;
        case Origin::StackLocal:
            ++ownership_counts[2];
            assert(args.item_id==74 && args.known);
            break;
        case Origin::SelectedKnownZero:
            ++ownership_counts[3];
            assert(args.item_id==74 && !args.known);
            break;
        }
    }
    assert((ownership_counts==std::array<int,4>{{11,3,2,1}}));
    assert(!rloft::OriginalF790CallerCatalog::arguments_at(0x1134dU));
    bool unknown_site=false;
    try {
        (void)rloft::OriginalF790RuntimeEvent::arguments_for_direct_call(
            0x12345U,inventory,74,true,false,{});
    } catch(const std::out_of_range&) {unknown_site=true;}
    assert(unknown_site);
    const auto pending_args=
        rloft::OriginalF790RuntimeEvent::arguments_for_direct_call(
            0x1184aU,inventory,74,true,true,"PENDING SOURCE 759C");
    assert(pending_args.item_id==-1 && !pending_args.known);
    assert(pending_args.pending_6dac);
    assert(source_message(pending_args,original_objects)==
           "PENDING SOURCE 759C");

    assert(source_message({2,true,false,{}},original_objects)==
           "ARMOR +3; A.C. BONUS: 2; ");
    assert(source_message({3,false,false,{}},original_objects)==
           "SWORD; DAMAGE: 2-7/1-6");
    assert(source_message({-1,false,false,{}},original_objects).empty());
    const std::vector<std::uint8_t> intentionally_invalid{};
    assert(source_message({900,true,true,"EXACT 759C"},
                          intentionally_invalid)=="EXACT 759C");

    // Complete source-only event-to-VGA bridge, with the ACTUAL native
    // framebuffer, prior modal capture, source-specific arguments,
    // mutable item text and ONE original gameplay RNG.
    rloft::IndexedFramebuffer framebuffer{};
    framebuffer.clear(0x31U);
    framebuffer.fill_rect(0x62,0,16,320,0x40U);
    rloft::OriginalF790ModalSequence sequence;
    sequence.enter_modal_13118();
    sequence.capture_after_actor_columns_112ec(framebuffer);
    rloft::OriginalRandom shared_rng(0x10203040U);
    int tick_observations=0;
    auto source_tick=[&]()->std::uint32_t {
        ++tick_observations;
        return 0x76543210U;
    };
    rloft::Type1Raster ribbon{};
    ribbon.count=6;ribbon.width=8;ribbon.height=3;
    ribbon.transparent_index=0xffU;
    ribbon.pixels.resize(6U*8U*3U);
    for(std::size_t i=0;i<ribbon.pixels.size();++i)
        ribbon.pixels[i]=static_cast<std::uint8_t>(10U+i/24U);
    rloft::Type2Font font{};
    font.widths.fill(1U);
    font.raster.count=128;
    font.raster.width=1;
    font.raster.height=1;
    font.raster.transparent_index=0U;
    font.raster.pixels.assign(128U,200U);
    const auto fire=[&](rloft::OriginalF790SourceArguments args) {
        rloft::OriginalF790RuntimeEvent::dispatch_source_call(
            sequence,framebuffer,args,original_objects,parsed,inventory,
            original_code2,original_t,ribbon,font,shared_rng,source_tick);
    };
    fire({0,false,false,{}});
    assert(tick_observations==1);
    assert(shared_rng.state()==0x76543210U);
    assert(sequence.f790_event_count()==1U);
    assert(framebuffer.pixels().at(0x62U*320U+0x44U)==10U);

    // Prebuilt 759C feedback takes priority over even an invalid
    // local item ID; pending feedback does not reseed from 30700.
    fire({900,true,true,"EXACT 759C"});
    assert(tick_observations==1);
    assert(sequence.f790_event_count()==2U);
    const auto after_pending=shared_rng.state();
    for(int redraw=0;redraw<100;++redraw) {
        framebuffer.clear(0x31U);
        framebuffer.fill_rect(0x62,0,16,320,0x40U);
        sequence.replay_after_host_redraw(framebuffer,ribbon,font);
    }
    assert(sequence.f790_event_count()==2U);
    assert(shared_rng.state()==after_pending);
    assert(tick_observations==1);

    fire({-1,false,false,{}});
    assert(sequence.f790_event_count()==3U);
    assert(shared_rng.state()==after_pending);
    assert(framebuffer.pixels().at(0x62U*320U+0x44U)==0x40U);
    sequence.leave_modal();

    // Full synthetic 117A0 transaction: 11630 draws 759C at 1173B
    // with pre-consumption held scroll, THEN 1183E clears the cursor
    // and 1184A draws the SAME pending ribbon a second time.
    sequence.enter_modal_13118();
    framebuffer.clear(0x31U);
    framebuffer.fill_rect(0x62,0,16,320,0x40U);
    sequence.capture_after_actor_columns_112ec(framebuffer);
    const rloft::HeldItemState scroll_before{0xA7,1U};
    rloft::MageScribeFeedback successful_feedback{};
    successful_feedback.kind=rloft::MageScribeFeedbackKind::Scribed;
    successful_feedback.t_spell_name_index=0x42U;
    successful_feedback.source_text=" LEARNED!";
    successful_feedback.pending_6dac=1U;
    successful_feedback.deadline_6da8=0x1234U;
    inventory.mage_scribe_feedback=successful_feedback;
    inventory.held={-1,0U};
    const auto plan=rloft::OriginalF790OrdinarySlotEventPlan::after_117a0(
        rloft::InventoryOrdinarySwapStatus::LearnedMageSpell,
        scroll_before,{0xA6,0U},inventory,original_t);
    assert(plan.count==2U);
    assert(plan.at(0).item_id==0xA7 && plan.at(1).item_id==-1);
    assert(plan.at(0).owned_text_759c=="FIREBALL LEARNED!");
    assert(plan.at(1).owned_text_759c==plan.at(0).owned_text_759c);
    const auto before_scribe_ticks=tick_observations;
    // Native batch boundary: validate ALL original 117A0 F790 source
    // events before touching the saved page, shared RNG or IRQ observer.
    using Batch=rloft::OriginalF790OrdinarySlotDispatch;
    const auto before_bad_rng=shared_rng.state();
    const auto before_bad_events=sequence.f790_event_count();
    auto malformed=plan;
    malformed.events[1].source_e8_offset=0x1178eU;
    bool rejected_pair=false;
    try {
        (void)Batch::dispatch(malformed,sequence,framebuffer,
            original_objects,parsed,inventory,original_code2,original_t,
            ribbon,font,shared_rng,source_tick);
    } catch(const std::invalid_argument&) {rejected_pair=true;}
    assert(rejected_pair && sequence.f790_event_count()==before_bad_events);
    assert(shared_rng.state()==before_bad_rng);
    assert(tick_observations==before_scribe_ticks);

    malformed=plan;
    malformed.events[1].item_id=74; // Original 1184A pushes -1
    rejected_pair=false;
    try {
        (void)Batch::dispatch(malformed,sequence,framebuffer,
            original_objects,parsed,inventory,original_code2,original_t,
            ribbon,font,shared_rng,source_tick);
    } catch(const std::invalid_argument&) {rejected_pair=true;}
    assert(rejected_pair && sequence.f790_event_count()==before_bad_events);
    assert(shared_rng.state()==before_bad_rng);

    const auto dispatched=Batch::dispatch(
        plan,sequence,framebuffer,original_objects,parsed,inventory,
        original_code2,original_t,ribbon,font,shared_rng,source_tick);
    assert(dispatched==2U);
    assert(sequence.f790_event_count()==2U);
    assert(tick_observations==before_scribe_ticks);
    const auto two_draw_rng=shared_rng.state();
    for(int redraw=0;redraw<256;++redraw) {
        framebuffer.clear(0x31U);
        framebuffer.fill_rect(0x62,0,16,320,0x40U);
        sequence.replay_after_host_redraw(framebuffer,ribbon,font);
    }
    assert(shared_rng.state()==two_draw_rng);
    assert(sequence.f790_event_count()==2U);
    assert(tick_observations==before_scribe_ticks);
    sequence.leave_modal();
    inventory.mage_scribe_feedback.reset();

    // Original 13118 entry order: clear feedback; run 112EC actor
    // drawing; save the clean 320x16 source page; dispatch 1134C
    // F790 once. Host VGA repaints never draw another source event.
    inventory.held={-1,0U};
    rloft::OriginalF790ModalSequence first_entry{};
    rloft::IndexedFramebuffer entry_frame{};
    std::size_t actor_draw_count=0U;
    const auto before_entry_tick=tick_observations;
    const auto before_entry_rng=shared_rng.state();
    auto draw_actor_columns=[&](rloft::IndexedFramebuffer& fb) {
        ++actor_draw_count;
        fb.clear(0x12U);
        fb.fill_rect(0x62,0,16,320,0x77U);
    };
    rloft::OriginalF790ModalEntry::enter_13118(
        first_entry,entry_frame,draw_actor_columns,inventory,
        original_objects,parsed,original_code2,original_t,ribbon,font,
        shared_rng,source_tick);
    assert(first_entry.captured() && first_entry.f790_event_count()==1U);
    assert(actor_draw_count==1U && tick_observations==before_entry_tick);
    assert(shared_rng.state()==before_entry_rng);
    assert(entry_frame.pixels().at(0x62U*320U+0x44U)==0x77U);
    bool duplicate_entry=false;
    try {
        rloft::OriginalF790ModalEntry::enter_13118(
            first_entry,entry_frame,draw_actor_columns,inventory,
            original_objects,parsed,original_code2,original_t,ribbon,font,
            shared_rng,source_tick);
    } catch(const std::logic_error&) {duplicate_entry=true;}
    assert(duplicate_entry && actor_draw_count==1U);
    assert(first_entry.f790_event_count()==1U);

    // Held item entry is NOT a framebuffer refresh. Source F790
    // reseeds shared RNG from the held ID, draws the original tiles,
    // then reseeds from the observed original 30700 tick.
    first_entry.leave_modal();
    inventory.held={0,0U};
    rloft::OriginalF790ModalEntry::enter_13118(
        first_entry,entry_frame,draw_actor_columns,inventory,
        original_objects,parsed,original_code2,original_t,ribbon,font,
        shared_rng,source_tick);
    assert(actor_draw_count==2U && first_entry.f790_event_count()==1U);
    assert(tick_observations==before_entry_tick+1);
    assert(shared_rng.state()==0x76543210U);
    const auto after_open_rng=shared_rng.state();
    for(int redraw=0;redraw<128;++redraw) {
        draw_actor_columns(entry_frame);
        first_entry.replay_after_host_redraw(entry_frame,ribbon,font);
    }
    assert(first_entry.f790_event_count()==1U);
    assert(shared_rng.state()==after_open_rng);
    assert(tick_observations==before_entry_tick+1);
    first_entry.leave_modal();
    inventory.held={-1,0U};

    bool rejected=false;
    try { (void)describe(900); } catch(const std::out_of_range&) {rejected=true;}
    assert(rejected);
    rejected=false;
    auto damaged=original_objects;
    damaged[4U+0x2FU]=7U;
    try { (void)Gateway::compose(damaged,parsed,inventory,0,false,
                                  original_code2,original_t); }
    catch(const std::invalid_argument&) {rejected=true;}
    assert(rejected);
    rejected=false;
    // Identity guard also rejects stale source names when category,
    // weight, subtype and enchantment happen to match.
    rejected=false;
    damaged=original_objects;
    damaged[4U]='X';
    try { (void)Gateway::compose(damaged,parsed,inventory,0,false,
                                  original_code2,original_t); }
    catch(const std::invalid_argument&) {rejected=true;}
    assert(rejected);
    damaged=original_objects;
    damaged[0]=0U;
    try { (void)Gateway::compose(damaged,parsed,inventory,0,false,
                                  original_code2,original_t); }
    catch(const std::invalid_argument&) {rejected=true;}
    assert(rejected);
    rejected=false;
    damaged=original_objects;damaged.pop_back();
    try { (void)Gateway::compose(damaged,parsed,inventory,0,false,
                                  original_code2,original_t); }
    catch(const std::invalid_argument&) {rejected=true;}
    assert(rejected);
}
