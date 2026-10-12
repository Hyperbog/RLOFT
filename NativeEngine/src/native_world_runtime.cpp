#include "rloft_native/native_world_runtime.hpp"
#include "rloft_native/native_sha256.hpp"

#include <cstdint>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft_native {
namespace {
void require(bool success,const char* why) {
    if(!success)throw std::runtime_error(why);
}
std::int8_t signed_delta(std::uint8_t value) {
    return static_cast<std::int8_t>(value);
}
void encode16(std::vector<std::uint8_t>& b,std::size_t n) {
    require(n<=65535U,"world source record length too large");
    b.push_back(static_cast<std::uint8_t>(n));
    b.push_back(static_cast<std::uint8_t>(n>>8U));
}
void encode32(std::vector<std::uint8_t>& b,std::size_t n) {
    require(n<=0xFFFFFFFFULL,"world source archive length too large");
    for(unsigned i=0;i<4;++i)
        b.push_back(static_cast<std::uint8_t>(n>>(8U*i)));
}
void bytes(std::vector<std::uint8_t>& target,
           const std::vector<std::uint8_t>& data) {
    encode32(target,data.size());
    target.insert(target.end(),data.begin(),data.end());
}
bool valid_control(const NativeOriginalControl& item,unsigned width,
                   unsigned height) {
    if(item.column>=width || item.bytes[5]!=item.column ||
       item.bytes[4]>=height)return false;
    if(item.bytes[0]!=1)return true;
    return item.bytes[6]<=8 && signed_delta(item.bytes[7])>=-1 &&
           signed_delta(item.bytes[7])<=1;
}
}

NativeWorldRuntime::NativeWorldRuntime(NativeWorldPayload original)
    :original_(std::move(original)),
     mutable_maps_(clone_native_mutable_world(original_)) {
    require(original_.levels.size()==32 &&
            mutable_maps_.levels.size()==32,
            "live native world requires 32 source levels");
    mutable_controls_.reserve(32);
    for(unsigned id=0;id<32;++id) {
        const auto& level=original_.level(id);
        require(level.width>0 && level.height>0 &&
                level.cell_words.size()==std::size_t(level.width)*level.height &&
                level.automap.size()==level.cell_words.size(),
                "live world requires complete source cell and automap grid");
        for(const auto& record:level.controls)
            require(valid_control(record,level.width,level.height),
                    "source ten-byte control coordinate/state invalid");
        mutable_controls_.push_back(level.controls);
    }
}

const NativeMutableLevel& NativeWorldRuntime::map(unsigned id) const {
    require(id<mutable_maps_.levels.size() &&
            mutable_maps_.levels[id].id==id,
            "live world level identifier outside original archive");
    return mutable_maps_.levels[id];
}
NativeMutableLevel& NativeWorldRuntime::map(unsigned id) {
    return const_cast<NativeMutableLevel&>(
        static_cast<const NativeWorldRuntime&>(*this).map(id));
}
std::size_t NativeWorldRuntime::index(
    unsigned level,unsigned column,unsigned row) const {
    const auto& m=map(level);
    if(column>=m.width || row>=m.height)
        throw std::out_of_range("native world cell coordinate outside map");
    return std::size_t(row)*m.width+column;
}
void NativeWorldRuntime::select_level(unsigned level) {
    static_cast<void>(map(level));
    active_level_=level;
}

NativeSceneCell NativeWorldRuntime::cell(
    unsigned level,unsigned column,unsigned row) const {
    const auto pos=index(level,column,row);
    const auto word=map(level).cell_words.at(pos);
    NativeSceneCell result;
    result.level=level;result.column=column;result.row=row;
    result.word=word;
    result.code=static_cast<std::uint8_t>(word&kCellCode);
    result.shade_decrement=static_cast<std::uint8_t>((word&kCellShade)>>14U);
    result.transient_visible=(word&kCellCurrentVisibility)!=0;
    result.explored=(word&kCellExplored)!=0;
    result.special_center=(word&kCellSpecialCenter)!=0;
    result.special_frame=(word&kCellSpecialFrame)!=0;
    result.extra_render_pass=(word&kCellExtraRenderPass)!=0;
    result.hidden_topology=(word&kCellHiddenTopology)!=0;
    result.authoring_marker=(word&kCellLegacyAuthoring)!=0;
    for(const auto& record:mutable_controls_.at(level)) {
        if(record.bytes[4]!=row || record.bytes[5]!=column)continue;
        if(record.bytes[0]==1 && result.dynamic_position<0)
            result.dynamic_position=record.bytes[6];
        if(record.bytes[0]!=1 && result.resource_variant_offset<0 &&
           (word&kCellStateResource)!=0)
            result.resource_variant_offset=record.bytes[1]&0x03U;
    }
    // This implements only the CODE.2 ordinary collision/1E930 gate.
    // Actor occupancy, secret-mode traversal, footprints are separate.
    if(result.special_center)
        result.normal_collision=result.dynamic_position==0?
            NativeCollision::Passable:NativeCollision::SpecialBlocked;
    else if(result.code!=0)
        result.normal_collision=NativeCollision::OrdinaryBlocked;
    else
        result.normal_collision=NativeCollision::Passable;
    return result;
}
NativeCollision NativeWorldRuntime::normal_collision(
    unsigned level,unsigned column,unsigned row) const {
    return cell(level,column,row).normal_collision;
}
void NativeWorldRuntime::replace_cell_code(
    unsigned level,unsigned column,unsigned row,std::uint8_t six_bit_code) {
    require(six_bit_code<=63U,"native world cell code exceeds six-bit field");
    auto& m=map(level);
    auto& word=m.cell_words.at(index(level,column,row));
    word=static_cast<std::uint16_t>(
        (word&~kCellCode)|six_bit_code);
}
void NativeWorldRuntime::mark_reached_by_view(
    unsigned level,unsigned column,unsigned row) {
    auto& m=map(level);
    m.cell_words.at(index(level,column,row))|=
        (kCellCurrentVisibility|kCellExplored);
}
void NativeWorldRuntime::clear_transient_visibility(unsigned level) {
    auto& m=map(level);
    for(auto& word:m.cell_words)
        word=static_cast<std::uint16_t>(word&~kCellCurrentVisibility);
}

void NativeWorldRuntime::debug_begin_dynamic_transition(
    unsigned level,std::size_t control_index) {
    static_cast<void>(map(level));
    auto& record=mutable_controls_.at(level).at(control_index).bytes;
    require(record[0]==1,"debug dynamic transition requires kind-1 record");
    require(signed_delta(record[7])==0,"dynamic transition already active");
    require(record[6]==0 || record[6]==8,
            "debug dynamic transition requires stable endpoint");
    // This is explicit debug setup. Original interaction eligibility,
    // placement, sound and script dispatch are NOT implemented here.
    record[7]=record[6]==0?std::uint8_t(1):std::uint8_t(255);
}

std::size_t NativeWorldRuntime::advance_dynamic_animations(unsigned level) {
    static_cast<void>(map(level));
    auto& controls=mutable_controls_.at(level);
    std::size_t running=0;
    for(auto& control:controls) {
        auto& record=control.bytes;
        if(record[0]!=1)continue;
        const auto delta=signed_delta(record[7]);
        require(delta>=-1 && delta<=1 && record[6]<=8,
                "dynamic position/animation corrupted");
        if(delta==0)continue;
        const auto next=int(record[6])+int(delta);
        require(next>=0 && next<=8,
                "dynamic position advance would overflow endpoint");
        record[6]=static_cast<std::uint8_t>(next);
        if(next==0 || next==8)record[7]=0;
        else ++running;
    }
    // CODE.2 cleanup normalizes all non-8 stationary group-2 records to 0.
    if(running==0) {
        for(auto& control:controls) {
            auto& record=control.bytes;
            if(record[0]==1 && record[6]!=8)record[6]=0;
        }
    }
    return running;
}

std::string NativeWorldRuntime::source_fingerprint() const {
    std::vector<std::uint8_t> buffer;
    // Domain-separated, length-prefixed hash over immutable original payload.
    static constexpr char domain[]="RLOFT-native-original-world-source-v1";
    buffer.insert(buffer.end(),domain,domain+sizeof(domain)-1U);
    encode32(buffer,original_.levels.size());
    for(const auto& level:original_.levels) {
        encode16(buffer,level.id);
        encode16(buffer,level.width);
        encode16(buffer,level.height);
        encode32(buffer,level.cell_words.size());
        for(auto word:level.cell_words)encode16(buffer,word);
        encode32(buffer,level.controls.size());
        for(const auto& control:level.controls) {
            encode16(buffer,control.column);
            buffer.insert(buffer.end(),control.bytes.begin(),control.bytes.end());
        }
        for(const auto& family:level.families) {
            encode32(buffer,family.size());
            for(const auto& record:family)
                buffer.insert(buffer.end(),record.begin(),record.end());
        }
        for(const auto& event:level.event_slots)
            buffer.insert(buffer.end(),event.begin(),event.end());
        bytes(buffer,level.automap);
        bytes(buffer,level.annotation);
    }
    return native_sha256_hex(buffer);
}
NativeRuntimeSnapshot NativeWorldRuntime::snapshot() const {
    return {source_fingerprint(),mutable_maps_,mutable_controls_,active_level_};
}
void NativeWorldRuntime::restore(const NativeRuntimeSnapshot& saved) {
    require(saved.source_fingerprint_sha256==source_fingerprint(),
            "runtime snapshot belongs to another original world payload");
    require(saved.maps.levels.size()==32 &&
            saved.controls.size()==32 && saved.active_level<32,
            "runtime snapshot missing original-level state");
    for(unsigned id=0;id<32;++id) {
        const auto& source=original_.level(id);
        const auto& candidate=saved.maps.levels[id];
        require(candidate.id==id && candidate.width==source.width &&
                candidate.height==source.height &&
                candidate.cell_words.size()==source.cell_words.size() &&
                candidate.automap.size()==source.automap.size(),
                "runtime snapshot map shape differs from original");
        require(saved.controls[id].size()==source.controls.size(),
                "runtime snapshot control table size differs from original");
        for(std::size_t j=0;j<source.controls.size();++j) {
            const auto& expected=source.controls[j];
            const auto& actual=saved.controls[id][j];
            require(actual.column==expected.column &&
                    actual.bytes[0]==expected.bytes[0] &&
                    actual.bytes[4]==expected.bytes[4] &&
                    actual.bytes[5]==expected.bytes[5] &&
                    valid_control(actual,source.width,source.height),
                    "runtime snapshot changed control identity or state range");
        }
    }
    // Validate all fields before replacing any active state.
    mutable_maps_=saved.maps;
    mutable_controls_=saved.controls;
    active_level_=saved.active_level;
}
std::vector<NativeSceneCell> NativeWorldRuntime::marked_scene_cells(
    unsigned level,std::size_t budget) const {
    const auto& m=map(level);
    require(budget<=m.cell_words.size(),"scene inventory budget exceeds map size");
    std::vector<NativeSceneCell> visible;
    visible.reserve(budget);
    for(unsigned row=0;row<m.height;++row) {
        for(unsigned column=0;column<m.width;++column) {
            if((m.cell_words[std::size_t(row)*m.width+column]&
                kCellCurrentVisibility)==0)continue;
            require(visible.size()<budget,
                    "marked scene cell count exceeds declared caller budget");
            visible.push_back(cell(level,column,row));
        }
    }
    return visible;
}

std::string NativeWorldRuntime::evidence_json() const {
    std::size_t original_cells=0, visible=0, explored=0;
    std::size_t dynamic=0,active=0,passable=0,ordinary_blocked=0,
                special_blocked=0,special_centers=0,legacy_cells=0;
    for(unsigned level=0;level<32;++level) {
        const auto& m=map(level);
        original_cells+=m.cell_words.size();
        for(unsigned row=0;row<m.height;++row)
            for(unsigned column=0;column<m.width;++column) {
                const auto scene=cell(level,column,row);
                visible+=scene.transient_visible;
                explored+=scene.explored;
                special_centers+=scene.special_center;
                legacy_cells+=scene.authoring_marker;
                switch(scene.normal_collision) {
                    case NativeCollision::Passable: ++passable;break;
                    case NativeCollision::OrdinaryBlocked: ++ordinary_blocked;break;
                    case NativeCollision::SpecialBlocked: ++special_blocked;break;
                }
            }
        for(const auto& r:mutable_controls_[level]) {
            if(r.bytes[0]==1) {
                ++dynamic;
                active+=signed_delta(r.bytes[7])!=0;
            }
        }
    }
    std::ostringstream out;
    out<<"{\"schema\":\"rloft-native-world-runtime-evidence/v1\","
       <<"\"original_world_sha256\":\""<<source_fingerprint()<<"\","
       <<"\"levels\":"<<original_.levels.size()
       <<",\"active_level\":"<<active_level_
       <<",\"total_cells\":"<<original_cells
       <<",\"visible_cells\":"<<visible
       <<",\"explored_cells\":"<<explored
       <<",\"kind1_dynamic_records\":"<<dynamic
       <<",\"animating_dynamic_records\":"<<active
       <<",\"passable_normal_cells\":"<<passable
       <<",\"ordinary_blocked_cells\":"<<ordinary_blocked
       <<",\"special_blocked_cells\":"<<special_blocked
       <<",\"special_center_cells\":"<<special_centers
       <<",\"legacy_authoring_markers\":"<<legacy_cells
       <<",\"scene_projection_verified\":false,"
       <<"\"event_vm_executed\":false,"
       <<"\"original_dos_execution_verified\":false,"
       <<"\"gameplay_parity_verified\":false}\n";
    return out.str();
}
} // namespace rloft_native
