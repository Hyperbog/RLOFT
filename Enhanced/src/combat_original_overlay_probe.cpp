#include "rloft/combat_original_overlay_probe.hpp"

#include <cstdint>
#include <stdexcept>
#include <utility>

namespace rloft {
namespace {
std::int64_t signed_dword(std::uint32_t bits) noexcept {
    return bits<=0x7FFFFFFFU
        ? static_cast<std::int64_t>(bits)
        : static_cast<std::int64_t>(bits)-0x100000000LL;
}
bool in_unsigned_word_interval(
    std::uint16_t lower,std::uint16_t upper,
    std::uint32_t query) noexcept {
    // Original MOVZX WORD then CMP DWORD with signed JG/JGE.
    const auto q=signed_dword(query);
    return static_cast<std::int64_t>(lower)<=q &&
        q<=static_cast<std::int64_t>(upper);
}
} // namespace

std::optional<std::int32_t> CombatOriginalOverlayProbeModel::probe_2ae30(
    const CombatOriginalOverlayProbeState& state,
    std::uint32_t row,std::uint32_t column,
    const CombatOriginalOverlayProbeServices& calls) {
    // 2AE3C..2AE4C: disabled D680 returns zero WITHOUT any reads.
    const auto disabled=calls.live_disabled_d680
        ? calls.live_disabled_d680()
        : state.skip_probing_d680;
    if(disabled!=0U)return 0;
    if(!calls.original_word)return std::nullopt;
    // Source toggles only AL, preserving upper 24 bits C050.
    const auto alternate=state.active_scene_page_c050^1U;
    const auto records=static_cast<std::uint32_t>(
        0x27148U+alternate*0x2260U);
    const auto count_word=static_cast<std::uint32_t>(
        0xF038U+alternate*2U);
    // 2AE6F reads count on EVERY loop; callbacks may affect its
    // value. Index monotonically advances, bounded by WORD max.
    for(std::uint32_t i=0U;i<=0xFFFFU;++i) {
        const auto count=calls.original_word(count_word);
        if(!count)return std::nullopt;
        if(i>=*count)return 0;
        const auto addr=static_cast<std::uint32_t>(
            records+i*0x16U);
        const auto flags=calls.original_word(addr+8U);
        if(!flags)return std::nullopt;
        std::optional<std::uint16_t> first_type;
        if(*flags==0U) {
            // 2AEA6 fetches the ID only for flag-zero records.
            const auto id=calls.original_word(addr+6U);
            if(!id)return std::nullopt;
            first_type=calls.original_word(
                static_cast<std::uint32_t>(
                    0x14D8EU+static_cast<std::uint32_t>(*id)*0x16U));
            if(!first_type)return std::nullopt;
            if(*first_type<0x22U || *first_type==0x50U) {
                const auto w=calls.original_word(addr+2U);
                if(!w)return std::nullopt;
                // Original SAR ((WORD+2)-7D00h),3 then JL 39h.
                // floor(signed_delta/8)<57 iff delta<456.
                if(static_cast<std::int32_t>(*w)-0x7D00 <
                   0x39*8)continue;
            }
        }
        const auto min_row=calls.original_word(addr+0xEU);
        if(!min_row)return std::nullopt;
        if(static_cast<std::int64_t>(*min_row)>signed_dword(row))
            continue;
        const auto max_row=calls.original_word(addr+0x10U);
        if(!max_row)return std::nullopt;
        if(!in_unsigned_word_interval(*min_row,*max_row,row))
            continue;
        const auto min_col=calls.original_word(addr+0x12U);
        if(!min_col)return std::nullopt;
        if(static_cast<std::int64_t>(*min_col)>signed_dword(column))
            continue;
        const auto max_col=calls.original_word(addr+0x14U);
        if(!max_col)return std::nullopt;
        if(!in_unsigned_word_interval(*min_col,*max_col,column))
            continue;

        if(*flags==0U) {
            // Only now does original code call 2AD18 then 2ADE0;
            // the second call is skipped when the first is true.
            if(!calls.predicate_2ad18)return std::nullopt;
            const auto first=calls.predicate_2ad18(*first_type);
            if(!first)return std::nullopt;
            if(*first)return 2;
            if(!calls.predicate_2ade0)return std::nullopt;
            const auto second=calls.predicate_2ade0(*first_type);
            if(!second)return std::nullopt;
            if(*second)return 2;
            if(*first_type<0x22U || *first_type==0x50U)
                return 1;
        } else if(*flags==1U) {
            // 2B000 fetches ID only AFTER both hit rectangles pass.
            const auto id=calls.original_word(addr+6U);
            if(!id)return std::nullopt;
            const auto second_type=calls.original_word(
                static_cast<std::uint32_t>(
                    0x22246U+static_cast<std::uint32_t>(*id)*0x16U));
            if(!second_type)return std::nullopt;
            if(*second_type==0x52U || *second_type==0x71U ||
               *second_type==0x6EU || *second_type==0x91U)
                return 2;
        }
    }
    return 0;
}

CombatOriginalOverlayServices CombatOriginalOverlayProbeModel::bind_1a288(
    CombatOriginalOverlayProbeState& state,
    CombatOriginalOverlayProbeServices calls,
    CombatOriginalOverlayServices overlays) {
    overlays.probe_2ae30=[&state,calls=std::move(calls)](
        std::int32_t row,std::int32_t column) {
        const auto result=probe_2ae30(
            state,static_cast<std::uint32_t>(row),
            static_cast<std::uint32_t>(column),calls);
        if(!result)
            throw std::logic_error(
                "original 2AE30 scene records or predicates uncaptured");
        return *result;
    };
    return overlays;
}

} // namespace rloft
