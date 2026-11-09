#pragma once

#include "rloft/RES9.hpp"
#include "rloft/voc_decoder.hpp"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

namespace rloft {
class ResArchive;

// Shipped legacy/pre-split RES12: graphics 0..369, 37 original interaction
// programs 370..406, and 260 VOC samples 407..666. Not opened by the
// certified CODE.1 / CODE.2 executables, which only select RES0..RES10.
enum class Res12Family : std::uint8_t { Raster = 1, Wordcode = 0, Voc = 4 };
struct Res12Range final { int first{}; int count{}; };
struct Res12Stats final {
    std::size_t archive_bytes{};
    std::size_t entries{}, rasters{}, raw_rasters{}, lzss_rasters{};
    std::uint64_t decoded_pixels{};
    std::size_t programs{}, program_words{}, instructions{}, inline_templates{};
    std::size_t nonzero_companion_words{};
    std::size_t voices{}, valid_voices{}, truncated_voices{};
};

// A VOC edit always supplies a structurally valid replacement; the shipped
// RES12:430 truncated VOC is preservation-only and cannot be replaced.
struct Res12VocPatch final {
    int index{}; // Absolute archive index, not a bank-relative ID.
    std::vector<std::uint8_t> expected_entry;
    std::vector<std::uint8_t> replacement_voc;
};
struct Res12Edits final {
    std::vector<Res9RasterPatch> rasters;
    std::vector<Res9ProgramPatch> programs;
    std::vector<Res12VocPatch> voices;
};

// A single source-preserving facade over the existing ResArchive and shared
// RES9 opcode grammar. The only dialect difference is inline opcode 05's
// 141 operand words instead of the later one-word static template ID.
class Res12Archive final {
public:
    explicit Res12Archive(std::vector<std::uint8_t> source);
    ~Res12Archive();
    Res12Archive(Res12Archive&&) noexcept;
    Res12Archive& operator=(Res12Archive&&) noexcept;
    Res12Archive(const Res12Archive&) = delete;
    Res12Archive& operator=(const Res12Archive&) = delete;

    [[nodiscard]] const Res12Stats& stats() const noexcept;
    [[nodiscard]] int entry_count() const noexcept;
    [[nodiscard]] Res12Range range(Res12Family family) const;
    [[nodiscard]] Res12Family family(int index) const;
    [[nodiscard]] std::vector<std::uint8_t> entry(int index) const;
    [[nodiscard]] std::vector<std::uint8_t> encode() const;
    [[nodiscard]] Res9RasterInfo raster_info(int index) const;
    [[nodiscard]] Type1Raster raster(int index) const;
    [[nodiscard]] Res9Program program(int id) const;
    [[nodiscard]] std::vector<Res9Instruction> instructions(int id) const;
    [[nodiscard]] std::vector<std::uint8_t> voc(int index) const;
    [[nodiscard]] PcmSound pcm16(int index) const;
    [[nodiscard]] bool is_truncated_voc(int index) const;

    // Exact expected source bytes/words are checked before any edit. Program
    // length remains constant, preserving the separate companion word bank.
    // Unmodified original compressed entries and the damaged VOC are retained.
    void apply(const Res12Edits& edits);

private:
    std::unique_ptr<ResArchive> archive_;
    Res12Range graphics_{}, programs_{}, voices_{};
    Res12Stats stats_{};
};
} // namespace rloft
