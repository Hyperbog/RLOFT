#include "rloft/LEV012.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft {
namespace {
std::uint16_t read16(const std::array<std::uint8_t,22>& b,
                     std::size_t at) noexcept {
    return static_cast<std::uint16_t>(b[at] |
        (static_cast<unsigned>(b[at+1U]) << 8U));
}
std::uint32_t read32(const std::array<std::uint8_t,22>& b,
                     std::size_t at) noexcept {
    return static_cast<std::uint32_t>(b[at]) |
        (static_cast<std::uint32_t>(b[at+1U]) << 8U) |
        (static_cast<std::uint32_t>(b[at+2U]) << 16U) |
        (static_cast<std::uint32_t>(b[at+3U]) << 24U);
}
void write16(std::array<std::uint8_t,22>& b, std::size_t at,
             std::uint16_t value) noexcept {
    b[at]=static_cast<std::uint8_t>(value);
    b[at+1U]=static_cast<std::uint8_t>(value >> 8U);
}
void write32(std::array<std::uint8_t,22>& b, std::size_t at,
             std::uint32_t value) noexcept {
    for (unsigned i=0;i<4U;++i) b[at+i]=static_cast<std::uint8_t>(value>>(8U*i));
}
std::int16_t signed16(std::uint16_t value) noexcept {
    return static_cast<std::int16_t>(value <= 0x7FFFU
        ? static_cast<std::int32_t>(value)
        : static_cast<std::int32_t>(value) - 65536);
}
} // namespace

std::uint32_t Lev012NpcRecord::y_position_16_16() const noexcept {return read32(raw,0U);}
std::uint32_t Lev012NpcRecord::x_position_16_16() const noexcept {return read32(raw,4U);}
std::uint32_t Lev012NpcRecord::runtime_flags() const noexcept {return read32(raw,8U);}
std::uint16_t Lev012NpcRecord::cached_cell_index() const noexcept {return read16(raw,12U);}
std::uint16_t Lev012NpcRecord::npc_type_id() const noexcept {return read16(raw,14U);}
std::uint16_t Lev012NpcRecord::action_state_countdown() const noexcept {return read16(raw,16U);}
std::int16_t Lev012NpcRecord::current_hp() const noexcept {return signed16(read16(raw,18U));}
std::uint16_t Lev012NpcRecord::packed_runtime_state() const noexcept {return read16(raw,20U);}
void Lev012NpcRecord::set_position_16_16(std::uint32_t x, std::uint32_t y,
                                           std::uint16_t cell) noexcept {
    write32(raw,0U,y);write32(raw,4U,x);write16(raw,12U,cell);
}
void Lev012NpcRecord::set_runtime_flags(std::uint32_t flags) noexcept {write32(raw,8U,flags);}
void Lev012NpcRecord::set_action_state_countdown(std::uint16_t n) noexcept {write16(raw,16U,n);}
void Lev012NpcRecord::set_current_hp(std::int16_t hp) noexcept {
    write16(raw,18U,static_cast<std::uint16_t>(hp));
}

Lev012File::Lev012File(std::vector<std::uint8_t> original)
    : direct_(std::move(original)) {}
const LevDirectSummary& Lev012File::summary() const noexcept {return direct_.summary();}
const std::vector<std::uint8_t>& Lev012File::bytes() const noexcept {return direct_.bytes();}
std::vector<std::uint8_t> Lev012File::encode() const {return direct_.encode();}
LevelState Lev012File::decode() const {return direct_.decode();}

std::size_t Lev012File::npc_offset(std::size_t index) const {
    if(index>=summary().world_counts[0])throw std::out_of_range("LEV012 NPC index");
    return static_cast<std::size_t>(summary().boundaries[2])+
        index*Lev012NpcRecord::kBytes;
}
Lev012NpcRecord Lev012File::npc(std::size_t index) const {
    const auto offset=npc_offset(index);
    Lev012NpcRecord record{};
    std::copy_n(bytes().begin()+static_cast<std::ptrdiff_t>(offset),
                record.raw.size(),record.raw.begin());
    return record;
}

void Lev012File::apply_npc_patches(const std::vector<Lev012NpcPatch>& patches) {
    if(patches.empty())return;
    std::vector<bool> seen(summary().world_counts[0],false);
    const auto old=decode();
    auto npc_bytes=old.family_a_raw;
    for(const auto& patch:patches){
        const auto at=npc_offset(patch.index); // bounds check
        if(seen[patch.index])throw std::invalid_argument("LEV012 duplicate NPC patch");
        seen[patch.index]=true;
        const auto local=at-static_cast<std::size_t>(summary().boundaries[2]);
        if(!std::equal(patch.expected.raw.begin(),patch.expected.raw.end(),
                       npc_bytes.begin()+static_cast<std::ptrdiff_t>(local)))
            throw std::runtime_error("LEV012 stale original NPC bytes");
        // Position and cached grid index must change together. The original
        // game may contain inactive/opaque records: never normalize those
        // on unrelated HP, state or flag edits.
        const auto& before=patch.expected;
        const auto& after=patch.replacement;
        if(before.x_position_16_16()!=after.x_position_16_16() ||
           before.y_position_16_16()!=after.y_position_16_16() ||
           before.cached_cell_index()!=after.cached_cell_index()) {
            const auto x=after.x_position_16_16()>>16U;
            const auto y=after.y_position_16_16()>>16U;
            if(x>=static_cast<unsigned>(summary().width) ||
               y>=static_cast<unsigned>(summary().height) ||
               after.cached_cell_index()!=y*static_cast<unsigned>(summary().width)+x)
                throw std::invalid_argument("LEV012 NPC position/cache mismatch");
        }
    }
    for(const auto& patch:patches){
        const auto local=npc_offset(patch.index)-
            static_cast<std::size_t>(summary().boundaries[2]);
        std::copy(patch.replacement.raw.begin(),patch.replacement.raw.end(),
                  npc_bytes.begin()+static_cast<std::ptrdiff_t>(local));
    }
    Lev011File checked(encode());
    checked.replace_world_family(LevWorldFamily::Npcs,npc_bytes);
    direct_=std::move(checked); // Commit atomically, after full grammar validation.
}

std::uint16_t Lev012File::cell_word(int x,int y) const {return direct_.cell_word(x,y);}
Lev005EventSlot Lev012File::event_slot(std::size_t id) const {return direct_.event_slot(id);}
std::vector<Lev011MapNote> Lev012File::notes() const {return direct_.notes();}
void Lev012File::apply_cell_patches(const std::vector<Lev010CellPatch>& p) {direct_.apply_cell_patches(p);}
void Lev012File::replace_event_slot(std::size_t id,const Lev005EventSlot& s) {direct_.replace_event_slot(id,s);}
void Lev012File::replace_notes(const std::vector<Lev011MapNote>& ns) {direct_.replace_notes(ns);}
void Lev012File::set_music_sequence(std::uint8_t id) {direct_.set_music_sequence(id);}
void Lev012File::replace_world_family(LevWorldFamily f,const std::vector<std::uint8_t>& v) {
    direct_.replace_world_family(f,v);
}

} // namespace rloft
