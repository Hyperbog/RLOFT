#include "rloft/game_source.hpp"
#include "rloft/original_ground_res4_audit.hpp"

#include <exception>
#include <iomanip>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>

int main(int argc,char** argv) {
    try {
        if(argc>2) {
            std::cerr<<"Usage: rloft_res4_ground_probe [RLOFT install or game.gog]\n";
            return 2;
        }
        auto source=argc==2
            ? rloft::GameSource::open(argv[1])
            : []() -> rloft::GameSource {
                auto found=rloft::GameSource::auto_locate();
                if(!found) {
                    throw std::runtime_error(
                        "Original media was not found; pass the install path");
                }
                return std::move(*found);
            }();
        const auto original_init=source.read("INIT");
        const auto original_base=
            rloft::OriginalGroundRes4Audit::base_from_init(
                original_init);
        const rloft::ResArchive res4(source.read("RES4"));
        const auto report=rloft::OriginalGroundRes4Audit::inspect(res4);
        // This is the source-authenticated CODE.2 1F388 lookup,
        // not a guess derived from shape, color or screenshot.
        constexpr int kLevel22RelativeGround=55;
        const auto original_selected=
            rloft::OriginalGroundRes4Audit::absolute_index(
                kLevel22RelativeGround,original_base,
                res4.entry_count());
        const auto original_tile=
            rloft::OriginalGroundRes4Audit::verified_tile_source(
                res4,original_selected);
        std::cout<<"RLOFT original RES4 ground candidates"
                 <<" [native diagnostic, not DOS live selector]\n"
                 <<"source_archive=RES4\n"
                 <<"original_Level22_header_plus7_relative_selector=55\n"
                 <<"source_CODE2_base=INIT+0x0E -> DGROUP_258F8\n"
                 <<"verified_INIT_base="<<original_base<<"\n"
                 <<"verified_Level22_absolute_ground_entry="
                 <<original_selected<<"\n"
                 <<"verified_decoded_ground_pixel_bytes="
                 <<original_tile.size()-2U<<"\n"
                 <<"entry_count="<<report.total_entries<<"\n"
                 <<"type1_count="<<report.type1_entries<<"\n"
                 <<"invalid_type1_count="<<report.rejected_type1_entries<<"\n"
                 <<"eligible_64x64_count="<<report.tiles_64x64.size()<<"\n";
        for(const auto& c:report.tiles_64x64) {
            // The arithmetic difference is only a hypothesis.
            // It is not a measured DOS 258F8 runtime value.
            const auto implied_base=c.absolute_entry-55;
            std::cout<<"candidate\t"<<c.absolute_entry
                     <<"\timplied_base_unverified="<<implied_base
                     <<"\t"<<c.compressed_entry_bytes<<"\t"
                     <<unsigned(c.compression)<<"\t"
                     <<unsigned(c.transform)<<"\t"
                     <<unsigned(c.transparent_index)<<"\t"
                     <<"fnv1a32=0x"<<std::hex<<std::setfill('0')
                     <<std::setw(8)<<c.decoded_fnv1a32<<std::dec<<"\n";
        }
        std::cout<<"interpretation=source-selected Level22 ground identified;"
                 <<" no scene pixels changed\n";
        return 0;
    } catch(const std::exception& error) {
        std::cerr<<"Original RES4 probe error: "<<error.what()<<"\n";
        return 1;
    }
}
