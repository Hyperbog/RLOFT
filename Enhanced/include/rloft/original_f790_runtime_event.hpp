#pragma once

#include "rloft/original_f790_modal_sequence.hpp"
#include "rloft/original_f790_caller_catalog.hpp"
#include "rloft/original_f790_runtime_description.hpp"

#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace rloft {

// The original F790 call is NOT universally given the global cursor.
// 11 of 17 direct callers push 8600/8602, three pass (-1,0),
// two pass stack locals, and one passes a selected ID with known=0.
// Preserve actual source arguments rather than substituting current held.
struct OriginalF790SourceArguments final {
    std::int32_t item_id{-1};
    bool known{};
    bool pending_6dac{};
    // Exact prebuilt 759C contents when pending_6dac is set.
    // A feedback event MUST NOT regenerate an ordinary item string.
    std::string_view pending_text_759c{};
};

// Source-only bridge from a real F790 caller's argument ownership,
// through MUTABLE nested weights and original text, to the existing
// event-owned saved-background/RNG/ribbon sequence.
struct OriginalF790RuntimeEvent final {
    // Only invoke for an actual identified original F790 callsite, not
    // a native repaint or a guessed click mapping. Explicit empty,
    // locally saved and selected item arguments are not the cursor.
    [[nodiscard]] static OriginalF790SourceArguments arguments_for_direct_call(
        std::uint32_t source_e8_offset,
        const InventoryRuntimeState& current_inventory,
        std::int32_t local_or_selected_id,
        bool local_known,
        bool pending_6dac,
        std::string_view pending_text_759c) {
        const auto origin=OriginalF790CallerCatalog::arguments_at(
            source_e8_offset);
        if(!origin)throw std::out_of_range("unknown CODE.2 F790 E8 site");
        OriginalF790SourceArguments args{};
        args.pending_6dac=pending_6dac;
        args.pending_text_759c=pending_text_759c;
        switch(*origin) {
        case OriginalF790ArgumentOrigin::GlobalHeld8600_8602:
            args.item_id=current_inventory.held.object_id;
            args.known=current_inventory.held.known!=0U;
            break;
        case OriginalF790ArgumentOrigin::ExplicitEmpty:
            args.item_id=-1;
            args.known=false;
            break;
        case OriginalF790ArgumentOrigin::StackLocal:
            args.item_id=local_or_selected_id;
            args.known=local_known;
            break;
        case OriginalF790ArgumentOrigin::SelectedKnownZero:
            args.item_id=local_or_selected_id;
            args.known=false;
            break;
        }
        return args;
    }

    [[nodiscard]] static std::string resolve_text_for_source_call(
        const OriginalF790SourceArguments& args,
        const std::vector<std::uint8_t>& raw_objects,
        const ObjectCatalog& objects,
        const InventoryRuntimeState& mutable_inventory,
        const OriginalF790StaticResources& static_code2,
        const OriginalTTextArchive& original_t) {
        // Original F7B4 tests 6DAC before the ordinary held ID, and
        // F7BD sends 759C straight to F66C. Neither object lookup
        // nor normal text formatting is valid on this branch.
        if(args.pending_6dac)return std::string(args.pending_text_759c);
        return OriginalF790RuntimeDescription::compose(
            raw_objects,objects,mutable_inventory,args.item_id,args.known,
            static_code2,original_t);
    }

    template<class ObserveGameTick30700>
    static void dispatch_source_call(
        OriginalF790ModalSequence& sequence,
        IndexedFramebuffer& framebuffer,
        const OriginalF790SourceArguments& args,
        const std::vector<std::uint8_t>& raw_objects,
        const ObjectCatalog& objects,
        const InventoryRuntimeState& mutable_inventory,
        const OriginalF790StaticResources& static_code2,
        const OriginalTTextArchive& original_t,
        const Type1Raster& original_res2_3,
        const Type2Font& original_font1,
        OriginalRandom& one_shared_game_rng,
        ObserveGameTick30700&& observe_tick_30700) {
        // Generate/validate the source string before touching the
        // original modal's saved backing or shared RNG.
        const auto source_text=resolve_text_for_source_call(
            args,raw_objects,objects,mutable_inventory,static_code2,original_t);
        sequence.source_event_f790(
            framebuffer,args.pending_6dac,args.item_id,source_text,
            original_res2_3,original_font1,one_shared_game_rng,
            std::forward<ObserveGameTick30700>(observe_tick_30700));
    }
};
} // namespace rloft
