#pragma once
#ifdef _WIN32
#include <windows.h>
namespace rloft_native {
class NativeMedia;
void open_native_gpu_world_preview(HWND owner,const NativeMedia& media);
}
#endif
