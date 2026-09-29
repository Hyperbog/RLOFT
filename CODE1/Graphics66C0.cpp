#include "Graphics66C0.h"

#include <cstddef>

namespace ravenloft::game {
namespace {

void clipRectArgs(std::int32_t& x,
                  std::int32_t& y,
                  std::int32_t& width,
                  std::int32_t& height,
                  const GraphicsState66C0& state) noexcept {
    if (x < 0) {
        width += x;
        x = 0;
    }

    if (y < 0) {
        height += y;
        y = 0;
    }

    if (x + width > state.screenWidth23D7) {
        width = state.screenWidth23D7 - x;
    }

    if (y + height > state.screenHeight23D9) {
        height = state.screenHeight23D9 - y;
    }
}

ClipWork6854& pushScratch(
    GraphicsState66C0& state,
    const GraphicsOps66C0& ops) {
    ops.callDA18();

    auto& work =
        state.scratch240C[
            static_cast<std::size_t>(
                state.scratchDepth246C)];

    ++state.scratchDepth246C;

    ops.callDA1A();
    return work;
}

void popScratch(GraphicsState66C0& state,
                const GraphicsOps66C0& ops) {
    ops.callDA18();
    --state.scratchDepth246C;
    ops.callDA1A();
}

void prepareClip(std::int32_t& x,
                 std::int32_t& y,
                 const ResourceView66C0& resource,
                 ClipWork6854& work,
                 GraphicsState66C0& state) noexcept {
    work = {};
    work.width = resource.width;

    std::int32_t delta =
        state.clipLeft23ED - x;

    if (delta > 0) {
        work.width -= delta;
        work.sourceLeadBytes =
            delta * resource.heightPixels();
        x = state.clipLeft23ED;
    }

    delta =
        x + work.width - 1 -
        state.clipRight23F1;

    if (delta > 0) {
        work.width -= delta;
        work.sourceTailBytes =
            delta * resource.heightPixels();
    }

    work.height =
        resource.heightPixels();

    delta =
        state.clipTop23F5 - y;

    if (delta > 0) {
        work.height -= delta;
        work.topSkip = delta;
        y = state.clipTop23F5;
    }

    delta =
        y + work.height - 1 -
        state.clipBottom23F9;

    if (delta > 0) {
        work.height -= delta;
        work.bottomSkip = delta;
    }

    state.words2401[0] =
        static_cast<std::int16_t>(x);
    state.words2401[1] =
        static_cast<std::int16_t>(y);
    state.words2401[2] =
        static_cast<std::int16_t>(work.width);
    state.words2401[3] =
        static_cast<std::int16_t>(work.height);
}

} // namespace

void Graphics66C0::capture(
    std::uintptr_t buffer,
    std::int32_t x,
    std::int32_t y,
    std::int32_t width,
    std::int32_t height,
    const GraphicsState66C0& state,
    const GraphicsOps66C0& ops) {
    clipRectArgs(x, y, width, height, state);

    if (width > 0 && height > 0) {
        ops.callA484(
            buffer, x, y, width, height);
    }
}

void Graphics66C0::restore(
    std::uintptr_t buffer,
    std::int32_t x,
    std::int32_t y,
    std::int32_t width,
    std::int32_t height,
    const GraphicsState66C0& state,
    const GraphicsOps66C0& ops) {
    clipRectArgs(x, y, width, height, state);

    // Original 0x679F uses JL rather than JLE:
    // height == 0 still reaches the low-level restore.
    if (width > 0 && height >= 0) {
        ops.callA4E8(
            buffer, x, y, width, height);
    }
}

void Graphics66C0::drawDirect(
    std::int32_t page,
    std::int32_t x,
    std::int32_t y,
    const ResourceView66C0& resource,
    const GraphicsOps66C0& ops) {
    if (resource) {
        ops.callA6F0(
            page, x, y, resource.handle);
        ops.call8960();
    }
}

void Graphics66C0::drawDirectMasked(
    std::int32_t page,
    std::int32_t x,
    std::int32_t y,
    const ResourceView66C0& resource,
    std::uint8_t mask,
    const GraphicsOps66C0& ops) {
    if (resource) {
        ops.callA7E8(
            page, x, y, resource.handle, mask);
        ops.call8960();
    }
}

void Graphics66C0::drawDirectMode(
    std::int32_t page,
    std::int32_t x,
    std::int32_t y,
    const ResourceView66C0& resource,
    std::int32_t mode,
    const GraphicsOps66C0& ops) {
    if (resource) {
        ops.callA8F4(
            page, x, y, resource.handle, mode);
        ops.call8960();
    }
}

std::int32_t Graphics66C0::drawClipped(
    std::int32_t page,
    std::int32_t x,
    std::int32_t y,
    const ResourceView66C0& resource,
    ClipWork6854& work,
    GraphicsState66C0& state,
    const GraphicsOps66C0& ops) {
    ops.call8960();

    if (!resource) {
        return 0;
    }

    prepareClip(
        x, y, resource, work, state);

    if (resource.width == work.width &&
        resource.heightPixels() == work.height) {
        ops.callA6F0(
            page, x, y, resource.handle);
        return 2;
    }

    if (work.width > 0 &&
        work.height > 0) {
        ops.callAA20(
            page, x, y, work, resource.handle);
        return 1;
    }

    return 0;
}

std::int32_t Graphics66C0::drawClippedMasked(
    std::int32_t page,
    std::int32_t x,
    std::int32_t y,
    const ResourceView66C0& resource,
    std::uint8_t mask,
    ClipWork6854& work,
    GraphicsState66C0& state,
    const GraphicsOps66C0& ops) {
    ops.call8960();

    if (!resource) {
        return 0;
    }

    prepareClip(
        x, y, resource, work, state);

    if (resource.width == work.width &&
        resource.heightPixels() == work.height) {
        ops.callA7E8(
            page, x, y, resource.handle, mask);
        return 2;
    }

    if (work.width > 0 &&
        work.height > 0) {
        ops.callAC10(
            page, x, y, work,
            resource.handle, mask);
        return 1;
    }

    return 0;
}

void Graphics66C0::setClip(
    std::int32_t left,
    std::int32_t top,
    std::int32_t right,
    std::int32_t bottom,
    GraphicsState66C0& state) noexcept {
    if (left < 0) {
        left = 0;
    }

    if (top < 0) {
        top = 0;
    }

    if (right >= state.screenWidth23D7) {
        right =
            state.screenWidth23D7 - 1;
    }

    if (bottom >= state.screenHeight23D9) {
        bottom =
            state.screenHeight23D9 - 1;
    }

    top &= ~3;

    // The original expands the bottom coordinate to the final scanline
    // of its four-line group.
    bottom =
        (bottom / 4) * 4 + 3;

    state.clipLeft23ED = left;
    state.clipTop23F5 = top;
    state.clipRight23F1 = right;
    state.clipBottom23F9 = bottom;
}

std::array<std::int32_t, 4>
Graphics66C0::getClip(
    const GraphicsState66C0& state) noexcept {
    return {
        state.clipLeft23ED,
        state.clipRight23F1,
        state.clipTop23F5,
        state.clipBottom23F9
    };
}

void Graphics66C0::drawResource(
    std::int32_t page,
    std::int32_t x,
    std::int32_t y,
    std::int32_t resourceId,
    std::int32_t bank,
    const GraphicsOps66C0& ops) {
    drawDirect(
        page, x, y,
        ops.call954C(resourceId, bank),
        ops);
}

std::int32_t
Graphics66C0::drawResourceClipped(
    std::int32_t page,
    std::int32_t x,
    std::int32_t y,
    std::int32_t resourceId,
    std::int32_t bank,
    GraphicsState66C0& state,
    const GraphicsOps66C0& ops) {
    auto& work =
        pushScratch(state, ops);

    const auto result =
        drawClipped(
            page, x, y,
            ops.call954C(resourceId, bank),
            work, state, ops);

    popScratch(state, ops);
    return result;
}

void Graphics66C0::callResource862C(
    std::int32_t x,
    std::int32_t y,
    std::int32_t resourceId,
    std::int32_t bank,
    GraphicsState66C0& state,
    const GraphicsOps66C0& ops) {
    const auto resource =
        ops.call954C(resourceId, bank);

    if (!resource) {
        return;
    }

    // The original reserves one of the four global scratch entries even
    // though 0x862C receives only x/y/resource.
    (void)pushScratch(state, ops);

    ops.call862C(
        x, y, resource.handle);

    popScratch(state, ops);
}

std::int32_t
Graphics66C0::drawViewClipped(
    std::int32_t page,
    std::int32_t x,
    std::int32_t y,
    const ResourceView66C0& resource,
    GraphicsState66C0& state,
    const GraphicsOps66C0& ops) {
    auto& work =
        pushScratch(state, ops);

    const auto result =
        drawClipped(
            page, x, y, resource,
            work, state, ops);

    popScratch(state, ops);
    return result;
}

void Graphics66C0::drawResourceMasked(
    std::int32_t page,
    std::int32_t x,
    std::int32_t y,
    std::int32_t resourceId,
    std::int32_t bank,
    std::uint8_t mask,
    const GraphicsOps66C0& ops) {
    drawDirectMasked(
        page, x, y,
        ops.call954C(resourceId, bank),
        mask, ops);
}

std::int32_t
Graphics66C0::drawResourceClippedMasked(
    std::int32_t page,
    std::int32_t x,
    std::int32_t y,
    std::int32_t resourceId,
    std::int32_t bank,
    std::uint8_t mask,
    GraphicsState66C0& state,
    const GraphicsOps66C0& ops) {
    auto& work =
        pushScratch(state, ops);

    const auto result =
        drawClippedMasked(
            page, x, y,
            ops.call954C(resourceId, bank),
            mask, work, state, ops);

    popScratch(state, ops);
    return result;
}

std::int32_t
Graphics66C0::drawViewClippedMasked(
    std::int32_t page,
    std::int32_t x,
    std::int32_t y,
    const ResourceView66C0& resource,
    std::uint8_t mask,
    GraphicsState66C0& state,
    const GraphicsOps66C0& ops) {
    auto& work =
        pushScratch(state, ops);

    const auto result =
        drawClippedMasked(
            page, x, y, resource,
            mask, work, state, ops);

    popScratch(state, ops);
    return result;
}

void Graphics66C0::drawResourceMode(
    std::int32_t page,
    std::int32_t x,
    std::int32_t y,
    std::int32_t resourceId,
    std::int32_t bank,
    std::int32_t mode,
    const GraphicsOps66C0& ops) {
    drawDirectMode(
        page, x, y,
        ops.call954C(resourceId, bank),
        mode, ops);
}

} // namespace ravenloft::game
