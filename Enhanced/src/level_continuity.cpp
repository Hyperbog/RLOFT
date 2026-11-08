#include "rloft/level_continuity.hpp"

#include <stdexcept>
#include <utility>

namespace rloft {
namespace {

void validate_target(
    const ExplorationSessionState& session,
    const LevelTransitionLocation& target,
    const LevelContinuityEntry& destination) {
    if (target.level_id < 0 || target.level_id > 999 ||
        session.level_id > 999U) {
        throw std::out_of_range("original level transition id");
    }
    if (session.level_id == static_cast<std::uint32_t>(target.level_id)) {
        throw std::logic_error("level transition cannot re-enter live level");
    }
    if (target.facing >= 360U) {
        throw std::out_of_range("original level transition facing");
    }
    const auto& level = destination.level;
    if (level.width <= 0 || level.height <= 0 ||
        level.cell_words.size() !=
            static_cast<std::size_t>(level.width) *
            static_cast<std::size_t>(level.height)) {
        throw std::runtime_error("destination level geometry invalid");
    }
    // Original CODE.2 16.16 coordinate A is column, B is row.
    // Reject an out-of-bounds entry instead of silently relocating party.
    if ((target.coordinate_a >> 16U) >=
            static_cast<std::uint32_t>(level.width) ||
        (target.coordinate_b >> 16U) >=
            static_cast<std::uint32_t>(level.height)) {
        throw std::out_of_range("original level destination outside grid");
    }
}

} // namespace

std::optional<LevelContinuityEntry> LevelContinuityStore::recall(
    int level_id) const {
    const auto it = snapshots_.find(level_id);
    if (it == snapshots_.end()) return std::nullopt;
    return it->second; // independent copy; caller cannot mutate cache
}

void LevelContinuityStore::enter(
    ExplorationSessionState& session,
    LevelWorldState& current_world,
    const LevelTransitionLocation& target,
    LevelContinuityEntry destination) {
    validate_target(session, target, destination);

    // Do all potentially throwing copies and allocations before updating
    // the authoritative live session. Previously visited destinations
    // are removed only from the staged store, never from live data.
    auto staged = snapshots_;
    LevelContinuityEntry departing{};
    departing.level = session.level;
    departing.world = current_world;
    departing.from_direct_save = session.level_from_direct_save;
    staged.insert_or_assign(
        static_cast<int>(session.level_id), std::move(departing));
    staged.erase(target.level_id);

    using std::swap;
    swap(session.level, destination.level);
    swap(current_world, destination.world);
    session.level_id = static_cast<std::uint32_t>(target.level_id);
    session.coordinate_a = target.coordinate_a;
    session.coordinate_b = target.coordinate_b;
    session.facing = target.facing;
    session.level_from_direct_save = destination.from_direct_save;
    snapshots_.swap(staged);
}

}  // namespace rloft
