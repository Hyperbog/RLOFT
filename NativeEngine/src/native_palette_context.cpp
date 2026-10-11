#include "rloft_native/native_palette_context.hpp"
#include "rloft_native/native_media.hpp"

#include <algorithm>
#include <array>
#include <stdexcept>
#include <string>
#include <utility>

namespace rloft_native {
namespace {
// Archived CODE.2 DGROUP 81AC: world palette selector byte in each
// three-byte level display entry. This table is not computed from RES name.
constexpr std::array<unsigned,32> kLevelBank{{
    4,4,4,2,2,3,3,2,4,2,3,3,2,2,2,4,
    4,4,3,4,3,3,4,3,3,4,4,3,2,0,3,4
}};

NativePaletteDecision exact(unsigned bank,std::string why) {
    return {bank,NativePaletteEvidence::ExactCallSite,{},std::move(why)};
}

NativePaletteDecision world(std::optional<std::size_t> level,
                            std::string why) {
    if (level) {
        if (*level>=kLevelBank.size())
            throw std::out_of_range("world level outside original 32-level table");
        return {kLevelBank[*level],NativePaletteEvidence::LevelContext,{},
                std::move(why)+" / CODE.2 level display state"};
    }
    return {4,NativePaletteEvidence::ContextualPreview,{0,2,3},
            std::move(why)+" / bank 4 is a PREVIEW only; level unknown"};
}
} // namespace

unsigned native_world_palette_bank(std::size_t level) {
    if (level>=kLevelBank.size())
        throw std::out_of_range("world palette level outside original 32 levels");
    return kLevelBank[level];
}

NativePaletteDecision resolve_native_palette(
    std::string_view archive, std::size_t record,
    std::optional<std::size_t> world_level) {
    const auto name=native_asset_key(archive);
    if (world_level && *world_level>=kLevelBank.size())
        throw std::out_of_range("world level outside original 32-level table");

    if (name=="RES0") {
        // Fixed per-resource CODE.2 callsite selections: the same archive
        // holds multiple independent palettes, unlike an archive-wide rule.
        if (record>=16 && record<=30) return exact(
            unsigned((record-16)/3),"CODE.2 world-mode triplet RES0 16..30");
        for (const auto base : {31U,36U,41U,46U,51U}) {
            if (record>=base && record<=base+4)
                return exact(unsigned(record-base),
                    "CODE.2 RES0 five-member world-mode family");
        }
        if (record==63 || record==64 || record==89)
            return exact(10,"CODE.2 character/inventory selector");
        if (record==65 || record==66 || record==90)
            return exact(11,"CODE.2 character/inventory selector");
        if (record==62 || (record>=97 && record<=105))
            return exact(12,"CODE.2 menu/modal palette");
        if (record==77 || record==116 || record==117)
            return exact(13,"CODE.2 special title palette");
        return world(world_level,"RES0 general world/UI asset");
    }

    if (name=="RES8") {
        if (record>=45 && record<=71)
            return exact(4,"INIT environment profile 1 / original level 22");
        if (record>=8 && record<=44) {
            if (world_level) {
                const auto bank=native_world_palette_bank(*world_level);
                if (bank==3 || bank==4)
                    return {bank,NativePaletteEvidence::LevelContext,{},
                        "INIT environment profile 0 / matching level state"};
            }
            return {4,NativePaletteEvidence::ContextualPreview,{3},
                    "INIT environment profile 0; original levels use banks 3 or 4"};
        }
    }

    // World artwork uses the active level display bank (not a per-file
    // palette). Some resources may have further per-callsite remaps.
    if (name=="RES1" || name=="RES2" || name=="RES3" ||
        name=="RES4" || name=="RES7" || name=="RES8" ||
        name=="RES9" || name=="RES11" || name=="RES12")
        return world(world_level,"CODE.2 level-context artwork");

    // RES5, RES6 and RES10 do not have a proved general palette rule
    // in the audited router. Unresolved is better than a false bank 12.
    return {std::nullopt,NativePaletteEvidence::Unresolved,{},
            "No proven bank for this resource and current rendering context"};
}
} // namespace rloft_native
