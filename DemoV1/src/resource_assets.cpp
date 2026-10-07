#include "rloft/resource_assets.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace rloft {
namespace {

std::uint16_t u16(const std::uint8_t* p) {
    return static_cast<std::uint16_t>(p[0]) |
           (static_cast<std::uint16_t>(p[1]) << 8U);
}

std::int16_t s16(const std::uint8_t* p) {
    return static_cast<std::int16_t>(u16(p));
}

std::uint32_t u32(const std::uint8_t* p) {
    return static_cast<std::uint32_t>(p[0]) |
           (static_cast<std::uint32_t>(p[1]) << 8U) |
           (static_cast<std::uint32_t>(p[2]) << 16U) |
           (static_cast<std::uint32_t>(p[3]) << 24U);
}

std::vector<std::uint8_t> lzss_4k(const std::uint8_t* src,
                                  std::size_t src_size,
                                  std::size_t target_size) {
    std::array<int, 4096> history{};
    history.fill(-1);
    for (std::size_t i = 0; i < 0xFEE; ++i) history[i] = 0xFE;

    std::size_t hp = 0xFEE;
    std::size_t ip = 0;
    std::uint32_t flags = 0;
    std::vector<std::uint8_t> out;
    out.reserve(target_size);

    while (out.size() < target_size) {
        flags >>= 1U;
        if ((flags & 0x100U) == 0) {
            if (ip >= src_size) throw std::runtime_error("LZSS control EOF");
            flags = static_cast<std::uint32_t>(src[ip++]) | 0xFF00U;
        }

        if ((flags & 1U) != 0) {
            if (ip >= src_size) throw std::runtime_error("LZSS literal EOF");
            const auto value = src[ip++];
            out.push_back(value);
            history[hp] = value;
            hp = (hp + 1U) & 0xFFFU;
        } else {
            if (ip + 1U >= src_size) throw std::runtime_error("LZSS backref EOF");
            const auto lo = src[ip++];
            const auto hi = src[ip++];
            const auto offset = static_cast<std::size_t>(lo) |
                                ((static_cast<std::size_t>(hi) & 0xF0U) << 4U);
            const auto count = static_cast<std::size_t>(hi & 0x0FU) + 3U;
            for (std::size_t j = 0; j < count && out.size() < target_size; ++j) {
                const auto source = (offset + j) & 0xFFFU;
                const int stored = history[source];
                if (stored < 0) {
                    throw std::runtime_error("LZSS read of undefined CODE.2 dictionary tail");
                }
                const auto value = static_cast<std::uint8_t>(stored);
                out.push_back(value);
                history[hp] = value;
                hp = (hp + 1U) & 0xFFFU;
            }
        }
    }

    return out;
}

}  // namespace

std::vector<EnvironmentProfile> decode_environment_profiles(
    const std::vector<std::uint8_t>& init_bytes) {
    constexpr std::size_t kProfileCountAt = 0x0545U;
    constexpr std::size_t kProfilesAt = 0x0547U;
    if (init_bytes.size() < kProfilesAt) {
        throw std::runtime_error("INIT is truncated before environment profiles");
    }

    const auto profile_count =
        static_cast<std::size_t>(u16(init_bytes.data() + kProfileCountAt));
    std::size_t pos = kProfilesAt;
    std::vector<EnvironmentProfile> out;
    out.reserve(profile_count);

    for (std::size_t profile_index = 0;
         profile_index < profile_count;
         ++profile_index) {
        if (pos + 4U > init_bytes.size()) {
            throw std::runtime_error("INIT environment profile header exceeds EOF");
        }

        EnvironmentProfile profile{};
        profile.res8_base = u16(init_bytes.data() + pos);
        profile.stored_record_count = u16(init_bytes.data() + pos + 2U);
        pos += 4U;

        if (profile.stored_record_count == 0) {
            throw std::runtime_error("INIT environment profile has no terminator record");
        }

        const auto bytes_needed =
            static_cast<std::size_t>(profile.stored_record_count) * 8U;
        if (pos > init_bytes.size() ||
            bytes_needed > init_bytes.size() - pos) {
            throw std::runtime_error("INIT environment placement list exceeds EOF");
        }

        profile.placements.reserve(profile.stored_record_count);
        for (std::size_t i = 0; i < profile.stored_record_count; ++i) {
            EnvironmentPlacement placement{};
            placement.resource_selector =
                s16(init_bytes.data() + pos + 0U);
            placement.azimuth_degrees =
                s16(init_bytes.data() + pos + 2U);
            placement.draw_width_pixels =
                s16(init_bytes.data() + pos + 4U);
            placement.screen_y =
                s16(init_bytes.data() + pos + 6U);
            pos += 8U;

            if (placement.resource_selector == 0 &&
                !placement.terminator()) {
                throw std::runtime_error(
                    "INIT environment terminator selector has nonzero payload");
            }
            if (placement.terminator() &&
                i + 1U != profile.stored_record_count) {
                throw std::runtime_error(
                    "INIT environment profile has premature terminator");
            }

            profile.placements.push_back(placement);
        }

        if (!profile.placements.back().terminator()) {
            throw std::runtime_error(
                "INIT environment profile lacks final zero terminator");
        }
        out.push_back(std::move(profile));
    }

    if (pos != init_bytes.size()) {
        throw std::runtime_error(
            "INIT contains bytes after environment profile table");
    }
    return out;
}

int resolve_environment_res8_id(
    const EnvironmentProfile& profile,
    const EnvironmentPlacement& placement) {
    if (placement.terminator()) {
        throw std::invalid_argument(
            "environment terminator has no RES8 resource");
    }
    const int selector = placement.resource_selector;
    if (selector < 0) {
        throw std::out_of_range("negative environment resource selector");
    }
    return selector >= 8
        ? static_cast<int>(profile.res8_base) + selector
        : selector;
}

ResArchive::ResArchive(std::vector<std::uint8_t> bytes)
    : bytes_(std::move(bytes)) {
    if (bytes_.size() < 4) throw std::runtime_error("RES archive is too small");
    const auto count = static_cast<std::int32_t>(u32(bytes_.data()));
    if (count < 0) throw std::runtime_error("RES archive has negative entry count");
    const auto table_end = 4ULL + static_cast<unsigned long long>(count) * 4ULL;
    if (table_end > bytes_.size()) throw std::runtime_error("RES offset table exceeds EOF");

    offsets_.reserve(static_cast<std::size_t>(count));
    std::uint32_t previous = static_cast<std::uint32_t>(table_end);
    for (int i = 0; i < count; ++i) {
        const auto off = u32(bytes_.data() + 4 + i * 4);
        if ((i == 0 && off != table_end) || off < previous || off > bytes_.size()) {
            throw std::runtime_error("RES archive has invalid entry offsets");
        }
        offsets_.push_back(off);
        previous = off;
    }
}

int ResArchive::entry_count() const noexcept {
    return static_cast<int>(offsets_.size());
}

std::vector<std::uint8_t> ResArchive::entry(int index) const {
    if (index < 0 || index >= entry_count()) throw std::out_of_range("RES entry index");
    const auto start = offsets_[static_cast<std::size_t>(index)];
    const auto end = (index + 1 < entry_count())
                         ? offsets_[static_cast<std::size_t>(index + 1)]
                         : static_cast<std::uint32_t>(bytes_.size());
    return {bytes_.begin() + start, bytes_.begin() + end};
}

Type2Font ResArchive::decode_type2(int index) const {
    const auto raw = entry(index);
    if (raw.size() < 5U + 136U ||
        raw[0] != 'E' || raw[1] != 'H' || raw[2] != 2) {
        throw std::runtime_error("RES entry is not an EH type-2 font");
    }

    const auto header_size = s16(raw.data() + 3);
    if (header_size < 136 ||
        5 + static_cast<std::size_t>(header_size) > raw.size()) {
        throw std::runtime_error("Invalid type-2 font header size");
    }

    Type2Font font;
    std::copy_n(raw.begin() + 5, font.widths.size(), font.widths.begin());

    const auto* desc = raw.data() + 5 + 128;
    const int count = s16(desc + 0);
    const int height = s16(desc + 2);
    const int width_units = desc[4];
    const auto key = desc[5];
    const auto compression = desc[6];
    if (count <= 0 || height <= 0 || width_units <= 0) {
        throw std::runtime_error("Invalid type-2 font raster dimensions");
    }

    const int width = width_units * 8;
    const auto expected = static_cast<std::size_t>(count) *
                          static_cast<std::size_t>(height) *
                          static_cast<std::size_t>(width);
    const auto payload_at = 5U + static_cast<std::size_t>(header_size);
    const auto* payload = raw.data() + payload_at;
    const auto payload_size = raw.size() - payload_at;

    std::vector<std::uint8_t> pixels;
    if (compression == 0) {
        if (payload_size != expected) {
            throw std::runtime_error("Raw type-2 font raster size mismatch");
        }
        pixels.assign(payload, payload + payload_size);
    } else if (compression == 3) {
        pixels = lzss_4k(payload, payload_size, expected);
    } else {
        throw std::runtime_error("Unsupported type-2 font compression");
    }

    font.raster =
        Type1Raster{count, width, height, key, std::move(pixels)};
    return font;
}

std::vector<std::uint8_t> ResArchive::decode_type3(int index) const {
    const auto raw = entry(index);
    if (raw.size() < 9 ||
        raw[0] != 'E' || raw[1] != 'H' || raw[2] != 3) {
        throw std::runtime_error("RES entry is not an EH type-3 XMIDI sequence");
    }

    const auto header_size = s16(raw.data() + 3);
    if (header_size != 0) {
        throw std::runtime_error("Shipped type-3 resource has unexpected header");
    }

    const auto declared = static_cast<std::size_t>(u32(raw.data() + 5));
    const auto available = raw.size() - 9U;
    if (declared != available) {
        throw std::runtime_error("Type-3 XMIDI length does not match RES entry");
    }

    return {
        raw.begin() + 9,
        raw.end()
    };
}

std::vector<std::uint8_t> ResArchive::decode_type4(int index) const {
    const auto raw = entry(index);
    if (raw.size() < 9 ||
        raw[0] != 'E' || raw[1] != 'H' || raw[2] != 4) {
        throw std::runtime_error("RES entry is not an EH type-4 sampled sound");
    }

    const auto header_size = s16(raw.data() + 3);
    if (header_size != 0) {
        throw std::runtime_error("Shipped type-4 resource has unexpected header");
    }

    const auto declared = static_cast<std::size_t>(u32(raw.data() + 5));
    const auto available = raw.size() - 9U;
    if (declared > available) {
        throw std::runtime_error("Type-4 VOC length exceeds active RES entry");
    }

    return {
        raw.begin() + 9,
        raw.begin() + 9 + static_cast<std::ptrdiff_t>(declared)
    };
}

Type1Raster ResArchive::decode_type1(int index) const {
    const auto raw = entry(index);
    if (raw.size() < 13 || raw[0] != 'E' || raw[1] != 'H' || raw[2] != 1) {
        throw std::runtime_error("RES entry is not an EH type-1 raster");
    }

    const auto header_size = s16(raw.data() + 3);
    if (header_size < 8 || 5 + static_cast<std::size_t>(header_size) > raw.size()) {
        throw std::runtime_error("Invalid type-1 header size");
    }

    const auto* desc = raw.data() + 5;
    const int count = s16(desc + 0);
    const int height = s16(desc + 2);
    const int width_units = desc[4];
    const auto key = desc[5];
    const auto compression = desc[6];
    const auto transform = desc[7];
    if (count <= 0 || height <= 0 || width_units <= 0) {
        throw std::runtime_error("Invalid type-1 raster dimensions");
    }

    const int width = width_units * 8;
    const auto expected = static_cast<std::size_t>(count) *
                          static_cast<std::size_t>(height) *
                          static_cast<std::size_t>(width);
    const auto payload_at = 5U + static_cast<std::size_t>(header_size);
    const auto* payload = raw.data() + payload_at;
    const auto payload_size = raw.size() - payload_at;

    std::vector<std::uint8_t> pixels;
    if (compression == 0) {
        if (payload_size != expected) throw std::runtime_error("Raw type-1 raster size mismatch");
        pixels.assign(payload, payload + payload_size);
    } else if (compression == 3) {
        pixels = lzss_4k(payload, payload_size, expected);
    } else {
        throw std::runtime_error("Unsupported type-1 compression in shipped boot resource");
    }

    // CODE.1 767C calls DC90 when source descriptor byte +7 is nonzero.
    // DC90 acts only on one-row images: it transposes the encoded
    // [element][plane] matrix into [plane][row], then rewrites the runtime
    // descriptor from N elements x 1 row to 1 element x N rows. In this
    // host-side chunky representation the decoded source bytes are already
    // laid out as consecutive visual rows, so the equivalent transformation
    // is purely descriptor geometry; pixel order is unchanged.
    if (transform != 0 && height == 1) {
        return Type1Raster{1, width, count, key, std::move(pixels)};
    }

    return Type1Raster{count, width, height, key, std::move(pixels)};
}

RavenloftBootAssets::RavenloftBootAssets(GameSource source)
    : source_(std::move(source)),
      res0_(source_.read("RES0")),
      res1_(source_.read("RES1")),
      res5_(source_.read("RES5")),
      res6_(source_.read("RES6")),
      res7_(source_.read("RES7")),
      res8_(source_.read("RES8")),
      res10_(source_.read("RES10")),
      colors_(source_.read("COLORS")),
      text_(source_.read("T")),
      init_(source_.read("INIT")),
      code1_(source_.read("CODE.1")) {
    if (colors_.size() != 1U + 15U * 768U || colors_[0] != 15U) {
        throw std::runtime_error("COLORS is not the certified 15-palette Ravenloft bank");
    }
    // Certified CODE.1 baseline from Runtime/CODE_OBJECT_MAP.md.
    // The active data object begins at file 0x3E724.
    if (code1_.size() != 0x43D1BU) {
        throw std::runtime_error(
            "CODE.1 does not match the certified Ravenloft 1.2a executable size");
    }
}

Type1Raster RavenloftBootAssets::type1(int archive, int index) const {
    switch (archive) {
        case 0: return res0_.decode_type1(index);
        case 1: return res1_.decode_type1(index);
        case 7: return res7_.decode_type1(index);
        case 8: return res8_.decode_type1(index);
        default:
            throw std::out_of_range("Demo v1 has no loaded type-1 archive for requested bank");
    }
}

Type1Raster RavenloftBootAssets::res0_type1(int index) const {
    return type1(0, index);
}

Type2Font RavenloftBootAssets::res0_font(int index) const {
    return res0_.decode_type2(index);
}

std::vector<std::uint8_t> RavenloftBootAssets::music_sequence(
    int index) const {
    return res5_.decode_type3(index);
}

std::vector<std::uint8_t> RavenloftBootAssets::sampled_audio(
    int archive, int index) const {
    switch (archive) {
        case 6: return res6_.decode_type4(index);
        case 10: return res10_.decode_type4(index);
        default:
            throw std::out_of_range(
                "Requested archive is not an active sampled-audio bank");
    }
}

std::string RavenloftBootAssets::text(int index) const {
    if (text_.size() < 4) {
        throw std::runtime_error("T text archive is truncated");
    }
    const auto count = static_cast<std::int32_t>(u32(text_.data()));
    if (index < 0 || index >= count) {
        throw std::out_of_range("T text record index");
    }

    const auto table_at = 4U + static_cast<std::size_t>(index) * 4U;
    if (table_at + 4U > text_.size()) {
        throw std::runtime_error("T text index exceeds archive");
    }
    const auto record_at = static_cast<std::size_t>(u32(text_.data() + table_at));
    if (record_at + 2U > text_.size()) {
        throw std::runtime_error("T text record offset exceeds archive");
    }
    const auto length = static_cast<std::size_t>(u16(text_.data() + record_at));
    if (record_at + 2U + length > text_.size()) {
        throw std::runtime_error("T text record payload exceeds archive");
    }

    std::string out(length, '\0');
    for (std::size_t i = 0; i < length; ++i) {
        out[i] = static_cast<char>(text_[record_at + 2U + i] ^ 0xDCU);
    }
    return out;
}

std::uint16_t RavenloftBootAssets::init_u16(
    std::size_t byte_offset) const {
    if (byte_offset + 2U > init_.size()) {
        throw std::out_of_range("INIT word exceeds installed file");
    }
    return static_cast<std::uint16_t>(init_[byte_offset]) |
           (static_cast<std::uint16_t>(init_[byte_offset + 1U]) << 8U);
}

std::vector<EnvironmentProfile> RavenloftBootAssets::environment_profiles() const {
    return decode_environment_profiles(init_);
}

std::uint8_t RavenloftBootAssets::code1_u8(
    std::size_t dgroup_offset) const {
    constexpr std::size_t kObject2FileStart = 0x3E724U;
    const auto at = kObject2FileStart + dgroup_offset;
    if (at >= code1_.size()) {
        throw std::out_of_range("CODE.1 DGROUP byte exceeds stored object");
    }
    return code1_[at];
}

std::uint16_t RavenloftBootAssets::code1_u16(
    std::size_t dgroup_offset) const {
    const std::uint16_t lo = code1_u8(dgroup_offset);
    const std::uint16_t hi = code1_u8(dgroup_offset + 1U);
    return static_cast<std::uint16_t>(lo | (hi << 8U));
}

std::int32_t RavenloftBootAssets::code1_s32(
    std::size_t dgroup_offset) const {
    const auto b0 = static_cast<std::uint32_t>(code1_u8(dgroup_offset));
    const auto b1 = static_cast<std::uint32_t>(code1_u8(dgroup_offset + 1U));
    const auto b2 = static_cast<std::uint32_t>(code1_u8(dgroup_offset + 2U));
    const auto b3 = static_cast<std::uint32_t>(code1_u8(dgroup_offset + 3U));
    const std::uint32_t raw =
        b0 | (b1 << 8U) | (b2 << 16U) | (b3 << 24U);
    return static_cast<std::int32_t>(raw);
}

std::string RavenloftBootAssets::code1_cstring(
    std::size_t dgroup_offset) const {
    constexpr std::size_t kObject2FileStart = 0x3E724U;
    const auto start = kObject2FileStart + dgroup_offset;
    if (start >= code1_.size()) {
        throw std::out_of_range("CODE.1 DGROUP string exceeds stored object");
    }

    std::string out;
    for (std::size_t at = start; at < code1_.size(); ++at) {
        const auto ch = code1_[at];
        if (ch == 0) return out;
        out.push_back(static_cast<char>(ch));
    }
    throw std::runtime_error("CODE.1 DGROUP string is unterminated");
}

Palette6 RavenloftBootAssets::colors_palette(int index, int brightness) const {
    if (index < 0 || index >= 15) throw std::out_of_range("COLORS palette index");
    brightness = std::clamp(brightness, 0, 63);

    Palette6 result;
    const auto start = 1U + static_cast<std::size_t>(index) * 768U;
    std::copy_n(colors_.begin() + static_cast<std::ptrdiff_t>(start), 768, result.rgb.begin());

    if (brightness == 0) return result;
    for (std::size_t i = 0; i < 256; ++i) {
        auto* rgb = result.rgb.data() + i * 3;
        const int peak = std::max({static_cast<int>(rgb[0]), static_cast<int>(rgb[1]), static_cast<int>(rgb[2])});
        if (peak == 0) continue;
        const int boost = ((63 - peak) * brightness + 31) / 63;
        const int target = peak + boost;
        for (int c = 0; c < 3; ++c) {
            rgb[c] = static_cast<std::uint8_t>((static_cast<int>(rgb[c]) * target + peak / 2) / peak);
        }
    }
    return result;
}

}  // namespace rloft
