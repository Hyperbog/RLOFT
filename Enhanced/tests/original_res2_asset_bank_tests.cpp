#ifdef NDEBUG
#undef NDEBUG
#endif

#include "rloft/game_source.hpp"
#include "rloft/resource_assets.hpp"

#include <cassert>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <initializer_list>

namespace {

void put32(std::vector<std::uint8_t>& bytes,
           std::size_t at,std::uint32_t value) {
    assert(at+4U<=bytes.size());
    for(unsigned byte=0;byte<4U;++byte)
        bytes[at+byte]=static_cast<std::uint8_t>(value>>(8U*byte));
}
void write_file(const std::filesystem::path& path,
                const std::vector<std::uint8_t>& bytes) {
    std::ofstream file(path,std::ios::binary|std::ios::trunc);
    assert(file);
    file.write(reinterpret_cast<const char*>(bytes.data()),
               static_cast<std::streamsize>(bytes.size()));
    assert(file);
}
std::vector<std::uint8_t> original_res2_modal_fixture() {
    // Immutable Resources/RES/INVENTORY.md: all 38 original RES2
    // entries are EH type-1. This synthetic archive exercises the
    // exact source-used indices, not any res0_base_ offset.
    constexpr std::size_t count=0x26U;
    constexpr std::size_t table_bytes=4U+4U*count;
    std::vector<std::uint8_t> bytes(table_bytes,0U);
    put32(bytes,0U,static_cast<std::uint32_t>(count));
    const auto is_source_modal_entry=[](std::size_t index) {
        for(const std::size_t used : {
                0U,1U,2U,4U,5U,6U,7U,8U,
                0x12U,0x13U,0x24U,0x25U}) {
            if(index==used)return true;
        }
        return false;
    };
    for(std::size_t i=0U;i<count;++i) {
        put32(bytes,4U+4U*i,static_cast<std::uint32_t>(bytes.size()));
        if(!is_source_modal_entry(i))continue;
        // Original-style EH type1 descriptor: one frame, 8x1,
        // no compression or transform. Unique pixel marker per ID
        // proves the original bank-2 entry was used verbatim.
        const std::vector<std::uint8_t> rec{
            'E','H',1U,8U,0U,1U,0U,1U,0U,1U,0xFFU,0U,0U,
            static_cast<std::uint8_t>(i+1U),20U,30U,40U,
            50U,60U,70U,80U
        };
        bytes.insert(bytes.end(),rec.begin(),rec.end());
    }
    return bytes;
}

} // namespace

int main() {
    namespace fs=std::filesystem;
    const auto stamp=std::to_string(
        std::chrono::steady_clock::now().time_since_epoch().count());
    const auto root=fs::temp_directory_path()/
        ("rloft_res2_asset_route_"+stamp);
    fs::remove_all(root);
    fs::create_directories(root);

    // Build only a structural test fixture; no original game bytes
    // are republished or modified. All normal mandatory archives
    // except RES2 are intentionally present on first construction.
    const std::vector<std::uint8_t> empty_res{0U,0U,0U,0U};
    for(const auto name:{"RES0","RES1","RES5","RES6",
                         "RES7","RES8","RES10","T"})
        write_file(root/name,empty_res);
    write_file(root/"INIT",std::vector<std::uint8_t>(16U,0U));
    std::vector<std::uint8_t> colors(1U+15U*768U,0U);
    colors[0]=15U;
    write_file(root/"COLORS",colors);
    write_file(root/"CODE.1",std::vector<std::uint8_t>(0x43D1BU,0U));

    {
        auto source=rloft::GameSource::open(root);
        auto assets=rloft::RavenloftBootAssets(std::move(source));

        // Original RES2 is NOT demanded on initial launch.
        assert(!fs::exists(root/"RES2"));

        // Encumbrance: original CODE.2 12988 requests RES2:0x25.
        write_file(root/"RES2",original_res2_modal_fixture());
        const auto panel=assets.type1(2,0x25);
        assert(panel.count==1);
        assert(panel.width==8);
        assert(panel.height==1);
        assert(panel.transparent_index==0xFFU);
        assert(panel.pixels==
               std::vector<std::uint8_t>(
                   {0x26U,20U,30U,40U,50U,60U,70U,80U}));

        // CODE.2 0FF3C,11268,1118C,10BBC and 10158 use these
        // actual RES2 indices. None is remapped through INIT[0].
        for(const int id : {
                0,1,2,4,5,6,7,8,0x12,0x13,0x24,0x25}) {
            const auto graphic=assets.type1(2,id);
            assert(graphic.count==1);
            assert(graphic.width==8);
            assert(graphic.height==1);
            assert(graphic.pixels.size()==8U);
            assert(graphic.pixels.front()==
                   static_cast<std::uint8_t>(id+1));
        }

        // Archive is now cached; a later temporary source-media
        // disappearance cannot silently switch to another bank.
        fs::remove(root/"RES2");
        const auto second=assets.type1(2,0x25);
        assert(second.pixels==panel.pixels);

        // Wrong original RES2 ID throws rather than loading RES0/1.
        bool missing=false;
        try { (void)assets.type1(2,0x23); }
        catch(const std::runtime_error&) { missing=true; }
        assert(missing);

        // An unsupported archive is still explicitly rejected.
        bool unsupported=false;
        try { (void)assets.type1(9,0x25); }
        catch(const std::out_of_range&) { unsupported=true; }
        assert(unsupported);
    }
    fs::remove_all(root);
}
