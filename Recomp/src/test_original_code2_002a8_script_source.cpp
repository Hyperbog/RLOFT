// Reconstructed CODE.2 002A8 source script interpreter regression.
// No original level IDs, retail media, mock guest mutation or host scripts.
#include "../../Code2/Code2.cpp"
#include "original_code2_script_preflight.hpp"
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {
using code2::u32;
code2::Machine* live=nullptr;
unsigned audio_calls=0u;
void demand(bool ok,const char* reason) {
    if(!ok)throw std::runtime_error(reason);
}
u32 world_record(u32 row,u32 col) noexcept {
    return code2::function_1e8c8(*live,row,col);
}
void set_bit(u32 bit) noexcept {
    code2::function_1c124(*live,bit);
}
void toggle_bit(u32 bit) noexcept {
    code2::function_1c1a0(*live,bit);
}
void source_sound(u32 id,u32 aux,u32 channel,u32 priority) noexcept {
    ++audio_calls;
    code2::function_28110(*live,id,aux,channel,priority);
}
}
int main() {
    try {
        std::vector<code2::u8> bytes(0x300000u,0u);
        code2::Machine m{};
        m.data=bytes.data();
        live=&m;
        code2::store_le16(bytes.data()+0x25619u,8u);
        code2::store_le16(bytes.data()+0x2561Bu,8u);
        const u32 script=0x250000u;
        code2::store_le32(bytes.data()+0xCF44u,script);
        code2::store_le32(bytes.data()+0xC038u,0u);
        bytes[script]=5u;
        // Five original five-byte commands: world tile, bits, source
        // state and audio request. No opcode is host reinterpreted.
        bytes[script+1u]=4u;
        bytes[script+2u]=2u;
        bytes[script+3u]=3u;
        bytes[script+4u]=0x0Bu;
        bytes[script+6u]=8u;
        bytes[script+9u]=0x3Du;
        bytes[script+11u]=10u;
        bytes[script+14u]=0x46u;
        bytes[script+16u]=12u;
        bytes[script+19u]=0x6Au;
        bytes[script+21u]=5u;
        bytes[script+24u]=0x23u;

        const auto admitted=rloft::source_script::validate(
            bytes.data(),bytes.size(),script,0);
        demand(admitted.accepted && admitted.commands==5u,
               "source script preflight rejected valid shared opcodes");
        code2::Dispatch002A8Boundary graph{};
        graph.call_1e8c8=&world_record;
        graph.call_1c124=&set_bit;
        graph.call_1c1a0=&toggle_bit;
        graph.call_28110=&source_sound;
        code2::function_002a8(m,graph,0);
        const u32 tile=0x198E0u+2u*(8u*2u+3u);
        demand((code2::load_le16(bytes.data()+tile)&0x3Fu)==0x0Bu,
               "original opcode4 tile mutation lost");
        demand(code2::function_1c1dc(m,0x3Du)!=0u &&
               code2::function_1c1dc(m,0x46u)!=0u,
               "original opcode8 or opcode10 guest event bit lost");
        demand(code2::load_le32(bytes.data()+0xBF44u)==0x6Au,
               "original opcode12 game state not committed");
        demand(audio_calls==1u,
               "original opcode5 sound request source callback missing");
        // Reject an unresolved opcode7 even if placed *after* an
        // earlier source mutation. A half-run is never authorized.
        bytes[script+1u]=12u;
        bytes[script+4u]=0x55u;
        bytes[script+21u]=7u;
        const auto excluded=rloft::source_script::validate(
            bytes.data(),bytes.size(),script,0);
        demand(!excluded.accepted &&
               code2::load_le32(bytes.data()+0xBF44u)==0x6Au,
               "source modal dialogue was not rejected before mutation");
        // Exhaust the original fixed 50-entry script descriptor index.
        for(std::int32_t index=1;index<50;++index) {
            const auto empty=rloft::source_script::validate(
                bytes.data(),bytes.size(),script,index);
            demand(empty.accepted && empty.commands==0u,
                   "valid original empty script entry refused");
            code2::function_002a8(m,graph,index);
        }
        demand(!rloft::source_script::validate(
            bytes.data(),bytes.size(),script,50).accepted,
            "out-of-table script index accepted");
        demand(rloft::source_script::validate(
            bytes.data(),bytes.size(),script,-1).accepted,
            "original no-script sentinel refused");
        std::cout<<"ORIGINAL_CODE2_SHARED_002A8_SCRIPT_SOURCE_PASS"
                 <<" shared_commands=5 original_descriptors=50"
                 <<" preexecution_invalid_modal_rejected=YES"
                 <<" guest_world_bits_audio=ORIGINAL"
                 <<" opcode7_dialogue=NOT_CONNECTED DOS_PARITY=NOT_CERTIFIED\n";
        return 0;
    }catch(const std::exception& e) {
        std::cerr<<"FAIL "<<e.what()<<"\n";return 1;
    }
}
