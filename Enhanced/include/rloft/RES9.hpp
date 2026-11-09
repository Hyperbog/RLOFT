#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

namespace rloft {
class ResArchive;
struct Type1Raster;

// Original CODE.2 RES9 bank: type-1 interaction graphics followed by raw
// interaction/display VM programs. The program's second word bank is not
// consumed by the retail loader, but must remain unmodified and serialized.
struct Res9Program final {
    std::vector<std::uint16_t> words;
    std::vector<std::uint16_t> companion_words;
};

struct Res9Instruction final {
    std::size_t word_ip{};
    std::uint16_t opcode{};
    std::vector<std::uint16_t> operands;
};

struct Res9RasterInfo final {
    std::uint16_t count{};
    std::uint16_t height{};
    std::uint16_t width{};
    std::uint8_t descriptor_field5{};
    std::uint8_t compression{};
    std::uint8_t descriptor_field7{};
    std::size_t decoded_bytes{};
};

struct Res9Stats final {
    std::size_t encoded_bytes{};
    std::size_t image_entries{};
    std::size_t raw_images{};
    std::size_t lzss_images{};
    std::size_t decoded_pixel_bytes{};
    std::size_t programs{};
    std::size_t program_words{};
    std::size_t instructions{};
    std::size_t nonzero_companion_words{};
};

// Expected original EH entry prevents editing against a stale source image.
// New raster pixels retain the source's count, dimensions, and descriptor
// bytes; compression changes to the original supported raw method 0.
struct Res9RasterPatch final {
    int image_index{};
    std::vector<std::uint8_t> expected_entry;
    std::vector<std::uint8_t> replacement_pixels;
    // Optional byte-exact restoration of a previously compressed entry.
    // Exclusive with replacement_pixels; fully decoded before committing.
    std::optional<std::vector<std::uint8_t>> replacement_encoded_entry;
};

// Program indexing is zero-based (first raw RES9 entry is program zero).
// Existing program word count must not change: a different-size program
// would need a separately proved companion-bank producer policy.
struct Res9ProgramPatch final {
    int program_id{};
    std::vector<std::uint16_t> expected_words;
    std::vector<std::uint16_t> replacement_words;
};

struct Res9Edits final {
    std::vector<Res9RasterPatch> rasters;
    std::vector<Res9ProgramPatch> programs;
};

// Decodes the original active 42-opcode wordcode grammar and checks that
// every direct branch/call/timer/choice target lands on an instruction start.
[[nodiscard]] std::vector<Res9Instruction> disassemble_res9_wordcode(
    const std::vector<std::uint16_t>& words);

class Res9Archive final {
public:
    explicit Res9Archive(std::vector<std::uint8_t> source);
    ~Res9Archive();
    Res9Archive(Res9Archive&&) noexcept;
    Res9Archive& operator=(Res9Archive&&) noexcept;
    Res9Archive(const Res9Archive&) = delete;
    Res9Archive& operator=(const Res9Archive&) = delete;

    [[nodiscard]] const Res9Stats& stats() const noexcept;
    [[nodiscard]] int entry_count() const noexcept;
    [[nodiscard]] int image_count() const noexcept;
    [[nodiscard]] int program_count() const noexcept;
    [[nodiscard]] std::vector<std::uint8_t> entry(int index) const;
    [[nodiscard]] std::vector<std::uint8_t> encode() const;
    [[nodiscard]] Res9RasterInfo raster_info(int image_index) const;
    [[nodiscard]] Type1Raster decode_raster(int image_index) const;
    [[nodiscard]] Res9Program program(int program_id) const;
    [[nodiscard]] std::vector<Res9Instruction> instructions(int program_id) const;

    // All changes are checked and applied in a single new archive build.
    // Unchanged compressed images and the entire companion banks are kept
    // byte-for-byte. Invalid program control flow fails without mutation.
    void apply(const Res9Edits& edits);

private:
    std::unique_ptr<ResArchive> archive_;
    int first_program_{};
    Res9Stats stats_{};
};

} // namespace rloft
