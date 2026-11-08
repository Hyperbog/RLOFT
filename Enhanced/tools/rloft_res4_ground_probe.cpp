#include "rloft/game_source.hpp"
#include "rloft/original_ground_res4_probe_snapshot.hpp"

#include <exception>
#include <iostream>
#include <stdexcept>
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
        const auto level22=rloft::ExplorationStateCodec::parse_archive_level(
            source.read("LEVELS.DAT"),22);
        const rloft::ResArchive res4(source.read("RES4"));

        // A wrong selected entry is precisely the condition that needs
        // diagnostics. Never resolve() before reporting candidate metadata.
        // On failure print the selected raw EH prefix AND complete archive
        // candidates, then retain the nonzero exit status for automation.
        const auto report=rloft::OriginalGroundRes4ProbeSnapshot::capture(
            original_init,level22,res4);
        report.print(std::cout);
        if(!report.verified) {
            std::cerr<<"Original RES4 probe selection failed: "
                     <<report.error<<"\n";
            return 1;
        }
        return 0;
    } catch(const std::exception& error) {
        std::cerr<<"Original RES4 probe error: "<<error.what()<<"\n";
        return 1;
    }
}
