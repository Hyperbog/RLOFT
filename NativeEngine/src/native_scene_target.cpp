#include "rloft_native/native_scene_target.hpp"
#include <cmath>
#include <cstddef>
#include <stdexcept>

namespace rloft_native {
namespace {
constexpr double kPi=3.1415926535897932384626433832795;
void require(bool ok,const char* message) {
    if(!ok)throw std::invalid_argument(message);
}
bool line_clear(const NativeWorldRuntime& world,unsigned level,
                double from_x,double from_y,double to_x,double to_y,
                double distance) {
    // Samples at <= 0.10 map cell spacing. For every segment before
    // the target's own cell, CODE.2 ordinary collision must be clear.
    // No invented special-case interaction / original DOS raycast claim.
    const int n=static_cast<int>(std::ceil(distance*10.0));
    const auto& map=world.original().level(level);
    const unsigned target_x=static_cast<unsigned>(to_x);
    const unsigned target_y=static_cast<unsigned>(to_y);
    for(int i=1;i<n;++i) {
        const double alpha=double(i)/double(n);
        const double x=from_x+(to_x-from_x)*alpha;
        const double y=from_y+(to_y-from_y)*alpha;
        if(x<0||y<0||x>=map.width||y>=map.height)return false;
        const auto cell_x=static_cast<unsigned>(x);
        const auto cell_y=static_cast<unsigned>(y);
        if(cell_x==target_x&&cell_y==target_y)continue;
        if(world.normal_collision(level,cell_x,cell_y)!=
           NativeCollision::Passable)return false;
    }
    return true;
}
} // namespace

std::optional<NativeSceneTarget> select_native_authored_scene_target(
    const NativeWorldRuntime& world,unsigned level,
    const std::vector<NativeAuthoredSceneInstance>& candidates,
    double camera_x,double camera_y,double heading,
    double radius,double half_cone) {
    require(level<32U && candidates.size()<=32768U,
            "native authored target scope or candidate budget invalid");
    const auto& map=world.original().level(level);
    require(std::isfinite(camera_x)&&std::isfinite(camera_y)&&
            std::isfinite(heading)&&std::isfinite(radius)&&
            std::isfinite(half_cone)&&
            camera_x>=0&&camera_y>=0&&
            camera_x<double(map.width)&&camera_y<double(map.height)&&
            radius>0.0&&radius<=8.0&&
            half_cone>0.0&&half_cone<=kPi/2.0,
            "native target camera, radius or angular cone invalid");
    std::optional<NativeSceneTarget> selected;
    double best_error=0.0,best_distance=0.0;
    for(const auto& item:candidates) {
        require(item.level==level &&
                item.cell_column<map.width&&item.cell_row<map.height,
                "native targeting candidate not bound to active source map");
        const auto x=double(item.x_position_16_16)/65536.0;
        const auto y=double(item.y_position_16_16)/65536.0;
        require(std::isfinite(x)&&std::isfinite(y)&&x>=0&&y>=0&&
                x<map.width&&y<map.height&&
                static_cast<unsigned>(x)==item.cell_column&&
                static_cast<unsigned>(y)==item.cell_row,
                "native targeting fixed-point source coordinates are malformed");
        const auto dx=x-camera_x,dy=y-camera_y;
        const auto distance=std::hypot(dx,dy);
        if(distance<0.05||distance>radius)continue;
        const auto error=std::abs(std::remainder(
            std::atan2(dy,dx)-heading,2*kPi));
        if(error>half_cone)continue;
        if(!line_clear(world,level,camera_x,camera_y,x,y,distance))continue;
        if(!selected||error<best_error-1e-10||
           (std::abs(error-best_error)<=1e-10&&distance<best_distance)) {
            selected=NativeSceneTarget{item,distance,error};
            best_error=error;best_distance=distance;
        }
    }
    return selected;
}
} // namespace rloft_native
