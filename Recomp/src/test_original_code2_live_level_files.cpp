// The owner-local original 171FC writer and 181B4 reader share this
// bounded private file transport. These tests never contain retail media.
#include "original_code2_live_level_files.hpp"
#include <array>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>
namespace {
void verify(bool v,const char* msg) {if(!v)throw std::runtime_error(msg);}
void test_filename() {
    using rloft::live_level_files::Overlay;
    std::uint32_t level=~0u;
    constexpr std::array<std::uint8_t,16> valid{{
        'W','O','R','K','\\','L','E','V','0','2','3','.','D','A','T',0}};
    verify(Overlay::level_name(valid.data(),valid.size(),level)&&level==23u,
           "Source filename WORK\\LEV023.DAT failed");
    auto bad=valid;bad[15]='x';
    verify(!Overlay::level_name(bad.data(),bad.size(),level),
           "Unterminated source WORK filename accepted");
    bad=valid;bad[10]='9';bad[9]='9';
    verify(!Overlay::level_name(bad.data(),bad.size(),level),
           "Out-of-range original level number accepted");
    bad=valid;bad[4]='/';
    verify(!Overlay::level_name(bad.data(),bad.size(),level),
           "Non-DOS path accepted");
    bad=valid;bad[4]='l';
    verify(!Overlay::level_name(bad.data(),bad.size(),level),
           "ASCII-folded backslash accepted as a letter");
    bad=valid;bad[0]='w';bad[1]='o';bad[2]='r';bad[3]='k';
    bad[5]='l';bad[6]='e';bad[7]='v';
    bad[12]='d';bad[13]='a';bad[14]='t';
    verify(Overlay::level_name(bad.data(),bad.size(),level)&&level==23u,
           "Original lowercase DOS WORK/LEV filename rejected");
}
void test_read_write() {
    using rloft::live_level_files::Overlay;
    Overlay o;
    verify(o.create(22u)==Overlay::write_handle,"Source save create failed");
    const std::array<std::uint8_t,6> bytes{{0x22,0x23,0x7A,0x11,0x00,0xEF}};
    verify(o.write(bytes.data(),static_cast<std::uint32_t>(bytes.size()))==6u,
           "Source 31304 guest write failed");
    verify(o.seek(Overlay::write_handle,2u,0u)==2u,
           "Source 3131C SET seek failed");
    constexpr std::array<std::uint8_t,2> patch{{0x45,0x67}};
    verify(o.write(patch.data(),2u)==2u,"Source 31304 local update failed");
    verify(o.close(Overlay::write_handle),"Writer close failed");
    verify(o.open(22u)==Overlay::read_handle,"Saved level open failed");
    std::array<std::uint8_t,6> restored{};
    verify(o.read(restored.data(),6u)==6u &&
           restored==std::array<std::uint8_t,6>{{0x22,0x23,0x45,0x67,0,0xEF}},
           "Saved data do not match source file ABI");
    verify(o.seek(Overlay::read_handle,static_cast<std::uint32_t>(-2),1u)==4u,
           "Negative original DOS relative seek failed");
    verify(o.read(restored.data(),3u)==0u&&o.failed,
           "Short original saved-level read was not rejected");
}
void test_guards() {
    using rloft::live_level_files::Overlay;
    Overlay o;
    verify(o.create(23u)==Overlay::write_handle,"Guard fixture create");
    verify(!o.erase(23u),"Open source save was deleted");
    std::array<std::uint8_t,2> bytes{{1,2}};
    verify(o.write(bytes.data(),2u)==2u,"Guard fixture write");
    verify(o.close(Overlay::write_handle),"Guard fixture close");
    verify(o.open(23u)==Overlay::read_handle,"Guard fixture read");
    verify(o.seek(Overlay::read_handle,0xFFFFFFFFu,0u)==Overlay::invalid &&
           o.failed,"Negative absolute seek accepted");
    verify(o.close(Overlay::read_handle),"Guard fixture read close");
    verify(o.erase(23u)&&!o.has(23u),"Snapshot cleanup failed");
}
}
int main() {
    try {
        test_filename();test_read_write();test_guards();
        std::cout<<"ORIGINAL_CODE2_171FC_181B4_PRIVATE_LEVEL_FILES_PASS"
                 <<" work_saves=MEMORY_ONLY"
                 <<" original_source_files=UNTOUCHED"
                 <<" dos_seek=SIGNED_BOUNDED"
                 <<" truncated_read=REJECTED\n";
        return 0;
    } catch(const std::exception& e) {
        std::cerr<<"ORIGINAL_CODE2_171FC_181B4_PRIVATE_LEVEL_FILES_FAIL "
                 <<e.what()<<'\n';
        return 1;
    }
}
