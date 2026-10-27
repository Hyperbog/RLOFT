// Bounded source classification of original DOS CHARS access + 20440 branch.
// All guest bytes and temporary files are test-only; no fake CHARS is shipped.
#include "../../Code2/Code2.cpp"
#include "original_code2_chars_file_access.hpp"
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
    std::cout<<"ORIGINAL_31218_CHARS_DOS_ACCESS_SOURCE_PASS"
             <<" missing=source_19888 present=source_16CD0"
             <<" actual_CHARS_bytes=NOT_CREATED_BY_HOST"
             <<" malformed_file=REJECTED\n";
    return 0;
 }catch(const std::exception& e){
    std::cerr<<"ORIGINAL_31218_CHARS_ACCESS_TEST_FAIL "<<e.what()<<"\n";
    return 1;
 }
}
