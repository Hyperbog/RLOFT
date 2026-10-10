#pragma once

#include "rloft/original_f790_background_page.hpp"
#include "rloft/original_feedback_ribbon_epoch.hpp"
#include "rloft/original_feedback_ribbon_raster.hpp"

#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

namespace rloft {

// Source-only composition of CODE.2 112EC -> F790 -> F66C.
// The live Windows host has NOT been connected: it must dispatch only
// proved F790 events, using its ONE ExplorationRuntime shared RNG.
class OriginalF790ModalSequence final {
public:
    void enter_modal_13118() noexcept {
        background_.enter_modal_13118();
        ribbon_.enter_modal_13118();
        message_.clear();
    }

    // 112EC captures exactly once AFTER the four party actor columns.
    void capture_after_actor_columns_112ec(const IndexedFramebuffer& framebuffer) {
        // Original 13133 invokes 112EC once at modal entry; recapturing
        // during a host VGA refresh would overwrite the clean saved page.
        if (background_.ready())
            throw std::logic_error("duplicate original 112EC backing capture");
        background_.capture_112ec(framebuffer);
    }

    // One explicit source F790 event: restore 2932C before examining
    // pending 6DAC or held ID; then prepare F66C and rasterize its output.
    // The native 70-Hz presentation loop must NEVER call this function.
    template<class ObserveGameTick30700>
    void source_event_f790(
        IndexedFramebuffer& framebuffer,
        bool pending_6dac,
        std::int32_t held_id,
        std::string_view original_message,
        const Type1Raster& ribbon_res2_3,
        const Type2Font& font1,
        OriginalRandom& shared_rng,
        ObserveGameTick30700&& observe_tick_30700) {
        if (!background_.ready())
            throw std::logic_error("F790 event before original 112EC capture");
        // F790 first restores 7F68 even for an empty held cursor.
        background_.restore_2932c(framebuffer);
        ribbon_.source_f790_call(
            pending_6dac,held_id,font1.widths,original_message,
            shared_rng,std::forward<ObserveGameTick30700>(observe_tick_30700));
        if (const auto* prepared=ribbon_.present_vga_frame()) {
            // Retain original message for subsequent RNG-free VGA redraws.
            message_=std::string(original_message);
            OriginalFeedbackRibbonRaster::draw_prepared(
                framebuffer,ribbon_res2_3,font1,message_,*prepared);
        } else {
            message_.clear();
        }
    }

    // Host repaints a newly reconstructed party panel at VGA cadence.
    // Only the already-prepared F66C frames/text are replayed; DO NOT
    // rerun 2932C (which belongs to F790) or consume 30CA2/30700.
    void replay_after_host_redraw(
        IndexedFramebuffer& framebuffer,
        const Type1Raster& ribbon_res2_3,
        const Type2Font& font1) const {
        if (const auto* prepared=ribbon_.present_vga_frame())
            OriginalFeedbackRibbonRaster::draw_prepared(
                framebuffer,ribbon_res2_3,font1,message_,*prepared);
    }

    void leave_modal() noexcept {
        background_.leave_modal();
        ribbon_.enter_modal_13118();
        message_.clear();
    }

    [[nodiscard]] bool captured() const noexcept {return background_.ready();}
    [[nodiscard]] std::size_t f790_event_count() const noexcept {
        return ribbon_.original_f790_calls();
    }

private:
    OriginalF790BackgroundPage background_{};
    OriginalFeedbackRibbonEpoch ribbon_{};
    std::string message_{};
};

} // namespace rloft
