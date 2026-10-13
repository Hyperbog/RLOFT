#pragma once
#include <array>
#include <cstdint>
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
// A read-only, original-file-backed seam for CODE.2 startup art.
// This interface has no dependency on GameSource, NativeMedia,
// Windows, DemoV1, code execution, or authored substitute imagery.
class StartupVisualSource {
public:
    virtual ~StartupVisualSource()=default;
    [[nodiscard]] virtual Type1Raster res0_type1(int index) const=0;
    [[nodiscard]] virtual Palette6 colors_palette(
        int index,int brightness=0) const=0;
};
}
