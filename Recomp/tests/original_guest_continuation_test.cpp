// Synthetic callback ABI test for the real host suspension mechanism.
// Verifies native stack survives DOS service waits; NO original game
// is claimed to execute by this test alone.
#include "../src/original_guest_continuation.hpp"
#include <array>
#include <cstdint>
#include <iostream>
#include <stdexcept>

namespace gc=rloft::guest_continuation;
void check(bool x,const char* text){if(!x)throw std::runtime_error(text);}
int main(){
    try {
        gc::Worker engine;
        int entry_count=0,return_count=0;
        std::uint64_t observed_stack=0;
        engine.start([&]() {
            ++entry_count;
            std::array<std::uint32_t,8> locals{11,22,33,44,55,66,77,88};
            auto nested=[&](std::uint32_t marker) {
                gc::Reply response{};
                const gc::Request req{gc::Service::DosMouseReset,marker,0,0,0,0};
                if(!engine.suspend(req,response))
                    throw std::runtime_error("unexpected source cancel");
                check(locals[3]==44 && response.ax==0xFFFFu &&
                      response.bx==3u, "Guest stack/response corrupted");
                locals[3]+=response.bx;
                ++return_count;
                return locals[3];
            };
            const auto first=nested(0x1234u);
            check(first==47u,"First DOS callback continuation incorrect");
            gc::Reply response{};
            const gc::Request req{gc::Service::DosMouseRange,7u,0,0,0x27Eu,0};
            if(!engine.suspend(req,response))
                throw std::runtime_error("unexpected second cancel");
            check(response.ax==0xFFFFu && locals[3]==47u,
                  "Second suspend lost the C++ guest stack");
            observed_stack=locals[3]+locals[7];
            ++return_count;
        });
        gc::Request first{};
        check(engine.await_pause(first) &&
              first.service==gc::Service::DosMouseReset &&
              first.ax==0x1234u && first.sequence==1u,
              "First original host service not exposed");
        check(engine.paused() && engine.request_count()==1u,
              "Host service suspension not persistent");
        bool wrong_rejected=false;
        try{auto wrong=first;wrong.sequence++;
            engine.resume(wrong,{0xFFFFu,3u,0u,0u});}
        catch(const std::logic_error&){wrong_rejected=true;}
        check(wrong_rejected && engine.paused(),
              "Host accepted stale/mismatched DOS service reply");
        engine.resume(first,{0xFFFFu,3u,0u,0u});
        gc::Request second{};
        check(engine.await_pause(second) &&
              second.service==gc::Service::DosMouseRange &&
              second.ax==7u && second.dx==0x27Eu &&
              second.sequence==2u,
              "Guest did not resume its existing CPU call path");
        engine.resume(second,{0xFFFFu,0,0,0});
        gc::Request completed{};
        check(!engine.await_pause(completed) && entry_count==1 &&
              return_count==2 && observed_stack==135,
              "Guest replayed entry or lost stack after second response");
        engine.stop();
        std::cout<<"ORIGINAL_GUEST_STACKFUL_DOS_SERVICE_SUSPEND_PASS"
                 <<" native_source_entries=1"
                 <<" dos_host_requests=2"
                 <<" CXX_stack_preserved=YES"
                 <<" service_reply_sequence_checked=YES"
                 <<" no_replayed_startup=YES"
                 <<" original_CODE2_gameplay=NOT_EXECUTED\n";
        return 0;
    }catch(const std::exception& e) {
        std::cerr<<"GUEST_CONTINUATION_TEST_FAIL: "<<e.what()<<'\n';
        return 1;
    }
}
