#include "rloft/RES9.hpp"
#include "rloft/resource_assets.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <map>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft {
namespace {
constexpr std::size_t kEH1Header = 13U;
constexpr std::size_t kMaximumRasterBytes = 64U * 1024U * 1024U;
constexpr std::array<unsigned, 42> kArity{{
    0U,0U,0U,2U,2U,1U,1U,0U,2U,2U,1U,1U,1U,2U,
    0U,3U,3U,0U,1U,2U,1U,0U,1U,1U,1U,1U,2U,0U,
    1U,1U,1U,1U,1U,2U,2U,4U,1U,3U,3U,1U,1U,1U}};

std::uint16_t le16(const std::vector<std::uint8_t>& data, std::size_t at) {
    return static_cast<std::uint16_t>(data.at(at) |
        (static_cast<unsigned>(data.at(at + 1U)) << 8U));
}
void put32(std::vector<std::uint8_t>& data, std::size_t at, std::uint32_t word) {
    for (unsigned n = 0U; n != 4U; ++n)
        data.at(at + n) = static_cast<std::uint8_t>(word >> (8U * n));
}
bool is_raster(const std::vector<std::uint8_t>& e) {
    return e.size() >= 3U && e[0] == 'E' && e[1] == 'H' && e[2] == 1U;
}
Res9RasterInfo inspect_raster(const std::vector<std::uint8_t>& e) {
    if (!is_raster(e) || e.size() < kEH1Header || le16(e, 3U) != 8U)
        throw std::runtime_error("RES9 invalid EH type-1 descriptor");
    Res9RasterInfo info{};
    info.count = le16(e, 5U);
    info.height = le16(e, 7U);
    info.width = static_cast<std::uint16_t>(e[9U] * 8U);
    info.descriptor_field5 = e[10U];
    info.compression = e[11U];
    info.descriptor_field7 = e[12U];
    const auto decoded = std::uint64_t(info.count) * info.height * info.width;
    if (!info.count || info.count > 32767U || !info.height ||
        info.height > 32767U || !info.width || decoded > kMaximumRasterBytes ||
        (info.compression != 0U && info.compression != 3U) ||
        (info.compression == 0U && e.size() - kEH1Header != decoded) ||
        (info.compression == 3U && e.size() == kEH1Header))
        throw std::runtime_error("RES9 raster size or compression is invalid");
    info.decoded_bytes = static_cast<std::size_t>(decoded);
    return info;
}
Res9Program decode_program_container(const std::vector<std::uint8_t>& data) {
    if (data.size() < 2U) throw std::runtime_error("RES9 wordcode header missing");
    const auto count = static_cast<std::size_t>(le16(data, 0U));
    if (count == 0U || data.size() != 2U + count * 4U)
        throw std::runtime_error("RES9 program and companion lengths differ");
    Res9Program p;
    p.words.reserve(count);
    p.companion_words.reserve(count);
    for (std::size_t i = 0U; i < count; ++i)
        p.words.push_back(le16(data, 2U + i * 2U));
    for (std::size_t i = 0U; i < count; ++i)
        p.companion_words.push_back(le16(data, 2U + count * 2U + i * 2U));
    return p;
}
void require_target(const std::vector<bool>& starts, std::uint16_t target) {
    if (target >= starts.size() || !starts[target])
        throw std::runtime_error("RES9 branch target is not an instruction start");
}
} // namespace

std::vector<Res9Instruction> disassemble_res9_wordcode(
    const std::vector<std::uint16_t>& words,
    std::size_t opcode05_operand_words) {
    if (opcode05_operand_words != 1U && opcode05_operand_words != 141U)
        throw std::invalid_argument("RES9/RES12 unsupported opcode-05 dialect");
    std::vector<Res9Instruction> result;
    std::vector<bool> starts(words.size(), false);
    for (std::size_t ip = 0U; ip < words.size();) {
        const auto at = ip;
        const auto op = words[ip++];
        if (op >= kArity.size())
            throw std::runtime_error("RES9 wordcode opcode exceeds 0x29");
        const auto arguments_at = ip;
        if (op == 0U || op == 1U) {
            if (ip >= words.size()) throw std::runtime_error("RES9 choice count missing");
            const auto count = static_cast<std::size_t>(words[ip++]);
            if (count > words.size() - ip)
                throw std::runtime_error("RES9 choice list truncated");
            ip += count;
        } else if (op == 2U) {
            if (ip >= words.size()) throw std::runtime_error("RES9 dialog voice missing");
            ++ip;
            for (;;) {
                if (words.size() - ip < 2U)
                    throw std::runtime_error("RES9 dialog lacks default text pair");
                const auto condition = words[ip];
                ip += 2U;
                if (condition == 0x21U) break;
            }
        } else {
            const auto arity = op == 5U ? opcode05_operand_words
                                        : static_cast<std::size_t>(kArity[op]);
            if (arity > words.size() - ip)
                throw std::runtime_error("RES9 wordcode operands truncated");
            ip += arity;
        }
        starts[at] = true;
        result.push_back({at,op,std::vector<std::uint16_t>(
            words.begin() + static_cast<std::ptrdiff_t>(arguments_at),
            words.begin() + static_cast<std::ptrdiff_t>(ip))});
    }
    for (const auto& ins : result) {
        const auto& a = ins.operands;
        const auto op = ins.opcode;
        if (op == 1U) {
            for (std::size_t i = 1U; i < a.size(); ++i) require_target(starts,a[i]);
        } else if (op == 3U || op == 0x12U || op == 0x18U) {
            require_target(starts,a[0]);
        } else if (op == 8U || op == 9U || op == 0x0DU ||
                   op == 0x13U || op == 0x1AU || op == 0x21U || op == 0x22U) {
            require_target(starts,a[1]);
        } else if (op == 0x0FU) {
            if (a[0] > 4U) throw std::runtime_error("RES9 invalid timer slot");
            if (a[1] != 0xFFFFU) require_target(starts,a[1]);
        }
    }
    return result;
}

Res9Archive::Res9Archive(std::vector<std::uint8_t> source)
    : archive_(std::make_unique<ResArchive>(std::move(source))) {
    stats_.encoded_bytes = archive_->encode().size();
    first_program_ = archive_->entry_count();
    for (int i = 0; i < archive_->entry_count(); ++i) {
        const auto raw = archive_->entry(i);
        if (is_raster(raw)) {
            if (i >= first_program_)
                throw std::runtime_error("RES9 image after wordcode section");
            const auto descriptor = inspect_raster(raw);
            ++stats_.image_entries;
            stats_.decoded_pixel_bytes += descriptor.decoded_bytes;
            if (descriptor.compression == 0U) ++stats_.raw_images;
            else ++stats_.lzss_images;
        } else {
            if (i < first_program_) first_program_ = i;
            const auto p = decode_program_container(raw);
            stats_.program_words += p.words.size();
            stats_.instructions += disassemble_res9_wordcode(p.words).size();
            stats_.nonzero_companion_words += static_cast<std::size_t>(
                std::count_if(p.companion_words.begin(),p.companion_words.end(),
                    [](std::uint16_t word) {return word != 0U;}));
            ++stats_.programs;
        }
    }
    if (!stats_.image_entries || !stats_.programs)
        throw std::runtime_error("RES9 lacks raster or interaction wordcode bank");
}
Res9Archive::~Res9Archive() = default;
Res9Archive::Res9Archive(Res9Archive&&) noexcept = default;
Res9Archive& Res9Archive::operator=(Res9Archive&&) noexcept = default;

const Res9Stats& Res9Archive::stats() const noexcept { return stats_; }
int Res9Archive::entry_count() const noexcept {return archive_->entry_count();}
int Res9Archive::image_count() const noexcept {return first_program_;}
int Res9Archive::program_count() const noexcept {
    return archive_->entry_count() - first_program_;
}
std::vector<std::uint8_t> Res9Archive::entry(int index) const {
    return archive_->entry(index);
}
std::vector<std::uint8_t> Res9Archive::encode() const {
    return archive_->encode();
}
Res9RasterInfo Res9Archive::raster_info(int index) const {
    if (index < 0 || index >= image_count())
        throw std::out_of_range("RES9 image index");
    return inspect_raster(archive_->entry(index));
}
Type1Raster Res9Archive::decode_raster(int index) const {
    (void)raster_info(index);
    return archive_->decode_type1(index);
}
Res9Program Res9Archive::program(int id) const {
    if (id < 0 || id >= program_count())
        throw std::out_of_range("RES9 program ID");
    return decode_program_container(archive_->entry(first_program_ + id));
}
std::vector<Res9Instruction> Res9Archive::instructions(int id) const {
    return disassemble_res9_wordcode(program(id).words);
}

void Res9Archive::apply(const Res9Edits& edits) {
    if (edits.rasters.empty() && edits.programs.empty()) return;
    std::map<int,std::vector<std::uint8_t>> changed;
    for (const auto& patch : edits.rasters) {
        const auto info = raster_info(patch.image_index);
        if (changed.count(patch.image_index))
            throw std::invalid_argument("RES9 duplicate raster replacement");
        const auto original = entry(patch.image_index);
        if (original != patch.expected_entry)
            throw std::runtime_error("RES9 stale encoded raster");
        std::vector<std::uint8_t> replacement;
        if (patch.replacement_encoded_entry) {
            if (!patch.replacement_pixels.empty())
                throw std::invalid_argument("RES9 multiple raster replacement forms");
            replacement = *patch.replacement_encoded_entry;
            const auto restored = inspect_raster(replacement);
            if (restored.count != info.count || restored.height != info.height ||
                restored.width != info.width ||
                restored.descriptor_field5 != info.descriptor_field5 ||
                restored.descriptor_field7 != info.descriptor_field7)
                throw std::invalid_argument("RES9 restored raster descriptor changed");
        } else {
            if (patch.replacement_pixels.size() != info.decoded_bytes)
                throw std::invalid_argument("RES9 replacement raster size mismatch");
            replacement = original;
            replacement.resize(kEH1Header);
            replacement[11U] = 0U; // CODE.2 raw type-1 method; other fields retained.
            replacement.insert(replacement.end(), patch.replacement_pixels.begin(),
                               patch.replacement_pixels.end());
        }
        // The already-shared decoder must accept even a restored compressed
        // raster; this rejects malformed LZSS rather than just its envelope.
        std::vector<std::uint8_t> single(8U, 0U);
        put32(single,0U,1U);
        put32(single,4U,8U);
        single.insert(single.end(),replacement.begin(),replacement.end());
        ResArchive trial(std::move(single));
        (void)trial.decode_type1(0);
        changed.emplace(patch.image_index,std::move(replacement));
    }
    for (const auto& patch : edits.programs) {
        if (patch.program_id < 0 || patch.program_id >= program_count())
            throw std::out_of_range("RES9 program ID");
        const auto archive_index = first_program_ + patch.program_id;
        if (changed.count(archive_index))
            throw std::invalid_argument("RES9 duplicate program replacement");
        const auto original = program(patch.program_id);
        if (original.words != patch.expected_words)
            throw std::runtime_error("RES9 stale wordcode program");
        if (patch.replacement_words.size() != original.words.size())
            throw std::invalid_argument("RES9 edit changes companion-bank length");
        (void)disassemble_res9_wordcode(patch.replacement_words);
        auto revised = entry(archive_index); // Preserve original second bank.
        for (std::size_t i = 0; i < patch.replacement_words.size(); ++i) {
            revised[2U + 2U*i] = static_cast<std::uint8_t>(patch.replacement_words[i]);
            revised[3U + 2U*i] = static_cast<std::uint8_t>(patch.replacement_words[i] >> 8U);
        }
        changed.emplace(archive_index,std::move(revised));
    }

    const auto count = static_cast<std::size_t>(entry_count());
    const auto directory_bytes = 4U + count * 4U;
    constexpr auto max32 = std::numeric_limits<std::uint32_t>::max();
    std::vector<std::uint8_t> rebuilt(directory_bytes,0U);
    put32(rebuilt,0U,static_cast<std::uint32_t>(count));
    for (std::size_t i = 0U; i < count; ++i) {
        const auto it = changed.find(static_cast<int>(i));
        auto part = it == changed.end() ? entry(static_cast<int>(i)) : it->second;
        if (part.size() > max32 - rebuilt.size())
            throw std::overflow_error("RES9 archive offsets exceed 32 bits");
        put32(rebuilt,4U + i*4U,static_cast<std::uint32_t>(rebuilt.size()));
        rebuilt.insert(rebuilt.end(),part.begin(),part.end());
    }
    Res9Archive validated(std::move(rebuilt));
    archive_.swap(validated.archive_);
    stats_ = validated.stats_;
    first_program_ = validated.first_program_;
}

} // namespace rloft
