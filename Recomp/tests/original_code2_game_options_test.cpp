// Retail CODE.2 5E38 GAME.OPT original object-1 branch regression.
// Tests absent-file return AND present-file exact 14-byte DOS read,
// actor byte, palette bank, DAC and audio callback sequence.
#include "../../Code2/Code2.cpp"
#include "../src/original_code2_5e38_game_options.hpp"
#include <array>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <vector>
namespace opts=rloft::original_code2_options;
using u32=std::uint32_t;
namespace {
std::vector<std::uint8_t>* mem=nullptr;
bool present=false;
std::array<std::uint8_t,14> payload{};
unsigned opens=0,reads=0,closes=0,palettes=0,dacs=0,volumes=0,flags=0;
std::int32_t open(u32 path) {
    ++opens;
    if(path!=0x374u||std::memcmp(mem->data()+path,"GAME.OPT",9u)!=0)
        throw std::runtime_error("Retail GAME.OPT pathname changed");
    return present?0x50:-1;
}
void read(u32 dst,u32 size,u32 h) {
    if(dst!=0xC025u||size!=14u||h!=0x50u)
        throw std::runtime_error("GAME.OPT original read ABI changed");
    ++reads;
    std::memcpy(mem->data()+dst,payload.data(),payload.size());
}
void close(u32 h) {if(h!=0x50u)throw std::runtime_error("DOS close");++closes;}
void palette(u32 dst,u32 first,u32 bank,u32 handle,u32 count) {
    if(dst!=0xC054u||first!=0u||bank!=3u||handle!=0x390u||count!=256u)
        throw std::runtime_error("Original GAME.OPT palette ABI");
    ++palettes;
}
void dac(u32 dst) {if(dst!=0xC054u)throw std::runtime_error("DAC");++dacs;}
void volume(u32 value) {
    if(value!=payload[6])throw std::runtime_error("Original volume byte");
    ++volumes;
}
void sound(u32 value) {
    if(value!=payload[7])throw std::runtime_error("Original sound-enabled byte");
    ++flags;
}
void require(bool ok,const char* error) {if(!ok)throw std::runtime_error(error);}
}
int main() {
try {
    std::vector<std::uint8_t> data(0x30000u);
    mem=&data;
    code2::Machine m{};
    m.data=data.data();
    std::memcpy(data.data()+0x374u,"GAME.OPT",9u);
    code2::store_le32(data.data()+0x6138u,0x374u);
    code2::store_le32(data.data()+0xF050u,0x28000u);
    code2::store_le32(data.data()+0xC034u,3u);
    for(unsigned i=0;i<payload.size();++i)payload[i]=std::uint8_t(i+2u);
    opts::Boundary b{};
    b.open_read=&open;
    b.read_guest=&read;
    b.close=&close;
    b.palette_bank=&palette;
    b.palette_dac=&dac;
    b.volume_28098=&volume;
    b.sound_enable_290a8=&sound;
    const auto absent=opts::function_5e38(m,b);
    require(!absent.found&&!absent.applied&&opens==1u &&
           reads==0u&&closes==0u&&palettes==0u &&
           volumes==0u&&flags==0u,
           "Original 5E38 GAME.OPT missing branch changed");
    present=true;
    const auto found=opts::function_5e38(m,b);
    require(found.found&&found.applied&&found.handle==0x50u &&
           opens==2u&&reads==1u&&closes==1u&&palettes==1u &&
           dacs==1u&&volumes==1u&&flags==1u &&
           data[0x28000u+0x6Cu]==payload[9] &&
           std::memcmp(data.data()+0xC025u,payload.data(),14u)==0,
           "Original 5E38 GAME.OPT present branch changed");
    std::cout<<"ORIGINAL_CODE2_5E38_GAME_OPT_BRANCHES_PASS"
             <<" source_start=5E38"
             <<" missing_game_opt=RETURN_UNMODIFIED"
             <<" present_game_opt=READ14_TO_C025"
             <<" palette_source=325B0_29234"
             <<" sound_values=28098_290A8"
             <<" next_gameplay=NOT_REACHED\n";
    return 0;
}catch(const std::exception& e) {
    std::cerr<<"ORIGINAL_CODE2_5E38_GAME_OPT_TEST_FAIL: "<<e.what()<<'\n';
    return 1;
}
}
