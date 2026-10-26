#pragma once
// Suspends the *actual* native thread running the compiled original CODE.2
// source at a DOS host service callback. Unlike setjmp/longjmp stop probes,
// this preserves the real C++ call stack and does not replay startup.
// Request/reply is an explicit host boundary; no replies are invented here.
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <exception>
#include <functional>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <utility>

namespace rloft { namespace guest_continuation {

enum class Service : std::uint8_t {
    DosMouseReset = 1,
    DosMouseRange = 2,
    DpmiRealModeInt33 = 3,
    DosDiskFree = 4,
    Original2F468ResourceSelection = 5,
    Original290F4VideoMode = 6,
    Original2E590PresentationReset = 7, // superseded: 2E590 is source-backed page push
    Original5E38PostDoublePageClear = 8, // superseded by reconstructed 5E38 source
    Original312A0CharacterPath = 9, // superseded by original DOS read-only source transport
    Original16D04SecondInit = 10,
    Original1FC14KeyboardEntry = 11,
    Original20440MousePointer = 12, // INT33 fn4 position and fn3 pointer state
    Original20440MouseCallback = 13 // INT33 fn0C install from original 1A064
};
struct Request {
    Service service=Service::DosMouseReset;
    std::uint32_t ax=0, bx=0, cx=0, dx=0;
    std::uint64_t sequence=0;
};
struct Reply {
    std::uint32_t ax=0, bx=0, cx=0, dx=0;
};
// Running CODE.2 code cannot be preempted safely. Cancellation is
// cooperative, at original DOS-service callbacks or explicit guest-side
// checkpoints. Never detach a running guest: it owns borrowed source RAM.
class Worker final {
    mutable std::mutex mutex_;
    std::condition_variable signal_;
    std::thread thread_;
    enum class State { Starting, Running, Paused, Finished };
    State state_=State::Starting;
    Request requested_{};
    Reply response_{};
    bool shutting_down_=false;
    std::exception_ptr failure_{};
    std::uint64_t requests_=0;
public:
    enum class WaitResult { Paused, Finished, TimedOut };
    Worker()=default;
    Worker(const Worker&)=delete;
    Worker& operator=(const Worker&)=delete;
    ~Worker() { stop(); }

    void start(std::function<void()> run_guest) {
        if(!run_guest)throw std::invalid_argument("Guest source entry is absent");
        std::lock_guard<std::mutex> guard(mutex_);
        if(thread_.joinable())throw std::logic_error("Guest entry already running");
        state_=State::Running;
        thread_=std::thread([this,run_guest=std::move(run_guest)]() {
            try { run_guest(); }
            catch(...) {
                std::lock_guard<std::mutex> lock(mutex_);
                failure_=std::current_exception();
            }
            {
                std::lock_guard<std::mutex> lock(mutex_);
                state_=State::Finished;
            }
            signal_.notify_all();
        });
    }
    // Called inside a reconstructed original CODE.2 DOS callback on
    // the worker's stack. False means shutdown: caller must exit its
    // source execution without consuming a made-up DOS result.
    bool suspend(Request query,Reply& answer) {
        std::unique_lock<std::mutex> lock(mutex_);
        if(std::this_thread::get_id()!=thread_.get_id())
            throw std::logic_error("Only original guest thread may suspend");
        if(shutting_down_)return false;
        query.sequence=++requests_;
        requested_=query;
        state_=State::Paused;
        signal_.notify_all();
        signal_.wait(lock,[this] {
            return shutting_down_ || state_==State::Running;
        });
        if(shutting_down_)return false;
        answer=response_;
        return true;
    }
    // A diagnostic/host supervisor may impose a deadline on the NEXT
    // original DOS callback. Timeout never resumes or destroys the guest;
    // a still-running uncooperative source needs a process-level watchdog.
    WaitResult await_pause_for(Request& out,
            std::chrono::milliseconds limit) {
        if(limit.count()<0)
            throw std::invalid_argument("Negative original guest wait limit");
        std::unique_lock<std::mutex> lock(mutex_);
        if(!signal_.wait_for(lock,limit,[this] {
            return state_==State::Paused || state_==State::Finished;
        }))return WaitResult::TimedOut;
        if(state_==State::Finished) {
            if(failure_)std::rethrow_exception(failure_);
            return WaitResult::Finished;
        }
        out=requested_;
        return WaitResult::Paused;
    }
    // Host waits for the exact DOS service requested by original code,
    // or a completed/failed guest. No polling a guessed instruction EIP.
    bool await_pause(Request& out) {
        std::unique_lock<std::mutex> lock(mutex_);
        signal_.wait(lock,[this] {
            return state_==State::Paused || state_==State::Finished;
        });
        if(state_==State::Finished) {
            if(failure_)std::rethrow_exception(failure_);
            return false;
        }
        out=requested_;
        return true;
    }
    void resume(const Request& observed,const Reply& response) {
        std::lock_guard<std::mutex> lock(mutex_);
        if(shutting_down_ || state_!=State::Paused ||
           observed.sequence!=requested_.sequence)
            throw std::logic_error("Host reply does not match paused original DOS request");
        response_=response;
        state_=State::Running;
        signal_.notify_all();
    }
    // Source callbacks can legitimately request heap services WHILE
    // their original guest thread is running, not only when it is
    // paused at an external DOS request. Do not grant a parallel host
    // thread access to mutable guest RAM in the Running state.
    bool calling_from_guest_thread() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return thread_.joinable() &&
               std::this_thread::get_id()==thread_.get_id();
    }
    bool paused() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return state_==State::Paused;
    }
    std::uint64_t request_count() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return requests_;
    }
    // Explicit two-phase shutdown. request_stop() may be called by the
    // host while the guest is still executing; suspend() then returns
    // false without manufacturing a DOS response. join() owns completion
    // and must run before the guest's captured memory is destroyed.
    void request_stop() noexcept {
        std::lock_guard<std::mutex> lock(mutex_);
        shutting_down_=true;
        signal_.notify_all();
    }
    bool stop_requested() const noexcept {
        std::lock_guard<std::mutex> lock(mutex_);
        return shutting_down_;
    }
    bool finished() const noexcept {
        std::lock_guard<std::mutex> lock(mutex_);
        return state_==State::Finished;
    }
    void join() noexcept {
        if(thread_.joinable()) {
            if(thread_.get_id()==std::this_thread::get_id())
                std::terminate();
            thread_.join();
        }
    }
    void stop() noexcept {
        request_stop();
        join();
    }
};
}} // namespace rloft::guest_continuation
