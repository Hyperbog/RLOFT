#pragma once
// Native R.EXE runtime lifecycle preserving the retail child-process protocol.
// Runs CODE.2 FIRST, can suspend and resume one persistent child session,
// and transfers exactly 0x920 original CHARS bytes from CODE.1 to CODE.2.
//
// A host child callback represents a resumable reconstructed game module.
// The callback must only report Exited after the original CODE child genuinely
// executes an exit path. Pending is NOT a synthetic exit code. The supervisor
// never makes unsupported CODE.1 transitions merely to show a visual demo.
#include "original_r_exe_supervisor.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <stdexcept>
#include <utility>

namespace rloft { namespace native_module_runtime {
constexpr std::size_t kOriginalCharacterRecordBytes=0x248u;
constexpr std::size_t kOriginalCharacterCount=4u;
constexpr std::size_t kOriginalCharsBytes=0x920u;
static_assert(kOriginalCharacterRecordBytes*kOriginalCharacterCount==
              kOriginalCharsBytes,"Retail CHARS serialization geometry changed");
using Chars = std::array<std::uint8_t,kOriginalCharsBytes>;
enum class Progress:std::uint8_t { Pending, Exited };
struct ChildStep {
    Progress progress=Progress::Pending;
    int status=0; // meaningful only when progress == Exited
};
// Original CHARS is a complete opaque byte payload. No fake character
// fields, arbitrary structure resizing or default party initialization.
class CharsHandoff final {
    Chars payload_{};
    bool available_=false;
public:
    bool available() const noexcept {return available_;}
    const Chars& value() const {
        if(!available_)throw std::logic_error("CHARS is not present");
        return payload_;
    }
    void write_exact(const std::uint8_t* bytes,std::size_t count) {
        if(!bytes || count!=kOriginalCharsBytes)
            throw std::invalid_argument("CODE.1 CHARS must be exactly 0x920 bytes");
        for(std::size_t i=0;i<count;++i)payload_[i]=bytes[i];
        available_=true;
    }
    void erase() noexcept {
        payload_.fill(0);available_=false;
    }
};
// Code sessions are created once per original process launch and can make
// many Pending steps without reconstruction/re-initialization. The factory
// also receives the optional exact CHARS image on CODE.2 process entry.
// A CODE.2 child can import a CHARS payload read-only; only CODE.1 can write.
struct ModulePort {
    virtual ~ModulePort()=default;
    virtual ChildStep step()=0;
    virtual bool produced_chars(Chars&) {return false;}
};
using PortFactory=std::function<std::unique_ptr<ModulePort>(
    original_supervisor::Module,const CharsHandoff&)>;
class SupervisorSession final {
    original_supervisor::Machine supervisor_{};
    CharsHandoff chars_{};
    PortFactory factory_{};
    std::unique_ptr<ModulePort> child_{};
    std::uint64_t step_count_=0;
    std::uint64_t module_launches_=0;
    std::uint64_t transitions_=0;
    bool stopped_=false;
    bool blocked_=false;
public:
    explicit SupervisorSession(PortFactory factory):factory_(std::move(factory)) {
        if(!factory_)throw std::invalid_argument("Missing original module factory");
    }
    struct State {
        original_supervisor::Module current;
        bool running;
        bool stopped;
        bool blocked;
        bool chars_available;
        std::uint64_t steps;
        std::uint64_t launches;
        std::uint64_t transitions;
    };
    State state() const noexcept {
        return {supervisor_.next_module(),child_!=nullptr,stopped_,blocked_,
                chars_.available(),step_count_,module_launches_,transitions_};
    }
    const CharsHandoff& chars() const noexcept{return chars_;}
    // One deterministic dispatch iteration, not a busy-spin / synthetic
    // timer tick. External event loop invokes step_once when work is ready.
    State step_once() {
        if(stopped_)return state();
        if(blocked_)throw std::logic_error("Native child factory failed; session blocked");
        if(!child_) {
            const auto next=supervisor_.next_module();
            if(next==original_supervisor::Module::Stopped) {
                stopped_=true;return state();
            }
            // No mutation if construction fails: calling code can diagnose.
            auto candidate=factory_(next,chars_);
            if(!candidate) {
                blocked_=true;
                throw std::runtime_error("Unimplemented original child runtime; blocked without invented status");
            }
            child_=std::move(candidate);
            ++module_launches_;
        }
        const auto owner=supervisor_.next_module();
        const auto outcome=child_->step();
        ++step_count_;
        if(outcome.progress==Progress::Pending)return state();
        // Only genuine CODE.1 exit 10 consumes the CHARS producer.
        if(owner==original_supervisor::Module::Code1 && outcome.status==10) {
            Chars transfer{};
            if(!child_->produced_chars(transfer)) {
                blocked_=true;
                throw std::runtime_error("CODE.1 status10 without original 0x920-byte CHARS producer");
            }
            chars_.write_exact(transfer.data(),transfer.size());
        }
        const auto transition=supervisor_.child_finished(owner,outcome.status);
        ++transitions_;
        child_.reset();  // DOS child terminated: next process has new guest
        if(transition.action==original_supervisor::Action::DeleteCharsAndRestartCode2 ||
           transition.action==original_supervisor::Action::DeleteCharsAndStop)
            chars_.erase();
        stopped_=transition.next==original_supervisor::Module::Stopped;
        return state();
    }
};
}} // namespace rloft::native_module_runtime
