#pragma once
#ifdef _WIN32
#include <windows.h>
namespace rloft_native {
class NativeMedia;
// A separate read-only original LEVELS.DAT inspection window, not a
// replacement for the original gameplay entrypoint or DOS raycaster.
void open_native_world_preview(HWND owner,const NativeMedia& original_media);
} // namespace rloft_native
#endif
