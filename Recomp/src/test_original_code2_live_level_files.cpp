// The owner-local original 171FC writer and 181B4 reader share this
// bounded private file transport. These tests never contain retail media.
#include "original_code2_live_level_files.hpp"
#include "original_code2_saved_level_policy.hpp"
#include "original_code2_checked_dos_seek.hpp"
#include <array>
#include <cstdint>
#include <iostream>
#include <new>
#include <utility>
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
void test_original_saved_level_policy() {
    using namespace rloft::original_saved_level_policy;
    verify(authenticated_no_departing_level(0xFFFFFFFFu,23u,true,1u,true,156u,1u),
           "Authentic source 07938 saved-level sentinel rejected");
    verify(authenticated_no_departing_level(0xFFFFFFFFu,26u,true,1u,true,156u,1u),
           "Valid original saved-level index was rejected");
    verify(!authenticated_no_departing_level(0xFFFFFFFFu,32u,true,1u,true,156u,1u),
           "Sentinel enabled a destination outside LEVELS.DAT index");
    verify(!authenticated_no_departing_level(0xFFFFFFFFu,23u,false,1u,true,156u,1u),
           "Sentinel accepted without 07938 restoration");
    verify(!authenticated_no_departing_level(0xFFFFFFFFu,23u,true,2u,true,156u,1u),
           "Sentinel accepted repeated loader dispatch");
    verify(!authenticated_no_departing_level(0xFFFFFFFFu,23u,true,1u,false,156u,1u),
           "Sentinel allowed missing original LEV023");
    verify(!authenticated_no_departing_level(0xFFFFFFFFu,23u,true,1u,true,0u,1u),
           "Sentinel accepted no original save reads");
    verify(!authenticated_no_departing_level(0xFFFFFFFFu,23u,true,1u,true,156u,0u),
           "Sentinel accepted no source save opens");
    verify(!authenticated_no_departing_level(22u,23u,true,1u,true,156u,1u),
           "Sentinel invented a previous level");
    verify(source_selected_transition(23u,26u) &&
           source_selected_transition(26u,23u) &&
           source_selected_transition(1u,31u) &&
           !source_selected_transition(31u,31u) &&
           !source_selected_transition(0xFFFFFFFFu,23u),
           "Original source-selected transition bounds changed");
}
void test_signed_dos_seek_contract() {
    // Unlike the writable WORK overlay, restored SAVE.DAT does not
    // allow seeking past EOF. Both transports share exact arithmetic.
    using rloft::dos_seek::compute;
    std::uint32_t result=0xDEADBEEFu;
    verify(compute(0u,0u,0u,9288u,9288u,result)&&result==0u,
           "SET 0 failed");
    verify(compute(0xFFFFFFFEu,2u,0u,9288u,9288u,result) &&
           result==9286u,"END -2 failed");
    verify(compute(0xFFFFFFFFu,1u,9288u,9288u,9288u,result)&&
           result==9287u,"CUR -1 failed");
    const auto before=result;
    verify(!compute(1u,2u,0u,9288u,9288u,result)&&result==before,
           "Source SAVE.DAT seek beyond EOF accepted");
    verify(compute(1u,2u,0u,9288u,
                   rloft::live_level_files::Overlay::capacity,result) &&
           result==9289u,"Writable overlay must allow seek beyond EOF");
    const auto next=result;
    verify(!compute(0x80000000u,0u,0u,9288u,9288u,result) &&
           result==next,"Negative 2^31 absolute displacement accepted");
    verify(!compute(0u,3u,0u,9288u,9288u,result) &&
           result==next,"Unsupported DOS seek origin accepted");
    verify(!compute(0u,2u,0u,9289u,9288u,result) &&
           result==next,"Invalid source length accepted");
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
void test_saved_snapshot_install() {
    using rloft::live_level_files::Overlay;
    // Deterministic allocation-failure injection exercises the exact
    // exception handler used by Overlay::install_saved_snapshot.
    struct NoMemoryMap {
        std::pair<int,bool> try_emplace(std::uint32_t,
                                         std::vector<std::uint8_t>&&) {
            throw std::bad_alloc();
        }
    };
    NoMemoryMap no_memory{};
    std::vector<std::uint8_t> rejected{0x11u,0x22u};
    verify(!Overlay::try_install_into(no_memory,23u,rejected) &&
           rejected==std::vector<std::uint8_t>({0x11u,0x22u}),
           "Allocation failure escaped noexcept snapshot handler or consumed saved source bytes");
    Overlay owner;
    std::vector<std::uint8_t> authentic_fixture{1u,2u,3u,4u};
    verify(owner.install_saved_snapshot(23u,authentic_fixture) &&
           authentic_fixture.empty() && owner.has(23u) && owner.idle(),
           "Prevalidated private saved level was not committed");
    verify(owner.levels.at(23u)==std::vector<std::uint8_t>({1u,2u,3u,4u}),
           "Saved snapshot bytes changed during ownership transfer");
    std::vector<std::uint8_t> duplicate{9u,8u};
    verify(!owner.install_saved_snapshot(23u,duplicate) && owner.failed &&
           duplicate==std::vector<std::uint8_t>({9u,8u}) &&
           owner.levels.at(23u)==std::vector<std::uint8_t>({1u,2u,3u,4u}),
           "Duplicate level overwrote snapshot or consumed source bytes");
    Overlay oversize;
    std::vector<std::uint8_t> too_large(Overlay::capacity+1u,0xA5u);
    verify(!oversize.install_saved_snapshot(23u,too_large) &&
           oversize.failed && oversize.levels.empty() &&
           too_large.size()==Overlay::capacity+1u,
           "Oversized original save was partially installed");
    Overlay busy;
    verify(busy.create(22u)==Overlay::write_handle,
           "Busy fixture failed to open original WORK writer");
    std::vector<std::uint8_t> pending{7u};
    verify(!busy.install_saved_snapshot(23u,pending) &&
           pending==std::vector<std::uint8_t>({7u}) && !busy.has(23u),
           "Saved snapshot was installed over active source writer");
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
        test_filename();test_original_saved_level_policy();
        test_signed_dos_seek_contract();
        test_read_write();test_guards();test_saved_snapshot_install();
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
