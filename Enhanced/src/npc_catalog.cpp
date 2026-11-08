#include "rloft/npc_catalog.hpp"
#include "rloft/NPCS.hpp"

#include <cstddef>
#include <stdexcept>
#include <utility>

namespace rloft {

NpcCatalog NpcCatalog::parse(const std::vector<std::uint8_t>& bytes) {
    NpcCatalog result{};
    result.npcs_ = decode_npcs_dat(bytes);
    return result;
}

NpcCatalog NpcCatalog::load(const GameSource& source) {
    return parse(source.read("NPCS.DAT"));
}

const NpcDefinition& NpcCatalog::at(std::size_t npc_id) const {
    if (npc_id >= npcs_.size()) throw std::out_of_range("NPCS.DAT npc id");
    return npcs_[npc_id];
}

NpcDefinition& NpcCatalog::edit(std::size_t npc_id) {
    if (npc_id >= npcs_.size()) throw std::out_of_range("NPCS.DAT npc id");
    return npcs_[npc_id];
}

std::vector<std::uint8_t> NpcCatalog::encode() const {
    return encode_npcs_dat(npcs_);
}

} // namespace rloft
