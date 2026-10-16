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
        'frame.world_scene_records);',
        'gpu_scene_->paint(render_rect(),width,height,bgra_,',
        'host.set_gpu_world_visible(false);',
        'if(!dev_trace)host.begin_gpu_scene(',
        'gpu_filter_mode_=(gpu_filter_mode_+1U)%3U;',
        'gpu_world_enabled_=!gpu_world_enabled_;',
        'gpu_pitch_=std::clamp(',
        'host.end_gpu_scene();',
    ):
        if item not in host:
            raise ValueError('main game not wired to live GPU: '+item)
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
        'if(draw_world && mesh_initialized_ && !world_failed_)',
        'glTexSubImage2D(',
    ):
        if item not in gpu:
            raise ValueError('native GPU graphics path lost: '+item)
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
        'add_test(NAME enhanced_game_gpu_source_contract_tests',
    ):
        if item not in cmake:
            raise ValueError('GPU game build contract omitted: '+item)
    for item in (
        'enhanced_game_gpu_source_contract_tests',
        'enhanced_gpu_world_mesh_tests',
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
        (0,'frame.world_scene_records);','{});'),
        (0,'gpu_scene_->paint(render_rect(),width,height,bgra_,','/* GPU draw missing */('),
        (0,'gpu_pitch_=std::clamp(','gpu_pitch_=0.0; //'),
        (1,'EnhancedGpuWorldMeshModel::build(session.level)','EnhancedGpuWorldMesh{}'),
        (1,'assets_.res3_type7_native_raster(','/* no sprites */ ('),
        (1,'gluBuild2DMipmaps(','/* no authentic GPU texture */('),
        (1,'glScissor(left,ch-bottom,right-left,bottom-top);','/* no clipped world */'),
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
    print('ENHANCED_GAME_GPU_SOURCE_PASS (11 negative mutations rejected)')

if __name__=='__main__':
    main()
