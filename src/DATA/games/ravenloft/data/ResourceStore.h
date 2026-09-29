#pragma once

#include "games/ravenloft/data/ResourceArchive.h"
#include "games/ravenloft/data/GraphicsDecode.h"
#include <array>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <span>

namespace ravenloft::data {

// Mirrors CODE.2's normal resource-manager namespace: archive selector 0..10
// corresponds directly to RES0..RES10. RES11/RES12 are intentionally excluded;
// the decomp proves they are outside the live manager.
//
// The CD release splits live resources between the installed RLOFT directory
// (normally RES0..RES8) and the CD RLOFT directory (RES9/RES10).  The store
// therefore accepts both roots and resolves either DOS spelling (RESn/RESn.).
class ResourceStore {
public:
    explicit ResourceStore(const std::filesystem::path& gameRoot);
    ResourceStore(const std::filesystem::path& gameRoot,
                  const std::filesystem::path& cdRoot);

    bool hasArchive(std::uint8_t archive) const;
    std::span<const std::uint8_t> record(std::uint8_t archive, std::uint16_t index) const;
    std::size_t count(std::uint8_t archive) const;

    IndexedImage texture(std::uint16_t index) const; // CODE.2 selector 4 -> RES4, EH type 6
    IndexedImage sprite(std::uint16_t index) const;  // CODE.2 selector 3 -> RES3, EH type 7
    IndexedImage skyBitmap(std::uint16_t index) const; // CODE.2 sky path -> RES8, EH type 1
    IndexedImage uiBitmap(std::uint16_t index) const;  // CODE.2 adventure/interface path -> RES0, EH type 1

private:
    void loadRoots(const std::filesystem::path& gameRoot,
                   const std::filesystem::path* cdRoot);
    std::array<std::unique_ptr<ResourceArchive>,11> archives_{};
};

}
