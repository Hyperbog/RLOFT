#include "games/ravenloft/game/OpeningDemoFlow.h"
namespace ravenloft::game {
OpeningDemoFlow::OpeningDemoFlow(const std::filesystem::path&r,const std::filesystem::path&c)
    :text_(r/"T"),game_(r),cinematic_(std::make_unique<CinematicSession>(c/"CINE00.ANM",text_)){}
void OpeningDemoFlow::advanceCinematicFrame(){
    if(phase_!=Phase::OpeningCinematic)return;
    cinematic_->step();
    if(cinematic_->finished())phase_=Phase::ForgottenRealmsForest;
}
void OpeningDemoFlow::skipOpeningCinematic(){if(phase_==Phase::OpeningCinematic)phase_=Phase::ForgottenRealmsForest;}
MistTransitionResult OpeningDemoFlow::acquireGroundObject(std::uint16_t id){
    auto r=game_.acquireGroundObject(id); if(r.triggered)phase_=Phase::MistTransfer; return r;
}
void OpeningDemoFlow::tickMist(){
    if(phase_!=Phase::MistTransfer)return;
    if(game_.tickMistTransition())phase_=Phase::DemoComplete;
}
} // namespace ravenloft::game
