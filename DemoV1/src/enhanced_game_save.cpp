#include "rloft/enhanced_game_save.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace rloft {
namespace {
constexpr std::size_t kCap=32U*1024U*1024U;
std::uint64_t sum(const std::vector<std::uint8_t>& d,std::size_t n) {
    auto h=std::uint64_t{14695981039346656037ULL};
    for(std::size_t i=0;i<n;++i){h^=d[i];h*=1099511628211ULL;}return h;
}
struct W {
    std::vector<std::uint8_t> b;
    void v(std::uint64_t x,unsigned n){for(unsigned i=0;i<n;++i)b.push_back(static_cast<std::uint8_t>(x>>(i*8U)));}
    void u8(std::uint8_t x){v(x,1);}
    void u16(std::uint16_t x){v(x,2);}
    void u32(std::uint32_t x){v(x,4);}
    void u64(std::uint64_t x){v(x,8);}
    void n(std::size_t x,std::size_t max=kCap){
        if(x>max || x>UINT32_MAX)throw std::invalid_argument("checkpoint collection too large");
        u32(static_cast<std::uint32_t>(x));
    }
    template<std::size_t N>void fixed(const std::array<std::uint8_t,N>& a){
        b.insert(b.end(),a.begin(),a.end());
    }
    void bytes(const std::vector<std::uint8_t>& a,std::size_t max=kCap){
        n(a.size(),max);b.insert(b.end(),a.begin(),a.end());
    }
    void text(const std::string& x){n(x.size(),4096U);b.insert(b.end(),x.begin(),x.end());}
};
struct R {
    const std::vector<std::uint8_t>& b;std::size_t i{};
    void check(std::size_t n){if(i>b.size() || n>b.size()-i)
        throw std::invalid_argument("truncated game checkpoint");}
    std::uint64_t v(unsigned n){check(n);std::uint64_t x=0;
        for(unsigned j=0;j<n;++j)x|=std::uint64_t(b[i++])<<(j*8U);return x;}
    std::uint8_t u8(){return static_cast<std::uint8_t>(v(1));}
    std::uint16_t u16(){return static_cast<std::uint16_t>(v(2));}
    std::uint32_t u32(){return static_cast<std::uint32_t>(v(4));}
    std::uint64_t u64(){return v(8);}
    std::size_t n(std::size_t cap=kCap){auto x=u32();if(x>cap)
        throw std::invalid_argument("checkpoint section count invalid");return x;}
    template<std::size_t N>void fixed(std::array<std::uint8_t,N>& a){
        check(N);for(auto& x:a)x=b[i++];}
    std::vector<std::uint8_t> bytes(std::size_t max=kCap){
        const auto k=n(max);check(k);
        auto r=std::vector<std::uint8_t>(
            b.begin()+static_cast<std::ptrdiff_t>(i),
            b.begin()+static_cast<std::ptrdiff_t>(i+k));
        i+=k;return r;
    }
    std::string text(){const auto r=bytes(4096);return {r.begin(),r.end()};}
};
void level(W& w,const LevelState& l) {
    w.u8(static_cast<std::uint8_t>(l.storage));w.u8(l.writer_unowned_marker);
    for(auto x:l.boundaries)w.u32(x);
    w.fixed(l.header);w.u32(static_cast<std::uint32_t>(l.width));
    w.u32(static_cast<std::uint32_t>(l.height));
    w.u8(l.environment_profile);w.u8(static_cast<std::uint8_t>(l.renderer_vertical_offset));
    w.u8(l.renderer_fill_base_a);w.u8(l.renderer_fill_base_b);
    w.u16(static_cast<std::uint16_t>(l.render_threshold_floor));w.u8(l.music_sequence);
    w.n(l.cell_words.size(),16384);
    for(auto x:l.cell_words)w.u16(x);
    w.n(l.group1.size(),16384);
    for(const auto& x:l.group1)w.fixed(x.raw);
    w.n(l.group2.size(),16384);
    for(const auto& x:l.group2)w.fixed(x.raw);
    w.u8(l.active_animation_count);
    w.bytes(l.family_a_raw,1048576);w.bytes(l.family_b_raw,1048576);
    w.bytes(l.family_c_raw,1048576);
    w.fixed(l.event_scripts);w.bytes(l.automap_cache,16384);
    w.bytes(l.automap_annotations,1048576);
}
LevelState level(R& r) {
    LevelState l{};
    auto storage=r.u8();
    if(storage>1)throw std::invalid_argument("invalid checkpoint level storage");
    l.storage=static_cast<LevelStorage>(storage);
    l.writer_unowned_marker=r.u8();
    for(auto& x:l.boundaries)x=r.u32();
    r.fixed(l.header);l.width=static_cast<int>(r.u32());
    l.height=static_cast<int>(r.u32());
    if(l.width<=0||l.height<=0||l.width>128||l.height>128)
        throw std::invalid_argument("invalid saved world dimensions");
    l.environment_profile=r.u8();
    l.renderer_vertical_offset=static_cast<std::int8_t>(r.u8());
    l.renderer_fill_base_a=r.u8();l.renderer_fill_base_b=r.u8();
    l.render_threshold_floor=static_cast<std::int16_t>(r.u16());
    l.music_sequence=r.u8();
    auto n=r.n(16384);
    if(n!=static_cast<std::size_t>(l.width*l.height))
        throw std::invalid_argument("checkpoint cell count does not match level");
    l.cell_words.resize(n);for(auto& x:l.cell_words)x=r.u16();
    n=r.n(16384);l.group1.resize(n);for(auto& x:l.group1)r.fixed(x.raw);
    n=r.n(16384);l.group2.resize(n);for(auto& x:l.group2)r.fixed(x.raw);
    l.active_animation_count=r.u8();
    l.family_a_raw=r.bytes(1048576);l.family_b_raw=r.bytes(1048576);
    l.family_c_raw=r.bytes(1048576);
    r.fixed(l.event_scripts);l.automap_cache=r.bytes(16384);
    l.automap_annotations=r.bytes(1048576);
    return l;
}
void world(W& w,const LevelWorldState& s){
    w.n(s.npcs.size(),20000);
    for(const auto& x:s.npcs) {
        w.u32(x.y_position_16_16);w.u32(x.x_position_16_16);
        w.u32(x.runtime_flags);w.u16(x.cached_cell_index);
        w.u16(x.npc_type_id);w.u16(x.action_state_countdown);
        w.u16(static_cast<std::uint16_t>(x.current_hp));w.u16(x.packed_runtime_state);
    }
    const auto pack=[&](const std::vector<LiveObjectRecord>& v){
        w.n(v.size(),30000);
        for(const auto& x:v){
            w.u32(x.y_position_16_16);w.u32(x.x_position_16_16);
            w.u32(x.runtime_flags);w.u16(x.cached_cell_index);
            w.u16(x.object_type_id);w.u16(x.state_word);
            w.u16(x.auxiliary_word);w.u16(x.packed_runtime_state);
        }
    };
    pack(s.placed_objects);pack(s.loose_items);
}
LevelWorldState world(R& r) {
    LevelWorldState s{};
    auto n=r.n(20000);s.npcs.resize(n);
    for(auto& x:s.npcs) {
        x.y_position_16_16=r.u32();x.x_position_16_16=r.u32();
        x.runtime_flags=r.u32();x.cached_cell_index=r.u16();
        x.npc_type_id=r.u16();x.action_state_countdown=r.u16();
        x.current_hp=static_cast<std::int16_t>(r.u16());
        x.packed_runtime_state=r.u16();
    }
    const auto load=[&](std::vector<LiveObjectRecord>& v) {
        const auto n=r.n(30000);v.resize(n);
        for(auto& x:v) {
            x.y_position_16_16=r.u32();x.x_position_16_16=r.u32();
            x.runtime_flags=r.u32();x.cached_cell_index=r.u16();
            x.object_type_id=r.u16();x.state_word=r.u16();
            x.auxiliary_word=r.u16();x.packed_runtime_state=r.u16();
        }
    };
    load(s.placed_objects);load(s.loose_items);return s;
}
void inventory(W& w,const InventoryRuntimeState& s){
    w.u16(static_cast<std::uint16_t>(s.held.object_id));w.u8(s.held.known);
    // Mage scribe descriptors are loaded again from same verified CODE.2.
    // Transient feedback must be reinitialized after a saved clock resumes.
    for(const auto& x:s.containers) {
        w.u8(x.slot_count);w.u8(x.ui_columns);
        w.u8(x.capacity_pounds);w.u8(x.ui_x_offset);
        w.u8(x.subtype);w.u8(x.per_slot_state_bits);
        w.n(x.items.size(),16384);
        for(auto v:x.items)w.u16(static_cast<std::uint16_t>(v));
    }
    for(const auto& x:s.party_container_ui_contexts)w.fixed(x.raw);
    w.u8(s.open_container?1U:0U);
    if(s.open_container) {
        w.u32(static_cast<std::uint32_t>(s.open_container->actor));
        w.u32(static_cast<std::uint32_t>(s.open_container->hand));
        w.u16(static_cast<std::uint16_t>(s.open_container->object_id));
        w.u32(static_cast<std::uint32_t>(s.open_container->descriptor_index));
    }
}
InventoryRuntimeState inventory(R& r){
    InventoryRuntimeState s{};
    s.held.object_id=static_cast<std::int16_t>(r.u16());s.held.known=r.u8();
    for(auto& x:s.containers) {
        x.slot_count=r.u8();x.ui_columns=r.u8();
        x.capacity_pounds=r.u8();x.ui_x_offset=r.u8();
        x.subtype=r.u8();x.per_slot_state_bits=r.u8();
        const auto n=r.n(16384);x.items.resize(n);
        for(auto& v:x.items)v=static_cast<std::int16_t>(r.u16());
    }
    for(auto& x:s.party_container_ui_contexts)r.fixed(x.raw);
    if(r.u8()) {
        OpenInventoryContainer x{};x.actor=r.u32();x.hand=r.u32();
        x.object_id=static_cast<std::int16_t>(r.u16());x.descriptor_index=r.u32();
        if(x.actor>=4 || x.hand>=2 ||
           x.descriptor_index>=InventoryRuntimeState::kRuntimeDescriptorCount)
            throw std::invalid_argument("invalid saved open container");
        s.open_container=x;
    }
    return s;
}
void session(W& w,const ExplorationSessionState& s) {
    w.u32(s.level_id);w.u32(s.coordinate_a);w.u32(s.coordinate_b);
    w.u32(s.facing);w.u32(s.time_of_day_16_16_hours);
    w.fixed(s.persistent_event_flags);w.fixed(s.party.bytes);
    level(w,s.level);w.u8(s.level_from_direct_save?1U:0U);
}
ExplorationSessionState session(R& r) {
    ExplorationSessionState s{};
    s.level_id=r.u32();s.coordinate_a=r.u32();s.coordinate_b=r.u32();
    s.facing=r.u32();s.time_of_day_16_16_hours=r.u32();
    r.fixed(s.persistent_event_flags);r.fixed(s.party.bytes);
    s.level=level(r);s.level_from_direct_save=r.u8()!=0U;
    if(s.level_id>=32 || s.facing>=360 ||
       (s.coordinate_a>>16U)>=static_cast<unsigned>(s.level.width) ||
       (s.coordinate_b>>16U)>=static_cast<unsigned>(s.level.height))
        throw std::invalid_argument("saved player position outside level");
    return s;
}
} // namespace

std::vector<std::uint8_t> EnhancedGameSaveCodec::encode(
    const EnhancedGameSnapshot& s){
    W w{};
    constexpr std::array<std::uint8_t,8> magic{{'R','L','O','F','T','S','V','1'}};
    w.fixed(magic);w.u64(s.original_media_id);
    session(w,s.session);world(w,s.world);inventory(w,s.inventory);
    w.n(s.visited_levels.size(),32);
    for(const auto& entry:s.visited_levels){
        w.u32(static_cast<std::uint32_t>(entry.first));
        level(w,entry.second.level);world(w,entry.second.world);
        w.u8(entry.second.from_direct_save?1U:0U);
    }
    w.u32(s.random_state);w.u32(s.original_story_selector);
    if(w.b.size()+8U>kCap)
        throw std::invalid_argument("native checkpoint exceeds 32MB");
    w.u64(sum(w.b,w.b.size()));
    return std::move(w.b);
}
EnhancedGameSnapshot EnhancedGameSaveCodec::decode(
    const std::vector<std::uint8_t>& bytes){
    if(bytes.size()<24 || bytes.size()>kCap)
        throw std::invalid_argument("invalid native checkpoint file size");
    R r{bytes};
    constexpr std::array<std::uint8_t,8> magic{{'R','L','O','F','T','S','V','1'}};
    std::array<std::uint8_t,8> header{};r.fixed(header);
    if(header!=magic)throw std::invalid_argument("not a Ravenloft native checkpoint");
    EnhancedGameSnapshot s{};s.original_media_id=r.u64();
    s.session=session(r);s.world=world(r);s.inventory=inventory(r);
    const auto n=r.n(32);
    for(std::size_t i=0;i<n;++i){
        const int id=static_cast<int>(r.u32());
        if(id<0||id>=32)throw std::invalid_argument("invalid remembered level");
        LevelContinuityEntry e{};
        e.level=level(r);e.world=world(r);e.from_direct_save=r.u8()!=0U;
        if(!s.visited_levels.emplace(id,std::move(e)).second)
            throw std::invalid_argument("duplicate remembered level");
    }
    s.random_state=r.u32();s.original_story_selector=r.u32();
    if(r.i+8U!=bytes.size())
        throw std::invalid_argument("truncated or appended save fields");
    if(r.u64()!=sum(bytes,bytes.size()-8U))
        throw std::invalid_argument("native checkpoint checksum mismatch");
    return s;
}
} // namespace rloft
