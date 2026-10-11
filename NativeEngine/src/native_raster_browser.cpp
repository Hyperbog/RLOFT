#include "rloft_native/native_raster_browser.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft_native {
namespace {
std::string checked_archive(std::string_view name) {
    const auto key = native_asset_key(name);
    constexpr std::array<std::string_view, 10> allowed{{
        "RES0", "RES1", "RES2", "RES3", "RES4",
        "RES5", "RES6", "RES7", "RES8", "RES10"
    }};
    if (std::find(allowed.begin(), allowed.end(), key) == allowed.end())
        throw std::invalid_argument("image browse requires an original RES archive");
    return key;
}

bool is_type1(const std::vector<std::uint8_t>& record) {
    return record.size() >= 3 &&
           record[0] == 'E' && record[1] == 'H' && record[2] == 1;
}
} // namespace

NativeRasterRecord load_native_raster_record(
    const NativeMedia& source, std::string_view archive_name,
    std::size_t index) {
    auto archive_name_safe = checked_archive(archive_name);
    NativeResArchive archive(source.read(archive_name_safe));
    if (index >= archive.entry_count())
        throw std::out_of_range("original RES browse entry outside archive");
    auto entry = archive.entry(index);
    if (!is_type1(entry))
        throw std::runtime_error("selected RES record is not EH type-1 image");
    NativeRasterRecord output;
    output.archive = std::move(archive_name_safe);
    output.record_index = index;
    output.raster = decode_native_type1_raster(entry);
    return output;
}

NativeRasterRecord seek_native_raster_record(
    const NativeMedia& source, std::string_view archive_name,
    std::size_t from_index, bool forward) {
    auto archive_name_safe = checked_archive(archive_name);
    NativeResArchive archive(source.read(archive_name_safe));
    const auto count = archive.entry_count();
    if (count == 0)
        throw std::runtime_error("RES archive has no images");
    // Treat the supplied position as exclusive, so users can reliably
    // advance/retreat through the *real* archive's original record IDs.
    if (forward) {
        if (from_index >= count || from_index == count - 1)
            throw std::out_of_range("no later original EH type-1 raster");
        for (std::size_t index = from_index + 1; index < count; ++index) {
            const auto entry = archive.entry(index);
            if (!is_type1(entry)) continue;
            NativeRasterRecord output;
            output.archive = std::move(archive_name_safe);
            output.record_index = index;
            output.raster = decode_native_type1_raster(entry);
            return output;
        }
    } else {
        if (from_index == 0 || from_index > count)
            throw std::out_of_range("no preceding original EH type-1 raster");
        for (auto index = from_index; index-- > 0;) {
            const auto entry = archive.entry(index);
            if (!is_type1(entry)) continue;
            NativeRasterRecord output;
            output.archive = std::move(archive_name_safe);
            output.record_index = index;
            output.raster = decode_native_type1_raster(entry);
            return output;
        }
    }
    throw std::out_of_range("no additional original EH type-1 record");
}
} // namespace rloft_native
