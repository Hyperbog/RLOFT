#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/original_automap_file_export.hpp"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

int main() {
    using rloft::OriginalAutomapFileExport;
    assert(OriginalAutomapFileExport::source_filename(22) ==
           "AUTOMAPS/AUTO022.MAP");
    // Original CODE.2 02A34 writes H, ONES, TENS in that order.
    assert(OriginalAutomapFileExport::source_filename(123) ==
           "AUTOMAPS/AUTO132.MAP");
    bool rejected=false;
    try{(void)OriginalAutomapFileExport::source_filename(-1);}
    catch(const std::out_of_range&){rejected=true;}
    assert(rejected);

    rloft::LevelState level{};
    level.width=2;
    level.height=3;
    level.cell_words.assign(6U,0U);
    level.automap_cache.assign(6U,0x2EU);
    level.header[0x28EU]=0U; // no family-B objects
    level.header[0x28FU]=0U;
    level.storage=rloft::LevelStorage::ArchiveCompact;
    const auto save=OriginalAutomapFileExport::merged_02a34(level);
    const std::vector<std::uint8_t> expected={
        0xFAU,0xFAU,0xFAU,13U,10U,
        0xFAU,0xFAU,0xFAU,13U,10U,
    };
    assert(save==expected);
    const auto print=OriginalAutomapFileExport::printer_03321(level);
    std::vector<std::uint8_t> printable;
    for(int i=0;i<2;++i) {
        printable.insert(printable.end(),3U,0xFAU);
        printable.push_back(13U);
        printable.insert(printable.end(),3U,0x20U);
        printable.push_back(13U);
        printable.insert(printable.end(),3U,0x20U);
        printable.push_back(13U);
        printable.push_back(10U);
    }
    assert(print==printable);

    // Original 026E8 uses a separate (third) label overlay, and
    // saves it merged over base. A real annotation payload adds
    // original 2474's label:value legend after both columns.
    level.automap_annotations={
        0U,0U,5U,0U,1U,0U,'X'
    };
    const auto annotated=OriginalAutomapFileExport::merged_02a34(level);
    assert(annotated.size()>save.size());
    const std::vector<std::uint8_t> legend={'A',':','X',13U,10U};
    assert(std::equal(legend.begin(),legend.end(),
                      annotated.end()-static_cast<std::ptrdiff_t>(legend.size())));
}
