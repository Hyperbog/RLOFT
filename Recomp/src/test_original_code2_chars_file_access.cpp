// Bounded source classification of original DOS CHARS access + 20440 branch.
// All guest bytes and temporary files are test-only; no fake CHARS is shipped.
#include "../../Code2/Code2.cpp"
#include "original_code2_chars_file_access.hpp"
#include "dos_readonly_files.hpp"
#include <array>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
namespace {
namespace fs=std::filesystem;
using O=rloft::original_chars_access::Outcome;
fs::path root;
unsigned intro=0,loaded=0,probes=0;
void check(bool value,const char* what) {
    if(!value)throw std::runtime_error(what);
}
code2::u32 file_access(code2::u32 path,code2::u32 mode) noexcept {
    if(path!=0x1140u || mode!=0u)return 2u;
    ++probes;
    return rloft::original_chars_access::probe(root)==O::Present?0u:1u;
}
code2::u32 original_31218(code2::u32 path,code2::u32 mode) noexcept {
    code2::Application72Boundary file{};
    file.call_37e39=&file_access;
    return code2::function_31218(file,path,mode);
}
void source_intro() noexcept {++intro;}
void source_chars() noexcept {++loaded;}
void execute_original_20440(bool present) {
    std::vector<code2::u8> data(0x300000u,0u);
    code2::Machine m{};
    m.data=data.data();
    code2::store_le32(data.data()+0xF050u,0x400u);
    data[0x472u]=1u; // skip unrelated 202F0 preface
    code2::Application46Boundary b{};
    b.call_31218=&original_31218;
    b.call_19888=&source_intro;
    b.call_16cd0=&source_chars;
    code2::function_20440(m,b,0u);
    check(intro==(present?0u:1u) &&
          loaded==(present?1u:0u),
          "compiled original 20440 chose incorrect CHARS branch");
}
// Exercise the unmodified compiled CODE.2 16CD0/16ADC functions using
// the exact CHARS handoff the native R.EXE supervisor will supply.
// This is source execution, not a host-authored party parser.
rloft_recomp::ReadOnlyDosFiles* handoff_files=nullptr;
std::vector<code2::u8>* handoff_memory=nullptr;
bool handoff_error=false;
code2::u32 handoff_open(code2::u32 path) noexcept {
    if(!handoff_files || path!=0x710u){handoff_error=true;return 0xFFFFFFFFu;}
    const auto result=handoff_files->open("CHARS",0u);
    if(result.carry){handoff_error=true;return 0xFFFFFFFFu;}
    return result.ax;
}
code2::u32 handoff_read(code2::u32 dst,code2::u32 size,
                         code2::u32 handle) noexcept {
    if(!handoff_memory || dst>handoff_memory->size() ||
       size>handoff_memory->size()-dst || !handoff_files ||
       handle>0xFFFFu){handoff_error=true;return 0u;}
    const auto result=handoff_files->read(static_cast<std::uint16_t>(handle),
                                         handoff_memory->data()+dst,size);
    if(result.carry){handoff_error=true;return 0u;}
    return result.ax;
}
void handoff_close(code2::u32 handle) noexcept {
    if(!handoff_files || handle>0xFFFFu ||
       handoff_files->close(static_cast<std::uint16_t>(handle)).carry)
        handoff_error=true;
}
void handoff_clear(code2::u32 dst,code2::u32 value,
                   code2::u32 size) noexcept {
    if(!handoff_memory || dst>handoff_memory->size() ||
       size>handoff_memory->size()-dst){handoff_error=true;return;}
    std::fill_n(handoff_memory->data()+dst,size,
                static_cast<code2::u8>(value));
}
void execute_original_chars_handoff() {
    rloft_recomp::ReadOnlyDosFiles files(root);
    std::array<code2::u8,0x920u> chars{};
    chars[0x30u]=0xA1u;
    chars[0x248u+0x30u]=0xB2u;
    chars[0x490u+0x30u]=0xC3u;
    chars[0x6D8u+0x30u]=0xD4u;
    files.install_virtual_chars(chars.data(),chars.size());
    std::vector<code2::u8> memory(0x300000u,0u);
    code2::Machine machine{};
    machine.data=memory.data();
    handoff_files=&files;
    handoff_memory=&memory;
    handoff_error=false;
    code2::Application30Boundary b{};
    b.call_312a0=&handoff_open;
    b.call_312ec=&handoff_read;
    b.call_312dc=&handoff_close;
    b.call_30d30=&handoff_clear;
    code2::function_16cd0(machine,b);
    check(!handoff_error && files.open_handles()==0u,
          "compiled original 16CD0 did not complete DOS CHARS transfer");
    for(const code2::u32 slot:{0u,1u,2u,3u}) {
        const code2::u32 offset=slot*0x248u+0x30u;
        check(memory[0x218E0u+offset]==chars[offset],
              "compiled original 16CD0 corrupted retained CHARS field");
    }
    check(code2::load_le16(memory.data()+0x218F0u)==0u &&
          code2::load_le16(memory.data()+0x21B38u)==0u &&
          code2::load_le16(memory.data()+0x21D80u)==0x8000u &&
          code2::load_le16(memory.data()+0x21FC8u)==0x8000u,
          "compiled original 16ADC party initialization not executed");
    handoff_files=nullptr;
    handoff_memory=nullptr;
}
struct Cleanup { ~Cleanup(){std::error_code ec;fs::remove_all(root,ec);} };
}
int main(){
 try {
    root=fs::temp_directory_path()/(
      "rloft_original_31218_chars_"+std::to_string(
       std::chrono::steady_clock::now().time_since_epoch().count()));
    fs::create_directories(root);
    Cleanup cleanup{};
    check(rloft::original_chars_access::probe(root)==O::Missing,
          "absent CHARS incorrectly classified as fatal");
    execute_original_20440(false);
    {
        std::ofstream output(root/"CHARS",std::ios::binary);
        check(static_cast<bool>(output),"cannot create scratch CHARS");
        std::vector<char> bytes(0x920u,0);
        output.write(bytes.data(),static_cast<std::streamsize>(bytes.size()));
        check(static_cast<bool>(output),"scratch CHARS write failed");
    }
    check(rloft::original_chars_access::probe(root)==O::Present,
          "present authentic-width CHARS not detected");
    intro=loaded=0;
    execute_original_20440(true);
    fs::remove(root/"CHARS");
    fs::create_directory(root/"CHARS");
    check(rloft::original_chars_access::probe(root)==O::UnsupportedFileType,
          "CHARS directory must not be treated as original party bytes");
    check(probes==2u,"original 20440 called 31218 unexpected number of times");
    execute_original_chars_handoff();
    std::cout<<"ORIGINAL_31218_CHARS_DOS_ACCESS_SOURCE_PASS"
             <<" missing=source_19888 present=source_16CD0"
             <<" actual_CHARS_bytes=NOT_CREATED_BY_HOST"
             <<" malformed_file=REJECTED"
             <<" original_16CD0_16ADC_virtual_CHARS=EXECUTED\n";
    return 0;
 }catch(const std::exception& e){
    std::cerr<<"ORIGINAL_31218_CHARS_ACCESS_TEST_FAIL "<<e.what()<<"\n";
    return 1;
 }
}
