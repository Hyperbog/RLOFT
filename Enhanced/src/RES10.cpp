#include "rloft/RES10.hpp"
#include "rloft/RES6.hpp"
#include "rloft/resource_assets.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <map>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft {
namespace {
constexpr std::size_t kEnvelopeBytes = 9U;
constexpr std::size_t kVocHeaderBytes = 26U;
constexpr std::size_t kPcmStart = kEnvelopeBytes + kVocHeaderBytes + 6U;
constexpr std::uint64_t kLargestBlock = 0xFFFFFFULL;

std::uint16_t read16(const std::vector<std::uint8_t>& b, std::size_t at) {
    return static_cast<std::uint16_t>(b.at(at) |
        (static_cast<unsigned>(b.at(at + 1U)) << 8U));
}
std::uint32_t read24(const std::vector<std::uint8_t>& b, std::size_t at) {
    return static_cast<std::uint32_t>(b.at(at)) |
        (static_cast<std::uint32_t>(b.at(at + 1U)) << 8U) |
        (static_cast<std::uint32_t>(b.at(at + 2U)) << 16U);
}
std::uint32_t read32(const std::vector<std::uint8_t>& b, std::size_t at) {
    return read24(b, at) |
        (static_cast<std::uint32_t>(b.at(at + 3U)) << 24U);
}
void put24(std::vector<std::uint8_t>& b, std::size_t at, std::uint32_t x) {
    for (unsigned i=0U; i<3U; ++i)
        b.at(at + i) = static_cast<std::uint8_t>(x >> (8U * i));
}
void put32(std::vector<std::uint8_t>& b, std::size_t at, std::uint32_t x) {
    for (unsigned i=0U; i<4U; ++i)
        b.at(at + i) = static_cast<std::uint8_t>(x >> (8U * i));
}
Res10VoiceInfo inspect_entry(const std::vector<std::uint8_t>& e) {
    if (e.size() < kPcmStart + 2U || e[0] != 'E' || e[1] != 'H' ||
        e[2] != 4U || read16(e, 3U) != 0U ||
        read32(e, 5U) != e.size() - kEnvelopeBytes)
        throw std::runtime_error("RES10 invalid EH type-4 envelope");
    const std::vector<std::uint8_t> voice(
        e.begin() + static_cast<std::ptrdiff_t>(kEnvelopeBytes), e.end());
    const auto parsed = inspect_res6_voc(voice); // Shared original VOC grammar.
    const auto pcm_count = e.size() - kPcmStart - 1U;
    if (read16(voice, 20U) != kVocHeaderBytes ||
        parsed.blocks != 1U || parsed.sound_blocks != 1U ||
        parsed.continuation_blocks != 0U || parsed.padding_bytes != 0U ||
        voice[26U] != 1U || voice[31U] != 0U ||
        read24(voice, 27U) != pcm_count + 2U || e.back() != 0U)
        throw std::runtime_error("RES10 requires one terminated mono PCM8 VOC block");
    Res10VoiceInfo result{};
    result.voc_version = parsed.version;
    result.time_constant = voice[30U];
    result.codec = voice[31U];
    result.nominal_sample_rate_hz = 1000000U /
        (256U - static_cast<unsigned>(result.time_constant));
    result.voc_bytes = voice.size();
    result.pcm_samples = pcm_count;
    result.duration_microseconds = static_cast<std::uint64_t>(pcm_count) *
        (256U - static_cast<unsigned>(result.time_constant));
    return result;
}
std::vector<std::uint8_t> with_pcm(
    const std::vector<std::uint8_t>& original,
    const std::vector<std::uint8_t>& samples) {
    (void)inspect_entry(original);
    if (samples.empty() || samples.size() > kLargestBlock - 2U)
        throw std::invalid_argument("RES10 PCM8 sample count outside VOC type-1 limit");
    std::vector<std::uint8_t> e(
        original.begin(), original.begin() + static_cast<std::ptrdiff_t>(kPcmStart));
    e.insert(e.end(), samples.begin(), samples.end());
    e.push_back(0U);
    put24(e, kEnvelopeBytes + kVocHeaderBytes + 1U,
          static_cast<std::uint32_t>(samples.size() + 2U));
    put32(e, 5U, static_cast<std::uint32_t>(e.size() - kEnvelopeBytes));
    (void)inspect_entry(e);
    return e;
}
} // namespace

Res10Archive::Res10Archive(std::vector<std::uint8_t> source)
    : archive_(std::make_unique<ResArchive>(std::move(source))) {
    if (archive_->entry_count() <= 0)
        throw std::runtime_error("RES10 has no voice entries");
    stats_.entries = static_cast<std::size_t>(archive_->entry_count());
    stats_.shortest_sample_count = std::numeric_limits<std::size_t>::max();
    for (int i=0; i<archive_->entry_count(); ++i) {
        const auto info = inspect_entry(archive_->entry(i));
        stats_.voc_bytes += static_cast<std::uint64_t>(info.voc_bytes);
        stats_.pcm_samples += static_cast<std::uint64_t>(info.pcm_samples);
        stats_.duration_microseconds += info.duration_microseconds;
        stats_.shortest_sample_count = std::min(
            stats_.shortest_sample_count, info.pcm_samples);
        stats_.longest_sample_count = std::max(
            stats_.longest_sample_count, info.pcm_samples);
    }
}
Res10Archive::~Res10Archive() = default;
Res10Archive::Res10Archive(Res10Archive&&) noexcept = default;
Res10Archive& Res10Archive::operator=(Res10Archive&&) noexcept = default;
int Res10Archive::entry_count() const noexcept { return archive_->entry_count(); }
const Res10ArchiveStats& Res10Archive::stats() const noexcept { return stats_; }
std::vector<std::uint8_t> Res10Archive::entry(int index) const {
    return archive_->entry(index);
}
std::vector<std::uint8_t> Res10Archive::encode() const {
    return archive_->encode();
}
Res10VoiceInfo Res10Archive::voice_info(int index) const {
    return inspect_entry(archive_->entry(index));
}
std::vector<std::uint8_t> Res10Archive::voc(int index) const {
    return decode_res6_voc(*archive_, index);
}
std::vector<std::uint8_t> Res10Archive::pcm8(int index) const {
    const auto raw = archive_->entry(index);
    (void)inspect_entry(raw);
    return {raw.begin() + static_cast<std::ptrdiff_t>(kPcmStart), raw.end() - 1};
}
PcmSound Res10Archive::pcm16(int index) const {
    return decode_creative_voice(voc(index)); // Existing native audio codec.
}
void Res10Archive::apply(const std::vector<Res10VoicePatch>& edits) {
    if (edits.empty()) return;
    std::map<int,std::vector<std::uint8_t>> replacement;
    for (const auto& patch : edits) {
        if (patch.index < 0 || patch.index >= entry_count())
            throw std::out_of_range("RES10 voice index");
        if (replacement.count(patch.index))
            throw std::invalid_argument("RES10 duplicate voice edit");
        const auto original = archive_->entry(patch.index);
        if (patch.expected_entry != original)
            throw std::runtime_error("RES10 voice source changed since patch was made");
        replacement.emplace(patch.index,
                            with_pcm(original, patch.replacement_pcm8));
    }
    const auto source = archive_->encode();
    const auto count = static_cast<std::size_t>(entry_count());
    const auto maximum = static_cast<std::uint64_t>(
        std::numeric_limits<std::uint32_t>::max());
    const auto table_bytes = 4U + 4U * count;
    std::uint64_t total = table_bytes;
    for (std::size_t i=0; i<count; ++i) {
        const auto begin = static_cast<std::size_t>(read32(source, 4U + 4U*i));
        const auto end = i+1U<count
            ? static_cast<std::size_t>(read32(source, 4U + 4U*(i+1U)))
            : source.size();
        const auto found = replacement.find(static_cast<int>(i));
        total += found == replacement.end() ? end - begin : found->second.size();
        if (total > maximum)
            throw std::overflow_error("RES10 archive exceeds 32-bit offsets");
    }
    std::vector<std::uint8_t> rebuilt(static_cast<std::size_t>(total), 0U);
    put32(rebuilt, 0U, static_cast<std::uint32_t>(count));
    std::size_t cursor = table_bytes;
    for (std::size_t i=0; i<count; ++i) {
        put32(rebuilt, 4U + 4U*i, static_cast<std::uint32_t>(cursor));
        const auto found = replacement.find(static_cast<int>(i));
        if (found == replacement.end()) {
            const auto begin = static_cast<std::size_t>(read32(source, 4U + 4U*i));
            const auto end = i+1U<count
                ? static_cast<std::size_t>(read32(source, 4U + 4U*(i+1U)))
                : source.size();
            std::copy(source.begin() + static_cast<std::ptrdiff_t>(begin),
                      source.begin() + static_cast<std::ptrdiff_t>(end),
                      rebuilt.begin() + static_cast<std::ptrdiff_t>(cursor));
            cursor += end - begin;
        } else {
            const auto& e = found->second;
            std::copy(e.begin(), e.end(),
                      rebuilt.begin() + static_cast<std::ptrdiff_t>(cursor));
            cursor += e.size();
        }
    }
    Res10Archive validated(std::move(rebuilt));
    *this = std::move(validated); // Single atomic commit after full validation.
}
} // namespace rloft
