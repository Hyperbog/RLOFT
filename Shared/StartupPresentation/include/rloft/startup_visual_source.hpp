#pragma once
#include <array>
#include <cstdint>
#include <string>
#include <vector>
namespace rloft {
struct Palette6 {
    std::array<std::uint8_t,768> rgb{};
};
struct Type1Raster {
    int count{};
    int width{};
    int height{};
    std::uint8_t transparent_index{0xFF};
    std::vector<std::uint8_t> pixels;
};
struct Type2Font {
    std::array<std::uint8_t,128> widths{};
    Type1Raster raster;
};
// Original ANM (CINE00/CINE12) subtitles load original RES0 EH2 font
// records and decode T text only on subtitle commands.
// A read-only, original-file-backed seam for CODE.2 startup art.
// This interface has no dependency on GameSource, NativeMedia,
// Windows, DemoV1, code execution, or authored substitute imagery.
class StartupVisualSource {
public:
    virtual ~StartupVisualSource()=default;
    [[nodiscard]] virtual Type1Raster res0_type1(int index) const=0;
    [[nodiscard]] virtual Type2Font res0_font(int index) const=0;
    [[nodiscard]] virtual std::string text(int index) const=0;
    [[nodiscard]] virtual Palette6 colors_palette(
        int index,int brightness=0) const=0;
};
}
