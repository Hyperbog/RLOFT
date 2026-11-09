#include "rloft/SAVE.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft {
namespace {
constexpr std::size_t kContainerStateCount = SaveDat::kContainerSlots.size();
constexpr std::size_t serialized_container_bytes() {
    std::size_t n=0;
    for (const auto slots:SaveDat::kContainerSlots) n+=2U*slots+1U;
    return n;
}
static_assert(serialized_container_bytes()==SaveDat::kContainerBytes,
              "CODE.2 SAVE container descriptor geometry changed");

std::uint16_t get16(const std::vector<std::uint8_t>& b,std::size_t at) noexcept {
    return static_cast<std::uint16_t>(b[at] | (std::uint16_t(b[at+1U])<<8U));
}
std::uint32_t get32(const std::vector<std::uint8_t>& b,std::size_t at) noexcept {
    return std::uint32_t(b[at]) | (std::uint32_t(b[at+1U])<<8U) |
        (std::uint32_t(b[at+2U])<<16U) | (std::uint32_t(b[at+3U])<<24U);
}
void put16(std::vector<std::uint8_t>& b,std::size_t at,std::uint16_t v) noexcept {
    b[at]=static_cast<std::uint8_t>(v);
    b[at+1U]=static_cast<std::uint8_t>(v>>8U);
}
void put32(std::vector<std::uint8_t>& b,std::size_t at,std::uint32_t v) noexcept {
    for(unsigned i=0;i<4U;++i)b[at+i]=static_cast<std::uint8_t>(v>>(8U*i));
}
std::int16_t signed16(std::uint16_t v) noexcept {
    return static_cast<std::int16_t>(v<0x8000U ? static_cast<int>(v) : static_cast<int>(v)-65536);
}
template<std::size_t N> std::array<std::uint8_t,N> read_fixed(
    const std::vector<std::uint8_t>& b,std::size_t at) {
    std::array<std::uint8_t,N> value{};
    std::copy_n(b.begin()+static_cast<std::ptrdiff_t>(at),N,value.begin());
    return value;
}
template<std::size_t N> void write_fixed(
    std::vector<std::uint8_t>& b,std::size_t at,const std::array<std::uint8_t,N>& value) {
    std::copy(value.begin(),value.end(),b.begin()+static_cast<std::ptrdiff_t>(at));
}
} // namespace

SaveDatLayout SaveDat::verify(const std::vector<std::uint8_t>& b) {
    if(b.size()<kZeroEffectFileBytes)
        throw std::runtime_error("SAVE.DAT is truncated before fixed sections");
    const auto count=get32(b,kEffectCountAt);
    if(count>kMaxEffects) throw std::runtime_error("SAVE.DAT transient count exceeds CODE.2 150-record limit");
    const auto shift=static_cast<std::size_t>(count)*kEffectBytes;
    if(b.size()!=kZeroEffectFileBytes+shift)
        throw std::runtime_error("SAVE.DAT byte length disagrees with transient count");
    SaveDatLayout l{};
    l.transient_count=count;
    l.transient_start=kEffectCountAt+4U;
    l.spell_selection_at=l.transient_start+shift;
    l.gameplay_ticks_at=l.spell_selection_at+8U;
    l.container_at=l.gameplay_ticks_at+4U;
    l.shading_at=l.container_at+kContainerBytes;
    l.charges_at=l.shading_at+8U;
    l.roster_at=l.charges_at+15U;
    l.container_ui_at=l.roster_at+kRosterActors*kCharacterBytes;
    if(l.container_ui_at+kPartyActors*4U!=b.size())
        throw std::runtime_error("SAVE.DAT sections do not reach original EOF");
    return l;
}

SaveDat::SaveDat(std::vector<std::uint8_t> b)
    : bytes_(std::move(b)), layout_(verify(bytes_)) {}

SaveDatGlobals SaveDat::globals() const {
    SaveDatGlobals x{};
    x.description=read_fixed<50>(bytes_,0U);
    x.event_flags=read_fixed<32>(bytes_,0x32U);
    x.level=get32(bytes_,0x52U);
    x.coordinate_a=get32(bytes_,0x56U);
    x.coordinate_b=get32(bytes_,0x5AU);
    x.facing=get32(bytes_,0x5EU);
    x.shared_effect_state=read_fixed<10>(bytes_,0x62U);
    x.interrupt_ticks=get32(bytes_,0x6CU);
    x.time_of_day_16_16=get32(bytes_,0x70U);
    x.held_object_id=signed16(get16(bytes_,0x74U));
    x.held_item_known=bytes_[0x76U];
    return x;
}
void SaveDat::set_globals(const SaveDatGlobals& x) {
    write_fixed(bytes_,0U,x.description);
    write_fixed(bytes_,0x32U,x.event_flags);
    put32(bytes_,0x52U,x.level);
    put32(bytes_,0x56U,x.coordinate_a);
    put32(bytes_,0x5AU,x.coordinate_b);
    put32(bytes_,0x5EU,x.facing);
    write_fixed(bytes_,0x62U,x.shared_effect_state);
    put32(bytes_,0x6CU,x.interrupt_ticks);
    put32(bytes_,0x70U,x.time_of_day_16_16);
    put16(bytes_,0x74U,static_cast<std::uint16_t>(x.held_object_id));
    bytes_[0x76U]=x.held_item_known;
}
SaveDat::CharacterImage SaveDat::party(std::size_t actor) const {
    if(actor>=kPartyActors)throw std::out_of_range("SAVE.DAT active actor index");
    return read_fixed<kCharacterBytes>(bytes_,kPrefixBytes+actor*kCharacterBytes);
}
void SaveDat::set_party(std::size_t actor,const CharacterImage& data) {
    if(actor>=kPartyActors)throw std::out_of_range("SAVE.DAT active actor index");
    write_fixed(bytes_,kPrefixBytes+actor*kCharacterBytes,data);
}
SaveDat::CharacterImage SaveDat::roster(std::size_t actor) const {
    if(actor>=kRosterActors)throw std::out_of_range("SAVE.DAT saved-roster index");
    return read_fixed<kCharacterBytes>(bytes_,layout_.roster_at+actor*kCharacterBytes);
}
void SaveDat::set_roster(std::size_t actor,const CharacterImage& data) {
    if(actor>=kRosterActors)throw std::out_of_range("SAVE.DAT saved-roster index");
    write_fixed(bytes_,layout_.roster_at+actor*kCharacterBytes,data);
}
SaveDat::TransientImage SaveDat::transient(std::size_t index) const {
    if(index>=layout_.transient_count)throw std::out_of_range("SAVE.DAT transient index");
    return read_fixed<kEffectBytes>(bytes_,layout_.transient_start+index*kEffectBytes);
}
void SaveDat::replace_transients(const std::vector<TransientImage>& effects) {
    if(effects.size()>kMaxEffects)throw std::invalid_argument("SAVE.DAT too many transient effects");
    // Build whole replacement first; keep all subsequent opaque saved state.
    std::vector<std::uint8_t> rebuilt;
    rebuilt.reserve(kZeroEffectFileBytes+effects.size()*kEffectBytes);
    rebuilt.insert(rebuilt.end(),bytes_.begin(),bytes_.begin()+
        static_cast<std::ptrdiff_t>(kEffectCountAt));
    for(unsigned i=0;i<4U;++i)
        rebuilt.push_back(static_cast<std::uint8_t>(effects.size()>>(8U*i)));
    for(const auto& e:effects)rebuilt.insert(rebuilt.end(),e.begin(),e.end());
    rebuilt.insert(rebuilt.end(),bytes_.begin()+
        static_cast<std::ptrdiff_t>(layout_.spell_selection_at),bytes_.end());
    const auto checked=verify(rebuilt);
    bytes_.swap(rebuilt);
    layout_=checked;
}
std::array<std::uint32_t,2> SaveDat::selected_spell_characters() const {
    return {{get32(bytes_,layout_.spell_selection_at),
             get32(bytes_,layout_.spell_selection_at+4U)}};
}
void SaveDat::set_selected_spell_characters(const std::array<std::uint32_t,2>& ids) {
    put32(bytes_,layout_.spell_selection_at,ids[0]);
    put32(bytes_,layout_.spell_selection_at+4U,ids[1]);
}
std::uint32_t SaveDat::gameplay_ticks() const {return get32(bytes_,layout_.gameplay_ticks_at);}
void SaveDat::set_gameplay_ticks(std::uint32_t ticks) {put32(bytes_,layout_.gameplay_ticks_at,ticks);}
std::size_t SaveDat::container_offset(std::size_t index) const {
    if(index>=kContainerStateCount)throw std::out_of_range("SAVE.DAT container index");
    std::size_t at=layout_.container_at;
    for(std::size_t i=0;i<index;++i)at+=kContainerSlots[i]*2U+1U;
    return at;
}
SaveDatContainer SaveDat::container(std::size_t index) const {
    auto at=container_offset(index);
    SaveDatContainer x{};
    x.object_ids.reserve(kContainerSlots[index]);
    for(std::size_t i=0;i<kContainerSlots[index];++i,at+=2U)
        x.object_ids.push_back(signed16(get16(bytes_,at)));
    x.state_bits=bytes_[at];
    return x;
}
void SaveDat::set_container(std::size_t index,const SaveDatContainer& x) {
    auto at=container_offset(index);
    if(x.object_ids.size()!=kContainerSlots[index])
        throw std::invalid_argument("SAVE.DAT container slot geometry changed");
    for(const auto item:x.object_ids) {put16(bytes_,at,static_cast<std::uint16_t>(item));at+=2U;}
    bytes_[at]=x.state_bits;
}
std::uint32_t SaveDat::environment_shading_level() const {return get32(bytes_,layout_.shading_at);}
std::uint32_t SaveDat::level_light_adjustment() const {return get32(bytes_,layout_.shading_at+4U);}
void SaveDat::set_environment_controls(std::uint32_t shading,std::uint32_t adjustment) {
    put32(bytes_,layout_.shading_at,shading);
    put32(bytes_,layout_.shading_at+4U,adjustment);
}
std::array<std::uint8_t,15> SaveDat::charged_items() const {
    return read_fixed<15>(bytes_,layout_.charges_at);
}
void SaveDat::set_charged_items(const std::array<std::uint8_t,15>& charges) {
    write_fixed(bytes_,layout_.charges_at,charges);
}
SaveDat::ContainerUiImage SaveDat::container_ui(std::size_t actor) const {
    if(actor>=kPartyActors)throw std::out_of_range("SAVE.DAT container UI actor");
    return read_fixed<4>(bytes_,layout_.container_ui_at+actor*4U);
}
void SaveDat::set_container_ui(std::size_t actor,const ContainerUiImage& x) {
    if(actor>=kPartyActors)throw std::out_of_range("SAVE.DAT container UI actor");
    write_fixed(bytes_,layout_.container_ui_at+actor*4U,x);
}
} // namespace rloft
