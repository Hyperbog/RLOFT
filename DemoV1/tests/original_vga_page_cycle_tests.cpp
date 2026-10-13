#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/original_vga_page_cycle.hpp"

#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

int main() {
    using rloft::OriginalVgaPageCycleCallbacks;
    using rloft::OriginalVgaPageCycleModel;
    using rloft::OriginalVgaPageCycleState;
    using State=OriginalVgaPageCycleState;
    using Model=OriginalVgaPageCycleModel;

    // Original shipped CODE.2 static initial data: C050=1, EF4D=0.
    // FDBC is not the current pointer until 1F1D6 runs.
    State s{1U,0U,0U};
    Model::calculate_fdbc_1f1d6(s);
    assert(s.ground_base_fdbc==0xA8000U);
    assert(s.selected_page_ef4d==0U);
    std::vector<std::string> order;
    OriginalVgaPageCycleCallbacks callbacks{};
    callbacks.present_1a510=[&](std::uint32_t pushed) {
        order.push_back("present");
        assert(pushed==1U); // FULL C050 argument, not EF4D=0
        assert(s.draw_page_c050==1U); // BEFORE XOR
        assert(s.ground_base_fdbc==0xA8000U);
        assert(s.selected_page_ef4d==0U);
    };
    Model::present_then_flip_1ea78(s,callbacks);
    assert((order==std::vector<std::string>{"present"}));
    assert(s.draw_page_c050==0U);
    assert(s.selected_page_ef4d==0U);
    // 1EA78 does NOT write FDBC; only a future 1F1D6 does.
    assert(s.ground_base_fdbc==0xA8000U);
    Model::calculate_fdbc_1f1d6(s);
    assert(s.ground_base_fdbc==0xA0000U);

    callbacks.select_29fec=[&](std::uint32_t page) {
        order.push_back("select");
        assert(page==1U); // ZERO EXTENDED low byte, not 0x101
        assert(s.selected_page_ef4d==0U); // BEFORE STORE
        assert(s.draw_page_c050==0U);
    };
    Model::select_then_store_291b0(s,0x101U,callbacks);
    assert(s.selected_page_ef4d==1U);
    assert(s.draw_page_c050==0U); // independent video-page domains
    assert((order==std::vector<std::string>{"present","select"}));

    // 1F1C8 calls 291B0(C050 low byte) BEFORE FDBC. The page
    // selector can have effects before 1F1D6 reads the full DWORD.
    State draw{1U,0U,0xDEADBEEFU};
    OriginalVgaPageCycleCallbacks draw_cb{};
    draw_cb.select_29fec=[&](std::uint32_t page) {
        assert(page==1U);
        assert(draw.selected_page_ef4d==0U); // before 291B0 store
        assert(draw.ground_base_fdbc==0xDEADBEEFU); // before FDBC
        assert(draw.draw_page_c050==1U);
    };
    Model::enter_ground_draw_1f1c8_prefix(draw,draw_cb);
    assert(draw.selected_page_ef4d==1U);
    assert(draw.draw_page_c050==1U);
    assert(draw.ground_base_fdbc==0xA8000U);

    // 291B0's provider can update guest page globals: original
    // 1F1D6 loads C050 AGAIN afterward, not the old pushed page.
    draw={1U,0U,0xF00DU};
    draw_cb.select_29fec=[&](std::uint32_t page) {
        assert(page==1U && draw.selected_page_ef4d==0U);
        draw.draw_page_c050=0U;
    };
    Model::enter_ground_draw_1f1c8_prefix(draw,draw_cb);
    assert(draw.draw_page_c050==0U);
    assert(draw.selected_page_ef4d==1U); // original 291B0 input
    assert(draw.ground_base_fdbc==0xA0000U); // post-callback C050

    // Any missing 29FEC dependency rejects BEFORE either store.
    draw={1U,0U,0xCAFEU};
    bool no_draw_callback=false;
    try {
        Model::enter_ground_draw_1f1c8_prefix(draw,{});
    } catch(const std::invalid_argument&) {no_draw_callback=true;}
    assert(no_draw_callback && draw.draw_page_c050==1U);
    assert(draw.selected_page_ef4d==0U);
    assert(draw.ground_base_fdbc==0xCAFEU);

    // 1EA78 XORs only BIT ZERO of the LOW BYTE, preserving high
    // bytes of DWORD[C050] even for unexpected guest memory values.
    s={0xA1B2C3D5U,7U,0x12345678U};
    callbacks.present_1a510=[&](std::uint32_t pushed) {
        assert(pushed==0xA1B2C3D5U);
        s.draw_page_c050=0xF0000101U;
        s.selected_page_ef4d=9U;
    };
    Model::present_then_flip_1ea78(s,callbacks);
    assert(s.draw_page_c050==0xF0000100U);
    assert(s.selected_page_ef4d==9U);
    assert(s.ground_base_fdbc==0x12345678U);
    Model::calculate_fdbc_1f1d6(s);
    assert(s.ground_base_fdbc==
           static_cast<std::uint32_t>(0xA0000U+(0xF0000100U<<15U)));
    // Byte selector is loaded from the incoming stack argument again
    // after 29FEC: a callback's EF4D write cannot replace it.
    callbacks.select_29fec=[&](std::uint32_t page) {
        assert(page==0xABU);
        s.selected_page_ef4d=3U;
        s.draw_page_c050=0x70000000U;
    };
    Model::select_then_store_291b0(s,0x1234ABU,callbacks);
    assert(s.selected_page_ef4d==0xABU);
    assert(s.draw_page_c050==0x70000000U);

    // No callback means the original function cannot be simulated.
    // Fail before mutation; never silently invent a successful draw.
    const State before=s;
    bool rejected=false;
    try {
        Model::present_then_flip_1ea78(s,{});
    } catch(const std::invalid_argument&) {rejected=true;}
    assert(rejected);
    assert(s.draw_page_c050==before.draw_page_c050);
    assert(s.selected_page_ef4d==before.selected_page_ef4d);
    assert(s.ground_base_fdbc==before.ground_base_fdbc);
    rejected=false;
    try {
        Model::select_then_store_291b0(s,1U,{});
    } catch(const std::invalid_argument&) {rejected=true;}
    assert(rejected && s.selected_page_ef4d==before.selected_page_ef4d);

    // Repeated 1EA78 calls flip C050 but do not touch selected EF4D.
    s={1U,0U,0U};
    callbacks.present_1a510=[&](std::uint32_t page) {
        assert(page==s.draw_page_c050);
    };
    for(unsigned i=0;i<100U;++i) {
        Model::present_then_flip_1ea78(s,callbacks);
        assert(s.draw_page_c050==(i%2U==0U?0U:1U));
        assert(s.selected_page_ef4d==0U);
    }
    return 0;
}
