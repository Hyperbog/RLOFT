#include "rloft/object_catalog.hpp"
#include "rloft/OBJECTS.hpp"

#include <cstddef>
#include <stdexcept>

namespace rloft {

ObjectCatalog ObjectCatalog::parse(const std::vector<std::uint8_t>& bytes) {
    ObjectCatalog catalog{};
    catalog.objects_ = decode_objects_dat(bytes);
    return catalog;
}

ObjectCatalog ObjectCatalog::load(const GameSource& source) {
    return parse(source.read("OBJECTS.DAT"));
}

const ObjectDefinition& ObjectCatalog::at(std::size_t object_id) const {
    if (object_id >= objects_.size()) throw std::out_of_range("OBJECTS.DAT object id");
    return objects_[object_id];
}

ObjectDefinition& ObjectCatalog::edit(std::size_t object_id) {
    if (object_id >= objects_.size()) throw std::out_of_range("OBJECTS.DAT object id");
    return objects_[object_id];
}

std::vector<std::uint8_t> ObjectCatalog::encode() const {
    return encode_objects_dat(objects_);
}

} // namespace rloft
