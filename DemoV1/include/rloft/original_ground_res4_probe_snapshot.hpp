#pragma once

#include "rloft/original_ground_res4_audit.hpp"

#include <algorithm>
#include <exception>
#include <cstdint>
#include <iomanip>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

namespace rloft {

// Read-only forensic snapshot: preserve archive and selected-entry evidence
// even when the original source-selected RES4 entry cannot be decoded.
// Never substitute an inspected candidate for the source-selected entry.
struct OriginalGroundRes4ProbeSnapshot final {
    OriginalGroundRes4Report archive{};
    int source_base{-1};
    int signed_level_selector{};
    int source_entry{-1};
    std::size_t selected_raw_bytes{};
    std::string selected_prefix_hex{"unavailable"};
    bool verified{};
    std::size_t verified_pixel_bytes{};
    std::string error{};

    [[nodiscard]] static OriginalGroundRes4ProbeSnapshot capture(
        const std::vector<std::uint8_t>& init,
        const LevelState& level,
        const ResArchive& res4) {
        OriginalGroundRes4ProbeSnapshot out{};
        // The former tool invoked resolve() before inspect(), losing ALL
        // archive evidence on any failed decode. Inspect FIRST.
        out.archive=OriginalGroundRes4Audit::inspect(res4);
        const auto selector=static_cast<std::uint16_t>(
            static_cast<std::uint16_t>(level.header[7U]) |
            (static_cast<std::uint16_t>(level.header[8U])<<8U));
        out.signed_level_selector=selector<=0x7FFFU
            ? static_cast<int>(selector)
            : static_cast<int>(selector)-0x10000;

        try {
            out.source_base=OriginalGroundRes4Audit::base_from_init(init);
            out.source_entry=OriginalGroundRes4Audit::absolute_index(
                out.signed_level_selector,out.source_base,out.archive.total_entries);
            const auto raw=res4.entry(out.source_entry);
            out.selected_raw_bytes=raw.size();
            std::ostringstream hex;
            hex<<std::hex<<std::uppercase<<std::setfill('0');
            const auto limit=std::min<std::size_t>(13U,raw.size());
            for(std::size_t i=0;i<limit;++i) {
                if(i!=0U) hex<<' ';
                hex<<std::setw(2)<<unsigned(raw[i]);
            }
            out.selected_prefix_hex=hex.str();
            if(out.selected_prefix_hex.empty())out.selected_prefix_hex="empty";
            // Exactly original bank4 + verified base + signed header +7.
            const auto pixels=OriginalGroundRes4Audit::verified_tile_source(
                res4,out.source_entry);
            out.verified=true;
            out.verified_pixel_bytes=pixels.size()-2U;
        } catch(const std::exception& exc) {
            out.error=exc.what();
        }
        return out;
    }

    void print(std::ostream& out) const {
        out<<"RLOFT original RES4 ground candidates"
           <<" [native diagnostic, not DOS pixel acceptance]\n"
           <<"source_archive=RES4\n"
           <<"source_CODE2_base=INIT+0x0E -> DGROUP_258F8\n"
           <<"original_Level22_header_plus7_relative_selector="
           <<signed_level_selector<<"\n"
           <<"verified_INIT_base="<<source_base<<"\n"
           <<"source_selected_absolute_entry="<<source_entry<<"\n"
           <<"selected_entry_raw_bytes="<<selected_raw_bytes<<"\n"
           <<"selected_entry_first_13_bytes_hex="<<selected_prefix_hex<<"\n"
           <<"selected_decoded_64x64_verified="<<(verified?1:0)<<"\n"
           <<"selected_decoded_pixel_bytes="<<verified_pixel_bytes<<"\n"
           <<"selection_error="<<(error.empty()?"none":error)<<"\n"
           <<"entry_count="<<archive.total_entries<<"\n"
           <<"type1_count="<<archive.type1_entries<<"\n"
           <<"invalid_type1_count="<<archive.rejected_type1_entries<<"\n"
           <<"type6_count="<<archive.type6_entries<<"\n"
           <<"invalid_type6_count="<<archive.rejected_type6_entries<<"\n"
           <<"eligible_64x64_count="<<archive.tiles_64x64.size()<<"\n";
        for(const auto& item:archive.tiles_64x64) {
            out<<"candidate\t"<<item.absolute_entry
               <<"\tsource_EH_type="<<unsigned(item.original_eh_type)
               <<"\timplied_base_unverified="
               <<item.absolute_entry-signed_level_selector
               <<"\t"<<item.compressed_entry_bytes<<"\t"
               <<unsigned(item.compression)<<"\t"
               <<unsigned(item.transform)<<"\t"
               <<unsigned(item.transparent_index)
               <<"\tfnv1a32=0x"<<std::hex<<std::setfill('0')
               <<std::setw(8)<<item.decoded_fnv1a32<<std::dec
               <<"\tsource_selected="
               <<(source_entry==item.absolute_entry?1:0)<<"\n";
        }
        out<<"interpretation="<<(verified
            ? "source-selected original 64x64 texture decoded; DOS pixels not compared"
            : "source-selected original 64x64 texture NOT verified; no art substituted")
           <<"\n";
    }
};

} // namespace rloft
