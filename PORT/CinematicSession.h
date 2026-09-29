#pragma once
#include "games/ravenloft/data/AnmMovie.h"
#include "games/ravenloft/data/TextDatabase.h"
#include <cstddef>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace ravenloft::game {

struct CinematicStep {
    std::size_t frameIndex = 0;
    bool finished = false;
    std::vector<data::AnmMetadataEvent> metadata;
};

// Platform-neutral cinematic state. CODE.2's visual chunk handlers are reproduced
// by AnmDecoder; this layer applies the subtitle control semantics and exposes the
// audio/music commands for the platform audio backend.
class CinematicSession {
public:
    CinematicSession(const std::filesystem::path& moviePath,
                     const data::TextDatabase& textDatabase);

    CinematicStep step();
    void reset();

    bool finished() const { return nextFrame_ >= movie_.frames().size(); }
    std::size_t nextFrame() const { return nextFrame_; }
    std::uint16_t timing() const { return movie_.timing(); }
    const data::AnmDecoder& decoder() const { return decoder_; }
    const std::optional<std::string>& subtitle() const { return subtitle_; }
    std::uint16_t subtitleDisplayParameter() const { return subtitleDisplayParameter_; }
    const data::AnmMovie& movie() const { return movie_; }

private:
    const data::TextDatabase& text_;
    data::AnmMovie movie_;
    data::AnmDecoder decoder_;
    std::size_t nextFrame_ = 0;
    std::optional<std::string> subtitle_;
    std::uint16_t subtitleDisplayParameter_ = 0;

    void applyMetadata(const data::AnmMetadataEvent& event);
};

} // namespace ravenloft::game
