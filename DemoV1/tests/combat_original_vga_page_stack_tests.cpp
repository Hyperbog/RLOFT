#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/combat_original_vga_page_stack.hpp"

#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <vector>

int main() {
    using namespace rloft;
    using Stack=CombatOriginalVgaPageStackModel;
    CombatOriginalVgaPageStackState state{};
    state.active_page_ef4d=7U; // intentional divergence from EFE9
    state.selected_page_efe9=0U;
    state.physical_page_base_f001=0xA0000U;
    state.page_stride_f005=0x800U;
    state.saved_pages_2b644.resize(10U,0xCCU);

    std::vector<std::uint32_t> diagnostics;
    std::vector<std::uint8_t> observed;
    CombatOriginalVgaPageStackServices services{};
    services.diagnostic_2f580=[&](std::uint32_t offset) {
        diagnostics.push_back(offset);
    };
    services.selected_page_291b0=[&](std::uint8_t page) {
        observed.push_back(page);
        // 291B0 performs 2AB6C before writing EF4D.
        assert(state.selected_page_efe9==page);
        assert(state.active_page_ef4d==page);
        assert(state.physical_page_base_f001==
               ((0xA000U+static_cast<std::uint32_t>(page)*
                 state.page_stride_f005)<<4U));
    };
    Stack::push_2e590(state,0x10002U,services);
    assert(state.depth_f0d0==1U);
    assert(state.saved_pages_2b644[0U]==7U); // NEVER EFE9=0
    assert(state.active_page_ef4d==2U);
    assert(state.selected_page_efe9==2U);
    assert(state.physical_page_base_f001==0xB0000U);

    Stack::push_2e590(state,1U,services);
    assert(state.depth_f0d0==2U);
    assert(state.saved_pages_2b644[1U]==2U);
    assert(state.active_page_ef4d==1U);
    Stack::pop_2e5d4(state,services);
    assert(state.depth_f0d0==1U&&state.active_page_ef4d==2U);
    Stack::pop_2e5d4(state,services);
    assert(state.depth_f0d0==0U&&state.active_page_ef4d==7U);
    assert((observed==std::vector<std::uint8_t>{2,1,2,7}));
    assert(diagnostics.empty());

    // Runtime F005 page stride is captured, NOT hardcoded to
    // 0x800 paragraphs (only the above fixture uses that).
    state.page_stride_f005=0x12U;
    Stack::push_2e590(state,3U,services);
    assert(state.physical_page_base_f001==
           ((0xA000U+3U*0x12U)<<4U));
    Stack::pop_2e5d4(state,services);

    // Original push checks exactly F0D0 == 0x0A. The diagnostic
    // is invoked before the later F0D0 re-read and stack store.
    state.depth_f0d0=10U;
    state.saved_pages_2b644.resize(11U);
    state.active_page_ef4d=9U;
    state.page_stride_f005=0U;
    services.diagnostic_2f580=[&](std::uint32_t offset) {
        diagnostics.push_back(offset);
        assert(offset==0x1358U);
        state.depth_f0d0=2U; // diagnostic callback can mutate DOS globals
    };
    services.selected_page_291b0={};
    Stack::push_2e590(state,4U,services);
    assert(state.saved_pages_2b644[2U]==9U);
    assert(state.depth_f0d0==3U);
    assert(state.active_page_ef4d==4U);
    assert(state.physical_page_base_f001==0xA0000U);
    assert(diagnostics.back()==0x1358U);

    // Overflow warning without an original handler is unsupported,
    // not a fabricated clamp-to-ten rule.
    state.depth_f0d0=10U;
    services.diagnostic_2f580={};
    bool rejected=false;
    try {Stack::push_2e590(state,0U,services);}
    catch(const std::invalid_argument&) {rejected=true;}
    assert(rejected&&state.depth_f0d0==10U);
    // Captured stack slot missing: fail before incrementing depth.
    state.depth_f0d0=8U;
    state.saved_pages_2b644.resize(8U);
    rejected=false;
    try {Stack::push_2e590(state,0U,services);}
    catch(const std::invalid_argument&) {rejected=true;}
    assert(rejected&&state.depth_f0d0==8U);

    // Empty-pop original diagnostic runs first; if it returns,
    // the byte decrement would wrap to FFh and access slot FFh.
    state.depth_f0d0=0U;
    state.saved_pages_2b644.assign(256U,0U);
    state.saved_pages_2b644[255U]=6U;
    services.diagnostic_2f580=[&](std::uint32_t offset){
        assert(offset==0x136CU);
        diagnostics.push_back(offset);
    };
    Stack::pop_2e5d4(state,services);
    assert(state.depth_f0d0==255U);
    assert(state.active_page_ef4d==6U);
    assert(diagnostics.back()==0x136CU);

    // Original pop also re-reads F0D0 following the diagnostic.
    state.depth_f0d0=0U;
    state.saved_pages_2b644[3U]=8U;
    services.diagnostic_2f580=[&](std::uint32_t){
        state.depth_f0d0=4U;
    };
    Stack::pop_2e5d4(state,services);
    assert(state.depth_f0d0==3U && state.active_page_ef4d==8U);
    assert(state.selected_page_efe9==8U);

    // Missing underflow error handler: no invented host recovery.
    state.depth_f0d0=0U;
    services.diagnostic_2f580={};
    rejected=false;
    try {Stack::pop_2e5d4(state,services);}
    catch(const std::invalid_argument&) {rejected=true;}
    assert(rejected && state.depth_f0d0==0U);

    // The original BYTE counter can increment FFh to zero when
    // its diagnostic threshold (=10) is not the active depth.
    state.depth_f0d0=255U;
    state.active_page_ef4d=3U;
    Stack::push_2e590(state,1U,services);
    assert(state.depth_f0d0==0U);
    assert(state.saved_pages_2b644[255U]==3U);
    assert(state.active_page_ef4d==1U);
}
