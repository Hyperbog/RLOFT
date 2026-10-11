#include "rloft_native/native_assets.hpp"
#include <algorithm>
#include <array>
#include <limits>
#include <stdexcept>

namespace rloft_native {
namespace {
std::uint32_t le32(const std::uint8_t* p) {
    return std::uint32_t(p[0]) | (std::uint32_t(p[1]) << 8) |
           (std::uint32_t(p[2]) << 16) | (std::uint32_t(p[3]) << 24);
}
}

NativeResArchive::NativeResArchive(std::vector<std::uint8_t> raw)
    : bytes_(std::move(raw)) {
    if (bytes_.size() < 4) throw std::runtime_error("original RES archive too short");
    const auto count = le32(bytes_.data());
    if (count > static_cast<std::uint32_t>(
            (bytes_.size() - 4) / 4))
        throw std::runtime_error("original RES archive offset table truncated");
    if (count > 100000)
        throw std::runtime_error("original RES record count implausibly large");
    const auto first = 4ULL + std::uint64_t(count) * 4ULL;
    std::uint32_t last = static_cast<std::uint32_t>(first);
    offsets_.reserve(count);
    for (std::uint32_t i = 0; i < count; ++i) {
        const auto pos = le32(bytes_.data() + 4 + i * 4);
        if ((i == 0 && pos != first) || pos < last || pos > bytes_.size())
            throw std::runtime_error("original RES entry offsets invalid");
        offsets_.push_back(pos);
        last = pos;
    }
}

std::vector<std::uint8_t> NativeResArchive::entry(std::size_t index) const {
    if (index >= offsets_.size())
        throw std::out_of_range("original RES record index outside archive");
    const auto begin = offsets_[index];
    const auto end = index + 1 < offsets_.size() ? offsets_[index + 1] :
        static_cast<std::uint32_t>(bytes_.size());
    return {bytes_.begin() + begin, bytes_.begin() + end};
}

NativePalette6 decode_native_palette(const std::vector<std::uint8_t>& bytes,
                                     std::size_t bank) {
    if (bytes.empty() || bytes[0] == 0 ||
        bytes.size() != 1ULL + std::size_t(bytes[0]) * 768ULL ||
        bank >= bytes[0])
        throw std::runtime_error("original COLORS bank count, range, or length invalid");
    NativePalette6 result{};
    const auto offset = 1 + bank * 768;
    std::copy_n(bytes.begin() + offset, result.rgb.size(), result.rgb.begin());
    if (std::any_of(result.rgb.begin(), result.rgb.end(),
                    [](std::uint8_t x) { return x > 63; }))
        throw std::runtime_error("original COLORS bank has non-DAC6 channel");
    return result;
}

NativeAssetReport inspect_native_assets(const NativeMedia& media) {
    NativeAssetReport result;
    result.media_kind = media.kind();
    const auto all = media.inventory();
    result.indexed_files = all.size();

    constexpr std::array<const char*, 19> names{{
        "INIT", "RES0", "T", "RES1", "RES2", "RES3", "RES4",
        "RES5", "RES6", "RES7", "RES8", "RES10", "COLORS",
        "LEVELS.DAT", "OBJECTS.DAT", "NPCS.DAT", "SINTAB",
        "CINE00.ANM", "CINE12.ANM"
    }};
    for (const auto* name : names) {
        NativeAssetCheck item;
        item.name = name;
        item.present = media.contains(name);
        if (!item.present) {
            ++result.missing;
            result.checks.push_back(std::move(item));
            continue;
        }
        const auto info = std::find_if(all.begin(), all.end(),
            [name](const NativeAssetInfo& entry) { return entry.path == name; });
        if (info != all.end()) item.bytes = info->size;
        try {
            const std::string key(name);
            if (key.size() >= 4 && key.substr(0, 3) == "RES" &&
                std::all_of(key.begin() + 3, key.end(),
                    [](char c) { return c >= '0' && c <= '9'; })) {
                NativeResArchive archive(media.read(name));
                item.res_entries = archive.entry_count();
                if (!item.res_entries)
                    throw std::runtime_error("RES has zero entries");
            } else if (key == "COLORS") {
                const auto colors = media.read(name);
                if (colors.empty() || colors[0] != 15 ||
                    colors.size() != 1 + 15 * 768)
                    throw std::runtime_error("COLORS must hold 15 original DAC banks");
                // Decode both gameplay and startup palette on original bytes.
                static_cast<void>(decode_native_palette(colors, 0));
                static_cast<void>(decode_native_palette(colors, 12));
            } else if (item.bytes == 0) {
                throw std::runtime_error("original required/candidate file is empty");
            }
            item.validated = true;
            ++result.valid;
        } catch (const std::exception& error) {
            item.issue = error.what();
            ++result.invalid;
        }
        result.checks.push_back(std::move(item));
    }
    return result;
}

} // namespace rloft_native
