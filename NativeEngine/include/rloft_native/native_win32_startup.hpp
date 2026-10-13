#pragma once
#ifdef _WIN32
#include <windows.h>
namespace rloft_native {
class NativeMedia;
// Read-only original CODE.2 cold-start visual and menu entry.
// Does not claim CODE.1 creation or gameplay is integrated.
void open_native_startup(HWND owner,const NativeMedia& media);
}
#endif
