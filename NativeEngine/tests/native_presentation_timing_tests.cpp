#include "rloft_native/native_presentation_timing.hpp"
#include <cassert>
#include <cstdint>
using namespace rloft_native;
int main() {
    static_assert(kNativeLegacyAnimationFrameMs==55U);
    static_assert(kNativeWin32PollIntervalMs==16U);
    static_assert(70U*kNativeLegacyAnimationFrameMs>=3000U);
    static_assert(70U*kNativeLegacyAnimationFrameMs<=5000U);
    assert(!native_presentation_frame_due(0U,0U,55U));
    assert(!native_presentation_frame_due(54U,0U,55U));
    assert(native_presentation_frame_due(55U,0U,55U));
    assert(native_presentation_frame_due(99U,44U,55U));
    assert(!native_presentation_frame_due(5U,6U,55U));
    assert(!native_presentation_frame_due(140U,100U,55U));
    assert(native_presentation_frame_due(155U,100U,55U));
    assert(native_presentation_frame_due(
        UINT64_C(9000000000000)+55U,UINT64_C(9000000000000),55U));
}
