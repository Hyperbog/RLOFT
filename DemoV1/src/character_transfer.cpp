#include "rloft/character_transfer.hpp"
#include "rloft/resource_assets.hpp"

namespace rloft {
// DemoV1-only original-install adapter. The source-driven serializer and
// original CODE.1/OBJECTS validation are compiled once by Shared.
CharacterTransferTables CharacterTransferTables::from_assets(
    const RavenloftBootAssets& assets) {
    return from_original_bytes(
        assets.source().read("CODE.1"),
        assets.source().read("OBJECTS.DAT"));
}
} // namespace rloft
