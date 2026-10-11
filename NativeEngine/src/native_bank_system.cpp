#include "rloft_native/native_bank_system.hpp"

#include "rloft_native/native_assets.hpp"
#include "rloft_native/native_lzss.hpp"
#include "rloft_native/native_type6_texture.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace rloft_native {
namespace {

std::uint16_t u16(const std::uint8_t* p) {
    return std::uint16_t(p[0]) | (std::uint16_t(p[1]) << 8);
}
std::int16_t s16(const std::uint8_t* p) {
    return static_cast<std::int16_t>(u16(p));
}
std::uint32_t u32(const std::uint8_t* p) {
    return std::uint32_t(p[0]) | (std::uint32_t(p[1]) << 8) |
           (std::uint32_t(p[2]) << 16) | (std::uint32_t(p[3]) << 24);
}

void require(bool ok, const char* reason) {
    if (!ok) throw std::runtime_error(reason);
}

bool legacy_voc_length_anomaly(std::string_view archive, std::size_t index,
                               std::uint32_t length, std::size_t actual) {
    return ((archive=="RES11" && index==4877) ||
            (archive=="RES12" && index==430)) &&
           length==0x11669 && actual==0x1669;
}

// EH7 stores compiled DRAW DESCRIPTORS, not ordinary pixels. Validate every
// column pointer and every terminated microcode selection list, without
// executing legacy x86 code from SINTAB or trying to invent pixel colors.
void verify_shape7(const std::vector<std::uint8_t>& decoded) {
    require(decoded.size()>=10,"EH7 geometry header truncated");
    const auto* p=decoded.data();
    const unsigned width=u16(p);
    const int x0=s16(p+2), x1=s16(p+4);
    const int y0=s16(p+6), y1=s16(p+8);
    require((width==64 || width==96 || width==128) &&
            x0>=0 && x0<=x1 && x1<static_cast<int>(width) &&
            y0>=0 && y0<=y1 && y1<=128,
            "EH7 shape source bounds invalid");
    const auto columns=std::size_t(x1-x0+1);
    const auto table_end=10+2*columns;
    require(table_end<=decoded.size(),"EH7 column table truncated");
    for (std::size_t col=0;col<columns;++col) {
        auto pos=std::size_t(u16(p+10+2*col));
        require(pos>=table_end && pos+2<=decoded.size(),
                "EH7 compiled column list pointer invalid");
        bool found_end=false;
        while (pos+2<=decoded.size()) {
            const auto stop=u16(p+pos);
            pos+=2;
            if (stop==0) {found_end=true;break;}
            require(pos+4<=decoded.size(),
                    "EH7 column descriptor truncated");
            const auto selector=u16(p+pos+2);
            require(stop<=0x100 && (stop%2)==0 &&
                    selector<=0x100 && (selector%2)==0,
                    "EH7 references unknown SINTAB selector");
            pos+=4;
        }
        require(found_end,"EH7 column list missing zero terminator");
    }
}

void verify_image(const std::vector<std::uint8_t>& e, bool font) {
    const auto validate_exact = [](const std::vector<std::uint8_t>& raster) {
        require(raster.size()>=13,"EH1 shared payload envelope truncated");
        const int frames=s16(raster.data()+5), rows=s16(raster.data()+7);
        require(frames>0 && rows>0 && raster[9]>0,
                "EH1 invalid image geometry");
        const auto decoded=std::size_t(frames)*std::size_t(rows)*
                           std::size_t(raster[9])*8U;
        if (raster[11]==3) {
            const auto result=native_lzss4k_decode(
                raster.data()+13,raster.size()-13,decoded);
            require(result.consumed==raster.size()-13,
                    "EH1 LZSS original compressed stream has trailing bytes");
        } else if (raster[11]==0) {
            require(raster.size()-13==decoded,
                    "EH1 raw compressed stream has wrong length");
        } else {
            throw std::runtime_error("unsupported original EH1 method");
        }
    };
    if (!font) {
        (void)decode_native_type1_raster(e);
        validate_exact(e);
        return;
    }
    require(e.size()>=141 && e[3]==0x88 && e[4]==0,
            "EH2 font width/descriptor header malformed");
    // Original EH2 uses 128 glyph widths and a type-1-compatible
    // raster descriptor at 5+128; reconstruct without decomp runtime.
    std::vector<std::uint8_t> EH1{'E','H',1,8,0};
    EH1.insert(EH1.end(),e.begin()+133,e.begin()+141);
    EH1.insert(EH1.end(),e.begin()+141,e.end());
    const auto image=decode_native_type1_raster(EH1);
    validate_exact(EH1);
    require(image.frames==128 && image.width==8,
            "EH2 font glyph count or width inconsistent");
}

void verify_wordcode(const std::vector<std::uint8_t>& e) {
    require(e.size()>=2,"raw interaction wordcode too short");
    const auto words=u16(e.data());
    const auto expected=2ULL+4ULL*words;
    require(expected==e.size(),"raw interaction wordcode size invalid");
    // The inactive companion bank is retained intact but must be
    // zero in the supplied CODE.2 original runtime generation.
    for(std::size_t i=2+2*std::size_t(words);i<e.size();++i)
        require(e[i]==0,"raw interaction wordcode companion bank not zero");
}

std::string quote_json(const std::string& s) {
    std::string out{"\""};
    for (unsigned char c:s) {
        switch(c) {
            case '"': out+="\\\"";break;
            case '\\':out+="\\\\";break;
            case '\n':out+="\\n";break;
            case '\r':out+="\\r";break;
            case '\t':out+="\\t";break;
            default:
                if(c<32)out+="?";
                else out+=static_cast<char>(c);
        }
    }
    out+="\"";
    return out;
}

} // namespace

BankRecordKind validate_native_bank_record(
    const std::vector<std::uint8_t>& e,
    std::string_view archive, std::size_t index, bool& anomaly) {
    anomaly=false;
    if(e.empty())return BankRecordKind::Empty;

    if(e.size()<5 || e[0]!='E' || e[1]!='H') {
        if(archive=="RES1" && e.size()==4 && u32(e.data())==0x60)
            return BankRecordKind::NonEh;
        if(archive=="RES9" || archive=="RES12") {
            verify_wordcode(e);
            return BankRecordKind::NonEh;
        }
        throw std::runtime_error("unclassified original non-EH record");
    }

    const unsigned type=e[2];
    const auto signed_header=s16(e.data()+3);
    require(signed_header>=0,"negative signed original EH header span");
    require(std::size_t(5)+std::size_t(signed_header)<=e.size(),
            "original EH type header extends beyond record");
    switch(type) {
        case 1:
            require(signed_header==8,"EH1 descriptor span not 8");
            verify_image(e,false);
            return BankRecordKind::Raster;
        case 2:
            require(signed_header==136,"EH2 font descriptor span not 136");
            verify_image(e,true);
            return BankRecordKind::Font;
        case 3:
        case 4: {
            require(signed_header==0 && e.size()>=9,
                    "EH music/VOC envelope malformed");
            const auto declared=u32(e.data()+5);
            const auto actual=e.size()-9;
            if(type==3) {
                require(declared==actual,"EH3 XDIR blob declared size differs");
                require(actual>=12 &&
                        e[9]=='F' && e[10]=='O' && e[11]=='R' && e[12]=='M' &&
                        e[17]=='X' && e[18]=='D' && e[19]=='I' && e[20]=='R',
                        "EH3 original XMIDI/XDIR signature missing");
                return BankRecordKind::Xmidi;
            }
            static constexpr char signature[]="Creative Voice File";
            require(actual>=sizeof(signature) &&
                    std::equal(signature,signature+sizeof(signature)-1,
                               e.data()+9) &&
                    e[9+sizeof(signature)-1]==0x1A,
                    "EH4 original Creative Voice File signature missing");
            if(declared!=actual) {
                if(legacy_voc_length_anomaly(archive,index,declared,actual))
                    anomaly=true;
                else throw std::runtime_error(
                    "EH4 VOC declared length does not match original bytes");
            }
            return BankRecordKind::Voc;
        }
        case 5:
            // No supplied Ravenloft EH5 specimens. Recognize its
            // physical loader class without inventing field semantics.
            require(signed_header>=28,"EH5 executable-only header too short");
            return BankRecordKind::Structured5;
        case 6: {
            require(signed_header==0 && e.size()>=8,
                    "EH6 world texture envelope truncated");
            (void)decode_native_type6_texture(e);
            const auto result=native_lzss4k_decode(
                e.data()+7,e.size()-7,u16(e.data()+5));
            require(result.consumed==e.size()-7,
                    "EH6 LZSS compressed stream has trailing bytes");
            return BankRecordKind::Texture;
        }
        case 7: {
            require(signed_header==0 && e.size()>=8,
                    "EH7 compiled scene envelope truncated");
            const auto decoded_bytes=u16(e.data()+5);
            require(decoded_bytes>=10,
                    "EH7 geometry bytes too short");
            const auto decoded=native_lzss4k_decode(
                e.data()+7,e.size()-7,decoded_bytes);
            require(decoded.consumed==e.size()-7,
                    "EH7 LZSS original compressed stream has trailing bytes");
            verify_shape7(decoded.bytes);
            return BankRecordKind::SceneShape;
        }
        default:
            throw std::runtime_error("unknown original EH resource type");
    }
}

NativeBankSystemReport audit_native_bank_system(
    const NativeMedia& media, bool include_legacy) {
    NativeBankSystemReport report;
    report.legacy_included=include_legacy;
    const unsigned bank_count=include_legacy?13U:11U;
    for(unsigned bank=0;bank<bank_count;++bank) {
        NativeBankSummary summary;
        summary.archive="RES"+std::to_string(bank);
        if(!media.contains(summary.archive)) {
            ++report.absent_banks;
            report.issues.push_back({summary.archive,0,
                                     "original archive missing"});
            report.banks.push_back(std::move(summary));
            continue;
        }
        try {
            // Eagerly index just ONE source archive per pass; original files
            // stay read only, and decoded assets are discarded per record.
            const auto raw=media.read(summary.archive);
            summary.original_bytes=raw.size();
            NativeResArchive archive(raw);
            summary.entry_count=archive.entry_count();
            report.records+=summary.entry_count;
            for(std::size_t record=0;record<archive.entry_count();++record) {
                const auto bytes=archive.entry(record);
                const unsigned type=bytes.size()>=3 && bytes[0]=='E' &&
                    bytes[1]=='H' ? unsigned(bytes[2]):0U;
                if(type>=1 && type<=7) {
                    ++summary.eh_types[type];
                    ++report.recognized_eh;
                }
                bool shipped_anomaly=false;
                try {
                    const auto kind=validate_native_bank_record(
                        bytes,summary.archive,record,shipped_anomaly);
                    ++summary.validated;
                    ++report.validated;
                    if(shipped_anomaly)++summary.retained_shipped_anomalies;
                    switch(kind) {
                        case BankRecordKind::Empty:++summary.empty;break;
                        case BankRecordKind::NonEh:
                            ++summary.non_eh;
                            if(bank==9 || bank==12)++summary.raw_wordcode;
                            break;
                        case BankRecordKind::Raster:++summary.decoded_raster;break;
                        case BankRecordKind::Font:++summary.decoded_font;break;
                        case BankRecordKind::Xmidi:++summary.decoded_music;break;
                        case BankRecordKind::Voc:++summary.decoded_sound;break;
                        case BankRecordKind::Texture:++summary.decoded_texture;break;
                        case BankRecordKind::SceneShape:
                            ++summary.decoded_scene_shape;break;
                        default:break;
                    }
                } catch(const std::exception& err) {
                    ++summary.failed;
                    ++report.failed;
                    // Retain per-record diagnostic identity; bounded to
                    // avoid huge reports on adversarial/corrupted media.
                    if(report.issues.size()<256)
                        report.issues.push_back(
                            {summary.archive,record,err.what()});
                }
            }
        } catch(const std::exception& err) {
            ++summary.failed;
            ++report.failed;
            if(report.issues.size()<256)
                report.issues.push_back({summary.archive,0,err.what()});
        }
        report.banks.push_back(std::move(summary));
    }
    return report;
}

std::string native_bank_system_json(const NativeBankSystemReport& report) {
    std::ostringstream out;
    out<<"{\"schema\":\"rloft-native-bank-audit/v1\","
       <<"\"source_original_assets_included\":false,"
       <<"\"legacy_included\":"<<(report.legacy_included?"true":"false")
       <<",\"records\":"<<report.records
       <<",\"validated\":"<<report.validated
       <<",\"failed\":"<<report.failed
       <<",\"absent_banks\":"<<report.absent_banks
       <<",\"recognized_eh\":"<<report.recognized_eh<<",\"banks\":[";
    bool first=true;
    for(const auto& b:report.banks) {
        if(!first)out<<",";
        first=false;
        out<<"{\"name\":"<<quote_json(b.archive)
           <<",\"bytes\":"<<b.original_bytes
           <<",\"records\":"<<b.entry_count
           <<",\"validated\":"<<b.validated
           <<",\"failed\":"<<b.failed
           <<",\"empty\":"<<b.empty
           <<",\"non_eh\":"<<b.non_eh
           <<",\"raw_wordcode\":"<<b.raw_wordcode
           <<",\"image_eh1\":"<<b.decoded_raster
           <<",\"font_eh2\":"<<b.decoded_font
           <<",\"xmidi_eh3\":"<<b.decoded_music
           <<",\"voc_eh4\":"<<b.decoded_sound
           <<",\"texture_eh6\":"<<b.decoded_texture
           <<",\"shape_eh7\":"<<b.decoded_scene_shape
           <<",\"shipped_anomalies\":"<<b.retained_shipped_anomalies
           <<",\"types\":{";
        for(unsigned t=1;t<=7;++t) {
            if(t>1)out<<",";
            out<<quote_json(std::to_string(t))<<":"<<b.eh_types[t];
        }
        out<<"}}";
    }
    out<<"],\"issues\":[";
    for(std::size_t i=0;i<report.issues.size();++i) {
        const auto& issue=report.issues[i];
        if(i)out<<",";
        out<<"{\"bank\":"<<quote_json(issue.archive)
           <<",\"index\":"<<issue.record
           <<",\"error\":"<<quote_json(issue.reason)<<"}";
    }
    out<<"]}\n";
    return out.str();
}

} // namespace rloft_native
