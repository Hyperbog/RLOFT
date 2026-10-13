#include "rloft_native/native_scene_view.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

namespace rloft_native {
namespace {
void require(bool okay, const char* message) {
    if(!okay)throw std::runtime_error(message);
}
constexpr double kNearZero=1e-12;
constexpr double kPi=3.14159265358979323846;
int coordinate(double position) {
    require(std::isfinite(position) && position>=0.0 &&
            position<double(std::numeric_limits<int>::max()-1),
            "native probe coordinate outside safe source range");
    return static_cast<int>(std::floor(position));
}
std::pair<int,int> step_for(NativeCardinal d) {
    switch(d) {
        case NativeCardinal::North:return {0,-1};
        case NativeCardinal::East:return {1,0};
        case NativeCardinal::South:return {0,1};
        case NativeCardinal::West:return {-1,0};
    }
    throw std::runtime_error("unsupported native scene cardinal direction");
}
bool in_map(const NativeMutableLevel& m,int x,int y) {
    return x>=0 && y>=0 && static_cast<unsigned>(x)<m.width &&
           static_cast<unsigned>(y)<m.height;
}
void camera_valid(const NativeWorldRuntime& world,const NativeProbeCamera& pose) {
    const auto& m=world.maps().levels.at(pose.level);
    require(m.id==pose.level && std::isfinite(pose.heading_radians) &&
            std::isfinite(pose.pitch_radians) &&
            std::abs(pose.pitch_radians)<=0.9 &&
            std::isfinite(pose.field_of_view_radians) &&
            pose.field_of_view_radians>0.01 &&
            pose.field_of_view_radians<kPi-0.01 &&
            std::isfinite(pose.column_position) &&
            std::isfinite(pose.row_position) &&
            pose.column_position>=0 && pose.column_position<m.width &&
            pose.row_position>=0 && pose.row_position<m.height,
            "native probe camera invalid/outside original level");
    // Host camera must start within a normally passable cell. This is not
    // a claim about original player's location/footprint algorithm.
    require(world.normal_collision(pose.level,
            static_cast<unsigned>(pose.column_position),
            static_cast<unsigned>(pose.row_position))==
            NativeCollision::Passable,
            "native probe camera begins in blocked original cell");
}
std::uint16_t add_wrap(std::uint16_t first,unsigned delta) {
    return static_cast<std::uint16_t>((unsigned(first)+delta)&0xffffU);
}
}

NativeSceneResourceCandidates native_scene_resource_candidates(
    const NativeWorldRuntime& world,unsigned level,unsigned column,
    unsigned row,std::optional<NativeCardinal> adjacent_direction) {
    auto cell=world.cell(level,column,row);
    auto code=cell.code;
    NativeSceneResourceCandidates r;
    r.level=level;r.column=column;r.row=row;
    r.original_code=code;
    const auto& level_source=world.original().level(level);
    if(cell.special_frame && adjacent_direction) {
        const auto delta=step_for(*adjacent_direction);
        const auto nx=static_cast<int>(column)+delta.first;
        const auto ny=static_cast<int>(row)+delta.second;
        if(nx>=0 && ny>=0 && static_cast<unsigned>(nx)<level_source.width &&
           static_cast<unsigned>(ny)<level_source.height) {
            const auto neighbor=world.cell(level,static_cast<unsigned>(nx),
                                           static_cast<unsigned>(ny));
            if(neighbor.special_center) {
                code=neighbor.code;
                r.direction_neighbor_substituted=true;
                r.selected_variant_offset=1;
            }
        }
    }
    if(!r.direction_neighbor_substituted &&
       cell.resource_variant_offset>=0) {
        r.state_record_found=true;
        r.selected_variant_offset=cell.resource_variant_offset;
    }
    r.effective_code=code;
    for(unsigned bank=0;bank<5;++bank)
        r.raw_lookup[bank]=level_source.resource_lookup(bank,code);
    for(unsigned bank=0;bank<4;++bank) {
        r.candidate_resource_ids[bank]=r.selected_variant_offset>=0
          ?add_wrap(r.raw_lookup[bank],
                    static_cast<unsigned>(r.selected_variant_offset))
          :r.raw_lookup[bank];
    }
    return r;
}

NativeProbeView trace_native_probe_view(
    const NativeWorldRuntime& world,const NativeProbeCamera& pose,
    unsigned column_count,unsigned max_steps) {
    camera_valid(world,pose);
    require(column_count>=1 && column_count<=640 &&
            max_steps>=1 && max_steps<=4096,
            "native probe column/step budget out of range");
    const auto& m=world.maps().levels.at(pose.level);
    NativeProbeView view;
    view.camera=pose;
    view.columns.reserve(column_count);
    const auto plane=std::tan(pose.field_of_view_radians*0.5);
    for(unsigned col=0;col<column_count;++col) {
        NativeProbeRay ray;
        ray.column_index=col;
        const double sample=(2.0*(double(col)+0.5)/double(column_count)-1.0);
        const double ray_angle=pose.heading_radians+
                               std::atan(plane*sample);
        const double dx=std::cos(ray_angle),dy=std::sin(ray_angle);
        const double absx=std::abs(dx),absy=std::abs(dy);
        const int stepx=dx<0?-1:1,stepy=dy<0?-1:1;
        int x=coordinate(pose.column_position);
        int y=coordinate(pose.row_position);
        const double tdelta_x=absx<kNearZero?
            std::numeric_limits<double>::infinity():1.0/absx;
        const double tdelta_y=absy<kNearZero?
            std::numeric_limits<double>::infinity():1.0/absy;
        double sidex=(dx<0?
                      pose.column_position-double(x):
                      double(x+1)-pose.column_position)*tdelta_x;
        double sidey=(dy<0?
                      pose.row_position-double(y):
                      double(y+1)-pose.row_position)*tdelta_y;
        for(unsigned n=0;n<max_steps;++n) {
            double distance{};
            if(sidex<=sidey) {
                distance=sidex;sidex+=tdelta_x;x+=stepx;ray.side=0;
            } else {
                distance=sidey;sidey+=tdelta_y;y+=stepy;ray.side=1;
            }
            if(!in_map(m,x,y)) {
                ray.left_archive=true;
                ++view.rays_that_left_archive;
                break;
            }
            const auto cx=static_cast<unsigned>(x);
            const auto cy=static_cast<unsigned>(y);
            ray.reached.emplace_back(cx,cy);
            const auto c=world.cell(pose.level,cx,cy);
            if(c.normal_collision==NativeCollision::Passable)
                continue;
            ray.hit=true;
            ray.hit_column=cx;
            ray.hit_row=cy;
            ray.hit_code=c.code;
            ray.hit_collision=c.normal_collision;
            ray.ray_distance=distance;
            ray.corrected_distance=std::max(
                kNearZero,distance*std::cos(ray_angle-pose.heading_radians));
            ++view.rays_that_hit;
            break;
        }
        view.walked_cells+=ray.reached.size();
        view.columns.push_back(std::move(ray));
    }
    return view;
}

void apply_native_probe_visibility(NativeWorldRuntime& world,
                                   const NativeProbeView& view) {
    camera_valid(world,view.camera);
    require(view.columns.size()<=640,"probe view exceeds column budget");
    const auto x=static_cast<unsigned>(view.camera.column_position);
    const auto y=static_cast<unsigned>(view.camera.row_position);
    world.mark_reached_by_view(view.camera.level,x,y);
    for(const auto& ray:view.columns) {
        require(ray.reached.size()<=4096,"probe visibility step overflow");
        for(const auto& position:ray.reached)
            world.mark_reached_by_view(view.camera.level,
                                      position.first,position.second);
    }
}

bool move_native_probe_camera(const NativeWorldRuntime& world,
                              NativeProbeCamera& camera,
                              double delta_columns,double delta_rows) {
    camera_valid(world,camera);
    require(std::isfinite(delta_columns) && std::isfinite(delta_rows),
            "native probe movement delta not finite");
    const auto magnitude=std::hypot(delta_columns,delta_rows);
    require(magnitude<=4.0,"native host probe step exceeds bounded length");
    if(magnitude==0)return true;
    const auto steps=static_cast<unsigned>(std::ceil(magnitude/0.2));
    require(steps>0 && steps<=32,"native host movement subdivision invalid");
    auto candidate=camera;
    for(unsigned n=1;n<=steps;++n) {
        candidate.column_position=camera.column_position+
                                  delta_columns*double(n)/double(steps);
        candidate.row_position=camera.row_position+
                               delta_rows*double(n)/double(steps);
        const auto& m=world.maps().levels.at(camera.level);
        if(candidate.column_position<0 || candidate.row_position<0 ||
           candidate.column_position>=m.width ||
           candidate.row_position>=m.height)
            return false;
        const auto x=static_cast<unsigned>(candidate.column_position);
        const auto y=static_cast<unsigned>(candidate.row_position);
        if(world.normal_collision(camera.level,x,y)!=
           NativeCollision::Passable)return false;
        // Axis-adjacent swept checks disallow diagonal wall-corner leakage;
        // original player footprint and slide behavior are still unknown.
        const auto axis_x=static_cast<unsigned>(candidate.column_position);
        const auto axis_y=static_cast<unsigned>(camera.row_position+
                                      delta_rows*double(n-1)/double(steps));
        const auto previous_x=static_cast<unsigned>(camera.column_position+
                                      delta_columns*double(n-1)/double(steps));
        if(world.normal_collision(camera.level,axis_x,axis_y)!=
                NativeCollision::Passable ||
           world.normal_collision(camera.level,previous_x,y)!=
                NativeCollision::Passable)return false;
    }
    camera.column_position+=delta_columns;
    camera.row_position+=delta_rows;
    return true;
}

namespace {
bool disk_passable(const NativeWorldRuntime& world,unsigned level,
                   double x,double y,double radius) {
    const auto& m=world.maps().levels.at(level);
    if(x-radius<0.0 || y-radius<0.0 ||
       x+radius>=double(m.width) || y+radius>=double(m.height))
        return false;
    const auto x0=static_cast<int>(std::floor(x-radius));
    const auto x1=static_cast<int>(std::floor(x+radius));
    const auto y0=static_cast<int>(std::floor(y-radius));
    const auto y1=static_cast<int>(std::floor(y+radius));
    for(int cy=y0;cy<=y1;++cy)for(int cx=x0;cx<=x1;++cx) {
        const double nearest_x=std::clamp(x,double(cx),double(cx+1));
        const double nearest_y=std::clamp(y,double(cy),double(cy+1));
        const double dx=x-nearest_x,dy=y-nearest_y;
        if(dx*dx+dy*dy>=radius*radius)continue;
        if(world.normal_collision(level,unsigned(cx),unsigned(cy))!=
                NativeCollision::Passable)
            return false;
    }
    return true;
}
}
NativeProbeMoveResult move_native_probe_camera_clearance(
    const NativeWorldRuntime& world,NativeProbeCamera& camera,
    double delta_columns,double delta_rows,double clearance) {
    camera_valid(world,camera);
    require(std::isfinite(delta_columns) && std::isfinite(delta_rows) &&
            std::isfinite(clearance) && clearance>=0.15 &&
            clearance<=0.4,"invalid native preview clearance step");
    const double distance=std::hypot(delta_columns,delta_rows);
    require(distance<=4.0,"native preview clearance step too long");
    NativeProbeMoveResult result{};
    if(distance==0)return result;
    const auto count=std::max(1U,static_cast<unsigned>(std::ceil(distance/0.04)));
    const double dx=delta_columns/double(count),dy=delta_rows/double(count);
    const auto start_x=camera.column_position,start_y=camera.row_position;
    for(unsigned i=0;i<count;++i) {
        const bool xy=disk_passable(world,camera.level,
            camera.column_position+dx,camera.row_position+dy,clearance);
        if(xy) {
            camera.column_position+=dx;camera.row_position+=dy;
            continue;
        }
        result.blocked=true;
        bool accepted=false;
        if(dx!=0.0 && disk_passable(world,camera.level,
                camera.column_position+dx,camera.row_position,clearance)) {
            camera.column_position+=dx;accepted=true;
        }
        if(dy!=0.0 && disk_passable(world,camera.level,
                camera.column_position,camera.row_position+dy,clearance)) {
            camera.row_position+=dy;accepted=true;
        }
        if(!accepted)break;
    }
    result.applied_columns=camera.column_position-start_x;
    result.applied_rows=camera.row_position-start_y;
    result.moved=std::abs(result.applied_columns)>1e-9 ||
                 std::abs(result.applied_rows)>1e-9;
    return result;
}

std::vector<std::uint8_t> render_native_probe_rgb(
    const NativeProbeView& view,unsigned width,unsigned height) {
    require(width>=1 && width<=640 && height>=1 && height<=480 &&
            view.columns.size()==width,
            "probe RGB output dimensions don't match bounded ray count");
    std::vector<std::uint8_t> image(std::size_t(width)*height*3U,0);
    for(unsigned x=0;x<width;++x) {
        const auto& ray=view.columns[x];
        int wall_height=0;
        if(ray.hit)wall_height=std::min(
            static_cast<int>(height*2U),
            static_cast<int>(double(height)/
                  std::max(0.05,ray.corrected_distance)));
        const auto top=static_cast<int>(height)/2-wall_height/2;
        const auto bottom=top+wall_height;
        const auto cell_code=static_cast<unsigned>(ray.hit_code);
        // Diagnostic artificial colors ONLY; not the original DAC/palette.
        const auto base=static_cast<int>(
            68U+(cell_code*17U)%158U);
        const auto fade=ray.hit?std::max(35,256-
                        static_cast<int>(ray.corrected_distance*21.0)):0;
        for(unsigned y=0;y<height;++y) {
            std::uint8_t r{},g{},b{};
            if(ray.hit && static_cast<int>(y)>=top &&
               static_cast<int>(y)<bottom) {
                const auto luminance=std::clamp(
                    base*fade/(ray.side?320:256),0,255);
                r=static_cast<std::uint8_t>(luminance);
                g=static_cast<std::uint8_t>(luminance*4/5);
                b=static_cast<std::uint8_t>(luminance*3/5);
            } else if(y<height/2U) {
                r=19;g=25;b=42;
            } else {
                r=40;g=34;b=35;
            }
            const auto offset=(std::size_t(y)*width+x)*3U;
            image[offset]=r;image[offset+1]=g;image[offset+2]=b;
        }
    }
    return image;
}
} // namespace rloft_native
