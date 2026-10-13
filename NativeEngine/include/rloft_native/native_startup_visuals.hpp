#pragma once
#include "rloft/character_visual_source.hpp"
#include <optional>
#include "rloft_native/native_assets.hpp"
#include <cstdint>
#include <vector>
namespace rloft_native {
class NativeMedia;
// Adapter only: original RES0 EH1 and COLORS bank bytes are read through
// the already-established immutable native loader and decoder.
class NativeStartupVisualSource final : public rloft::CharacterVisualSource {
public:
    explicit NativeStartupVisualSource(const NativeMedia& source);
    [[nodiscard]] rloft::Type1Raster res0_type1(int index) const override;
    [[nodiscard]] rloft::Type1Raster type1(int archive,int index) const override;
    [[nodiscard]] std::uint8_t code1_u8(std::size_t offset) const override;
    [[nodiscard]] std::uint16_t code1_u16(std::size_t offset) const override;
    [[nodiscard]] std::int32_t code1_s32(std::size_t offset) const override;
    [[nodiscard]] std::string code1_cstring(std::size_t offset) const override;
    [[nodiscard]] rloft::Type2Font res0_font(int index) const override;
    [[nodiscard]] std::string text(int index) const override;
    [[nodiscard]] rloft::Palette6 colors_palette(
        int index,int brightness=0) const override;
private:
    const NativeMedia& media_;
    NativeResArchive res0_;
    mutable std::optional<NativeResArchive> res1_,res7_,res8_;
    mutable std::optional<std::vector<std::uint8_t>> code1_;
    std::vector<std::uint8_t> colors_;
    std::vector<std::uint8_t> text_;
};
} // namespace rloft_native
