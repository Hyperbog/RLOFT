#pragma once

#include <cstddef>
#include <cstdint>
#include <deque>
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>

#include "rloft/dev_level_diagnostics.hpp"
#include "rloft/resource_assets.hpp"
#include "rloft/startup_presenter.hpp"

namespace rloft {

// Explicitly opt-in HOST instrumentation, never original CODE.2 behavior.
// All dimensions and limits are fixed to bound session output on disk.
class DevSessionTrace final {
public:
    static constexpr std::size_t kMaxSnapshots=96U;
    static constexpr std::size_t kMaxViewportSnapshots=64U;
    static constexpr std::size_t kMaxMarkers=16U;
    static constexpr std::size_t kMaxEvents=16384U;
    static constexpr std::size_t kRecentEvents=256U;
    static constexpr std::size_t kFramebufferBytes=320U*400U;
    // Full party panels now invoke many one-row original RES2 frames.
    // The first Build #651 field session recorded <=1,562 draws per
    // presentation and truncated the final equipment/overlay calls.
    // 4,096 retains those source-ordered ops while remaining bounded.
    static constexpr std::size_t kMaxDrawsPerPresent=4096U;

    DevSessionTrace(const std::filesystem::path& directory,
                    std::string_view source_revision,
                    std::string_view source_kind);
    ~DevSessionTrace();

    DevSessionTrace(const DevSessionTrace&)=delete;
    DevSessionTrace& operator=(const DevSessionTrace&)=delete;

    [[nodiscard]] const std::filesystem::path& directory() const noexcept {
        return directory_;
    }
    [[nodiscard]] std::size_t snapshot_count() const noexcept {
        return snapshots_;
    }
    [[nodiscard]] std::size_t marker_count() const noexcept {
        return markers_;
    }

    // Only symbolic actions, mouse coordinates and translated scan codes.
    // Never capture key text, names, writable CHARS, SAVE or install paths.
    void event(std::string_view name,
               std::int64_t a=0,
               std::int64_t b=0);
    void request_marker();
    // The IndexedFramebuffer's optional native drawing API observer.
    // This changes NO raster bytes, order, or original game state.
    void indexed_draw(const IndexedDrawAuditCall& call);
    void observe(std::string_view screen,
                 const IndexedFramebuffer& framebuffer,
                 const Palette6& palette,
                 const DevLevelSnapshot& snapshot,
                 std::uint32_t irq_tick);
    void error(std::string_view stage,std::string_view message);
    void finish();

private:
    static std::string escape_json(std::string_view text);
    void append(std::string line);
    void capture(std::string_view screen,
                 const IndexedFramebuffer& framebuffer,
                 const Palette6& palette,
                 const DevLevelSnapshot& snapshot,
                 std::uint32_t irq_tick,
                 bool is_marker);
    std::filesystem::path directory_;
    std::ofstream log_;
    std::deque<std::string> recent_;
    std::vector<IndexedDrawAuditCall> drawn_this_present_;
    std::size_t draws_discarded_{};
    std::string last_screen_;
    std::uint64_t last_camera_{};
    std::uint64_t last_party_{};
    std::uint64_t last_inventory_{};
    std::uint64_t last_world_{};
    std::uint64_t last_flags_{};
    std::size_t snapshots_{};
    std::size_t viewport_snapshots_{};
    std::size_t markers_{};
    std::size_t events_{};
    std::uint64_t sequence_{};
    std::uint32_t last_captured_tick_{};
    bool pending_marker_{};
    bool observed_{};
    bool dirty_{};
    bool finished_{};
};

} // namespace rloft
