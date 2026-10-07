#pragma once

#include <cstdint>
#include <optional>
#include <string_view>
#include <vector>

#include "rloft/indexed_font_presenter.hpp"
#include "rloft/resource_assets.hpp"
#include "rloft/startup_presenter.hpp"

namespace rloft {

enum class QuitConfirmChoice {
    None,
    Cancel,
    Confirm,
};

class Code2Random15 final {
public:
    explicit Code2Random15(std::uint32_t seed) noexcept
        : state_(seed) {}

    [[nodiscard]] std::uint32_t next() noexcept;
    [[nodiscard]] std::uint32_t state() const noexcept {
        return state_;
    }

private:
    std::uint32_t state_{};
};

struct QuitConfirmTile final {
    int frame{};
    int row{};
    int column{};
};

class QuitConfirmModel final {
public:
    static constexpr int kBackgroundLocalResource = 0x35;
    static constexpr int kButtonLocalResource = 0x63;

    static constexpr int kBoxTop = 0xA0;
    static constexpr int kBoxBottom = 0xE0;
    static constexpr int kBoxLeft = 0x20;
    static constexpr int kBoxRight = 0x118;

    static constexpr int kButtonRow = 0xC8;
    static constexpr int kCancelColumn = 0x60;
    static constexpr int kConfirmColumn = 0xB0;

    [[nodiscard]] static constexpr std::string_view prompt() noexcept {
        return "do you wish to quit?";
    }

    [[nodiscard]] static int background_resource_id(
        std::uint16_t res0_raster_base) noexcept {
        return static_cast<int>(res0_raster_base) +
               kBackgroundLocalResource;
    }

    [[nodiscard]] static int button_resource_id(
        std::uint16_t res0_raster_base) noexcept {
        return static_cast<int>(res0_raster_base) +
               kButtonLocalResource;
    }

    [[nodiscard]] static QuitConfirmChoice choice_at(
        int row,
        int column) noexcept;

    // Exact 07D44 tile order and frame-selection arithmetic, conditional on
    // the caller-provided current 30CA2 RNG seed.
    [[nodiscard]] static std::vector<QuitConfirmTile> background_tiles(
        std::uint32_t seed);
};

class QuitConfirmPresenter final {
public:
    explicit QuitConfirmPresenter(
        const RavenloftBootAssets& assets)
        : assets_(assets),
          font1_(assets.res0_font(1)) {}

    void draw(
        IndexedFramebuffer& framebuffer,
        std::uint32_t rng_seed,
        QuitConfirmChoice pressed =
            QuitConfirmChoice::None) const;

private:
    const RavenloftBootAssets& assets_;
    Type2Font font1_{};
};

}  // namespace rloft
