#pragma once

#include <cstdint>
#include <optional>

namespace rloft {

// Native gameplay soundtrack cursor. The authored LEVELS.DAT header
// byte +0x29C identifies the current RES5 sequence (0 is valid music).
// MIDI playback belongs to the Windows host, not to gameplay state.
// Repeated 60Hz world frames must not restart their music, while a level
// change or full-screen cinematic must re-enable the authored cue.
class EnhancedWorldMusicState final {
public:
    [[nodiscard]] bool should_start(std::uint32_t level_id,
                                     std::uint8_t sequence_id) noexcept {
        const Cue next{level_id,sequence_id};
        if(last_.has_value() && *last_==next) return false;
        last_=next;
        return true;
    }

    void invalidate() noexcept {last_.reset();}

private:
    struct Cue final {
        std::uint32_t level_id{};
        std::uint8_t sequence_id{};
        [[nodiscard]] bool operator==(const Cue& rhs) const noexcept {
            return level_id==rhs.level_id &&
                   sequence_id==rhs.sequence_id;
        }
    };
    std::optional<Cue> last_;
};

} // namespace rloft
