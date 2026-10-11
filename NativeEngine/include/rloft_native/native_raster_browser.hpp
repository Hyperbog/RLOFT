#pragma once

#include "rloft_native/native_assets.hpp"
#include <cstddef>
#include <string>
#include <string_view>

namespace rloft_native {

struct NativeRasterRecord {
    std::string archive;
    std::size_t record_index{};
    unsigned resource_type{}; // 1=EH raster, 6=EH world texture
    NativeType1Raster raster;
};

// Browser supports original EH1 rasters and EH6 RES4 world textures.
// Independent original on-demand resource image browser. Supports the
// original RES archive names, not arbitrary filesystem paths or mod scripts.
[[nodiscard]] NativeRasterRecord load_native_raster_record(
    const NativeMedia& source, std::string_view archive_name,
    std::size_t index);

// Step through an original RES archive to find the next or preceding EH1
// raster. Non-image types are intentionally skipped, malformed EH1 data is
// rejected rather than disguised as another entry.
[[nodiscard]] NativeRasterRecord seek_native_raster_record(
    const NativeMedia& source, std::string_view archive_name,
    std::size_t from_index, bool forward);

} // namespace rloft_native
