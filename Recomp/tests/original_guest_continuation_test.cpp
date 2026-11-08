// Synthetic callback ABI test for the real host suspension mechanism.
// Verifies native stack survives DOS service waits; NO original game
// is claimed to execute by this test alone.
#include "../src/original_guest_continuation.hpp"
#include <array>
#include <atomic>
#include <chrono>
#include <thread>
#include <cstdint>
#include <iostream>
#include <stdexcept>

namespace gc=rloft::guest_continuation;
void check(bool x,const char* text){if(!x)throw std::runtime_error(text);}
void check_cooperative_cancel() {
    gc::Worker worker;
    std::atomic<bool> cancelled{false};
    worker.start([&]() {
        gc::Reply reply{};
        if(!worker.suspend({gc::Service::DosMouseReset,9u,0,0,0,0},reply)) {
            cancelled.store(true);
            return;
        }
        throw std::logic_error("Cancellation consumed a fabricated mouse reply");
    });
    gc::Request request{};
    check(worker.await_pause(request) && request.ax==9u,
          "Cancellation test did not reach original host boundary");
    worker.request_stop();
    check(worker.stop_requested(), "Host shutdown request lost");
    bool rejected=false;
    try {worker.resume(request,{0xFFFFu,0u,0u,0u});}
    catch(const std::logic_error&) {rejected=true;}
    check(rejected, "Shutdown accepted a late fake DOS reply");
    worker.join();
    check(cancelled.load() && worker.finished(),
          "Paused guest did not cooperatively unwind at the original boundary");
}
void check_running_wait_timeout_and_cooperative_checkpoint() {
    gc::Worker worker;
    std::atomic<bool> finished_source{false};
    worker.start([&]() {
        while(!worker.stop_requested())std::this_thread::yield();
        finished_source.store(true);
    });
    gc::Request request{};
    check(worker.await_pause_for(request,std::chrono::milliseconds(15))==
          gc::Worker::WaitResult::TimedOut,
          "Running unsuspended source fabricated a DOS host service");
    worker.request_stop();
    worker.join();
    check(finished_source.load() && worker.finished(),
          "Running guest ignored a cooperative source checkpoint");
}
void check_failure_propagation() {
    gc::Worker worker;
    worker.start([]() {throw std::runtime_error("EXPECTED_GUEST_TEST_FAILURE");});
    gc::Request request{};
    bool caught=false;
    try {(void)worker.await_pause(request);}
    catch(const std::runtime_error& e) {
        caught=std::string(e.what())=="EXPECTED_GUEST_TEST_FAILURE";
    }
    worker.stop();
    check(caught && worker.finished(),
          "Failed guest silently lost its original exception");
}
int main(){
    try {
        gc::Worker engine;
        int entry_count=0,return_count=0;
        std::uint64_t observed_stack=0;
        check(!engine.calling_from_guest_thread(),
              "Host must never impersonate original CODE.2 worker");
        engine.start([&]() {
            check(engine.calling_from_guest_thread(),
                  "Original CODE.2 callback must own running worker thread");
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
        check(engine.paused() && engine.request_count()==1u &&
              !engine.calling_from_guest_thread(),
              "Paused worker must not make its host a guest thread");
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
        check_cooperative_cancel();
        check_running_wait_timeout_and_cooperative_checkpoint();
        check_failure_propagation();
        std::cout<<"ORIGINAL_GUEST_STACKFUL_DOS_SERVICE_SUSPEND_PASS"
                 <<" native_source_entries=1"
                 <<" dos_host_requests=2"
                 <<" CXX_stack_preserved=YES"
                 <<" service_reply_sequence_checked=YES"
                 <<" cooperative_shutdown=VERIFIED" 
                 <<" late_reply=REJECTED"
                 <<" unsuspended_guest_timeout=REPORTED"
                 <<" source_failure=PROPAGATED"
                 <<" no_replayed_startup=YES"
                 <<" original_CODE2_gameplay=NOT_EXECUTED\n";
        return 0;
    }catch(const std::exception& e) {
        std::cerr<<"GUEST_CONTINUATION_TEST_FAIL: "<<e.what()<<'\n';
        return 1;
    }
}
