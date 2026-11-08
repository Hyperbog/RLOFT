#include "extender_data_break.hpp"
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>
using rloft_recomp::ExtenderDataBreak;
static void need(bool b,const char* s){if(!b)throw std::runtime_error(s);}
int main() {
    try {
        std::vector<std::uint8_t> guest(0x9e90u,0x5au);
        ExtenderDataBreak brk(0x30u,guest.size());
        const auto selector=brk.resize(0x24u,10u,guest);
        need(selector.carry && selector.ax==9u && guest.size()==0x9e90u,
             "wrong ES selector accepted");
        const auto undersized=brk.resize(0x30u,9u,guest);
        need(undersized.carry && undersized.ax==8u &&
             guest.size()==0x9e90u,
             "extender shrank below linked CODE.1 static DGROUP");
        const auto enlarged=brk.resize(0x30u,10u,guest);
        need(!enlarged.carry && guest.size()==0xa000u,
             "4KiB count interpreted as 16-byte paragraphs");
        for(unsigned i=0;i<0x9e90u;++i)
            need(guest[i]==0x5au,"extender AH4A destroyed original bytes");
        for(unsigned i=0x9e90u;i<guest.size();++i)
            need(guest[i]==0u,"new DGROUP extension not zero initialized");
        guest[0x9e8fu]=0xc3u;
        need(!brk.resize(0x30u,16u,guest).carry &&
             guest.size()==0x10000u &&
             guest[0x9e8fu]==0xc3u,"second resize lost source data");
        const auto overflow=brk.resize(0x30u,0xffffu,guest);
        need(overflow.carry && overflow.ax==8u &&
             guest.size()==0x10000u &&
             guest[0x9e8fu]==0xc3u,
             "rejected extender growth not atomic");
        need(!brk.resize(0x30u,10u,guest).carry &&
             guest.size()==0xa000u && guest[0x9e8fu]==0xc3u,
             "valid shrink did not preserve static memory");
        need(brk.successful_resizes()==3u,"wrong accepted resize count");
        std::cout<<"ORIGINAL_PHARLAP_CODE1_AH4A_4KIB_RESIZE=PASS\n"
                    "ORIGINAL_STATIC_DGROUP_PRESERVED=PASS\n"
                    "DOS_AH4A_PARAGRAPHS_CONFUSED=NO\n"
                    "ORIGINAL_DOS_PARITY=NO\n";
        return 0;
    }catch(const std::exception& e){
        std::cerr<<"EXTENDER_DATA_BREAK_FAIL: "<<e.what()<<'\n';
        return 1;
    }
}
