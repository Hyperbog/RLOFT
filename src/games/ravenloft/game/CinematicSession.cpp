#include "games/ravenloft/game/CinematicSession.h"
#include <stdexcept>

namespace ravenloft::game {
namespace {
constexpr std::size_t CinematicTextBase = 0x324; // CODE.2 type-7 handler 0x137C0.
}

CinematicSession::CinematicSession(const std::filesystem::path& moviePath,
                                   const data::TextDatabase& textDatabase)
    : text_(textDatabase), movie_(moviePath) {}

void CinematicSession::applyMetadata(const data::AnmMetadataEvent& event) {
    if (event.kind != data::AnmMetadataEvent::Kind::Subtitle) return;
    subtitleDisplayParameter_ = event.b;
    if (event.a == 0) {
        subtitle_.reset();
        return;
    }
    const std::size_t textIndex = CinematicTextBase + event.a;
    if (textIndex >= text_.count()) throw std::runtime_error("ANM subtitle text index outside T database");
    subtitle_ = text_.at(textIndex);
}

CinematicStep CinematicSession::step() {
    if (finished()) return {nextFrame_, true, {}};
    const std::size_t frameIndex = nextFrame_++;
    auto events = decoder_.apply(movie_.frames().at(frameIndex));
    for (const auto& event : events) applyMetadata(event);
    return {frameIndex, finished(), std::move(events)};
}

void CinematicSession::reset() {
    decoder_ = data::AnmDecoder{};
    nextFrame_ = 0;
    subtitle_.reset();
    subtitleDisplayParameter_ = 0;
}

} // namespace ravenloft::game
