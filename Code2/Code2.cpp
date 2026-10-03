// Faithful reconstruction of Ravenloft: Strahd's Possession CODE.2.
// Immutable source: CODE.2, 558689 bytes
// SHA-256 853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314
// Object-1 SHA-256 014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3
// Instruction-accountable reconstruction; CODE.2 bytes are controlling evidence.

#include <cstddef>
#include <cstdint>

namespace code2 {

using u8  = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using s8  = std::int8_t;
using s16 = std::int16_t;
using s32 = std::int32_t;

static_assert(sizeof(u8)==1 && sizeof(u16)==2 && sizeof(u32)==4);

constexpr u32 kObject1FileStart = 0x0003AF24u;
constexpr u32 kObject1VirtualBase = 0x00010000u;
constexpr u32 kObject1Length = 0x0003A852u;
constexpr u32 kObject1EntryOffset = 0x000353A0u;
constexpr u32 kVerifiedTranslatedBytes = 4017u;
constexpr u32 kVerifiedNonCodeBytes = 0u;

constexpr u32 kObject2FileStart = 0x00075F24u;
constexpr u32 kObject2VirtualBase = 0x00050000u;
constexpr u32 kObject2VirtualSize = 0x0002DD10u;
constexpr u32 kObject2StoredBytes = 0x0001273Du;

[[nodiscard]] constexpr u16 load_le16(const u8* p) noexcept {
    return static_cast<u16>(p[0] | (static_cast<u16>(p[1]) << 8));
}
[[nodiscard]] constexpr u32 load_le32(const u8* p) noexcept {
    return static_cast<u32>(p[0] | (static_cast<u32>(p[1]) << 8) |
                            (static_cast<u32>(p[2]) << 16) | (static_cast<u32>(p[3]) << 24));
}
constexpr void store_le16(u8* p,u16 v) noexcept {
    p[0]=static_cast<u8>(v); p[1]=static_cast<u8>(v>>8);
}
constexpr void store_le32(u8* p,u32 v) noexcept {
    p[0]=static_cast<u8>(v); p[1]=static_cast<u8>(v>>8);
    p[2]=static_cast<u8>(v>>16); p[3]=static_cast<u8>(v>>24);
}

struct CpuState {
    u32 eax=0, ebx=0, ecx=0, edx=0, esi=0, edi=0, ebp=0, esp=0;
    u16 cs=0, ds=0, es=0, ss=0, fs=0, gs=0;
    bool interrupt_enable=false;
};
struct Machine { CpuState cpu{}; u8* data=nullptr; };
[[nodiscard]] u8* dg(Machine& m,u32 off) noexcept { return m.data+off; }

// -----------------------------------------------------------------------------
// 00010..00270 — first directly reconstructed application routines.
// External calls remain address-named until their CODE.2 semantics are proved.
// -----------------------------------------------------------------------------

struct EarlyBoundaries {
    u32 (*call_16524)(u32)=nullptr;
    u32 (*call_1c124)(u32)=nullptr;
    u32 (*call_1c1dc)(u32)=nullptr;
    u32 (*call_1c160)(u32,u32)=nullptr; // stack argument, incoming EBX
    u32 (*call_1f1c8)()=nullptr;
    u32 (*call_27c70)(u32,u32)=nullptr;
    s32 (*call_14a60)(u32)=nullptr;
};

// 00010..00072: locate the first 22-byte record with type 54h and matching
// signed word keys. On match, clear word bits 01C0h and then set byte bit 40h.
[[nodiscard]] u32 function_00010(Machine& m,s32 key_at_2223e,s32 key_at_2223a) noexcept {
    u32 record_off=0;
    s32 index=0;
    for (;;) {
        const s32 count=static_cast<s16>(load_le16(dg(m,0x258A6u)));
        if(index>=count) return record_off;
        if(load_le16(dg(m,0x22246u+record_off))==0x54u &&
           static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x2223Eu+record_off))))==key_at_2223e &&
           static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x2223Au+record_off))))==key_at_2223a) {
            const u32 a=0x2224Cu+record_off;
            store_le16(dg(m,a),static_cast<u16>(load_le16(dg(m,a)) & 0xFE3Fu));
            *dg(m,a)=static_cast<u8>(*dg(m,a) | 0x40u);
            return record_off;
        }
        record_off+=0x16u;
        ++index;
    }
}
static_assert(0x00073u-0x00010u==99u);

// 00074..000B7: clear bit 1 in byte +14D8B for every type-8 record, with
// the signed record count reloaded on each loop test exactly as encoded.
[[nodiscard]] u32 function_00074(Machine& m,EarlyBoundaries& b) noexcept {
    if(b.call_16524) (void)b.call_16524(0x1Cu);
    if(b.call_1c124) (void)b.call_1c124(0x3Au);
    s32 index=0; u32 record_off=0;
    for (;;) {
        const s32 count=static_cast<s16>(load_le16(dg(m,0x258A2u)));
        if(index>=count) break;
        if(load_le16(dg(m,0x14D8Eu+record_off))==8u)
            *dg(m,0x14D8Bu+record_off)=static_cast<u8>(*dg(m,0x14D8Bu+record_off)&0xFDu);
        record_off+=0x16u;
        ++index;
    }
    return b.call_1f1c8 ? b.call_1f1c8() : 0u;
}
static_assert(0x000B8u-0x00074u==68u);

// 000B8..001F9: mode-3D/20h/3Ch slot-selection state machine. The fallback
// scan deliberately allows ESI==4 to flow into the final selection path.
[[nodiscard]] u32 function_000b8(Machine& m,EarlyBoundaries& b) noexcept {
    u32 eax=b.call_1c1dc?b.call_1c1dc(0x3Du):0u;
    if(eax==1u) return eax;
    eax=b.call_1c1dc?b.call_1c1dc(0x20u):0u;
    u32 esi=0;
    if(eax!=0u) {
        if(b.call_16524) (void)b.call_16524(9u);
        if(b.call_1c124) (void)b.call_1c124(0x3Du);
        if(b.call_1c160) (void)b.call_1c160(0x3Cu,0u);
        u32 record_off=0;
        for(u32 ebx=1;;++ebx) {
            record_off+=0x248u;
            *dg(m,0xCF59u+ebx)=1u;
            const u32 a=0x216A8u+record_off;
            store_le16(dg(m,a),static_cast<u16>(load_le16(dg(m,a))&0xFEBFu));
            if(ebx>=4u) break;
        }
        return b.call_1c124 ? b.call_1c124(0x3Cu) : record_off;
    }

    eax=b.call_1c1dc?b.call_1c1dc(0x3Cu):0u;
    if(eax!=0u) return eax;

    u32 record_off=0;
    esi=0;
    for (;;) {
        const u16 flags=load_le16(dg(m,0x218F0u+record_off));
        if((flags&0xE000u)==0u && (flags&0x0080u)==0u) break;
        record_off+=0x248u;
        ++esi;
        if(static_cast<s32>(record_off)>=static_cast<s32>(0x920u)) break;
    }

    if(esi==4u) {
        esi=0;
        u32 ebx=0;
        for (;;) {
            const u16 flags=load_le16(dg(m,0x218F0u+ebx));
            bool choose=false;
            if((flags&0xE000u)==0u) {
                const u32 p=0x218E0u+ebx;
                const u32 r4=b.call_27c70?b.call_27c70(p,4u):0u;
                if(r4!=0u) {
                    const u32 r16=b.call_27c70?b.call_27c70(p,0x10u):0u;
                    if(r16!=0u) {
                        const u32 r28=b.call_27c70?b.call_27c70(p,0x1Cu):0u;
                        choose=(r28!=0u);
                    }
                }
            }
            if(choose) break;
            ebx+=0x248u;
            ++esi;
            if(static_cast<s32>(ebx)>=static_cast<s32>(0x920u)) break;
        }
    }

    if(b.call_16524) (void)b.call_16524(9u);
    const u32 selected_off=esi*0x248u;
    *dg(m,0xCF5Au+esi)=1u;
    *dg(m,0x218F1u+selected_off)=static_cast<u8>(*dg(m,0x218F1u+selected_off)|1u);
    return b.call_1c124 ? b.call_1c124(0x3Cu) : selected_off;
}
static_assert(0x001FAu-0x000B8u==322u);

// 001FC..00270: gate resource 46h, allocate/select one 248h-byte slot, reject
// flags 5000h, clear two bits in the indexed 1994C table byte, and publish code 33h.
[[nodiscard]] u32 function_001fc(Machine& m,EarlyBoundaries& b) noexcept {
    if((b.call_1c1dc?b.call_1c1dc(0x46u):0u)!=0u) return 0u;
    const s32 slot=b.call_14a60?b.call_14a60(4u):-1;
    if(slot==-1) { store_le32(dg(m,0xBF44u),0x4Au); return 0u; }
    const u32 record_off=static_cast<u32>(slot)*0x248u;
    if((load_le16(dg(m,0x218F0u+record_off))&0x5000u)!=0u) return 0u;
    const s32 index=static_cast<s16>(load_le16(dg(m,0x25619u)));
    const u32 byte_addr=static_cast<u32>(0x1994Cu + static_cast<s32>(index*0x24));
    *dg(m,byte_addr)=static_cast<u8>(*dg(m,byte_addr)&0xC0u);
    if(b.call_1c124) (void)b.call_1c124(0x46u);
    store_le32(dg(m,0xBF44u),0x33u);
    return 1u;
}
static_assert(0x00271u-0x001FCu==117u);

// -----------------------------------------------------------------------------
// 35F88..369A6 — Watcom timer/event subsystem. This region is structurally the
// same audited runtime family as CODE.1 E3B4..EDD2, relocated by +27BD4h, with
// its DGROUP state block shifted from 510Ch..52E8h to 10270h..1044Ch.
// Every address below was rechecked against fresh CODE.2 disassembly.
// -----------------------------------------------------------------------------

struct VectorAddress { u32 offset=0; u16 selector=0; };
struct LowLevelBoundaries {
    u8 (*in8)(u16)=nullptr;
    void (*out8)(u16,u8)=nullptr;
    void (*set_vector_legacy)(u8,u16,u32)=nullptr;
    void (*get_vector_legacy)(u8,u16*,u32*)=nullptr;
};
[[nodiscard]] static VectorAddress get_vector(LowLevelBoundaries& b,u8 vector) noexcept {
    VectorAddress r{}; if(b.get_vector_legacy)b.get_vector_legacy(vector,&r.selector,&r.offset); return r;
}
static void out8(LowLevelBoundaries& b,u16 port,u32 value) noexcept {
    if(b.out8)b.out8(port,static_cast<u8>(value));
}
static bool irq_save_cli(Machine& m) noexcept { const bool p=m.cpu.interrupt_enable; m.cpu.interrupt_enable=false; return p; }
static void irq_restore(Machine& m,bool p) noexcept { m.cpu.interrupt_enable=p; }

struct DispatcherBoundary {
    u32 (*invoke)(u32,const u32*,u32)=nullptr;
    void (*invoke_timer_callback)(u32)=nullptr;
    u32 (*allocate_timer)(u32)=nullptr;
    void (*release_timer)(u32)=nullptr;
    void (*set_timer_rate)(u32,u32)=nullptr;
    void (*enable_timer)(u32)=nullptr;
    void (*out8)(u16,u8)=nullptr;
};

[[nodiscard]] u32 function_35f88(Machine& m,u32 event,u32 slot) noexcept {
    if(slot>=16u) return 0u;
    u32 entry=load_le32(dg(m,0x1037Au+slot*4u));
    if(entry==0u) return 0u;
    for(;;entry+=8u) {
        const u32 key=load_le32(dg(m,entry));
        if(key==event) return load_le32(dg(m,entry+4u));
        if(key==0xFFFFFFFFu) return 0u;
    }
}
[[nodiscard]] u32 function_35fb4(Machine& m,DispatcherBoundary& b,u32 event,const u32* args,u32 count) noexcept {
    const u32 slot=args[0];
    const u32 target=function_35f88(m,event,slot);
    return target&&b.invoke?b.invoke(target,args,count):0u;
}

enum class Irq8Action:u8 { Handled, ChainSavedVector };
struct Irq8Result { Irq8Action action=Irq8Action::Handled; u16 selector=0; u32 offset=0; };

[[nodiscard]] Irq8Result function_35fc6(Machine& m,DispatcherBoundary& b) noexcept {
    if(load_le16(dg(m,0x10272u))!=0u) {
        if(b.out8)b.out8(0x20u,0x20u);
        return{};
    }
    store_le16(dg(m,0x10272u),1u);
    store_le32(dg(m,0x1036Eu),0u);
    for(;;) {
        const u32 slot=load_le32(dg(m,0x1036Eu));
        if(load_le16(dg(m,0x102B8u+slot*2u))==2u) {
            const u32 i=slot*4u;
            u32 elapsed=load_le32(dg(m,0x102DAu+i));
            elapsed+=load_le32(dg(m,0x10362u));
            const u32 period=load_le32(dg(m,0x1031Eu+i));
            if(elapsed<period) store_le32(dg(m,0x102DAu+i),elapsed);
            else {
                elapsed-=period;
                store_le32(dg(m,0x102DAu+i),elapsed);
                if(slot>=16u) store_le16(dg(m,0x10272u),0u);
                const u32 callback=load_le32(dg(m,0x10274u+i));
                if(b.invoke_timer_callback)b.invoke_timer_callback(callback);
            }
        }
        const u32 next=slot+1u;
        store_le32(dg(m,0x1036Eu),next);
        if(next>16u) break;
    }
    store_le16(dg(m,0x10272u),0u);
    if(b.out8)b.out8(0x20u,0x20u);
    return{};
}
[[nodiscard]] Irq8Result function_3607b(Machine& m) noexcept {
    return{Irq8Action::ChainSavedVector,static_cast<u16>(load_le32(dg(m,0x1036Au))),load_le32(dg(m,0x10366u))};
}

void function_3609b(Machine& m) noexcept {
    const bool p=irq_save_cli(m);
    store_le32(dg(m,0x10362u),0xFFFFFFFFu);
    for(u32 i=0;i<17u;++i) store_le16(dg(m,0x102B8u+i*2u),0u);
    for(u32 i=0;i<17u;++i) store_le32(dg(m,0x102DAu+i*4u),0u);
    for(u32 i=0;i<17u;++i) store_le32(dg(m,0x1031Eu+i*4u),0u);
    irq_restore(m,p);
}
void function_360e8(Machine& m,LowLevelBoundaries& b,u16 code_selector) noexcept {
    const bool p=irq_save_cli(m);
    const VectorAddress old=get_vector(b,8u);
    m.cpu.edx=(m.cpu.edx&0xFFFF0000u)|old.selector;
    store_le32(dg(m,0x10366u),old.offset);
    store_le32(dg(m,0x1036Au),m.cpu.edx);
    store_le32(dg(m,0x102B4u),0x0003607Bu);
    if(b.set_vector_legacy)b.set_vector_legacy(8u,code_selector,0x00035FC6u);
    irq_restore(m,p);
}
void function_36137(Machine& m,LowLevelBoundaries& b) noexcept {
    const bool p=irq_save_cli(m);
    store_le32(dg(m,0x1036Eu),0xFFFFFFFFu);
    if(b.set_vector_legacy)b.set_vector_legacy(8u,static_cast<u16>(load_le32(dg(m,0x1036Au))),load_le32(dg(m,0x10366u)));
    irq_restore(m,p);
}
void function_3616f(Machine& m,LowLevelBoundaries& b,u32 reload) noexcept {
    const bool p=irq_save_cli(m);
    out8(b,0x43u,0x36u);
    store_le32(dg(m,0x10376u),reload);
    out8(b,0x40u,reload);
    out8(b,0x40u,reload>>8);
    irq_restore(m,p);
}
void function_3619d(Machine& m,LowLevelBoundaries& b,u32 period) noexcept {
    u32 reload=0u;
    if(period<0xD68Du) reload=static_cast<u32>((static_cast<u64>(period)*0x2710u)/0x20BCu);
    function_3616f(m,b,reload);
}
void function_361ce(Machine& m,LowLevelBoundaries& b) noexcept {
    const bool p=irq_save_cli(m);
    u32 minimum=0xFFFFFFFFu;
    for(u32 i=0;i<=16u;++i) if(load_le16(dg(m,0x102B8u+i*2u))!=0u) {
        const u32 v=load_le32(dg(m,0x1031Eu+i*4u)); if(v<minimum)minimum=v;
    }
    store_le32(dg(m,0x10372u),minimum);
    if(minimum!=load_le32(dg(m,0x10362u))) {
        store_le32(dg(m,0x1036Eu),0xFFFFFFFFu);
        store_le32(dg(m,0x10362u),minimum);
        function_3619d(m,b,minimum);
        for(u32 i=0;i<17u;++i)store_le32(dg(m,0x102DAu+i*4u),0u);
    }
    irq_restore(m,p);
}
void function_36254(Machine& m) noexcept {
    const bool p=irq_save_cli(m);
    store_le16(dg(m,0x1044Au),m.cpu.ds);
    store_le16(dg(m,0x10270u),0u);
    store_le16(dg(m,0x10272u),0u);
    for(u32 i=0;i<16u;++i)store_le32(dg(m,0x1037Au+i*4u),0u);
    for(u32 i=0;i<16u;++i)store_le32(dg(m,0x103BAu+i*4u),0xFFFFFFFFu);
    for(u32 i=0;i<16u;++i)store_le32(dg(m,0x103FAu+i*4u),0u);
    irq_restore(m,p);
}

struct TimerContinuationBoundaries { void(*call_3678a)(u32,u32)=nullptr; u16 code_selector=0; };
void function_363dd(Machine&,LowLevelBoundaries&,u32) noexcept;
void function_3642a(Machine&,LowLevelBoundaries&) noexcept;
void function_3644f(Machine&,u32) noexcept;
void function_364f9(Machine&,LowLevelBoundaries&,u32,u32) noexcept;

void function_362be(Machine& m,LowLevelBoundaries& b,TimerContinuationBoundaries& x,u32 argument) noexcept {
    const bool p=irq_save_cli(m);
    store_le32(dg(m,0x1043Eu),0u);
    do {
        const u32 slot=load_le32(dg(m,0x1043Eu)); const u32 i=slot*4u;
        const u32 handle=load_le32(dg(m,0x103BAu+i));
        if(load_le32(dg(m,0x1037Au+i))!=0u) {
            if(handle!=0xFFFFFFFFu)function_363dd(m,b,handle);
            if(x.call_3678a)x.call_3678a(slot,argument);
        }
        store_le32(dg(m,0x1043Eu),slot+1u);
    } while(load_le32(dg(m,0x1043Eu))!=16u);
    function_3642a(m,b);
    irq_restore(m,p);
}
[[nodiscard]] u32 function_3632d(Machine& m,LowLevelBoundaries& b,TimerContinuationBoundaries& x,u32 value) noexcept {
    const bool p=irq_save_cli(m);
    u32 byte_index=0u;
    while(byte_index<0x20u && load_le16(dg(m,0x102B8u+byte_index))!=0u)byte_index+=2u;
    if(byte_index==0x20u){irq_restore(m,p);return 0xFFFFFFFFu;}
    const u32 slot=byte_index>>1; const u32 di=slot*4u;
    store_le16(dg(m,0x102B8u+byte_index),1u);
    store_le32(dg(m,0x10274u+di),value);
    store_le32(dg(m,0x1031Eu+di),0xFFFFFFFFu);
    const u16 count=static_cast<u16>(load_le16(dg(m,0x10270u))+1u);
    store_le16(dg(m,0x10270u),count);
    if(count==1u) {
        function_3609b(m);
        store_le16(dg(m,0x102D8u),1u);
        function_360e8(m,b,x.code_selector);
        function_364f9(m,b,16u,0xD68Du);
        function_3644f(m,16u);
        store_le16(dg(m,0x102B8u+slot*2u),1u);
        store_le32(dg(m,0x1031Eu+di),0xFFFFFFFFu);
    }
    irq_restore(m,p); return slot;
}
void function_363dd(Machine& m,LowLevelBoundaries& b,u32 slot) noexcept {
    const bool p=irq_save_cli(m);
    if(slot!=0xFFFFFFFFu) {
        const u32 wi=slot*2u;
        if(load_le16(dg(m,0x102B8u+wi))!=0u) {
            store_le16(dg(m,0x102B8u+wi),0u);
            const u16 count=static_cast<u16>(load_le16(dg(m,0x10270u))-1u);
            store_le16(dg(m,0x10270u),count);
            if(count==0u){function_3616f(m,b,0u);function_36137(m,b);}
        }
    }
    irq_restore(m,p);
}
void function_3642a(Machine& m,LowLevelBoundaries& b) noexcept {
    const bool p=irq_save_cli(m); for(s32 slot=15;slot>=0;--slot)function_363dd(m,b,static_cast<u32>(slot)); irq_restore(m,p);
}
void function_3644f(Machine& m,u32 slot) noexcept {
    const bool p=irq_save_cli(m); u8* q=dg(m,0x102B8u+slot*2u); if(load_le16(q)==1u)store_le16(q,2u); irq_restore(m,p);
}
void function_3647f(Machine& m) noexcept {
    const bool p=irq_save_cli(m); for(s32 slot=15;slot>=0;--slot)function_3644f(m,static_cast<u32>(slot)); irq_restore(m,p);
}
void function_364a4(Machine& m,u32 slot) noexcept {
    const bool p=irq_save_cli(m); u8* q=dg(m,0x102B8u+slot*2u); if(load_le16(q)==2u)store_le16(q,1u); irq_restore(m,p);
}
void function_364d4(Machine& m) noexcept {
    const bool p=irq_save_cli(m); for(s32 slot=15;slot>=0;--slot)function_364a4(m,static_cast<u32>(slot)); irq_restore(m,p);
}
void function_364f9(Machine& m,LowLevelBoundaries& b,u32 slot,u32 period) noexcept {
    const bool p=irq_save_cli(m); u8* state=dg(m,0x102B8u+slot*2u); const u16 saved=load_le16(state);
    store_le16(state,1u); store_le32(dg(m,0x1031Eu+slot*4u),period); store_le32(dg(m,0x102DAu+slot*4u),0u);
    function_361ce(m,b); store_le16(state,saved); irq_restore(m,p);
}
void function_3654e(Machine& m,LowLevelBoundaries& b,u32 slot,u32 divisor) noexcept {
    const bool p=irq_save_cli(m); function_364f9(m,b,slot,1000000u/divisor); irq_restore(m,p);
}
void function_36581(Machine& m,LowLevelBoundaries& b,u32 slot,u32 value) noexcept {
    const bool p=irq_save_cli(m); u32 period=0xD68Du; if(value!=0u)period=static_cast<u32>((static_cast<u64>(0x2710u)*value)/0x2E9Cu); function_364f9(m,b,slot,period); irq_restore(m,p);
}
[[nodiscard]] u32 function_365c6(Machine& m) noexcept {
    const bool p=irq_save_cli(m); const u32 v=load_le32(dg(m,0x10376u)); irq_restore(m,p); return v;
}

void function_3666e(Machine& m,u32 slot) noexcept {
    const bool p=irq_save_cli(m); if(slot<16u)store_le32(dg(m,0x1037Au+slot*4u),0u); irq_restore(m,p);
}
[[nodiscard]] u32 function_3669c(Machine& m,DispatcherBoundary& b,u32 slot) noexcept {
    const u32 args[2]={slot,0x000365C6u}; return function_35fb4(m,b,0x64u,args,2u);
}
[[nodiscard]] u32 function_365df(Machine& m,DispatcherBoundary& b,u32 descriptor) noexcept {
    const bool p=irq_save_cli(m); store_le32(dg(m,0x1043Eu),0u); u32 result=0xFFFFFFFFu;
    for(;;) {
        const u32 slot=load_le32(dg(m,0x1043Eu)); const u32 i=slot*4u;
        if(load_le32(dg(m,0x1037Au+i))==0u) {
            if(load_le32(dg(m,descriptor+4u))==0x79706F43u) {
                store_le32(dg(m,0x1037Au+i),load_le32(dg(m,descriptor)));
                const u32 returned=function_3669c(m,b,slot);
                if(returned!=0u && load_le32(dg(m,returned))<=load_le32(dg(m,0x1044Cu)))result=load_le32(dg(m,0x1043Eu));
            }
            break;
        }
        store_le32(dg(m,0x1043Eu),slot+1u);
        if(load_le32(dg(m,0x1043Eu))==16u)break;
    }
    irq_restore(m,p); return result;
}
[[nodiscard]] u32 function_366bb(Machine& m,DispatcherBoundary& b,const u32* args,u32 count) noexcept { return function_35fb4(m,b,0x65u,args,count); }
void function_366c5(Machine& m,DispatcherBoundary& b,u32 a1,u32 a2,u32 a3,u32 a4,u32 a5) noexcept {
    const bool p=irq_save_cli(m);
    if(a1<16u) {
        store_le32(dg(m,0x10446u),0xFFFFFFFFu);
        const u32 record=function_3669c(m,b,a1);
        const u32 divisor=load_le32(dg(m,record+0x20u));
        if(divisor!=0xFFFFFFFFu) {
            const u32 target=function_35f88(m,0x67u,a1);
            if(target!=0u && b.allocate_timer) {
                const u32 timer=b.allocate_timer(target);
                store_le32(dg(m,0x103BAu+a1*4u),timer);
                store_le32(dg(m,0x10446u),timer);
                if(b.set_timer_rate)b.set_timer_rate(timer,divisor);
            }
        }
        const u32 args[5]={a1,a2,a3,a4,a5}; (void)function_35fb4(m,b,0x66u,args,5u);
        store_le32(dg(m,0x103FAu+a1*4u),1u);
        const u32 timer=load_le32(dg(m,0x10446u)); if(timer!=0xFFFFFFFFu&&b.enable_timer)b.enable_timer(timer);
    }
    irq_restore(m,p);
}
[[nodiscard]] u32 function_3678a(Machine& m,DispatcherBoundary& b,u32 slot,u32 argument) noexcept {
    if(slot>=16u)return 0u;
    const u32 i=slot*4u;
    const u32 pending=load_le32(dg(m,0x103FAu+i));
    store_le32(dg(m,0x103FAu+i),0u); if(pending==0u)return 0u;
    const u32 timer=load_le32(dg(m,0x103BAu+i)); if(timer!=0xFFFFFFFFu&&b.release_timer)b.release_timer(timer);
    const u32 args[2]={slot,argument}; return function_35fb4(m,b,0x68u,args,2u);
}

constexpr u32 kWrapperEvents[48]={
    0x78,0x79,0x86,0x7A,0x7B,0x85,0x7C,0x7D,0x7E,0x7F,0x80,0x81,
    0x82,0x83,0x84,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,
    0x9F,0xAA,0xAB,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,
    0xB6,0xB7,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF,0xC0,0xC1,0xC2};
[[nodiscard]] u32 function_367c7_369a6(Machine& m,DispatcherBoundary& b,u32 wrapper_index,const u32* args,u32 count) noexcept {
    return wrapper_index<48u?function_35fb4(m,b,kWrapperEvents[wrapper_index],args,count):0u;
}
#define CODE2_EVENT_WRAPPER(hexaddr,index) \
[[nodiscard]] u32 function_##hexaddr(Machine& m,DispatcherBoundary& b,const u32* a,u32 n) noexcept { return function_367c7_369a6(m,b,index,a,n); }
CODE2_EVENT_WRAPPER(367c7,0)  CODE2_EVENT_WRAPPER(367d1,1)  CODE2_EVENT_WRAPPER(367db,2)  CODE2_EVENT_WRAPPER(367e5,3)
CODE2_EVENT_WRAPPER(367ef,4)  CODE2_EVENT_WRAPPER(367f9,5)  CODE2_EVENT_WRAPPER(36803,6)  CODE2_EVENT_WRAPPER(3680d,7)
CODE2_EVENT_WRAPPER(36817,8)  CODE2_EVENT_WRAPPER(36821,9)  CODE2_EVENT_WRAPPER(3682b,10) CODE2_EVENT_WRAPPER(36835,11)
CODE2_EVENT_WRAPPER(3683f,12) CODE2_EVENT_WRAPPER(36849,13) CODE2_EVENT_WRAPPER(36853,14) CODE2_EVENT_WRAPPER(3685d,15)
CODE2_EVENT_WRAPPER(36867,16) CODE2_EVENT_WRAPPER(36871,17) CODE2_EVENT_WRAPPER(3687b,18) CODE2_EVENT_WRAPPER(36885,19)
CODE2_EVENT_WRAPPER(3688f,20) CODE2_EVENT_WRAPPER(36899,21) CODE2_EVENT_WRAPPER(368a3,22) CODE2_EVENT_WRAPPER(368ad,23)
CODE2_EVENT_WRAPPER(368b7,24) CODE2_EVENT_WRAPPER(368c1,25) CODE2_EVENT_WRAPPER(368cb,26) CODE2_EVENT_WRAPPER(368d5,27)
CODE2_EVENT_WRAPPER(368df,28) CODE2_EVENT_WRAPPER(368e9,29) CODE2_EVENT_WRAPPER(368f3,30) CODE2_EVENT_WRAPPER(368fd,31)
CODE2_EVENT_WRAPPER(36907,32) CODE2_EVENT_WRAPPER(36911,33) CODE2_EVENT_WRAPPER(3691b,34) CODE2_EVENT_WRAPPER(36925,35)
CODE2_EVENT_WRAPPER(3692f,36) CODE2_EVENT_WRAPPER(36939,37) CODE2_EVENT_WRAPPER(36943,38) CODE2_EVENT_WRAPPER(3694d,39)
CODE2_EVENT_WRAPPER(36957,40) CODE2_EVENT_WRAPPER(36961,41) CODE2_EVENT_WRAPPER(3696b,42) CODE2_EVENT_WRAPPER(36975,43)
CODE2_EVENT_WRAPPER(3697f,44) CODE2_EVENT_WRAPPER(36989,45) CODE2_EVENT_WRAPPER(36993,46) CODE2_EVENT_WRAPPER(3699d,47)
#undef CODE2_EVENT_WRAPPER

static_assert(0x35FB4u-0x35F88u==44u);
static_assert(0x35FC6u-0x35FB4u==18u);
static_assert(0x3607Bu-0x35FC6u==181u);
static_assert(0x3609Bu-0x3607Bu==32u);
static_assert(0x369A7u-0x35F88u==2591u);

// -----------------------------------------------------------------------------
// 391F2..39525 — full-register interrupt adapter plus 256 three-byte veneers.
// CODE.2 adds a CS override to the veneer-address LEA, making this family one
// byte longer than the corresponding audited CODE.1 FFA1..102D3 family.
// Veneer N is at 39226h + 3*N; vector 3 is CC 90 C3, all others CD imm8 C3.
// -----------------------------------------------------------------------------

struct RegisterImage {
    u32 eax=0,ebx=0,ecx=0,edx=0,esi=0,edi=0,ebp=0;
    u16 ds=0,es=0,fs=0,gs=0;
};
struct InterruptBridgeBoundary { void(*invoke_full)(u32,RegisterImage&)=nullptr; };
void function_391f2(InterruptBridgeBoundary& b,u32 vector,RegisterImage& image) noexcept {
    if(b.invoke_full)b.invoke_full(vector&0xFFu,image);
}
[[nodiscard]] constexpr u32 interrupt_veneer_39226(u32 vector) noexcept {
    return 0x00039226u + 3u*(vector&0xFFu);
}
static_assert(interrupt_veneer_39226(0)==0x39226u);
static_assert(interrupt_veneer_39226(3)==0x3922Fu);
static_assert(interrupt_veneer_39226(255)==0x39523u);
static_assert(0x39526u-0x391F2u==820u);

static_assert(kObject1Length == 239698u);
static_assert(kObject2FileStart + kObject2StoredBytes == 0x00088661u);
static_assert(kVerifiedTranslatedBytes + kVerifiedNonCodeBytes <= kObject1Length);
static_assert(kVerifiedTranslatedBytes ==
              (0x00073u-0x00010u) + (0x000B8u-0x00074u) +
              (0x001FAu-0x000B8u) + (0x00271u-0x001FCu) +
              (0x369A7u-0x35F88u) + (0x39526u-0x391F2u));

} // namespace code2
