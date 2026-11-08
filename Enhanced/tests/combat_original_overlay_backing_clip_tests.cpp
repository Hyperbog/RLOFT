#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/combat_original_overlay_backing_clip.hpp"

#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <vector>

int main() {
    using namespace rloft;
    using Model=CombatOriginalOverlayClipModel;
    const CombatOriginalOverlayClipLimits bounds{400,320};
    const std::uint32_t negative3=0xFFFFFFFDU;
    const std::uint32_t negative2=0xFFFFFFFEU;
    const auto original=CombatOriginalOverlayBackingRequest{
        0x9E3AU,negative3,negative2,32U,16U};
    auto capture=Model::inspect_292bc(bounds,original);
    assert(capture.backend_called);
    assert(capture.args.row_bits==0U);
    assert(capture.args.column_bits==0U);
    assert(capture.args.row_count_bits==29U);
    assert(capture.args.column_count_bits==14U);
    // Original clipped negative top/left does NOT slide backing
    // pointer; the 2A114 resource address stays unchanged.
    assert(capture.args.backing_pointer==0x9E3AU);
    auto restore=Model::inspect_2932c(bounds,original);
    assert(restore.backend_called);
    assert(restore.args.row_count_bits==29U);
    assert(restore.args.column_count_bits==14U);

    const auto high=CombatOriginalOverlayBackingRequest{
        0xA23AU,398U,318U,10U,5U};
    capture=Model::inspect_292bc(bounds,high);
    assert(capture.backend_called);
    assert(capture.args.row_count_bits==2U);
    assert(capture.args.column_count_bits==2U);
    restore=Model::inspect_2932c(bounds,high);
    assert(restore.backend_called);
    assert(restore.args.row_count_bits==2U);
    assert(restore.args.column_count_bits==2U);

    // Exact last-row bound yields zero rows: both skip backend.
    const auto last_row=CombatOriginalOverlayBackingRequest{
        0x9E3AU,400U,8U,10U,16U};
    assert(!Model::inspect_292bc(bounds,last_row).backend_called);
    assert(!Model::inspect_2932c(bounds,last_row).backend_called);

    // DIFFERENCE: 292BC uses JLE for zero columns, whereas
    // 2932C uses JL and still invokes underlying 2A178
    // with ZERO column count. This can affect VGA register state.
    const auto zero_columns=CombatOriginalOverlayBackingRequest{
        0x9E3AU,20U,320U,2U,0U};
    capture=Model::inspect_292bc(bounds,zero_columns);
    restore=Model::inspect_2932c(bounds,zero_columns);
    assert(!capture.backend_called);
    assert(restore.backend_called);
    assert(restore.args.column_count_bits==0U);
    assert(restore.args.row_count_bits==2U);

    std::vector<char> trace;
    CombatOriginalOverlayBackingServices callback{};
    callback.capture_2a114=[&](const auto& args) {
        trace.push_back('C');
        assert(args.backing_pointer==0x9E3AU);
        assert(args.row_bits==0U && args.column_bits==0U);
        assert(args.row_count_bits==29U);
        assert(args.column_count_bits==14U);
    };
    callback.restore_2a178=[&](const auto& args) {
        trace.push_back('R');
        assert(args.backing_pointer==0x9E3AU);
        assert(args.column_count_bits==0U);
    };
    assert(Model::capture_292bc(bounds,original,callback).backend_called);
    assert(!Model::capture_292bc(bounds,zero_columns,callback).backend_called);
    assert(Model::restore_2932c(bounds,zero_columns,callback).backend_called);
    assert((trace==std::vector<char>{'C','R'}));

    // Missing backend only fails on a path original code calls.
    CombatOriginalOverlayBackingServices missing{};
    assert(!Model::capture_292bc(bounds,zero_columns,missing).backend_called);
    bool rejected=false;
    try{(void)Model::restore_2932c(bounds,zero_columns,missing);}
    catch(const std::invalid_argument&){rejected=true;}
    assert(rejected);
    rejected=false;
    try{(void)Model::capture_292bc(bounds,original,missing);}
    catch(const std::invalid_argument&){rejected=true;}
    assert(rejected);

    // Clipped negative column can produce a negative remaining
    // count and skip BOTH backends; no invented source reads.
    const auto negative_clipped=CombatOriginalOverlayBackingRequest{
        0x9E3AU,1U,negative2,10U,1U};
    assert(!Model::inspect_292bc(bounds,negative_clipped).backend_called);
    assert(!Model::inspect_2932c(bounds,negative_clipped).backend_called);

    // Two sequential original 32-bit ADDs can wrap. The final
    // counts are tested SIGNED, not normalized or saturated.
    const auto wrapped=CombatOriginalOverlayBackingRequest{
        0x9E3AU,0x7FFFFFFFU,0U,0x80000002U,1U};
    assert(!Model::inspect_292bc(bounds,wrapped).backend_called);
    assert(!Model::inspect_2932c(bounds,wrapped).backend_called);
}
