#include "rloft/original_executable.hpp"
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <vector>

using namespace rloft;

static void store16(std::vector<std::uint8_t>& b, std::size_t p, unsigned x) {
    b[p] = static_cast<std::uint8_t>(x);
    b[p+1] = static_cast<std::uint8_t>(x >> 8);
}
static void store32(std::vector<std::uint8_t>& b, std::size_t p, unsigned x) {
    store16(b, p, x & 65535U);
    store16(b, p+2, x >> 16);
}
static void rejected(const std::vector<std::uint8_t>& b) {
    bool threw = false;
    try { (void)inspect_original_executable(b); }
    catch (const std::invalid_argument&) { threw = true; }
    assert(threw);
}
static void synthetic_test() {
    std::vector<std::uint8_t> mz(1024);
    mz[0] = 'M'; mz[1] = 'Z';
    store16(mz,2,0); store16(mz,4,2); // 1024-byte MZ
    store16(mz,8,32); // 512-byte header
    const auto plain = inspect_original_executable(mz);
    assert(plain.kind == OriginalExecutableKind::DosMz);
    assert(plain.mz_entry_file_offset == 512);
    auto bad = mz;
    bad[0] = 'A'; rejected(bad);
    bad = mz; store16(bad,4,0xffff); rejected(bad);
    bad = mz; bad.push_back(0); rejected(bad); // overlay without LE

    auto bound = mz;
    bound.resize(4096);
    const std::size_t le = 1024;
    bound[le] = 'L'; bound[le+1] = 'E';
    store32(bound,le+0x14,2); // two 4-KiB pages
    store32(bound,le+0x18,1); // entry in object 1
    store32(bound,le+0x1c,0x40);
    store32(bound,le+0x28,4096);
    store32(bound,le+0x40,0xc4);
    store32(bound,le+0x44,2);
    store32(bound,le+0xc4,0x100); // code object size
    store32(bound,le+0xc8,0x10000);
    store32(bound,le+0xcc,0x2005);
    store32(bound,le+0xd0,1); store32(bound,le+0xd4,1);
    store32(bound,le+0xdc,0x100); // data object size
    store32(bound,le+0xe0,0x30000);
    store32(bound,le+0xe4,0x2003);
    store32(bound,le+0xe8,2); store32(bound,le+0xec,1);
    const auto parsed = inspect_original_executable(bound);
    assert(parsed.kind == OriginalExecutableKind::BoundLinearExecutable);
    assert(parsed.le_file_offset == le && parsed.le_entry_offset == 0x40);
    assert(parsed.le_pages == 2 && parsed.objects.size() == 2);
    bad = bound; store32(bad,le+0x1c,0x100); rejected(bad);
    bad = bound; store32(bad,le+0xe8,3); rejected(bad);
    bad = bound; store32(bad,le+0x14,3); rejected(bad);
}
static std::vector<std::uint8_t> read_file(const char* path) {
    std::ifstream f(path,std::ios::binary);
    if (!f) throw std::runtime_error(std::string("could not open ")+path);
    return {std::istreambuf_iterator<char>(f),std::istreambuf_iterator<char>()};
}
int main(int argc,char** argv) {
    synthetic_test();
    for (const auto& spec : kOriginalExecutables) {
        assert(spec.sha256.size() == 64);
        assert(spec.file_size != 0);
    }
    if (argc > 1) {
        if (argc != 4) throw std::invalid_argument("usage: executable_tests [R.EXE CODE.1 CODE.2]");
        for (int i=1;i!=4;++i) {
            const auto bytes=read_file(argv[i]);
            const auto result=inspect_original_executable(bytes);
            const auto& spec=kOriginalExecutables[i-1];
            if (!matches_original_executable_structure(spec, bytes.size(), result))
                throw std::runtime_error("Strahd executable structure mismatch");
        }
    }
    std::cout << "original_executable_tests: PASS\n";
}
