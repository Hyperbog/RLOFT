#include "rloft/RES6.hpp"
#include "rloft/resource_assets.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace {
void require(bool value,const char* message) {
    if (!value) throw std::runtime_error(message);
}
template<class F> void rejects(F action) {
    bool rejected=false;
    try {action();} catch(const std::exception&) {rejected=true;}
    require(rejected,"invalid RES6 operation accepted");
}
void le16(std::vector<std::uint8_t>& b,std::size_t at,unsigned value) {
    b.at(at)=static_cast<std::uint8_t>(value);
    b.at(at+1U)=static_cast<std::uint8_t>(value>>8U);
}
void le32(std::vector<std::uint8_t>& b,std::size_t at,unsigned value) {
    for(unsigned i=0U;i<4U;++i)b.at(at+i)=static_cast<std::uint8_t>(value>>(8U*i));
}
std::vector<std::uint8_t> simple_voc() {
    std::vector<std::uint8_t> b(26U,0U);
    constexpr char signature[]="Creative Voice File";
    for(std::size_t i=0;i<sizeof(signature)-1U;++i)
        b[i]=static_cast<std::uint8_t>(signature[i]);
    b[19U]=0x1AU;
    le16(b,20U,26U);
    le16(b,22U,0x010AU);
    le16(b,24U,0x1129U);
    // Three unsigned PCM8 samples (time constant A5, codec 0).
    b.insert(b.end(),{1U,5U,0U,0U,0xA5U,0U,0x80U,0x7FU,0x81U,0U});
    return b;
}
std::vector<std::uint8_t> archive_with(const std::vector<std::uint8_t>& b) {
    std::vector<std::uint8_t> out(8U,0U);
    le32(out,0U,1U); le32(out,4U,8U);
    out.insert(out.end(),{'E','H',4U,0U,0U});
    const auto p=out.size();out.resize(p+4U);
    le32(out,p,static_cast<unsigned>(b.size()));
    out.insert(out.end(),b.begin(),b.end());
    return out;
}
void synthetic() {
    const auto original=simple_voc();
    const auto bytes=archive_with(original);
    rloft::ResArchive archive(bytes);
    require(archive.encode()==bytes,"RES6 synthetic byte-exact round trip");
    const auto parsed=rloft::inspect_res6_voc(original);
    require(parsed.bytes==original.size() && parsed.version==0x10AU &&
            parsed.blocks==1U && parsed.sound_blocks==1U &&
            parsed.continuation_blocks==0U && parsed.padding_bytes==0U,
            "synthetic VOC metadata incorrect");
    require(rloft::decode_res6_voc(archive,0)==original,
            "synthetic EH4 decode differs");
    require(rloft::inspect_res6_archive(archive).standard_sounds==1U,
            "synthetic RES6 inventory incorrect");
    auto edited=original;
    edited[33U]=0x33U;
    edited.insert(edited.end(),{0U,0U,0U});
    rloft::replace_res6_voc(archive,0,edited);
    const rloft::ResArchive readback(archive.encode());
    require(rloft::decode_res6_voc(readback,0)==edited &&
            rloft::inspect_res6_entry(readback,0).padding_bytes==3U,
            "RES6 variable-length VOC edit did not round trip");
    rejects([&]{(void)rloft::decode_res6_voc(archive,-1);});
    rejects([&]{(void)rloft::decode_res6_voc(archive,1);});
    rejects([&]{rloft::replace_res6_voc(archive,0,{});});
    auto invalid=edited;
    invalid[24U]^=1U; // version checksum mismatch
    rejects([&]{rloft::replace_res6_voc(archive,0,invalid);});
    invalid=edited;
    invalid[27U]=0xFFU; // length extends past VOC file
    rejects([&]{rloft::replace_res6_voc(archive,0,invalid);});
    invalid=edited;
    invalid.back()=0x20U; // nonzero after terminating block
    rejects([&]{rloft::replace_res6_voc(archive,0,invalid);});
    invalid=edited;
    invalid.resize(35U); // missing VOC termination block
    rejects([&]{rloft::replace_res6_voc(archive,0,invalid);});
    require(rloft::decode_res6_voc(archive,0)==edited,
            "failed RES6 edit corrupted archive");
}
void original(const std::vector<std::uint8_t>& data) {
    require(data.size()==6210899U,"not original RES6 file size");
    const rloft::ResArchive archive(data);
    require(archive.entry_count()==266 && archive.encode()==data,
            "RES6 original archive did not round trip");
    const auto stats=rloft::inspect_res6_archive(archive);
    require(stats.entries==266U && stats.standard_sounds==265U &&
            stats.new_format_sounds==1U && stats.continuation_blocks==14U &&
            stats.padded_sounds==6U && stats.voc_bytes==6207437U,
            "RES6 original sound inventory differs");
    for(int id=0;id<266;++id) {
        const auto voc=rloft::decode_res6_voc(archive,id);
        const auto info=rloft::inspect_res6_entry(archive,id);
        require(voc.size()==info.bytes && info.sound_blocks==1U,
                "RES6 invalid source sound layout");
    }
    // Edit a real PCM sample and grow its length. The remaining original
    // compressed-free VOC blocks, headers, and archive records are unchanged.
    auto edit=rloft::decode_res6_voc(archive,0);
    edit[32U]^=0x5AU;
    edit.insert(edit.end(),{0U,0U,0U});
    auto changed=archive;
    rloft::replace_res6_voc(changed,0,edit);
    const auto encoded=changed.encode();
    const rloft::ResArchive reloaded(encoded);
    require(encoded!=data && encoded.size()==data.size()+3U &&
            reloaded.entry_count()==266 &&
            rloft::decode_res6_voc(reloaded,0)==edit,
            "RES6 edited original file does not reload");
    for(int id=1;id<266;++id) {
        require(archive.entry(id)==reloaded.entry(id),
                "RES6 edit modified an unrelated original entry");
    }
}
}
int main(int argc,char** argv) {
    synthetic();
    if(argc>2)throw std::invalid_argument("usage: RES6_tests [RES6]");
    if(argc==2){
        std::ifstream f(argv[1],std::ios::binary);
        if(!f)throw std::runtime_error("cannot open original RES6 file");
        const std::vector<std::uint8_t> data(
            (std::istreambuf_iterator<char>(f)),std::istreambuf_iterator<char>());
        original(data);
    }
}
