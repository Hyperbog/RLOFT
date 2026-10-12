#include "rloft_native/native_code2_projection.hpp"
#include "rloft_native/native_media.hpp"
#include "rloft_native/native_sha256.hpp"

#include <cstddef>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
void add16(std::vector<std::uint8_t>& b,std::uint16_t x){
    b.push_back(static_cast<std::uint8_t>(x));
    b.push_back(static_cast<std::uint8_t>(x>>8U));
}
void add32(std::vector<std::uint8_t>& b,std::uint32_t x){
    for(unsigned j=0;j<4;++j)
        b.push_back(static_cast<std::uint8_t>(x>>(8U*j)));
}
std::string span_sha(
    const std::vector<std::uint8_t>& file,std::size_t begin,
    std::size_t end){
    constexpr std::size_t offset=0x3AF24U;
    if(begin>=end || end>0x3A852U ||
       offset+end>file.size())
        throw std::runtime_error("original CODE.2 object projection segment truncated");
    return rloft_native::native_sha256_hex(std::vector<std::uint8_t>(
        file.begin()+static_cast<std::ptrdiff_t>(offset+begin),
        file.begin()+static_cast<std::ptrdiff_t>(offset+end)));
}
int run(const std::filesystem::path& original_install){
    try{
        const auto media=rloft_native::NativeMedia::open(original_install);
        const auto code=media.read("CODE.2");
        const auto mul=span_sha(code,0x35680U,0x356d8U);
        const auto height=span_sha(code,0x35730U,0x35797U);
        const auto point=span_sha(code,0x35798U,0x35869U);
        if(mul!="4a0de259caeeb0ea8a47809aefce47c363bf37e956578ad0bb402399cd353df6" ||
           height!="9c37f50641e5c02afc0d68f0c070ca2b38874be1a600d3eace267c9c3eeb9e60" ||
           point!="af99ef81b74f6d16cc3cc666717ea76fad4188393dc951ea68bfb01c523550b3")
            throw std::runtime_error("original CODE.2 35680/35730/35798 source bytes changed");
        constexpr char label[]="RLOFT-CODE2-projection-35680-35730-35798-v1";
        std::vector<std::uint8_t> record(label,label+sizeof(label)-1U);
        std::uint32_t seed=0xC2A35730U;
        std::size_t accepted=0,rejected=0,clamped=0;
        std::size_t source_byte_events=0;
        std::cout<<"{\"schema\":\"rloft-code2-projection-original-arithmetic/v1\","
                 <<"\"original_code2_sha256\":\""
                 <<rloft_native::native_sha256_hex(code)<<"\","
                 <<"\"original_helper_35680_sha256\":\""<<mul<<"\","
                 <<"\"original_height_35730_sha256\":\""<<height<<"\","
                 <<"\"original_point_35798_sha256\":\""<<point<<"\","
                 <<"\"deterministic_cases\":4096,"
                 <<"\"input_seed\":3265484592,"
                 <<"\"first_samples\":[";
        for(unsigned i=0;i<4096;++i){
            auto next=[&](){
                seed=1664525U*seed+1013904223U;
                return seed;
            };
            rloft_native::NativeCode2ProjectionRegisters s;
            s.origin_x_255ec=next()&0x1ffffU;
            s.origin_y_255f4=next()&0x1ffffU;
            s.live_x_255f0=s.origin_x_255ec+(next()&0x7ffffU);
            s.live_y_255e8=s.origin_y_255f4+(next()&0x7ffffU);
            s.direction_x_255f8=next()&0x7fffU;
            s.direction_y_255fc=next()&0x7fffU;
            s.height_numerator_255e0=0x10000U+(next()&0x7ffffU);
            s.horizontal_scale_c6cc=256U+(next()&0x3ffU);
            s.horizontal_origin_c6d0=static_cast<std::uint16_t>(next());
            const auto w=rloft_native::project_native_code2_wall_height_35730(s);
            const auto x=s.origin_x_255ec+(next()&0x7ffffU);
            const auto y=s.origin_y_255f4+(next()&0x7ffffU);
            const auto p=rloft_native::project_native_code2_point_35798(
                s,x,y,1234U);
            accepted+=p.accepted;
            rejected+=!p.accepted;
            clamped+=w.clamped_to_original_near_limit;
            add32(record,w.source_depth);
            add32(record,w.clamped_depth);
            add32(record,w.unsigned_divisor);
            add16(record,w.original_return_word);
            add32(record,p.source_depth);
            add32(record,p.transverse);
            add16(record,p.original_screen_x);
            add16(record,p.original_projected_height);
            record.push_back(static_cast<std::uint8_t>(p.accepted));
            record.push_back(static_cast<std::uint8_t>(
                w.clamped_to_original_near_limit));
            source_byte_events+=28U;
            if(i<8){
                if(i)std::cout<<",";
                std::cout<<"{\"case\":"<<i
                         <<",\"wall_depth\":"<<w.source_depth
                         <<",\"wall_height\":"<<w.original_return_word
                         <<",\"near_clamped\":"
                         <<(w.clamped_to_original_near_limit?"true":"false")
                         <<",\"point_depth\":"<<p.source_depth
                         <<",\"point_transverse\":"<<p.transverse
                         <<",\"accepted\":"<<(p.accepted?"true":"false")
                         <<",\"screen_word\":"<<p.original_screen_x
                         <<",\"height_word\":"<<p.original_projected_height
                         <<"}";
            }
        }
        std::cout<<"],\"accepted_projected_points\":"<<accepted
                 <<",\"near_rejected_points\":"<<rejected
                 <<",\"wall_near_clamped_cases\":"<<clamped
                 <<",\"encoded_case_bytes\":"<<source_byte_events
                 <<",\"original_algorithm_case_digest\":\""
                 <<rloft_native::native_sha256_hex(record)<<"\","
                 <<"\"original_fixed_point_arithmetic_implemented\":true,"
                 <<"\"original_live_camera_state_verified\":false,"
                 <<"\"original_ray_traversal_verified\":false,"
                 <<"\"original_full_frame_parity_verified\":false}\n";
        return 0;
    }catch(const std::exception& e){
        std::cerr<<"CODE.2 projection original-byte audit FAILED: "
                 <<e.what()<<"\n";
        return 3;
    }
}
}
#ifdef _WIN32
int wmain(int argc,wchar_t** argv){
    return argc==2?run(std::filesystem::path(argv[1])):64;
}
#else
int main(int argc,char** argv){
    return argc==2?run(std::filesystem::path(argv[1])):64;
}
#endif
