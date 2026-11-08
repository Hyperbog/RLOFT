// Canonical CODE.1 source included unchanged: imported-original-image and isolated witnesses.
#include "../../Code1/Code1.cpp"
#include "original_le_image.h"
#include <vector>
extern "C" bool rloft_recomp_code1_memory_probe() {
    std::vector<code1::u8> data(code1::kDataObjectVirtualSize,0);
    code1::Machine state{};state.data=data.data();
    constexpr code1::u32 value=0x8A7031D2u;
    code1::store_le32(data.data()+0x2934u,value);
    return code1::function_7f50(state)==value;
}
extern "C" bool rloft_recomp_code1_original_data_probe(const rloft_recomp::OriginalLeImage& img) {
    if(img.objects.size()!=2 || img.objects[1].virtual_size!=code1::kDataObjectVirtualSize ||
       img.entry_offset!=code1::kEntryOffset)return false;
    std::vector<code1::u8> data=img.objects[1].raw;
    code1::Machine state{};state.data=data.data();
    return code1::function_7f50(state)==code1::load_le32(data.data()+0x2934u);
}


#include <array>

namespace {
std::array<code1::u8,512> b01a_data{};
bool b01a_oob=false;
unsigned b01a_single_writes=0,b01a_word_writes=0;
code1::u8 b01a_read8(code1::u32 at) noexcept {
    if(at>=b01a_data.size()){b01a_oob=true;return 0;}
    return b01a_data[at];
}
code1::u16 b01a_read16(code1::u32 at) noexcept {
    if(at+1>=b01a_data.size()){b01a_oob=true;return 0;}
    return static_cast<code1::u16>(b01a_data[at]) |
           static_cast<code1::u16>(b01a_data[at+1]<<8u);
}
void b01a_write8(code1::u32 at,code1::u8 value) noexcept {
    ++b01a_single_writes;
    if(at>=b01a_data.size()){b01a_oob=true;return;}
    b01a_data[at]=value;
}
void b01a_write16(code1::u32 at,code1::u16 value) noexcept {
    ++b01a_word_writes;
    if(at+1>=b01a_data.size()){b01a_oob=true;return;}
    b01a_data[at]=static_cast<code1::u8>(value);
    b01a_data[at+1]=static_cast<code1::u8>(value>>8u);
}
}
extern "C" bool rloft_recomp_code1_b01a_stosb_regression() {
    code1::LowGraphics13Boundary callbacks{};
    callbacks.read8=&b01a_read8;
    callbacks.read16=&b01a_read16;
    callbacks.write8=&b01a_write8;
    callbacks.write16=&b01a_write16;
    const auto reset=[](){
        b01a_data.fill(0xC5u);
        b01a_oob=false;
        b01a_single_writes=0;
        b01a_word_writes=0;
        b01a_data[16]='X';b01a_data[17]=0;
    };
    reset();
    b01a_data[128]='A';b01a_data[129]='B';
    b01a_data[130]=0;b01a_data[131]=0xFF;
    const bool even_ret=code1::function_b01a(callbacks,16u,128u)==16u;
    const bool even=even_ret && b01a_data[16]=='X' && b01a_data[17]=='A' &&
                    b01a_data[18]=='B' && b01a_data[19]==0 &&
                    b01a_single_writes==1 && b01a_word_writes==1 && !b01a_oob;
    reset();
    b01a_data[128]='A';b01a_data[129]='B';
    b01a_data[130]='C';b01a_data[131]=0;
    const bool odd_ret=code1::function_b01a(callbacks,16u,128u)==16u;
    const bool odd=odd_ret && b01a_data[17]=='A' && b01a_data[18]=='B' &&
                   b01a_data[19]=='C' && b01a_data[20]==0 &&
                   b01a_single_writes==0 && b01a_word_writes==2 && !b01a_oob;
    reset();
    b01a_data[128]=0;b01a_data[129]=0xEE;
    const bool empty_ret=code1::function_b01a(callbacks,16u,128u)==16u;
    const bool empty=empty_ret && b01a_data[17]==0 &&
                     b01a_single_writes==1 && b01a_word_writes==0 && !b01a_oob;
    return even && odd && empty;
}
