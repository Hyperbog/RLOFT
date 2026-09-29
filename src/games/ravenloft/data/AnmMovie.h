#pragma once
#include "games/ravenloft/data/PaletteDatabase.h"
#include <array>
#include <cstdint>
#include <filesystem>
#include <vector>

namespace ravenloft::data {

struct AnmChunk {
    std::uint16_t type=0;
    std::vector<std::uint8_t> payload;
};
struct AnmFrame { std::vector<AnmChunk> chunks; };
struct AnmMetadataEvent {
    enum class Kind { Music, Sample, Subtitle, DirectSample } kind;
    std::uint16_t a=0,b=0;
};

class AnmMovie {
public:
    explicit AnmMovie(const std::filesystem::path& path);
    std::uint16_t frameCount() const { return frameCount_; }
    std::uint16_t timing() const { return timing_; }
    const std::vector<AnmFrame>& frames() const { return frames_; }
    std::size_t parsedBytes() const { return parsedBytes_; }
    std::size_t fileSize() const { return fileSize_; }
private:
    std::uint16_t frameCount_=0,timing_=0;
    std::vector<AnmFrame> frames_;
    std::size_t parsedBytes_=0,fileSize_=0;
};

// Faithful logical 320x400 Mode-X cinematic surface. Chunks are applied cumulatively.
class AnmDecoder {
public:
    static constexpr int Width=320, Height=400;
    AnmDecoder();
    std::vector<AnmMetadataEvent> apply(const AnmFrame& frame);
    const std::vector<std::uint8_t>& pixels() const { return pixels_; }
    const std::array<Rgb8,256>& palette() const { return palette_; }
private:
    std::vector<std::uint8_t> pixels_;
    std::array<Rgb8,256> palette_{};
    void applyPalette(const std::vector<std::uint8_t>& p);
    void applyFullRle(const std::vector<std::uint8_t>& p);
    void applySparsePlanar(const std::vector<std::uint8_t>& p);
    void applyRawPlanar(const std::vector<std::uint8_t>& p);
};

} // namespace ravenloft::data
