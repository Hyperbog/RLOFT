#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

namespace rloft {

// CODE.2 ANM stream: uint16 frame count, uint16 wait, then framed chunks.
// Works for all original CINE*.ANM streams; Level 00 is its first fixture.
struct CineChunkInfo final {
    std::uint16_t id{};
    std::size_t payload_offset{};
    std::size_t payload_bytes{};
};

struct CineStats final {
    std::size_t frames{};
    std::size_t chunks{};
    std::uint16_t default_wait{};
    std::array<std::size_t, 10> by_id{};
};

// Indexes refer to the pre-transaction frame/packet, not byte offsets.
// The expected payload is compared byte-for-byte; no guessed semantics.
struct CineChunkPatch final {
    std::size_t frame{};
    std::size_t chunk{};
    std::vector<std::uint8_t> expected_payload;
    std::vector<std::uint8_t> replacement_payload;
};
struct CineEdits final {
    std::optional<std::uint16_t> expected_wait;
    std::optional<std::uint16_t> replacement_wait;
    std::vector<CineChunkPatch> chunks;
};

class Cine00File final {
public:
    explicit Cine00File(std::vector<std::uint8_t> source);

    [[nodiscard]] const CineStats& stats() const noexcept;
    [[nodiscard]] const std::vector<std::uint8_t>& bytes() const noexcept;
    [[nodiscard]] std::vector<std::uint8_t> encode() const;
    [[nodiscard]] std::size_t chunk_count(std::size_t frame) const;
    [[nodiscard]] CineChunkInfo info(std::size_t frame, std::size_t chunk) const;
    [[nodiscard]] std::vector<std::uint8_t> payload(
        std::size_t frame, std::size_t chunk) const;

    // Decode the CODE.2 row-RLE full screen, native 320x400 indices.
    [[nodiscard]] std::vector<std::uint8_t> full_frame(
        std::size_t frame, std::size_t chunk) const;
    // Decode/encode the four planar 320x256 viewport at framebuffer y=51.
    // No 2x presentation scaling or palette remapping is performed.
    [[nodiscard]] std::vector<std::uint8_t> viewport(
        std::size_t frame, std::size_t chunk) const;
    [[nodiscard]] static std::vector<std::uint8_t> encode_viewport(
        const std::vector<std::uint8_t>& indexed_pixels);

    // Validates every requested edit and the complete rebuilt stream first.
    // All original bytes outside altered packet data/lengths survive.
    void apply(const CineEdits& edits);

private:
    std::vector<std::uint8_t> source_;
    std::vector<std::vector<CineChunkInfo>> frames_;
    CineStats stats_{};
    void index_and_validate();
};

} // namespace rloft
