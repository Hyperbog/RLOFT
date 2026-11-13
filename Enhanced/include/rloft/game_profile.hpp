#pragma once

// Shared native game identity. A profile is not proof of runtime compatibility.
// Media layouts and rule differences must be verified from each original game.
#include <array>
#include <cstdint>
#include <optional>
#include <string_view>

namespace rloft {

enum class GameId : std::uint8_t {
    StrahdsPossession,
    Menzoberranzan,
    StoneProphet,
};

enum class GameReadiness : std::uint8_t {
    NativePreAlpha,
    OriginalMediaNotYetAnalyzed,
};

struct GameProfile final {
    GameId id;
    std::string_view key;
    std::string_view title;
    GameReadiness readiness;
};

inline constexpr std::array<GameProfile, 3> kGameProfiles{{
    {GameId::StrahdsPossession, "strahd", "Ravenloft: Strahd's Possession",
     GameReadiness::NativePreAlpha},
    {GameId::Menzoberranzan, "menzo", "Menzoberranzan",
     GameReadiness::OriginalMediaNotYetAnalyzed},
    {GameId::StoneProphet, "stone-prophet", "Ravenloft: Stone Prophet",
     GameReadiness::OriginalMediaNotYetAnalyzed},
}};

[[nodiscard]] constexpr const GameProfile* game_profile(GameId id) noexcept {
    for (const auto& profile : kGameProfiles) {
        if (profile.id == id) return &profile;
    }
    return nullptr;
}

[[nodiscard]] constexpr std::optional<GameId> game_id_from_key(
    std::string_view key) noexcept {
    for (const auto& profile : kGameProfiles) {
        if (profile.key == key) return profile.id;
    }
    return std::nullopt;
}

[[nodiscard]] constexpr bool has_native_player(GameId id) noexcept {
    const auto* profile = game_profile(id);
    return profile && profile->readiness == GameReadiness::NativePreAlpha;
}

}  // namespace rloft
