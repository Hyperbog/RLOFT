#include "engine/render/IndexedFrameBuffer.h"
#include "games/ravenloft/data/DefinitionDatabases.h"
#include "games/ravenloft/data/InitDatabase.h"
#include "games/ravenloft/data/LevelDatabase.h"
#include "games/ravenloft/data/PaletteDatabase.h"
#include "games/ravenloft/data/ResourceStore.h"
#include "games/ravenloft/data/SaveGame.h"
#include "games/ravenloft/data/TrigTable.h"
#include "games/ravenloft/game/OpeningDemoSession.h"
#include "games/ravenloft/game/AdventureScreenRenderer.h"
#include "games/ravenloft/game/FloorPlaneRenderer.h"
#include "games/ravenloft/game/LightingSystem.h"
#include "games/ravenloft/game/WorldRenderer.h"
#include "games/ravenloft/game/SkyRenderer.h"
#include "games/ravenloft/game/ProjectionSystem.h"
#include "games/ravenloft/game/SceneProjector.h"
#include "games/ravenloft/game/SpritePassRenderer.h"
#include "games/ravenloft/game/WorldScene.h"
#include <filesystem>
#include <iostream>

using namespace ravenloft::data;
using namespace ravenloft::game;

int main(int argc,char** argv){
    try{
        if(argc<3){std::cerr<<"Usage: ravenloft_render_probe <RLOFT dir> <out.ppm> [heading|approach]\n";return 2;}
        std::filesystem::path root=argv[1], out=argv[2];
        LevelDatabase levels(root/"LEVELS.DAT");
        NpcDatabase npcs(root/"NPCS.DAT");
        InitDatabase init(root/"INIT");
        PaletteDatabase palettes(root/"COLORS");
        ResourceStore resources(root);
        SaveGame save(root/"SAVE.DAT");
        OpeningDemoSession demo(root);
        TrigTable trig(root/"SINTAB");
        ProjectionSystem projection(trig);
        SceneProjector projector(projection);
        SpritePassRenderer renderer(projector,resources);

        const auto& q=save.quickStart();
        LightingSystem lightingSystem(trig);
        const auto lighting=lightingSystem.fromQuickStart(q);
        std::int32_t cameraX=q.partyXFixed,cameraY=q.partyYFixed;
        int heading=q.facingDegrees;
        if(argc>=4 && std::string(argv[3])=="approach"){
            for(int i=0;i<8;++i)demo.moveOriginalAxes(-200,0,0);
            for(int i=0;i<18;++i)demo.moveOriginalAxes(0,-400,0);
            for(int i=0;i<11;++i)demo.moveOriginalAxes(-200,0,0);
            cameraX=demo.party().position.x;
            cameraY=demo.party().position.y;
            heading=demo.party().headingDegrees;
        } else if(argc>=4) heading=std::stoi(argv[3]);
        const auto level=levels.load(22);
        WorldScene scene(level,init,npcs,demo.assassin());
        ssi::render::IndexedFrameBuffer fb(256,256);
        WorldRenderer worldRenderer(init,resources,trig);
        const auto worldStats=worldRenderer.render(fb,scene,cameraX,cameraY,heading,lighting);
        ssi::render::IndexedFrameBuffer screen(320,200);
        AdventureScreenRenderer screenRenderer(init,resources);
        const auto screenStats=screenRenderer.renderOpening(screen,fb,22);
        const auto projected=projector.project(scene,cameraX,cameraY,heading);
        const auto queue=OriginalRenderQueue::build(projected,scene);
        for(std::size_t i=0;i<queue.size();++i){
            if(i>=12 && queue[i].category!=OriginalRenderCategory::Npc) continue;
            std::cout<<"q"<<i<<" cat="<<(queue[i].category==OriginalRenderCategory::Npc?"npc":"scenery")<<" scale="<<queue[i].scale8_8;
            for(const auto& d:queue[i].draws) std::cout<<" [res="<<d.resource<<" x="<<d.screenX<<" off="<<unsigned(d.verticalSourceOffsetRows)<<"]";
            std::cout<<"\n";
        }
        const auto& stats=worldStats.sprites;
        if(out.extension()==".bmp"||out.extension()==".BMP") screen.writeBmp(out,palettes.at(scene.paletteIndex()));
        else screen.writePpm(out,palettes.at(scene.paletteIndex()));
        std::cout<<"camera=("<<(cameraX/65536.0)<<","<<(cameraY/65536.0)<<") heading="<<heading
                 <<" sky="<<worldStats.sky.rasterizedDraws
                 <<" floorRows="<<worldStats.floor.rows
                 <<" scenery="<<stats.projectedScenery
                 <<" npcs="<<stats.projectedNpcs
                 <<" queue="<<stats.queuedRecords
                 <<" calls="<<stats.drawCalls
                 <<" rasterized="<<stats.rasterizedDraws
                 <<" skyStrips="<<worldStats.sky.backgroundStrips
                 <<" skyElements="<<worldStats.sky.panoramaElements
                 <<" hour="<<lighting.hour
                 <<" skyShade="<<unsigned(lighting.skyShade)
                 <<" floorShade0="<<unsigned(lighting.floorShade[0])
                 <<" uiPrimary="<<screenStats.primaryResource
                 <<" uiSecondary="<<screenStats.secondaryResource
                 <<" viewportPixels="<<screenStats.worldPixels<<"\n";
        return 0;
    }catch(const std::exception& e){std::cerr<<"ERROR: "<<e.what()<<"\n";return 1;}
}
