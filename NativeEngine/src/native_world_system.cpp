#include "rloft_native/native_world_system.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace rloft_native {
namespace {
void require(bool check,const char* error) {
    if(!check) throw std::runtime_error(error);
}
std::uint16_t word(const std::vector<std::uint8_t>& data,std::size_t offset) {
    require(offset<=data.size() && data.size()-offset>=2,
            "original world table word beyond source");
    return std::uint16_t(data[offset]) |
           (std::uint16_t(data[offset+1])<<8U);
}
std::int16_t sword(const std::vector<std::uint8_t>& data,std::size_t offset) {
    return static_cast<std::int16_t>(word(data,offset));
}
std::uint32_t dword(const std::vector<std::uint8_t>& data,std::size_t offset) {
    require(offset<=data.size() && data.size()-offset>=4,
            "original world table dword beyond source");
    return std::uint32_t(data[offset]) |
           (std::uint32_t(data[offset+1])<<8U) |
           (std::uint32_t(data[offset+2])<<16U) |
           (std::uint32_t(data[offset+3])<<24U);
}
}

NativeLevelSystem decode_native_levels(const std::vector<std::uint8_t>& file) {
    require(file.size()>=4+32*4,
            "LEVELS.DAT 32-level offset table truncated");
    require(dword(file,0)==32,"LEVELS.DAT wrong original level count");
    require(dword(file,4)==4+32*4,
            "LEVELS.DAT first record does not begin after directory");
    NativeLevelSystem result;
    result.archive_bytes=file.size();
    for(unsigned id=0;id<32;++id) {
        const auto start=std::size_t(dword(file,4+id*4));
        const auto end=id==31?file.size():
            std::size_t(dword(file,4+(id+1)*4));
        require(start<end && end<=file.size() &&
                end-start>=0x1D+0x2A4+4+4,
                "LEVELS.DAT record offsets overlap or extend beyond EOF");

        std::vector<std::uint8_t> record(
            file.begin()+static_cast<std::ptrdiff_t>(start),
            file.begin()+static_cast<std::ptrdiff_t>(end));
        std::array<std::size_t,7> boundary{};
        for(unsigned n=0;n<7;++n)
            boundary[n]=std::size_t(dword(record,1+4*n));
        require(boundary[0]==0x1D &&
                boundary[0]+0x2A4<record.size(),
                "LEVELS.DAT record preamble or 0x2A4 header inconsistent");
        for(unsigned n=1;n<7;++n)
            require(boundary[n]>=boundary[n-1] &&
                    boundary[n]<=record.size(),
                    "LEVELS.DAT section boundary ordering invalid");

        const auto w=sword(record,boundary[0]+1);
        const auto h=sword(record,boundary[0]+3);
        require(w>0 && h>0 && w<=512 && h<=512,
                "LEVELS.DAT grid dimensions invalid");
        const auto cells=std::size_t(w)*std::size_t(h);
        require(boundary[1]==boundary[0]+0x2A4+cells*2,
                "LEVELS.DAT 16-bit packed cell block length wrong");

        NativeLevelSource level;
        level.id=id;level.width=static_cast<unsigned>(w);
        level.height=static_cast<unsigned>(h);
        level.packed_cells=cells;

        auto cursor=boundary[1];
        for(int col=0;col<w;++col) {
            require(cursor+2<=boundary[2],
                    "LEVELS.DAT 10-byte list count extends beyond section");
            const auto count=sword(record,cursor);
            require(count>=0,"LEVELS.DAT has negative control count");
            cursor+=2;
            require(std::size_t(count)<=(boundary[2]-cursor)/10,
                    "LEVELS.DAT 10-byte list truncated");
            level.control_records+=static_cast<std::size_t>(count);
            cursor+=std::size_t(count)*10;
        }
        require(cursor==boundary[2],
                "LEVELS.DAT control list section not fully consumed");

        const auto countA=sword(record,boundary[0]+0x28A);
        const auto countC=sword(record,boundary[0]+0x28C);
        const auto countB=sword(record,boundary[0]+0x28E);
        require(countA>=0 && countC>=0 && countB>=0,
                "LEVELS.DAT negative compact world family count");
        level.family_a=static_cast<std::size_t>(countA);
        level.family_b=static_cast<std::size_t>(countB);
        level.family_c=static_cast<std::size_t>(countC);
        require(boundary[3]-boundary[2]==level.family_a*11 &&
                boundary[4]-boundary[3]==level.family_b*11 &&
                boundary[5]-boundary[4]==level.family_c*11,
                "LEVELS.DAT compact 11-byte world family lengths inconsistent");
        require(boundary[6]-boundary[5]==50U*26U,
                "LEVELS.DAT event table must contain fifty 26-byte slots");

        for(unsigned slot=0;slot<50;++slot) {
            const auto at=boundary[5]+slot*26;
            const auto commands=record[at];
            require(commands<=5,
                    "LEVELS.DAT event slot exceeds five original commands");
            if(commands)++level.event_slots_active;
            level.event_commands+=commands;
            for(unsigned cmd=0;cmd<commands;++cmd)
                require(record[at+1+cmd*5]<=12,
                        "LEVELS.DAT event command opcode exceeds CODE.2 dispatcher");
        }

        cursor=boundary[6];
        require(sword(record,cursor)==w && sword(record,cursor+2)==h,
                "LEVELS.DAT repeated automap dimensions differ");
        cursor+=4;
        require(cursor<=record.size() &&
                cells<=record.size()-cursor &&
                record.size()-cursor-cells>=4,
                "LEVELS.DAT automap byte grid truncated");
        cursor+=cells;
        const auto annotation=dword(record,cursor);
        cursor+=4;
        require(annotation<=0x7FE && annotation==record.size()-cursor,
                "LEVELS.DAT annotation block exceeds or mismatches original tail");
        level.annotation_bytes=annotation;
        result.levels.push_back(level);
        result.map_cells+=level.packed_cells;
        result.control_records+=level.control_records;
        result.active_event_slots+=level.event_slots_active;
        result.active_event_commands+=level.event_commands;
    }
    return result;
}

NativeSintabSystem decode_native_sintab(const std::vector<std::uint8_t>& file) {
    // Fixed source tables:
    // +0000 450 x sine_q16, +0708 900 x tangent_q16
    // +1518/+1618 two 128-word projection tables
    // +1718 eight renderer constants, +1738 256 signed view angles
    // +1B38 count/max/aux, +1B44 runtime lookup + 4 byte arena sum
    require(file.size()>=0x1F42+512,
            "original SINTAB static projection table truncated");
    NativeSintabSystem result;
    result.file_bytes=file.size();
    result.scale_count=dword(file,0x1B38);
    result.lookup_max=dword(file,0x1B3C);
    result.lookup_aux=dword(file,0x1B40);
    require(result.scale_count==127 && result.lookup_max==508 &&
            result.lookup_aux==128,
            "original SINTAB renderer table count/lookup shape unexpected");

    const auto lookup_start=std::size_t(0x1B44);
    const auto size_offset=lookup_start+
        2U*(std::size_t(result.lookup_max)+1U);
    result.declared_scale_bytes=dword(file,size_offset);
    auto cursor=size_offset+4;
    for(unsigned index=0;index<result.scale_count;++index) {
        const auto length=std::size_t(word(file,cursor));
        cursor+=2;
        require(length>=0x205 && cursor<=file.size() &&
                length<=file.size()-cursor,
                "SINTAB generated rasterizer block length invalid");
        NativeScaleBlock scale;
        scale.payload_bytes=static_cast<std::uint16_t>(length);
        unsigned last_code=0;
        for(unsigned selector=0;selector<129;++selector) {
            const auto code_offset=unsigned(word(file,cursor+selector*2U));
            require(code_offset>=0x204 && code_offset<length &&
                    (selector==0 || code_offset>=last_code),
                    "SINTAB 129-entry microcode offset table not monotonic");
            if(selector==0)scale.initial_code_offset=
                static_cast<std::uint16_t>(code_offset);
            if(selector==128)scale.final_code_offset=
                static_cast<std::uint16_t>(code_offset);
            last_code=code_offset;
            scale.summed_step+=word(file,cursor+0x102U+selector*2U);
        }
        require(scale.initial_code_offset==0x204,
                "SINTAB microcode table has invalid initial raster offset");
        require(file[cursor+length-1]==0xC3,
                "SINTAB generated scale code missing final x86 RET");
        result.scales.push_back(scale);
        require(result.actual_scale_bytes<=
                std::numeric_limits<std::uint32_t>::max()-length,
                "SINTAB scale block sum overflow");
        result.actual_scale_bytes+=static_cast<std::uint32_t>(length);
        cursor+=length;
    }
    require(result.actual_scale_bytes==result.declared_scale_bytes,
            "SINTAB scale code aggregate size differs");
    require(cursor<=file.size() && file.size()-cursor==512,
            "SINTAB renderer package must preserve 512-byte trailing reserve");
    for(std::size_t pos=cursor;pos<file.size();++pos)
        require(file[pos]==0,"SINTAB nonzero reserved trailer data");
    return result;
}

NativeWorldSystem load_native_world_system(const NativeMedia& media) {
    return {decode_native_levels(media.read("LEVELS.DAT")),
            decode_native_sintab(media.read("SINTAB"))};
}

std::string native_world_system_json(const NativeWorldSystem& system) {
    std::ostringstream out;
    out<<"{\"schema\":\"rloft-native-world-system/v1\","
       <<"\"level_count\":"<<system.world.levels.size()
       <<",\"level_cells\":"<<system.world.map_cells
       <<",\"controls\":"<<system.world.control_records
       <<",\"event_slots\":"<<system.world.active_event_slots
       <<",\"event_commands\":"<<system.world.active_event_commands
       <<",\"level_archive_bytes\":"<<system.world.archive_bytes
       <<",\"sintab_bytes\":"<<system.renderer_tables.file_bytes
       <<",\"sintab_scales\":"<<system.renderer_tables.scale_count
       <<",\"sintab_scale_bytes\":"
       <<system.renderer_tables.actual_scale_bytes
       <<",\"levels\":[";
    for(std::size_t i=0;i<system.world.levels.size();++i) {
        const auto& l=system.world.levels[i];
        if(i)out<<",";
        out<<"{\"id\":"<<l.id<<",\"w\":"<<l.width
           <<",\"h\":"<<l.height
           <<",\"cells\":"<<l.packed_cells
           <<",\"controls\":"<<l.control_records
           <<",\"a\":"<<l.family_a
           <<",\"b\":"<<l.family_b
           <<",\"c\":"<<l.family_c
           <<",\"events\":"<<l.event_slots_active
           <<",\"commands\":"<<l.event_commands<<"}";
    }
    out<<"]}\n";
    return out.str();
}

} // namespace rloft_native
