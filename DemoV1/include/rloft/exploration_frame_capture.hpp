#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>
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

// Last fatal frame-building error is captured even after the 16-frame
// index/palette budget is exhausted. It records real native session
// inputs but contains NO invented post-failure framebuffer.
struct ExplorationCaptureFailure final {
    std::uint32_t level{};
    std::uint32_t coordinate_a{};
    std::uint32_t coordinate_b{};
    std::uint32_t facing{};
    int map_width{};
    int map_height{};
    std::size_t frames_written{};
    std::string source_revision;
    std::string reason;
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
        const std::vector<std::uint8_t>& code2,
        std::string_view source_revision = "local-unversioned");

    // A pure, independently testable append-only fatal diagnostic; does
    // not modify existing idx/pal/tsv captures or game scene state.
    static void write_failure(
        const std::filesystem::path& directory,
        const ExplorationCaptureFailure& failure);
};

} // namespace rloft
