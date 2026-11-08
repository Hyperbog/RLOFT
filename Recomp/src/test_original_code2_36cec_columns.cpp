#ifdef NDEBUG
#undef NDEBUG
#endif
#include "original_code2_36cec_columns.hpp"
#include <array>
#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <vector>

using namespace rloft_recomp;

int main() {
    // Synthetic RES3 EH7, 64 columns with [31..33] around source center 32.
    // The table at 0x0A has three offsets, including a shared unaligned list.
    std::vector<std::uint8_t> body(27,0);
    body[0]=64;body[2]=31;body[4]=33;
    body[6]=0;body[8]=128;
    body[10]=17;body[12]=17;body[14]=25;
    // At offset 17: (stop=2, signed_adjustment=-1, entry=4), terminator.
    body[17]=2;body[19]=0xff;body[20]=0xff;body[21]=4;
    // At offset 25: zero-terminated empty list.
    std::array<std::uint16_t,509> lookup{};
    lookup[64]=0x0909;
    Original36cecCall call{};
    call.screen_x=128;call.res3_id=1829;
    call.projected_size=128;call.lookup_bias=0;
    call.vertical_mode=0x58;call.ray_depth=4000;
    const auto plan=prepare_original_36cec(
        call,body.data(),body.size(),127,lookup.data(),lookup.size(),508);
    assert(plan.drawable);
    assert(plan.original_scale==16 && plan.clamped_scale==16);
    assert(plan.scale_pointer_index==14);
    assert(plan.lookup_index==64 && plan.packed_lookup==0x0808);
    assert((plan.source_x_order==std::vector<std::uint16_t>{31,32,33}));
    assert(plan.descriptors.size()==2);
    assert(plan.descriptors[0].source_x==31 &&
           plan.descriptors[1].source_x==32);
    assert(plan.descriptors[0].signed_adjustment==-1);
    // Source 36D07 skips RES3 when projected size is less than 8.
    call.projected_size=7;
    const auto zero=prepare_original_36cec(
        call,nullptr,0,127,lookup.data(),lookup.size(),508);
    assert(!zero.drawable && zero.source_x_order.empty());
    // 36CFA uses the size's masked *low 16 bits*, not an unclamped dword.
    call.projected_size=0x10080;
    const auto wrapping=prepare_original_36cec(
        call,body.data(),body.size(),127,lookup.data(),lookup.size(),508);
    assert(wrapping.original_scale==0x2010);
    assert(wrapping.clamped_scale==127);
    assert(wrapping.lookup_index==64);
    // Full display-list preflight rejects malformed data before any writes.
    call.projected_size=128;
    auto invalid=body;
    invalid[10]=0xff;invalid[11]=0xff;
    bool rejected=false;
    try {(void)prepare_original_36cec(call,invalid.data(),invalid.size(),
         127,lookup.data(),lookup.size(),508);}
    catch (const std::out_of_range&) {rejected=true;}
    assert(rejected);
    invalid=body;invalid[17]=3;
    rejected=false;
    try {(void)prepare_original_36cec(call,invalid.data(),invalid.size(),
         127,lookup.data(),lookup.size(),508);}
    catch (const std::invalid_argument&) {rejected=true;}
    assert(rejected);
    // The second argument, not the first, identifies the original RES3.
    call.res3_id=4234;
    rejected=false;
    try {(void)prepare_original_36cec(call,body.data(),body.size(),
         127,lookup.data(),lookup.size(),508);}
    catch (const std::out_of_range&) {rejected=true;}
    assert(rejected);
    return 0;
}
