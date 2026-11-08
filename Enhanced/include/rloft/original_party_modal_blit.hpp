#pragma once

#include "rloft/startup_presenter.hpp"

namespace rloft {

// Distinct original CODE.2 graphics dispatchers used by the Level22
// party/inventory modal. 2E510( row, column, resource, bank )
// resolves a descriptor and 2E4A4 draws ALL frames, advancing the
// output row by descriptor WORD+4 after each one.
// 2975C( frame, row, column, resource, bank ) draws ONE selected
// source frame. Do not conflate them: RES0 backgrounds may be stored
// as 200 frames of 320x1; icons remain single-frame blits.
class OriginalPartyModalBlit final {
public:
    static void complete_2e510(
        IndexedFramebuffer& framebuffer,
        const Type1Raster& resource,int row,int column) {
        framebuffer.draw(resource,row,column);
    }
    static void frame_2975c(
        IndexedFramebuffer& framebuffer,
        const Type1Raster& resource,int frame,int row,int column) {
        framebuffer.draw_frame(resource,frame,row,column);
    }
};

} // namespace rloft
