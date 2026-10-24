// Isolated source 28168 -> 27F24 software dispatch and Miles ABI receipt.
// Synthetic 8-byte payload and callbacks. NO actual music, sound or driver.
#include "../../Code2/Code2.cpp"
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>
namespace {
struct Calls {
    unsigned lookups=0, status=0, set_sample=0, set_rate=0, start=0;
    code2::u32 last_id=0, last_bank=0, last_device=0, last_source=0;
    code2::u32 last_selector=0, last_flags=0, last_format=0;
} observed{};
void check(bool b,const char* message) {
    if(!b)throw std::runtime_error(message);
}
code2::u32 fetch_resource(code2::u32 id,code2::u32 bank) {
    ++observed.lookups; observed.last_id=id; observed.last_bank=bank;
    return 0x20000u;
}
code2::u32 status(code2::u32 dev) {
    ++observed.status; observed.last_device=dev;
    return 0u; // Synthesized idle device for isolated ABI test only.
}
code2::u32 set_sample(code2::u32 dev,code2::u32 src,
                      code2::u32 selector,code2::u32 flags) {
    ++observed.set_sample; observed.last_device=dev;
    observed.last_source=src; observed.last_selector=selector;
    observed.last_flags=flags; return 0u;
}
code2::u32 set_rate(code2::u32 dev,code2::u32 src,
                    code2::u32 selector,code2::u32 format,code2::u32 flags) {
    ++observed.set_rate; observed.last_device=dev;
    observed.last_source=src; observed.last_selector=selector;
    observed.last_format=format; observed.last_flags=flags; return 0u;
}
code2::u32 start(code2::u32 dev) {
    ++observed.start; observed.last_device=dev; return 0u;
}
}
int main() {
    try {
        std::vector<code2::u8> memory(code2::kObject2VirtualSize,0u);
        code2::Machine m{}; m.data=memory.data(); m.cpu.ds=0x30u;
        const auto put=[&](code2::u32 a,code2::u32 b){
            code2::store_le32(memory.data()+a,b);
        };
        const auto get=[&](code2::u32 a){
            return code2::load_le32(memory.data()+a);
        };
        code2::Application55Boundary device{};
        device.call_2f29c=&fetch_resource;
        device.call_36803=&status;
        device.call_367f9=&set_sample;
        device.call_367ef=&set_rate;
        device.call_3680d=&start;
        put(0xEE48u,0xFFFFFFFFu);
        put(0xEE54u,0xFFFFFFFFu);
        put(0xEE40u,0u);
        code2::function_28168(m,device);
        check(observed.lookups==0u && get(0xEE48u)==0xFFFFFFFFu,
              "Source 28168 disabled-queue path unexpectedly called driver");
        // Source 28168 clears both requests even though 27F24's original
        // EE70 sound switch causes a no-playback early return.
        put(0xEE40u,1u);
        memory[0xEE70u]=0u;
        code2::function_28110(m,11u,0xFFu,0u,1u);
        code2::function_28110(m,22u,0xFFu,1u,5u);
        code2::function_28168(m,device);
        check(get(0xEE48u)==0xFFFFFFFFu &&
              get(0xEE54u)==0xFFFFFFFFu &&
              observed.lookups==0u && observed.start==0u,
              "Source 28168 muted dispatch did not clear both request slots");
        // Active playback requests now execute the actual reconstructed
        // 27F24 resource copy and its exact 367F9/367EF/3680D order.
        // This test owns its synthetic sample packet and the fake AIL ABI.
        memory[0xEE70u]=1u;
        put(0xEE44u,2u);
        put(0xEE60u,0x31000u);
        put(0xEE64u,0x1400u);
        put(0xEE6Cu,0x67u);
        put(0x20000u,8u);
        for(unsigned i=0;i<8u;++i)memory[0x20004u+i]=static_cast<code2::u8>(0xA0u+i);
        code2::function_28110(m,34u,0xFFu,0u,1u);
        code2::function_28168(m,device);
        check(observed.lookups==1u && observed.last_id==34u &&
              observed.last_bank==2u && observed.status==1u &&
              observed.set_sample==1u && observed.set_rate==1u &&
              observed.start==1u && observed.last_device==0x67u &&
              observed.last_source==0x31000u &&
              observed.last_selector==0x30u &&
              observed.last_flags==0xFFFFFFFFu &&
              observed.last_format==0x1400u &&
              get(0xEE48u)==0xFFFFFFFFu &&
              get(0xEE54u)==0xFFFFFFFFu,
              "Compiled source 28168/27F24 selected the wrong driver ABI sequence");
        for(unsigned i=0;i<8u;++i)
            check(memory[0x31000u+i]==static_cast<code2::u8>(0xA0u+i),
                  "Original 27F24 payload transfer does not match synthetic bytes");
        std::cout<<"ORIGINAL_CODE2_28168_27F24_SOURCE_DISPATCH_PASS"
                 <<" muted_queue=SOURCE_CLEAR"
                 <<" active_sample_copy=8"
                 <<" resource_lookup=1"
                 <<" AIL_36803_367F9_367EF_3680D=SYNTHETIC_ABI"
                 <<" real_device=NOT_CONNECTED\n";
        return 0;
    }catch(const std::exception& e){
        std::cerr<<"ORIGINAL_CODE2_28168_27F24_SOURCE_DISPATCH_FAIL: "
                 <<e.what()<<"\n";
        return 1;
    }
}
