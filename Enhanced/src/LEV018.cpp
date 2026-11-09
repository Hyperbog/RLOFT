#include "rloft/LEV018.hpp"

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
    return static_cast<std::uint16_t>(
        static_cast<unsigned>(b[at]) |
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
    b[at+1U]=static_cast<std::uint8_t>(value>>8U);
}
void write32(std::array<std::uint8_t,22>& b, std::size_t at,
             std::uint32_t value) noexcept {
    for (unsigned i=0U; i<4U; ++i)
        b[at+i]=static_cast<std::uint8_t>(value>>(8U*i));
}
} // namespace

std::uint32_t Lev018PlacedRecord::y_position_16_16() const noexcept {return read32(raw,0U);}
std::uint32_t Lev018PlacedRecord::x_position_16_16() const noexcept {return read32(raw,4U);}
std::uint32_t Lev018PlacedRecord::runtime_flags() const noexcept {return read32(raw,8U);}
std::uint16_t Lev018PlacedRecord::cached_cell_index() const noexcept {return read16(raw,12U);}
std::uint16_t Lev018PlacedRecord::object_type_id() const noexcept {return read16(raw,14U);}
std::uint16_t Lev018PlacedRecord::state_word() const noexcept {return read16(raw,16U);}
std::uint16_t Lev018PlacedRecord::auxiliary_word() const noexcept {return read16(raw,18U);}
std::uint16_t Lev018PlacedRecord::packed_runtime_state() const noexcept {return read16(raw,20U);}
void Lev018PlacedRecord::set_position_16_16(
    std::uint32_t x, std::uint32_t y, std::uint16_t cell) noexcept {
    write32(raw,0U,y);write32(raw,4U,x);write16(raw,12U,cell);
}
void Lev018PlacedRecord::set_runtime_flags(std::uint32_t flags) noexcept {write32(raw,8U,flags);}
void Lev018PlacedRecord::set_state_word(std::uint16_t value) noexcept {write16(raw,16U,value);}
void Lev018PlacedRecord::set_auxiliary_word(std::uint16_t value) noexcept {write16(raw,18U,value);}
void Lev018PlacedRecord::set_packed_runtime_state(std::uint16_t value) noexcept {write16(raw,20U,value);}

Lev018File::Lev018File(std::vector<std::uint8_t> original)
    : direct_(std::move(original)) {}
const LevDirectSummary& Lev018File::summary() const noexcept {return direct_.summary();}
const std::vector<std::uint8_t>& Lev018File::bytes() const noexcept {return direct_.bytes();}
std::vector<std::uint8_t> Lev018File::encode() const {return direct_.encode();}
LevelState Lev018File::decode() const {return direct_.decode();}

std::size_t Lev018File::placed_offset(std::size_t index) const {
    if (index >= summary().world_counts[1])
        throw std::out_of_range("LEV018 placed-object index");
    // Family B begins with the original signed 16-bit object count.
    return static_cast<std::size_t>(summary().boundaries[3]) + 2U +
           index*Lev018PlacedRecord::kBytes;
}
Lev018PlacedRecord Lev018File::placed_object(std::size_t index) const {
    const auto offset=placed_offset(index);
    Lev018PlacedRecord result{};
    std::copy_n(bytes().begin()+static_cast<std::ptrdiff_t>(offset),
                result.raw.size(),result.raw.begin());
    return result;
}
void Lev018File::apply_placed_patches(
    const std::vector<Lev018PlacedPatch>& patches) {
    if (patches.empty()) return;
    std::vector<bool> seen(summary().world_counts[1],false);
    const auto old=decode();
    auto family_b=old.family_b_raw;
    const auto start=static_cast<std::size_t>(summary().boundaries[3]);
    for (const auto& patch:patches) {
        const auto at=placed_offset(patch.index); // Reject bounds before indexing.
        if (seen[patch.index])
            throw std::invalid_argument("LEV018 duplicate placed-object index");
        seen[patch.index]=true;
        const auto local=at-start;
        if (!std::equal(patch.expected.raw.begin(),patch.expected.raw.end(),
             family_b.begin()+static_cast<std::ptrdiff_t>(local)))
            throw std::runtime_error("LEV018 stale original placed-object bytes");
        const auto& before=patch.expected;
        const auto& after=patch.replacement;
        // Do not reject non-positional edits of opaque/inactive source records.
        if (before.x_position_16_16()!=after.x_position_16_16() ||
            before.y_position_16_16()!=after.y_position_16_16() ||
            before.cached_cell_index()!=after.cached_cell_index()) {
            const auto x=after.x_position_16_16()>>16U;
            const auto y=after.y_position_16_16()>>16U;
            if (x>=static_cast<std::uint32_t>(summary().width) ||
                y>=static_cast<std::uint32_t>(summary().height) ||
                after.cached_cell_index()!=
                    y*static_cast<std::uint32_t>(summary().width)+x)
                throw std::invalid_argument("LEV018 object position/cache mismatch");
        }
    }
    for (const auto& patch:patches) {
        const auto local=placed_offset(patch.index)-start;
        std::copy(patch.replacement.raw.begin(),patch.replacement.raw.end(),
                  family_b.begin()+static_cast<std::ptrdiff_t>(local));
    }
    // Lev001's replacement API expects only the 22-byte records: strip
    // Family B's serialized count word, which the codec reconstructs.
    std::vector<std::uint8_t> records(
        family_b.begin()+2, family_b.end());
    Lev012File checked(encode());
    checked.replace_world_family(LevWorldFamily::Placed,records);
    direct_=std::move(checked); // Commit only after full source-level validation.
}

Lev012NpcRecord Lev018File::npc(std::size_t index) const {return direct_.npc(index);}
std::uint16_t Lev018File::cell_word(int x,int y) const {return direct_.cell_word(x,y);}
Lev005EventSlot Lev018File::event_slot(std::size_t id) const {return direct_.event_slot(id);}
void Lev018File::apply_npc_patches(const std::vector<Lev012NpcPatch>& p) {direct_.apply_npc_patches(p);}
void Lev018File::apply_cell_patches(const std::vector<Lev010CellPatch>& p) {direct_.apply_cell_patches(p);}
void Lev018File::replace_event_slot(std::size_t id,const Lev005EventSlot& slot) {direct_.replace_event_slot(id,slot);}
void Lev018File::set_music_sequence(std::uint8_t id) {direct_.set_music_sequence(id);}
void Lev018File::replace_world_family(LevWorldFamily f,const std::vector<std::uint8_t>& data) {
    direct_.replace_world_family(f,data);
}

} // namespace rloft
