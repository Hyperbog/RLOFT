#include "rloft/combat_original_overlay_predicates.hpp"

#include <cstdint>
#include <optional>
#include <utility>

namespace rloft {

std::optional<bool> CombatOriginalOverlayPredicatesModel::bit_1c1dc(
    std::uint32_t bit_number,
    const CombatOriginalObjectTypeBitServices& memory) {
    // All original 2AD18 CALLs pass positive constant bit indices.
    // Original 1C1DC calculates floor(index/32) and index%32,
    // reading exactly DWORD[22200+4*floor(index/32)].
    if(!memory.original_dgroup_dword)return std::nullopt;
    const auto addr=static_cast<std::uint32_t>(
        0x22200U+((bit_number>>5U)<<2U));
    const auto value=memory.original_dgroup_dword(addr);
    if(!value)return std::nullopt;
    return (*value&(1U<<(bit_number&31U)))!=0U;
}

std::optional<bool> CombatOriginalOverlayPredicatesModel::type_2ad18(
    std::uint32_t type,
    const CombatOriginalObjectTypeBitServices& memory) {
    if(type==0x3BU || type==0x3CU) {
        // Original first tests world-state bit 0x48 and returns
        // FALSE if set; only then tests the type-specific flag.
        const auto lock=bit_1c1dc(0x48U,memory);
        if(!lock)return std::nullopt;
        if(*lock)return false;
        const auto bit=bit_1c1dc(
            type==0x3BU ? 0x35U : 0x36U,memory);
        if(!bit)return std::nullopt;
        return *bit;
    }
    if(type==0x3AU)return false;
    // Original signed CMP/JL/JLE with DWORD type argument.
    const auto signed_type=type<=0x7FFFFFFFU
        ? static_cast<std::int64_t>(type)
        : static_cast<std::int64_t>(type)-0x100000000LL;
    return (signed_type>=0x22LL&&signed_type<=0x46LL)
        || type==0x52U;
}

bool CombatOriginalOverlayPredicatesModel::type_2ade0(
    std::uint32_t type) noexcept {
    const auto signed_type=type<=0x7FFFFFFFU
        ? static_cast<std::int64_t>(type)
        : static_cast<std::int64_t>(type)-0x100000000LL;
    return (signed_type>=0x48LL&&signed_type<=0x4ALL)
        || type==0x4DU || type==0x4FU;
}

CombatOriginalOverlayProbeServices
CombatOriginalOverlayPredicatesModel::bind_2ae30(
    CombatOriginalObjectTypeBitServices memory,
    CombatOriginalOverlayProbeServices services) {
    services.predicate_2ad18=[
        memory
    ](std::uint32_t type) {
        return CombatOriginalOverlayPredicatesModel::type_2ad18(
            type,memory);
    };
    services.predicate_2ade0=[](std::uint32_t type)
        ->std::optional<bool> {
        return CombatOriginalOverlayPredicatesModel::type_2ade0(type);
    };
    return services;
}

} // namespace rloft
