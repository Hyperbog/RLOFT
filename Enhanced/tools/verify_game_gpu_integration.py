#!/usr/bin/env python3
"""Prove that the continuous native CODE.1->CODE.2 executable owns GPU
presentation, not a resurrected standalone Level22 walkaround process.

Source-only CI contract; visual field acceptance remains independent.
"""
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]

def check(host,gpu,mesh,cmake,workflow):
    for item in (
        '#include "rloft/win32_gpu_scene.hpp"',
        'wc.style = CS_OWNDC;',
        'host.begin_gpu_scene(assets,exploration_palette);',
        'host.bind_gpu_frame(runtime.session(),runtime.world(),',
        'presented.environment_backdrop_indices,',
        'presented.palette);',
        'gpu_scene_->paint(render_rect(),width,height,bgra_,',
        'host.set_gpu_world_visible(false);',
        'if(!dev_trace)host.begin_gpu_scene(',
        # The same modern presentation target owns original logos, movies,
        # title, character creation and gameplay. Optional 3D starts OFF.
        'host.begin_gpu_scene(assets,presenter.palette12());',
        'bool gpu_world_enabled_{false};',
        'unsigned gpu_filter_mode_{0U};',
        'options.resume_existing_party &&',
        'source.set_runtime_overlay(root);',
        # The exact four original title actions remain operable.
        'if (*click == 0) {',
        'if (*click == 1) {',
        'if (*click == rloft::BootFlow::kCharacterWorkflowMenuIndex) {',
        'if (*click == 3) {',
        # Native progress is restored independently of original SAVE.DAT.
        'write_native_save(runtime,assets.source());',
        '#include "rloft/title_keyboard.hpp"',
        'rloft::TitleKeyboardModel::key_down(',
        'void ensure_gdi_backbuffer(',
        'if(gdi_dc_)BitBlt(',
        'refresh_world_palette();',
        'if (wparam==VK_F11 &&',
        'read_native_save(assets.source());',
        'runtime.restore_native_checkpoint(',
        'gpu_filter_mode_=(gpu_filter_mode_+1U)%3U;',
        'gpu_world_enabled_=!gpu_world_enabled_;',
        'gpu_pitch_=std::clamp(',
        'host.end_gpu_scene();',
    ):
        if item not in host:
            raise ValueError('main game not wired to live GPU: '+item)
    # The original palette must be refreshed before EVERY normal
    # host.pump exploration iteration. Merely finding a palette refresh
    # in an unrelated level-transition branch is insufficient.
    if 'while (host.pump()) {\n        refresh_world_palette();' not in host:
        raise ValueError('main exploration world-palette refresh removed')
    # Both the transition-time DAC update and the normal exploration
    # attachment are mandatory. A mutation deleting only one call must
    # still fail even though the other retains the same text signature.
    if host.count('host.begin_gpu_scene(assets,exploration_palette);')!=2:
        raise ValueError('palette-refresh and exploration GPU binds diverged')
    for item in (
        'EnhancedGpuWorldMeshModel::build(session.level)',
        'session_=&session;world_=&world;records_=records;',
        'raster.mask[at]==0U',
        'assets_.res3_type7_native_raster(',
        'const auto decoded=res4_.texture(',
        'gluBuild2DMipmaps(',
        'glScissor(left,ch-bottom,right-left,bottom-top);',
        'glVertexPointer(',
        'glDrawArrays(GL_TRIANGLES',
        'gluLookAt(x,y,1.',
        'static_cast<double>(session_->facing)*pi/180.0',
        'SwapBuffers(dc_);',
        'if(gpu_frame) {',
        'EnhancedGpuBackdrop::compose(',
        'environment_backdrop_=environment_backdrop;',
        'set_palette(frame_palette);',
        'EnhancedGpuBackdrop::restore(bgra,canvas_pixels_);',
        'glClear(GL_DEPTH_BUFFER_BIT);',
        'glTexSubImage2D(',
        'EnhancedGpuCameraModel::billboard_world_width(',
        'sprite_width_.at(record.resource_id)',
        'sprite_width_.clear();',
        'EnhancedGpuCameraModel::all_gpu_sprites_failed(',
        '++rendered_gpu_sprites;',
        'draw_live_canvas(); // restore source canvas if optional 3D fails',
        'glDisableClientState(GL_VERTEX_ARRAY);',
    ):
        if item not in gpu:
            raise ValueError('native GPU graphics path lost: '+item)
    # The first viewport-scissor call protects the 256px world render;
    # the second protects the source-canvas fallback after GL failure.
    # A naive single-string guard missed deletion of one of these.
    if gpu.count('glScissor(left,ch-bottom,right-left,bottom-top);')!=2:
        raise ValueError('GPU viewport or fallback scissor missing')
    for item in (
        'const LevelState& l',
        'const auto word=l.cell_words[',
        'const auto code=static_cast<unsigned>(word&0x003FU);',
        'const unsigned id=resource(l,bank,code);',
        'l.group2',
        'scene.floor_faces',
        'scene.lower_faces',
    ):
        if item not in mesh:
            raise ValueError('GPU renderer invented a second level: '+item)
    for item in (
        'src/win32_gpu_scene.cpp',
        'Opengl32 Glu32',
        'src/enhanced_gpu_world_mesh.cpp',
        'add_executable(enhanced_gpu_world_mesh_tests',
        'add_executable(enhanced_gpu_backdrop_tests',
        'add_test(NAME enhanced_game_gpu_source_contract_tests',
    ):
        if item not in cmake:
            raise ValueError('GPU game build contract omitted: '+item)
    for item in (
        'enhanced_game_gpu_source_contract_tests',
        'enhanced_gpu_world_mesh_tests',
        'enhanced_gpu_backdrop_tests',
        'Ravenloft-Enhanced-Game-Windows-x64',
        'build/demo-v1/Release/Ravenloft.exe',
    ):
        if item not in workflow:
            raise ValueError('GPU executable not gated or published: '+item)
    if 'Explore-Level22.cmd' in workflow:
        raise ValueError('isolated Level22 preview was republished')

def main():
    paths=[
        'DemoV1/src/win32_main.cpp',
        'DemoV1/src/win32_gpu_scene.cpp',
        'DemoV1/src/enhanced_gpu_world_mesh.cpp',
        'DemoV1/CMakeLists.txt',
        '.github/workflows/demo-v1-build.yml',
    ]
    sources=[(ROOT/p).read_text(encoding='utf-8') for p in paths]
    check(*sources)
    mutations=(
        (0,'host.begin_gpu_scene(assets,exploration_palette);','/* GPU init missing */'),
        (0,'host.begin_gpu_scene(assets,presenter.palette12());','/* full game GL missing */'),
        (0,'bool gpu_world_enabled_{false};','bool gpu_world_enabled_{true};'),
        (0,'unsigned gpu_filter_mode_{0U};','unsigned gpu_filter_mode_{2U};'),
        (0,'if (*click == 1) {','if (false) {'),
        (0,'write_native_save(runtime,assets.source());','/* no Save */'),
        (0,'rloft::TitleKeyboardModel::key_down(','/* no keyboard menu */('),
        (0,'void ensure_gdi_backbuffer(','void ignored_backbuffer('),
        (0,'while (host.pump()) {\n        refresh_world_palette();',
           'while (host.pump()) {\n        /* stale level palette */'),
        (0,'if (wparam==VK_F11 &&','if (false &&'),
        (0,'options.resume_existing_party &&','true &&'),
        (0,'presented.palette);','{});'),
        (0,'gpu_scene_->paint(render_rect(),width,height,bgra_,','/* GPU draw missing */('),
        (0,'gpu_pitch_=std::clamp(','gpu_pitch_=0.0; //'),
        (1,'EnhancedGpuWorldMeshModel::build(session.level)','EnhancedGpuWorldMesh{}'),
        (1,'assets_.res3_type7_native_raster(','/* no sprites */ ('),
        (1,'gluBuild2DMipmaps(','/* no authentic GPU texture */('),
        (1,'glScissor(left,ch-bottom,right-left,bottom-top);','/* no clipped world */'),
        (1,'sprite_width_.at(record.resource_id)','0U'),
        (1,'++rendered_gpu_sprites;','/* all source sprites appear broken */'),
        (1,'draw_live_canvas(); // restore source canvas if optional 3D fails',
           '/* erased frame left blank after GPU failure */'),
        (1,'EnhancedGpuBackdrop::compose(','/* backdrop missing */('),
        (1,'EnhancedGpuBackdrop::restore(bgra,canvas_pixels_);',
           '/* software frame not restored */'),
        (1,'glClear(GL_DEPTH_BUFFER_BIT);',
           'glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);'),
        (1,'set_palette(frame_palette);','/* static stale color lookup */'),
        (2,'l.group2','l.group1'),
        (3,'Opengl32 Glu32',''),
        (4,'Ravenloft-Enhanced-Game-Windows-x64','Standalone-Preview'),
    )
    for index,old,new in mutations:
        if old not in sources[index]:
            raise ValueError('missing regression mutation target: '+old)
        altered=list(sources)
        altered[index]=altered[index].replace(old,new,1)
        try:check(*altered)
        except ValueError:continue
        raise ValueError('GPU game regression escaped: '+old)
    print('ENHANCED_GAME_GPU_SOURCE_PASS ('+str(len(mutations))+
          ' negative mutations rejected; original display default protected)')

if __name__=='__main__':
    main()
