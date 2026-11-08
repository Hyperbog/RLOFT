#include "rloft/INFO.hpp"

#include <stdexcept>

namespace rloft {
namespace {
constexpr std::size_t kHeaderBytes = InfoDat::kSelectorCount + 2;
constexpr std::size_t kMaxPathBytes = 0x7FFF; // signed int16, per CODE.1/CODE.2

void validate_selectors(const InfoDat& info) {
    for (const auto selector : info.resource_locations) {
        if (selector > 9) {
            throw std::invalid_argument("INFO.DAT selector must be 0..9");
        }
    }
}
} // namespace

InfoDat parse_info_dat(const std::vector<std::uint8_t>& bytes) {
    if (bytes.size() < kHeaderBytes) {
        throw std::invalid_argument("INFO.DAT is shorter than 15 bytes");
    }

    InfoDat result;
    for (std::size_t i = 0; i < InfoDat::kSelectorCount; ++i) {
        const auto ascii = bytes[i];
        if (ascii < '0' || ascii > '9') {
            throw std::invalid_argument("INFO.DAT selector is not ASCII decimal");
        }
        result.resource_locations[i] = static_cast<std::uint8_t>(ascii - '0');
    }

    const auto path_size = static_cast<std::uint16_t>(
        static_cast<std::uint16_t>(bytes[13]) |
        (static_cast<std::uint16_t>(bytes[14]) << 8U));
    if (path_size > kMaxPathBytes) {
        throw std::invalid_argument("INFO.DAT has a negative signed path length");
    }
    if (bytes.size() - kHeaderBytes != path_size) {
        throw std::invalid_argument("INFO.DAT path length does not match file size");
    }

    result.resource_path.assign(
        bytes.begin() + static_cast<std::ptrdiff_t>(kHeaderBytes), bytes.end());
    return result;
}

std::vector<std::uint8_t> encode_info_dat(const InfoDat& info) {
    validate_selectors(info);
    if (info.resource_path.size() > kMaxPathBytes) {
        throw std::invalid_argument("INFO.DAT path exceeds signed int16 length");
    }

    std::vector<std::uint8_t> result;
    result.reserve(kHeaderBytes + info.resource_path.size());
    for (const auto selector : info.resource_locations) {
        result.push_back(static_cast<std::uint8_t>('0' + selector));
    }
    const auto path_size = static_cast<std::uint16_t>(info.resource_path.size());
    result.push_back(static_cast<std::uint8_t>(path_size & 0xFFU));
    result.push_back(static_cast<std::uint8_t>((path_size >> 8U) & 0xFFU));
    result.insert(result.end(), info.resource_path.begin(), info.resource_path.end());
    return result;
}

std::string info_legacy_res_path(const InfoDat& info,
                                 std::size_t archive_index) {
    if (archive_index >= InfoDat::kActiveResCount) {
        throw std::out_of_range("original runtime supports RES0..RES10 only");
    }
    const auto name = std::string("RES") + std::to_string(archive_index);
    if (info.resource_locations[archive_index] == 0) {
        return name;
    }
    return info.resource_path + "\\" + name;
}

std::string info_legacy_numbered_path(const InfoDat& info,
                                      unsigned resource_number) {
    if (resource_number > 99) {
        throw std::out_of_range("numbered resource ID must fit two digits");
    }
    const auto number = std::to_string(resource_number);
    return info.resource_path + "\\" +
           (resource_number < 10 ? "0" : "") + number;
}

} // namespace rloft
