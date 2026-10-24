// Asset-free Creative Voice PCM8 parser test, no OS audio device.
#include "original_voc_pcm8.hpp"
#include <algorithm>
#include <cstdint>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <string>
namespace {
using Byte=std::uint8_t;
void check(bool v,const char* message) {if(!v)throw std::runtime_error(message);}
void reject(const std::vector<Byte>& v,const char* scenario) {
    try { (void)rloft::host_voc::decode_pcm8_mono(v); }
    catch(const std::runtime_error&) { return; }
    throw std::runtime_error(std::string("Rejected VOC case accepted: ")+scenario);
}
std::vector<Byte> minimal() {
    const char signature[20] = {'C','r','e','a','t','i','v','e',' ',
        'V','o','i','c','e',' ','F','i','l','e','\x1a'};
    std::vector<Byte> v(signature,signature+20);
    const unsigned ver=0x10Au;
    const unsigned ck=((~ver)+0x1234u)&0xFFFFu;
    v.insert(v.end(),{26u,0u,Byte(ver),Byte(ver>>8),Byte(ck),Byte(ck>>8)});
    // Type 1: 2 control bytes + 3 bytes unsigned PCM, 8000 Hz.
    v.insert(v.end(),{1u,5u,0u,0u,131u,0u,0u,127u,255u});
    // Type 2: continue the existing byte stream and sample rate.
    v.insert(v.end(),{2u,1u,0u,0u,64u,0u});
    return v;
}
}
int main() {
    try {
        const auto correct=minimal();
        const auto data=rloft::host_voc::decode_pcm8_mono(correct);
        check(data.rate_hz==8000u && data.pcm8==std::vector<Byte>({0u,127u,255u,64u}),
              "Original Creative Voice type-1/2 PCM sample contents diverged");
        auto bad=correct;
        bad[0]='x'; reject(bad,"magic mismatch");
        bad=correct;bad[24]^=1u;reject(bad,"checksum mismatch");
        bad=correct;bad[31]=1u;reject(bad,"non-PCM codec");
        bad=correct;bad[35]=3u; // unsupported sound block after first source block
        reject(bad,"unsupported block");
        bad=correct;bad[27]=255u;reject(bad,"oversized source block");
        bad=correct;bad.pop_back();reject(bad,"missing terminator");
        bad=correct;bad.push_back(0x42u);reject(bad,"non-padding after terminator");
        std::cout<<"RLOFT_PRIVATE_VOC_PCM8_STRICT_SOURCE_TEST_PASS"
                 <<" type1_and_type2=VERIFIED"
                 <<" rate_hz=8000"
                 <<" unsupported_formats=REJECTED"
                 <<" sample_bytes=SYNTHETIC_ONLY"
                 <<" original_Miles_AIL=NOT_CONNECTED\n";
        return 0;
    }catch(const std::exception& e){
        std::cerr<<"RLOFT_PRIVATE_VOC_PCM8_STRICT_SOURCE_TEST_FAIL: "
                 <<e.what()<<"\n";
        return 1;
    }
}
