#include "rloft_native/native_session_profile.hpp"
#include "rloft_native/native_sha256.hpp"

#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using namespace rloft_native;
namespace {
template<class F> void rejects(F&& fn) {
    bool thrown=false;
    try{fn();}catch(const std::exception&){thrown=true;}
    assert(thrown);
}
NativeWorldRuntime original_fixture(bool variant=false) {
    NativeWorldPayload source;
    for(unsigned id=0;id<32U;++id) {
        NativeLevelPayload level;
        level.id=id;
        level.width=8;
        level.height=8;
        level.cell_words.assign(64U,0);
        level.automap.assign(64U,0);
        if(id==22U) {
            level.cell_words[2U*8U+3U]=1;
            if(variant)level.cell_words[4U*8U+7U]=1;
        }
        source.levels.push_back(std::move(level));
        source.total_cells+=64U;
    }
    return NativeWorldRuntime(std::move(source));
}
NativeSessionProfile original_profile(const NativeWorldRuntime& world) {
    NativeSessionProfile p;
    p.original_world_sha256=world.snapshot().source_fingerprint_sha256;
    const std::string names[]={"ALPHA","BETA"};
    for(unsigned i=0;i<2;++i) {
        const auto base=i*0x248U;
        for(std::size_t j=0;j<names[i].size();++j)
            p.chars.at(base+j)=static_cast<std::uint8_t>(names[i][j]);
        // Accepted, race0, sex0, class0, alignment0, current/max HP 5.
        p.chars.at(base+0x1CU)=5;
        p.chars.at(base+0x1EU)=5;
    }
    p.level=22;
    p.column=4.5;
    p.row=4.5;
    p.heading_radians=-1.5707963267948966;
    p.pitch_radians=0.25;
    p.discovered_cells.assign(8U*8U,0);
    p.discovered_cells[4U*8U+4U]=1;
    p.discovered_cells[4U*8U+5U]=1;
    return p;
}
void integrity(std::vector<std::uint8_t>& bytes) {
    // Only for the synthetic checksum-verification tests below.
    bytes.resize(bytes.size()-64U);
    const auto sum=native_sha256_hex(bytes);
    bytes.insert(bytes.end(),sum.begin(),sum.end());
}
} // namespace
int main() {
    auto world=original_fixture();
    const auto before=world.snapshot();
    const auto source=original_profile(world);
    const auto bytes=encode_native_session_profile(source,world);
    assert(bytes.size()==8U+4U+64U+0x920U+4U+32U+4U+64U+64U);
    const auto restored=decode_native_session_profile(bytes,world);
    assert(restored.chars==source.chars);
    assert(restored.original_world_sha256==source.original_world_sha256);
    assert(restored.level==22 && restored.column==4.5 && restored.row==4.5);
    assert(restored.heading_radians==source.heading_radians);
    assert(restored.pitch_radians==source.pitch_radians);
    assert(restored.discovered_cells==source.discovered_cells);
    assert(encode_native_session_profile(restored,world)==bytes);
    // Restoration never mutates original level, cells, party or marks.
    assert(world.snapshot().source_fingerprint_sha256==
           before.source_fingerprint_sha256);
    assert(world.cell(22,3,2).code==1);

    auto tampered=bytes;
    tampered[78U]^=0x01U;
    rejects([&]{static_cast<void>(decode_native_session_profile(tampered,world));});
    tampered=bytes;
    tampered[0]='X';
    rejects([&]{static_cast<void>(decode_native_session_profile(tampered,world));});
    tampered=bytes;
    tampered.pop_back();
    rejects([&]{static_cast<void>(decode_native_session_profile(tampered,world));});
    tampered=bytes;
    tampered[8]=2; // Recomputed SHA cannot bypass format version check.
    integrity(tampered);
    rejects([&]{static_cast<void>(decode_native_session_profile(tampered,world));});

    auto invalid=source;
    invalid.discovered_cells[0]=2;
    rejects([&]{static_cast<void>(encode_native_session_profile(invalid,world));});
    invalid=source;
    invalid.level=23;
    rejects([&]{static_cast<void>(encode_native_session_profile(invalid,world));});
    invalid=source;
    invalid.column=3.5;
    invalid.row=2.5; // Actual blocked source cell 3,2.
    rejects([&]{static_cast<void>(encode_native_session_profile(invalid,world));});
    invalid=source;
    invalid.heading_radians=100.0;
    rejects([&]{static_cast<void>(encode_native_session_profile(invalid,world));});
    invalid=source;
    invalid.pitch_radians=std::nan("");
    rejects([&]{static_cast<void>(encode_native_session_profile(invalid,world));});
    invalid=source;
    invalid.chars[0]='?';
    rejects([&]{static_cast<void>(encode_native_session_profile(invalid,world));});
    // Same actors, different original LEVELS means an unrelated campaign.
    const auto unrelated=original_fixture(true);
    rejects([&]{static_cast<void>(decode_native_session_profile(bytes,unrelated));});
    assert(world.active_level()==0);
}
