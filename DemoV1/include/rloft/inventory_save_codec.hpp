#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "rloft/exploration_state.hpp"
#include "rloft/inventory_runtime.hpp"

namespace rloft {

// This is an INVENTORY-SCOPED projection over an otherwise preserved
// original SAVE.DAT byte image. It is not a complete game-session loader:
// world transient effects, party movement, roster and the independent
// engine clocks remain original opaque bytes until their own runtime
// synchronization paths are implemented.
struct InventorySaveSnapshot final {
    std::vector<std::uint8_t> preserved_save_bytes;
    std::size_t container_bank_offset{};
    std::uint32_t transient_effect_count{};
    PartyImage party{};
    std::array<std::uint8_t,32> persistent_event_flags{};
    InventoryRuntimeState inventory{};
    // CODE.2 writes four raw 4-byte 6FA0 actor UI contexts at EOF.
    // They remain raw: the hand association and half-open transition
    // must be reconstructed before live modal reactivation on LOAD.
    std::array<std::array<std::uint8_t,4>,4> container_ui_contexts{};
};

class InventorySaveCodec final {
public:
    static constexpr std::size_t kPrefixBytes=0x77U;
    static constexpr std::size_t kPartyStart=kPrefixBytes;
    static constexpr std::size_t kPartyBytes=PartyImage::kSize; // 0x920
    static constexpr std::size_t kTransientCountAt=
        kPartyStart+kPartyBytes; // 0x997
    static constexpr std::size_t kTransientRecordBytes=0x16U;
    static constexpr std::uint32_t kMaxTransientCount=0x96U;
    static constexpr std::size_t kZeroTransientSaveBytes=0x2448U;
    static constexpr std::size_t kPersistentContainerBytes=0x3AAU;

    // Each caller supplies the already source-parsed CODE.2
    // descriptor template from InventoryRuntimeState::fresh().
    // Only descriptors 0..61 are serialized. Descriptor 62 is
    // deliberately preserved separately; do NOT normalize
    // OBJECTS ID329's source selector 62 to descriptor 61.
    [[nodiscard]] static InventorySaveSnapshot decode(
        const std::vector<std::uint8_t>& original_save,
        const InventoryRuntimeState& code2_template);

    // Re-emits a byte-for-byte original SAVE image, changing ONLY
    // cursor identity/known, 32 event bytes, active 4x0x248 party,
    // 62 container contents/known bits, and four raw 6FA0 contexts.
    // The counted moving effects and all other original globals,
    // saved party roster, timing and unknown bytes are untouched.
    [[nodiscard]] static std::vector<std::uint8_t> encode(
        const InventorySaveSnapshot& snapshot);
};

} // namespace rloft
