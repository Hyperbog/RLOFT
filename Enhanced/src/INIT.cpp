#include "rloft/INIT.hpp"

#include <limits>
#include <stdexcept>
#include <utility>

namespace rloft {
namespace {

std::uint16_t read_u16(const std::vector<std::uint8_t>& bytes,
                       std::size_t& pos) {
    if (pos > bytes.size() || bytes.size() - pos < 2) {
        throw std::runtime_error("INIT truncated while reading 16-bit value");
    }
    const auto value = static_cast<std::uint16_t>(
        static_cast<unsigned>(bytes[pos]) |
        (static_cast<unsigned>(bytes[pos + 1]) << 8U));
    pos += 2;
    return value;
}

std::int16_t read_i16(const std::vector<std::uint8_t>& bytes,
                      std::size_t& pos) {
    const auto raw = read_u16(bytes, pos);
    const int signed_value = raw <= 0x7FFFU ? static_cast<int>(raw)
                                            : static_cast<int>(raw) - 0x10000;
    return static_cast<std::int16_t>(signed_value);
}

void write_u16(std::vector<std::uint8_t>& out, std::uint16_t value) {
    out.push_back(static_cast<std::uint8_t>(value));
    out.push_back(static_cast<std::uint8_t>(value >> 8U));
}

void write_i16(std::vector<std::uint8_t>& out, std::int16_t value) {
    write_u16(out, static_cast<std::uint16_t>(value));
}

void write_count(std::vector<std::uint8_t>& out, std::size_t count) {
    if (count > std::numeric_limits<std::uint16_t>::max()) {
        throw std::invalid_argument("INIT table exceeds 16-bit count");
    }
    write_u16(out, static_cast<std::uint16_t>(count));
}

std::vector<InitSceneDescriptor> read_descriptors(
    const std::vector<std::uint8_t>& bytes, std::size_t& pos) {
    const auto count = read_u16(bytes, pos);
    if (pos > bytes.size() ||
        static_cast<std::size_t>(count) > (bytes.size() - pos) / 7U) {
        throw std::runtime_error("INIT descriptor table exceeds EOF");
    }
    std::vector<InitSceneDescriptor> out;
    out.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
        InitSceneDescriptor desc{};
        desc.local_shape_base = read_u16(bytes, pos);
        desc.spatial_extent = read_u16(bytes, pos);
        desc.shape_stride = bytes[pos++];
        desc.variant_count = bytes[pos++];
        desc.scene_influence_step = bytes[pos++];
        out.push_back(desc);
    }
    return out;
}

void write_descriptors(std::vector<std::uint8_t>& out,
                       const std::vector<InitSceneDescriptor>& table) {
    write_count(out, table.size());
    for (const auto& desc : table) {
        write_u16(out, desc.local_shape_base);
        write_u16(out, desc.spatial_extent);
        out.push_back(desc.shape_stride);
        out.push_back(desc.variant_count);
        out.push_back(desc.scene_influence_step);
    }
}

std::vector<EnvironmentProfile> read_profiles(
    const std::vector<std::uint8_t>& bytes, std::size_t& pos) {
    const auto count = read_u16(bytes, pos);
    // Each profile has a four-byte header and at least one terminator record.
    if (static_cast<std::size_t>(count) > (bytes.size() - pos) / 12U) {
        throw std::runtime_error("INIT environment headers exceed EOF");
    }
    std::vector<EnvironmentProfile> profiles;
    profiles.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
        EnvironmentProfile profile{};
        profile.res8_base = read_u16(bytes, pos);
        profile.stored_record_count = read_u16(bytes, pos);
        if (profile.stored_record_count == 0 ||
            static_cast<std::size_t>(profile.stored_record_count) >
                (bytes.size() - pos) / 8U) {
            throw std::runtime_error("INIT environment placement count invalid");
        }
        profile.placements.reserve(profile.stored_record_count);
        for (std::size_t j = 0; j < profile.stored_record_count; ++j) {
            EnvironmentPlacement placement{};
            placement.resource_selector = read_i16(bytes, pos);
            placement.azimuth_degrees = read_i16(bytes, pos);
            placement.draw_width_pixels = read_i16(bytes, pos);
            placement.screen_y = read_i16(bytes, pos);
            if (placement.resource_selector == 0 && !placement.terminator()) {
                throw std::runtime_error("INIT environment terminator has payload");
            }
            if (placement.terminator() && j + 1U != profile.stored_record_count) {
                throw std::runtime_error("INIT environment premature terminator");
            }
            profile.placements.push_back(placement);
        }
        if (!profile.placements.back().terminator()) {
            throw std::runtime_error("INIT environment profile lacks terminator");
        }
        profiles.push_back(std::move(profile));
    }
    return profiles;
}

} // namespace

std::vector<EnvironmentProfile> decode_init_environment_profiles_at(
    const std::vector<std::uint8_t>& bytes, std::size_t offset) {
    if (offset > bytes.size()) {
        throw std::runtime_error("INIT environment offset beyond EOF");
    }
    auto pos = offset;
    auto profiles = read_profiles(bytes, pos);
    if (pos != bytes.size()) {
        throw std::runtime_error("INIT has trailing environment bytes");
    }
    return profiles;
}

InitFile parse_init(const std::vector<std::uint8_t>& bytes) {
    InitFile result{};
    std::size_t pos = 0;
    for (auto& word : result.header_words) {
        word = read_u16(bytes, pos);
    }
    result.class1_descriptors = read_descriptors(bytes, pos);
    result.class3_descriptors = read_descriptors(bytes, pos);
    result.environment_profiles = read_profiles(bytes, pos);
    if (pos != bytes.size()) {
        throw std::runtime_error("INIT contains trailing bytes");
    }
    return result;
}

std::vector<std::uint8_t> encode_init(const InitFile& file) {
    std::vector<std::uint8_t> out;
    for (const auto word : file.header_words) {
        write_u16(out, word);
    }
    write_descriptors(out, file.class1_descriptors);
    write_descriptors(out, file.class3_descriptors);
    write_count(out, file.environment_profiles.size());
    for (const auto& profile : file.environment_profiles) {
        if (profile.stored_record_count != profile.placements.size() ||
            profile.placements.empty()) {
            throw std::invalid_argument("INIT environment stored count mismatch");
        }
        write_u16(out, profile.res8_base);
        write_u16(out, profile.stored_record_count);
        for (std::size_t i = 0; i < profile.placements.size(); ++i) {
            const auto& placement = profile.placements[i];
            if ((placement.resource_selector == 0 && !placement.terminator()) ||
                (placement.terminator() && i + 1U != profile.placements.size()) ||
                (i + 1U == profile.placements.size() && !placement.terminator())) {
                throw std::invalid_argument("INIT environment terminator invalid");
            }
            write_i16(out, placement.resource_selector);
            write_i16(out, placement.azimuth_degrees);
            write_i16(out, placement.draw_width_pixels);
            write_i16(out, placement.screen_y);
        }
    }
    return out;
}

} // namespace rloft
