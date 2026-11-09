#include "rloft/LEV031.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft {
namespace {
constexpr std::size_t kBankCount = 5U;
constexpr std::size_t kCodesPerBank = 64U;

std::size_t resource_key(Lev023ResourceBank bank, std::uint8_t code) {
    const auto id = static_cast<std::size_t>(bank);
    if (id >= kBankCount || code >= kCodesPerBank)
        throw std::out_of_range("LEV031 invalid resource bank or cell code");
    return id * kCodesPerBank + code;
}
} // namespace

Lev031File::Lev031File(std::vector<std::uint8_t> original)
    : direct_(std::move(original)) {}
const LevDirectSummary& Lev031File::summary() const noexcept {
    return direct_.summary();
}
const std::vector<std::uint8_t>& Lev031File::bytes() const noexcept {
    return direct_.bytes();
}
std::vector<std::uint8_t> Lev031File::encode() const {
    return direct_.encode();
}
LevelState Lev031File::decode() const { return direct_.decode(); }
std::uint16_t Lev031File::resource_word(
    Lev023ResourceBank bank, std::uint8_t code) const {
    return direct_.resource_word(bank, code);
}
std::array<std::uint16_t, 3> Lev031File::resource_triplet(
    Lev023ResourceBank bank, const std::array<std::uint8_t, 3>& codes) const {
    return {{resource_word(bank, codes[0]), resource_word(bank, codes[1]),
             resource_word(bank, codes[2])}};
}
std::vector<CellControlRecord> Lev031File::controls(
    Lev019ControlGroup group, int column) const {
    return direct_.controls(group, column);
}
Lev012NpcRecord Lev031File::npc(std::size_t i) const { return direct_.npc(i); }
Lev018PlacedRecord Lev031File::placed_object(std::size_t i) const {
    return direct_.placed_object(i);
}
Lev021LooseRecord Lev031File::loose_item(std::size_t i) const {
    return direct_.loose_item(i);
}
std::uint16_t Lev031File::cell_word(int x, int y) const {
    return direct_.cell_word(x, y);
}
std::uint8_t Lev031File::automap_cell(int x, int y) const {
    return direct_.automap_cell(x, y);
}
Lev005EventSlot Lev031File::event_slot(std::size_t i) const {
    return direct_.event_slot(i);
}

void Lev031File::apply(const Lev031Edits& edits) {
    Lev027Edits merged = edits.state;
    // A single resource entry cannot participate in two separate cycles.
    // LEV023 checks overlaps with any ordinary resource patches as well.
    std::array<bool, kBankCount * kCodesPerBank> seen{};
    for (const auto& cycle : edits.resource_cycles) {
        for (std::size_t i = 0U; i < cycle.cell_codes.size(); ++i) {
            const auto key = resource_key(cycle.bank, cycle.cell_codes[i]);
            if (seen[key])
                throw std::invalid_argument("LEV031 duplicate resource cycle entry");
            seen[key] = true;
            if (resource_word(cycle.bank, cycle.cell_codes[i]) != cycle.expected[i])
                throw std::runtime_error("LEV031 stale resource cycle");
        }
        for (std::size_t i = 0U; i < cycle.cell_codes.size(); ++i) {
            const auto previous = cycle.rotate_right ? (i + 2U) % 3U
                                                     : (i + 1U) % 3U;
            merged.state.level.world.resources.push_back({
                cycle.bank, cycle.cell_codes[i], cycle.expected[i],
                cycle.expected[previous]});
        }
    }
    // Lev027File -> Lev026/025/024/023 validate and stage all edits, including
    // group transfers and resource duplicate/expected-word checks. Only here
    // is the fully validated candidate assigned to the current instance.
    Lev027File candidate(encode());
    candidate.apply(merged);
    direct_ = std::move(candidate);
}

} // namespace rloft
