#pragma once
// Original R.EXE (SHA256 a6d5874d...) DOS supervisor status protocol.
// Authentication: PreservationArchive/01_EXECUTABLE_CONTAINER_AND_MODULE_CYCLE.md
// This is a PURE STATE MACHINE, not a DOS process launcher or CHARS writer.
#include <cstdint>
#include <stdexcept>
namespace rloft { namespace original_supervisor {
enum class Module:std::uint8_t {Code2,Code1,Stopped};
enum class Action:std::uint8_t {None,StartCode2,StartCode1,DeleteCharsAndRestartCode2,DeleteCharsAndStop};
struct Transition {Module next=Module::Stopped;Action action=Action::None;};
class Machine {
    Module next_=Module::Code2;
public:
    Module next_module() const noexcept { return next_; }
    Transition child_finished(Module ran, int exit_code) {
        if(next_==Module::Stopped || ran!=next_)
            throw std::logic_error("R.EXE child completion is out of supervisor order");
        if(ran==Module::Code2) {
            if(exit_code==10){next_=Module::Code1;return {next_,Action::StartCode1};}
            if(exit_code==12){next_=Module::Code2;return {next_,Action::DeleteCharsAndRestartCode2};}
            next_=Module::Stopped;return {next_,Action::DeleteCharsAndStop};
        }
        // In the original R.EXE loop, status 10 from CODE.1 causes a
        // CODE.2 relaunch. Other exits terminate and erase CHARS.
        if(exit_code==10){next_=Module::Code2;return {next_,Action::StartCode2};}
        next_=Module::Stopped;return {next_,Action::DeleteCharsAndStop};
    }
};
}} // namespace rloft::original_supervisor
