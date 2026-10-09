#pragma once
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace rloft {

// Original CODE.2 21F70..222A6 camera staging: source angular branches,
// high-word cell updates, low-word 8000 recenter and draw/present per step.
// Pure source trace; this does NOT substitute for 1F1C8/1EA78 execution.
struct MistStagePose {
    std::uint32_t coordinate_a{};
    std::uint32_t coordinate_b{};
    std::uint32_t facing{};
};
enum class MistStagePhase : std::uint8_t {
    TurnColumn, MoveColumn, TurnRow, MoveRow, FinalTurn
};
struct MistStageDraw {
    MistStagePose pose{};
    MistStagePhase phase{};
    std::uint32_t draw_call_offset{};
    std::uint32_t present_call_offset{};
};
struct MistStageTrace {
    std::vector<MistStageDraw> draws;
    MistStagePose final_pose{};
};

class OriginalMistStagingModel final {
    static int toward90(int h) noexcept {
        if(h>=85 && h<=95) return 90;
        if(h>90 && h<270) return h-10;
        if(h<=80) return h+10;
        return 90;
    }
    static int toward270(int h) noexcept {
        if(h>=265 && h<=275) return 270;
        if(h>270) return h-10;
        if(h<90 || h>265) return 275;
        return h+10;
    }
    static int toward180(int h) noexcept {
        if(h>=175 && h<=185) return 180;
        if(h>180) return h-10;
        if(h>170) return 180;
        return h+10;
    }
    static int toward0(int h) noexcept {
        if(h<10 || h>355) return 0;
        return h-10;
    }
    static int toward213(int h) noexcept {
        if(h>=210 && h<=220) return 213;
        if(h>=180 && h<213) {
            const int n=h+10;
            return n>=210 ? 213 : n;
        }
        int n=h-10;
        if(n<0) n+=360;
        return n<=220 ? 213 : n;
    }
    static std::uint32_t centered(std::uint32_t cell) noexcept {
        return (cell<<16U)|0x8000U;
    }
public:
    [[nodiscard]] static MistStageTrace trace(MistStagePose initial) {
        const auto start_column=initial.coordinate_a>>16U;
        const auto start_row=initial.coordinate_b>>16U;
        if(start_column>=30U || start_row>=15U || initial.facing>=360U)
            throw std::out_of_range("21F70 original Level22 stage pose");
        MistStageTrace out{};
        auto pose=initial;
        auto add=[&](MistStagePhase phase,
                     std::uint32_t draw,std::uint32_t present) {
            out.draws.push_back({pose,phase,draw,present});
        };
        const bool left=start_column>23U;
        const auto column_heading=left?90U:270U;
        while(pose.facing!=column_heading) {
            const int h=static_cast<int>(pose.facing);
            pose.facing=static_cast<std::uint32_t>(
                left?toward90(h):toward270(h));
            add(MistStagePhase::TurnColumn,
                left?0x22012U:0x220C4U,left?0x22017U:0x220C9U);
        }
        auto column=start_column;
        pose.coordinate_a=centered(column);
        while(column!=23U) {
            column=left?column-1U:column+1U;
            pose.coordinate_a=centered(column);
            add(MistStagePhase::MoveColumn,
                left?0x2203EU:0x220EFU,left?0x22043U:0x220F4U);
        }
        const bool up=start_row>7U;
        const auto row_heading=up?180U:0U;
        while(pose.facing!=row_heading) {
            const int h=static_cast<int>(pose.facing);
            pose.facing=static_cast<std::uint32_t>(
                up?toward180(h):toward0(h));
            add(MistStagePhase::TurnRow,
                up?0x2216FU:0x221F9U,up?0x22174U:0x221FEU);
        }
        auto row=start_row;
        pose.coordinate_b=centered(row);
        while(row!=7U) {
            row=up?row-1U:row+1U;
            pose.coordinate_b=centered(row);
            add(MistStagePhase::MoveRow,
                up?0x2219BU:0x22221U,up?0x221A0U:0x22226U);
        }
        while(pose.facing!=213U) {
            pose.facing=static_cast<std::uint32_t>(
                toward213(static_cast<int>(pose.facing)));
            add(MistStagePhase::FinalTurn,0x2229AU,0x2229FU);
        }
        out.final_pose=pose;
        return out;
    }
};
} // namespace rloft
