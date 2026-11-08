#include "rloft/exploration_session.hpp"

#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace rloft {

ExplorationSessionState ExplorationSessionState::fresh_party(
    PartyImage party,
    LevelState level,
    bool level_from_direct_save) {
    if (level.width <= 4 || level.height <= 4) {
        throw std::runtime_error(
            "fresh Ravenloft party entry lies outside loaded level geometry");
    }

    ExplorationSessionState out{};
    out.level_id = kFreshLevelId;
    out.coordinate_a = kFreshCoordinateA;
    out.coordinate_b = kFreshCoordinateB;
    out.facing = kFreshFacing;
    out.time_of_day_16_16_hours = kFreshTimeOfDay;
    out.party = std::move(party);
    out.level = std::move(level);
    out.level_from_direct_save = level_from_direct_save;
    return out;
}

std::string ExplorationSessionLoader::standalone_level_filename(int level_id) {
    if (level_id < 0 || level_id > 999) {
        throw std::out_of_range("standalone level id");
    }

    std::ostringstream name;
    name << "LEV" << std::setfill('0') << std::setw(3) << level_id << ".DAT";
    return name.str();
}

LevelState ExplorationSessionLoader::load_level(
    const GameSource& source,
    int level_id,
    bool* used_direct_save) {
    if (level_id < 0) {
        throw std::out_of_range("level id");
    }

    const auto direct_name = standalone_level_filename(level_id);
    if (const auto direct = source.read_runtime(direct_name)) {
        if (used_direct_save != nullptr) {
            *used_direct_save = true;
        }
        return ExplorationStateCodec::parse_direct_level(*direct);
    }

    if (used_direct_save != nullptr) {
        *used_direct_save = false;
    }
    return ExplorationStateCodec::parse_archive_level(
        source.read("LEVELS.DAT"), level_id);
}

ExplorationSessionState ExplorationSessionLoader::load_fresh_party(
    const GameSource& source,
    const std::vector<std::uint8_t>& chars) {
    bool used_direct_save = false;
    auto level = load_level(
        source,
        static_cast<int>(ExplorationSessionState::kFreshLevelId),
        &used_direct_save);
    return ExplorationSessionState::fresh_party(
        PartyImage::from_bytes(chars),
        std::move(level),
        used_direct_save);
}

}  // namespace rloft
