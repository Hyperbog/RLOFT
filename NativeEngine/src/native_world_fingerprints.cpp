#include "rloft_native/native_world_fingerprints.hpp"
#include "rloft_native/native_sha256.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace rloft_native {
namespace {
void require(bool ok,const char* error) {
    if(!ok)throw std::runtime_error(error);
}
std::uint32_t read32(const std::vector<std::uint8_t>& in,std::size_t pos) {
    require(pos<=in.size() && in.size()-pos>=4,
            "original world fingerprint offset extends beyond source");
    return std::uint32_t(in[pos])|(std::uint32_t(in[pos+1])<<8U)|
           (std::uint32_t(in[pos+2])<<16U)|(std::uint32_t(in[pos+3])<<24U);
}
void write16(std::vector<std::uint8_t>& data,std::size_t number) {
    require(number<=65535U,"native world control count exceeds 16-bit range");
    data.push_back(static_cast<std::uint8_t>(number));
    data.push_back(static_cast<std::uint8_t>(number>>8U));
}
void assert_same(const std::vector<std::uint8_t>& original,std::size_t start,
                 std::size_t end,const std::vector<std::uint8_t>& encoded) {
    require(start<=end && end<=original.size() &&
            end-start==encoded.size() &&
            std::equal(encoded.begin(),encoded.end(),
                       original.begin()+static_cast<std::ptrdiff_t>(start)),
            "decoded native world payload differs from original section bytes");
}
template<std::size_t N>
void append(std::vector<std::uint8_t>& output,
            const std::array<std::uint8_t,N>& record) {
    output.insert(output.end(),record.begin(),record.end());
}
}

std::string native_world_payload_fingerprints_json(
    const NativeWorldPayload& decoded,
    const std::vector<std::uint8_t>& original) {
    require(decoded.levels.size()==32 && read32(original,0)==32,
            "native original payload fingerprint needs 32 complete levels");
    std::ostringstream out;
    out<<"{\"schema\":\"rloft-native-world-payload-fingerprints/v1\","
       <<"\"input_levels_sha256\":\""<<native_sha256_hex(original)<<"\","
       <<"\"input_levels_bytes\":"<<original.size()
       <<",\"decoded_section_roundtrip_equal\":true,"
       <<"\"original_file_identity_independently_attested\":false,"
       <<"\"original_dos_execution_observed\":false,"
       <<"\"native_gameplay_parity_verified\":false,"
       <<"\"levels\":[";
    std::size_t cell_total=0,control_total=0,command_total=0;
    for(unsigned id=0;id<32;++id) {
        const auto& level=decoded.levels[id];
        require(level.id==id && level.width>0 && level.height>0 &&
                level.cell_words.size()==std::size_t(level.width)*level.height &&
                level.automap.size()==level.cell_words.size(),
                "native world payload level dimensions or cells inconsistent");
        const auto start=std::size_t(read32(original,4U+id*4U));
        const auto end=id==31?original.size():
            std::size_t(read32(original,8U+id*4U));
        require(start<end && end<=original.size(),
                "native source level record span inconsistent");
        std::vector<std::uint8_t> record(
            original.begin()+static_cast<std::ptrdiff_t>(start),
            original.begin()+static_cast<std::ptrdiff_t>(end));
        std::array<std::size_t,7> boundary{};
        for(unsigned k=0;k<7;++k)boundary[k]=read32(record,1U+4U*k);
        const std::vector<std::uint8_t> retained_header(
            level.original_header.begin(),level.original_header.end());
        assert_same(record,boundary[0],boundary[0]+0x2A4,retained_header);

        std::vector<std::uint8_t> cells;
        cells.reserve(level.cell_words.size()*2U);
        for(const auto word:level.cell_words)write16(cells,word);
        assert_same(record,boundary[0]+0x2A4,boundary[1],cells);

        std::vector<std::uint8_t> controls;
        std::size_t next=0;
        for(unsigned column=0;column<level.width;++column) {
            const auto begin=next;
            while(next<level.controls.size() &&
                  level.controls[next].column==column)++next;
            write16(controls,next-begin);
            for(std::size_t i=begin;i<next;++i)append(controls,level.controls[i].bytes);
        }
        require(next==level.controls.size(),
                "native world controls are not column-sorted");
        assert_same(record,boundary[1],boundary[2],controls);

        std::array<std::string,3> family_digest{};
        for(std::size_t family=0;family<3;++family) {
            std::vector<std::uint8_t> bytes;
            bytes.reserve(level.families[family].size()*11U);
            for(const auto& item:level.families[family])append(bytes,item);
            assert_same(record,boundary[family+2],boundary[family+3],bytes);
            family_digest[family]=native_sha256_hex(bytes);
        }

        std::vector<std::uint8_t> events;
        events.reserve(50U*26U);
        std::size_t commands=0;
        for(const auto& slot:level.event_slots) {
            append(events,slot);
            require(slot[0]<=5,"native event slot has invalid command count");
            commands+=slot[0];
        }
        assert_same(record,boundary[5],boundary[6],events);

        const auto automap_start=boundary[6]+4U;
        assert_same(record,automap_start,
                    automap_start+level.automap.size(),level.automap);
        const auto annotation_start=automap_start+level.automap.size()+4U;
        assert_same(record,annotation_start,record.size(),level.annotation);

        if(id)out<<",";
        out<<"{\"id\":"<<id
           <<",\"cells\":"<<level.cell_words.size()
           <<",\"controls\":"<<level.controls.size()
           <<",\"commands\":"<<commands
           <<",\"header_sha256\":\""<<native_sha256_hex(retained_header)
           <<",\"cells_sha256\":\""<<native_sha256_hex(cells)
           <<"\",\"controls_sha256\":\""<<native_sha256_hex(controls)
           <<"\",\"events_sha256\":\""<<native_sha256_hex(events)
           <<"\",\"automap_sha256\":\""<<native_sha256_hex(level.automap)
           <<"\",\"annotations_sha256\":\""<<native_sha256_hex(level.annotation)
           <<"\",\"family_a_sha256\":\""<<family_digest[0]
           <<"\",\"family_b_sha256\":\""<<family_digest[1]
           <<"\",\"family_c_sha256\":\""<<family_digest[2]<<"\"}";
        cell_total+=level.cell_words.size();
        control_total+=level.controls.size();
        command_total+=commands;
    }
    require(cell_total==decoded.total_cells &&
            control_total==decoded.total_control_records &&
            command_total==decoded.total_event_commands,
            "native payload totals disagree with original parser");
    out<<"],\"total_cells\":"<<cell_total
       <<",\"total_controls\":"<<control_total
       <<",\"total_commands\":"<<command_total<<"}\n";
    return out.str();
}
} // namespace rloft_native
