#include "rloft_native/native_bank_system.hpp"
#include "rloft_native/native_assets.hpp"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace fs = std::filesystem;
using namespace rloft_native;
namespace {

void p16(std::vector<std::uint8_t>& b,std::size_t offset,unsigned value) {
    b.at(offset)=std::uint8_t(value);
    b.at(offset+1)=std::uint8_t(value>>8);
}
void p32(std::vector<std::uint8_t>& b,std::size_t offset,unsigned value) {
    for(unsigned i=0;i<4;++i)b.at(offset+i)=std::uint8_t(value>>(8*i));
}
std::vector<std::uint8_t> pack(const std::vector<std::vector<std::uint8_t>>& records) {
    std::vector<std::uint8_t> result(4+4*records.size(),0);
    p32(result,0,static_cast<unsigned>(records.size()));
    for(std::size_t i=0;i<records.size();++i) {
        p32(result,4+4*i,static_cast<unsigned>(result.size()));
        result.insert(result.end(),records[i].begin(),records[i].end());
    }
    return result;
}
void save(const fs::path& path,const std::vector<std::uint8_t>& bytes) {
    std::ofstream output(path,std::ios::binary|std::ios::trunc);
    if(!output) throw std::runtime_error("fixture cannot open file");
    if(!bytes.empty())
        output.write(reinterpret_cast<const char*>(bytes.data()),
                     static_cast<std::streamsize>(bytes.size()));
    if(!output)throw std::runtime_error("fixture write failed");
}
std::vector<std::uint8_t> literal(const std::vector<std::uint8_t>& data) {
    std::vector<std::uint8_t> encoded;
    for(std::size_t i=0;i<data.size();i+=8) {
        encoded.push_back(0xFF);
        const auto end=std::min(data.size(),i+8);
        encoded.insert(encoded.end(),data.begin()+i,data.begin()+end);
    }
    return encoded;
}
std::vector<std::uint8_t> eh1() {
    std::vector<std::uint8_t> r{
        'E','H',1,8,0,1,0,1,0,1,255,3,0
    };
    const std::vector<std::uint8_t> pixels{0,1,2,3,4,5,6,7};
    const auto comp=literal(pixels);
    r.insert(r.end(),comp.begin(),comp.end());
    return r;
}
std::vector<std::uint8_t> eh2() {
    std::vector<std::uint8_t> r(141,0);
    r[0]='E';r[1]='H';r[2]=2;
    p16(r,3,136);
    for(int i=0;i<128;++i)r[5+i]=8;
    p16(r,133,128);p16(r,135,1);
    r[137]=1; r[138]=255; r[139]=3;
    std::vector<std::uint8_t> pixels(1024,0x7B);
    const auto compressed=literal(pixels);
    r.insert(r.end(),compressed.begin(),compressed.end());
    return r;
}
std::vector<std::uint8_t> eh6() {
    std::vector<std::uint8_t> r{'E','H',6,0,0,0,16};
    std::vector<std::uint8_t> source(4096,7);
    const auto compressed=literal(source);
    r.insert(r.end(),compressed.begin(),compressed.end());
    return r;
}
std::vector<std::uint8_t> eh7() {
    // One compiled 64-wide source column with an empty terminated list.
    std::vector<std::uint8_t> decoded(14,0);
    p16(decoded,0,64);p16(decoded,2,0);p16(decoded,4,0);
    p16(decoded,6,0);p16(decoded,8,128);
    p16(decoded,10,12);p16(decoded,12,0);
    std::vector<std::uint8_t> r{'E','H',7,0,0,14,0};
    const auto compressed=literal(decoded);
    r.insert(r.end(),compressed.begin(),compressed.end());
    return r;
}
std::vector<std::uint8_t> eh3() {
    const std::vector<std::uint8_t> blob{
        'F','O','R','M',0,0,0,4,'X','D','I','R'};
    std::vector<std::uint8_t> r{'E','H',3,0,0,12,0,0,0};
    r.insert(r.end(),blob.begin(),blob.end());
    return r;
}
std::vector<std::uint8_t> eh4(bool original_anomaly=false) {
    static const std::string signature="Creative Voice File";
    std::vector<std::uint8_t> blob(signature.begin(),signature.end());
    blob.push_back(0x1A);
    if(original_anomaly)blob.resize(0x1669);
    const unsigned declared=original_anomaly?0x11669:
                            static_cast<unsigned>(blob.size());
    std::vector<std::uint8_t> r(9,0);
    r[0]='E';r[1]='H';r[2]=4;
    p32(r,5,declared);
    r.insert(r.end(),blob.begin(),blob.end());
    return r;
}
template<class Fn> void rejects(Fn fn) {
    bool yes=false;
    try{fn();}catch(const std::exception&){yes=true;}
    assert(yes);
}
void run() {
    const auto r1=eh1(),r2=eh2(),r3=eh3(),r4=eh4(),
               r6=eh6(),r7=eh7();
    for(const auto& example:{r1,r2,r3,r4,r6,r7}) {
        bool anomaly=false;
        const auto kind=validate_native_bank_record(example,"RES0",0,anomaly);
        assert(kind!=BankRecordKind::UnknownTyped && !anomaly);
    }
    bool anomaly=false;
    assert(validate_native_bank_record({}, "RES1",0,anomaly)==
           BankRecordKind::Empty);
    assert(validate_native_bank_record({0x60,0,0,0}, "RES1",381,anomaly)==
           BankRecordKind::NonEh);
    assert(validate_native_bank_record({1,0,2,0,0,0}, "RES9",517,anomaly)==
           BankRecordKind::NonEh);
    rejects([&] {static_cast<void>(validate_native_bank_record(
        {1,0,2,0,0}, "RES9",517,anomaly));});
    rejects([&] {static_cast<void>(validate_native_bank_record(
        {0x60,0,0,0}, "RES0",12,anomaly));});
    auto broken=r7;
    broken[2]=8;
    rejects([&] {static_cast<void>(validate_native_bank_record(
        broken,"RES3",0,anomaly));});
    broken=r7;broken.back()=1;
    rejects([&] {static_cast<void>(validate_native_bank_record(
        broken,"RES3",0,anomaly));});
    broken=r6;broken.push_back(0);
    rejects([&] {static_cast<void>(validate_native_bank_record(
        broken,"RES4",0,anomaly));});
    broken=r1;broken.push_back(0);
    rejects([&] {static_cast<void>(validate_native_bank_record(
        broken,"RES0",0,anomaly));});
    // Five original RES12 legacy EH1 slots represent ZERO decoded pixels,
    // not malformed active images. Keep inert and reject nonempty payload.
    auto inert=eh1();
    p16(inert,5,0);
    inert.resize(13);
    assert(validate_native_bank_record(
        inert,"RES12",74,anomaly)==BankRecordKind::InertLegacyRaster);
    rejects([&] {static_cast<void>(validate_native_bank_record(
        inert,"RES0",74,anomaly));});
    inert.push_back(0x00);
    rejects([&] {static_cast<void>(validate_native_bank_record(
        inert,"RES12",74,anomaly));});
    const auto legacy=eh4(true);
    assert(validate_native_bank_record(
        legacy,"RES11",4877,anomaly)==BankRecordKind::Voc && anomaly);
    assert(validate_native_bank_record(
        legacy,"RES12",430,anomaly)==BankRecordKind::Voc && anomaly);
    rejects([&] {static_cast<void>(validate_native_bank_record(
        legacy,"RES10",0,anomaly));});

    const auto root=fs::temp_directory_path()/
        ("rloft-complete-native-banks-"+std::to_string(
           std::chrono::steady_clock::now().time_since_epoch().count()));
    fs::create_directories(root);
    try {
        save(root/"INIT",{0,0});
        save(root/"T",{1,2});
        save(root/"RES0",pack({r2,r1}));
        save(root/"RES1",pack({{},r1,{0x60,0,0,0}}));
        save(root/"RES2",pack({r1}));
        save(root/"RES3",pack({r7,r7}));
        save(root/"RES4",pack({r6,r6}));
        save(root/"RES5",pack({r3,r3}));
        save(root/"RES6",pack({r4}));
        save(root/"RES7",pack({r1}));
        save(root/"RES8",pack({r1}));
        save(root/"RES9",pack({r1,{1,0,2,0,0,0}}));
        save(root/"RES10",pack({r4}));
        save(root/"RES11",pack({r7,r6,r3,r4}));
        save(root/"RES12",pack({r1,{1,0,2,0,0,0}}));
        const auto source=NativeMedia::open(root);
        const auto full=audit_native_bank_system(source,true);
        assert(full.banks.size()==13 && full.failed==0);
        assert(full.absent_banks==0 && full.records==24);
        assert(full.validated==full.records);
        assert(full.banks[0].decoded_font==1);
        assert(full.banks[3].decoded_scene_shape==2);
        assert(full.banks[4].decoded_texture==2);
        assert(full.banks[5].decoded_music==2);
        assert(full.banks[6].decoded_sound==1);
        assert(full.banks[1].empty==1 && full.banks[1].non_eh==1);
        assert(full.banks[9].raw_wordcode==1);
        const auto encoded=native_bank_system_json(full);
        assert(encoded.find("\"schema\":\"rloft-native-bank-audit/v1\"")!=
               std::string::npos);
        assert(encoded.find("\"failed\":0")!=std::string::npos);
        assert(encoded.find("\"name\":\"RES12\"")!=std::string::npos);
        const auto active=audit_native_bank_system(source,false);
        assert(active.banks.size()==11 && active.absent_banks==0);
        assert(active.records==18);
        fs::remove(root/"RES6");
        const auto incomplete=audit_native_bank_system(
            NativeMedia::open(root),false);
        assert(incomplete.absent_banks==1);
        assert(!incomplete.issues.empty());
        assert(incomplete.banks[6].entry_count==0);
    } catch(...) {fs::remove_all(root);throw;}
    fs::remove_all(root);
}
}
int main(){run();}
