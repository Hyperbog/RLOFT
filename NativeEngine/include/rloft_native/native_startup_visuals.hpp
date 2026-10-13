#pragma once
#include "rloft/startup_visual_source.hpp"
#include "rloft_native/native_assets.hpp"
#include <cstdint>
#include <vector>
namespace rloft_native {
class NativeMedia;
// Adapter only: original RES0 EH1 and COLORS bank bytes are read through
// the already-established immutable native loader and decoder.
class NativeStartupVisualSource final : public rloft::StartupVisualSource {
public:
    explicit NativeStartupVisualSource(const NativeMedia& source);
    [[nodiscard]] rloft::Type1Raster res0_type1(int index) const override;
    [[nodiscard]] rloft::Type2Font res0_font(int index) const override;
    [[nodiscard]] std::string text(int index) const override;
    [[nodiscard]] rloft::Palette6 colors_palette(
        int index,int brightness=0) const override;
private:
    NativeResArchive res0_;
    std::vector<std::uint8_t> colors_;
    std::vector<std::uint8_t> text_;
};
} // namespace rloft_native
