// SPDX-License-Identifier: GPL-2.0-or-later
// Opt-in Stone Prophet original DOS/4G trace, DOSBox-X 2026.10.01 normal core.
// Only a copy of the original game may be run; no guest memory is modified.
#ifndef RLOFT_STONE_ORIGINAL_RUNTIME_CAPTURE_HPP
#define RLOFT_STONE_ORIGINAL_RUNTIME_CAPTURE_HPP
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace rloft_stone_capture {
class Recorder {
    static const unsigned kEventBytes = 0x514;
    static const unsigned long long kMaxRows = 50000;
    std::FILE* file_;
    unsigned long long rows_;
    bool strict_, overflow_, signed_code_, bound_, snapshot_;
    std::uint32_t code_base_, data_base_, event_offset_, prior_ip_;
    unsigned nonzero_divs_;
    unsigned char before_[kEventBytes];

    static bool signature(std::uint32_t csbase) {
        const unsigned char prologue[] = {
            0x53,0x56,0x57,0x55,0x89,0xe5,0x83,0xec,0x14,
            0x83,0x7d,0x14,0xff
        };
        for (unsigned i=0;i<sizeof(prologue);++i)
            if (mem_readb_inline(csbase+0x29c+i)!=prologue[i]) return false;
        return mem_readb_inline(csbase+0x27ca8)==0xf7 &&
               mem_readb_inline(csbase+0x27ca9)==0xf9 &&
               mem_readb_inline(csbase+0x70d)==0xe8;
    }
    void emit(const char* kind, std::uint32_t cs, std::uint32_t pc,
              std::uint32_t x=0, std::uint32_t y=0,
              std::uint32_t z=0, std::uint32_t w=0) {
        if (!file_ || overflow_) return;
        if (rows_==kMaxRows) {
            std::fputs("{\"overflow\":true,\"reason\":\"capture_limit\"}\n",file_);
            std::fflush(file_);
            overflow_=true;
            return;
        }
        if (std::fprintf(file_,
              "{\"seq\":%llu,\"kind\":\"%s\",\"cs\":%u,\"eip\":%u,"
              "\"x\":%u,\"y\":%u,\"z\":%u,\"w\":%u,\"prior_eip\":%u}\n",
              rows_,kind,cs,pc,x,y,z,w,prior_ip_)<0 ||
            std::fflush(file_)!=0) {
            std::fclose(file_);
            file_=NULL;
            overflow_=true;
            return;
        }
        ++rows_;
    }
    void scan(std::uint32_t cs, std::uint32_t pc) {
        if (!bound_ || !file_) return;
        unsigned char bytes[kEventBytes];
        for (unsigned i=0;i<kEventBytes;++i)
            bytes[i]=mem_readb_inline(data_base_+event_offset_+i);
        unsigned spawn=0, target=0, malformed=0;
        for (unsigned slot=0;slot<50;++slot) {
            unsigned pos=slot*26, count=bytes[pos];
            if (count>5) {++malformed;continue;}
            for (unsigned i=0;i<count;++i) {
                unsigned at=pos+1+i*5;
                if (bytes[at]!=13) continue;
                ++spawn;
                const unsigned id=bytes[at+3];
                if (id==4 || id==24 || id==27) ++target;
            }
        }
        if (malformed) {
            emit("invalid_event_buffer",cs,pc,malformed,0,0,0);
            return; // Do not certify a malformed or uninitialized snapshot.
        }
        if (!snapshot_) {
            std::memcpy(before_,bytes,kEventBytes);
            snapshot_=true;
            emit("event_baseline",cs,pc,spawn,target,event_offset_,data_base_);
            if (target) emit("affected_event_commands_present",cs,pc,target,spawn);
            return;
        }
        unsigned changed=0, first=0;
        for (unsigned i=0;i<kEventBytes;++i) {
            if (bytes[i]!=before_[i]) {
                if (!changed) first=i;
                ++changed;
            }
        }
        if (changed) {
            emit("event_buffer_changed",cs,pc,first,
                 before_[first],bytes[first],changed);
            emit("event_buffer_opcode13_count",cs,pc,spawn,target,0,0);
            if (target) emit("affected_event_commands_present",cs,pc,target,spawn);
            std::memcpy(before_,bytes,kEventBytes);
        }
    }
public:
    Recorder():file_(NULL),rows_(0),strict_(false),overflow_(false),
      signed_code_(false),bound_(false),snapshot_(false),
      code_base_(0xffffffffu),data_base_(0),event_offset_(0),
      prior_ip_(0xffffffffu),nonzero_divs_(0) {
        const char* path=std::getenv("RLOFT_STONE_ORIGINAL_TRACE_JSONL");
        if (!path || !*path) return;
        strict_=std::getenv("RLOFT_STONE_TRACE_EVERY_INSTRUCTION") &&
                std::strcmp(std::getenv("RLOFT_STONE_TRACE_EVERY_INSTRUCTION"),"1")==0;
        file_=std::fopen(path,"wb");
        if (!file_) {
            std::fputs("Stone capture: cannot open private trace path\n",stderr);
            return;
        }
        std::fputs(
          "{\"hook\":\"dosboxx-stone-original-v1\","
          "\"upstream_core_blob\":\"e582cc06f38809f2ee8ee8d6f1922f3eba5b6ab5\","
          "\"stone_code2_sha256\":\"00e225ec8f141a8fdb72ff0547fe93b2ea3aabdb4ec3b59d9261a5268de9203e\","
          "\"core\":\"normal\",\"captures_guest_memory\":false,"
          "\"note\":\"raw observations, not proof of a completed DOS gameplay session\"}\n",
          file_);
        std::fflush(file_);
    }
    ~Recorder(){if(file_){std::fflush(file_);std::fclose(file_);}}
    void tick(std::uint32_t cs, std::uint32_t csbase,
              std::uint32_t dsbase, std::uint32_t ssbase,
              std::uint32_t pc, std::uint32_t eax,
              std::uint32_t esi, std::uint32_t ebx,
              std::uint32_t edi, std::uint32_t ecx,
              std::uint32_t edx, std::uint32_t esp) {
        if (!file_ || overflow_) return;
        const bool checkpoint = pc==0x2ba || pc==0x2462e ||
          pc==0x24633 || pc==0x29c || pc==0x1e1e4 ||
          pc==0x10 || pc==0x1364c || pc==0x70d ||
          pc==0x23964 || pc==0x23af1 || pc==0x27ca8;
        if (csbase!=code_base_) {
            if (!checkpoint || !signature(csbase)) return;
            code_base_=csbase;
            signed_code_=true;
            bound_=false;
            snapshot_=false;
            emit("code2_signature_verified",cs,pc,csbase,dsbase);
        }
        if (!signed_code_) return;
        if (bound_ && data_base_!=dsbase) {
            bound_=false;
            snapshot_=false;
            emit("data_segment_changed",cs,pc,dsbase);
        }
        if (pc==0x2462e || pc==0x2ba) {
            // eax is the resource manager's returned DS-relative pointer.
            if (eax>=0x1000 && eax<0x10000000u) {
                if (!bound_ || data_base_!=dsbase || event_offset_!=eax) {
                    event_offset_=eax;
                    data_base_=dsbase;
                    bound_=true;
                    snapshot_=false;
                    emit("event_buffer_bound",cs,pc,eax,dsbase);
                }
                if (pc==0x2ba) scan(cs,pc);
            }
        }
        // Opt-in, potentially slow, per-instruction mutation detection.
        // Attribution is only a hint: other CPU cores / interrupts are not traced.
        if (bound_ && (strict_ || pc==0x24633 ||
                        pc==0x1e1e4 || pc==0x29c)) scan(cs,pc);
        if (pc==0x70d) {
            const unsigned char id=mem_readb_inline(dsbase+esi+4);
            emit("event_spawn_attempt",cs,pc,id,
                 mem_readb_inline(dsbase+esi+2),
                 mem_readb_inline(dsbase+esi+3));
        } else if (pc==0x10) {
            emit("constructor_entry",cs,pc,
                 mem_readd_inline(ssbase+esp+4));
        } else if (pc==0x1364c) {
            emit("alternate_constructor_entry",cs,pc,ebx,edi,esi);
        } else if (pc==0x23964) {
            emit("authored_live_type",cs,pc,eax&0xffffu,esi);
        } else if (pc==0x23af1) {
            emit("restored_live_type",cs,pc,
                 mem_readw_inline(dsbase+0x44ff4+edi),ebx);
        } else if (pc==0x27ca8) {
            if (ecx==0) emit("idiv_zero_before_execution",cs,pc,ecx,
                 mem_readw_inline(dsbase+ebx+4),eax,edx);
            else if (nonzero_divs_++<6)
                emit("idiv_nonzero_sample",cs,pc,ecx,
                     mem_readw_inline(dsbase+ebx+4),eax,edx);
        }
        prior_ip_=pc;
    }
};
static inline void tick(std::uint32_t cs, std::uint32_t csbase,
    std::uint32_t dsbase, std::uint32_t ssbase, std::uint32_t pc,
    std::uint32_t eax, std::uint32_t esi, std::uint32_t ebx,
    std::uint32_t edi, std::uint32_t ecx, std::uint32_t edx,
    std::uint32_t esp) {
    static Recorder instance;
    instance.tick(cs,csbase,dsbase,ssbase,pc,eax,esi,ebx,edi,ecx,edx,esp);
}
} // namespace rloft_stone_capture
#endif
