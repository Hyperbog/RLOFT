// Original supervisor CONTINUOUS NATIVE SESSION contract tests.
// These are synthetic child results exercising the real protocol, NOT
// a claim CODE.1 or CODE.2 gameplay itself can run to status10 yet.
#include "../src/original_r_exe_native_session.hpp"
#include <array>
#include <cstdint>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
namespace NR=rloft::native_module_runtime;
namespace OS=rloft::original_supervisor;
void check(bool ok,const char* msg){if(!ok)throw std::runtime_error(msg);}
struct Child final:NR::ModulePort {
    std::vector<NR::ChildStep> outcomes;
    std::size_t cursor=0;
    bool has_party=false;
    explicit Child(std::vector<NR::ChildStep> steps,bool writes=false)
      :outcomes(std::move(steps)),has_party(writes){}
    NR::ChildStep step() override {
        if(cursor>=outcomes.size())
            throw std::runtime_error("Unexpected second completed child execution");
        return outcomes[cursor++];
    }
    bool produced_chars(NR::Chars& output) override {
        if(!has_party)return false;
        for(std::size_t i=0;i<output.size();++i)
            output[i]=static_cast<std::uint8_t>((i*17u+3u)&0xFFu);
        return true;
    }
};
int main(){
try {
    unsigned started2=0,started1=0,withchars=0;
    NR::SupervisorSession runtime(
        [&](OS::Module module,const NR::CharsHandoff& chars)->std::unique_ptr<NR::ModulePort> {
            if(module==OS::Module::Code2) {
                ++started2;
                if(started2==1) {
                    check(!chars.available(),"First CODE.2 child must not invent CHARS");
                    return std::make_unique<Child>(
                        std::vector<NR::ChildStep>{{NR::Progress::Pending,0},
                                                   {NR::Progress::Pending,0},
                                                   {NR::Progress::Exited,10}});
                }
                if(started2==2) {
                    check(chars.available(),"CODE.1 CHARS absent from next CODE.2");
                    for(std::size_t i=0;i<NR::kOriginalCharsBytes;++i)
                        check(chars.value()[i]==
                              static_cast<std::uint8_t>((i*17u+3u)&0xFFu),
                              "CHARS byte mismatch at next CODE.2 start");
                    ++withchars;
                    return std::make_unique<Child>(
                        std::vector<NR::ChildStep>{{NR::Progress::Pending,0},
                                                   {NR::Progress::Exited,12}});
                }
                if(started2==3) {
                    check(!chars.available(),"Original status12 must delete CHARS");
                    return std::make_unique<Child>(
                        std::vector<NR::ChildStep>{{NR::Progress::Exited,0}});
                }
                throw std::runtime_error("Unexpected fourth CODE.2 launch");
            }
            if(module==OS::Module::Code1) {
                ++started1;
                check(!chars.available(),"Unexpected preexisting CHARS at CODE.1");
                return std::make_unique<Child>(
                    std::vector<NR::ChildStep>{{NR::Progress::Pending,0},
                                               {NR::Progress::Exited,10}},true);
            }
            throw std::runtime_error("Cannot launch Stopped module");
        });
    auto state=runtime.step_once();
    check(state.current==OS::Module::Code2 && state.running &&
          state.steps==1 && state.launches==1 && state.transitions==0,
          "CODE.2 initial Pending lost persistent child");
    state=runtime.step_once();
    check(state.steps==2 && state.launches==1 && state.transitions==0,
          "CODE.2 second Pending relaunched module or invented exit");
    state=runtime.step_once();
    check(state.current==OS::Module::Code1 && state.launches==1 &&
          state.transitions==1,"CODE.2 status10 did not select CODE.1");
    state=runtime.step_once();
    check(state.current==OS::Module::Code1 && state.running &&
          state.launches==2 && state.transitions==1,
          "CODE.1 must persist across pending");
    state=runtime.step_once();
    check(state.current==OS::Module::Code2 && state.chars_available &&
          state.transitions==2,"CODE.1 status10 failed CHARS handoff");
    state=runtime.step_once();
    check(state.current==OS::Module::Code2 && state.running &&
          state.launches==3,"CODE.2 CHARS receiver not running");
    state=runtime.step_once();
    check(state.current==OS::Module::Code2 && !state.chars_available &&
          state.transitions==3,"CODE.2 status12 must delete CHARS and restart");
    state=runtime.step_once();
    check(state.stopped && state.current==OS::Module::Stopped &&
          !state.chars_available && state.launches==4 &&
          state.transitions==4,"Ordinary exit did not stop supervisor");
    check(started2==3 && started1==1 && withchars==1,
          "Original CODE.2-first/CHARS/exit sequence changed");
    const auto stopped_steps=state.steps;
    check(runtime.step_once().steps==stopped_steps,"Stopped process was stepped");

    // Never fake status10 from a callback factory that cannot reconstruct
    // the current CODE.2 execution session.
    NR::SupervisorSession unavailable(
        [](OS::Module,const NR::CharsHandoff&)->std::unique_ptr<NR::ModulePort>{
            return nullptr;});
    bool blocked=false;
    try{(void)unavailable.step_once();}
    catch(const std::runtime_error&){blocked=true;}
    check(blocked && unavailable.state().blocked &&
          unavailable.state().transitions==0 &&
          unavailable.state().current==OS::Module::Code2,
          "Unavailable CODE.2 child fabricated a supervisor transition");
    NR::SupervisorSession absent_party(
        [](OS::Module module,const NR::CharsHandoff&)->std::unique_ptr<NR::ModulePort> {
            if(module==OS::Module::Code2)
                return std::make_unique<Child>(
                    std::vector<NR::ChildStep>{{NR::Progress::Exited,10}});
            return std::make_unique<Child>(
                std::vector<NR::ChildStep>{{NR::Progress::Exited,10}});
        });
    check(absent_party.step_once().current==OS::Module::Code1,
          "No party test did not reach CODE.1");
    blocked=false;
    try{(void)absent_party.step_once();}
    catch(const std::runtime_error&){blocked=true;}
    check(blocked && absent_party.state().blocked &&
          absent_party.state().transitions==1 &&
          !absent_party.state().chars_available,
          "CODE.1 cannot handoff without genuine CHARS production");
    NR::CharsHandoff bounded{};
    const std::array<std::uint8_t,6> too_short{1,2,3,4,5,6};
    bool length_rejected=false;
    try{bounded.write_exact(too_short.data(),too_short.size());}
    catch(const std::invalid_argument&){length_rejected=true;}
    check(length_rejected && !bounded.available(),
          "Undersized CHARS image accepted");
    std::cout<<"NATIVE_R_EXE_PERSISTENT_MODULE_SESSION_PASS"
             <<" first_child=CODE2"
             <<" pending_preserves_child=YES"
             <<" status10_CODE1=YES"
             <<" opaque_CHARS_bytes=2336"
             <<" status12_cleanup=YES"
             <<" unsupported_child=FAIL_CLOSED"
             <<" missing_CHARS=FAIL_CLOSED"
             <<" original_gameplay_child_callbacks=NOT_YET_CONNECTED\n";
    return 0;
}catch(const std::exception& ex){
    std::cerr<<"NATIVE_R_EXE_SESSION_FAIL: "<<ex.what()<<"\n";
    return 1;
}
}