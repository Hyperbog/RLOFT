// Original-source CODE.2 28110 sampled-sound request priority gate.
// Does not create a Miles/AIL device, synthesize PCM or ship media.
#include "../../Code2/Code2.cpp"
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {
void check(bool ok,const char* reason) {
    if(!ok)throw std::runtime_error(reason);
}
}

int main() {
    try {
        std::vector<code2::u8> ram(code2::kObject2VirtualSize,0u);
        code2::Machine m{};
        m.data=ram.data();
        const auto load=[&](code2::u32 off) {
            return code2::load_le32(ram.data()+off);
        };
        const auto put=[&](code2::u32 off,code2::u32 value) {
            code2::store_le32(ram.data()+off,value);
        };
        // Source 28110 must not write to the request slots when the
        // digital-sound queue is disabled, even for valid arguments.
        put(0xEE40u,0u);
        put(0xEE48u,0xFFFFFFFFu);
        code2::function_28110(m,0xABCDu,0xFFu,0u,2u);
        check(load(0xEE48u)==0xFFFFFFFFu,
              "Disabled original audio queue accepted a request");
        // Once original EE40 enables the software queue, the very same
        // compiled source owns all accepted bytes and priority decisions.
        put(0xEE40u,1u);
        code2::function_28110(m,0xABCDu,0xFFu,0u,5u);
        check(load(0xEE48u)==0xABCDu &&
              load(0xEE4Cu)==0xFFu &&
              load(0xEE50u)==5u,
              "Original 28110 failed first request");
        // Original signed compare accepts a numerically lower priority
        // and rejects a higher incoming priority for an occupied slot.
        code2::function_28110(m,0xBBu,7u,0u,6u);
        check(load(0xEE48u)==0xABCDu && load(0xEE50u)==5u,
              "Original 28110 accepted a lower-ranking pending request");
        code2::function_28110(m,0xCCu,8u,0u,3u);
        check(load(0xEE48u)==0xCCu && load(0xEE4Cu)==8u &&
              load(0xEE50u)==3u,
              "Original 28110 did not replace pending request");
        // Channel 1 is independent and original 28168 services the two
        // request records EE48 and EE54.
        put(0xEE54u,0xFFFFFFFFu);
        code2::function_28110(m,0x1234u,0x55u,1u,0u);
        check(load(0xEE54u)==0x1234u &&
              load(0xEE58u)==0x55u &&
              load(0xEE5Cu)==0u &&
              load(0xEE48u)==0xCCu,
              "Original two-channel sound queue diverged");
        // The comparison is signed, not unsigned.
        put(0xEE50u,0xFFFFFFFEu);
        code2::function_28110(m,0xDDu,9u,0u,0u);
        check(load(0xEE48u)==0xCCu &&
              load(0xEE50u)==0xFFFFFFFEu,
              "Original signed sound-priority decision was lost");
        std::cout<<"ORIGINAL_CODE2_28110_SOUND_REQUEST_QUEUE_PASS"
                 <<" original_source=COMPILED"
                 <<" enabled_and_disabled=VERIFIED"
                 <<" priority=SIGNED_COMPARISON"
                 <<" independent_channels=2"
                 <<" active_AIL_playback=NOT_VERIFIED\n";
        return 0;
    } catch(const std::exception& e) {
        std::cerr<<"ORIGINAL_CODE2_28110_SOUND_REQUEST_QUEUE_FAIL: "
                 <<e.what()<<"\n";
        return 1;
    }
}
