#pragma once

#include <vector>

#include "rloft/automap_annotations.hpp"
#include "rloft/automap_viewport_presenter.hpp"
#include "rloft/resource_assets.hpp"
#include "rloft/startup_presenter.hpp"

namespace rloft {

class AutomapAnnotationPresenter final {
public:
    // All automap layers share the SAME physical map rectangle.
    // The older text-only bounds (y=393, x=239) bled into chrome.
    static constexpr int kClipRowMin = AutomapViewportPresenter::kMapMinY;
    static constexpr int kClipRowMax = AutomapViewportPresenter::kMapMaxY;
    static constexpr int kClipColumnMin = AutomapViewportPresenter::kMapMinX;
    static constexpr int kClipColumnMax = AutomapViewportPresenter::kMapMaxX;

    explicit AutomapAnnotationPresenter(
        const RavenloftBootAssets& assets)
        : font_(assets.res0_font(1)) {}

    void draw(
        IndexedFramebuffer& framebuffer,
        const std::vector<AutomapAnnotation>& annotations,
        int viewport_column,
        int viewport_row) const;

    [[nodiscard]] static int bounded_extent(
        const AutomapAnnotation& annotation,
        const Type2Font& font);

    [[nodiscard]] static bool visible(
        const AutomapAnnotation& annotation,
        const Type2Font& font,
        int viewport_column,
        int viewport_row);

    static void draw_with_font(
        IndexedFramebuffer& framebuffer,
        const std::vector<AutomapAnnotation>& annotations,
        int viewport_column,
        int viewport_row,
        const Type2Font& font);

private:
    static void draw_glyph_clipped(
        IndexedFramebuffer& framebuffer,
        const Type2Font& font,
        std::uint8_t glyph,
        int row,
        int column);

    Type2Font font_;
};

}  // namespace rloft
