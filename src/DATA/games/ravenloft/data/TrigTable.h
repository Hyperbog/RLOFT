#pragma once
#include <cstdint>
#include <filesystem>
#include <array>
#include <vector>

namespace ravenloft::data {
struct RendererConstants {
    std::uint32_t tableWidthA=0;      // SINTAB loader -> DS:C6D8
    std::uint32_t tableWidthB=0;      // -> DS:C6D4
    std::uint32_t screenCenterX=0;    // -> DS:C6D0
    std::uint32_t viewportConstantA=0;// -> DS:C6DC
    std::uint32_t viewportConstantB=0;// -> DS:C6E0
    std::uint32_t cameraOffset=0;     // -> DS:2560C
    std::uint32_t perspectiveScale=0; // -> DS:C6CC
    std::uint32_t depthScaleNumerator=0; // -> DS:255E0
};

class TrigTable {
public:
    explicit TrigTable(const std::filesystem::path& sintabPath);
    // Reproduces CODE.2's signed-magnitude sine table + fixed multiply helper.
    std::int32_t sinMul16_16(std::int32_t magnitude16_16, int degrees) const;
    std::int32_t cosMul16_16(std::int32_t magnitude16_16, int degrees) const;
    std::uint32_t rawSine(int degrees) const;
    // Exact 2.14 fixed trig values used by CODE.2 0x34270/0x34275.
    std::int16_t sin14(int degrees) const;
    std::int16_t cos14(int degrees) const;
    const std::array<std::int16_t,128>& planeDistanceTable() const { return planeDistance_; } // SINTAB -> DS:19660
    const std::array<std::int16_t,128>& planeStartTable() const { return planeStart_; }       // SINTAB -> DS:193E0
    const RendererConstants& rendererConstants() const { return rendererConstants_; }
    int lightingPivot() const { return lightingPivot_; }       // SINTAB -> DS:C738
    int lightingTableExtent() const { return lightingTableExtent_; } // -> DS:C730
    int lightingAux() const { return lightingAux_; }           // -> DS:C734
private:
    std::vector<std::uint32_t> sine_; // 450 entries; +90 cosine alias is shipped in-table
    std::array<std::int16_t,128> planeDistance_{};
    std::array<std::int16_t,128> planeStart_{};
    RendererConstants rendererConstants_{};
    int lightingPivot_=0;
    int lightingTableExtent_=0;
    int lightingAux_=0;
    static int normalize(int d);
    static std::int32_t fixedMulSignMagnitude(std::int32_t value, std::uint32_t signMagnitude);
};
}
