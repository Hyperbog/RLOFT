// Standalone source-data gate for the original CODE.2 scene-palette resolver.
// Retail media is supplied read-only at test time; no pixels/assets are output.
#include "../src/original_code2_scene_palette.hpp"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
void require(bool yes,const char* why) {
    if(!yes)throw std::runtime_error(why);
}
std::vector<std::uint8_t> read(const char* path) {
    std::ifstream f(path,std::ios::binary);
    if(!f)throw std::runtime_error("Missing owner-provided original media");
    return {std::istreambuf_iterator<char>(f),std::istreambuf_iterator<char>()};
}
}

int main(int argc,char** argv) {
    try {
        // No media fixture needed to verify dynamic selection, bounds and
        // fail-closed rejection of invalid descriptors.
        std::vector<std::uint8_t> synthetic(0x81ACu+32u*3u+4u,0u);
        synthetic[0x81ACu+5u*3u]=3u;
        synthetic[0x81ACu+9u*3u]=7u;
        require(rloft::original_scene_palette::select_bank(synthetic,5u)==3u,
                "synthetic Level 5 selection failed");
        require(rloft::original_scene_palette::select_bank(synthetic,9u)==7u,
                "selector is not data-driven");
        synthetic[0x81ACu+5u*3u]=0u;
        require(rloft::original_scene_palette::select_bank(synthetic,5u)==0u,
                "selector hardcodes bank 3");
        synthetic[0x81ACu+5u*3u]=15u;
        bool rejected=false;
        try{(void)rloft::original_scene_palette::select_bank(synthetic,5u);}
        catch(const std::runtime_error&){rejected=true;}
        require(rejected,"out-of-range bank accepted");
        rejected=false;
        try{(void)rloft::original_scene_palette::select_bank(synthetic,32u);}
        catch(const std::out_of_range&){rejected=true;}
        require(rejected,"out-of-range level accepted");
        rejected=false;
        try{std::vector<std::uint8_t> tiny(32u);
            (void)rloft::original_scene_palette::select_bank(tiny,5u);}
        catch(const std::out_of_range&){rejected=true;}
        require(rejected,"truncated descriptor table accepted");
        if (argc==3) {
            const auto code2=read(argv[1]),colors=read(argv[2]);
            require(code2.size()==558689u,"Unexpected original CODE.2 size");
            require(colors.size()==11521u && colors[0]==15u,
                    "Unexpected COLORS header/size");
            constexpr std::size_t obj2=0x75F24u;
            std::vector<std::uint8_t> code2_data(
                code2.begin()+obj2,code2.end());
            const auto selected=rloft::original_scene_palette::select_bank(
                code2_data,5u);
            require(selected==3u,
                    "Original Level-5 palette index changed");
            const auto b0=colors.begin()+1u,b3=colors.begin()+1u+selected*768u;
            unsigned changed_indices=0u,channels=0u;
            for(unsigned i=0;i<256u;++i) {
                bool changed=false;
                for(unsigned j=0;j<3u;++j) {
                    if(b0[i*3u+j]!=b3[i*3u+j]) {
                        ++channels;changed=true;
                    }
                }
                if(changed){
                    require(i>=246u && i<=253u,
                            "Retail colors differ beyond 246-253");
                    ++changed_indices;
                }
            }
            require(changed_indices==8u && channels==24u,
                    "Original COLORS 0->3 transition changed");
            std::cout<<"AUTHENTIC_CODE2_SCENE_COLORS_SELECTION_PASS"
                     <<" level=5 bank=3 changed_indices=8 channels=24"
                     <<" native_source_palette=FULL_BANK"<<"\n";
        } else {
            require(argc==1,"usage: scene_palette_selection [CODE.2 COLORS]");
            std::cout<<"SYNTHETIC_SCENE_COLORS_SELECTION_PASS"
                     <<" independent_level_values=YES invalid_bank_rejected=YES"
                     <<" invalid_level_rejected=YES truncated_table_rejected=YES"
                     <<"\n";
        }
        return 0;
    }catch(const std::exception& e){
        std::cerr<<"SCENE_COLORS_SELECTION_FAIL: "<<e.what()<<"\n";
        return 1;
    }
}
