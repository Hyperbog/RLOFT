#include "original_le_image.h"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <vector>
extern "C" bool rloft_recomp_code1_memory_probe();
extern "C" bool rloft_recomp_code1_b01a_stosb_regression();
extern "C" bool rloft_recomp_code2_memory_probe();
extern "C" bool rloft_recomp_code2_entry_edge_probe();
extern "C" bool rloft_recomp_code1_original_data_probe(const rloft_recomp::OriginalLeImage&);
extern "C" bool rloft_recomp_code2_original_data_probe(const rloft_recomp::OriginalLeImage&);
namespace {
std::vector<std::uint8_t> original_file(const char* path) {
    std::ifstream file(path,std::ios::binary);
    if(!file)throw std::runtime_error("cannot read supplied original executable");
    return {std::istreambuf_iterator<char>(file),std::istreambuf_iterator<char>()};
}
}
int main(int argc,char**argv) {
    try {
        const bool code1=rloft_recomp_code1_memory_probe();
        const bool b01a=rloft_recomp_code1_b01a_stosb_regression();
        const bool code2=rloft_recomp_code2_memory_probe();
        const bool entry=rloft_recomp_code2_entry_edge_probe();
        std::cout << "CANONICAL_CODE1_CPP_COMPILE_LINK_MEMORY="<<(code1?"PASS":"FAIL")<<'\n';
        std::cout << "CODE1_ORIGINAL_B01A_OVERLAPPING_STOSB="<<(b01a?"PASS":"FAIL")<<'\n';
        std::cout << "CANONICAL_CODE2_CPP_COMPILE_LINK_MEMORY="<<(code2?"PASS":"FAIL")<<'\n';
        std::cout << "CODE2_ENTRY_353A0_TO_35418_BOUNDARY="<<(entry?"PASS":"FAIL")<<'\n';
        bool images=true;
        if(argc==3) {
            const auto input1=original_file(argv[1]);const auto input2=original_file(argv[2]);
            if(input1.size()!=277787u || input2.size()!=558689u)
                throw std::runtime_error("unrecognized original CODE file lengths");
            const auto img1=rloft_recomp::OriginalLeImage::parse(input1);
            const auto img2=rloft_recomp::OriginalLeImage::parse(input2);
            images=rloft_recomp_code1_original_data_probe(img1)&&
                   rloft_recomp_code2_original_data_probe(img2);
            std::cout << "CANONICAL_SOURCE_WITH_ORIGINAL_DGROUP="<<(images?"PASS":"FAIL")<<'\n';
        } else if(argc!=1)throw std::runtime_error("usage: rloft_recomp_link_gate [CODE.1 CODE.2]");
        else std::cout<<"CANONICAL_SOURCE_WITH_ORIGINAL_DGROUP=NOT_RUN_ASSETS_ABSENT\n";
        std::cout<<"ORIGINAL_DOS_STARTUP_EXECUTED=NO\nCOMPLETE_GAME_RECOMPILATION=NO\n";
        return (code1&&b01a&&code2&&entry&&images)?0:1;
    }catch(const std::exception& e){std::cerr<<"RECOMP_LINK_GATE_FAIL: "<<e.what()<<'\n';return 1;}
}
