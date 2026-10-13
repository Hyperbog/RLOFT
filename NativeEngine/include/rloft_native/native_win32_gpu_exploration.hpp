#pragma once
#ifdef _WIN32
#include <windows.h>
#include "rloft_native/native_party_start.hpp"
namespace rloft_native {
class NativeMedia;
void open_native_gpu_world_preview(HWND owner,const NativeMedia& media);
// Real CODE.1 two-actor transfer; source-level party entry, not a free
// level-select preview. Interactions, saves and DOS frame parity still open.
void open_native_gpu_world_session(
    HWND owner,const NativeMedia& media,const NativeCharsImage& chars);
}
#endif
