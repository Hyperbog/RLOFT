#ifdef NDEBUG
#undef NDEBUG // Keep assertions active in optimized Release CTest.
#endif
#include "rloft/combat_irq_display_runtime.hpp"

#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

int main() {
    using namespace rloft;

    // 291B0 -> 29FEC -> 2AB6C selects original VGA page and
    // computes F001 from OBSERVED F005 rather than guessing stride.
    const auto p0=CombatVgaPageSelectionModel::inspect(0U,0x800U);
    const auto p1=CombatVgaPageSelectionModel::inspect(1U,0x800U);
    const auto p2=CombatVgaPageSelectionModel::inspect(2U,0x800U);
    assert(p0.original_f001_vga_linear_base==0xA0000U);
    assert(p1.original_f001_vga_linear_base==0xA8000U);
    assert(p2.original_f001_vga_linear_base==0xB0000U);
    assert(p2.original_ef4d_page==2U && p2.original_efe9_page==2U);
    assert(CombatVgaPageSelectionModel::inspect(1U,0U)
           .original_f001_vga_linear_base==0xA0000U);

    CombatIrqDisplayRuntime state{};
    state.pending_cf59=1U;
    state.deadline_cf54=190U;
    state.active_render_mode_c050=1U;
    state.background_resource_2222c=0x1234U;
    state.queued_message_14580={'A',0U};
    std::vector<std::string> events;

    CombatIrqDisplayServices cb{};
    cb.select_vga_page_291b0=[&](std::uint8_t p) {
        assert(p==1U);
        events.push_back("page");
    };
    cb.draw_background_2939c=[&](
        std::uint32_t x,std::uint32_t y,std::uint32_t w,std::uint32_t id) {
        assert(x==0U && y==262U && w==32U && id==0x1234U);
        events.push_back("background");
    };
    cb.draw_text_3060c=[&](
        std::uint32_t x,std::uint32_t y,
        const std::vector<std::uint8_t>& bytes,std::uint32_t style) {
        assert(x==263U && y==34U && style==1U && bytes[0]=='A');
        events.push_back("text");
    };
    cb.composite_1a8c8=[&](
        std::uint32_t page,std::uint32_t x,std::uint32_t y,
        std::uint32_t h,std::uint32_t w) {
        assert(page==1U && x==262U && y==32U && h==12U && w==256U);
        events.push_back("composite");
    };
    cb.observed_irq_tick_30700=[&]() {
        assert(state.pending_cf59==0U);
        events.push_back("irq");
        return 191U;
    };
    CombatIrqDisplayRuntimeModel::update(state,cb);
    assert((events==std::vector<std::string>{
        "page","background","text","composite","irq",
        "page","background","composite"
    }));
    assert(state.deadline_cf54==0xFFFFFFFFU);
    assert(state.pending_cf59==0U);

    // Renderer callbacks can mutate original globals. Read them at
    // EVERY later call site, not once at function entry.
    events.clear();
    state.pending_cf59=1U;
    state.deadline_cf54=190U;
    state.active_render_mode_c050=1U;
    cb.composite_1a8c8=[&](
        std::uint32_t,std::uint32_t,std::uint32_t,
        std::uint32_t,std::uint32_t) {
        events.push_back("composite");
        if (state.pending_cf59!=0U) {
            state.active_render_mode_c050=0U;
            state.background_resource_2222c=17U;
        }
    };
    cb.select_vga_page_291b0=[&](std::uint8_t p) {
        events.push_back(p!=0U?"page1":"page0");
    };
    cb.draw_background_2939c=[&](
        std::uint32_t,std::uint32_t,std::uint32_t,std::uint32_t id) {
        assert(id==0x1234U || id==17U);
        events.push_back("background");
    };
    cb.draw_text_3060c=[&](
        std::uint32_t,std::uint32_t,
        const std::vector<std::uint8_t>&,std::uint32_t) {
        events.push_back("text");
    };
    CombatIrqDisplayRuntimeModel::update(state,cb);
    assert((events==std::vector<std::string>{
        "page1","background","text","composite","irq",
        "page0","background","composite"
    }));

    // Without pending/expired presentation, original 1C2C4 only
    // reads IRQ time and does not require renderer collaborators.
    events.clear();
    state.pending_cf59=0U;
    state.deadline_cf54=0xFFFFFFFFU;
    CombatIrqDisplayRuntimeModel::update(state,cb);
    assert((events==std::vector<std::string>{"irq"}));
    CombatIrqDisplayServices only_clock{};
    only_clock.observed_irq_tick_30700=[](){return 1U;};
    CombatIrqDisplayRuntimeModel::update(state,only_clock);
    assert(state.deadline_cf54==0xFFFFFFFFU);

    // A missing clock is unsupported and cannot be replaced with
    // Windows elapsed time or a synthetic game-world tick.
    cb.observed_irq_tick_30700={};
    bool rejected=false;
    try {CombatIrqDisplayRuntimeModel::update(state,cb);}
    catch(const std::invalid_argument&) {rejected=true;}
    assert(rejected);
}
