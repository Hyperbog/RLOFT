#include "rloft/LEV020.hpp"

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft {

Lev020File::Lev020File(std::vector<std::uint8_t> source)
    : direct_(std::move(source)) {}
const LevDirectSummary& Lev020File::summary() const noexcept {return direct_.summary();}
const std::vector<std::uint8_t>& Lev020File::bytes() const noexcept {return direct_.bytes();}
std::vector<std::uint8_t> Lev020File::encode() const {return direct_.encode();}
LevelState Lev020File::decode() const {return direct_.decode();}
Lev012NpcRecord Lev020File::npc(std::size_t i) const {return Lev012File(encode()).npc(i);}
Lev018PlacedRecord Lev020File::placed_object(std::size_t i) const {
    return Lev018File(encode()).placed_object(i);
}
std::vector<CellControlRecord> Lev020File::controls(
    Lev019ControlGroup group, int column) const {
    return Lev019File(encode()).controls(group, column);
}
std::uint16_t Lev020File::cell_word(int x, int y) const {return direct_.cell_word(x, y);}
std::uint8_t Lev020File::automap_cell(int x, int y) const {
    return Lev010File(encode()).automap_cell(x, y);
}
Lev005EventSlot Lev020File::event_slot(std::size_t slot) const {
    return Lev005File(encode()).event_slot(slot);
}

void Lev020File::apply(const Lev020Edits& e) {
    if (e.npcs.empty() && e.placed.empty() && e.controls.empty() &&
        e.cells.empty() && e.cache.empty() && e.events.empty()) return;
    auto staging = encode();
    if (!e.npcs.empty()) {
        Lev012File stage(std::move(staging));
        stage.apply_npc_patches(e.npcs);
        staging = stage.encode();
    }
    if (!e.placed.empty()) {
        Lev018File stage(std::move(staging));
        stage.apply_placed_patches(e.placed);
        staging = stage.encode();
    }
    if (!e.cells.empty() || !e.cache.empty()) {
        Lev010File stage(std::move(staging));
        if (!e.cache.empty()) {
            const auto width = stage.summary().width;
            const auto height = stage.summary().height;
            std::vector<bool> seen(static_cast<std::size_t>(width) * height);
            auto cache = stage.decode().automap_cache;
            for (const auto& patch : e.cache) {
                if (patch.column < 0 || patch.row < 0 ||
                    patch.column >= width || patch.row >= height)
                    throw std::out_of_range("LEV020 automap coordinate");
                const auto i = static_cast<std::size_t>(patch.row) * width +
                               static_cast<std::size_t>(patch.column);
                if (seen[i]) throw std::invalid_argument("LEV020 duplicate automap cell");
                seen[i] = true;
                if (cache.at(i) != patch.expected)
                    throw std::runtime_error("LEV020 stale automap byte");
                cache[i] = patch.replacement;
            }
            stage.replace_automap_cache(cache);
        }
        stage.apply_cell_patches(e.cells);
        staging = stage.encode();
    }
    if (!e.events.empty()) {
        Lev005File stage(std::move(staging));
        std::vector<bool> seen(50U);
        for (const auto& patch : e.events) {
            if (patch.slot >= seen.size())
                throw std::out_of_range("LEV020 event slot index");
            if (seen[patch.slot]) throw std::invalid_argument("LEV020 duplicate event slot");
            seen[patch.slot] = true;
            if (stage.event_slot(patch.slot).raw != patch.expected.raw)
                throw std::runtime_error("LEV020 stale event slot");
        }
        for (const auto& patch : e.events)
            stage.replace_event_slot(patch.slot, patch.replacement);
        staging = stage.encode();
    }
    if (!e.controls.empty()) {
        Lev019File stage(std::move(staging));
        stage.apply_control_patches(e.controls);
        staging = stage.encode();
    }
    Lev001File validated(std::move(staging));
    direct_ = std::move(validated); // Single commit after every stage succeeds.
}

} // namespace rloft
