#pragma once
#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#include <GL/gl.h>
#include <cstdint>
namespace rloft_native {
class NativeGlViewportTarget {
public:
    NativeGlViewportTarget()=default;
    NativeGlViewportTarget(const NativeGlViewportTarget&)=delete;
    NativeGlViewportTarget& operator=(const NativeGlViewportTarget&)=delete;
    void initialize();
    void release();
    void begin(int window_width,int window_height,unsigned preset,unsigned aa);
    void present(int window_width,int window_height);
    [[nodiscard]] int width()const noexcept{return render_width_;}
    [[nodiscard]] int height()const noexcept{return render_height_;}
    [[nodiscard]] unsigned actual_samples()const noexcept{return allocated_samples_;}
    [[nodiscard]] bool supports_fbo()const noexcept{return supported_;}
    [[nodiscard]] bool supports_msaa()const noexcept{return multisample_supported_;}
    [[nodiscard]] static unsigned preset_count()noexcept{return 5U;}
    [[nodiscard]] static const wchar_t* preset_name(unsigned preset);
    [[nodiscard]] static void resolve_preset(unsigned preset,int window_width,
       int window_height,int& render_width,int& render_height);
private:
    void allocate(int w,int h,unsigned aa);
    void destroy_target();
    int render_width_{},render_height_{};
    unsigned allocated_samples_{};
    bool supported_{},multisample_supported_{},use_target_{};
    GLuint texture_{},fbo_{},depth_{},multisample_fbo_{},multisample_color_{},multisample_depth_{};
};
}
#endif
