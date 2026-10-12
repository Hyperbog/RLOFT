#include "rloft_native/native_sintab_raster.hpp"
#include "rloft_native/native_sha256.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft_native {
namespace {
void require(bool ok,const char* message) {
    if(!ok)throw std::runtime_error(message);
}
std::uint16_t w16(const std::vector<std::uint8_t>& b,std::size_t at) {
    require(at<=b.size() && b.size()-at>=2,"SINTAB 16-bit source read overflow");
    return std::uint16_t(b[at]) | (std::uint16_t(b[at+1])<<8U);
}
std::uint32_t w32(const std::vector<std::uint8_t>& b,std::size_t at) {
    require(at<=b.size() && b.size()-at>=4,"SINTAB 32-bit source read overflow");
    return std::uint32_t(b[at]) | (std::uint32_t(b[at+1])<<8U) |
           (std::uint32_t(b[at+2])<<16U) | (std::uint32_t(b[at+3])<<24U);
}
std::int64_t signed32(std::uint32_t u) {
    return u<0x80000000U?std::int64_t(u):
           std::int64_t(u)-4294967296LL;
}
enum class Opcode { SourceRead, MovAhAl, SubEaxEcx, StoreAl, StoreAh, Ret };
struct Instruction {
    Opcode kind{};
    std::size_t length{};
    std::uint32_t displacement{};
};
Instruction opcode(const std::vector<std::uint8_t>& code,std::size_t pc) {
    require(pc<code.size(),"SINTAB opcode read outside generated code");
    if(code[pc]==0xC3U)return {Opcode::Ret,1U,0};
    require(code.size()-pc>=2U,"SINTAB opcode truncated");
    const auto a=code[pc],b=code[pc+1U];
    if(a==0x88U && b==0xC4U)return {Opcode::MovAhAl,2U,0};
    if(a==0x29U && b==0xC8U)return {Opcode::SubEaxEcx,2U,0};
    require((a==0x8AU && b==0x86U) ||
            (a==0x88U && (b==0x87U || b==0xA7U)),
            "unknown original SINTAB generated-code opcode");
    require(code.size()-pc>=6U,"SINTAB displacement truncated");
    const auto imm=w32(code,pc+2U);
    if(a==0x8AU) {
        require(signed32(imm)>=0 && signed32(imm)<128,
                "SINTAB source displacement exceeds 128-byte column");
        return {Opcode::SourceRead,6U,imm};
    }
    require(imm<=20400U && imm%80U==0U,
            "SINTAB VGA destination is not aligned to original row grid");
    return {b==0x87U?Opcode::StoreAl:Opcode::StoreAh,6U,imm};
}
std::size_t check_program(NativeSintabProgram& program) {
    const auto& code=program.original_x86_bytes;
    require(!code.empty() && code.back()==0xC3U,
            "original SINTAB block must terminate in x86 RET");
    std::vector<bool> boundary(code.size()+1U,false);
    std::size_t pc=0;
    boundary[0]=true;
    bool ended=false;
    while(pc<code.size()) {
        const auto op=opcode(code,pc);
        ++program.instruction_count;
        if(op.kind==Opcode::SourceRead)++program.source_reads;
        if(op.kind==Opcode::StoreAl || op.kind==Opcode::StoreAh)
            ++program.destination_writes;
        pc+=op.length;
        require(pc<=code.size(),"SINTAB opcode crosses generated-code boundary");
        boundary[pc]=true;
        if(op.kind==Opcode::Ret) {
            require(pc==code.size(),"SINTAB original code has interior RET");
            ended=true;
        }
    }
    require(ended,"original SINTAB code missing terminal RET");
    for(const auto physical:program.patch_offsets) {
        require(physical>=0x204U &&
                std::size_t(physical-0x204U)<code.size() &&
                boundary[physical-0x204U],
                "original SINTAB selector patch is not an instruction boundary");
    }
    return pc;
}
struct PlannedWrite {
    std::size_t at{};
    std::uint8_t color{};
};
} // namespace

NativeSintabPrograms decode_native_sintab_programs(
    const std::vector<std::uint8_t>& b) {
    require(b.size()>=0x1F42U+512U,
            "SINTAB original fixed prefix/trailer truncated");
    const auto n=w32(b,0x1B38U);
    const auto lookup_max=w32(b,0x1B3CU);
    const auto aux=w32(b,0x1B40U);
    require(n==127U && lookup_max==508U && aux==128U,
            "SINTAB original scale/lookup cardinality invalid");
    NativeSintabPrograms result;
    result.original_sintab_sha256=native_sha256_hex(b);
    for(unsigned i=0;i<509;++i)
        result.original_runtime_lookup[i]=w16(b,0x1B44U+i*2U);
    const auto declared=w32(b,0x1F3EU);
    std::size_t pos=0x1F42U;
    result.blocks.reserve(127);
    for(unsigned id=0;id<127;++id) {
        const auto length=std::size_t(w16(b,pos));
        pos+=2U;
        require(length>=0x205U && pos<=b.size() &&
                length<=b.size()-pos,
                "SINTAB original scale code body missing/truncated");
        NativeSintabProgram block;
        std::vector<std::uint8_t> whole(
            b.begin()+static_cast<std::ptrdiff_t>(pos),
            b.begin()+static_cast<std::ptrdiff_t>(pos+length));
        block.original_block_sha256=native_sha256_hex(whole);
        for(unsigned i=0;i<129;++i) {
            block.patch_offsets[i]=w16(b,pos+i*2U);
            block.scale_steps[i]=w16(b,pos+0x102U+i*2U);
            block.summed_scale_steps+=block.scale_steps[i];
            require(block.patch_offsets[i]>=0x204U &&
                    block.patch_offsets[i]<length &&
                    (i==0 || block.patch_offsets[i]>=block.patch_offsets[i-1]),
                    "original SINTAB code patch table offsets invalid");
        }
        require(block.patch_offsets[0]==0x204U,
                "original SINTAB initial code pointer not +0x204");
        block.original_x86_bytes.assign(
            b.begin()+static_cast<std::ptrdiff_t>(pos+0x204U),
            b.begin()+static_cast<std::ptrdiff_t>(pos+length));
        block.generated_code_sha256=native_sha256_hex(
            block.original_x86_bytes);
        check_program(block);
        result.total_payload_bytes+=length;
        result.total_generated_code_bytes+=block.original_x86_bytes.size();
        result.total_read_opcodes+=block.source_reads;
        result.total_write_opcodes+=block.destination_writes;
        result.blocks.push_back(std::move(block));
        pos+=length;
    }
    require(result.total_payload_bytes==declared,
            "SINTAB original declared payload byte sum differs");
    require(pos<=b.size() && b.size()-pos==512,
            "SINTAB original 512-byte trailer size invalid");
    for(;pos<b.size();++pos)
        require(b[pos]==0,"SINTAB original trailer nonzero");
    return result;
}
NativeEh7SpanPlan resolve_native_eh7_span(
    const NativeType7SceneShape& shape,
    const NativeSintabPrograms& tables,
    unsigned scale_index,unsigned column_index,unsigned descriptor_index) {
    require(scale_index<tables.blocks.size() &&
            column_index<shape.columns.size(),
            "source EH7 span scale/column outside decoded material");
    const auto& col=shape.columns[column_index];
    require(descriptor_index<col.descriptors.size(),
            "EH7 source span descriptor index outside column list");
    const auto& desc=col.descriptors[descriptor_index];
    require(desc.entry_selector<=0x100U &&
            desc.stop_selector<=0x100U &&
            (desc.entry_selector&1U)==0U &&
            (desc.stop_selector&1U)==0U &&
            desc.entry_selector<desc.stop_selector,
            "EH7 source code entry and stop selectors invalid");
    const auto& scale=tables.blocks[scale_index];
    require(!scale.original_x86_bytes.empty(),
            "original SINTAB source code is empty");
    const auto begin=std::size_t(scale.patch_offsets.at(desc.entry_selector/2U));
    const auto end=std::size_t(scale.patch_offsets.at(desc.stop_selector/2U));
    require(begin>=0x204U && end>=begin &&
            end-0x204U<scale.original_x86_bytes.size(),
            "EH7 source descriptor resolves outside generated-code boundaries");
    return {scale_index,column_index,descriptor_index,
            desc.signed_adjustment,desc.entry_selector,desc.stop_selector,
            begin-0x204U,end-0x204U,scale.summed_scale_steps,false};
}

NativeIndexedVgaPlanes::NativeIndexedVgaPlanes() {
    for(auto& plane:planes_)plane.assign(kPlaneSize,0);
}
void NativeIndexedVgaPlanes::masked_write(
    std::size_t address,std::uint8_t mask,std::uint8_t color) {
    require(address<kPlaneSize && mask<=15,
            "native VGA plane write outside four planar aperture");
    for(unsigned i=0;i<4;++i)
        if(mask & (1U<<i))planes_[i][address]=color;
}
std::uint8_t NativeIndexedVgaPlanes::indexed_pixel(
    unsigned x,unsigned y,unsigned page) const {
    require(x<320U && y<400U && page<=1U,
            "native VGA pixel coordinate/page invalid");
    const auto offset=std::size_t(page)*32768U+
                      std::size_t(y)*kRowBytes+std::size_t(x/4U);
    require(offset<kPlaneSize,"native VGA page coordinate out of bounds");
    return planes_[x&3U][offset];
}
std::string NativeIndexedVgaPlanes::planes_sha256() const {
    std::vector<std::uint8_t> joined;
    joined.reserve(kPlaneSize*4U);
    for(const auto& plane:planes_)
        joined.insert(joined.end(),plane.begin(),plane.end());
    return native_sha256_hex(joined);
}
NativeSafeRasterExecution draw_native_eh7_span(
    const NativeSintabPrograms& tables,const NativeEh7SpanPlan& plan,
    const std::vector<std::uint8_t>& original_body,
    std::int64_t source_base,std::uint32_t ecx,
    std::size_t destination,std::uint8_t mask,
    NativeIndexedVgaPlanes& output) {
    require(mask<=15U && destination<NativeIndexedVgaPlanes::kPlaneSize,
            "native SINTAB VGA destination or sequencer mask invalid");
    require(plan.scale_block<tables.blocks.size(),
            "native SINTAB span references absent scale");
    const auto& program=tables.blocks[plan.scale_block];
    require(plan.entry_selector<plan.stop_selector &&
            (plan.entry_selector&1U)==0U &&
            (plan.stop_selector&1U)==0U &&
            plan.stop_selector<=0x100U,
            "native SINTAB span invalid selector pair");
    const auto begin=std::size_t(program.patch_offsets[plan.entry_selector/2U]);
    const auto end=std::size_t(program.patch_offsets[plan.stop_selector/2U]);
    require(begin>=0x204U && end>=begin &&
            begin-0x204U==plan.entry_code_offset &&
            end-0x204U==plan.stop_code_offset &&
            plan.stop_code_offset<program.original_x86_bytes.size(),
            "native SINTAB span modified/unvalidated instruction boundaries");
    const auto& code=program.original_x86_bytes;
    std::vector<PlannedWrite> writes;
    NativeSafeRasterExecution result;
    std::uint32_t eax=0;
    std::size_t pc=plan.entry_code_offset;
    while(pc<plan.stop_code_offset) {
        require(result.executed_instructions<=code.size(),
                "native SINTAB span instruction bound exceeded");
        const auto instruction=opcode(code,pc);
        require(instruction.kind!=Opcode::Ret &&
                pc+instruction.length<=plan.stop_code_offset,
                "SINTAB span reaches RET or partial opcode before patch stop");
        ++result.executed_instructions;
        switch(instruction.kind) {
            case Opcode::SourceRead: {
                const auto offset=source_base+
                    std::int64_t(plan.source_adjustment)+
                    signed32(instruction.displacement);
                require(offset>=0 && std::uint64_t(offset)<original_body.size(),
                        "SINTAB span source read outside decoded EH7 body");
                eax=(eax&0xFFFFFF00U)|
                    original_body[static_cast<std::size_t>(offset)];
                ++result.source_reads;
                break;
            }
            case Opcode::MovAhAl:
                eax=(eax&0xFFFF00FFU)|((eax&0xFFU)<<8U);
                break;
            case Opcode::SubEaxEcx:
                eax-=ecx; // original DWORD subtraction, wrapping
                break;
            case Opcode::StoreAl:
            case Opcode::StoreAh: {
                const auto addr=destination+
                                std::size_t(instruction.displacement);
                require(addr>=destination &&
                        addr<NativeIndexedVgaPlanes::kPlaneSize,
                        "SINTAB span VGA store outside destination plane");
                writes.push_back({addr,static_cast<std::uint8_t>(
                    instruction.kind==Opcode::StoreAl?
                    (eax&0xFFU):((eax>>8U)&0xFFU))});
                ++result.masked_destination_writes;
                break;
            }
            case Opcode::Ret:
                throw std::runtime_error("unexpected generated-code RET");
        }
        pc+=instruction.length;
    }
    require(pc==plan.stop_code_offset,
            "native SINTAB span ended between original instruction boundaries");
    // Commit after *every* source read and VGA write has been validated.
    for(const auto& item:writes) {
        output.masked_write(item.at,mask,item.color);
        for(unsigned bit=0;bit<4;++bit)
            result.individual_plane_writes+=((mask>>bit)&1U);
    }
    return result;
}
} // namespace rloft_native
