#include "rloft_native/native_raster_browser.hpp"
#include "rloft_native/native_type6_texture.hpp"

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
    constexpr std::array<std::string_view, 11> allowed{{
        "RES0", "RES1", "RES2", "RES3", "RES4",
        "RES5", "RES6", "RES7", "RES8", "RES9", "RES10"
    }};
    if (std::find(allowed.begin(), allowed.end(), key) == allowed.end())
        throw std::invalid_argument("image browse requires an original RES archive");
    return key;
}

unsigned browsable_type(const std::vector<std::uint8_t>& record,
                        std::string_view archive) {
    if(record.size()<3 || record[0]!='E' || record[1]!='H')return 0;
    if(record[2]==1)return 1;
    if(archive=="RES4" && record[2]==6)return 6;
    return 0;
}

NativeType1Raster decode_browsable(
    const std::vector<std::uint8_t>& entry,unsigned type) {
    if(type==1)return decode_native_type1_raster(entry);
    if(type==6)return decode_native_type6_texture(entry);
    throw std::runtime_error("original record is not a supported visual format");
}

std::string unsupported_message(std::string_view archive) {
    if(archive=="RES3")
        return "RES3 contains EH7 compiled scene geometry; SINTAB-based renderer needed";
    if(archive=="RES5")
        return "RES5 contains EH3 XMIDI music sequences, not images";
    if(archive=="RES6")
        return "RES6 contains EH4 VOC sampled sound effects, not images";
    if(archive=="RES10")
        return "RES10 contains EH4 VOC voices/audio, not images";
    return "no additional supported EH1 image or EH6 texture record";
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
    const auto type=browsable_type(entry,archive_name_safe);
    if (!type) throw std::runtime_error(unsupported_message(archive_name_safe));
    NativeRasterRecord output;
    output.archive = std::move(archive_name_safe);
    output.record_index = index;
    output.resource_type = type;
    output.raster = decode_browsable(entry,type);
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
            throw std::out_of_range("no later supported original graphics record");
        for (std::size_t index = from_index + 1; index < count; ++index) {
            const auto entry = archive.entry(index);
            const auto type=browsable_type(entry,archive_name_safe);
            if (!type) continue;
            NativeRasterRecord output;
            output.archive = std::move(archive_name_safe);
            output.record_index = index;
            output.resource_type = type;
            output.raster = decode_browsable(entry,type);
            return output;
        }
    } else {
        if (from_index == 0 || from_index > count)
            throw std::out_of_range("no preceding supported original graphics record");
        for (auto index = from_index; index-- > 0;) {
            const auto entry = archive.entry(index);
            const auto type=browsable_type(entry,archive_name_safe);
            if (!type) continue;
            NativeRasterRecord output;
            output.archive = std::move(archive_name_safe);
            output.record_index = index;
            output.resource_type = type;
            output.raster = decode_browsable(entry,type);
            return output;
        }
    }
    throw std::out_of_range(unsupported_message(archive_name_safe));
}
} // namespace rloft_native
