#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <vector>

#include "rloft/exploration_presenter.hpp"

namespace rloft {

struct ExplorationCapturePosition final {
    std::size_t sequence{};
    std::uint32_t level{};
    std::int32_t cell_x{};
    std::int32_t cell_y{};
    std::uint32_t facing{};
};

// Optional native diagnostic artifact, NOT a simulated DOS frame.
// The original indexed 320x400 framebuffer and 256*3 palette bytes
// are serialized losslessly, plus all coalesced CODE.2 wall runs.
// Mask bytes are from immutable shipped CODE.2, NOT live DOS RAM.
class ExplorationFrameCapture final {
public:
    static void write(
        const std::filesystem::path& directory,
        const ExplorationCapturePosition& position,
        const ExplorationRuntimeFrame& runtime,
        const ExplorationPresentedFrame& presented,
        const IndexedFramebuffer& framebuffer,
        const std::vector<std::uint8_t>& code2);
};

} // namespace rloft
