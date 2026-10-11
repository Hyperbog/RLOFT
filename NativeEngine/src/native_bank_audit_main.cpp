#include "rloft_native/native_bank_system.hpp"

#include <exception>
#include <filesystem>
#include <iostream>
#include <string>

namespace {
int audit(const std::filesystem::path& source,bool legacy) {
    try {
        const auto media=rloft_native::NativeMedia::open(source);
        const auto result=rloft_native::audit_native_bank_system(media,legacy);
        std::cout<<rloft_native::native_bank_system_json(result);
        std::cout.flush();
        return result.failed==0 && result.absent_banks==0 ? 0 : 3;
    } catch(const std::exception& e) {
        std::cerr<<"rloft native full-bank audit: "<<e.what()<<"\n";
        return 2;
    }
}
}
#ifdef _WIN32
int wmain(int argc,wchar_t** argv) {
    if(argc!=2 && argc!=3) {
        std::cerr<<"usage: rloft_native_bank_audit [--active-only] <original game directory or game.gog>\n";
        return 64;
    }
    if(argc==2)return audit(std::filesystem::path(argv[1]),true);
    if(std::wstring(argv[1])!=L"--active-only")return 64;
    return audit(std::filesystem::path(argv[2]),false);
}
#else
int main(int argc,char** argv) {
    if(argc!=2 && argc!=3) {
        std::cerr<<"usage: rloft_native_bank_audit [--active-only] <original game path>\n";
        return 64;
    }
    if(argc==2)return audit(std::filesystem::path(argv[1]),true);
    if(std::string(argv[1])!="--active-only")return 64;
    return audit(std::filesystem::path(argv[2]),false);
}
#endif
