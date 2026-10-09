#include "rloft/original_ground_texture_raster.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft {
namespace {
constexpr std::size_t kCode2DgroupFileOffset=0x75F24U;
constexpr std::size_t kShadingBegin=0xFE15U;
constexpr std::int64_t kPlanarBytesPerScanline=80;

std::uint16_t word(const std::uint8_t* source) noexcept {
    return static_cast<std::uint16_t>(
        static_cast<std::uint16_t>(source[0]) |
        (static_cast<std::uint16_t>(source[1]) << 8U));
}

// CODE.2 34350 uses two consecutive SHLD ESI,EDX,6 and
// SHLD ESI,EAX,6, after XOR ESI,ESI. The resulting source index
// is exactly ((V >> 26) << 6) | (U >> 26): a wrapped 64x64 texel.
std::size_t source_offset(std::uint32_t u,std::uint32_t v) noexcept {
    return static_cast<std::size_t>(
        ((v >> 26U) << 6U) | (u >> 26U));
}
} // namespace

GroundTextureShadeTable GroundTextureShadeTable::from_code2(
    const std::vector<std::uint8_t>& code2) {
    constexpr auto bytes=GroundTextureShadeTable::kBytes;
    const auto start=kCode2DgroupFileOffset+kShadingBegin;
    if(start>code2.size() || bytes>code2.size()-start) {
        throw std::runtime_error(
            "CODE.2 original textured shading table FE15..FF14 truncated");
    }
    GroundTextureShadeTable out{};
    for(std::size_t i=0U;i<bytes;++i) {
        out.subtract[i]=code2[start+i];
    }
    return out;
}

std::array<GroundPerspectiveRow,GroundTextureRasterSpec::kMaxRows>
OriginalGroundTextureRaster::decode_ff58(
    const std::vector<std::uint8_t>& original_ff58) {
    constexpr auto count=GroundTextureRasterSpec::kMaxRows;
    if(original_ff58.size()!=count*6U) {
        throw std::invalid_argument(
            "CODE.2 34998 perspective records must contain 128*6 bytes");
    }
    std::array<GroundPerspectiveRow,count> out{};
    for(std::size_t row=0U;row<count;++row) {
        const auto* record=original_ff58.data()+row*6U;
        out[row]={word(record),word(record+2U),
                  record[4],record[5]};
    }
    return out;
}

GroundTextureRasterReport OriginalGroundTextureRaster::draw(
    IndexedFramebuffer& framebuffer,
    const std::vector<std::uint8_t>& original_texture,
    const GroundTextureShadeTable& shades,
    const std::array<GroundPerspectiveRow,
        GroundTextureRasterSpec::kMaxRows>& rows,
    const GroundTextureRasterSpec& spec) {
    if(original_texture.size() <
            GroundTextureRasterSpec::kTextureHeaderBytes+
            GroundTextureRasterSpec::kTextureBytes) {
        throw std::invalid_argument(
            "original 34A74 source lacks 2-byte header plus 64x64 tile");
    }
    if(spec.row_count==0U ||
       spec.row_count>GroundTextureRasterSpec::kMaxRows ||
       (spec.row_step_bytes!=80 && spec.row_step_bytes!=-80) ||
       (spec.shade_index_step!=1 && spec.shade_index_step!=-1)) {
        throw std::invalid_argument(
            "original 34A74 requires 1..128 rows and signed 80-byte steps");
    }

    // Verify EVERY destination before modifying the framebuffer.
    // The active VGA page base FDBC is intentionally omitted here.
    std::array<std::size_t,GroundTextureRasterSpec::kMaxRows>
        destination_start{};
    for(std::size_t row=0U;row<spec.row_count;++row) {
        const auto planar_address=
            static_cast<std::int64_t>(
                spec.first_planar_byte_offset)+
            static_cast<std::int64_t>(spec.row_step_bytes)*
                static_cast<std::int64_t>(row);
        if(planar_address<0 ||
           planar_address>=0x8000 ||
           planar_address%kPlanarBytesPerScanline != 8) {
            throw std::out_of_range(
                "original 34A74 destination must start at viewport x32");
        }
        const auto y=planar_address/kPlanarBytesPerScanline;
        if(y<0 || y>=IndexedFramebuffer::kHeight) {
            throw std::out_of_range(
                "original 34A74 destination leaves indexed framebuffer");
        }
        destination_start[row]=
            static_cast<std::size_t>(y)*
                static_cast<std::size_t>(IndexedFramebuffer::kWidth)+
            32U;
    }

    auto staged=framebuffer.pixels(); // Transactional VGA simulation.
    GroundTextureRasterReport report{};
    for(std::size_t row=0U;row<spec.row_count;++row) {
        const auto& record=rows[row];

        // 34AD5 starts at FE94 + (0x7F * FF4C), then 34B24
        // subtracts FF4C once per row. The original 256-byte
        // extent FE15..FF14 is sufficient for either direction.
        const auto shade_index=static_cast<std::int64_t>(0x7FU) +
            (static_cast<std::int64_t>(0x7FU)-
             static_cast<std::int64_t>(row))*
                static_cast<std::int64_t>(spec.shade_index_step);
        if(shade_index<0 ||
           shade_index>=static_cast<std::int64_t>(
               GroundTextureShadeTable::kBytes)) {
            throw std::out_of_range(
                "original 34A74 FE94 shading pointer leaves table");
        }
        const auto shade=shades.subtract[
            static_cast<std::size_t>(shade_index)];
        const auto first_u=static_cast<std::uint32_t>(
            static_cast<std::uint8_t>(
                record.origin_u+spec.phase_u))<<24U;
        const auto first_v=static_cast<std::uint32_t>(
            static_cast<std::uint8_t>(
                record.origin_v+spec.phase_v))<<24U;
        const auto delta_u=static_cast<std::uint32_t>(
            record.delta_u)<<16U;
        const auto delta_v=static_cast<std::uint32_t>(
            record.delta_v)<<16U;
        const bool individual=row<
            GroundTextureRasterSpec::kFirstFullRows;
        const std::size_t phase_step=individual?1U:2U;
        for(std::size_t column=0U;
            column<GroundTextureRasterSpec::kViewportWidth;
            column+=phase_step) {
            // 342E4 and 34954 start each lane at initial U/V +
            // lane*delta and then sample every fourth column.
            // 34954 sets VGA map masks 0x0C and 0x03, so one
            // palette index is replicated into each adjacent pair.
            const auto u=first_u+
                delta_u*static_cast<std::uint32_t>(column);
            const auto v=first_v+
                delta_v*static_cast<std::uint32_t>(column);
            const auto index=source_offset(u,v);
            const auto color=static_cast<std::uint8_t>(
                original_texture[
                    GroundTextureRasterSpec::kTextureHeaderBytes+index]-
                shade);
            for(std::size_t lane=0U;lane<phase_step;++lane) {
                staged[destination_start[row]+column+lane]=color;
                ++report.indexed_writes;
            }
            ++report.source_reads;
        }
        if(individual) ++report.individual_plane_rows;
        else ++report.paired_plane_rows;
        ++report.rows;
    }
    framebuffer.mutable_pixels().swap(staged);
    return report;
}

} // namespace rloft
