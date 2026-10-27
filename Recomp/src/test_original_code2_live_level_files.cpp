// The owner-local original 171FC writer and 181B4 reader share this
// bounded private file transport. These tests never contain retail media.
#include "original_code2_live_level_files.hpp"
#include "original_code2_owner_work_snapshot.hpp"
#include "original_code2_saved_level_actor_proof.hpp"
#include "../../Code2/graphics/original_levels_grid.hpp"
#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>
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
void test_original_owner_work_readonly_snapshot() {
    namespace fs=std::filesystem;
    using rloft::original_work_files::Result;
    using rloft::live_level_files::Overlay;
    const auto directory=fs::temp_directory_path()/(
        "rloft_owner_work_source_"+std::to_string(
            std::chrono::steady_clock::now().time_since_epoch().count()));
    fs::create_directories(directory);
    struct Cleanup {
        fs::path path;
        ~Cleanup(){std::error_code ec;fs::remove_all(path,ec);}
    } cleanup{directory};
    std::vector<std::uint8_t> snapshot;
    verify(rloft::original_work_files::snapshot(directory,22u,snapshot)==
           Result::Missing && snapshot.empty(),
           "Missing owner WORK directory must be a DOS file-not-found");
    fs::create_directories(directory/"WORK");
    verify(rloft::original_work_files::snapshot(directory,22u,snapshot)==
           Result::Missing,"Missing saved level did not fall back to archive");
    // Structurally valid original saved-format 1x1 media-free record;
    // no commercial resources and no fake game-party state.
    const std::uint32_t header=0x1Du,body=0x2A4u;
    const std::uint32_t grid_end=header+body+2u;
    const std::uint32_t tail=grid_end+0x514u;
    std::vector<std::uint8_t> original(tail+4u+1u+4u,0u);
    const auto store32=[&](std::size_t pos,std::uint32_t value) {
        for(int j=0;j<4;++j)original.at(pos+j)=
            static_cast<std::uint8_t>((value>>(j*8))&0xFFu);
    };
    const std::uint32_t section[7]{
        header,grid_end,grid_end,grid_end,grid_end,grid_end,tail};
    for(unsigned j=0;j<7;++j)store32(1u+j*4u,section[j]);
    original[header+1u]=1u;original[header+3u]=1u;
    store32(tail+4u+1u,0u);
    const auto file=directory/"WORK"/"LEV022.DAT";
    {
        std::ofstream out(file,std::ios::binary);
        out.write(reinterpret_cast<const char*>(original.data()),
                  static_cast<std::streamsize>(original.size()));
        verify(static_cast<bool>(out),"Failed to create scratch owned WORK");
    }
    verify(rloft::original_work_files::snapshot(directory,22u,snapshot)==
           Result::Loaded && snapshot==original,
           "Owner WORK snapshot was not copied faithfully");
    const auto decoded=rloft::original_levels::decode_saved_grid(snapshot,22u);
    verify(decoded.level==22u && decoded.width==1u && decoded.height==1u,
           "Source saved-world format not preserved");
    Overlay overlay{};
    verify(overlay.install_saved_snapshot(22u,snapshot) && snapshot.empty(),
           "Owner WORK snapshot was not installed privately");
    verify(overlay.open(22u)==Overlay::read_handle,
           "Original 181B4 could not open captured WORK bytes");
    std::vector<std::uint8_t> witness(original.size());
    verify(overlay.read(witness.data(),static_cast<std::uint32_t>(witness.size()))==
           witness.size() && witness==original,
           "Private original WORK source read diverged");
    verify(overlay.close(Overlay::read_handle),
           "Original 181B4 did not close snapshot read handle");
    verify(overlay.erase(22u) && overlay.create(22u)==Overlay::write_handle,
           "Source 171FC could not replace owner WORK in private overlay");
    const std::uint8_t private_byte=0xB7u;
    verify(overlay.write(&private_byte,1u)==1u &&
           overlay.close(Overlay::write_handle),
           "Source 171FC private replacement failed");
    std::ifstream check(file,std::ios::binary|std::ios::ate);
    verify(static_cast<bool>(check) && check.tellg()==
           static_cast<std::streampos>(original.size()),
           "Owner WORK changed size during source write");
    check.seekg(0);
    std::vector<std::uint8_t> unchanged(original.size());
    check.read(reinterpret_cast<char*>(unchanged.data()),
               static_cast<std::streamsize>(unchanged.size()));
    verify(static_cast<bool>(check) && unchanged==original &&
           overlay.levels.at(22u)==std::vector<std::uint8_t>{private_byte},
           "A source save mutated owner WORK or failed to replace overlay");
    check.close();
    fs::remove(file);
    fs::create_directory(file);
    std::vector<std::uint8_t> rejected;
    verify(rloft::original_work_files::snapshot(directory,22u,rejected)==
           Result::Unsupported && rejected.empty(),
           "WORK/LEV directory must never be opened as source level file");
}

void test_saved_actor_branch_proof() {
    // Media-free standalone 1x1 source level with 1 moving and 1 primary
    // 22-byte record, including 17A68 post-load mutable actor bit.
    constexpr std::uint32_t s0=29u,s1=s0+0x2A4u+2u,s2=s1+5u;
    constexpr std::uint32_t s3=s2+22u,s4=s3+2u+22u,s5=s4,s6=s5+0x514u;
    std::vector<std::uint8_t> file(s6+9u,0u);
    const auto w16=[&](std::size_t p,std::uint16_t x) {
        file.at(p)=static_cast<std::uint8_t>(x);
        file.at(p+1u)=static_cast<std::uint8_t>(x>>8u);
    };
    const auto w32=[&](std::size_t p,std::uint32_t x) {
        for(unsigned i=0;i<4u;++i)file.at(p+i)=
            static_cast<std::uint8_t>(x>>(i*8u));
    };
    const std::array<std::uint32_t,7> sec{s0,s1,s2,s3,s4,s5,s6};
    for(unsigned i=0;i<7u;++i)w32(1u+i*4u,sec[i]);
    w16(s0+1u,1u);w16(s0+3u,1u);
    w16(s0+0x28Au,1u);w16(s0+0x28Eu,1u);
    w16(s3,1u);w16(s2+14u,9u);w16(s3+2u+14u,2u);
    w16(s2+6u,0x1234u);w16(s3+2u+6u,0x5678u);
    w32(s6+5u,0u);
    std::vector<std::uint8_t> guest(0x30000u,0u);
    std::copy_n(file.begin()+s2,22u,guest.begin()+0x14D80u);
    std::copy_n(file.begin()+s3+2u,22u,guest.begin()+0x22238u);
    guest[0x14D80u+11u]|=2u;
    const auto pass=rloft::saved_level_actor_proof::verify(file,guest,22u);
    verify(pass.moving==1u && pass.primary==1u &&
           pass.alternate==0u && pass.indexed_columns==0u,
           "Saved 181B4 22-byte actor branch witness failed");
    guest[0x14D80u+6u]^=1u;
    bool rejected=false;
    try{(void)rloft::saved_level_actor_proof::verify(file,guest,22u);}
    catch(const std::runtime_error&){rejected=true;}
    verify(rejected,"Corrupted source saved actor was accepted");
    guest[0x14D80u+6u]^=1u;
    w32(1u+3u*4u,0u);
    rejected=false;
    try{(void)rloft::saved_level_actor_proof::verify(file,guest,22u);}
    catch(const std::runtime_error&){rejected=true;}
    verify(rejected,"Corrupted saved actor section was accepted");
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
        test_original_owner_work_readonly_snapshot();
        test_saved_actor_branch_proof();
        std::cout<<"ORIGINAL_CODE2_171FC_181B4_PRIVATE_LEVEL_FILES_PASS"
                 <<" work_saves=MEMORY_ONLY"
                 <<" original_source_files=UNTOUCHED"
                 <<" owner_WORK_readonly_snapshot=PASS"
                 <<" dos_seek=SIGNED_BOUNDED"
                 <<" truncated_read=REJECTED\n";
        return 0;
    } catch(const std::exception& e) {
        std::cerr<<"ORIGINAL_CODE2_171FC_181B4_PRIVATE_LEVEL_FILES_FAIL "
                 <<e.what()<<'\n';
        return 1;
    }
}
