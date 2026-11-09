#pragma once

#include "rloft/voc_decoder.hpp"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

namespace rloft {
class ResArchive;

// RES10's shipped EH type-4 entries each contain one standard Creative VOC
// type-1 block: mono, unsigned PCM8, with an unaltered time constant.
// The physical sample period is (256 - time_constant) microseconds.
struct Res10VoiceInfo final {
    std::uint16_t voc_version{};
    std::uint8_t time_constant{};
    std::uint8_t codec{};
    std::uint32_t nominal_sample_rate_hz{};
    std::size_t voc_bytes{};
    std::size_t pcm_samples{};
    std::uint64_t duration_microseconds{};
};

struct Res10ArchiveStats final {
    std::size_t entries{};
    std::uint64_t voc_bytes{};
    std::uint64_t pcm_samples{};
    std::uint64_t duration_microseconds{};
    std::size_t shortest_sample_count{};
    std::size_t longest_sample_count{};
};

struct Res10VoicePatch final {
    int index{};
    // Exact EH type-4 bytes observed by the caller. Reject stale sources.
    std::vector<std::uint8_t> expected_entry;
    // Replacement unsigned PCM8 samples. Header, version, time constant,
    // codec and original fields remain unchanged. Nonempty only.
    std::vector<std::uint8_t> replacement_pcm8;
};

class Res10Archive final {
public:
    explicit Res10Archive(std::vector<std::uint8_t> source);
    ~Res10Archive();
    Res10Archive(Res10Archive&&) noexcept;
    Res10Archive& operator=(Res10Archive&&) noexcept;
    Res10Archive(const Res10Archive&) = delete;
    Res10Archive& operator=(const Res10Archive&) = delete;

    [[nodiscard]] int entry_count() const noexcept;
    [[nodiscard]] const Res10ArchiveStats& stats() const noexcept;
    [[nodiscard]] std::vector<std::uint8_t> entry(int index) const;
    [[nodiscard]] std::vector<std::uint8_t> encode() const;
    [[nodiscard]] Res10VoiceInfo voice_info(int index) const;
    [[nodiscard]] std::vector<std::uint8_t> voc(int index) const;
    [[nodiscard]] std::vector<std::uint8_t> pcm8(int index) const;
    [[nodiscard]] PcmSound pcm16(int index) const;

    // Validates every source/patch first, then repacks the directory once.
    // Untouched EH/VOC entries stay byte-exact; failures never mutate live
    // data. No CODE.2 audio re-encoding or speculative VOC conversion.
    void apply(const std::vector<Res10VoicePatch>& edits);

private:
    std::unique_ptr<ResArchive> archive_;
    Res10ArchiveStats stats_{};
};

} // namespace rloft
