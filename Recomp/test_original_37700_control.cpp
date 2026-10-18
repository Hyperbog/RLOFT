#include "../Code2/graphics/original_37700_control.hpp"
#include <array>
#include <cstdint>
#include <stdexcept>
#include <vector>
#include <iostream>
#define VERIFY(x) do { if(!(x))throw std::runtime_error(#x); } while(false)
using rloft::code2source::Decode37700;
template<class Fn>
auto run(const std::vector<std::uint8_t>& input,unsigned stop_at,Fn&& callback) {
    unsigned n=0,position=0;
    std::vector<std::uint8_t> output;
    const auto result=rloft::code2source::function_37700_stream(
        [&](std::uint8_t& value){
            if(position==input.size())return false;
            value=input[position++];return true;
        },
        [&](std::uint8_t value){output.push_back(value);++n;},
        [&]{return n>=stop_at;},
        std::size_t(500));
    callback(result,output);
}
int main() {
    run({0xFF,1,2,3,4,5,6,7,8},8,
        [](const auto& r,const auto& out){
            VERIFY(r.result==Decode37700::completed);
            VERIFY(r.bytes_in==9 && r.bytes_out==8);
            for(unsigned i=0;i<8;i++)VERIFY(out[i]==i+1);
        });
    run({0x00,0x00,0x00},3,
        [](const auto& r,const auto& out){
            VERIFY(r.result==Decode37700::completed);
            VERIFY(r.bytes_in==3 && r.bytes_out==3);
            for(auto v:out)VERIFY(v==0xFE);
        });
    // Check that the binary's inner copy loop completes a backreference
    // even when 2FA30 sets the stop byte before its final copied byte.
    run({0x00,0x00,0x0F},3,
        [](const auto& r,const auto& out){
            VERIFY(r.result==Decode37700::completed);
            VERIFY(r.bytes_in==3 && r.bytes_out==18);
            for(auto v:out)VERIFY(v==0xFE);
        });
    run({0x01,0x7B},2,
        [](const auto& r,const auto&){
            VERIFY(r.result==Decode37700::truncated_stream);
        });
    run({0x00,0xEE,0xF0},3,
        [](const auto& r,const auto&){
            VERIFY(r.result==Decode37700::uninitialized_dictionary);
        });
    run({0x00,0x00,0x0F},100,
        [](const auto& r,const auto&){
            VERIFY(r.result==Decode37700::truncated_stream);
        });
    // Original 2FA30's stop flag can initially be already asserted.
    run({},0,[](const auto& r,const auto& out){
        VERIFY(r.result==Decode37700::completed && out.empty() && r.bytes_in==0);
    });
    std::cout<<"CODE2_37700_SOURCE_GUEST_STOP_AND_RING_CONTROL_PASS\n";
}
