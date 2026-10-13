#ifdef _WIN32
#define NOMINMAX
#include "rloft_native/native_win32_gl_target.hpp"
#include <algorithm>
#include <stdexcept>
#include <string>
namespace rloft_native {
namespace {
// Resolve core OpenGL 3.x and ARB/EXT function names at runtime. Windows
// gl.h exports only GL 1.1. Do not require a third-party DLL.
constexpr GLenum kFramebuffer=0x8D40;
constexpr GLenum kReadFramebuffer=0x8CA8;
constexpr GLenum kDrawFramebuffer=0x8CA9;
constexpr GLenum kRenderbuffer=0x8D41;
constexpr GLenum kColorAttachment0=0x8CE0;
constexpr GLenum kDepthAttachment=0x8D00;
constexpr GLenum kFramebufferComplete=0x8CD5;
constexpr GLenum kRGBA8=0x8058;
constexpr GLenum kDepth24=0x81A6;
constexpr GLenum kMaxSamples=0x8D57;
using Gen=(void(APIENTRY*)(GLsizei,GLuint*));
using Bind=(void(APIENTRY*)(GLenum,GLuint));
using Delete=(void(APIENTRY*)(GLsizei,const GLuint*));
using TexAttach=(void(APIENTRY*)(GLenum,GLenum,GLenum,GLuint,GLint));
using RenderAttach=(void(APIENTRY*)(GLenum,GLenum,GLenum,GLuint));
using RenderStore=(void(APIENTRY*)(GLenum,GLenum,GLsizei,GLsizei));
using RenderStoreMsaa=(void(APIENTRY*)(GLenum,GLsizei,GLenum,GLsizei,GLsizei));
using Check=(GLenum(APIENTRY*)(GLenum));
using Blit=(void(APIENTRY*)(GLint,GLint,GLint,GLint,GLint,GLint,GLint,GLint,GLbitfield,GLenum));
struct Functions {
    Gen gen_frames{};
    Bind bind_frames{};
    Delete delete_frames{};
    Gen gen_render{};
    Bind bind_render{};
    Delete delete_render{};
    TexAttach tex_attach{};
    RenderAttach render_attach{};
    RenderStore store{};
    RenderStoreMsaa store_msaa{};
    Check check{};
    Blit blit{};
};
Functions fn;
void* proc(const char* base,const char* ext) {
    PROC addr=wglGetProcAddress(base);
    if(!addr || addr==reinterpret_cast<PROC>(1) ||
       addr==reinterpret_cast<PROC>(2) ||
       addr==reinterpret_cast<PROC>(3) ||
       addr==reinterpret_cast<PROC>(-1))
        addr=wglGetProcAddress(ext);
    return reinterpret_cast<void*>(addr);
}
bool load() {
    fn.gen_frames=reinterpret_cast<Gen>(proc("glGenFramebuffers","glGenFramebuffersEXT"));
    fn.bind_frames=reinterpret_cast<Bind>(proc("glBindFramebuffer","glBindFramebufferEXT"));
    fn.delete_frames=reinterpret_cast<Delete>(proc("glDeleteFramebuffers","glDeleteFramebuffersEXT"));
    fn.gen_render=reinterpret_cast<Gen>(proc("glGenRenderbuffers","glGenRenderbuffersEXT"));
    fn.bind_render=reinterpret_cast<Bind>(proc("glBindRenderbuffer","glBindRenderbufferEXT"));
    fn.delete_render=reinterpret_cast<Delete>(proc("glDeleteRenderbuffers","glDeleteRenderbuffersEXT"));
    fn.tex_attach=reinterpret_cast<TexAttach>(proc("glFramebufferTexture2D","glFramebufferTexture2DEXT"));
    fn.render_attach=reinterpret_cast<RenderAttach>(proc("glFramebufferRenderbuffer","glFramebufferRenderbufferEXT"));
    fn.store=reinterpret_cast<RenderStore>(proc("glRenderbufferStorage","glRenderbufferStorageEXT"));
    fn.check=reinterpret_cast<Check>(proc("glCheckFramebufferStatus","glCheckFramebufferStatusEXT"));
    fn.store_msaa=reinterpret_cast<RenderStoreMsaa>(proc("glRenderbufferStorageMultisample","glRenderbufferStorageMultisampleEXT"));
    fn.blit=reinterpret_cast<Blit>(proc("glBlitFramebuffer","glBlitFramebufferEXT"));
    return fn.gen_frames&&fn.bind_frames&&fn.delete_frames&&fn.gen_render&&
        fn.bind_render&&fn.delete_render&&fn.tex_attach&&fn.render_attach&&
        fn.store&&fn.check;
}
void require_complete() {
    if(fn.check(kFramebuffer)!=kFramebufferComplete)
        throw std::runtime_error("GPU render-target framebuffer incomplete; try Native or disable anti-aliasing");
}
} // namespace
const wchar_t* NativeGlViewportTarget::preset_name(unsigned p) {
    constexpr const wchar_t* names[]{
        L"Native / Auto",L"HD 1280x720",L"Full HD 1920x1080",
        L"QHD 2560x1440",L"4K UHD 3840x2160"};
    if(p>=5U)throw std::invalid_argument("invalid GPU render preset");
    return names[p];
}
void NativeGlViewportTarget::resolve_preset(unsigned p,int ww,int wh,int& w,int& h) {
    if(p>=5U || ww<=0 || wh<=0 || ww>7680 || wh>4320)
        throw std::invalid_argument("invalid GPU window size or preset");
    constexpr int sizes[5][2]{
        {0,0},{1280,720},{1920,1080},{2560,1440},{3840,2160}};
    w=p==0?ww:sizes[p][0];
    h=p==0?wh:sizes[p][1];
}
void NativeGlViewportTarget::initialize() {
    supported_=load();
    multisample_supported_=supported_&&fn.store_msaa&&fn.blit;
}
void NativeGlViewportTarget::destroy_target() {
    if(fn.bind_frames)fn.bind_frames(kFramebuffer,0);
    if(texture_)glDeleteTextures(1,&texture_);
    if(depth_&&fn.delete_render)fn.delete_render(1,&depth_);
    if(multisample_color_&&fn.delete_render)fn.delete_render(1,&multisample_color_);
    if(multisample_depth_&&fn.delete_render)fn.delete_render(1,&multisample_depth_);
    if(fbo_&&fn.delete_frames)fn.delete_frames(1,&fbo_);
    if(multisample_fbo_&&fn.delete_frames)fn.delete_frames(1,&multisample_fbo_);
    texture_=fbo_=depth_=multisample_fbo_=multisample_color_=multisample_depth_=0;
    render_width_=render_height_=0;allocated_samples_=0;use_target_=false;
}
void NativeGlViewportTarget::release(){destroy_target();}
void NativeGlViewportTarget::allocate(int w,int h,unsigned samples) {
    if(!supported_)
        throw std::runtime_error(
            "Selected native-to-4K rendering needs OpenGL framebuffer objects, unsupported by this graphics driver");
    GLint max_texture=0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE,&max_texture);
    if(w>max_texture || h>max_texture)
        throw std::runtime_error("Requested internal rendering resolution exceeds GPU texture limit");
    destroy_target();
    render_width_=w;render_height_=h;allocated_samples_=samples;
    glGenTextures(1,&texture_);
    glBindTexture(GL_TEXTURE_2D,texture_);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,w,h,0,GL_RGBA,GL_UNSIGNED_BYTE,nullptr);
    fn.gen_frames(1,&fbo_);fn.bind_frames(kFramebuffer,fbo_);
    fn.tex_attach(kFramebuffer,kColorAttachment0,GL_TEXTURE_2D,texture_,0);
    fn.gen_render(1,&depth_);fn.bind_render(kRenderbuffer,depth_);
    fn.store(kRenderbuffer,kDepth24,w,h);
    fn.render_attach(kFramebuffer,kDepthAttachment,kRenderbuffer,depth_);
    require_complete();
    if(samples>0) {
        if(!multisample_supported_)
            throw std::runtime_error("GPU does not support 2x/4x OpenGL framebuffer MSAA");
        GLint max_samples=0;
        glGetIntegerv(kMaxSamples,&max_samples);
        if(unsigned(max_samples)<samples)
            throw std::runtime_error("Selected MSAA exceeds OpenGL device sample limit");
        fn.gen_frames(1,&multisample_fbo_);fn.bind_frames(kFramebuffer,multisample_fbo_);
        fn.gen_render(1,&multisample_color_);fn.bind_render(kRenderbuffer,multisample_color_);
        fn.store_msaa(kRenderbuffer,GLsizei(samples),kRGBA8,w,h);
        fn.render_attach(kFramebuffer,kColorAttachment0,kRenderbuffer,multisample_color_);
        fn.gen_render(1,&multisample_depth_);fn.bind_render(kRenderbuffer,multisample_depth_);
        fn.store_msaa(kRenderbuffer,GLsizei(samples),kDepth24,w,h);
        fn.render_attach(kFramebuffer,kDepthAttachment,kRenderbuffer,multisample_depth_);
        require_complete();
    }
    fn.bind_frames(kFramebuffer,0);
    use_target_=true;
}
void NativeGlViewportTarget::begin(int ww,int wh,unsigned preset,unsigned aa) {
    int w=0,h=0;resolve_preset(preset,ww,wh,w,h);
    if(aa!=0 && aa!=2 && aa!=4)throw std::invalid_argument("invalid AA sample count");
    use_target_=preset!=0U || aa!=0U;
    if(!use_target_) {
        // Original native/auto mode renders directly into the actual
        // window, with NO invented upscale or hidden 4K buffer.
        if(fn.bind_frames)fn.bind_frames(kFramebuffer,0);
        render_width_=w;render_height_=h;allocated_samples_=0;
        glViewport(0,0,w,h);return;
    }
    if(!fbo_ || render_width_!=w || render_height_!=h ||
       allocated_samples_!=aa)allocate(w,h,aa);
    fn.bind_frames(kFramebuffer,aa?multisample_fbo_:fbo_);
    glViewport(0,0,w,h);
}
void NativeGlViewportTarget::present(int ww,int wh) {
    if(!use_target_)return;
    const int w=render_width_,h=render_height_;
    if(allocated_samples_) {
        fn.bind_frames(kReadFramebuffer,multisample_fbo_);
        fn.bind_frames(kDrawFramebuffer,fbo_);
        fn.blit(0,0,w,h,0,0,w,h,GL_COLOR_BUFFER_BIT,GL_NEAREST);
    }
    fn.bind_frames(kFramebuffer,0);
    glViewport(0,0,ww,wh);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glMatrixMode(GL_PROJECTION);glLoadIdentity();
    glOrtho(0,ww,0,wh,-1,1);
    glMatrixMode(GL_MODELVIEW);glLoadIdentity();
    glClearColor(0,0,0,1);glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    const double scale=std::min(double(ww)/w,double(wh)/h);
    const double vw=w*scale,vh=h*scale;
    const double left=(ww-vw)*0.5,bottom=(wh-vh)*0.5;
    glEnable(GL_TEXTURE_2D);glBindTexture(GL_TEXTURE_2D,texture_);
    glColor3f(1.f,1.f,1.f);
    glBegin(GL_QUADS);
    glTexCoord2f(0,0);glVertex2d(left,bottom);
    glTexCoord2f(1,0);glVertex2d(left+vw,bottom);
    glTexCoord2f(1,1);glVertex2d(left+vw,bottom+vh);
    glTexCoord2f(0,1);glVertex2d(left,bottom+vh);
    glEnd();
    glEnable(GL_DEPTH_TEST);
}
} // namespace rloft_native
#endif
