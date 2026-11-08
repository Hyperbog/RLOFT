#pragma once
#include <cstddef>
#include <cstdint>
#include <optional>
#include <stdexcept>

namespace rloft {

// Enhanced PC INPUT binding, not a substitute for original DOS DB94.
// The target click must refer to the SAME source scene the player saw
// when arming a source-qualified hand. Prevent a held click from
// becoming a different action after movement, map/modal or save-load.
struct EnhancedCombatSceneStamp final {
    std::uint64_t world_revision{};
    // ExplorationRuntimeFrame::source_level_id is an int.
    // Match the actual level source type to avoid a Windows MSVC
    // brace-initializer narrowing and any signed/unsigned wrap.
    int level_id{};
    std::uint32_t coordinate_a{};
    std::uint32_t coordinate_b{};
    std::uint32_t facing{};
    [[nodiscard]] constexpr bool matches(
        const EnhancedCombatSceneStamp& other) const noexcept {
        return world_revision==other.world_revision &&
            level_id==other.level_id &&
            coordinate_a==other.coordinate_a &&
            coordinate_b==other.coordinate_b &&
            facing==other.facing;
    }
};

struct EnhancedCombatTargetAttempt final {
    bool consumed{}; // also true when stale or no NPC was hit!
    bool valid{};
    std::size_t actor{};
    std::size_t hand{};
};

class EnhancedCombatTargetArming final {
public:
    void arm(std::size_t actor,std::size_t hand,
             EnhancedCombatSceneStamp frame) {
        if(actor>=4U || hand>=2U)
            throw std::out_of_range("native combat hand bounds");
        armed_=Armed{actor,hand,frame};
    }
    [[nodiscard]] bool pending() const noexcept {
        return armed_.has_value();
    }
    void cancel() noexcept { armed_.reset(); }

    // Once any viewport target click arrives, consume it exactly once.
    // A missing/stale frame is NOT a floor-item pickup request.
    [[nodiscard]] EnhancedCombatTargetAttempt take(
        const std::optional<EnhancedCombatSceneStamp>& frame) noexcept {
        if(!armed_)return {};
        const auto previous=*armed_;
        armed_.reset();
        return {true,frame && previous.frame.matches(*frame),
                previous.actor,previous.hand};
    }

private:
    struct Armed final {
        std::size_t actor{};
        std::size_t hand{};
        EnhancedCombatSceneStamp frame{};
    };
    std::optional<Armed> armed_;
};

} // namespace rloft
