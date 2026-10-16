#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include "rloft/exploration_state.hpp"

namespace rloft {

// CODE.2 02A34 / 026E8: the actual DOS automap SAVE and PRINT
// byte streams, backed by the existing 3-plane 026E8 exporter.
// The source path is relative to the writable original runtime
// directory, not a fabricated screenshot or modern vector map.
class OriginalAutomapFileExport final {
public:
    [[nodiscard]] static std::string source_filename(int level_id);
    [[nodiscard]] static std::vector<std::uint8_t> merged_02a34(
        const LevelState& level);
    [[nodiscard]] static std::vector<std::uint8_t> printer_03321(
        const LevelState& level);
};

} // namespace rloft
