#!/usr/bin/env python3
"""Public Alpha guard: modern native Level22 scene (not DOS VGA fidelity).

Keeps the already-working intro animation/music timing separate from the
enhanced 60Hz exploration path. The actual Windows render/field acceptance
is independent of this source-only test.
"""
from __future__ import annotations

from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]


def verify(compositor:str,presenter:str,presenter_header:str,
           host:str,cmake:str,runtime:str,runtime_header:str,
           plan:str,plan_header:str) -> None:
    for item in (
        'class EnhancedType7Compositor final',
        'const Type7NativeRaster& sprite',
        'const std::vector<Type7VisibleColumn>& columns',
        'const std::array<std::uint16_t,129>& scale_steps',
        'kViewportLeft = 32',
        'kViewportRight = 287',
        'kViewportTop = 4',
        'kViewportBottom = 259',
        'kHorizonY = 132',
        'sprite.mask[src]==0U',
        'sprite.pixels[src]',
        'std::invalid_argument',
        'std::out_of_range',
    ):
        if item not in compositor:
            raise ValueError('native original-art scene compositor changed: '+item)

    for item in (
        'bool ExplorationPresenter::enhanced_ground_eligible(',
        'if (enhanced_ground_eligible(frame)) {',
        'assets_.res3_type7_native_raster(',
        'EnhancedType7Compositor::draw(',
        'constexpr bool kUseNativePcSprites = true;',
        'if (!kUseNativePcSprites &&',
        'hud_.draw_party_panels(',
        'SurfaceFillModel::draw_lower(',
        'walls_.draw_main_runs(',
    ):
        if item not in presenter:
            raise ValueError('native terrain/sprite pipeline changed: '+item)
    for item in (
        'if (writes == 0U)',
        '++output_coverage.type7_calls_zero_pixels;',
        'Type7SceneRenderOutcome::RasterizedNoPixels',
        'zero_pixel_draws.push_back(Type7SceneZeroPixelDraw{',
        'out.type7_zero_pixel_draws = std::move(zero_pixel_draws);',
    ):
        if item not in presenter:
            raise ValueError('native zero-write scene falsely accepted: '+item)
    for item in ('type7_calls_zero_pixels', 'type7_zero_pixel_draws',
                 'RasterizedNoPixels'):
        if item not in presenter_header:
            raise ValueError('missing zero-write scene evidence: '+item)
    for item in ('enhanced_type7_cache_','enhanced_ground_eligible('):
        if item not in presenter_header:
            raise ValueError('missing native sprite/ground interface: '+item)

    order=(
        'draw_environment(frame);',
        'if (enhanced_ground_eligible(frame)) {',
        'walls_.draw_main_runs(',
        'EnhancedType7Compositor::draw(',
        'hud_.draw_party_panels('
    )
    positions=[presenter.find(q) for q in order]
    if any(p<0 for p in positions) or positions!=sorted(positions):
        raise ValueError('viewport is not layered sky/ground/walls/sprites/HUD')

    # Frame pacing only changes for exploration. The opening cinematics,
    # DreamForge/SSI/TSR transitions and CODE.1 lady/cat/cards retain their
    # prior validated frame clocks and sound cues.
    start=host.index('int run_exploration(')
    end=host.index('bool code1_palette_reveal(',start)
    exploration=host[start:end]
    startup=host[end:]
    if 'constexpr double kNativeGameplayRefreshHz = 60.0;' not in host:
        raise ValueError('normal PC exploration refresh must remain 60Hz')
    # Adding a real native Options modal introduces another 60 Hz screen.
    # Do not penalize legitimate new modals by freezing the old count,
    # but reject deletion of any established presentation path.
    if exploration.count('1.0 / kNativeGameplayRefreshHz')<9:
        raise ValueError('all gameplay/UI modal paths require native cadence')
    if '1.0 / kVgaRefreshHz' in exploration:
        raise ValueError('VGA 70Hz leakage into native scene loop')
    if '1.0 / kVgaRefreshHz' not in startup:
        raise ValueError('previously accepted intro/character timing was replaced')
    if 'std::chrono::microseconds(54925LL * ticks)' not in host:
        raise ValueError('existing CODE.1 animation clock altered')
    if 'frame == 1 || frame == 36' not in host or 'frame == 25' not in host:
        raise ValueError('lady/cat character-opening sound cues changed')
    # Opt-in enhanced runtime skips the DOS VGA trace on every player frame.
    if ('!enhanced_pc_scene_rendering_' not in runtime or
            'scene_vga_masks_.has_value()' not in runtime or
            'set_enhanced_pc_scene_rendering(bool enabled)' not in runtime_header or
            'runtime.set_enhanced_pc_scene_rendering(!dev_trace);' not in host):
        raise ValueError('normal native frame still builds DOS VGA trace')
    if ('enhanced_pc_scene_rendering_);' not in runtime or
            'bool skip_dos_microcode = false' not in plan_header or
            'if (!skip_dos_microcode) {' not in plan):
        raise ValueError('native scene still requires DOS x86 descriptor planning')
    for marker in (
        'add_executable(enhanced_type7_compositor_tests',
        'add_test(NAME enhanced_type7_compositor_tests',
        'add_test(NAME enhanced_scene_mode_tests',
        'add_executable(world_scene_render_plan_tests',
        'NAME alpha_frontend_preservation_contract_tests',
    ):
        if marker not in cmake:
            raise ValueError('missing native/known-good frontend regression: '+marker)


def main()->int:
    p=ROOT
    paths=(
        'DemoV1/include/rloft/enhanced_type7_compositor.hpp',
        'DemoV1/src/exploration_presenter.cpp',
        'DemoV1/include/rloft/exploration_presenter.hpp',
        'DemoV1/src/win32_main.cpp',
        'DemoV1/CMakeLists.txt',
        'DemoV1/src/exploration_runtime.cpp',
        'DemoV1/include/rloft/exploration_runtime.hpp',
        'DemoV1/src/world_scene_render_plan.cpp',
        'DemoV1/include/rloft/world_scene_render_plan.hpp',
    )
    try:
        sources=[(p/path).read_text(encoding='utf-8') for path in paths]
        verify(*sources)
        mutations=(
            (0,'sprite.mask[src]==0U','false'),
            (0,'kHorizonY = 132','kHorizonY = 0'),
            (1,'if (enhanced_ground_eligible(frame)) {','if (false) {'),
            (1,'constexpr bool kUseNativePcSprites = true;',
             'constexpr bool kUseNativePcSprites = false;'),
            (1,'EnhancedType7Compositor::draw(', '/* no sprite */ ('),
            (1,'if (writes == 0U)', 'if (false)'),
            (3,'constexpr double kNativeGameplayRefreshHz = 60.0;',
             'constexpr double kNativeGameplayRefreshHz = 90.0;'),
            (3,'frame == 1 || frame == 36','frame == 2 || frame == 36'),
            (4,'add_executable(enhanced_type7_compositor_tests',
             'add_executable(disabled_sprite_tests'),
            (5,'!enhanced_pc_scene_rendering_', 'false'),
            (3,'runtime.set_enhanced_pc_scene_rendering(!dev_trace);',
             '/* native renderer disabled */'),
            (5,'enhanced_pc_scene_rendering_);','false);'),
            (7,'if (!skip_dos_microcode) {','if (true) {'),
        )
        for target,before,after in mutations:
            if before not in sources[target]:
                raise ValueError('negative fixture missing '+before)
            changed=sources.copy()
            changed[target]=changed[target].replace(before,after,1)
            try:
                verify(*changed)
            except ValueError:
                pass
            else:
                raise ValueError('native scene regression escaped: '+before)
    except (ValueError,OSError) as exc:
        raise SystemExit('ENHANCED_LEVEL22_REJECTED: '+str(exc))
    print('ENHANCED_LEVEL22_SOURCE_PASS (13 regression mutations rejected)')
    return 0

if __name__=='__main__':
    raise SystemExit(main())
