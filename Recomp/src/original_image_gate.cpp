#include "original_le_image.h"
#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
using u8=std::uint8_t; using u32=std::uint32_t;
void check(bool yes,const char* why) {if(!yes)throw std::runtime_error(why);}
void put(std::vector<u8>& b,std::size_t at,u32 value,unsigned width=4) {
    check(at<=b.size() && width<=b.size()-at,"fixture invalid write");
    for(unsigned i=0;i<width;++i)b[at+i]=u8(value>>(i*8));
}
std::vector<u8> example() {
    constexpr std::size_t module=0x25D24,le=0x27BDC, data=module+0x4000;
    std::vector<u8> b(data+2*4096+0x30,0);
    put(b,module,0x5a4d,2);put(b,module+0x3c,u32(le-module));
    put(b,le,0x454c,2); put(b,le+8,2,2); put(b,le+0xA,1,2);
    put(b,le+0x14,3);put(b,le+0x18,1);put(b,le+0x1c,0x100);
    put(b,le+0x20,2);put(b,le+0x24,0x2000);
    put(b,le+0x28,4096);put(b,le+0x2c,0x30);
    put(b,le+0x30,0x25);put(b,le+0x40,0xc4);put(b,le+0x44,2);
    put(b,le+0x48,0xf4);put(b,le+0x68,0x10c);put(b,le+0x6c,0x11c);
    put(b,le+0x80,0x4000);put(b,le+0x94,2);
    const std::size_t a=le+0xc4,c=le+0xdc;
    put(b,a,0x2000);put(b,a+4,0x10000);put(b,a+8,0x2005);put(b,a+12,1);put(b,a+16,2);
    put(b,c,0x2000);put(b,c+4,0x50000);put(b,c+8,0x2003);put(b,c+12,3);put(b,c+16,1);
    for(unsigned i=0;i<3;++i)put(b,le+0xf4+4*i,(i+1)<<16);
    for(unsigned i=0;i<4;++i)put(b,le+0x10c+4*i,i*7);
    auto record=[&](unsigned index,std::uint16_t offset,u8 object,std::uint16_t target){
        const std::size_t p=le+0x11c+7*index;
        b[p]=7; b[p+1]=0; put(b,p+2,offset,2);b[p+4]=object;put(b,p+5,target,2);
    };
    record(0,0x50,2,0x150);
    record(1,0xfffe,2,0x200); // negative displacement across previous page
    record(2,0,1,0x100);
    put(b,data+0x50,0x150);put(b,data+0xffe,0x200);put(b,data+0x2000,0x100);
    return b;
}
void negative_test(const std::vector<u8>& sample,std::size_t at,u8 value) {
    auto altered=sample;altered.at(at)=value;
    try {(void)rloft_recomp::OriginalLeImage::parse(altered);}
    catch(const std::runtime_error&) {return;}
    throw std::runtime_error("malformed LE fixture was accepted");
}
void selftest() {
    const auto sample=example();
    const auto image=rloft_recomp::OriginalLeImage::parse(sample);
    check(image.applied_fixups==3 && image.backward_page_fixups==1,"cross-page fixup inventory incorrect");
    check(image.objects.size()==2 && image.objects[1].raw.size()==0x2000,"object virtual size wrong");
    check(image.objects[1].raw[0x1000]==0,"file-unbacked BSS must start zero");
    check(image.objects[0].relocated[0x50]==0x50 && image.objects[0].relocated[0x51]==0x01
          && image.objects[0].relocated[0x52]==0x05,"first relocation target incorrect");
    check(image.objects[0].relocated[0xffe]==0x00 && image.objects[0].relocated[0x1000]==0x05,
          "cross-page relocation not applied");
    check(image.objects[0].raw[0xffe]==0 && image.objects[0].raw[0xfff]==2,
          "raw object was mutated by fixups");
    negative_test(sample,0x27BDC+0x11c,0xFF); // unsupported source fixup type
    negative_test(sample,0x27BDC+0x11c+5,0xFF); // record/file bytes disagree
    negative_test(sample,0x27BDC+0xF4+3,1); // unsupported page flag
    negative_test(sample,0x25D24+0x3C,0); // incorrect nested MZ header pointer
    std::cout<<"SYNTHETIC_LE_FULL_OBJECTS=PASS\n"
             <<"SYNTHETIC_LE_NEGATIVE_FIXUPS=PASS\n"
             <<"SYNTHETIC_LE_CORRUPTION_REJECTION=PASS\n";
}
std::vector<u8> load_file(const std::string& name) {
    std::ifstream stream(name,std::ios::binary);
    check(bool(stream),"cannot open original binary");
    return {std::istreambuf_iterator<char>(stream),std::istreambuf_iterator<char>()};
}
void original_test(const std::string& file,std::size_t size,u32 fixes,u32 backwards,
                   u32 code_size,u32 data_size,u32 code_base,u32 data_base,u32 entry) {
    const auto bytes=load_file(file);
    check(bytes.size()==size,"original file length differs from authenticated version");
    const auto image=rloft_recomp::OriginalLeImage::parse(bytes);
    check(image.entry_object==1 && image.entry_offset==entry,"original entry mismatch");
    check(image.applied_fixups==fixes && image.backward_page_fixups==backwards,
          "original relocation inventory mismatch");
    check(image.objects.size()==2 && image.objects[0].virtual_size==code_size &&
          image.objects[1].virtual_size==data_size,"original object dimensions mismatch");
    check(image.objects[0].base==code_base && image.objects[1].base==data_base,
          "original object addresses mismatch");
    check(image.objects[1].raw.back()==0,"original unbacked data must be zero filled");
    std::cout<<"ORIGINAL_LE_IMAGE="<<file<<" OBJECTS=2 FIXUPS="<<fixes
             <<" BACKWARD_PAGE="<<backwards<<" CODE_BYTES="<<code_size
             <<" DATA_BYTES="<<data_size<<" PASS\n";
}
}
int main(int argc,char**argv) {
    try {
        selftest();
        if(argc==3){
            original_test(argv[1],277787u,2617u,1u,0x11302u,0x9E90u,0x10000u,0x30000u,0xCC1Cu);
            original_test(argv[2],558689u,9813u,6u,0x3A852u,0x2DD10u,0x10000u,0x50000u,0x353A0u);
        } else check(argc==1,"usage: original_image_gate [CODE.1 CODE.2]");
        return 0;
    }catch(const std::exception& e){std::cerr<<"ORIGINAL_LE_IMAGE_FAIL: "<<e.what()<<'\n';return 1;}
}
