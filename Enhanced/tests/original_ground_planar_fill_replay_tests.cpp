#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/original_ground_planar_fill_replay.hpp"
#include <cassert>
#include <stdexcept>

int main() {
    using namespace rloft;
    using Fill=OriginalGroundPlanarFillReplay;
    using Strip=OriginalGroundFillStrip;
    SurfaceFillTable table{};
    for(std::size_t i=0;i<table.subtract.size();++i)
        table.subtract[i]=static_cast<std::uint8_t>(i);
    IndexedFramebuffer empty;
    empty.clear(0xE6U);
    IndexedFramebuffer reference=empty;
    SurfaceFillModel::draw_upper(reference,table,200U);
    SurfaceFillModel::draw_lower(reference,table,10U);
    VgaPlanarMemory memory;
    memory.import_page(empty,0U);
    memory.import_page(empty,1U);
    const auto upper=Fill::replay_flat_es(
        memory,table,Strip::Upper33E28,200U,0U);
    assert(upper.source_fdbc==0xA0000U);
    assert(upper.first_linear_address==0xA0148U);
    assert(upper.last_row_first_linear_address==0xA28F8U);
    assert(upper.last_linear_address==0xA2937U);
    assert(upper.planar_byte_addresses_written==8192U);
    assert(upper.indexed_lanes_written==32768U);
    assert(upper.sequencer_plane_mask==0x0FU);
    assert(memory.sequencer_map_mask_02().value()==0x0FU);
    const auto lower=Fill::replay_flat_es(
        memory,table,Strip::Lower33DE0,10U,0U);
    assert(lower.first_linear_address==0xA50F8U);
    assert(lower.last_row_first_linear_address==0xA2948U);
    assert(lower.last_linear_address==0xA2987U);
    IndexedFramebuffer actual;
    memory.export_page(actual,0U);
    assert(actual.pixels()==reference.pixels());
    memory.export_page(actual,1U);
    assert(actual.pixels()==empty.pixels());

    // Original second planar bank is +0x8000 BYTE addresses,
    // not +320*400 chunky pixels; first bank remains unchanged.
    const auto page1=Fill::replay_flat_es(
        memory,table,Strip::Upper33E28,72U,1U);
    assert(page1.source_fdbc==0xA8000U);
    assert(page1.first_linear_address==0xA8148U);
    assert(page1.last_row_first_linear_address==0xAA8F8U);
    assert(page1.last_linear_address==0xAA937U);
    IndexedFramebuffer page1_ref=empty;
    SurfaceFillModel::draw_upper(page1_ref,table,72U);
    memory.export_page(actual,1U);
    assert(actual.pixels()==page1_ref.pixels());
    memory.export_page(actual,0U);
    assert(actual.pixels()==reference.pixels());

    // 8-bit wrapping subtraction and every physical plane.
    table.subtract[128U]=0xF1U;
    const auto altered=Fill::replay_flat_es(
        memory,table,Strip::Lower33DE0,2U,1U);
    for(std::uint8_t plane=0U;plane<4U;++plane)
        assert(memory.read(altered.first_linear_address,plane)==
               static_cast<std::uint8_t>(2U-0xF1U));

    const auto mask=memory.sequencer_map_mask_02();
    bool failed=false;
    try {
        (void)Fill::replay_flat_es(
            memory,table,Strip::Lower33DE0,3U,2U);
    } catch(const std::out_of_range&) {
        failed=true;
    }
    assert(failed && memory.sequencer_map_mask_02()==mask);
    return 0;
}
