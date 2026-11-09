#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

namespace rloft {
class ResArchive;
struct Type7SceneShape;
struct SurfaceTexture;
struct PcmSound;

enum class Res11Family : std::uint8_t {
    SceneShape = 7, Texture = 6, Xmidi = 3, Voice = 4
};

struct Res11Range final {
    int first{}; // Absolute RES archive entry index
    int count{};
};

struct Res11EntryInfo final {
    Res11Family family{Res11Family::SceneShape};
    std::size_t stored_bytes{};
    std::uint32_t declared_bytes{}; // Decoded size for 6/7; payload size for 3/4
    bool legacy_length_anomaly{};
};

struct Res11Stats final {
    std::size_t archive_bytes{};
    std::size_t entries{};
    std::size_t shapes{};
    std::size_t textures{};
    std::size_t xmidi{};
    std::size_t voices{};
    std::size_t anomalous_voices{};
};

struct Res11Verification final {
    std::size_t decoded_shapes{};
    std::size_t decoded_textures{};
    std::size_t decoded_xmidi{};
    std::size_t validated_voices{};
    std::size_t preserved_anomalies{};
    std::uint64_t shape_body_bytes{};
    std::uint64_t texture_texel_bytes{};
};

// Exactly source-matched, already encoded EH entries; patches must retain
// their resource family. Other entries, including legacy VOC anomalies, stay
// bit-for-bit unchanged. The complete archive is rebuilt just once.
struct Res11EntryPatch final {
    int index{};
    std::vector<std::uint8_t> expected_entry;
    std::vector<std::uint8_t> replacement_entry;
};

// RES11 is the CD-shipped *legacy/pre-split* RES3+RES4+RES5+RES6 composite.
// The certified CODE.1/CODE.2 loads RES0..RES10, NOT RES11. This editor does
// not introduce a new active runtime dependency or an alternate decoder.
class Res11Archive final {
public:
    explicit Res11Archive(std::vector<std::uint8_t> source);
    ~Res11Archive();
    Res11Archive(Res11Archive&&) noexcept;
    Res11Archive& operator=(Res11Archive&&) noexcept;
    Res11Archive(const Res11Archive&) = delete;
    Res11Archive& operator=(const Res11Archive&) = delete;

    [[nodiscard]] int entry_count() const noexcept;
    [[nodiscard]] const Res11Stats& stats() const noexcept;
    [[nodiscard]] Res11Range range(Res11Family family) const;
    [[nodiscard]] Res11EntryInfo entry_info(int index) const;
    [[nodiscard]] std::vector<std::uint8_t> entry(int index) const;
    [[nodiscard]] std::vector<std::uint8_t> encode() const;

    [[nodiscard]] Type7SceneShape shape(int index) const;
    [[nodiscard]] std::vector<std::uint8_t> shape_body(int index) const;
    [[nodiscard]] SurfaceTexture texture(int index) const;
    [[nodiscard]] std::vector<std::uint8_t> xmidi(int index) const;
    // Returns physical VOC bytes even for the one truncated legacy sample.
    [[nodiscard]] std::vector<std::uint8_t> voc(int index) const;
    // Explicitly rejects the shipped malformed RES11:4877; no invented PCM.
    [[nodiscard]] PcmSound pcm16(int index) const;
    [[nodiscard]] Res11Verification verify_all() const;

    // Builders reuse the original type-6 texture codec, type-7 LZSS format,
    // type-3 XMIDI envelope, and type-4 VOC validator respectively.
    [[nodiscard]] Res11EntryPatch texture_patch(
        int index, const SurfaceTexture& replacement) const;
    [[nodiscard]] Res11EntryPatch shape_body_patch(
        int index, const std::vector<std::uint8_t>& replacement) const;
    [[nodiscard]] Res11EntryPatch xmidi_patch(
        int index, const std::vector<std::uint8_t>& replacement) const;
    [[nodiscard]] Res11EntryPatch voc_patch(
        int index, const std::vector<std::uint8_t>& replacement) const;

    // Validates all expected originals, family identity, and decoded content
    // before the single commit. A failure never mutates this archive.
    void apply(const std::vector<Res11EntryPatch>& patches);

private:
    std::unique_ptr<ResArchive> archive_;
    std::array<Res11Range, 4> ranges_{};
    Res11Stats stats_{};
};

} // namespace rloft
