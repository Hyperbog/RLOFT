// Synthetic and media-free ABI test for borrowed CODE.2 IVT/overlay callbacks.
#include "game/core/GuestCallbacks.hpp"
#include "game/save/Save.hpp"
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {
void require(bool ok,const char* why) {if(!ok)throw std::runtime_error(why);}
}
int main() {
    try {
        using namespace rloft::guest_callbacks;
        VectorTable ivt{};
        set_vector(ivt,9u,0x308F8u,0x30u);
        std::uint32_t offset=0u;
        std::uint16_t selector=0u;
        get_vector(ivt,9u,&offset,&selector);
        require(offset==0x308F8u && selector==0x30u,"IRQ9 vector callback differs");
        set_vector(ivt,8u,0x30708u,0x30u);
        get_vector(ivt,8u,&offset,&selector);
        require(offset==0x30708u && selector==0x30u,"IRQ8 vector callback differs");

        std::vector<std::uint8_t> guest(32u,0u);
        for(std::uint32_t i=0u;i<8u;++i)guest[3u+i]=static_cast<std::uint8_t>(0xA0u+i);
        rloft::live_level_files::Overlay overlay{};
        const auto write=overlay.create(7u);
        require(write==rloft::live_level_files::Overlay::write_handle,
                "Original WORK writer unavailable");
        const auto invalid=write_level_from_guest(overlay,guest,31u,2u,write);
        require(invalid.status==OverlayIoStatus::GuestOutOfBounds &&
                !overlay.failed && overlay.write_position==0u,
                "Guest write overflow touched retained overlay");
        const auto badHandle=write_level_from_guest(overlay,guest,3u,8u,0u);
        require(badHandle.status==OverlayIoStatus::InvalidHandle &&
                !overlay.failed && overlay.write_position==0u,
                "Unexpected write handle touched retained overlay");
        const auto written=write_level_from_guest(overlay,guest,3u,8u,write);
        require(written.status==OverlayIoStatus::Ok && written.bytes==8u,
                "Source guest bytes not written through private overlay");
        require(overlay.close(write),"Original writer close failed");
        const auto read=overlay.open(7u);
        require(read==rloft::live_level_files::Overlay::read_handle,
                "Saved level reopening failed");
        const auto failedRead=read_level_into_guest(overlay,guest,30u,3u,read);
        require(failedRead.status==OverlayIoStatus::GuestOutOfBounds &&
                !overlay.failed && overlay.read_position==0u,
                "Guest read overflow advanced retained file pointer");
        const auto readResult=read_level_into_guest(overlay,guest,17u,8u,read);
        require(readResult.status==OverlayIoStatus::Ok && readResult.bytes==8u,
                "Source bytes were not read back from original overlay");
        for(std::uint32_t i=0u;i<8u;++i)
            require(guest[17u+i]==guest[3u+i],
                    "Original guest memory differs after saved-level transfer");
        require(overlay.close(read) && overlay.idle(),
                "Readback changed the session-owned overlay lifetime");
        std::cout<<"RLOFT_CODE2_GUEST_BORROWED_CALLBACK_ABI_PASS\n";
        return 0;
    }catch(const std::exception& e) {
        std::cerr<<"RLOFT_CODE2_GUEST_BORROWED_CALLBACK_ABI_FAIL "<<e.what()<<"\n";
        return 1;
    }
}
