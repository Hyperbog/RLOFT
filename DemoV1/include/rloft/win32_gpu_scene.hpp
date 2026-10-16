#pragma once
#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#include <GL/gl.h>
#include <array>
#include <cstdint>
#include <map>
#include <memory>
#include <vector>
#include "rloft/enhanced_gpu_world_mesh.hpp"
#include "rloft/exploration_session.hpp"
#include "rloft/level_world_state.hpp"
#include "rloft/resource_assets.hpp"
#include "rloft/surface_texture.hpp"
#include "rloft/world_scene_record.hpp"

namespace rloft {
// OpenGL backend for the ONE continuous CODE.1 -> CODE.2 game.
// No private world, party, savefile, or separate exploration loop.
// A single frame combines GPU source geometry with the existing original
// indexed UI/chrome; modal UIs remain game-state-owned and unchanged.
class Win32GpuScene final {
public:
    Win32GpuScene(HWND hwnd,const RavenloftBootAssets& assets,
                  const Palette6& palette);
    ~Win32GpuScene();
    Win32GpuScene(const Win32GpuScene&)=delete;
    Win32GpuScene& operator=(const Win32GpuScene&)=delete;
    void set_palette(const Palette6& palette);
    void bind(const ExplorationSessionState& session,
              const LevelWorldState& world,
              const std::vector<WorldSceneRecord>& records,
              const std::vector<std::uint8_t>& environment_backdrop);
    void paint(const RECT& dst,int client_width,int client_height,
               const std::vector<std::uint32_t>& bgra,
               bool draw_world,unsigned filter_mode,double pitch_radians,
               bool menu_overlay=false);
    [[nodiscard]] bool ready() const noexcept { return context_!=nullptr; }
private:
    HWND hwnd_{};
    HDC dc_{};
    HGLRC context_{};
    const RavenloftBootAssets& assets_;
    SurfaceTextureArchive res4_;
    Palette6 palette_{};
    GLuint canvas_{};
    std::map<unsigned,GLuint> materials_,sprites_;
    std::map<unsigned,std::array<float,2>> sprite_uv_;
    std::map<unsigned,unsigned> sprite_width_;
    std::vector<std::uint8_t> canvas_pixels_;
    EnhancedGpuWorldMesh mesh_;
    const ExplorationSessionState* session_{};
    const LevelWorldState* world_{};
    std::vector<WorldSceneRecord> records_;
    std::vector<std::uint8_t> environment_backdrop_;
    std::uint64_t world_fingerprint_{};
    bool mesh_initialized_{};
    int current_level_{-1};
    bool world_failed_{};
    unsigned original_projection_focal_{};
    [[nodiscard]] std::uint64_t fingerprint(const LevelState& level) const;
    void make_current() const;
    [[nodiscard]] GLuint material(unsigned id);
    [[nodiscard]] GLuint sprite(unsigned id);
    void draw_mesh(double pitch,unsigned filter_mode,double aspect);
    void draw_sprites(double heading);
    void destroy_textures() noexcept;
};
} // namespace rloft
#endif
