#include "rloft_native/native_sintab_raster.hpp"

#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

using namespace rloft_native;
namespace {
void p16(std::vector<std::uint8_t>& b,std::size_t off,unsigned n) {
    b.at(off)=static_cast<std::uint8_t>(n);
    b.at(off+1)=static_cast<std::uint8_t>(n>>8U);
}
void p32(std::vector<std::uint8_t>& b,std::size_t off,unsigned n) {
    for(unsigned j=0;j<4;++j)
        b.at(off+j)=static_cast<std::uint8_t>(n>>(8U*j));
}
template<typename F> void rejects(F action) {
    bool failed=false;
    try{action();}catch(const std::exception&){failed=true;}
    assert(failed);
}
std::vector<std::uint8_t> code() {
    // Six source-verified opcodes; source column 128 indexed bytes
    // copied to two distinct original VGA 80-byte row addresses.
    return {0x8A,0x86,0,0,0,0, // mov al,[esi+0]
            0x88,0xC4,         // mov ah,al
            0x29,0xC8,         // sub eax,ecx
            0x88,0x87,0,0,0,0, // mov [edi+0],al
            0x88,0xA7,80,0,0,0,// mov [edi+80],ah
            0xC3};            // RET (never executed)
}
std::vector<std::uint8_t> synthetic_original() {
    const auto opcodes=code();
    const auto length=unsigned(0x204U+opcodes.size());
    std::vector<std::uint8_t> file(0x1F42,0);
    p32(file,0x1B38,127);
    p32(file,0x1B3C,508);
    p32(file,0x1B40,128);
    p32(file,0x1F3E,length*127U);
    for(unsigned scale=0;scale<127;++scale) {
        std::vector<std::uint8_t> block(length,0);
        for(unsigned i=0;i<129;++i)
            p16(block,i*2U,0x204U+(i==128?22U:0U));
        p16(block,0x102U+128U*2U,4U);
        for(unsigned i=0;i<opcodes.size();++i)
            block[0x204U+i]=opcodes[i];
        file.push_back(static_cast<std::uint8_t>(length));
        file.push_back(static_cast<std::uint8_t>(length>>8U));
        file.insert(file.end(),block.begin(),block.end());
    }
    file.resize(file.size()+512,0);
    return file;
}
void run() {
    const auto original=synthetic_original();
    const auto parsed=decode_native_sintab_programs(original);
    assert(parsed.blocks.size()==127);
    assert(parsed.original_sintab_sha256.size()==64);
    assert(parsed.total_payload_bytes==(0x204U+23U)*127U);
    assert(parsed.total_generated_code_bytes==23U*127U);
    assert(parsed.total_read_opcodes==127);
    assert(parsed.total_write_opcodes==254);
    assert(!parsed.x86_executed && !parsed.original_frame_parity_verified);
    assert(parsed.blocks[126].instruction_count==6);
    assert(parsed.blocks[0].summed_scale_steps==4);
    assert(parsed.blocks[0].patch_offsets[128]==0x204U+22U);
    assert(parsed.blocks[0].generated_code_sha256.size()==64);

    NativeType7SceneShape shape;
    shape.source_width=64;
    shape.x_min=0;shape.x_max=0;
    shape.columns.resize(1);
    shape.columns[0].descriptors.push_back({256,0,0});
    const auto plan=resolve_native_eh7_span(shape,parsed,126,0,0);
    assert(plan.entry_code_offset==0 && plan.stop_code_offset==22);
    assert(plan.summed_steps==4);
    assert(!plan.visual_frame_parity_verified);
    std::vector<std::uint8_t> body(128,0);
    body[0]=200;
    NativeIndexedVgaPlanes video;
    const auto initial_sha=video.planes_sha256();
    const auto report=draw_native_eh7_span(
        parsed,plan,body,0,1,0,0xA,video);
    assert(report.executed_instructions==5);
    assert(report.source_reads==1);
    assert(report.masked_destination_writes==2);
    assert(report.individual_plane_writes==4);
    assert(!report.x86_executed && !report.original_frame_parity_verified);
    assert(video.indexed_pixel(0,0)==0);
    assert(video.indexed_pixel(1,0)==199); // AL after wrapping DWORD subtraction
    assert(video.indexed_pixel(3,0)==199);
    assert(video.indexed_pixel(1,1)==200); // AH after DWORD subtraction
    assert(video.indexed_pixel(3,1)==200);
    assert(video.planes_sha256()!=initial_sha);
    rejects([&]{static_cast<void>(video.indexed_pixel(320,0));});
    rejects([&]{video.masked_write(0,16,1);});
    rejects([&]{static_cast<void>(resolve_native_eh7_span(
        shape,parsed,127,0,0));});
    rejects([&]{static_cast<void>(resolve_native_eh7_span(
        shape,parsed,0,1,0));});

    const auto before=video.planes_sha256();
    rejects([&]{static_cast<void>(draw_native_eh7_span(
        parsed,plan,body,129,1,0,0xF,video));});
    assert(video.planes_sha256()==before);
    rejects([&]{static_cast<void>(draw_native_eh7_span(
        parsed,plan,body,0,1,65534,0xF,video));});
    assert(video.planes_sha256()==before);
    rejects([&]{static_cast<void>(draw_native_eh7_span(
        parsed,plan,body,0,1,0,16,video));});
    assert(video.planes_sha256()==before);
    auto broken_plan=plan;broken_plan.stop_code_offset--;
    rejects([&]{static_cast<void>(draw_native_eh7_span(
        parsed,broken_plan,body,0,1,0,1,video));});
    auto broken=original;
    broken[0x1F42U+2U+0x204U]=0x90;
    rejects([&]{static_cast<void>(decode_native_sintab_programs(broken));});
    broken=original;
    broken[0x1F42U+2U+0x204U+22U]=0x90;
    rejects([&]{static_cast<void>(decode_native_sintab_programs(broken));});
    broken=original;
    p16(broken,0x1F42U+2U+4U,0x205U); // index 2 not opcode boundary
    rejects([&]{static_cast<void>(decode_native_sintab_programs(broken));});
    broken=original;
    broken.back()=1;
    rejects([&]{static_cast<void>(decode_native_sintab_programs(broken));});
    broken=original;
    p32(broken,0x1F3E,123U);
    rejects([&]{static_cast<void>(decode_native_sintab_programs(broken));});
    auto badshape=shape;badshape.columns[0].descriptors[0].stop_selector=255;
    rejects([&]{static_cast<void>(resolve_native_eh7_span(
        badshape,parsed,0,0,0));});
    badshape=shape;badshape.columns[0].descriptors[0].entry_selector=256;
    rejects([&]{static_cast<void>(resolve_native_eh7_span(
        badshape,parsed,0,0,0));});
}
}
int main(){run();}
