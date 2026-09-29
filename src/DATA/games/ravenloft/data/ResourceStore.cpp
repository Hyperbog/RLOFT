#include "games/ravenloft/data/ResourceStore.h"
#include <stdexcept>
#include <string>

namespace ravenloft::data {
namespace {
std::filesystem::path findArchive(const std::filesystem::path& root, std::size_t index) {
    if (root.empty()) return {};
    const auto stem="RES"+std::to_string(index);
    const auto plain=root/stem;
    if (std::filesystem::is_regular_file(plain)) return plain;
    const auto dotted=root/(stem+".");
    if (std::filesystem::is_regular_file(dotted)) return dotted;
    return {};
}
}

ResourceStore::ResourceStore(const std::filesystem::path& gameRoot) {
    loadRoots(gameRoot,nullptr);
}

ResourceStore::ResourceStore(const std::filesystem::path& gameRoot,
                             const std::filesystem::path& cdRoot) {
    loadRoots(gameRoot,&cdRoot);
}

void ResourceStore::loadRoots(const std::filesystem::path& gameRoot,
                              const std::filesystem::path* cdRoot) {
    for (std::size_t i=0;i<archives_.size();++i) {
        auto path=findArchive(gameRoot,i);
        if (path.empty() && cdRoot) path=findArchive(*cdRoot,i);
        if (!path.empty()) archives_[i]=std::make_unique<ResourceArchive>(path);
    }
}

bool ResourceStore::hasArchive(std::uint8_t archive) const {
    return archive<archives_.size() && static_cast<bool>(archives_[archive]);
}

std::span<const std::uint8_t> ResourceStore::record(std::uint8_t archive, std::uint16_t index) const {
    if (archive>=archives_.size()) throw std::out_of_range("Live resource archive selector");
    if (!archives_[archive]) throw std::runtime_error("Required live resource archive RES"+std::to_string(archive)+" is not available");
    return archives_[archive]->record(index);
}

std::size_t ResourceStore::count(std::uint8_t archive) const {
    if (archive>=archives_.size()) throw std::out_of_range("Live resource archive selector");
    return archives_[archive] ? archives_[archive]->count() : 0;
}

IndexedImage ResourceStore::texture(std::uint16_t index) const {
    return decodeTextureType6(record(4,index));
}

IndexedImage ResourceStore::sprite(std::uint16_t index) const {
    return decodeSpriteType7(record(3,index));
}

IndexedImage ResourceStore::skyBitmap(std::uint16_t index) const {
    return decodeBitmapType1(record(8,index));
}

IndexedImage ResourceStore::uiBitmap(std::uint16_t index) const {
    return decodeBitmapType1(record(0,index));
}

}
