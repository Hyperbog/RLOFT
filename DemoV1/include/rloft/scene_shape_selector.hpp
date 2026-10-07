#pragma once

#include <cstdint>

#include "rloft/exploration_session.hpp"
#include "rloft/renderer_tables.hpp"

namespace rloft {

class SceneShapeSelectorModel final {
public:
    // Faithful CODE.2 2B2F4 view/state subframe selection for source classes
    // that do not enter the class-1 cache path. The returned ID is still
    // bank-local; callers add the INIT source-bank base afterward.
    [[nodiscard]] static std::uint16_t select_bank_local(
        std::uint16_t initial_bank_local_resource,
        std::uint16_t screen_x,
        std::uint16_t packed_runtime_state,
        std::uint16_t type_id,
        std::uint8_t source_class,
        std::uint8_t shape_stride,
        const ExplorationSessionState& session,
        const RendererTables& tables);
};

}  // namespace rloft
