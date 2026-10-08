#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/combat_original_sprite_clip.hpp"
#include "rloft/combat_original_sprite_clip_binding.hpp"

#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace {
std::uint32_t dword(const rloft::CombatOriginalDgroupCapture& m,
                     unsigned index) {
    const auto at=static_cast<std::size_t>(index)*4U;
    return static_cast<std::uint32_t>(m.bytes[at])
        |(static_cast<std::uint32_t>(m.bytes[at+1U])<<8U)
        |(static_cast<std::uint32_t>(m.bytes[at+2U])<<16U)
        |(static_cast<std::uint32_t>(m.bytes[at+3U])<<24U);
}
}
int main() {
    using namespace rloft;
    CombatOriginalSpriteClipGlobals viewport{};
    viewport.viewport_first_row_ef55=0U;
    viewport.viewport_last_row_ef59=319U;
    viewport.viewport_first_column_ef5d=0U;
    viewport.viewport_last_column_ef61=399U;
    CombatOriginalDgroupCapture dos{};
    dos.captured_base_dgroup_offset=0xEF74U;
    dos.bytes.assign(24U,0xCCU);
    unsigned refreshes=0U,full_draws=0U,clipped_draws=0U;
    std::uint16_t resource_height=2U;
    std::uint8_t resource_byte_width=1U;
    CombatOriginalSpriteClipServices s{};
    s.refresh_2e758=[&](){++refreshes;};
    s.read_sprite_word=[&](std::uint32_t at)
        ->std::optional<std::uint16_t> {
        assert(at==0x1004U);return resource_height;
    };
    s.read_sprite_byte=[&](std::uint32_t at)
        ->std::optional<std::uint8_t> {
        assert(at==0x1006U);return resource_byte_width;
    };
    s.full_sprite_2a380=[&](
        std::uint32_t mode,std::uint32_t row,
        std::uint32_t col,std::uint32_t resource) {
        ++full_draws;
        assert(mode==4U&&row==10U&&col==20U&&resource==0x1000U);
        for(unsigned i=0U;i<6U;++i)
            assert(dword(dos,i)==(i==2U?2U:(i==5U?8U:0U)));
    };
    s.clipped_sprite_2a6b0=[&](
        std::uint32_t mode,std::uint32_t row,std::uint32_t col,
        std::uint32_t scratch,std::uint32_t resource) {
        ++clipped_draws;
        assert(mode==4U&&resource==0x1000U&&scratch==0xEF74U);
        assert(row==0U&&col==0U);
    };
    assert(CombatOriginalSpriteClipModel::draw_29414(
        viewport,dos,4U,10U,20U,0x1000U,0xEF74U,s)==2U);
    assert(full_draws==1U && clipped_draws==0U && refreshes==1U);
    assert(viewport.output_row_ef69==10U);
    assert(viewport.output_column_ef6b==20U);
    assert(viewport.output_rows_ef6d==2U);
    assert(viewport.output_columns_ef6f==8U);

    // 29414 refreshes BEFORE null resource check, and must
    // not access EF74 when the pointer is null.
    assert(CombatOriginalSpriteClipModel::draw_29414(
        viewport,dos,0U,0U,0U,0U,0xABCDU,s)==0U);
    assert(refreshes==2U);

    // Crop from negative first row/column and the last column.
    viewport.viewport_last_row_ef59=10U;
    viewport.viewport_last_column_ef61=7U;
    resource_height=4U;resource_byte_width=2U;
    const auto neg1=0xFFFFFFFFU;
    const auto neg2=0xFFFFFFFEU;
    assert(CombatOriginalSpriteClipModel::draw_29414(
        viewport,dos,4U,neg1,neg2,0x1000U,0xEF74U,s)==1U);
    assert(clipped_draws==1U);
    assert(dword(dos,0U)==16U); // top one row * 16 bytes
    assert(dword(dos,1U)==0U);
    assert(dword(dos,2U)==3U);
    assert(dword(dos,3U)==2U);
    assert(dword(dos,4U)==6U);
    assert(dword(dos,5U)==8U);
    assert(viewport.output_row_ef69==0U);
    assert(viewport.output_column_ef6b==0U);
    assert(viewport.output_rows_ef6d==3U);
    assert(viewport.output_columns_ef6f==8U);

    // Bottom and right trims use the CURRENT remaining dimensions.
    s.clipped_sprite_2a6b0=[&](
        std::uint32_t mode,std::uint32_t row,std::uint32_t col,
        std::uint32_t scratch,std::uint32_t resource) {
        ++clipped_draws;
        assert(mode==4U&&row==8U&&col==6U);
        assert(scratch==0xEF74U&&resource==0x1000U);
    };
    viewport.viewport_last_row_ef59=9U;
    assert(CombatOriginalSpriteClipModel::draw_29414(
        viewport,dos,4U,8U,6U,0x1000U,0xEF74U,s)==1U);
    assert(dword(dos,0U)==0U&&dword(dos,1U)==32U);
    assert(dword(dos,2U)==2U&&dword(dos,3U)==0U);
    assert(dword(dos,4U)==14U&&dword(dos,5U)==2U);

    // Entire sprite outside top edge returns zero after updating
    // record and viewport WORDs; no synthetic original raster call.
    const auto old_count=clipped_draws;
    assert(CombatOriginalSpriteClipModel::draw_29414(
        viewport,dos,4U,0xFFFFFFCEU,0U,0x1000U,0xEF74U,s)==0U);
    assert(clipped_draws==old_count);
    assert(viewport.output_rows_ef6d!=0U);

    // Unsupported original resource/scratch memory is an explicit
    // error, not an arbitrary invented sprite height.
    dos.bytes.resize(3U);
    bool rejected=false;
    try {(void)CombatOriginalSpriteClipModel::draw_29414(
        viewport,dos,4U,0U,0U,0x1000U,0xEF74U,s);}
    catch(const std::logic_error&) {rejected=true;}
    assert(rejected);

    // Fully bound 29804 passes the original EF74 record pointer
    // and increments EFD4 BEFORE the source 29414 service.
    dos.bytes.assign(24U,0U);
    viewport.viewport_last_row_ef59=319U;
    viewport.viewport_last_column_ef61=399U;
    resource_height=2U;resource_byte_width=1U;
    CombatOriginalOverlayDrawState depth{};
    auto original_draw=CombatOriginalSpriteClipBinder::bind_29804(
        viewport,dos,s,CombatOriginalOverlayDrawServices{});
    assert(original_draw.draw_29414);
    original_draw.draw_29414(4,10,20,0x1000U,0xEF74U);
    assert(dword(dos,2U)==2U && dword(dos,5U)==8U);
}
