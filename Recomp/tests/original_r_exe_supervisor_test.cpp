// Machine-checked source-level R.EXE supervisor transitions.
#include "../src/original_r_exe_supervisor.hpp"
#include "../src/original_media_identity.hpp"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <vector>
namespace S=rloft::original_supervisor;
void require(bool condition,const char* what){
    if(!condition)throw std::runtime_error(what);
}
int main(int argc,char** argv) {
    try {
        if(argc>2)throw std::invalid_argument("usage: supervisor_test [owner-R.EXE]");
        if(argc==2){
            std::ifstream f(argv[1],std::ios::binary);
            if(!f)throw std::runtime_error("Cannot open original R.EXE");
            const std::vector<std::uint8_t> bytes{
                std::istreambuf_iterator<char>(f),
                std::istreambuf_iterator<char>()};
            require(bytes.size()==12368u &&
                rloft_recomp::sha256_hex(bytes)==
                "a6d5874dc91fb068a3ed0a07290547c310ee7f4f50be4d8176c4788f251631b0",
                "Original R.EXE supervisor SHA-256 mismatch");
        }
        S::Machine host{};
        require(host.next_module()==S::Module::Code2,"Initial supervisor must start CODE.2");
        const auto c2wantschars=host.child_finished(S::Module::Code2,10);
        require(c2wantschars.next==S::Module::Code1 &&
                c2wantschars.action==S::Action::StartCode1,"CODE.2 status10");
        const auto c1sentchars=host.child_finished(S::Module::Code1,10);
        require(c1sentchars.next==S::Module::Code2 &&
                c1sentchars.action==S::Action::StartCode2,"CODE.1 status10");
        const auto restart=host.child_finished(S::Module::Code2,12);
        require(restart.next==S::Module::Code2 &&
                restart.action==S::Action::DeleteCharsAndRestartCode2,"CODE.2 status12");
        const auto done=host.child_finished(S::Module::Code2,0);
        require(done.next==S::Module::Stopped &&
                done.action==S::Action::DeleteCharsAndStop,"CODE.2 ordinary exit");
        bool stopped=false;
        try{(void)host.child_finished(S::Module::Code2,10);}
        catch(const std::logic_error&){stopped=true;}
        require(stopped,"Stopped supervisor accepted a child exit");
        S::Machine out_of_order{};
        bool wrong=false;
        try{(void)out_of_order.child_finished(S::Module::Code1,10);}
        catch(const std::logic_error&){wrong=true;}
        require(wrong,"CODE.1 launched before CODE.2");
        std::cout<<"ORIGINAL_R_EXE_SUPERVISOR_PROTOCOL_PASS"
                 <<" startup=CODE2 status10=CODE1 status12=DELETE_CHARS_RESTART"
                 <<" module_exit=DELETE_CHARS_STOP"
                 <<" invalid_sequence=REJECTED"
                 <<" original_media="<<(argc==2?"SHA256_AUTHENTIC":"SYNTHETIC_STATES")
                 <<" DOS_PROCESS_CREATION=NOT_IMPLEMENTED\n";
    }catch(const std::exception& e){
        std::cerr<<"R_EXE_SUPERVISOR_PROTOCOL_FAIL: "<<e.what()<<"\n";
        return 1;
    }
    return 0;
}
