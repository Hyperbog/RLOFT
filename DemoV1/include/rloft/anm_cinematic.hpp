#pragma once

#include <cstdint>
#include <optional>
#include <vector>

#include "rloft/resource_assets.hpp"
#include "rloft/startup_presenter.hpp"

namespace rloft {

enum class AnmAudioCommand : std::uint8_t {
    MusicSequence,
    QueuedSfx,
    DirectSfx,
};

struct AnmAudioEvent {
    AnmAudioCommand command{};
    int first{};
    int second{};
    int archive{};
    int resource{};
};

struct AnmFrameResult {
    int index{};
    int wait_ticks{};
    std::vector<AnmAudioEvent> audio;
};

class AnmCinematic final {
public:
    AnmCinematic(
        std::vector<std::uint8_t> bytes,
        int cinematic_number,
        const RavenloftBootAssets& assets);

    void reset();
    [[nodiscard]] bool finished() const noexcept;
    [[nodiscard]] int frame_count() const noexcept;
    [[nodiscard]] int default_wait() const noexcept;

    AnmFrameResult render_next();

    [[nodiscard]] const IndexedFramebuffer& framebuffer() const noexcept {
        return framebuffer_;
    }
    [[nodiscard]] const Palette6& palette() const noexcept { return palette_; }

private:
    struct Chunk {
        std::uint16_t id{};
        std::vector<std::uint8_t> payload;
    };
    struct Frame {
        std::vector<Chunk> chunks;
    };

    void apply_type2(const std::vector<std::uint8_t>& payload);
    void apply_type3(const std::vector<std::uint8_t>& payload);
    void apply_type8(const std::vector<std::uint8_t>& payload);
    void render_subtitle();
    void draw_centered_line(
        int y, const std::string& text, const Type2Font& font);
    [[nodiscard]] std::vector<std::string> wrap_subtitle(
        const std::string& text, const Type2Font& font) const;

    const RavenloftBootAssets& assets_;
    int cinematic_number_{};
    int default_wait_{};
    std::vector<Frame> frames_;
    std::size_t next_frame_{};

    IndexedFramebuffer framebuffer_;
    Palette6 palette_{};

    bool subtitle_active_{};
    std::string subtitle_text_;
    std::optional<Type2Font> subtitle_font_;
};

}  // namespace rloft
