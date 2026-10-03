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
constexpr u32 kVerifiedTranslatedBytes = 42612u;
constexpr u32 kVerifiedNonCodeBytes = 384u;

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
static_assert(kVerifiedTranslatedBytes == 39537u +
              (0x0869Fu-0x080ACu) + (0x0871Bu-0x086C4u) +
              (0x0893Cu-0x0871Cu) + (0x08A7Du-0x0893Cu) +
              (0x08C88u-0x08A80u) + (0x08CD8u-0x08C88u));

// -----------------------------------------------------------------------------
// Batch 02: 002A8 dispatcher and CODE.1-audited VGA/image/sprite anchors.
// -----------------------------------------------------------------------------

struct Dispatch002A8Boundary {
    EarlyBoundaries* early=nullptr;
    u32 (*call_1e8c8)(u32,u32)=nullptr;
    void (*call_20c0c)(u32,u32,u32)=nullptr;
    void (*call_08a80)(u32,s32,u32,u32,u32)=nullptr;
    void (*call_13ef8)(u32)=nullptr;
    void (*call_28110)(u32,u32,u32,u32)=nullptr;
    void (*call_28dcc)(u32)=nullptr;
    void (*call_28ef0)()=nullptr;
    void (*call_28c54)(u32,u32)=nullptr;
    void (*call_291b0)(u32)=nullptr;
    void (*call_1aacc)()=nullptr;
    void (*call_2a020)(u32)=nullptr;
    void (*call_1a288)()=nullptr;
    void (*call_1c40c)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_1c368)(u32,u32)=nullptr;
    void (*call_13fd4)(u32,u32,u32,u32,u32,u32,u32)=nullptr;
    void (*call_1a510)(u32)=nullptr;
    u32 (*call_1a1f4)()=nullptr;
    void (*call_1a5b0)(u32,s32,u32)=nullptr;
    void (*call_1c124)(u32)=nullptr;
    void (*call_1c160)(u32,u32)=nullptr; // stack argument, live EBX
    void (*call_1c1a0)(u32)=nullptr;
};

// 002A8..006B8: execute the selected 1Ah-byte script descriptor.  Byte 0 is
// the command count; each command occupies five following bytes.  The 13-way
// branch at 002FE uses the address table at 00274..002A7 (recorded as data).
void function_002a8(Machine& m,Dispatch002A8Boundary& b,s32 script_index) noexcept {
    if(script_index==-1) return;
    const u32 script=load_le32(dg(m,0xCF44u))+static_cast<u32>(script_index)*0x1Au;
    u32 cmd=script;
    s32 command_index=0;
    bool stop=false;
    while(command_index<static_cast<s32>(*dg(m,script))) {
        const u32 a2=*dg(m,cmd+2u),a3=*dg(m,cmd+3u);
        const u32 record=b.call_1e8c8?b.call_1e8c8(a3,a2):0u;
        const u8 opcode=*dg(m,cmd+1u);
        if(opcode<=0x0Cu) switch(opcode) {
            case 0u:
                if(record!=0u) {
                    u8* const flag=dg(m,record+1u);
                    if(*dg(m,record)==1u) {
                        if(*dg(m,record+6u)==0u) break;
                        const u8 saved=static_cast<u8>((*flag>>2)&1u);
                        *flag=static_cast<u8>(*flag|4u);
                        if(b.call_20c0c)b.call_20c0c(a3,a2,record);
                        *flag=static_cast<u8>((*flag&0xFBu)|(saved<<2));
                    } else *flag=static_cast<u8>(*flag|4u);
                } else (void)function_00010(m,static_cast<s32>(a3),static_cast<s32>(a2));
                break;
            case 1u:
                if(record!=0u) {
                    u8* const flag=dg(m,record+1u);
                    if(*dg(m,record)==1u) {
                        if(*dg(m,record+6u)!=0u) break;
                        const u8 saved=static_cast<u8>((*flag>>2)&1u);
                        *flag=static_cast<u8>(*flag|4u);
                        if(b.call_20c0c)b.call_20c0c(a3,a2,record);
                        *flag=static_cast<u8>((*flag&0xFBu)|(saved<<2));
                    } else *flag=static_cast<u8>(*flag&0xFBu);
                }
                break;
            case 2u:
                if(record!=0u) {
                    u8* const flag=dg(m,record+1u);
                    const u8 kind=*dg(m,record);
                    if(kind==1u) {
                        const u8 saved=static_cast<u8>((*flag>>2)&1u);
                        *flag=static_cast<u8>(*flag|4u);
                        if(b.call_20c0c)b.call_20c0c(a3,a2,record);
                        *flag=static_cast<u8>((*flag&0xFBu)|(saved<<2));
                    } else if(kind==6u) {
                        const u8 bit=static_cast<u8>((((*flag>>2)&1u)^1u)<<2);
                        *flag=static_cast<u8>((*flag&0xFBu)|bit);
                    } else {
                        const u8 low=static_cast<u8>((*flag&3u)^1u);
                        *flag=static_cast<u8>((*flag&0xFCu)|(low&3u));
                    }
                }
                break;
            case 3u: {
                const s32 numerator=8-static_cast<s32>(*dg(m,cmd+5u));
                const s32 rem=numerator%8;
                const s32 offset=rem*0x2D;
                const u32 packed3=(static_cast<u32>(*dg(m,cmd+3u))<<16)|0x8000u;
                const u32 packed2=(static_cast<u32>(*dg(m,cmd+2u))<<16)|0x8000u;
                if(b.call_08a80)b.call_08a80(*dg(m,cmd+4u),offset,4u,packed2,packed3);
                break;
            }
            case 4u: {
                const s32 product=static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x25619u))))*
                                  static_cast<s32>(*dg(m,cmd+2u))+
                                  static_cast<s32>(*dg(m,cmd+3u));
                const u32 addr=0x198E0u+static_cast<u32>(product)*2u;
                const u16 v=static_cast<u16>((load_le16(dg(m,addr))&0xFFC0u)|(*dg(m,cmd+4u)&0x3Fu));
                store_le16(dg(m,addr),v);
                break;
            }
            case 5u:
                if(b.call_28110)b.call_28110(load_le32(dg(m,cmd+4u)),0xFFu,0u,0u);
                break;
            case 6u:
                if(b.call_28dcc)b.call_28dcc(0x9C4u);
                if(b.call_28ef0)b.call_28ef0();
                if(b.call_28c54)b.call_28c54(load_le32(dg(m,cmd+4u)),1u);
                break;
            case 7u: {
                const u32 toggled=static_cast<u32>(static_cast<u8>(*dg(m,0xC050u)^1u));
                if(b.call_291b0)b.call_291b0(toggled);
                if(b.call_1aacc)b.call_1aacc();
                if(b.call_2a020)b.call_2a020(load_le32(dg(m,0xC050u)));
                if(b.call_1a288)b.call_1a288();
                if(b.call_291b0)b.call_291b0(*dg(m,0xC050u));
                if(b.call_1c40c)b.call_1c40c(load_le32(dg(m,0xC050u)),0x10u,0x30u,0xC8u,0xC8u);
                if(b.call_1c368)b.call_1c368(0x759Cu,*dg(m,cmd+4u));
                if(b.call_13fd4)b.call_13fd4(0u,0x10u,0x30u,0x0Au,0xC8u,0x759Cu,1u);
                if(b.call_1a510)b.call_1a510(load_le32(dg(m,0xC050u)));
                *dg(m,0xC050u)=static_cast<u8>(*dg(m,0xC050u)^1u);
                if(b.call_1a1f4)while(b.call_1a1f4()!=0u){}
                if(b.call_1a1f4)while(b.call_1a1f4()==0u){}
                if(b.call_1a1f4)while(b.call_1a1f4()!=0u){}
                if(b.call_1a5b0)b.call_1a5b0(load_le32(dg(m,0xC050u)),
                    static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x8600u)))),*dg(m,0x8602u));
                *dg(m,0xC050u)=static_cast<u8>(*dg(m,0xC050u)^1u);
                break;
            }
            case 8u:
                if(b.call_1c124)b.call_1c124(*dg(m,cmd+4u));
                break;
            case 9u:
                if(b.call_1c160)b.call_1c160(*dg(m,cmd+4u),record);
                break;
            case 10u:
                if(b.call_1c1a0)b.call_1c1a0(*dg(m,cmd+4u));
                break;
            case 11u: {
                const u32 sub=*dg(m,cmd+4u);
                if(sub==0x0Bu) { if(b.early)(void)function_00074(m,*b.early); }
                else if(sub==0x0Cu) { if(b.early)(void)function_000b8(m,*b.early); }
                else if(sub==0x0Du) {
                    const u32 ok=b.early?function_001fc(m,*b.early):0u;
                    if(ok!=0u&&record!=0u)*dg(m,record+1u)=static_cast<u8>(*dg(m,record+1u)&0xFBu);
                } else { if(b.call_13ef8)b.call_13ef8(sub); stop=true; }
                break;
            }
            case 12u:
                store_le32(dg(m,0xBF44u),*dg(m,cmd+4u));
                break;
        }
        if(stop) break;
        cmd+=5u;
        ++command_index;
    }
}
static_assert(0x006B9u-0x002A8u==1041u);


struct Graphics2Boundary {
    u8 (*read8)(u32)=nullptr;
    u16 (*read16)(u32)=nullptr;
    u32 (*read32)(u32)=nullptr;
    void (*write8)(u32,u8)=nullptr;
    void (*write16)(u32,u16)=nullptr;
    void (*write32)(u32,u32)=nullptr;
    u8 (*in8)(u16)=nullptr;
    void (*out8)(u16,u8)=nullptr;
    void (*out16)(u16,u16)=nullptr;
    void (*sync_2ab98)()=nullptr;
    void (*sync_2abc4)(u32)=nullptr;
    void (*call_2abf4)(u32)=nullptr;
    void (*call_2ac10)(u32)=nullptr;
    u8 (*bios_get_mode)()=nullptr;
    void (*bios_set_mode)(u8)=nullptr;
    void (*translate_2ab38)(u32,u32,u32*,u8*)=nullptr;
    void (*planar_fill)(u32,u32,u32,u8,u8,u8,u32,u8)=nullptr;
};
static u8 g2r8(Graphics2Boundary& b,u32 a) noexcept { return b.read8?b.read8(a):0u; }
static u16 g2r16(Graphics2Boundary& b,u32 a) noexcept { return b.read16?b.read16(a):0u; }
static u32 g2r32(Graphics2Boundary& b,u32 a) noexcept { return b.read32?b.read32(a):0u; }
static void g2w8(Graphics2Boundary& b,u32 a,u8 v) noexcept { if(b.write8)b.write8(a,v); }
static void g2w16(Graphics2Boundary& b,u32 a,u16 v) noexcept { if(b.write16)b.write16(a,v); }
static constexpr u8 g2low8(u32 v) noexcept { return static_cast<u8>(v); }
static constexpr u16 g2low16(u32 v) noexcept { return static_cast<u16>(v); }
static constexpr void g2setlow16(u32& v,u16 x) noexcept { v=(v&0xFFFF0000u)|x; }
struct Planar2 { u32 addr=0; u32 plane=0; };
static Planar2 g2planar(Graphics2Boundary& b,u32 a,u32 c) noexcept {
    Planar2 p{}; u8 q=0; if(b.translate_2ab38)b.translate_2ab38(a,c,&p.addr,&q); p.plane=q; return p;
}

[[nodiscard]] u32 function_29930(Machine& m,Graphics2Boundary& b,u32 mode,u32 out) noexcept {
    store_le32(dg(m,0xEFE1u),mode&0x80u); mode&=0x7Fu;
    store_le32(dg(m,0xEFE5u),mode); store_le32(dg(m,0xEFF5u),0xC8u);
    store_le32(dg(m,0xEFF9u),0x50u); store_le32(dg(m,0xF001u),0xA0000u);
    store_le32(dg(m,0xEFE9u),0u); *dg(m,0xEFE0u)=0u; store_le32(dg(m,0xF005u),0x400u);
    store_le32(dg(m,0xEFEDu),4u); store_le32(dg(m,0xEFF1u),2u);
    if(b.sync_2ab98)b.sync_2ab98();
    if(mode==3u)store_le32(dg(m,0xF005u),0x800u);
    const u8 vm=b.bios_get_mode?b.bios_get_mode():0x13u;
    if(vm!=0x13u&&b.bios_set_mode)b.bios_set_mode(0x13u);
    auto in=[&](u16 p){return b.in8?b.in8(p):0u;};
    auto o8=[&](u16 p,u8 v){if(b.out8)b.out8(p,v);};
    auto o16=[&](u16 p,u16 v){if(b.out16)b.out16(p,v);};
    o16(0x3CEu,static_cast<u16>(3u|(static_cast<u16>(*dg(m,0xEFE0u))<<8)));
    o8(0x3C4u,4u); u8 x=in(0x3C5u); o8(0x3C5u,static_cast<u8>((x&0xF7u)|4u));
    o8(0x3CEu,5u); x=in(0x3CFu); o8(0x3CFu,static_cast<u8>(x&0xEFu));
    o8(0x3CEu,6u); x=in(0x3CFu); o8(0x3CFu,static_cast<u8>(x&0xFDu));
    if(b.sync_2ab98)b.sync_2ab98();
    o8(0x3D4u,9u); x=in(0x3D5u); o8(0x3D5u,mode==3u?static_cast<u8>(x&0xE0u):0x41u);
    o8(0x3D4u,0x14u); x=in(0x3D5u); o8(0x3D5u,static_cast<u8>(x&0xBFu));
    o8(0x3D4u,0x17u); x=in(0x3D5u); o8(0x3D5u,static_cast<u8>(x|0x40u));
    if(b.call_2abf4)b.call_2abf4(0u);
    if(b.call_2ac10)b.call_2ac10(0xFFu);
    u32 height=0xC8u,width=0x140u; if(mode==3u)height<<=1;
    store_le32(dg(m,0xEFFDu),height); store_le32(dg(m,0xEFE1u),0u);
    g2w8(b,out+1u,2u); g2w16(b,out+3u,static_cast<u16>(height)); g2w16(b,out+5u,static_cast<u16>(width));
    return 0x0Du;
}
static_assert(0x29A84u-0x29930u==340u);
void function_29a84(Graphics2Boundary& b) noexcept { if(b.sync_2ab98)b.sync_2ab98(); if(b.bios_set_mode)b.bios_set_mode(3u); }
static_assert(0x29A9Fu-0x29A84u==27u);
void function_29aa4(Machine& m,Graphics2Boundary& b,u32 x,u32 y,u8 value) noexcept {
    const Planar2 p=g2planar(b,x,y);
    if(b.out16){b.out16(0x3CEu,static_cast<u16>(4u|(static_cast<u16>(p.plane)<<8)));b.out16(0x3C4u,static_cast<u16>(2u|(static_cast<u16>(1u<<p.plane)<<8)));b.out16(0x3CEu,static_cast<u16>(3u|(static_cast<u16>(*dg(m,0xEFE0u))<<8)));}
    (void)g2r8(b,p.addr);g2w8(b,p.addr,value);if(b.out16)b.out16(0x3CEu,3u);
}
static_assert(0x29AF0u-0x29AA4u==76u);
[[nodiscard]] u8 function_29af0(Graphics2Boundary& b,u32 x,u32 y) noexcept {
    const Planar2 p=g2planar(b,x,y);if(b.out16)b.out16(0x3CEu,static_cast<u16>(4u|(static_cast<u16>(p.plane)<<8)));return g2r8(b,p.addr);
}
static_assert(0x29B19u-0x29AF0u==41u);

static u8 sar8_exact_29b40(u8 v,u32 count) noexcept {
    count&=0x1Fu;if(count==0u)return v;const bool neg=(v&0x80u)!=0u;if(count>=8u)return neg?0xFFu:0u;
    u8 r=static_cast<u8>(v>>count);if(neg)r=static_cast<u8>(r|static_cast<u8>(0xFFu<<(8u-count)));return r;
}
void function_29b40(Machine& m,Graphics2Boundary& b,s32 x,s32 y,s32 w,s32 h,u8 color) noexcept {
    if(w<=0||h<=0)return;
    const Planar2 p=g2planar(b,static_cast<u32>(x),static_cast<u32>(y));
    u8 left=static_cast<u8>(0x0Fu<<p.plane),right=0u;u32 work=static_cast<u32>(h-1),phase=static_cast<u32>(p.plane^3u),middle=0u;
    const bool single=work<=phase;work-=phase;
    if(!single){--work;const u32 rem=work&3u;work-=rem;const u32 shift=rem^3u;right=static_cast<u8>(~sar8_exact_29b40(0xF0u,shift));middle=work>>2;}
    else{work+=3u;const u32 shift=static_cast<u32>(g2low8(work)^3u);right=static_cast<u8>(~sar8_exact_29b40(0xF0u,shift));left=static_cast<u8>(left&right);right=0u;middle=0u;}
    u32 stride=load_le32(dg(m,0xEFF9u))-middle-1u;if(right!=0u)g2setlow16(stride,static_cast<u16>(g2low16(stride)-1u));
    if(b.planar_fill)b.planar_fill(p.addr,static_cast<u32>(w),middle,left,right,color,stride,*dg(m,0xEFE0u));
}
static_assert(0x29C9Eu-0x29B40u==350u);
void function_29ca0(Graphics2Boundary& b,u32 src,u32 first,u32 count) noexcept {
    if(b.out8)b.out8(0x3C8u,static_cast<u8>(first));
    u32 p=src,n=count;
    for(;;){if(b.sync_2abc4)b.sync_2abc4(n);if(static_cast<s32>(n)>0xC0){if(b.out8)for(u32 i=0;i<0xC0u;++i)b.out8(0x3C9u,g2r8(b,p+i));p+=0xC0u;n-=0xC0u;continue;}if(n!=0u&&b.out8)for(u32 i=0;i<n;++i)b.out8(0x3C9u,g2r8(b,p+i));break;}
}
static_assert(0x29CE9u-0x29CA0u==73u);

static u8 g2rol1(u8 v) noexcept { return static_cast<u8>((v<<1)|(v>>7)); }
static void g2mask(Graphics2Boundary& b,u8 mask) noexcept { if(b.out16)b.out16(0x3C4u,static_cast<u16>(2u|(static_cast<u16>(mask)<<8))); }
static void g2put(Graphics2Boundary& b,u32 addr,u8 value) noexcept { (void)g2r8(b,addr);g2w8(b,addr,value); }
static bool g2neg32(u32 v) noexcept { return (v&0x80000000u)!=0u; }
static bool g2sgt32(u32 a,u32 b) noexcept { const bool sa=g2neg32(a),sb=g2neg32(b);return sa==sb?a>b:(!sa&&sb); }
static void g2step_plane4(Graphics2Boundary& b,u8& mask,u32& addr) noexcept { mask=static_cast<u8>((static_cast<u32>(mask)<<1)&0x0Fu);if(mask==0u){mask=1u;++addr;}g2mask(b,mask); }

void function_29cec(Machine& m,Graphics2Boundary& b,s32 a0,s32 b0,s32 a1,s32 b1,u32 color) noexcept {
    if(b.out16)b.out16(0x3CEu,static_cast<u16>(3u|(static_cast<u16>(*dg(m,0xEFE0u))<<8)));
    u32 r0=static_cast<u32>(a0),c0=static_cast<u32>(b0),r1=static_cast<u32>(a1),c1=static_cast<u32>(b1);u32 pitch=load_le32(dg(m,0xEFF9u));const u8 pixel=static_cast<u8>(color);u32 dc=c1-c0;
    if(dc==0u){u32 dr=r1-r0;if(g2neg32(dr)){dr=0u-dr;r0=r1;}u32 count=dr+1u;const Planar2 p=g2planar(b,r0,c0);u32 addr=p.addr;const u8 mask=static_cast<u8>(1u<<(p.plane&3u));g2mask(b,mask);do{g2put(b,addr,pixel);addr+=pitch;}while(--count!=0u);if(b.out16)b.out16(0x3CEu,3u);return;}
    if(g2neg32(dc)){dc=0u-dc;u32 t=c0;c0=c1;c1=t;t=r0;r0=r1;r1=t;}u32 dr=r1-r0;
    if(dr==0u){
        const Planar2 p=g2planar(b,r0,c0);u32 addr=p.addr;u8 left=static_cast<u8>(0x0Fu<<(p.plane&3u));const u32 end_shift=(c1&3u)^3u;u8 right=static_cast<u8>(~sar8_exact_29b40(0xF0u,end_shift));u32 bytes=(c1>>2)-(c0>>2);
        if((left&0x80u)==0u){if(bytes==0u)right=static_cast<u8>(right&left);else{g2mask(b,left);g2put(b,addr,pixel);++addr;--bytes;if(bytes!=0u){g2mask(b,0xFFu);do{g2put(b,addr,pixel);++addr;}while(--bytes!=0u);}}}
        else{g2mask(b,0xFFu);do{g2put(b,addr,pixel);++addr;}while(--bytes!=0u);}g2mask(b,right);g2put(b,addr,pixel);if(b.out16)b.out16(0x3CEu,3u);return;
    }
    if(g2neg32(dr)){dr=0u-dr;pitch=0u-pitch;}const bool row_major=g2sgt32(dr,dc);u32 major=row_major?dr:dc,minor=row_major?dc:dr;const u32 twice=minor<<1;u32 err=twice-major;const u32 diagonal=err-major;u32 count=major+1u;
    const Planar2 p=g2planar(b,r0,c0);u32 addr=p.addr;u8 mask=static_cast<u8>(1u<<(p.plane&3u));g2mask(b,mask);const u32 scratch_hi=m.cpu.eax&0xFFFF0000u;store_le32(dg(m,0xF009u),scratch_hi|(static_cast<u32>(mask)<<8)|2u);
    do{g2put(b,addr,pixel);if(row_major){addr+=pitch;if(g2neg32(err))err+=twice;else{err+=diagonal;g2step_plane4(b,mask,addr);store_le32(dg(m,0xF009u),scratch_hi|(static_cast<u32>(mask)<<8)|2u);}}else{g2step_plane4(b,mask,addr);store_le32(dg(m,0xF009u),scratch_hi|(static_cast<u32>(mask)<<8)|2u);if(g2neg32(err))err+=twice;else{err+=diagonal;addr+=pitch;}}}while(--count!=0u);
    if(b.out16)b.out16(0x3CEu,3u);
}
static_assert(0x29EAAu-0x29CECu==446u);

[[nodiscard]] u32 function_2a1e8(u32 frames,u32 rows,u32 bytes_per_row,u32,u32 key) noexcept {u32 v=rows*bytes_per_row;v*=frames;v<<=3;if(key!=0xFFu)v+=frames;return v;}
static_assert(0x2A20Cu-0x2A1E8u==36u);
void function_2a20c(Graphics2Boundary& b,u32 frame,u32 count,u32 src,u32 desc,u32 key) noexcept {
    u32 dst=desc+8u+static_cast<u32>(g2r16(b,desc))*frame;if(static_cast<u8>(key)!=0xFFu)g2w8(b,dst++,static_cast<u8>(key));const u32 groups=count>>2;u32 left=groups,s=src;
    do{const u32 base=dst;g2w8(b,base,g2r8(b,s++));g2w8(b,base+groups,g2r8(b,s++));g2w8(b,base+groups*2u,g2r8(b,s++));g2w8(b,base+groups*3u,g2r8(b,s++));dst=base+1u;}while(--left!=0u);
}
static_assert(0x2A256u-0x2A20Cu==74u);
void function_2a258(Graphics2Boundary& b,u32 frame,u32 desc) noexcept {
    u32 payload=desc+8u+static_cast<u32>(g2r16(b,desc))*frame;const u16 rows=g2r16(b,desc+4u);const u16 plane_bytes=static_cast<u8>(static_cast<u8>(g2r8(b,desc+6u))<<1);if((g2r8(b,desc+7u)&0x80u)!=0u)++payload;const u16 span=static_cast<u16>(rows*plane_bytes);
    auto swap_rows=[&](u32 lo,u16 offset){u16 rr=rows;do{u32 hi=lo+static_cast<u32>(offset);u16 cc=plane_bytes;do{const u8 v=g2r8(b,lo);const u8 z=g2r8(b,hi);g2w8(b,lo,z);g2w8(b,hi,v);++lo;--hi;}while(--cc!=0u);}while(--rr!=0u);};
    u16 off0=span;off0=static_cast<u16>(off0<<1);off0=static_cast<u16>(off0+span);u16 tail=plane_bytes;--tail;off0=static_cast<u16>(off0+tail);swap_rows(payload,off0);
    u16 off1=span;tail=plane_bytes;--tail;off1=static_cast<u16>(off1+tail);swap_rows(payload+static_cast<u32>(span),off1);
}
static_assert(0x2A305u-0x2A258u==173u);
void function_2a308(Graphics2Boundary& b,u32 frame,u32 desc) noexcept {
    u32 src=desc+8u+static_cast<u32>(g2r16(b,desc))*frame;const u32 rows=g2r16(b,desc+4u),width=g2r8(b,desc+6u);if((g2r8(b,desc+7u)&0x80u)!=0u)++src;u32 bottom=(rows-1u)*width;bottom<<=1;const u32 rowbytes=width<<1;
    u32 planes=4u;do{const u32 plane_start=src;u32 lo=src,hi=src+bottom;u32 pairs=rows>>1;do{u32 cc=rowbytes;do{const u8 v=g2r8(b,lo);const u8 z=g2r8(b,hi);g2w8(b,lo,z);g2w8(b,hi,v);++lo;++hi;}while(--cc!=0u);hi-=rowbytes;hi-=rowbytes;}while(--pairs!=0u);src=plane_start+bottom+rowbytes;}while(--planes!=0u);
}
static_assert(0x2A380u-0x2A308u==120u);

void function_2a380(Machine& m,Graphics2Boundary& b,u32 frame,u32 y,u32 x,u32 image) noexcept {
    const Planar2 p=g2planar(b,y,x);u32 dst0=p.addr;u8 mask=static_cast<u8>(0x11u<<p.plane);const u16 rows=g2r16(b,image+4u);const u8 half=g2r8(b,image+6u),flags=g2r8(b,image+7u);u32 src=image+8u+static_cast<u32>(g2r16(b,image))*frame;const u32 doubled=static_cast<u32>(half)<<1,skip=load_le32(dg(m,0xEFF9u))-doubled;
    if((flags&0x80u)==0u){for(u32 plane=0;plane<4u;++plane){g2mask(b,mask);u32 dst=dst0,rc=rows;do{u32 wc=half;while(wc--!=0u){const u16 v=g2r16(b,src);g2w16(b,dst,v);src+=2u;dst+=2u;}dst+=skip;}while(--rc!=0u);if(plane!=3u){const bool carry=(mask&0x80u)!=0u;mask=g2rol1(mask);if(carry)++dst0;}}}
    else{const u8 key=g2r8(b,src++);store_le32(dg(m,0xF009u),rows);const u32 bytes=static_cast<u8>(doubled);for(u32 plane=0;plane<4u;++plane){g2mask(b,mask);u32 dst=dst0;u16 rc=rows;do{u32 n=bytes;do{const u8 v=g2r8(b,src++);if(v!=key)g2w8(b,dst,v);++dst;}while(--n!=0u);dst+=skip;}while(--rc!=0u);if(plane!=3u){const bool carry=(mask&0x80u)!=0u;mask=g2rol1(mask);if(carry)++dst0;}}}
}
static_assert(0x2A478u-0x2A380u==248u);
void function_2a478(Machine& m,Graphics2Boundary& b,u32 frame,u32 y,u32 x,u32 image,u32 delta) noexcept {
    const Planar2 p=g2planar(b,y,x);u32 dst0=p.addr;u8 mask=static_cast<u8>(0x11u<<p.plane);const u8 sub=static_cast<u8>(delta);const u16 rows=g2r16(b,image+4u);const u8 half=g2r8(b,image+6u),flags=g2r8(b,image+7u);u32 src=image+8u+static_cast<u32>(g2r16(b,image))*frame;const u32 doubled=static_cast<u32>(half)<<1,skip=load_le32(dg(m,0xEFF9u))-doubled;
    if((flags&0x80u)==0u){const u32 bytes=static_cast<u32>(static_cast<u8>(doubled))<<1;for(u32 plane=0;plane<4u;++plane){g2mask(b,mask);u32 dst=dst0,rc=rows;do{u32 n=bytes;do{g2w8(b,dst++,static_cast<u8>(g2r8(b,src++)-sub));}while(--n!=0u);dst+=skip;}while(--rc!=0u);if(plane!=3u){const bool carry=(mask&0x80u)!=0u;mask=g2rol1(mask);if(carry)++dst0;}}}
    else{const u8 key=g2r8(b,src++);store_le32(dg(m,0xF009u),rows);const u32 bytes=static_cast<u8>(doubled);for(u32 plane=0;plane<4u;++plane){g2mask(b,mask);u32 dst=dst0,rc=rows;do{u32 n=bytes;do{const u8 v=g2r8(b,src++);if(v!=key)g2w8(b,dst,static_cast<u8>(v-sub));++dst;}while(--n!=0u);dst+=skip;}while(--rc!=0u);if(plane!=3u){const bool carry=(mask&0x80u)!=0u;mask=g2rol1(mask);if(carry)++dst0;}}}
}
static_assert(0x2A584u-0x2A478u==268u);
void function_2a584(Machine& m,Graphics2Boundary& b,u32 frame,u32 y,u32 x,u32 image,u32 table) noexcept {
    const Planar2 p=g2planar(b,y,x);u32 dst0=p.addr;u8 mask=static_cast<u8>(0x11u<<(p.plane&3u));const u16 rows=g2r16(b,image+4u);const u8 half=g2r8(b,image+6u),flags=g2r8(b,image+7u);u32 src=image+8u+static_cast<u32>(g2r16(b,image))*frame;const u32 doubled=static_cast<u32>(half)<<1,skip=load_le32(dg(m,0xEFF9u))-doubled;
    if((flags&0x80u)==0u){const u32 bytes=static_cast<u32>(static_cast<u8>(doubled))<<1;for(u32 plane=0;plane<4u;++plane){g2mask(b,mask);u32 dst=dst0,rc=rows;do{u32 n=bytes;do{const u8 v=g2r8(b,src++);g2w8(b,dst++,g2r8(b,table+v));}while(--n!=0u);dst+=skip;}while(--rc!=0u);if(plane!=3u){const bool carry=(mask&0x80u)!=0u;mask=g2rol1(mask);if(carry)++dst0;}}}
    else{const u8 key=g2r8(b,src++);store_le32(dg(m,0xF009u),rows);const u32 bytes=static_cast<u8>(doubled);for(u32 plane=0;plane<4u;++plane){g2mask(b,mask);u32 dst=dst0;u16 rc=rows;do{u32 n=bytes;do{const u8 v=g2r8(b,src++);if(v!=key)g2w8(b,dst,g2r8(b,table+v));++dst;}while(--n!=0u);dst+=skip;}while(--rc!=0u);if(plane!=3u){const bool carry=(mask&0x80u)!=0u;mask=g2rol1(mask);if(carry)++dst0;}}}
}
static_assert(0x2A6B0u-0x2A584u==300u);

static void g2_clipped_sprite(Machine& m,Graphics2Boundary& b,u32 frame,u32 y,u32 x,u32 clip,u32 image,u8 delta,bool subtract) noexcept {
    auto w=[&](u32 o,u32 v){store_le32(dg(m,o),v);};auto r=[&](u32 o){return load_le32(dg(m,o));};
    w(0xF029u,0u);w(0xF02Du,0u);w(0xF00Du,g2r32(b,clip)>>2);w(0xF011u,g2r32(b,clip+4u)>>2);w(0xF015u,g2r32(b,clip+8u));w(0xF019u,(g2r32(b,clip+0x0Cu)+3u)>>2);
    u32 q=g2r32(b,clip+0x10u);w(0xF025u,q&3u);if(q!=0u){w(0xF029u,1u);q>>=2;}w(0xF01Du,q);
    q=(g2r32(b,clip+0x14u)+r(0xF025u))>>2;
    if(q==0u){q=1u;if(r(0xF029u)!=1u){w(0xF02Du,4u-g2r32(b,clip+0x14u));w(0xF019u,r(0xF019u)-1u);}}
    w(0xF021u,q);w(0xF025u,g2r32(b,clip+0x0Cu)&3u);
    const Planar2 p=g2planar(b,y,x);u32 dst0=p.addr;const u32 rot=(p.plane+4u-r(0xF025u))&3u;u8 mask=0x11u;
    if(subtract)mask=static_cast<u8>(mask<<rot);else for(u32 i=0;i<rot;++i)mask=g2rol1(mask);
    u32 src=image+8u+static_cast<u32>(g2r16(b,image))*frame;const u8 key=(g2r8(b,image+7u)&0x80u)?g2r8(b,src++):0xFFu;const u32 plane_stride=static_cast<u32>(g2r16(b,image))>>2;
    u32 planes=4u,lead=r(0xF02Du);
    if(subtract){if(lead!=0u){src+=plane_stride;--planes;mask=g2rol1(mask);}}
    else{while(lead!=0u){src+=plane_stride;--lead;--planes;mask=g2rol1(mask);}}
    w(0xF031u,planes);u32 dst_skip=load_le32(dg(m,0xEFF9u))-r(0xF021u)-r(0xF01Du);g2mask(b,mask);store_le32(dg(m,0xF009u),r(0xF015u));
    for(;;){
        u32 dst=dst0;u32 rows=subtract?static_cast<u16>(r(0xF015u)):r(0xF015u);
        do{src+=r(0xF00Du);src+=r(0xF019u);u32 n=r(0xF021u);do{const u8 v=g2r8(b,src++);if(v!=key)g2w8(b,dst,subtract?static_cast<u8>(v-delta):v);++dst;}while(--n!=0u);const u32 tail=r(0xF01Du);dst+=tail;src+=tail;dst+=dst_skip;}while(--rows!=0u);
        src+=r(0xF011u);--planes;w(0xF031u,planes);if(planes==0u)break;const bool carry=(mask&0x80u)!=0u;mask=g2rol1(mask);
        if(carry){
            ++dst0;
            if(r(0xF019u)!=0u){w(0xF019u,r(0xF019u)-1u);--dst_skip;--dst0;if(r(0xF029u)!=0u)w(0xF01Du,r(0xF01Du)+1u);else w(0xF021u,r(0xF021u)+1u);}
            else if(r(0xF029u)!=0u){w(0xF01Du,r(0xF01Du)+1u);w(0xF021u,r(0xF021u)-1u);if(r(0xF021u)==0u)break;}
        }
        g2mask(b,mask);
    }
}
void function_2a6b0(Machine& m,Graphics2Boundary& b,u32 frame,u32 y,u32 x,u32 clip,u32 image) noexcept {g2_clipped_sprite(m,b,frame,y,x,clip,image,0u,false);}
static_assert(0x2A89Fu-0x2A6B0u==495u);
void function_2a8a0(Machine& m,Graphics2Boundary& b,u32 frame,u32 y,u32 x,u32 clip,u32 image,u32 delta) noexcept {g2_clipped_sprite(m,b,frame,y,x,clip,image,static_cast<u8>(delta),true);}
static_assert(0x2AA9Bu-0x2A8A0u==507u);

// -----------------------------------------------------------------------------
// Batch 03: 006C0..011A4 application map/search/render helpers.
// -----------------------------------------------------------------------------

struct Application03Boundary {
    u32 (*call_2acdc)(u32)=nullptr;
    u32 (*call_2ad18)(u32)=nullptr;
    u32 (*call_2ade0)(u32)=nullptr;
    void (*call_2975c)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_2df2c)(u32*,u32*,u32,u32)=nullptr;
};

// 006C0..006FC: derive the two signed camera-relative coordinates and clamp
// each stored coordinate to -40. EAX's unclamped second value is not used by
// any known caller, so only the encoded memory side effects are exposed.
[[nodiscard]] u32 function_006c0(Machine& m) noexcept {
    s32 x=static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x255D2u))))-0x18;
    const s32 y=static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x255D6u))))-0x18;
    store_le32(dg(m,0x12740u),static_cast<u32>(y));
    if(x<-40)x=-40;
    if(static_cast<s32>(load_le32(dg(m,0x12740u)))<-40)
        store_le32(dg(m,0x12740u),static_cast<u32>(-40));
    store_le32(dg(m,0x12744u),static_cast<u32>(x));
    return static_cast<u32>(y);
}
static_assert(0x006FDu-0x006C0u==61u);

// 00700..00791: search either the primary 2223A/2223E record set or the
// alternate list rooted at 6048, selected by byte 5FCB. The 6048 base is
// snapshotted and written back on every return path exactly as encoded.
[[nodiscard]] s32 function_00700(Machine& m,s32 key6,s32 key2,s32 start) noexcept {
    const u32 base=load_le32(dg(m,0x6048u));
    s32 i=start;
    if(*dg(m,0x5FCBu)!=0u) {
        u32 off=static_cast<u32>(i)*0x16u;
        while(i<static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x258A6u))))) {
            if(static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x2223Eu+off))))==key6 &&
               static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x2223Au+off))))==key2) {
                store_le32(dg(m,0x6048u),base);
                return i;
            }
            off+=0x16u;
            ++i;
        }
    } else {
        u32 off=static_cast<u32>(i)*0x16u;
        while(i<static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x604Cu))))) {
            const u32 p=base+off;
            if(static_cast<s32>(static_cast<s16>(load_le16(dg(m,p+6u))))==key6 &&
               static_cast<s32>(static_cast<s16>(load_le16(dg(m,p+2u))))==key2) {
                store_le32(dg(m,0x6048u),base);
                return i;
            }
            off+=0x16u;
            ++i;
        }
    }
    store_le32(dg(m,0x6048u),base);
    return -1;
}
static_assert(0x00792u-0x00700u==146u);

// 00794..007E1: when 5FCB is set, search the 1761A/1761E 22-byte record set.
[[nodiscard]] s32 function_00794(Machine& m,s32 key1e,s32 key1a,s32 start) noexcept {
    if(*dg(m,0x5FCBu)==0u)return -1;
    s32 i=start;
    u32 off=static_cast<u32>(i)*0x16u;
    while(i<static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x258A4u))))) {
        if(static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x1761Eu+off))))==key1e &&
           static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x1761Au+off))))==key1a)
            return i;
        off+=0x16u;
        ++i;
    }
    return -1;
}
static_assert(0x007E2u-0x00794u==78u);

// 007E4..00838: search type-54h records in the primary 2223A/2223E table.
[[nodiscard]] s32 function_007e4(Machine& m,s32 key3e,s32 key3a,s32 start) noexcept {
    s32 i=start;
    u32 off=static_cast<u32>(i)*0x16u;
    while(i<static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x258A6u))))) {
        if(load_le16(dg(m,0x22246u+off))==0x54u &&
           static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x2223Eu+off))))==key3e &&
           static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x2223Au+off))))==key3a)
            return i;
        off+=0x16u;
        ++i;
    }
    return -1;
}
static_assert(0x00839u-0x007E4u==85u);

// 0083C..00930: filtered type search. Admission is controlled by 2ACDC and
// F03C/F03E, then by table bit 40h plus the encoded +/-10 coordinate test.
[[nodiscard]] s32 function_0083c(Machine& m,Application03Boundary& b,s32 key86,s32 key82,s32 start) noexcept {
    s32 i=start;
    u32 off=static_cast<u32>(i)*0x16u;
    while(i<static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x258A2u))))) {
        bool found=false;
        if((*dg(m,0x14D8Bu+off)&2u)==0u) {
            const u32 type=load_le16(dg(m,0x14D8Eu+off));
            if((b.call_2acdc?b.call_2acdc(type):0u)!=0u &&
               static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x14D86u+off))))==key86 &&
               static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x14D82u+off))))==key82) {
                if(load_le16(dg(m,0xF03Eu))!=0u) {
                    found=true;
                } else if(load_le16(dg(m,0xF03Cu))!=0u) {
                    const u32 p=load_le32(dg(m,0x22230u))+type*0x45u;
                    found=(*dg(m,p+0x3Au)&0x20u)!=0u;
                } else if((*dg(m,0x198E0u+static_cast<u32>(load_le16(dg(m,0x14D8Cu+off)))*2u)&0x40u)!=0u) {
                    s32 dx=static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x14D86u+off))))-
                           static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x255D2u))));
                    if(dx<0)dx=-dx;
                    s32 dy=static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x14D82u+off))))-
                           static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x255D6u))));
                    if(dy<0)dy=-dy;
                    found=dx<=10&&dy<=10;
                }
            }
        }
        if(found)return i;
        off+=0x16u;
        ++i;
    }
    return -1;
}
static_assert(0x00931u-0x0083Cu==245u);

// 00934..009AF: alternate type-predicate record search using 2AD18/2ADE0.
[[nodiscard]] s32 function_00934(Machine& m,Application03Boundary& b,s32 key86,s32 key82,s32 start) noexcept {
    s32 i=start;
    u32 off=static_cast<u32>(i)*0x16u;
    while(i<static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x258A2u))))) {
        if((*dg(m,0x14D8Bu+off)&2u)==0u) {
            const u32 type=load_le16(dg(m,0x14D8Eu+off));
            const bool admitted=(b.call_2ad18&&b.call_2ad18(type)!=0u)||
                                (b.call_2ade0&&b.call_2ade0(type)!=0u);
            if(admitted &&
               static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x14D86u+off))))==key86 &&
               static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x14D82u+off))))==key82)
                return i;
        }
        off+=0x16u;
        ++i;
    }
    return -1;
}
static_assert(0x009B0u-0x00934u==124u);

// 009B0..00B6B: classify a grid cell from the four orthogonal neighbors.
// EAX tracks neighbors below 20h, EDX tracks 2Eh/2Fh neighbors; the current
// cell byte selects special return codes or the dword table at 5FF0.
[[nodiscard]] u32 function_009b0(Machine& m,u32 cell,s32 col,s32 row) noexcept {
    u32 low_mask=0u;
    u32 slash_mask=0u;
    if(col!=0) {
        const u8 v=*dg(m,cell+0x175Fu);
        if(v<0x20u)low_mask=1u;
        if(v==0x2Fu||v==0x2Eu)slash_mask=1u;
    }
    const s32 width=static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x12756u))));
    if(width-1>col) {
        const u8 v=*dg(m,cell+0x1761u);
        if(v<0x20u)low_mask+=8u;
        if(v==0x2Fu||v==0x2Eu)slash_mask+=8u;
    }
    if(row!=0) {
        const u32 p=cell-static_cast<u32>(width);
        const u8 v=*dg(m,p+0x1760u);
        if(v<0x20u)low_mask+=2u;
        if(v==0x2Fu||v==0x2Eu)slash_mask+=2u;
    }
    const s32 height=static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x12754u))));
    if(height-1>row) {
        const u8 v=*dg(m,cell+static_cast<u32>(width)+0x1760u);
        if(v<0x20u)low_mask+=4u;
        if(v==0x2Fu||v==0x2Eu)slash_mask+=4u;
    }
    const u8 kind=*dg(m,cell+0x1760u);
    if(kind==1u)return load_le32(dg(m,0x5FF0u+low_mask*4u));
    if(kind==2u) {
        if(low_mask==6u)return 0x29u;
        if(low_mask==9u)return 0x2Bu;
        return low_mask;
    }
    if(kind==3u) {
        if(slash_mask==1u)return 0x21u;
        if(slash_mask==2u)return 0x23u;
        if(slash_mask==4u)return 0x22u;
        if(slash_mask==8u)return 0x20u;
        if((slash_mask&9u)!=0u)return 0x20u;
        if((slash_mask&6u)!=0u)return 0x22u;
        return low_mask;
    }
    if(kind==4u) {
        if(low_mask==6u)return 0x28u;
        if(low_mask==9u)return 0x2Au;
        return low_mask;
    }
    if(kind==5u) {
        if((slash_mask&6u)==6u||(slash_mask&9u)==9u)return 0x33u;
        return load_le32(dg(m,0x5FF0u+low_mask*4u));
    }
    return low_mask;
}
static_assert(0x00B6Cu-0x009B0u==444u);

// 00B6C..00BF9: derive a tile graphic code, temporarily install its two
// palette bytes at 764D/7651, draw it through 2975C, then restore B1h/B5h.
void function_00b6c(Machine& m,Application03Boundary& b,s32 col,s32 row,s32 draw0,s32 draw1) noexcept {
    const s32 width=static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x12756u))));
    const u32 cell=static_cast<u32>(col)+static_cast<u32>(width)*static_cast<u32>(row);
    u32 code=*dg(m,cell+0x1760u);
    if(code==0xFFu)return;
    if(code<0x20u)code=function_009b0(m,cell,col,row);
    const u8 palette_index=static_cast<u8>(load_le16(dg(m,0x198E0u+cell*2u)))&0x3Fu;
    *dg(m,0x764Du)=*dg(m,0x25822u+static_cast<u32>(palette_index)*2u);
    *dg(m,0x7651u)=*dg(m,0x25823u+static_cast<u32>(palette_index)*2u);
    if(b.call_2975c)b.call_2975c(code,static_cast<u32>(draw0),static_cast<u32>(draw1),load_le32(dg(m,0x258DCu))+0x34u,0u);
    *dg(m,0x764Du)=0xB1u;
    *dg(m,0x7651u)=0xB5u;
}
static_assert(0x00BFAu-0x00B6Cu==142u);

// 00BFC..00CAF: draw marker 34h, obtain two fixed-point deltas from 2DF2C,
// advance both accumulators eight times, then draw a line to the signed-word
// endpoints when the encoded lower/upper-bound tests pass.
void function_00bfc(Machine& m,Application03Boundary& b,Graphics2Boundary& g,s32 a0,s32 a1) noexcept {
    if(b.call_2975c)b.call_2975c(0x34u,static_cast<u32>(a0)-2u,static_cast<u32>(a1),load_le32(dg(m,0x258DCu))+0x34u,0u);
    u32 step0=0u,step4=0u;
    if(b.call_2df2c)b.call_2df2c(&step4,&step0,0x10000u,load_le32(dg(m,0xC658u)));
    u32 acc8=static_cast<u32>(static_cast<u16>(a1))<<16;
    u32 accc=static_cast<u32>(static_cast<u16>(a0))<<16;
    for(u32 i=0;i<8u;++i){accc+=step4;acc8+=step0;}
    const s32 end0=static_cast<s32>(static_cast<s16>(accc>>16));
    const s32 end1=static_cast<s32>(static_cast<s16>(acc8>>16));
    if(end0<0||end1<0||end0>0x18F)return;
    function_29cec(m,g,a0,a1,end0,end1,0xFFu);
}
static_assert(0x00CB0u-0x00BFCu==180u);

static s32 application03_div3333(u16 v) noexcept { return static_cast<s32>(v)/0x3333; }
static s32 application03_div2000(u16 v) noexcept { return static_cast<s32>(v)/0x2000; }
static s32 application03_add_wrap(s32 a,s32 b) noexcept { return static_cast<s32>(static_cast<u32>(a)+static_cast<u32>(b)); }

// 00CB0..010C2: render all record families occupying one map coordinate. The
// four search families retain distinct sprite codes and record layouts; every
// 2975C call reloads 258DC, matching the binary across opaque draw calls.
void function_00cb0(Machine& m,Application03Boundary& b,Graphics2Boundary& g,s32 key6,s32 key2,s32 base_coord,s32 add_coord) noexcept {
    const s32 width=static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x12756u))));
    const u32 cell=static_cast<u32>(key6)+static_cast<u32>(width)*static_cast<u32>(key2);
    const s32 base_minus1=static_cast<s32>(static_cast<u32>(base_coord)-1u);
    if(*dg(m,cell+0x1760u)==0xFFu)return;
    auto draw=[&](u32 sprite,s32 p0,s32 p1) noexcept {
        if(b.call_2975c)b.call_2975c(sprite,static_cast<u32>(p0),static_cast<u32>(p1),load_le32(dg(m,0x258DCu))+0x34u,0u);
    };
    s32 i=function_00700(m,key6,key2,0);
    while(i!=-1) {
        const u32 off=static_cast<u32>(i)*0x16u;
        u32 sprite=0x38u;
        s32 p0=0,p1=0;
        if(*dg(m,0x5FCBu)!=0u) {
            const u32 type=load_le16(dg(m,0x22246u+off));
            if(type>=0x3Cu&&type<=0x43u)sprite=0x35u;
            else if(type==0xADu)sprite=0x31u;
            p0=application03_add_wrap(application03_div3333(load_le16(dg(m,0x22238u+off))),add_coord);
            p1=application03_add_wrap(application03_div2000(load_le16(dg(m,0x2223Cu+off))),base_minus1);
        } else {
            const u32 p=load_le32(dg(m,0x6048u))+off;
            const u32 alt_type=load_le16(dg(m,p+0x0Eu));
            if(alt_type>=0x3Cu&&alt_type<=0x43u)sprite=0x35u;
            else if(load_le16(dg(m,0x22246u+off))==0xADu)sprite=0x31u;
            p0=application03_add_wrap(application03_div3333(load_le16(dg(m,p))),add_coord);
            p1=application03_add_wrap(application03_div2000(load_le16(dg(m,p+4u))),base_minus1);
        }
        draw(sprite,p1,p0);
        i=function_00700(m,key6,key2,i+1);
    }
    if(*dg(m,0x5FCBu)!=0u) {
        i=function_00794(m,key6,key2,0);
        while(i!=-1) {
            const u32 off=static_cast<u32>(i)*0x16u;
            const s32 p0=application03_add_wrap(application03_div3333(load_le16(dg(m,0x14D80u+off))),add_coord);
            const s32 p1=application03_add_wrap(application03_div2000(load_le16(dg(m,0x14D84u+off))),base_minus1);
            draw(0x32u,p1,p0);
            i=function_00794(m,key6,key2,i+1);
        }
        i=function_007e4(m,key6,key2,0);
        while(i!=-1) {
            const u32 off=static_cast<u32>(i)*0x16u;
            const s32 p0=application03_add_wrap(application03_div3333(load_le16(dg(m,0x22238u+off))),add_coord);
            const s32 p1=application03_add_wrap(application03_div2000(load_le16(dg(m,0x2223Cu+off))),base_minus1);
            draw(0x30u,p1,p0);
            i=function_007e4(m,key6,key2,i+1);
        }
    }
    i=function_00934(m,b,key6,key2,0);
    while(i!=-1) {
        const u32 off=static_cast<u32>(i)*0x16u;
        const s32 p0=application03_add_wrap(application03_div3333(load_le16(dg(m,0x14D80u+off))),add_coord);
        const s32 p1=application03_add_wrap(application03_div2000(load_le16(dg(m,0x14D84u+off))),base_minus1);
        draw(0x37u,p1,p0);
        i=function_00934(m,b,key6,key2,i+1);
    }
    if(*dg(m,0x5FCBu)!=0u) {
        i=function_0083c(m,b,key6,key2,0);
        while(i!=-1) {
            const u32 off=static_cast<u32>(i)*0x16u;
            const s32 p0=application03_add_wrap(application03_div3333(load_le16(dg(m,0x14D80u+off))),add_coord);
            const s32 p1=application03_add_wrap(application03_div2000(load_le16(dg(m,0x14D84u+off))),base_minus1);
            draw(0x36u,p1,p0);
            i=function_0083c(m,b,key6,key2,i+1);
        }
        if(static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x255D2u))))==key6 &&
           static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x255D6u))))==key2) {
            const s32 p0=application03_add_wrap(application03_div3333(load_le16(dg(m,0x255D4u))),add_coord);
            const s32 p1=application03_add_wrap(application03_div2000(load_le16(dg(m,0x255D0u))),base_coord);
            function_00bfc(m,b,g,p1,p0);
        }
    }
}
static_assert(0x010C3u-0x00CB0u==1043u);

// 010C4..011A4: fixed frame-decoration draw list. 258DC is deliberately
// reloaded before each opaque 2975C call.
void function_010c4(Machine& m,Application03Boundary& b) noexcept {
    auto draw=[&](u32 a0,u32 a1,u32 a2,u32 offset) noexcept {
        if(b.call_2975c)b.call_2975c(a0,a1,a2,load_le32(dg(m,0x258DCu))+offset,0u);
    };
    draw(0u,0u,0x67u,0x3Cu);
    draw(1u,0x170u,0x67u,0x3Cu);
    draw(0u,0xA4u,0u,0x3Du);
    draw(1u,0xA4u,0xE4u,0x3Du);
    draw(0u,8u,0x75u,0x36u);
    draw(1u,0x17Au,0x75u,0x36u);
    draw(2u,0xC0u,4u,0x36u);
    draw(3u,0xC0u,0xE9u,0x36u);
}
static_assert(0x011A5u-0x010C4u==225u);


// -----------------------------------------------------------------------------
// Batch 04: 011A8..021D7 text/map UI, tile-classification, and editor helpers.
// Every routine below was reconciled directly against fresh CODE.2 object-1
// disassembly; address-named calls remain explicit until separately proved.
// -----------------------------------------------------------------------------

struct Application04Boundary {
    void (*call_296c0)(u32,u32,u32,u32)=nullptr;
    void (*call_291b0)(u32)=nullptr;
    void (*call_2978c)(u32,u32,u32,u32,u32)=nullptr;
    u32 (*call_2f29c)(u32,u32)=nullptr;
    s32 (*call_302cc)(u32,u32,u32)=nullptr;
    void (*call_29804)(u32,u32,u32,u32)=nullptr;
    void (*call_1c368)(u32,u32)=nullptr;
    void (*call_30508)(u32,u32,u32,u32,u32)=nullptr;
    u32 (*call_1e8c8)(u32,u32)=nullptr;
    u32 (*call_1e930)(u32)=nullptr;
    u32 (*call_30700)()=nullptr;
    void (*call_290d0)(u32)=nullptr;
    u32 (*call_30ca2)(u32,u32,u32,u32)=nullptr;
    void (*call_3060c)(u32,u32,u32,u32)=nullptr;
    s32 (*call_3058c)()=nullptr;
    s32 (*call_30594)()=nullptr;
    void (*call_304f4)(u32,u32)=nullptr;
    void (*call_30354)(s32,u32)=nullptr;
    s32 (*call_30318)(s32,u32)=nullptr;
    u32 (*call_30aec)()=nullptr;
    void (*call_1a8c8)(u32,u32,u32,u32,u32)=nullptr;
    u32 (*call_1a1f4)()=nullptr;
    void (*call_1a510)(u32)=nullptr;
    void (*call_1aacc)()=nullptr;
    void (*call_2e590)(u32)=nullptr;
    void (*call_2a020)(u32)=nullptr;
    void (*call_2e5d4)()=nullptr;
    void (*call_1a288)()=nullptr;
    u32 (*call_30070)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_30040)(u32)=nullptr;
    void (*call_22d70)()=nullptr;
};

[[nodiscard]] static s32 app04_add_s32(s32 a,s32 b) noexcept {
    return static_cast<s32>(static_cast<u32>(a)+static_cast<u32>(b));
}
[[nodiscard]] static u32 app04_mask_low_byte(u32 v,u8 mask) noexcept {
    return (v&0xFFFFFF00u)|static_cast<u32>(static_cast<u8>(v)&mask);
}

void function_011a8(Machine& m,Application04Boundary& b) noexcept {
    if(b.call_296c0)b.call_296c0(6u,4u,0x189u,0xEFu);
    if(b.call_291b0)b.call_291b0(*dg(m,0xC050u));
    for(u32 x=0u;x!=0x120u;x+=0x60u) {
        if(b.call_2978c)b.call_2978c(0u,0u,x,load_le32(dg(m,0x258DCu))+0x3Eu,0u);
        if(b.call_2978c)b.call_2978c(0u,0xC8u,x,load_le32(dg(m,0x258DCu))+0x3Eu,0u);
    }
    if(b.call_296c0)b.call_296c0(0u,0u,0x18Fu,0x13Fu);
}
static_assert(0x01227u-0x011A8u==127u);

void function_01228(Machine& m,Application04Boundary& b,s32 grid_x,s32 grid_y) noexcept {
    const s32 x0=static_cast<s32>(static_cast<u32>(grid_x)*8u);
    const s32 y0=static_cast<s32>(static_cast<u32>(grid_y)*5u);
    const s32 x1=static_cast<s32>(static_cast<u32>(x0)+0x184u);
    const s32 y1=static_cast<s32>(static_cast<u32>(y0)+0xECu);
    const u32 font=b.call_2f29c?b.call_2f29c(1u,0u):0u;
    if(b.call_296c0)b.call_296c0(6u,4u,0x189u,0xEFu);
    u32 cursor=0x5760u;
    for(;;) {
        const u32 header=cursor;
        cursor+=6u;
        const s32 len=static_cast<s32>(static_cast<s16>(load_le16(dg(m,header+4u))));
        if(len==0)break;
        const s32 rx=static_cast<s32>(static_cast<s16>(load_le16(dg(m,header))));
        const s32 ry=static_cast<s32>(static_cast<s16>(load_le16(dg(m,header+2u))));
        const s32 extent=b.call_302cc?b.call_302cc(cursor,1u,static_cast<u32>(len)):0;
        bool visible=rx>=x0&&rx<=x1;
        if(visible&&(ry<y0||ry>y1)) {
            const s32 end=static_cast<s32>(static_cast<u32>(ry)+static_cast<u32>(extent));
            visible=end>=y0&&end<=y1;
        }
        if(visible) {
            s32 draw_x=static_cast<s32>(static_cast<u32>(rx)-static_cast<u32>(x0));
            s32 draw_y=static_cast<s32>(static_cast<u32>(ry)-static_cast<u32>(y0));
            const s32 reset_y=draw_y;
            for(s32 i=0;i<len;++i) {
                const u8 ch=*dg(m,cursor+static_cast<u32>(i));
                if(ch==0x0Du) {
                    draw_x=static_cast<s32>(static_cast<u32>(draw_x)+0x0Cu);
                    draw_y=reset_y;
                } else {
                    if(b.call_29804)b.call_29804(ch,static_cast<u32>(draw_x),static_cast<u32>(draw_y),load_le32(dg(m,font)));
                    draw_y=static_cast<s32>(static_cast<u32>(draw_y)+*dg(m,font+static_cast<u32>(ch)+4u));
                }
            }
        }
        cursor+=static_cast<u32>(len);
    }
    if(b.call_296c0)b.call_296c0(0u,0u,0x18Fu,0x13Fu);
}
static_assert(0x01399u-0x01228u==369u);

void function_0139c(Machine& m,Application04Boundary& b,Application03Boundary& a3,Graphics2Boundary& g,
                     s32 col,s32 row,s32 cols,s32 rows,u32 decorate) noexcept {
    function_011a8(m,b);
    const s32 end_col=app04_add_s32(col,cols);
    const s32 end_row=app04_add_s32(row,rows);
    for(u32 cbits=static_cast<u32>(col);static_cast<s32>(cbits)<end_col;++cbits) {
        const s32 c=static_cast<s32>(cbits);
        if(c<0||c>=static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x12756u)))))continue;
        for(u32 rbits=static_cast<u32>(row);static_cast<s32>(rbits)<end_row;++rbits) {
            const s32 r=static_cast<s32>(rbits);
            if(r>=0&&r<static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x12754u)))))
                function_00b6c(m,a3,c,r,6,4);
        }
    }
    for(u32 cbits=static_cast<u32>(col);static_cast<s32>(cbits)<end_col;++cbits) {
        const s32 c=static_cast<s32>(cbits);
        if(c<0||c>=static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x12756u)))))continue;
        for(u32 rbits=static_cast<u32>(row);static_cast<s32>(rbits)<end_row;++rbits) {
            const s32 r=static_cast<s32>(rbits);
            if(r>=0&&r<static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x12754u)))))
                function_00cb0(m,a3,g,c,r,6,4);
        }
    }
    if(*dg(m,0x5FD9u)!=0u)function_01228(m,b,col,row);
    if(decorate!=0u) {
        if(*dg(m,0x6058u)==0u&&b.call_1c368)
            b.call_1c368(0x6058u,load_le32(dg(m,0x1274Cu))+0x26Du);
        if(b.call_30508)b.call_30508(0x26u,4u,0xECu,0x6058u,1u);
        function_010c4(m,a3);
    }
}
static_assert(0x01557u-0x0139Cu==443u);

[[nodiscard]] static bool app04_empty_neighbor(Machine& m,u32 index) noexcept {
    const u32 a=0x198E0u+index*2u;
    return (*dg(m,a+1u)&4u)!=0u&&(*dg(m,a)&0x3Fu)==0u;
}

void function_01558(Machine& m,Application04Boundary& b) noexcept {
    store_le16(dg(m,0x12756u),load_le16(dg(m,0x25619u)));
    store_le16(dg(m,0x12754u),load_le16(dg(m,0x2561Bu)));
    for(s32 col=0;col<static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x12756u))));++col) {
        for(s32 row=0;row<static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x12754u))));++row) {
            const s32 width=static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x12756u))));
            const s32 height=static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x12754u))));
            const u32 index=static_cast<u32>(width)*static_cast<u32>(row)+static_cast<u32>(col);
            const u32 tile=0x198E0u+index*2u;
            u32 code=0xFFu;
            const bool player=row==static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x255D6u))))&&
                              col==static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x255D2u))));
            if(player) {
                code=(*dg(m,0x258A8u)==0xFFu)?0x2Fu:0x2Eu;
            } else if((*dg(m,tile+1u)&4u)!=0u) {
                const u32 low=*dg(m,tile)&0x3Fu;
                if(low==0u)code=(*dg(m,0x258A8u)==0xFFu)?0x2Fu:0x2Eu;
                else {
                    const s32 type=static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x25622u+low*2u))));
                    if(type==0x93)code=(*dg(m,0x258A8u)==0xFFu)?0x2Fu:0x2Eu;
                    else if((*dg(m,tile+1u)&8u)!=0u)code=5u;
                    else if((*dg(m,tile)&0x80u)==0u)code=1u;
                    else {
                        const u32 rec=b.call_1e8c8?b.call_1e8c8(static_cast<u32>(col),static_cast<u32>(row)):0u;
                        if(rec==0u)code=3u;
                        else code=(static_cast<u16>(b.call_1e930?b.call_1e930(index):0u)==0x8000u)?4u:2u;
                    }
                }
            } else {
                const u32 low=*dg(m,tile)&0x3Fu;
                if(low!=0u) {
                    const s32 type=static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x25622u+low*2u))));
                    if(type==0x93)code=(*dg(m,0x258A8u)==0xFFu)?0x2Fu:0x2Eu;
                    else {
                        bool open=false;
                        if(col>0) {
                            if(row>0&&app04_empty_neighbor(m,index-1u-static_cast<u32>(width)))open=true;
                            if(app04_empty_neighbor(m,index-1u))open=true;
                            if(row<height-1&&app04_empty_neighbor(m,index-1u+static_cast<u32>(width)))open=true;
                        }
                        if(row>0&&app04_empty_neighbor(m,index-static_cast<u32>(width)))open=true;
                        if(row<height-1&&app04_empty_neighbor(m,index+static_cast<u32>(width)))open=true;
                        if(col<width-1) {
                            if(row>0&&app04_empty_neighbor(m,index+1u-static_cast<u32>(width)))open=true;
                            if(app04_empty_neighbor(m,index+1u))open=true;
                            if(row<height-1&&app04_empty_neighbor(m,index+1u+static_cast<u32>(width)))open=true;
                        }
                        if(open)code=1u;
                    }
                }
            }
            *dg(m,0x1760u+index)=static_cast<u8>(code);
        }
    }
}
static_assert(0x01831u-0x01558u==729u);

[[nodiscard]] static u32 app04_scale15_4(u32 v) noexcept { return (v*4u)>>15; }
[[nodiscard]] static u32 app04_scale15_2(u32 v) noexcept { return (v*2u)>>15; }

void function_01834(Machine& m,Application04Boundary& b,Application03Boundary& a3) noexcept {
    if(b.call_291b0)b.call_291b0(*dg(m,0xC050u));
    for(u32 x=0u;x!=0x120u;x+=0x60u) {
        if(a3.call_2975c)a3.call_2975c(0u,0u,x,load_le32(dg(m,0x258DCu))+0x3Eu,0u);
        if(a3.call_2975c)a3.call_2975c(0u,0xC8u,x,load_le32(dg(m,0x258DCu))+0x3Eu,0u);
    }
    const u32 r0=b.call_30700?b.call_30700():0u;
    store_le32(dg(m,0x12748u),r0);
    if(b.call_290d0)b.call_290d0(load_le16(dg(m,0x12748u)));
    for(u32 y=0xF0u;y<0x140u;y+=8u)for(u32 x=0u;x<0x190u;x+=0x10u) {
        const u32 p=load_le32(dg(m,0x258DCu))+0x35u;
        const u32 r=b.call_30ca2?b.call_30ca2(x,y,p,0u):0u;
        if(a3.call_2975c)a3.call_2975c(x,y,p,0u,app04_scale15_4(r));
    }
    u32 y=0x1Au,label=0xE2u;
    for(u32 i=0u;i<0x0Du;++i,++label) {
        if(i==9u&&*dg(m,0x258A8u)!=0xFFu)continue;
        if(b.call_1c368)b.call_1c368(0x769Cu,label);
        const u32 p=load_le32(dg(m,0x258DCu))+0x34u;
        if(a3.call_2975c)a3.call_2975c(*dg(m,0x5FCCu+i),y,0xF6u,p,0u);
        if(b.call_3060c)b.call_3060c(y,0xFFu,0x769Cu,1u);
        y+=0x0Au;
    }
    const u32 r1=b.call_30700?b.call_30700():0u;
    store_le32(dg(m,0x12750u),r1);
    if(b.call_290d0)b.call_290d0(load_le16(dg(m,0x12750u)));
    for(u32 x=8u;x<0x138u;x+=8u) {
        u32 p=load_le32(dg(m,0x258DCu))+0x35u;
        u32 r=b.call_30ca2?b.call_30ca2(0u,x,p,0u):0u;
        if(a3.call_2975c)a3.call_2975c(0u,x,p,0u,app04_scale15_2(r)+9u);
        p=load_le32(dg(m,0x258DCu))+0x35u;
        r=b.call_30ca2?b.call_30ca2(0x180u,x,p,0u):0u;
        if(a3.call_2975c)a3.call_2975c(0x180u,x,p,0u,app04_scale15_2(r)+5u);
    }
    for(u32 x=0x10u;x<0x180u;x+=0x10u) {
        u32 p=load_le32(dg(m,0x258DCu))+0x35u;
        u32 r=b.call_30ca2?b.call_30ca2(x,0u,p,0u):0u;
        if(a3.call_2975c)a3.call_2975c(x,0u,p,0u,app04_scale15_2(r)+0x0Cu);
        p=load_le32(dg(m,0x258DCu))+0x35u;
        r=b.call_30ca2?b.call_30ca2(x,0xF0u,p,0u):0u;
        if(a3.call_2975c)a3.call_2975c(x,0xF0u,p,0u,app04_scale15_2(r)+0x12u);
        p=load_le32(dg(m,0x258DCu))+0x35u;
        r=b.call_30ca2?b.call_30ca2(x,0x138u,p,0u):0u;
        if(a3.call_2975c)a3.call_2975c(x,0x138u,p,0u,app04_scale15_2(r)+0x0Eu);
    }
    auto draw=[&](u32 a0,u32 a1,u32 a2,u32 off) noexcept {if(a3.call_2975c)a3.call_2975c(a0,a1,a2,load_le32(dg(m,0x258DCu))+off,0u);};
    draw(8u,0u,0u,0x35u); draw(0x0Bu,0u,0x138u,0x35u); draw(4u,0x180u,0u,0x35u); draw(7u,0x180u,0x138u,0x35u);
    draw(0x10u,0u,0xF0u,0x35u); draw(0x11u,0x180u,0xF0u,0x35u); draw(0u,0x11Du,0xF0u,0x37u); draw(1u,0x11Du,0x130u,0x37u);
    draw(*dg(m,0x5FD9u),0x11Au,0x100u,0x38u);
    u32 sx=0x136u; for(u32 sprite=2u;sprite!=8u;sprite+=2u,sx+=0x1Cu)draw(sprite,sx,0x100u,0x38u);
    draw(0u,3u,0xF3u,0x39u); draw(0u,0xAEu,0x120u,0x3Au); draw(0u,0xA8u,0xF8u,0x3Bu); draw(0u,0xF3u,0xF5u,0x3Fu);
}
static_assert(0x01C08u-0x01834u==980u);

[[nodiscard]] u32 function_01c08(Machine& m) noexcept {
    u32 p=0x5760u;
    if(load_le16(dg(m,0x5764u))==0u)return p;
    do {p+=6u;const s32 len=static_cast<s32>(static_cast<s16>(load_le16(dg(m,p-2u))));p+=static_cast<u32>(len);} while(load_le16(dg(m,p+4u))!=0u);
    return p;
}
static_assert(0x01C28u-0x01C08u==32u);

[[nodiscard]] u32 function_01c28(Machine& m) noexcept {
    u32 p=0x5760u;
    if(load_le16(dg(m,0x5764u))!=0u)do {p+=6u;const s32 len=static_cast<s32>(static_cast<s16>(load_le16(dg(m,p-2u))));p+=static_cast<u32>(len);} while(load_le16(dg(m,p+4u))!=0u);
    return p-0x5760u;
}
static_assert(0x01C4Du-0x01C28u==37u);

void function_01c50(Machine& m,Application04Boundary& b,u32 text,u32 style,u32 count) noexcept {
    s32 cursor0=b.call_3058c?b.call_3058c():0;
    const s32 cursor1=b.call_30594?b.call_30594():0;
    if(text==0u||*dg(m,text)==0u||count==0u)return;
    u32 p=text,remaining=count;
    while(*dg(m,p)!=0u&&remaining!=0u) {
        const s32 ch=static_cast<s32>(static_cast<s8>(*dg(m,p)));
        if(ch==0x0D) {cursor0=static_cast<s32>(static_cast<u32>(cursor0)+0x0Cu);if(b.call_304f4)b.call_304f4(static_cast<u32>(cursor0),static_cast<u32>(cursor1));}
        else if(b.call_30354)b.call_30354(ch,style);
        ++p;--remaining;
    }
}
static_assert(0x01CA8u-0x01C50u==88u);

[[nodiscard]] s32 function_01ca8(Machine& m,Application04Boundary& b,u32 text,u32 style) noexcept {
    s32 total=0;if(text==0u||*dg(m,text)==0u)return total;
    for(u32 p=text;*dg(m,p)!=0u;++p) {if(*dg(m,p)==0x0Du)total=0;else {const s32 ch=static_cast<s32>(static_cast<s8>(*dg(m,p)));const s32 w=b.call_30318?b.call_30318(ch,style):0;total=static_cast<s32>(static_cast<u32>(total)+static_cast<u32>(w));}}
    return total;
}
static_assert(0x01CE2u-0x01CA8u==58u);

void function_01ce4(Machine& m,Application04Boundary& b,Graphics2Boundary& g,u32 origin0,u32 origin1,u32 max_lines,u32 image) noexcept {
    u32 key=0u;bool redraw=true;u32 record=0x5760u;
    if(load_le16(dg(m,0x5764u))!=0u)do {record+=6u;const s32 len=static_cast<s32>(static_cast<s16>(load_le16(dg(m,record-2u))));record+=static_cast<u32>(len);} while(load_le16(dg(m,record+4u))!=0u);
    const u32 text=record+6u;s32 line=0;if(function_01c08(m)-0x5760u>=0x7EEu)return;
    const u32 xbits=static_cast<u32>(static_cast<u16>(load_le16(dg(m,0x12744u))))<<3;
    store_le16(dg(m,record),static_cast<u16>(origin0+xbits));
    const u16 ydelta=static_cast<u16>(static_cast<u16>(load_le16(dg(m,0x12740u)))*5u);
    store_le16(dg(m,record+2u),static_cast<u16>(origin1+static_cast<u32>(ydelta)));store_le16(dg(m,record+4u),0u);*dg(m,text)=0u;
    const s32 line_limit=static_cast<s32>(static_cast<u32>(max_lines)-1u);const u32 masked_origin1=app04_mask_low_byte(origin1,0xF8u);const u32 right_extent=0x140u-origin1;const u32 line_span=max_lines*0x0Cu;
    while(key!=0x1Bu) {
        if(redraw) {function_2a380(m,g,0u,origin0,masked_origin1,image);if(b.call_304f4)b.call_304f4(origin0,origin1);function_01c50(m,b,text,1u,static_cast<u32>(static_cast<s32>(static_cast<s16>(load_le16(dg(m,record+4u))))));if(b.call_30354)b.call_30354(0x7Cu,1u);if(b.call_1a8c8)b.call_1a8c8(load_le32(dg(m,0xC050u)),origin0,origin1,line_span,right_extent);redraw=false;}
        key=b.call_30aec?b.call_30aec():0u;bool mutated=false;
        if(key==8u) {const u16 len=load_le16(dg(m,record+4u));if(len!=0u){store_le16(dg(m,record+4u),static_cast<u16>(len-1u));mutated=true;}}
        else if(key==0x0Du) {if(function_01c08(m)-0x5760u<0x7EEu){if(line<line_limit){const s32 old_len=static_cast<s32>(static_cast<s16>(load_le16(dg(m,record+4u))));store_le16(dg(m,record+4u),static_cast<u16>(load_le16(dg(m,record+4u))+1u));*dg(m,text+static_cast<u32>(old_len))=0x0Du;line=app04_add_s32(line,1);mutated=true;}else key=0x1Bu;}}
        else {const u8 idx=static_cast<u8>(static_cast<u8>(key)+1u);const u8 cls=*dg(m,0x165Cu+idx);const bool accepted=(cls&0xE0u)!=0u||(cls&4u)!=0u||key==0x20u;if(accepted&&function_01c08(m)-0x5760u<0x7EEu){const s32 width=function_01ca8(m,b,text,1u);if(static_cast<s32>(static_cast<u32>(width)+origin1)<0xE3){const s32 old_len=static_cast<s32>(static_cast<s16>(load_le16(dg(m,record+4u))));store_le16(dg(m,record+4u),static_cast<u16>(load_le16(dg(m,record+4u))+1u));*dg(m,text+static_cast<u32>(old_len))=static_cast<u8>(key);mutated=true;}}}
        if(mutated){const s32 len=static_cast<s32>(static_cast<s16>(load_le16(dg(m,record+4u))));*dg(m,text+static_cast<u32>(len))=0u;redraw=true;}
    }
    if(load_le16(dg(m,record+4u))==0u){*dg(m,text)=0u;store_le16(dg(m,record+4u),0u);store_le16(dg(m,record+2u),0u);store_le16(dg(m,record),0u);}
    function_2a380(m,g,0u,origin0,masked_origin1,image);if(b.call_304f4)b.call_304f4(origin0,origin1);function_01c50(m,b,text,1u,static_cast<u32>(static_cast<s32>(static_cast<s16>(load_le16(dg(m,record+4u))))));if(b.call_1a8c8)b.call_1a8c8(load_le32(dg(m,0xC050u)),origin0,origin1,line_span,0x140u-origin1);
}
static_assert(0x01F6Bu-0x01CE4u==647u);

void function_01f6c(Machine& m,Application04Boundary& b,Application03Boundary& a3,Graphics2Boundary& g,u32 decorate) noexcept {
    if(b.call_291b0)b.call_291b0(*dg(m,0xC050u));if(b.call_290d0)b.call_290d0(load_le16(dg(m,0x12750u)));
    for(u32 x=8u;x<0xB8u;x+=8u){u32 p=load_le32(dg(m,0x258DCu))+0x35u;u32 r=b.call_30ca2?b.call_30ca2(0u,x,p,0u):0u;if(a3.call_2975c)a3.call_2975c(0u,x,p,0u,app04_scale15_2(r)+9u);p=load_le32(dg(m,0x258DCu))+0x35u;r=b.call_30ca2?b.call_30ca2(0x180u,x,p,0u):0u;if(a3.call_2975c)a3.call_2975c(0x180u,x,p,0u,app04_scale15_2(r)+5u);}
    for(u32 x=0x10u;x<0x180u;x+=0x10u){u32 p=load_le32(dg(m,0x258DCu))+0x35u;u32 r=b.call_30ca2?b.call_30ca2(x,0u,p,0u):0u;if(a3.call_2975c)a3.call_2975c(x,0u,p,0u,app04_scale15_2(r)+0x0Cu);p=load_le32(dg(m,0x258DCu))+0x35u;r=b.call_30ca2?b.call_30ca2(x,0xF0u,p,0u):0u;if(a3.call_2975c)a3.call_2975c(x,0xF0u,p,0u,app04_scale15_2(r)+0x12u);}
    function_0139c(m,b,a3,g,static_cast<s32>(load_le32(dg(m,0x12744u))),static_cast<s32>(load_le32(dg(m,0x12740u))),0x30,0x2F,decorate);if(b.call_1a510)b.call_1a510(load_le32(dg(m,0xC050u)));*dg(m,0xC050u)=static_cast<u8>(*dg(m,0xC050u)^1u);
}
static_assert(0x02079u-0x01F6Cu==269u);

void function_0207c(Machine& m,Application04Boundary& b,Application03Boundary& a3,Graphics2Boundary& g) noexcept {
    if(b.call_1a1f4)while(b.call_1a1f4()!=0u){}function_01f6c(m,b,a3,g,0u);if(b.call_291b0)b.call_291b0(*dg(m,0xC050u));
    for(;;){if(b.call_1a1f4)while(b.call_1a1f4()==0u){}const s32 x=static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x9E31u))));const s32 y=static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x9E33u))));const bool exit_rect=x>=static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x5F82u))))&&x<static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x5F86u))))&&y>=static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x5F84u))))&&y<static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x5F88u))));if(exit_rect)break;if(x<8||x>0x188||y<4||y>0xCE)continue;if(b.call_1aacc)b.call_1aacc();if(b.call_2e590)b.call_2e590(*dg(m,0xEF4Eu));if(b.call_2a020)b.call_2a020(static_cast<u8>(*dg(m,0xEF4Eu)^1u));s32 lines=(0x188-x)/0x0C;if(lines==0){if(b.call_2e5d4)b.call_2e5d4();if(b.call_1a288)b.call_1a288();continue;}if(lines>4)lines=4;const s32 aligned_y=static_cast<s32>(static_cast<u32>(y)&0xFFFFFFF8u);const u32 image=b.call_30070?b.call_30070(static_cast<u32>(x),static_cast<u32>(aligned_y),static_cast<u32>(lines*0x0C),0x140u-static_cast<u32>(aligned_y),0xFFu):0u;if(b.call_2e5d4)b.call_2e5d4();function_01ce4(m,b,g,static_cast<u32>(x),static_cast<u32>(y),static_cast<u32>(lines),image);if(b.call_30040)b.call_30040(image);if(b.call_1a288)b.call_1a288();}
    function_01f6c(m,b,a3,g,1u);if(b.call_22d70)b.call_22d70();
}
static_assert(0x021D8u-0x0207Cu==348u);

// -----------------------------------------------------------------------------
// Batch 05: 021D8..03630 modal text deletion, map export/load, control panel,
// and viewport movement. CODE.2 bytes are the controlling evidence.
// -----------------------------------------------------------------------------

struct Application05Boundary {
    Application04Boundary* ui=nullptr;
    Application03Boundary* app=nullptr;
    Graphics2Boundary* gfx=nullptr;
    void (*call_30cd4)(u32,u32,u32)=nullptr;
    void (*call_30d30)(u32,u32,u32)=nullptr;
    void (*call_30d52)(u32,u8*,u8*)=nullptr;
    u32 (*call_3104c)(u32)=nullptr;
    void (*call_31107)(u32,u32)=nullptr;
    void (*call_31304)(u32,u32,u32)=nullptr;
    void (*call_310b0)(u32)=nullptr;
    void (*call_2e6d0)()=nullptr;
    void (*call_31491)(u32)=nullptr;
    void (*call_31380)(u32)=nullptr;
    u32 (*call_312c8)(u32)=nullptr;
    void (*call_312dc)(u32)=nullptr;
    u32 (*call_312b4)(u32)=nullptr;
    u32 (*call_312a0)(u32)=nullptr;
    void (*call_3131c)(u32,u32,u32)=nullptr;
    u32 (*call_312ec_local32)(u32)=nullptr;
    void (*call_312ec_guest)(u32,u32,u32)=nullptr;
    void (*call_298f0)(u32,u32,u32,u32,u32,u32)=nullptr;
    void (*call_1f8fc)()=nullptr;
    void (*call_1ae30)()=nullptr;
};

static void app05_copy(Machine& m,Application05Boundary& b,u32 dst,u32 src,u32 count) noexcept {
    if(b.call_30cd4){b.call_30cd4(dst,src,count);return;}
    for(u32 i=0;i<count;++i)*dg(m,dst+i)=*dg(m,src+i);
}
static void app05_fill(Machine& m,Application05Boundary& b,u32 dst,u8 value,u32 count) noexcept {
    if(b.call_30d30){b.call_30d30(dst,value,count);return;}
    for(u32 i=0;i<count;++i)*dg(m,dst+i)=value;
}
static void app05_putc(Application05Boundary& b,u32 ch,u32 stream) noexcept {
    if(b.call_31107)b.call_31107(ch,stream);
}
static void app05_write(Application05Boundary& b,u32 src,u32 count,u32 sink) noexcept {
    if(b.call_31304)b.call_31304(src,count,sink);
}
static s32 app05_s16(Machine& m,u32 a) noexcept {
    return static_cast<s32>(static_cast<s16>(load_le16(dg(m,a))));
}

// 021D8..02352: wait for a mouse press, locate a packed text record under the
// cursor, delete it with the encoded forward-copy/clear arithmetic, redraw,
// and repeat until the exit rectangle is selected.
void function_021d8(Machine& m,Application05Boundary& b) noexcept {
    if(!b.ui)return;
    if(b.ui->call_1a1f4)while(b.ui->call_1a1f4()!=0u){}
    for(;;) {
        if(b.ui->call_1a1f4)while(b.ui->call_1a1f4()==0u){}
        const s32 sx=app05_s16(m,0x9E31u), sy=app05_s16(m,0x9E33u);
        const s32 ex0=app05_s16(m,0x5F8Au), ex1=app05_s16(m,0x5F8Eu);
        const s32 ey0=app05_s16(m,0x5F8Cu), ey1=app05_s16(m,0x5F90u);
        if(sx>=ex0&&sx<ex1&&sy>=ey0&&sy<ey1)break;
        if(sx<0x20||sx>0x16F||sy<0x10||sy>0xE3)continue;
        const s32 x=static_cast<s32>(static_cast<u32>(sx)+load_le32(dg(m,0x12744u))*8u);
        const s32 y=static_cast<s32>(static_cast<u32>(sy)+load_le32(dg(m,0x12740u))*5u);
        u32 selected=0u, record=0x5760u;
        for(;;) {
            const s32 len=app05_s16(m,record+4u);
            if(len==0)break;
            const u32 text=record+6u;
            const s32 rx=app05_s16(m,record), ry=app05_s16(m,record+2u);
            const s32 extent=b.ui->call_302cc?b.ui->call_302cc(text,1u,static_cast<u32>(len)):0;
            if(rx<x&&static_cast<s32>(static_cast<u32>(rx)+0x0Bu)>x&&
               ry<y&&static_cast<s32>(static_cast<u32>(ry)+static_cast<u32>(extent))>y)
                selected=record;
            record=static_cast<u32>(text+static_cast<u32>(len));
        }
        if(selected==0u)continue;
        const u32 len_bits=static_cast<u32>(app05_s16(m,selected+4u));
        const u32 terminator=function_01c08(m);
        const u32 end=selected+6u+len_bits;
        const u32 tail=(terminator-0x5760u)-(end-0x5760u);
        app05_copy(m,b,selected,end,tail);
        const u32 clear=(end-0x5760u)+selected;
        app05_fill(m,b,clear,0u,len_bits+6u);
        if(b.ui&&b.app&&b.gfx)
            function_0139c(m,*b.ui,*b.app,*b.gfx,static_cast<s32>(load_le32(dg(m,0x12744u))),
                           static_cast<s32>(load_le32(dg(m,0x12740u))),0x30,0x2F,1u);
        if(b.ui->call_1a510)b.ui->call_1a510(load_le32(dg(m,0xC050u)));
        *dg(m,0xC050u)=static_cast<u8>(*dg(m,0xC050u)^1u);
    }
    if(b.ui->call_22d70)b.ui->call_22d70();
}
static_assert(0x02353u-0x021D8u==379u);

// 02354..023B5: query the encoded 17h status frame and accept exactly the
// 80h+10h set / 20h+08h clear combination in byte one.
[[nodiscard]] u32 function_02354(Application05Boundary& b) noexcept {
    u8 frame[0x1Cu]{};
    store_le16(frame,0x0200u);
    store_le16(frame+0x0Cu,0u);
    if(b.call_30d52)b.call_30d52(0x17u,frame,frame);
    const u8 f=frame[1];
    return ((f&0x80u)!=0u&&(f&0x20u)==0u&&(f&0x10u)!=0u&&(f&0x08u)==0u)?1u:0u;
}
static_assert(0x023B6u-0x02354u==98u);

// 023B8..02472: locate the packed text record whose signed coordinates map to
// the supplied /8 and /5 cell, while advancing the binary's A..Z/A.. label.
struct App05LabelResult { u32 found=0u; u8 first=0x41u,second=0x20u; };
[[nodiscard]] static App05LabelResult app05_label_for_cell(Machine& m,s32 want_x,s32 want_y) noexcept {
    App05LabelResult r{};u32 record=0x5760u;
    for(;;) {
        const s32 len=app05_s16(m,record+4u);
        if(len==0)break;
        const s32 qx=app05_s16(m,record)/8;
        const s32 qy=app05_s16(m,record+2u)/5;
        if(qx==want_x&&qy==want_y){r.found=1u;break;}
        if(r.first==0x5Au){r.first=0x41u;if(r.second==0x20u)r.second=0x41u;else ++r.second;}
        else ++r.first;
        record=static_cast<u32>(record+6u+static_cast<u32>(len));
    }
    return r;
}
[[nodiscard]] u32 function_023b8(Machine& m,s32 want_x,s32 want_y,u32 out) noexcept {
    const App05LabelResult r=app05_label_for_cell(m,want_x,want_y);
    *dg(m,out)=r.first;*dg(m,out+1u)=r.second;return r.found;
}
static_assert(0x02473u-0x023B8u==187u);

// 02474..026E5: serialize packed text records either as character output or
// fixed-width block writes. Labels and CR/LF behavior follow the original.
void function_02474(Machine& m,Application05Boundary& b,u32 stream,u32 sink,u32 block_mode) noexcept {
    u8 first=0x41u, second=0x20u;
    const u32 buffer=b.call_3104c?b.call_3104c(0x4Fu):0u;
    u32 record=0x5760u;
    for(;;) {
        const s32 len=app05_s16(m,record+4u);
        if(len==0)break;
        const u32 text=record+6u;
        app05_fill(m,b,buffer,0x20u,0x4Fu);
        *dg(m,buffer)=first;*dg(m,buffer+1u)=second;*dg(m,buffer+2u)=0x3Au;
        if(first==0x5Au){first=0x41u;if(second==0x20u)second=0x41u;else ++second;}else ++first;
        u32 column=4u;u32 dirty=0u;
        for(s32 i=0;i<len;++i) {
            const u8 ch=*dg(m,text+static_cast<u32>(i));
            if(ch==0x0Du) {
                if(block_mode==0u) {
                    for(u32 p=buffer;p!=buffer+0x4Fu;++p)app05_putc(b,*dg(m,p),stream);
                    app05_putc(b,*dg(m,0x5F60u),stream);app05_putc(b,*dg(m,0x5F61u),stream);
                } else {
                    app05_write(b,buffer,0x4Fu,sink);app05_write(b,0x5F60u,2u,sink);
                }
                dirty=0u;app05_fill(m,b,buffer,0x20u,0x4Fu);column=4u;
            } else {
                *dg(m,buffer+column)=ch;++column;dirty=1u;
            }
        }
        if(block_mode==0u) {
            if(dirty!=0u){for(u32 p=buffer;p!=buffer+0x4Fu;++p)app05_putc(b,*dg(m,p),stream);app05_putc(b,*dg(m,0x5F60u),stream);app05_putc(b,*dg(m,0x5F61u),stream);}
            app05_putc(b,*dg(m,0x5F60u),stream);app05_putc(b,*dg(m,0x5F61u),stream);
        } else {
            if(dirty!=0u){app05_write(b,buffer,0x4Fu,sink);app05_write(b,0x5F60u,2u,sink);}
            *dg(m,buffer)=0x0Au;app05_write(b,buffer,1u,sink);
        }
        record=static_cast<u32>(record+6u+static_cast<u32>(len));
    }
    if(block_mode==0u){app05_putc(b,*dg(m,0x5F60u),stream);app05_putc(b,*dg(m,0x5F61u),stream);}
    else app05_write(b,0x5F60u,2u,sink);
    if(b.call_310b0)b.call_310b0(buffer);
}
static_assert(0x026E6u-0x02474u==626u);
// 026E8..02A33: emit the three map text planes, then append the packed text
// listing. In block mode the two overlay planes replace non-space base bytes.
void function_026e8(Machine& m,Application05Boundary& b,u32 stream,u32 sink,u32 block_mode) noexcept {
    if(block_mode==0u) {
        if(function_02354(b)==0u){if(b.call_2e6d0)b.call_2e6d0();return;}
        if(app05_s16(m,0x2561Bu)>0x50)app05_putc(b,0x0Fu,stream);
    }
    const s32 height=app05_s16(m,0x12754u);
    const u32 alloc_size=static_cast<u32>(height);
    const u32 base=b.call_3104c?b.call_3104c(alloc_size):0u;
    const u32 overlay=b.call_3104c?b.call_3104c(alloc_size):0u;
    const u32 labels=b.call_3104c?b.call_3104c(alloc_size):0u;
    for(s32 col=0;col<app05_s16(m,0x12756u);++col) {
        app05_fill(m,b,base,0x20u,alloc_size);app05_fill(m,b,overlay,0x20u,alloc_size);app05_fill(m,b,labels,0x20u,alloc_size);
        for(s32 row=0;row<height;++row) {
            const s32 width=app05_s16(m,0x12756u);
            const u32 index=static_cast<u32>(width)*static_cast<u32>(row)+static_cast<u32>(col);
            u32 code=*dg(m,0x1760u+index);
            u8 out=0x20u;
            if(code!=0xFFu) {
                if(code<0x20u)code=function_009b0(m,index,col,row);
                if(code>=0x20u&&code<=0x33u)out=*dg(m,0x6011u+code);
                else if(code<0x20u)out=*dg(m,0x6030u);
            }
            *dg(m,base+static_cast<u32>(row))=out;
        }
        for(s32 row=0;row<height;++row) {
            const s32 width=app05_s16(m,0x12756u);
            const u32 index=static_cast<u32>(width)*static_cast<u32>(row)+static_cast<u32>(col);
            if(*dg(m,0x1760u+index)==0xFFu)continue;
            s32 r=function_00700(m,col,row,0);
            while(r!=-1) {
                const u32 off=static_cast<u32>(r)*0x16u;
                const u16 type=load_le16(dg(m,0x22246u+off));
                *dg(m,overlay+static_cast<u32>(row))=(type>=0x3Cu&&type<=0x41u)?0x2Au:0xFEu;
                r=function_00700(m,col,row,r+1);
            }
        }
        u32 label_ptr=labels;
        for(s32 row=0;row<height;++row) {
            const App05LabelResult lr=app05_label_for_cell(m,col,row);
            if(lr.found!=0u){*dg(m,label_ptr)=lr.first;label_ptr+=2u;*dg(m,label_ptr-1u)=lr.second;}
            ++label_ptr;
        }
        if(block_mode==0u) {
            for(s32 i=0;i<height;++i)app05_putc(b,*dg(m,base+static_cast<u32>(i)),stream);
            app05_putc(b,0x0Du,stream);
            for(s32 i=0;i<height;++i)app05_putc(b,*dg(m,overlay+static_cast<u32>(i)),stream);
            app05_putc(b,0x0Du,stream);
            for(s32 i=0;i<height;++i)app05_putc(b,*dg(m,labels+static_cast<u32>(i)),stream);
            app05_putc(b,*dg(m,0x5F60u),stream);app05_putc(b,*dg(m,0x5F61u),stream);
        } else {
            for(s32 i=0;i<height;++i) {
                const u32 k=static_cast<u32>(i);
                if(*dg(m,overlay+k)!=0x20u)*dg(m,base+k)=*dg(m,overlay+k);
                if(*dg(m,labels+k)!=0x20u)*dg(m,base+k)=*dg(m,labels+k);
            }
            app05_write(b,base,alloc_size,sink);app05_write(b,0x5F60u,2u,sink);
        }
    }
    function_02474(m,b,stream,sink,block_mode);
    if(block_mode==0u){app05_putc(b,0x0Cu,stream);if(b.call_31491)b.call_31491(stream);}
    if(b.call_310b0){b.call_310b0(base);b.call_310b0(overlay);b.call_310b0(labels);}
}
static_assert(0x02A34u-0x026E8u==844u);

// 02A34..02AE4: derive the three decimal template bytes in the original order,
// prepare/open the destination, export in block mode, and close it.
void function_02a34(Machine& m,Application05Boundary& b,s32 value) noexcept {
    const s32 hundreds=value/100;
    *dg(m,0x5FE7u)=static_cast<u8>(hundreds+0x30);
    *dg(m,0x5FE8u)=static_cast<u8>((value%10)+0x30);
    *dg(m,0x5FE9u)=static_cast<u8>(((value-hundreds*100)/10)+0x30);
    if(b.call_31380)b.call_31380(0x5FDAu);
    const u32 q=b.call_312c8?b.call_312c8(0x5FDAu):0u;
    if(b.call_312dc)b.call_312dc(q);
    const u32 h=b.call_312b4?b.call_312b4(0x5FDAu):0xFFFFFFFFu;
    if(h==0xFFFFFFFFu){if(b.call_2e6d0)b.call_2e6d0();return;}
    function_026e8(m,b,0u,h,1u);
    if(b.call_312dc)b.call_312dc(h);
}
static_assert(0x02AE5u-0x02A34u==177u);

// 02AE8..02D6E: load a numbered map/resource file while preserving the current
// packed-text buffer for the live level. Runtime seek/read calls remain explicit.
[[nodiscard]] u32 function_02ae8(Machine& m,Application05Boundary& b,s32 index) noexcept {
    if(load_le32(dg(m,0x6050u))==0u) {
        const u32 used=function_01c08(m)-0x5760u;store_le32(dg(m,0x6054u),used);
        const u32 save=b.call_3104c?b.call_3104c(used):0u;store_le32(dg(m,0x6050u),save);
        app05_copy(m,b,save,0x5760u,used);
    }
    app05_fill(m,b,0x1760u,0xFFu,0x4000u);app05_fill(m,b,0x5760u,0u,0x800u);
    if(b.ui&&b.ui->call_1c368)b.ui->call_1c368(0x6058u,static_cast<u32>(index)+0x26Du);
    if(static_cast<u32>(index)==load_le32(dg(m,0xC6C8u))) {
        if(b.ui)function_01558(m,*b.ui);
        app05_fill(m,b,0x5760u,0u,0x800u);
        app05_copy(m,b,0x5760u,load_le32(dg(m,0x6050u)),load_le32(dg(m,0x6054u)));
        const u32 save=load_le32(dg(m,0x6050u));store_le32(dg(m,0x6054u),0u);
        if(b.call_310b0)b.call_310b0(save);
        store_le32(dg(m,0x6050u),0u);return 1u;
    }
    const s32 hundreds=index/100;
    *dg(m,0x81A4u)=static_cast<u8>(hundreds+0x30);
    *dg(m,0x81A6u)=static_cast<u8>((index%10)+0x30);
    *dg(m,0x81A5u)=static_cast<u8>(((index-hundreds*100)/10)+0x30);
    const u32 h=b.call_312a0?b.call_312a0(0x819Cu):0xFFFFFFFFu;
    if(h==0xFFFFFFFFu)return 0u;
    if(load_le32(dg(m,0x6048u))!=0u){const u32 p=load_le32(dg(m,0x6048u));if(b.call_310b0)b.call_310b0(p);store_le32(dg(m,0x6048u),0u);}
    if(b.call_3131c)b.call_3131c(0x0Du,0u,h);
    u32 pos=b.call_312ec_local32?b.call_312ec_local32(h):0u;
    if(b.call_3131c)b.call_3131c(pos,0u,h);
    if(b.call_312ec_guest)b.call_312ec_guest(0x604Cu,2u,h);
    const s32 count=app05_s16(m,0x604Cu);
    if(count!=0) {
        const u32 bytes=static_cast<u32>(count)*0x16u;
        const u32 p=b.call_3104c?b.call_3104c(bytes):0u;store_le32(dg(m,0x6048u),p);
        if(b.call_312ec_guest)b.call_312ec_guest(p,bytes,h);
    }
    if(b.call_3131c)b.call_3131c(0x19u,0u,h);
    pos=b.call_312ec_local32?b.call_312ec_local32(h):0u;
    if(b.call_3131c)b.call_3131c(pos,0u,h);
    if(b.call_312ec_guest){
        b.call_312ec_guest(0x12756u,2u,h);b.call_312ec_guest(0x12754u,2u,h);
        const s32 w=app05_s16(m,0x12756u), hh=app05_s16(m,0x12754u);
        b.call_312ec_guest(0x1760u,static_cast<u32>(w)*static_cast<u32>(hh),h);
    }
    pos=b.call_312ec_local32?b.call_312ec_local32(h):0u;
    if(static_cast<s32>(pos)<0x7FF&&b.call_312ec_guest)b.call_312ec_guest(0x5760u,pos,h);
    if(b.call_312dc)b.call_312dc(h);
    return 1u;
}
static_assert(0x02D6Fu-0x02AE8u==647u);

static s32 app05_rect_word(Machine& m,u32 action,u32 byte_off) noexcept {
    return app05_s16(m,0x5F62u+action*8u+byte_off);
}
static void app05_panel_region(Machine& m,Application05Boundary& b,u32 action,u32 sprite,u32 image_off,u32 w,u32 h,bool select_page=true) noexcept {
    if(!b.ui||!b.app)return;
    const u32 x=static_cast<u32>(app05_rect_word(m,action,0u));
    const u32 y=static_cast<u32>(app05_rect_word(m,action,2u));
    if(select_page&&b.ui->call_291b0)b.ui->call_291b0(*dg(m,0xC050u));
    if(b.app->call_2975c)b.app->call_2975c(sprite,x,y,load_le32(dg(m,0x258DCu))+image_off,0u);
    if(b.ui->call_1a8c8)b.ui->call_1a8c8(load_le32(dg(m,0xC050u)),x,y,w,h);
}
static void app05_redraw_panel_map(Machine& m,Application05Boundary& b) noexcept {
    if(!b.ui||!b.app||!b.gfx)return;
    if(b.ui->call_291b0)b.ui->call_291b0(*dg(m,0xC050u));
    function_0139c(m,*b.ui,*b.app,*b.gfx,static_cast<s32>(load_le32(dg(m,0x12744u))),
                   static_cast<s32>(load_le32(dg(m,0x12740u))),0x30,0x2F,1u);
    if(b.ui->call_1a510)b.ui->call_1a510(load_le32(dg(m,0xC050u)));
    *dg(m,0xC050u)=static_cast<u8>(*dg(m,0xC050u)^1u);
}
static void app05_select_level_camera(Machine& m) noexcept {
    if(*dg(m,0x5FCBu)!=0u) {
        s32 x=app05_s16(m,0x255D2u)-0x18, y=app05_s16(m,0x255D6u)-0x18;
        if(x<-40)x=-40;
        if(y<-40)y=-40;
        store_le32(dg(m,0x12744u),static_cast<u32>(x));store_le32(dg(m,0x12740u),static_cast<u32>(y));
    } else {store_le32(dg(m,0x12744u),0xFFFFFFFCu);store_le32(dg(m,0x12740u),0xFFFFFFFCu);}
}

// 02DB0..034AE: 16-way control-panel dispatcher driven by the 64-byte CS jump
// table at 02D70. Rectangles are inclusive on all four bounds.
void function_02db0(Machine& m,Application05Boundary& b,s32 x,s32 y,u32 flags) noexcept {
    const u32 saved_x=load_le32(dg(m,0x12744u));
    s32 index=0;
    for(;;++index) {
        const s32 x0=app05_rect_word(m,static_cast<u32>(index),0u);
        if(x0==-1)break;
        if(x>=x0&&x<=app05_rect_word(m,static_cast<u32>(index),4u)&&
           y>=app05_rect_word(m,static_cast<u32>(index),2u)&&y<=app05_rect_word(m,static_cast<u32>(index),6u))break;
    }
    u32 action=0u;
    if(app05_rect_word(m,static_cast<u32>(index),0u)==-1) {
        store_le32(dg(m,0x12744u),saved_x);
        if((flags&2u)==0u)return;
        action=11u;
    } else if((flags&2u)!=0u) {
        action=(index>=0&&index<=3)?static_cast<u32>(index+12):11u;
    } else action=static_cast<u32>(index);
    store_le32(dg(m,0x12744u),saved_x);
    if(action<=3u){*dg(m,0x5FCAu)=static_cast<u8>(1u<<(action&0x1Fu));return;}
    if(action>=12u&&action<=15u){*dg(m,0x5FCAu)=static_cast<u8>(1u<<((action-8u)&0x1Fu));return;}
    if(action==4u) {
        if(*dg(m,0x5FD9u)==0u||*dg(m,0x5FCBu)==0u)return;
        const u32 px=static_cast<u32>(app05_rect_word(m,action,0u)), py=static_cast<u32>(app05_rect_word(m,action,2u));
        if(b.ui&&b.ui->call_291b0)b.ui->call_291b0(*dg(m,0xC050u));
        if(b.call_298f0)b.call_298f0(0u,px,py,load_le32(dg(m,0x258DCu))+0x3Bu,0u,0xD370u);
        if(b.ui&&b.ui->call_1a8c8)b.ui->call_1a8c8(load_le32(dg(m,0xC050u)),px,py,0x3Fu,0x28u);
        if(b.ui&&b.app&&b.gfx)function_0207c(m,*b.ui,*b.app,*b.gfx);
        app05_panel_region(m,b,action,0u,0x3Bu,0x3Fu,0x28u);return;
    }
    if(action==5u) {
        if(*dg(m,0x5FD9u)==0u||*dg(m,0x5FCBu)==0u)return;
        const u32 px=static_cast<u32>(app05_rect_word(m,action,0u)), py=static_cast<u32>(app05_rect_word(m,action,2u));
        if(b.ui&&b.ui->call_291b0)b.ui->call_291b0(*dg(m,0xC050u));
        if(b.call_298f0)b.call_298f0(0u,px,py,load_le32(dg(m,0x258DCu))+0x3Fu,0u,0xD370u);
        if(b.ui&&b.ui->call_1a8c8)b.ui->call_1a8c8(load_le32(dg(m,0xC050u)),px,py,0x1Au,0x28u);
        function_021d8(m,b);app05_panel_region(m,b,action,0u,0x3Fu,0x1Au,0x28u);return;
    }
    if(action==6u||action==7u) {
        do {
            s32 level=static_cast<s32>(load_le32(dg(m,0x1274Cu)));
            if(action==6u){++level;if(level>31)level=0;}else{--level;if(level<0)level=31;}
            store_le32(dg(m,0x1274Cu),static_cast<u32>(level));
            *dg(m,0x5FCBu)=static_cast<u8>(static_cast<u32>(level)==load_le32(dg(m,0xC6C8u)));
            app05_select_level_camera(m);
        } while(function_02ae8(m,b,static_cast<s32>(load_le32(dg(m,0x1274Cu))))==0u);
        app05_redraw_panel_map(m,b);return;
    }
    if(action==8u) {
        *dg(m,0x5FD9u)=static_cast<u8>(*dg(m,0x5FD9u)^1u);
        app05_panel_region(m,b,action,*dg(m,0x5FD9u),0x38u,0x20u,0x30u);
        if(b.ui&&b.app&&b.gfx){function_0139c(m,*b.ui,*b.app,*b.gfx,static_cast<s32>(load_le32(dg(m,0x12744u))),static_cast<s32>(load_le32(dg(m,0x12740u))),0x30,0x2F,1u);if(b.ui->call_1a510)b.ui->call_1a510(load_le32(dg(m,0xC050u)));*dg(m,0xC050u)=static_cast<u8>(*dg(m,0xC050u)^1u);}
        return;
    }
    if(action==9u) {
        if(*dg(m,0x5FCBu)==0u)return;
        app05_panel_region(m,b,action,3u,0x38u,0x20u,0x30u);
        function_026e8(m,b,0xF764u,0u,0u);
        app05_panel_region(m,b,action,2u,0x38u,0x20u,0x30u,false);return;
    }
    if(action==10u) {
        if(*dg(m,0x5FCBu)==0u)return;
        app05_panel_region(m,b,action,5u,0x38u,0x20u,0x30u);
        function_02a34(m,b,static_cast<s32>(load_le32(dg(m,0xC6C8u))));
        app05_panel_region(m,b,action,4u,0x38u,0x20u,0x30u,false);return;
    }
    if(action==11u) {
        app05_panel_region(m,b,action,7u,0x38u,0x20u,0x30u);
        *dg(m,0x5FCAu)=0u;if(b.call_1f8fc)b.call_1f8fc();if(b.call_1ae30)b.call_1ae30();
    }
}
static_assert(0x034AFu-0x02DB0u==1791u);

// 034B0..03630: apply the first active navigation bit to the two viewport
// offsets, then redraw and page-flip. Signed clamps match the encoded branches.
void function_034b0(Machine& m,Application05Boundary& b) noexcept {
    s32 row=static_cast<s32>(load_le32(dg(m,0x12740u)));
    s32 col=static_cast<s32>(load_le32(dg(m,0x12744u)));
    const u8 mask=*dg(m,0x5FCAu);
    if((mask&1u)!=0u){if(row>-40)row=static_cast<s32>(static_cast<u32>(row)-4u);}
    else if((mask&2u)!=0u){const s32 lim=app05_s16(m,0x12754u)-0x2B;if(row<lim||row<-4)row=static_cast<s32>(static_cast<u32>(row)+4u);}
    else if((mask&4u)!=0u){if(col>-40)col=static_cast<s32>(static_cast<u32>(col)-4u);}
    else if((mask&8u)!=0u){const s32 lim=app05_s16(m,0x12756u)-0x2C;if(col<lim||col<-4)col=static_cast<s32>(static_cast<u32>(col)+4u);}
    else if((mask&0x10u)!=0u){row=static_cast<s32>(static_cast<u32>(row)-0x28u);if(row<-40)row=-40;}
    else if((mask&0x20u)!=0u){const s32 lim=app05_s16(m,0x12754u)-0x2F;row=static_cast<s32>(static_cast<u32>(row)+0x28u);if(row>lim)row=lim;}
    else if((mask&0x40u)!=0u){col=static_cast<s32>(static_cast<u32>(col)-0x28u);if(col<-40)col=-40;}
    else if((mask&0x80u)!=0u){const s32 lim=app05_s16(m,0x12756u)-0x30;col=static_cast<s32>(static_cast<u32>(col)+0x28u);if(col>lim)col=lim;}
    store_le32(dg(m,0x12740u),static_cast<u32>(row));store_le32(dg(m,0x12744u),static_cast<u32>(col));
    app05_redraw_panel_map(m,b);
}
static_assert(0x03631u-0x034B0u==385u);

// -----------------------------------------------------------------------------
// Batch 06: 03634..04619 map-mode setup, resource tables, nibble grids,
// proximity/collision helpers, and record movement/accounting.
// -----------------------------------------------------------------------------

struct Application06Boundary {
    u32 (*call_19ccc)(u32)=nullptr;
    void (*call_1a9f8)()=nullptr;
    void (*call_30d30)(u32,u32,u32)=nullptr;
    void (*call_22d70)()=nullptr;
    void (*call_22d8c)()=nullptr;
    void (*call_30cd4)(u32,u32,u32)=nullptr;
    void (*call_310b0)(u32)=nullptr;
    u32 (*call_19cf4)()=nullptr;
    u32 (*call_312a0)(u32)=nullptr;
    u32 (*call_3104c)(u32)=nullptr;
    void (*call_312ec)(u32,u32,u32)=nullptr;
    u32 (*call_312ec_local)(u32,u32)=nullptr;
    void (*call_312dc)(u32)=nullptr;
    void (*call_05bdc)(u32,u32,u32,u32)=nullptr;
    void (*call_1c40c)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_1a5b0)(u32,u32,u32)=nullptr;
    u32 (*call_30b40)()=nullptr;
    u32 (*call_2c8e4)(u32,u32,u32)=nullptr;
    u16 (*call_2d5d0)(u32,u32,u32)=nullptr;
    u32 (*call_50dc)(u32,u32)=nullptr;
    void (*call_9d90)(u32,u32,u32)=nullptr;
    void (*call_1f494)(u32)=nullptr;
};

[[nodiscard]] u32 function_03634(Machine& m,Application06Boundary& b,Application05Boundary& a5,Application04Boundary& a4,
                                  Application03Boundary& a3,Graphics2Boundary& g) noexcept {
    if(b.call_19ccc)(void)b.call_19ccc(0u);
    for(u32 i=0;i<0x100u;++i)*dg(m,0x759Cu+i)=static_cast<u8>(i);
    *dg(m,0x6058u)=0u;
    if(b.call_1a9f8)b.call_1a9f8();
    if(a4.call_291b0)a4.call_291b0(*dg(m,0xC050u));
    store_le32(dg(m,0x12744u),static_cast<u32>(static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x255D2u))))-0x18));
    store_le32(dg(m,0x12740u),static_cast<u32>(static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x255D6u))))-0x18));
    if(static_cast<s32>(load_le32(dg(m,0x12744u)))<-40)store_le32(dg(m,0x12744u),static_cast<u32>(-40));
    if(static_cast<s32>(load_le32(dg(m,0x12740u)))<-40)store_le32(dg(m,0x12740u),static_cast<u32>(-40));
    store_le16(dg(m,0x12756u),load_le16(dg(m,0x25619u)));
    store_le16(dg(m,0x12754u),load_le16(dg(m,0x2561Bu)));
    store_le32(dg(m,0x1274Cu),load_le32(dg(m,0xC6C8u)));
    *dg(m,0x5FCBu)=1u;
    const s32 w=static_cast<s16>(load_le16(dg(m,0x12756u)));
    const s32 h=static_cast<s16>(load_le16(dg(m,0x12754u)));
    if(b.call_30d30)b.call_30d30(0x1760u,0xFFu,static_cast<u32>(w*h));
    function_01558(m,a4);
    function_01834(m,a4,a3);
    function_0139c(m,a4,a3,g,static_cast<s32>(load_le32(dg(m,0x12744u))),
                   static_cast<s32>(load_le32(dg(m,0x12740u))),0x30,0x2F,1u);
    if(b.call_1a5b0)b.call_1a5b0(load_le32(dg(m,0xC050u)),
        static_cast<u32>(static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x8600u))))),*dg(m,0x8602u));
    *dg(m,0xC050u)=static_cast<u8>(*dg(m,0xC050u)^1u);
    if(b.call_22d70)b.call_22d70();
    *dg(m,0xD680u)=4u;
    while(*dg(m,0xD680u)==4u) {
        if(*dg(m,0x5FCAu)!=0u)function_034b0(m,a5);
        if(load_le16(dg(m,0x9E35u))==0u)*dg(m,0x5FCAu)=0u;
        if(b.call_22d8c)b.call_22d8c();
    }
    if(load_le32(dg(m,0x6050u))!=0u) {
        if(b.call_30d30)b.call_30d30(0x5760u,0u,0x800u);
        if(b.call_30cd4)b.call_30cd4(0x5760u,load_le32(dg(m,0x6050u)),load_le32(dg(m,0x6054u)));
        if(b.call_310b0)b.call_310b0(load_le32(dg(m,0x6050u)));
        store_le32(dg(m,0x6050u),0u);store_le32(dg(m,0x6054u),0u);
    }
    if(load_le32(dg(m,0x6048u))!=0u) {
        if(b.call_310b0)b.call_310b0(load_le32(dg(m,0x6048u)));
        store_le32(dg(m,0x6048u),0u);store_le16(dg(m,0x604Cu),0u);
    }
    return b.call_19cf4?b.call_19cf4():0u;
}
static_assert(0x0380Du-0x03634u==473u);

void function_03810(Machine& m,Application06Boundary& b) noexcept {
    if(b.call_310b0)b.call_310b0(load_le32(dg(m,0x608Cu)));
}
static_assert(0x0381Fu-0x03810u==15u);

[[nodiscard]] u32 function_03820(Machine& m,u32 size) noexcept {
    const u32 r=load_le32(dg(m,0x608Cu))+load_le32(dg(m,0x6094u));
    store_le32(dg(m,0x6094u),load_le32(dg(m,0x6094u))+size);
    return r;
}
static_assert(0x03836u-0x03820u==22u);

void function_03838(Machine& m,Application06Boundary& b) noexcept {
    const u32 h=b.call_312a0?b.call_312a0(0x24u):0u;
    u32 p=b.call_3104c?b.call_3104c(0x708u):0u;store_le32(dg(m,0xC65Cu),p);
    if(b.call_312ec)b.call_312ec(p,0x708u,h);
    store_le32(dg(m,0xC664u),load_le32(dg(m,0xC65Cu))+0x168u);
    p=b.call_3104c?b.call_3104c(0xE10u):0u;store_le32(dg(m,0xC660u),p);
    if(b.call_312ec)b.call_312ec(p,0xE10u,h);
    const struct ReadSpec{u32 dst,size;} specs[]={{0x19660u,0x100u},{0x193E0u,0x100u},{0xC6D8u,4u},{0xC6D4u,4u},{0xC6D0u,4u},{0xC6DCu,4u},{0xC6E0u,4u},{0x2560Cu,4u},{0xC6CCu,4u},{0x255E0u,4u},{0xC73Cu,0x400u},{0xC738u,4u},{0xC730u,4u},{0xC734u,4u}};
    if(b.call_312ec)for(const auto& q:specs)b.call_312ec(q.dst,q.size,h);
    const u32 words=(load_le32(dg(m,0xC730u))+1u)*2u;
    p=b.call_3104c?b.call_3104c(words):0u;store_le32(dg(m,0x258C4u),p);if(b.call_312ec)b.call_312ec(p,words,h);
    if(b.call_312ec)b.call_312ec(0x6090u,4u,h);
    p=b.call_3104c?b.call_3104c(load_le32(dg(m,0x6090u))):0u;store_le32(dg(m,0x608Cu),p);store_le32(dg(m,0x15618u),0u);
    if(static_cast<s32>(load_le32(dg(m,0xC738u)))>0) {
        u32 table_off=4u;
        for(s32 i=0;i<static_cast<s32>(load_le32(dg(m,0xC738u)));++i,table_off+=4u) {
            const u32 n=(b.call_312ec_local?b.call_312ec_local(2u,h):0u)&0xFFFFu;
            u32 dst=function_03820(m,n);
            store_le32(dg(m,0x18FE0u+table_off),dst);
            store_le32(dg(m,0x15618u+table_off),table_off);
            if(b.call_312ec)b.call_312ec(dst,n,h);
            dst=load_le32(dg(m,0x18FE0u+table_off))+0x204u;
            store_le32(dg(m,0x18FE0u+table_off),dst);
        }
    }
    store_le32(dg(m,0x18FE0u),load_le32(dg(m,0x18FE4u)));
    if(b.call_312dc)b.call_312dc(h);
}
static_assert(0x03A64u-0x03838u==556u);

static void application06_constrain_nibbles(Machine& m,u32 base,u32 byte_base) noexcept {
    for(u32 row=0;row<8u;++row) {
        const u32 cap=*dg(m,0x6208u+row);u32 low_sum=0u,high_sum=0u;
        const s32 count=static_cast<s32>(load_le32(dg(m,0x6440u+row*4u)));
        if(count<=0)continue;
        for(s32 i=0;i<count;++i) {
            u8& cell=*dg(m,byte_base+base+row*10u+static_cast<u32>(i));
            u32 low=cell&0x0Fu;if(low==0x0Fu)continue;
            if(cap<low_sum+low){low=(cap-low_sum)&0x0Fu;cell=static_cast<u8>((cell&0xF0u)|low);}
            low_sum+=cell&0x0Fu;
            u32 high=cell>>4;low=cell&0x0Fu;if(high>low){high=low;cell=static_cast<u8>(low|(high<<4));}
            high=cell>>4;
            if(high_sum+high>cap){high=(cap-high_sum)&0x0Fu;cell=static_cast<u8>((cell&0x0Fu)|(high<<4));}
            high_sum+=cell>>4;
        }
    }
}

void function_03a78(Machine& m,Application06Boundary& b,u32 slot,u32 phase) noexcept {
    if(b.call_05bdc)b.call_05bdc(1u,slot,phase-1u,0x6210u);
    application06_constrain_nibbles(m,slot*0x248u,0x2194Au);
}
static_assert(0x03C03u-0x03A78u==395u);

void function_03c04(Machine& m,Application06Boundary& b,u32 slot,u32 phase) noexcept {
    const u32 base=slot*0x248u;const s32 kind=static_cast<s8>(*dg(m,base+0x218F5u));
    if(b.call_05bdc)b.call_05bdc(0u,slot,phase-1u,kind==1?0x6350u:0x62B0u);
    if(kind==1){*dg(m,base+0x219A1u)=0x11u;if(static_cast<s32>(phase-1u)<3)*dg(m,base+0x219A2u)=0xFFu;else *dg(m,base+0x219A2u)=0x11u;}
    else *dg(m,base+0x219A2u)=0x11u;
    application06_constrain_nibbles(m,base,0x2199Au);
}
static_assert(0x03DE3u-0x03C04u==479u);

static void application06_gate_nibbles(Machine& m,u32 base,u32 phase,u32 table) noexcept {
    const u32 selector=(phase-1u)*8u;
    for(u32 row=0;row<8u;++row) {
        const bool enabled=*dg(m,table+selector+row)!=0u;
        const s32 count=static_cast<s32>(load_le32(dg(m,0x6420u+row*4u)));
        if(count<=0)continue;
        for(s32 i=0;i<count;++i) {
            u8& cell=*dg(m,base+0x2199Au+row*10u+static_cast<u32>(i));
            const u8 low=cell&0x0Fu;
            if(enabled){if(low==0x0Fu)cell=0u;}
            else if(low!=0x0Fu)cell=0xFFu;
        }
    }
}

void function_03de4(Machine& m,u32 slot,u32 phase) noexcept {
    const u32 base=slot*0x248u;*dg(m,base+0x219A2u)=0x11u;application06_gate_nibbles(m,base,phase,0x62B0u);
}
static_assert(0x03EA4u-0x03DE4u==192u);

void function_03ea4(Machine& m,u32 slot,u32 phase) noexcept {
    const u32 base=slot*0x248u;*dg(m,base+0x219A1u)=0x11u;if(static_cast<s32>(phase-1u)>=3)*dg(m,base+0x219A2u)=0x11u;application06_gate_nibbles(m,base,phase,0x6350u);
}
static_assert(0x03F75u-0x03EA4u==209u);

void function_03f78(Machine& m,Application06Boundary& b,Application04Boundary& a4,u32 text0,u32 text1) noexcept {
    if(a4.call_291b0)a4.call_291b0(*dg(m,0xC050u));
    if(b.call_1c40c)b.call_1c40c(load_le32(dg(m,0xC050u)),0x50u,0x50u,0xF0u,0xA0u);
    if(a4.call_30508)a4.call_30508(0xC4u,0x50u,0xA0u,text0,1u);
    if(a4.call_30508)a4.call_30508(0xCEu,0x50u,0xA0u,text1,1u);
    if(a4.call_1a510)a4.call_1a510(load_le32(dg(m,0xC050u)));
    *dg(m,0xC050u)=static_cast<u8>(*dg(m,0xC050u)^1u);
    if(a4.call_1a1f4)while(a4.call_1a1f4()!=0u){}
    for(;;){const u32 pressed=a4.call_1a1f4?a4.call_1a1f4():1u;if(pressed!=0u)break;if(b.call_30b40&&b.call_30b40()!=0u)break;}
    for(;;){if(a4.call_1a1f4)while(a4.call_1a1f4()!=0u){}if(!b.call_30b40||b.call_30b40()==0u)break;}
    if(b.call_1a5b0)b.call_1a5b0(load_le32(dg(m,0xC050u)),static_cast<u32>(static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x8600u))))),*dg(m,0x8602u));
    *dg(m,0xC050u)=static_cast<u8>(*dg(m,0xC050u)^1u);
}
static_assert(0x04040u-0x03F78u==200u);

[[nodiscard]] u32 function_04040(Machine& m,Application06Boundary& b,Application03Boundary& a3,Application04Boundary& a4) noexcept {
    u32 x=load_le32(dg(m,0x255D0u)),y=load_le32(dg(m,0x255D4u));
    for(u32 angle=0;angle<0x168u;++angle) {
        u32 dy=0u,dx=0u;if(a3.call_2df2c)a3.call_2df2c(&dx,&dy,0x5000u,angle);
        for(;;){x+=dx;y+=dy;const s32 row=static_cast<s16>(static_cast<u16>(y>>16));const s32 col=static_cast<s16>(static_cast<u16>(x>>16));const s32 width=static_cast<s16>(load_le16(dg(m,0x25619u)));const u32 index=static_cast<u32>(row*width+col);const u32 tile=0x198E0u+index*2u;
            bool probe=(load_le16(dg(m,tile))&0x3Fu)==0u;
            if(!probe&&(*dg(m,tile)&0x80u)!=0u)probe=static_cast<u16>(a4.call_1e930?a4.call_1e930(index):0u)!=0x8000u;
            if(!probe)break;
            if(b.call_2c8e4&&b.call_2c8e4(y,x,0xFFFFFFFFu)!=0u)return 0u;
        }
    }
    return 1u;
}
static_assert(0x040FFu-0x04040u==191u);

[[nodiscard]] u32 function_04100(Machine& m,Application06Boundary& b,Application03Boundary& a3,Application04Boundary& a4) noexcept {
    store_le32(dg(m,0x6688u),0u);
    const s32 px=static_cast<s16>(load_le16(dg(m,0x255D2u))),py=static_cast<s16>(load_le16(dg(m,0x255D6u)));
    if(load_le32(dg(m,0xC6C8u))==0x1Du&&px>0x41)return 1u;
    if(load_le32(dg(m,0xC6C8u))==0x0Bu&&px>=4&&px<=7&&py>=7&&py<=0x0B)return 0u;
    if(function_04040(m,b,a3,a4)!=0u){store_le32(dg(m,0x6688u),1u);return 0u;}
    s32 i=0;u32 off=0u;
    while(i<static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x258A2u))))) {
        if((*dg(m,0x14D8Bu+off)&2u)==0u) {
            const u32 type=load_le16(dg(m,0x14D8Eu+off));
            if((a3.call_2acdc?a3.call_2acdc(type):0u)!=0u) {
                const u32 tile=load_le16(dg(m,0x14D8Cu+off));
                if((*dg(m,0x198E0u+tile*2u)&0x40u)!=0u)return 1u;
                s32 d=static_cast<s16>(load_le16(dg(m,0x14D82u+off)))-py;if(d<0)d=-d;
                if(d<15){d=static_cast<s16>(load_le16(dg(m,0x14D86u+off)))-px;if(d<0)d=-d;if(d<15&&(*dg(m,0x198E1u+tile*2u)&4u)!=0u)return 1u;}
            }
        }
        off+=0x16u;++i;
    }
    return 0u;
}
static_assert(0x04225u-0x04100u==293u);

[[nodiscard]] u32 function_04228(Machine& m,Application06Boundary& b) noexcept {
    for(u32 type_off=0u;type_off<6u;type_off+=2u) {
        const u32 wanted=load_le16(dg(m,0x258AAu+type_off));
        s32 i=0;u32 off=0u;
        while(i<static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x258A2u))))) {
            bool moved=false;
            if((*dg(m,0x14D8Bu+off)&2u)==0u&&(*dg(m,0x198E0u+static_cast<u32>(load_le16(dg(m,0x14D8Cu+off)))*2u)&0x40u)==0u&&load_le16(dg(m,0x14D8Eu+off))==wanted) {
                s32 dx=static_cast<s16>(load_le16(dg(m,0x14D86u+off)))-static_cast<s16>(load_le16(dg(m,0x255D2u)));if(dx<0)dx=-dx;
                if(dx>10){s32 dy=static_cast<s16>(load_le16(dg(m,0x14D82u+off)))-static_cast<s16>(load_le16(dg(m,0x255D6u)));if(dy<0)dy=-dy;if(dy>10){
                    const u32 old0=load_le32(dg(m,0x14D80u+off)),old4=load_le32(dg(m,0x14D84u+off));
                    store_le32(dg(m,0x14D84u+off),load_le32(dg(m,0x255D0u)));store_le32(dg(m,0x14D80u+off),load_le32(dg(m,0x255D4u)));
                    store_le16(dg(m,0x14D82u+off),static_cast<u16>(load_le16(dg(m,0x14D82u+off))+1u));
                    moved=b.call_2d5d0&&b.call_2d5d0(load_le32(dg(m,0x14D80u+off)),load_le32(dg(m,0x14D84u+off)),static_cast<u32>(i))!=0u;
                    if(!moved){store_le32(dg(m,0x14D80u+off),load_le32(dg(m,0x255D4u)));store_le16(dg(m,0x14D82u+off),static_cast<u16>(load_le16(dg(m,0x14D82u+off))-1u));moved=b.call_2d5d0&&b.call_2d5d0(load_le32(dg(m,0x14D80u+off)),load_le32(dg(m,0x14D84u+off)),static_cast<u32>(i))!=0u;}
                    if(!moved){store_le32(dg(m,0x14D84u+off),load_le32(dg(m,0x255D4u)));store_le16(dg(m,0x14D86u+off),static_cast<u16>(load_le16(dg(m,0x14D86u+off))-1u));moved=b.call_2d5d0&&b.call_2d5d0(load_le32(dg(m,0x14D80u+off)),load_le32(dg(m,0x14D84u+off)),static_cast<u32>(i))!=0u;}
                    if(!moved){store_le32(dg(m,0x14D84u+off),load_le32(dg(m,0x255D4u)));store_le16(dg(m,0x14D86u+off),static_cast<u16>(load_le16(dg(m,0x14D86u+off))+1u));moved=b.call_2d5d0&&b.call_2d5d0(load_le32(dg(m,0x14D80u+off)),load_le32(dg(m,0x14D84u+off)),static_cast<u32>(i))!=0u;}
                    if(!moved){store_le32(dg(m,0x14D84u+off),old4);store_le32(dg(m,0x14D80u+off),old0);}
                }}
            }
            if(moved){const u32 roff=static_cast<u32>(i)*0x16u;*dg(m,0x14D8Bu+roff)=static_cast<u8>((*dg(m,0x14D8Bu+roff)&0xCFu)|0x10u);return 1u;}
            off+=0x16u;++i;
        }
    }
    return 0u;
}
static_assert(0x043F1u-0x04228u==457u);

void function_043f4(Machine& m,Application06Boundary& b,u32 slot,s32* second,s32* first) noexcept {
    const u32 a=b.call_50dc?b.call_50dc(0u,slot):0u;const u32 c=b.call_50dc?b.call_50dc(1u,slot):0u;const u32 base=0x218E0u+slot*0x248u;
    for(u32 row=0;row<8u;++row)for(u32 i=0;i<10u;++i){
        if(c!=0u){const u8 v=*dg(m,base+row*10u+i+0x6Au);const u8 lo=v&0x0Fu,hi=v>>4;if(lo!=0x0Fu&&hi!=lo&&hi<lo&&first)*first-=static_cast<s32>(lo-hi);}
        if(a!=0u){const u8 v=*dg(m,base+row*10u+i+0xBAu);const u8 lo=v&0x0Fu,hi=v>>4;if(lo!=0x0Fu&&hi!=lo&&hi<lo&&second)--*second;}
    }
}
static_assert(0x044CFu-0x043F4u==219u);

[[nodiscard]] u32 function_044d0(Machine& m,Application06Boundary& b,u32 slot,u32 row,u32 col,u32 kind,u32* out,u32* remaining,u32 value) noexcept {
    const u32 cell=slot*0x248u+row*10u+col+0x2199Au;
    if((*dg(m,cell)&0x0Fu)==0x0Fu||(*dg(m,cell)&0xF0u)==0u)return 0u;
    u32 changed=0u,slot_off=0u;
    for(u32 s=0;s<4u;++s,slot_off+=0x248u){
        const u16 flags=load_le16(dg(m,0x218F0u+slot_off));
        if((flags&0xC000u)!=0u)continue;
        if(load_le16(dg(m,0x218FCu+slot_off))==load_le16(dg(m,0x218FEu+slot_off))){if(remaining)--*remaining;continue;}
        while((*dg(m,cell)&0xF0u)!=0u&&
              static_cast<s16>(load_le16(dg(m,0x218FCu+slot_off)))<static_cast<s16>(load_le16(dg(m,0x218FEu+slot_off)))) {
            if(b.call_9d90)b.call_9d90(kind,s,slot);
            *dg(m,0xCF5Au+s)=1u;
            if(b.call_1f494)b.call_1f494(1u);
            const u8 v=*dg(m,cell);const u8 hi=static_cast<u8>(((v>>4)-1u)&0x0Fu);
            *dg(m,cell)=static_cast<u8>((v&0x0Fu)|(hi<<4));
            if(kind!=0x57u&&(!out||*out==0u)){if(out)*out=value+0xF0u;changed=1u;}
        }
        if(load_le16(dg(m,0x218FCu+slot_off))==load_le16(dg(m,0x218FEu+slot_off))&&remaining)--*remaining;
    }
    return changed;
}
static_assert(0x0461Au-0x044D0u==330u);

// -----------------------------------------------------------------------------
// Batch 07: 0461C..0518B four-party simulation/update loop and classifier.
// -----------------------------------------------------------------------------

struct Application07Boundary {
    void (*call_1c21c)(u32)=nullptr;
    void (*call_1c258)()=nullptr;
    u32 (*call_7d44)(u32)=nullptr;
    u32 (*call_23eac)(u32,u32)=nullptr;
    void (*call_cd44)(u32,u32)=nullptr;
    void (*call_31f94)()=nullptr;
    void (*call_893c)()=nullptr;
    u32 (*call_30ca2)()=nullptr;
    void (*call_3210f3)(u32,u32,u32)=nullptr;
    void (*call_3210f4)(u32,u32,u32,u32)=nullptr;
    void (*call_1c368)(u32,u32)=nullptr;
    void (*call_30aec)()=nullptr;
    void (*call_231c0)(u32,u32)=nullptr;
    void (*call_1e74c)(u32)=nullptr;
};

[[nodiscard]] u32 function_050dc(Machine& m,u32 mode,u32 slot) noexcept;

static u32 app07_string_end(Machine& m,u32 p) noexcept {
    while(*dg(m,p)!=0u)++p;
    return p;
}
static void app07_emit(Application07Boundary& b) noexcept {
    if(b.call_1c21c)b.call_1c21c(0x759Cu);
    if(b.call_1c258)b.call_1c258();
}
static void app07_format_record_line(Machine& m,Application07Boundary& b,u32 rec,u32 suffix) noexcept {
    if(b.call_3210f3)b.call_3210f3(0x759Cu,load_le32(dg(m,0x6098u)),rec);
    if(b.call_1c368)b.call_1c368(app07_string_end(m,0x759Cu),suffix);
    app07_emit(b);
}

void function_0461c(Machine& m,Application07Boundary& b,Application06Boundary& a6,
                    Application03Boundary& a3,Application04Boundary& a4) noexcept {
    if(function_04100(m,a6,a3,a4)!=0u) {
        u32 src=load_le32(dg(m,0x60ACu)),dst=0x759Cu;
        for(;;) {
            const u8 a=*dg(m,src);*dg(m,dst)=a;
            if(a==0u)break;
            const u8 c=*dg(m,src+1u);*dg(m,dst+1u)=c;
            src+=2u;dst+=2u;
            if(c==0u)break;
        }
        app07_emit(b);
        return;
    }

    u32 deadlines[4]={0u,0u,0u,0u};
    u32 pending=0u;
    s32 deficit=0;
    s32 queued=0;
    u32 special=0u;
    u32 force_exit=0u;
    u32 tick=0u;
    bool cleanup_only=false;

    for(u32 p=0;p<4u;++p) {
        const u32 rec=0x218E0u+p*0x248u;
        if((load_le16(dg(m,rec+0x10u))&0x1000u)!=0u) {
            *dg(m,0xCF5Au+p)=1u;
            *dg(m,rec+0x11u)=static_cast<u8>(*dg(m,rec+0x11u)&0xEFu);
        }
    }

    for(u32 p=0;p<4u;++p) {
        const u32 rec=0x218E0u+p*0x248u;
        const u16 flags=load_le16(dg(m,rec+0x10u));
        const u16 cur=load_le16(dg(m,rec+0x1Cu));
        const u16 target=load_le16(dg(m,rec+0x1Eu));
        if((flags&0xC000u)==0u&&cur!=target)++pending;
        if(cur==target&&(flags&0x0C00u)!=0u)++pending;
        if((flags&0xC000u)!=0u)continue;
        for(u32 row=0;row<8u;++row) {
            const u32 rb=rec+row*10u;
            for(u32 col=0;col<10u;++col) {
                if((*dg(m,rec+0x131u)&2u)!=0u) {
                    const u32 cell=rb+col+0x6Au;const u8 raw=*dg(m,cell);
                    const u8 lo=raw&0x0Fu,hi=raw>>4;
                    if(lo!=0x0Fu&&hi!=lo) {
                        if(hi>lo)*dg(m,cell)=static_cast<u8>(lo|(lo<<4));
                        else deficit+=static_cast<s32>(lo-hi);
                    }
                }
                if((*dg(m,rec+0x131u)&5u)!=0u) {
                    const u32 cell=rb+col+0xBAu;const u8 raw=*dg(m,cell);
                    const u8 lo=raw&0x0Fu,hi=raw>>4;
                    if(lo!=0x0Fu&&hi!=lo) {
                        if(hi>lo)*dg(m,cell)=static_cast<u8>(lo|(lo<<4));
                        else deadlines[p]=0x2D0u;
                    }
                }
            }
        }
        if(deadlines[p]!=0u)++queued;
    }
    for(u32 p=0;p<4u;++p)*dg(m,0xCF5Au+p)=1u;
    const u32 had_work=static_cast<u32>(queued)|static_cast<u32>(deficit);

    if(pending!=0u) {
        for(u32 p=0;p<4u;++p) {
            const u32 rec=0x218E0u+p*0x248u;
            if((load_le16(dg(m,rec+0x10u))&0xC000u)!=0u)continue;
            const s32 cls=static_cast<s32>(function_050dc(m,0u,p));
            bool trigger=false;
            if(cls>=0) {
                const u8 a=*dg(m,rec+0xBBu)&0x0Fu;
                const u8 c=*dg(m,rec+0xD8u)&0x0Fu;
                const u8 d=*dg(m,rec+0xE2u)&0x0Fu;
                const u8 e=*dg(m,rec+0xECu)&0x0Fu;
                trigger=a!=0u&&(a!=0x0Fu||c!=0x0Fu||d!=0x0Fu||e!=0x0Fu);
            } else if(b.call_23eac)trigger=b.call_23eac(p,1u)!=0u;
            if(trigger) {
                special=b.call_7d44?b.call_7d44(load_le32(dg(m,0x60D0u))):0u;
                break;
            }
        }
    }

    if(pending!=0u||deficit!=0||queued!=0) {
        for(;;) {
            for(u32 step=0;step<10u;++step) {
                ++tick;
                store_le32(dg(m,0xA7E4u),load_le32(dg(m,0xA7E4u))+0x438u);
                store_le32(dg(m,0xCF4Cu),load_le32(dg(m,0xCF4Cu))+0x445u);
                if(b.call_31f94)b.call_31f94();
                if(load_le16(dg(m,0xCF4Eu))==0u) {
                    for(u32 p=0;p<4u;++p) {
                        const u32 rec=0x218E0u+p*0x248u;
                        if((load_le16(dg(m,rec+0x10u))&0xC000u)!=0u)continue;
                        if(static_cast<s8>(*dg(m,rec+0x15u))==1)*dg(m,rec+0xC1u)=0x11u;
                        if(static_cast<s32>(function_050dc(m,0u,p))>=0)*dg(m,rec+0xC2u)=0x11u;
                    }
                }
            }
            if(static_cast<s32>(tick)>=0x1E0)force_exit=1u;
            if(b.call_893c)b.call_893c();
            if(*dg(m,0x258A8u)!=0xFFu) {
                const u32 r=b.call_30ca2?b.call_30ca2():0u;
                if(((r*100u)>>15)>0x5Fu&&function_04228(m,a6)!=0u){cleanup_only=true;break;}
            }

            bool early=false;
            for(u32 p=0;p<4u&&!early;++p) {
                const u32 rec=0x218E0u+p*0x248u;
                const u16 flags=load_le16(dg(m,rec+0x10u));
                if(static_cast<u32>(flags)>=0x4000u)continue;
                if((flags&0x0800u)!=0u) {
                    if(static_cast<s16>(load_le16(dg(m,rec+0x1Cu)))==1) {
                        --pending;function_043f4(m,a6,p,&queued,&deficit);
                    }
                    if(b.call_cd44)b.call_cd44(p,1u);
                    *dg(m,0xCF5Au+p)=1u;
                    if(p==0u) {
                        force_exit=1u;
                        if((b.call_7d44?b.call_7d44(load_le32(dg(m,0x60D4u))):0u)==0u)early=true;
                    }
                }
                if(!early&&(flags&0x0400u)!=0u) {
                    if(static_cast<s16>(load_le16(dg(m,rec+0x1Cu)))<=3) {
                        --pending;function_043f4(m,a6,p,&queued,&deficit);
                    }
                    if(b.call_cd44)b.call_cd44(p,3u);
                    *dg(m,0xCF5Au+p)=1u;
                    if(p==0u) {
                        force_exit=1u;
                        if((b.call_7d44?b.call_7d44(load_le32(dg(m,0x60D8u))):0u)==0u)early=true;
                    }
                }
            }
            if(early){cleanup_only=true;break;}

            if(special!=0u) {
                for(u32 p=0;p<4u;++p) {
                    const u32 rec=0x218E0u+p*0x248u;
                    if((load_le16(dg(m,rec+0x10u))&0xC000u)!=0u)continue;
                    if(static_cast<s32>(function_050dc(m,0u,p))>=0) {
                        queued+=static_cast<s32>(function_044d0(m,a6,p,0u,1u,0x51u,&deadlines[p],&pending,tick));
                        queued+=static_cast<s32>(function_044d0(m,a6,p,3u,0u,0x6Eu,&deadlines[p],&pending,tick));
                        queued+=static_cast<s32>(function_044d0(m,a6,p,4u,0u,0x78u,&deadlines[p],&pending,tick));
                    }
                    if(b.call_23eac&&b.call_23eac(p,1u)!=0u)
                        (void)function_044d0(m,a6,p,0u,7u,0x57u,nullptr,&pending,tick);
                }
            }

            if(static_cast<s32>(tick)%60==0) {
                for(u32 p=0;p<4u;++p) {
                    const u32 rec=0x218E0u+p*0x248u;
                    if((load_le16(dg(m,rec+0x10u))&0x8000u)==0u&&static_cast<s8>(*dg(m,rec+0x15u))==1)
                        *dg(m,rec+0xC1u)=0x11u;
                }
            }
            if(pending!=0u&&static_cast<s32>(tick)%0x5A0==0) {
                for(u32 p=0;p<4u;++p) {
                    const u32 rec=0x218E0u+p*0x248u;
                    if((load_le16(dg(m,rec+0x10u))&0xC000u)!=0u)continue;
                    s16 cur=static_cast<s16>(load_le16(dg(m,rec+0x1Cu)));
                    const s16 target=static_cast<s16>(load_le16(dg(m,rec+0x1Eu)));
                    if(cur==target)continue;
                    cur=static_cast<s16>(cur+3);store_le16(dg(m,rec+0x1Cu),static_cast<u16>(cur));
                    if(cur>=target){store_le16(dg(m,rec+0x1Cu),static_cast<u16>(target));--pending;}
                    *dg(m,0xCF5Au+p)=1u;
                }
            }
            if(a6.call_1f494)a6.call_1f494(1u);

            if(force_exit!=0u&&queued!=0) {
                for(u32 p=0;p<4u;++p) {
                    const u32 rec=0x218E0u+p*0x248u;
                    if((load_le16(dg(m,rec+0x10u))&0xC000u)!=0u||(*dg(m,rec+0x131u)&5u)==0u||
                       deadlines[p]==0u||static_cast<s32>(tick)<=static_cast<s32>(deadlines[p]))continue;
                    --queued;deadlines[p]=0u;
                    for(u32 row=0;row<8u;++row)for(u32 col=0;col<10u;++col) {
                        const u32 cell=rec+row*10u+col+0xBAu;const u8 raw=*dg(m,cell);
                        const u8 lo=raw&0x0Fu,hi=raw>>4;
                        if(lo==0x0Fu||hi==lo)continue;
                        *dg(m,cell)=static_cast<u8>(lo|(lo<<4));
                        app07_format_record_line(m,b,rec,0x92u+row*10u+col);
                    }
                }
            }
            if(force_exit!=0u&&deficit!=0) {
                for(u32 p=0;p<4u;++p) {
                    const u32 rec=0x218E0u+p*0x248u;
                    if((load_le16(dg(m,rec+0x10u))&0xC000u)!=0u||(*dg(m,rec+0x131u)&2u)==0u)continue;
                    const s32 q=(static_cast<s32>(tick)-static_cast<s32>(deadlines[p]))/10;
                    if(q<=0)continue;
                    bool changed=false;
                    for(u32 rowoff=0u;rowoff<0x50u&&static_cast<s32>(rowoff)<q*10&&!changed;rowoff+=10u) {
                        for(u32 col=0;col<10u;++col) {
                            const u32 cell=rec+rowoff+col+0x6Au;const u8 raw=*dg(m,cell);
                            const u8 lo=raw&0x0Fu,hi=raw>>4;
                            if(lo==0x0Fu||hi==lo)continue;
                            const u8 nhi=static_cast<u8>((hi+1u)&0x0Fu);
                            *dg(m,cell)=static_cast<u8>(lo|(nhi<<4));
                            deadlines[p]=tick;
                            app07_format_record_line(m,b,rec,0x42u+rowoff+col);
                            --deficit;changed=true;break;
                        }
                    }
                }
            }

            const u32 key=a6.call_30b40?a6.call_30b40():0x1Bu;
            if(key==0x1Bu)break;
        }
    }

    if(!cleanup_only) {
        if(a6.call_30b40&&a6.call_30b40()!=0u&&b.call_30aec)b.call_30aec();
        if(had_work!=0u) {
            if(*dg(m,0xD681u)!=0u) {
                const u32 p=load_le32(dg(m,0xD8A8u));*dg(m,0xD681u)=2u;
                if(b.call_231c0)b.call_231c0(p,*dg(m,0x21A0Fu+p*0x248u));
            }
            if(*dg(m,0xD682u)!=0u) {
                const u32 p=load_le32(dg(m,0xD8ACu));*dg(m,0xD682u)=2u;
                if(b.call_231c0)b.call_231c0(p,*dg(m,0x21A0Fu+p*0x248u));
            }
        }
        const s32 q=static_cast<s32>(tick)/60,r=static_cast<s32>(tick)%60;
        if(q!=0) {if(b.call_3210f4)b.call_3210f4(0x759Cu,load_le32(dg(m,0x609Cu)),static_cast<u32>(q),static_cast<u32>(r));}
        else {if(b.call_3210f3)b.call_3210f3(0x759Cu,load_le32(dg(m,0x60A0u)),static_cast<u32>(r));}
        app07_emit(b);
    }
    if(b.call_1e74c)b.call_1e74c(load_le32(dg(m,0xA7E4u)));
}
static_assert(0x050D9u-0x0461Cu==2749u);

[[nodiscard]] u32 function_050dc(Machine& m,u32 mode,u32 slot) noexcept {
    const u32 rec=0x218E0u+slot*0x248u;
    if((load_le16(dg(m,rec+0x10u))&0xF240u)!=0u)return 0xFFFFFFFFu;
    if(mode==0u) {
        const s8 a=static_cast<s8>(*dg(m,rec+0x15u));
        if(a==3)return 0u;
        if(static_cast<s8>(*dg(m,rec+0x16u))==3)return 1u;
        if(static_cast<s8>(*dg(m,rec+0x17u))==3)return 2u;
        if(a==1&&static_cast<s16>(load_le16(dg(m,rec+0x20u)))>=9)return 0u;
    } else {
        if(static_cast<s8>(*dg(m,rec+0x15u))==4)return 0u;
        if(static_cast<s8>(*dg(m,rec+0x16u))==4)return 1u;
        if(static_cast<s8>(*dg(m,rec+0x17u))==4)return 2u;
    }
    return 0xFFFFFFFFu;
}
static_assert(0x0518Cu-0x050DCu==176u);

// -----------------------------------------------------------------------------
// Batch 08: 0518C..06007 party/status UI buffers, interaction loops, settings,
// and ten-way status icon renderer.
// -----------------------------------------------------------------------------

struct Application08Boundary {
    void (*call_291b0)(u32)=nullptr;
    void (*call_3059c)(u32)=nullptr;
    void (*call_2e510)(u32,u32,u32,u32)=nullptr;
    void (*call_2975c)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_30508)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_30d30)(u32,u32,u32)=nullptr;
    void (*call_3210f)(u32,u32,u32)=nullptr;
    void (*call_3060c)(u32,u32,u32,u32)=nullptr;
    u32 (*call_2e6f0)(u32,u32)=nullptr;
    void (*call_1c368)(u32,u32)=nullptr;
    void (*call_30630)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_32220)(u32,u32,u32,u32,u32,u32)=nullptr;
    void (*call_325b0)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_32468)(u32,u32,u32,u32,u32,const u8*)=nullptr;
    void (*call_2aa9c)(u32,u32,u32)=nullptr;
    void (*call_1a5b0)(u32,u32,u32)=nullptr;
    void (*call_322ec)(u32,u32,u32,u32,u32,u32)=nullptr;
    u32 (*call_1a1f4)()=nullptr;
    u32 (*call_2f29c)(u32,u32)=nullptr;
    void (*call_296c0)(u32,u32,u32,u32)=nullptr;
    void (*call_2e438)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_1a8c8)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_1a510)(u32)=nullptr;
    void (*call_29234)(u32)=nullptr;
    u32 (*call_3104c)(u32)=nullptr;
    void (*call_310b0)(u32)=nullptr;
    void (*call_30cd4)(u32,u32,u32)=nullptr;
    u32 (*call_f2d4)(u32,u32)=nullptr;
    void (*call_31380)(u32)=nullptr;
    u32 (*call_312c8)(u32)=nullptr;
    void (*call_312dc)(u32)=nullptr;
    u32 (*call_312b4)(u32)=nullptr;
    void (*call_2f5f4)(u32,u32)=nullptr;
    void (*call_31304)(u32,u32,u32)=nullptr;
    u32 (*call_312a0)(u32)=nullptr;
    void (*call_312ec)(u32,u32,u32)=nullptr;
    void (*call_28098)(u32)=nullptr;
    void (*call_290a8)(u32)=nullptr;
};

static void app08_identity_table(Machine& m,u32 base) noexcept {
    for(u32 i=0;i<0x100u;++i)*dg(m,base+i)=static_cast<u8>(i);
}

void function_0518c(Machine& m,Application08Boundary& b,u32 mode,u32 selected,u32 image,u32 x,
                     u32 grid,u32 row_limit,u32 source) noexcept {
    const u8 saved0=*dg(m,0x3A70u),saved1=*dg(m,0x3A71u),saved2=*dg(m,0x3A72u);
    if(b.call_291b0)b.call_291b0(*dg(m,0xC050u));
    app08_identity_table(m,0x12A30u);
    *dg(m,0x12A4Bu)=0xF1u;*dg(m,0x12A30u)=0xF0u;
    if(b.call_3059c)b.call_3059c(0x12A30u);
    if(b.call_2e510){b.call_2e510(0u,0u,image,0u);b.call_2e510(0u,0u,image+1u,0u);}
    u32 y=5u,slot_y=6u,rec=0x218E0u;
    for(u32 slot=0;slot<5u;++slot,y+=0x42u,slot_y+=0x42u,rec+=0x248u){
        if(slot!=4u&&static_cast<s32>(function_050dc(m,mode,slot))<0)continue;
        if(b.call_2975c)b.call_2975c(slot==selected?1u:0u,0u,y,load_le32(dg(m,0x258DCu))+0x4Au+mode,0u);
        const u32 src=slot==4u?load_le32(dg(m,0x60E0u)):rec;
        if(b.call_30508)b.call_30508(slot==selected?1u:2u,slot_y,0x30u,src,1u);
    }
    const u32 mode_count_off=mode<<5;
    u32 count_off=0u,coord_off=0u,source_row=source,row_limit_ptr=row_limit;
    for(u32 row=0;row<6u;++row,count_off+=4u,coord_off+=8u,source_row+=10u,++row_limit_ptr){
        const u32 out=load_le32(dg(m,0x12B58u+count_off));
        if(b.call_30d30)b.call_30d30(out,0xFFFFFFFFu,0x14u);
        const u32 nrow=row*10u;
        if(b.call_3210f)b.call_3210f(0x759Cu,load_le32(dg(m,0x60A4u)),row+1u);
        u32 px=load_le32(dg(m,0x63F0u+coord_off));
        const u32 py=load_le32(dg(m,0x63F4u+coord_off));
        if(b.call_30508)b.call_30508(px+2u,py,0x64u,0x759Cu,1u);
        if(b.call_3210f)b.call_3210f(0x759Cu,load_le32(dg(m,0x60A8u)),*dg(m,row_limit_ptr));
        if(b.call_30508)b.call_30508(px+0x0Eu,py,0x64u,0x759Cu,1u);
        u32 item=0u,out_off=0u,label=py+2u,text_y=py+0x19u,bar_y=py+0x20u;
        px+=0x2Du;
        u32 source_index=source_row;
        const s32 count=static_cast<s32>(load_le32(dg(m,0x6420u+count_off+mode_count_off)));
        while(static_cast<s32>(item)<count){
            const u8 cell=*dg(m,grid+nrow+item);
            const u8 lo=static_cast<u8>(cell&0x0Fu);
            if(lo!=0x0Fu){
                if(b.call_3060c)b.call_3060c(px,label,0x380u,1u);
                const u32 glyph=b.call_2e6f0?b.call_2e6f0(lo,1u):0u;
                if(b.call_3060c)b.call_3060c(px,text_y,glyph,1u);
                if(b.call_1c368)b.call_1c368(0x759Cu,source_index);
                if(b.call_30630)b.call_30630(px,bar_y,0x759Cu,1u,0x44u);
                *dg(m,out+out_off)=static_cast<u8>(item);
                *dg(m,out+out_off+1u)=lo;
                out_off+=2u;px+=0x0Cu;
            }
            ++source_index;++item;
        }
    }
    if(b.call_32220)b.call_32220(0xC054u,0u,0xFFu,0u,0u,0u);
    if(b.call_325b0)b.call_325b0(0xC054u,0u,x,0x388u,0x100u);
    if(b.call_32468){const u8 saved[3]={saved0,saved1,saved2};b.call_32468(0xC054u,0u,0xECu,6u,3u,saved);}
    app08_identity_table(m,0x12A30u);
    const struct MapByte{u32 off;u8 value;} map[]={
        {0x38u,0xF0u},{0x4Bu,0xF1u},{0x46u,0xF2u},{0x4Au,0xF3u},{0x42u,0xF4u},
        {0x39u,0xF5u},{0x40u,0xF6u},{0x3Bu,0xF7u},{0x3Au,0xF8u},{0x5Cu,0xF9u},
        {0x58u,0xFAu},{0x55u,0xFBu},{0x53u,0xFCu},{0x57u,0xFDu},{0x54u,0xFEu},
        {0x56u,0xFFu},{0x59u,0xEDu},{0x3Du,0xEEu},{0x3Cu,0xEFu}};
    for(const auto& q:map)*dg(m,0x12A30u+q.off)=q.value;
    if(b.call_2aa9c)b.call_2aa9c(0u,0x12A30u,0x880Du);
    if(b.call_1a5b0)b.call_1a5b0(load_le32(dg(m,0xC050u)),static_cast<u32>(static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x8600u))))),*dg(m,0x8602u));
    *dg(m,0xC050u)=static_cast<u8>(*dg(m,0xC050u)^1u);
    if(b.call_322ec)b.call_322ec(0xC054u,0u,0xFFu,0u,0u,0u);
}
static_assert(0x05578u-0x0518Cu==1004u);

void function_05578(Machine& m,Application08Boundary& b) noexcept {
    if(b.call_32220)b.call_32220(0xC054u,0u,0xFFu,0u,0u,0u);
    if(b.call_30d30)b.call_30d30(0xC054u,0u,0x300u);
    app08_identity_table(m,0x12A30u);
    const u8 values[19]={0x08u,0x1Bu,0x16u,0x1Au,0x12u,0x09u,0x10u,0x0Bu,0x0Au,0x5Cu,0x58u,0x55u,0x53u,0x57u,0x54u,0x56u,0x59u,0x0Du,0x0Cu};
    for(u32 i=0;i<16u;++i)*dg(m,0x12B20u+i)=values[i];
    for(u32 i=0;i<3u;++i)*dg(m,0x12B1Du+i)=values[16u+i];
    if(b.call_2aa9c)b.call_2aa9c(0u,0x12A30u,0x880Du);
    app08_identity_table(m,0x12A30u);
    if(b.call_3059c)b.call_3059c(0u);
}
static_assert(0x05695u-0x05578u==285u);

void function_05698(Machine& m,Application08Boundary& b,u32 mode,s32 mouse_x,s32 mouse_y,u32 image,
                     u32 grid_base,u32 limit_base,u32 source_base) noexcept {
    u32 row_grid=0u,count_off=0u,coord_off=0u,limit=limit_base;
    for(u32 row=0;row<6u;++row,row_grid+=10u,count_off+=4u,coord_off+=8u,++limit){
        const s32 left=static_cast<s32>(load_le32(dg(m,0x63F0u+coord_off)))+0x2D;
        if(mouse_x<=left||mouse_x>=left+0x78)continue;
        const s32 top=static_cast<s32>(load_le32(dg(m,0x63F4u+coord_off)));
        s32 delta=0;
        if(mouse_y>=top+2&&mouse_y<=top+9)delta=-1;
        if(mouse_y>=top+0x0D&&mouse_y<=top+0x15)delta=1;
        if(delta==0)continue;
        const s32 col=(mouse_x-left)/12;
        const u32 out=load_le32(dg(m,0x12B58u+count_off));
        const s8 item=static_cast<s8>(*dg(m,out+static_cast<u32>(col)*2u));
        if(item==-1)continue;
        s32 total=0;
        const s32 count=static_cast<s32>(load_le32(dg(m,0x6420u+count_off+(mode<<5))));
        for(s32 i=0;i<count;++i){const s8 id=static_cast<s8>(*dg(m,out+static_cast<u32>(i)*2u));if(id!=-1)total+=static_cast<s8>(*dg(m,out+static_cast<u32>(i)*2u+1u));}
        total+=delta;
        if(total>static_cast<s32>(*dg(m,limit)))continue;
        u8* qty=dg(m,out+static_cast<u32>(col)*2u+1u);
        if(delta==-1&&*qty==0u)return;
        *qty=static_cast<u8>(*qty+delta);
        const u32 draw_x=static_cast<u32>(left+col*12),draw_y=static_cast<u32>(top);
        if(b.call_291b0)b.call_291b0(*dg(m,0xC050u));
        if(b.call_296c0)b.call_296c0(draw_x,draw_y+1u,draw_x+12u,draw_y+0x62u);
        const u32 tile=b.call_2f29c?b.call_2f29c(row<3u?image:image+1u,0u):0u;
        if(b.call_2e438)b.call_2e438(draw_x,0u,row<3u?draw_x:draw_x-0xC8u,0x0Cu,tile);
        if(b.call_296c0)b.call_296c0(0u,0u,0x18Fu,0x13Fu);
        if(b.call_3060c)b.call_3060c(draw_x,draw_y+2u,0x380u,1u);
        const u32 glyph=b.call_2e6f0?b.call_2e6f0(*qty,1u):0u;
        if(b.call_3060c)b.call_3060c(draw_x,draw_y+0x19u,glyph,1u);
        const u32 source=source_base+row_grid+static_cast<u32>(static_cast<s32>(item));
        if(b.call_1c368)b.call_1c368(0x759Cu,source);
        if(b.call_30630)b.call_30630(draw_x,draw_y+0x20u,0x759Cu,1u,0x44u);
        const u32 cell=grid_base+source;
        *dg(m,cell)=static_cast<u8>((*dg(m,cell)&0xF0u)|(*qty&0x0Fu));
        if(b.call_1a8c8)b.call_1a8c8(load_le32(dg(m,0xC050u)),draw_x,draw_y,0x0Cu,0x64u);
        return;
    }
}
static_assert(0x05983u-0x05698u==747u);

void function_05984(Machine& m,Application08Boundary& b,u32 image,u32 x) noexcept {
    if(*dg(m,0x6460u)!=0u)return;
    *dg(m,0x6460u)=1u;
    if(b.call_325b0)b.call_325b0(0xC054u,0u,x,0x390u,0x100u);
    if(b.call_291b0)b.call_291b0(*dg(m,0xC050u));
    if(b.call_2e510){b.call_2e510(0u,0u,image,0u);b.call_2e510(0u,0u,image+1u,0u);}
    if(b.call_1a510)b.call_1a510(load_le32(dg(m,0xC050u)));
    *dg(m,0xC050u)=static_cast<u8>(*dg(m,0xC050u)^1u);
    if(b.call_29234)b.call_29234(0xC054u);
}
static_assert(0x05A11u-0x05984u==141u);

void function_05a14(Machine& m,Application08Boundary& b,u32 x) noexcept {
    if(*dg(m,0x6460u)==0u)return;
    *dg(m,0x6460u)=0u;
    if(b.call_325b0)b.call_325b0(0xC054u,0u,x,0x390u,0x100u);
    if(b.call_32468){const u8 saved[3]={*dg(m,0x3A73u),*dg(m,0x3A74u),*dg(m,0x3A75u)};b.call_32468(0xC054u,0u,0xECu,6u,3u,saved);}
    if(b.call_29234)b.call_29234(0xC054u);
    if(b.call_1a5b0)b.call_1a5b0(load_le32(dg(m,0xC050u)),static_cast<u32>(static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x8600u))))),*dg(m,0x8602u));
    *dg(m,0xC050u)=static_cast<u8>(*dg(m,0xC050u)^1u);
}
static_assert(0x05AA3u-0x05A14u==143u);

[[nodiscard]] u32 function_05aa4(Machine& m,Application08Boundary& b,u32 mode,u32 selected,u32 image,u32 x,
                                  u32 grid,u32 limit,u32 source) noexcept {
    for(u32 off=4u;off!=0x1Cu;off+=4u){const u32 p=b.call_3104c?b.call_3104c(0x14u):0u;store_le32(dg(m,0x12B54u+off),p);}
    function_0518c(m,b,mode,selected,image,x,grid,limit,source);
    u32 choice=selected;
    for(;;){
        const u32 event=b.call_1a1f4?b.call_1a1f4():0u;
        if(event==0u){
            const s32 mx=static_cast<s16>(load_le16(dg(m,0x9E31u))),my=static_cast<s16>(load_le16(dg(m,0x9E33u)));
            if(my==0x13F||mx==0x18F)function_05984(m,b,image,x);else function_05a14(m,b,x);
            continue;
        }
        function_05a14(m,b,x);
        const s32 mx=static_cast<s16>(load_le16(dg(m,0x9E31u))),my=static_cast<s16>(load_le16(dg(m,0x9E33u)));
        bool accept=false;
        if(mx<0x0F){
            const s32 t=my-5,q=t/0x42,r=t%0x42;
            if(r>=0&&r<0x30&&q!=static_cast<s32>(selected)){
                if(q==4||static_cast<s32>(function_050dc(m,mode,static_cast<u32>(q)))>=0){choice=static_cast<u32>(q);accept=true;}
            }
        }
        if(accept)break;
        function_05698(m,b,mode,mx,my,image,grid,limit,source);
        if(b.call_1a1f4)while(b.call_1a1f4()!=0u){}
    }
    function_05578(m,b);
    for(u32 off=4u;off!=0x1Cu;off+=4u)if(b.call_310b0)b.call_310b0(load_le32(dg(m,0x12B54u+off)));
    return choice;
}
static_assert(0x05BDCu-0x05AA4u==312u);

void function_05bdc(Machine& m,Application08Boundary& b,u32 mode,u32 slot,u32 index,u32 source) noexcept {
    if(b.call_30d30)b.call_30d30(0x6208u,0u,8u);
    if(b.call_30cd4)b.call_30cd4(0x6208u,source+index*8u,8u);
    const u32 rec=slot*0x248u;
    if(mode==0u){
        if(static_cast<s8>(*dg(m,0x218F5u+rec))==1)return;
        const u32 cls=*dg(m,0x2191Bu+rec);
        for(u32 i=0;i<8u;++i)if(*dg(m,0x62B0u+index*8u+i)!=0u)*dg(m,0x6208u+i)=static_cast<u8>(*dg(m,0x6208u+i)+*dg(m,0x6160u+cls*8u+i));
    }else{
        const u32 cls=*dg(m,0x21919u+rec),first=*dg(m,0x613Fu+cls);
        if(first!=0u&&first<8u)for(u32 i=first;i<8u;++i)*dg(m,0x6208u+i)=0u;
    }
}
static_assert(0x05C81u-0x05BDCu==165u);

[[nodiscard]] u32 function_05c84(Machine& m,Application08Boundary& b) noexcept {
    u32 slot=0u; s32 gate=-1;
    while(slot<4u){gate=static_cast<s32>(function_050dc(m,0u,slot));if(gate>=0)break;++slot;}
    if(gate<0||slot==4u)return 0u;
    u32 touched=0u;
    for(;;){
        if(gate<0||slot==4u)return touched;
        const u32 rec=slot*0x248u,idx=function_050dc(m,0u,slot);touched=1u;
        const s32 value=static_cast<s16>(load_le16(dg(m,0x21900u+rec+idx*2u)))-1;
        gate=static_cast<s8>(*dg(m,0x218F5u+rec));
        function_05bdc(m,b,0u,slot,static_cast<u32>(value),gate==1?0x6350u:0x62B0u);
        slot=function_05aa4(m,b,0u,slot,load_le32(dg(m,0x258DCu))+0x30u,0x0Au,0x218E0u+rec+0xBAu,0x6208u,0x92u);
    }
}
static_assert(0x05D34u-0x05C84u==176u);

[[nodiscard]] u32 function_05d34(Machine& m,Application08Boundary& b) noexcept {
    u32 slot=0u; s32 initial=-1;
    while(slot<4u){initial=static_cast<s32>(function_050dc(m,1u,slot));if(initial>=0)break;++slot;}
    if(initial<0||slot==4u)return 0u;
    u32 touched=0u;
    while(slot!=4u){
        const u32 rec=slot*0x248u,idx=function_050dc(m,1u,slot);touched=1u;
        const s32 value=static_cast<s16>(load_le16(dg(m,0x21900u+rec+idx*2u)))-1;
        function_05bdc(m,b,1u,slot,static_cast<u32>(value),0x6210u);
        if(b.call_f2d4&&b.call_f2d4(slot,0x48u)!=0u)*dg(m,0x620Au)=static_cast<u8>(*dg(m,0x620Au)<<1);
        slot=function_05aa4(m,b,1u,slot,load_le32(dg(m,0x258DCu))+0x32u,0x0Bu,0x218E0u+rec+0x6Au,0x6208u,0x42u);
    }
    return touched;
}
static_assert(0x05DD0u-0x05D34u==156u);

void function_05dd0(Machine& m,Application08Boundary& b) noexcept {
    const u32 name=load_le32(dg(m,0x6138u));
    if(b.call_31380)b.call_31380(name);
    const u32 old=b.call_312c8?b.call_312c8(name):0u;if(b.call_312dc)b.call_312dc(old);
    const u32 h=b.call_312b4?b.call_312b4(name):0xFFFFFFFFu;
    if(h==0xFFFFFFFFu){if(b.call_2f5f4)b.call_2f5f4(load_le32(dg(m,0x60DCu)),0x36u);return;}
    if(b.call_31304)b.call_31304(0xC025u,0x0Eu,h);if(b.call_312dc)b.call_312dc(h);
}
static_assert(0x05E38u-0x05DD0u==104u);

void function_05e38(Machine& m,Application08Boundary& b) noexcept {
    const u32 h=b.call_312a0?b.call_312a0(load_le32(dg(m,0x6138u))):0xFFFFFFFFu;
    if(h==0xFFFFFFFFu)return;
    if(b.call_312ec)b.call_312ec(0xC025u,0x0Eu,h);if(b.call_312dc)b.call_312dc(h);
    const u32 p=load_le32(dg(m,0xF050u));*dg(m,p+0x6Cu)=*dg(m,0xC02Eu);
    if(b.call_325b0)b.call_325b0(0xC054u,0u,load_le32(dg(m,0xC034u)),0x390u,0x100u);
    if(b.call_29234)b.call_29234(0xC054u);
    if(b.call_28098)b.call_28098(*dg(m,0xC02Bu));if(b.call_290a8)b.call_290a8(*dg(m,0xC02Cu));
}
static_assert(0x05EC3u-0x05E38u==139u);

void function_05eec(Machine& m,Application08Boundary& b) noexcept {
    u32 y=0x17u,x=0x0Fu;
    if(b.call_291b0)b.call_291b0(*dg(m,0xC050u));
    for(u32 i=0;i<10u;++i,y+=0x1Eu,x+=0x1Eu){
        if(b.call_3060c)b.call_3060c(y,0x0Fu,load_le32(dg(m,0x6110u+i*4u)),1u);
        const u32 image=load_le32(dg(m,0x258DCu))+0x40u;
        u32 a0=0u,a2=0x78u;
        switch(i){
            case 0:a2=0x78u+static_cast<u32>(*dg(m,0xC025u))*10u;break;
            case 1:a0=static_cast<u32>(*dg(m,0xC026u))+2u;break;
            case 2:a0=static_cast<u32>(*dg(m,0xC027u))+2u;break;
            case 3:a0=static_cast<u32>(*dg(m,0xC028u))+2u;break;
            case 4:a0=static_cast<u32>(*dg(m,0xC029u))+2u;break;
            case 5:a2=0x78u+static_cast<u32>(*dg(m,0xC02Au));break;
            case 6:a0=static_cast<u32>(*dg(m,0xC02Bu))+2u;break;
            case 7:a0=static_cast<u32>(*dg(m,0xC02Cu))+2u;break;
            case 8:a2=0x78u+static_cast<u32>(*dg(m,0xC02Eu))*10u;break;
            case 9:a0=static_cast<u32>(*dg(m,0xC02Fu))+2u;break;
        }
        if(b.call_2975c)b.call_2975c(a0,x,a2,image,0u);
    }
}
static_assert(0x06008u-0x05EECu==284u);

// -----------------------------------------------------------------------------
// Batch 09: 06008..06CAC settings-panel drawing/interaction, save serializer,
// and tile/record flag probe. CODE.2 bytes are controlling evidence.
// -----------------------------------------------------------------------------

struct Application09Boundary {
    void (*call_291b0)(u32)=nullptr;
    u32 (*call_30ca2)()=nullptr;
    void (*call_2975c)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_2e374)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_3060c)(u32,u32,u32,u32)=nullptr;
    void (*call_305a8)(u32,u32,u32,u32)=nullptr;
    u32 (*call_30070)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_05eec)()=nullptr;
    void (*call_1a510)(u32)=nullptr;
    void (*call_1a9f8)()=nullptr;
    u32 (*call_1a1f4)()=nullptr;
    void (*call_2a380)(u32,u32,u32,u32)=nullptr;
    void (*call_1a8c8)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_325b0)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_29234)(u32)=nullptr;
    void (*call_28098)(u32)=nullptr;
    void (*call_290a8)(u32)=nullptr;
    void (*call_28e58)(u32,u32,u32)=nullptr;
    void (*call_20994)()=nullptr;
    void (*call_30040)(u32)=nullptr;
    void (*call_31380)(u32)=nullptr;
    u32 (*call_312c8)(u32)=nullptr;
    void (*call_312dc)(u32)=nullptr;
    u32 (*call_312b4)(u32)=nullptr;
    u32 (*call_31304)(u32,u32,u32)=nullptr;
    u32 (*call_31304_local)(const u8*,u32,u32)=nullptr;
    u32 (*call_30700)()=nullptr;
    u32 (*call_1e8c8)(s32,s32)=nullptr;
};

static void app09_copy_cstr(Machine& m,u32 dst,u32 src) noexcept {
    for(;;){const u8 v=*dg(m,src++);*dg(m,dst++)=v;if(v==0u)break;}
}
static void app09_append_cstr(Machine& m,u32 dst,u32 src) noexcept {
    while(*dg(m,dst)!=0u)++dst;
    app09_copy_cstr(m,dst,src);
}

void function_06008(Machine& m,Application09Boundary& b) noexcept {
    if(b.call_291b0)b.call_291b0(*dg(m,0xC050u));
    const u32 image=load_le32(dg(m,0x258DCu))+0x35u;
    for(u32 y=0u;y<0x190u;y+=0x10u) {
        for(u32 x=0u;x<0x140u;x+=8u) {
            const u32 r=b.call_30ca2?b.call_30ca2():0u;
            if(b.call_2975c)b.call_2975c((r*4u)>>15,y,x,image,0u);
        }
    }
    for(u32 x=8u;x<0x138u;x+=8u) {
        u32 r=b.call_30ca2?b.call_30ca2():0u;
        if(b.call_2975c)b.call_2975c(((r*2u)>>15)+9u,0u,x,image,0u);
        r=b.call_30ca2?b.call_30ca2():0u;
        if(b.call_2975c)b.call_2975c(((r*2u)>>15)+5u,0x180u,x,image,0u);
    }
    for(u32 y=0x10u;y<0x180u;y+=0x10u) {
        u32 r=b.call_30ca2?b.call_30ca2():0u;
        if(b.call_2975c)b.call_2975c(((r*2u)>>15)+0x0Cu,y,0u,image,0u);
        r=b.call_30ca2?b.call_30ca2():0u;
        if(b.call_2975c)b.call_2975c(((r*2u)>>15)+0x0Eu,y,0x138u,image,0u);
    }
    if(b.call_2975c){
        b.call_2975c(8u,0u,0u,image,0u);
        b.call_2975c(0x0Bu,0u,0x138u,image,0u);
        b.call_2975c(4u,0x180u,0u,image,0u);
        b.call_2975c(7u,0x180u,0x138u,image,0u);
    }
    if(b.call_2e374){
        b.call_2e374(0x0Fu,0x78u,0x18u,0x89u,0x3Du);
        b.call_2e374(0xA5u,0x78u,0x18u,0xAFu,0x3Du);
        b.call_2e374(0xFFu,0x78u,0x18u,0xBBu,0x3Du);
    }
    if(b.call_3060c){
        b.call_3060c(0x17u,0x7Cu,load_le32(dg(m,0x60E4u)),1u);
        b.call_3060c(0xADu,0x7Cu,load_le32(dg(m,0x60E4u)),1u);
        b.call_3060c(0x107u,0x7Cu,load_le32(dg(m,0x60E4u)),1u);
    }
    if(b.call_305a8){
        b.call_305a8(0x17u,0xFCu,load_le32(dg(m,0x60E8u)),1u);
        b.call_305a8(0xADu,0x122u,load_le32(dg(m,0x60E8u)),1u);
        b.call_305a8(0x107u,0x12Du,load_le32(dg(m,0x60E8u)),1u);
    }
    u32 y=0x0Fu;
    for(u32 off=4u;off<=0x28u;off+=4u,y+=0x1Eu) {
        const u32 h=b.call_30070?b.call_30070(y,0x78u,0x19u,0xC8u,0xFFu):0u;
        store_le32(dg(m,0x12B2Cu+off),h);
    }
    if(b.call_05eec)b.call_05eec();
    if(b.call_1a510)b.call_1a510(load_le32(dg(m,0xC050u)));
    *dg(m,0xC050u)=static_cast<u8>(*dg(m,0xC050u)^1u);
    if(b.call_1a9f8)b.call_1a9f8();
}
static_assert(0x062ABu-0x06008u==675u);

void function_062ac(Machine& m,Application09Boundary& b) noexcept {
    for(;;) {
        if(!b.call_1a1f4||b.call_1a1f4()==0u)return;
        const s32 delta=static_cast<s16>(load_le16(dg(m,0x9E33u)))-0x90;
        s32 index=delta/10;
        if(index<0)index=0;
        if(index>9)index=9;
        if(static_cast<u32>(index)==*dg(m,0xC025u))continue;
        *dg(m,0xC025u)=static_cast<u8>(index);
        if(b.call_291b0)b.call_291b0(*dg(m,0xC050u));
        if(b.call_2a380)b.call_2a380(0u,0x0Fu,0x78u,load_le32(dg(m,0x12B30u)));
        if(b.call_2975c)b.call_2975c(1u,0x0Fu,static_cast<u32>(index*10+0x78),load_le32(dg(m,0x258DCu))+0x40u,0u);
        if(b.call_1a8c8)b.call_1a8c8(load_le32(dg(m,0xC050u)),0x0Fu,0x78u,0x19u,0xC8u);
    }
}
static_assert(0x06352u-0x062ACu==166u);

void function_06354(Machine& m,Application09Boundary& b) noexcept {
    for(;;) {
        if(!b.call_1a1f4||b.call_1a1f4()==0u)return;
        const s32 delta=static_cast<s16>(load_le16(dg(m,0x9E33u)))-0x90;
        s32 index=delta/10;
        if(index<0)index=0;
        if(index>0x0E)index=0x0E;
        if(static_cast<u32>(index)==*dg(m,0xC02Eu))continue;
        *dg(m,0xC02Eu)=static_cast<u8>(index);
        if(b.call_291b0)b.call_291b0(*dg(m,0xC050u));
        if(b.call_2a380)b.call_2a380(0u,0xFFu,0x78u,load_le32(dg(m,0x12B50u)));
        if(b.call_2975c)b.call_2975c(1u,0xFFu,static_cast<u32>(index*10+0x78),load_le32(dg(m,0x258DCu))+0x40u,0u);
        if(b.call_1a8c8)b.call_1a8c8(load_le32(dg(m,0xC050u)),0xFFu,0x78u,0x19u,0xC8u);
        const u32 p=load_le32(dg(m,0xF050u));
        *dg(m,p+0x6Cu)=*dg(m,0xC02Eu);
        if(b.call_325b0)b.call_325b0(0xC054u,0u,load_le32(dg(m,0xC034u)),0x390u,0x100u);
        if(b.call_29234)b.call_29234(0xC054u);
    }
}
static_assert(0x06441u-0x06354u==237u);

void function_06444(Machine& m,Application09Boundary& b) noexcept {
    for(;;) {
        if(!b.call_1a1f4||b.call_1a1f4()==0u)break;
        s32 index=static_cast<s16>(load_le16(dg(m,0x9E33u)))-0x90;
        if(index<0)index=0;
        if(index>0x80)index=0x80;
        if(static_cast<u32>(index)==*dg(m,0xC02Au))continue;
        *dg(m,0xC02Au)=static_cast<u8>(index);
        if(b.call_291b0)b.call_291b0(*dg(m,0xC050u));
        if(b.call_2a380)b.call_2a380(0u,0xA5u,0x78u,load_le32(dg(m,0x12B44u)));
        if(b.call_2975c)b.call_2975c(1u,0xA5u,static_cast<u32>(index+0x78),load_le32(dg(m,0x258DCu))+0x40u,0u);
        if(b.call_1a8c8)b.call_1a8c8(load_le32(dg(m,0xC050u)),0xA5u,0x78u,0x19u,0xC8u);
    }
    const s32 factor=0x80-static_cast<s16>(load_le16(dg(m,0x258B2u)));
    const s32 numer=factor*static_cast<s32>(*dg(m,0xC02Au))+0x40;
    const s32 quotient=numer/0x80;
    *dg(m,0xC02Du)=static_cast<u8>(0x80-quotient);
}
static_assert(0x06520u-0x06444u==220u);

void function_06548(Machine& m,Application09Boundary& b) noexcept {
    function_06008(m,b);
    for(;;) {
        while(b.call_1a1f4&&b.call_1a1f4()!=0u){}
        while(b.call_1a1f4&&b.call_1a1f4()==0u){}
        const u16 buttons=load_le16(dg(m,0x9E35u));
        if((buttons&2u)!=0u)break;
        const s32 y=static_cast<s16>(load_le16(dg(m,0x9E33u)));
        const s32 x=static_cast<s16>(load_le16(dg(m,0x9E31u)));
        if(y<0x78)continue;
        const s32 adjusted=x-0x0F;
        const s32 rem=adjusted%0x1E;
        const s32 action=adjusted/0x1E;
        if(rem>=0x19)continue;
        if(action>=0&&action<=9) {
            switch(action) {
                case 0: function_062ac(m,b);break;
                case 1: if(y<0xA8)*dg(m,0xC026u)^=1u;break;
                case 2: if(y<0xA8)*dg(m,0xC027u)^=1u;break;
                case 3: if(y<0xA8)*dg(m,0xC028u)^=1u;break;
                case 4: if(y<0xA8)*dg(m,0xC029u)^=1u;break;
                case 5: function_06444(m,b);break;
                case 6:
                    if(y<0xA8){*dg(m,0xC02Bu)^=1u;if(b.call_28098)b.call_28098(*dg(m,0xC02Bu));}
                    break;
                case 7:
                    if(y<0xA8){
                        *dg(m,0xC02Cu)^=1u;
                        if(b.call_290a8)b.call_290a8(*dg(m,0xC02Cu));
                        if(*dg(m,0xC02Cu)!=0u&&b.call_28e58)b.call_28e58(*dg(m,0x258B4u),1u,0x9C4u);
                    }
                    break;
                case 8: function_06354(m,b);break;
                case 9: if(y<0xA8&&b.call_20994)b.call_20994();break;
            }
        }
        if(b.call_05eec)b.call_05eec();
        if(b.call_1a510)b.call_1a510(load_le32(dg(m,0xC050u)));
        if(b.call_1a9f8)b.call_1a9f8();
        *dg(m,0xC050u)=static_cast<u8>(*dg(m,0xC050u)^1u);
    }
    for(u32 off=0u;off<0x24u;off+=4u)if(b.call_30040)b.call_30040(load_le32(dg(m,0x12B30u+off)));
}
static_assert(0x066E6u-0x06548u==414u);

[[nodiscard]] u32 function_066e8(Machine& m,Application09Boundary& b,u32 slot) noexcept {
    app09_copy_cstr(m,0x12990u,0xC020u);
    app09_append_cstr(m,0x12990u,0x398u);
    app09_append_cstr(m,0x12990u,0x39Cu);
    if(b.call_31380)b.call_31380(0x12990u);
    const u32 probe=b.call_312c8?b.call_312c8(0x12990u):0u;
    if(b.call_312dc)b.call_312dc(probe);
    const u32 h=b.call_312b4?b.call_312b4(0x12990u):0xFFFFFFFFu;
    if(h==0xFFFFFFFFu)return 1u;
    if(slot==0xFFFFFFFFu) {
        for(u32 i=0;i<5u;++i)*dg(m,0x12990u+i)=*dg(m,0x3A8u+i);
    } else {
        app09_copy_cstr(m,0x12990u,0x6461u+slot*0x32u);
    }
    auto write_guest=[&](u32 ptr,u32 n)->bool{return b.call_31304&&b.call_31304(ptr,n,h)==n;};
    auto fail=[&]()->u32{if(b.call_312dc)b.call_312dc(h);return 1u;};
    const struct Spec{u32 ptr,n;} first[]={
        {0x12990u,0x32u},{0x22200u,0x20u},{0xC6C8u,4u},{0x255D0u,4u},{0x255D4u,4u},
        {0xC658u,4u},{0xF03Cu,2u},{0xF03Eu,2u},{0xFD08u,4u},{0x6DAEu,2u}
    };
    for(const auto& q:first)if(!write_guest(q.ptr,q.n))return fail();
    const u32 runtime=b.call_30700?b.call_30700():0u;
    if(!b.call_31304_local||b.call_31304_local(reinterpret_cast<const u8*>(&runtime),4u,h)!=4u)return fail();
    const Spec second[]={
        {0xCF4Cu,4u},{0x8600u,2u},{0x8602u,1u},{0x218E0u,0x920u},{0xC038u,4u}
    };
    for(const auto& q:second)if(!write_guest(q.ptr,q.n))return fail();
    const u32 records=load_le32(dg(m,0xC038u))*0x16u;
    if(!write_guest(0x248ECu,records))return fail();
    if(!write_guest(0xD8A8u,8u))return fail();
    if(!write_guest(0xA7E4u,4u))return fail();
    for(u32 i=0;i<0x3Eu;++i) {
        const u32 d=0x7330u+i*10u;
        const u32 n=static_cast<u32>(*dg(m,d))*2u;
        if(!write_guest(load_le32(dg(m,d+5u)),n))return fail();
        if(!write_guest(d+9u,1u))return fail();
    }
    const Spec tail[]={
        {0xF920u,4u},{0xF91Cu,4u},{0x67ECu,0x0Fu},{0xA808u,0x16D0u},{0x6FA0u,0x10u}
    };
    for(const auto& q:tail)if(!write_guest(q.ptr,q.n))return fail();
    if(b.call_312dc)b.call_312dc(h);
    return 0u;
}
static_assert(0x06C67u-0x066E8u==1407u);

void function_06c68(Machine& m,Application09Boundary& b) noexcept {
    const s32 x=static_cast<s16>(load_le16(dg(m,0x255D2u)));
    const s32 y=static_cast<s16>(load_le16(dg(m,0x255D6u)));
    const u32 p=b.call_1e8c8?b.call_1e8c8(x,y):0u;
    store_le32(dg(m,0xCF6Cu),0u);
    if(p!=0u&&*dg(m,p)==6u&&(*dg(m,p+1u)&4u)!=0u&&*dg(m,p+6u)!=0u)
        store_le32(dg(m,0xCF6Cu),1u);
}
static_assert(0x06CADu-0x06C68u==69u);


// -----------------------------------------------------------------------------
// Batch 10: 06CAD shared save-load tail, 07070 resource-panel helpers,
// background renderer, FCB delete wrapper, wildcard file copy, and 07938 loader.
// -----------------------------------------------------------------------------

struct Application10Boundary {
    u32 (*call_3131c)(u32,u32,u32)=nullptr;
    u32 (*call_312ec)(u32,u32,u32)=nullptr;
    u32 (*call_312ec_local)(u8*,u32,u32)=nullptr;
    void (*call_1c0c0)(u32)=nullptr;
    void (*call_30738)(u32)=nullptr;
    void (*call_1a344)(s32,u32)=nullptr;
    void (*call_1e74c)(u32)=nullptr;
    u32 (*call_3104c)(u32)=nullptr;
    void (*call_30cd4)(u32,u32,u32)=nullptr;
    void (*call_310b0)(u32)=nullptr;
    void (*call_312dc)(u32)=nullptr;
    void (*call_31edc)(u32)=nullptr;
    void (*call_31f38)(u32)=nullptr;
    void (*call_181b4)(u32)=nullptr;
    u32 (*call_312a0)(u32)=nullptr;
    u32 (*call_30070)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_2975c)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_3060c)(u32,u32,u32,u32)=nullptr;
    void (*call_30040)(u32)=nullptr;
    u32 (*call_30ca2)()=nullptr;
    void (*call_291b0)(u32)=nullptr;
    void (*call_1a510)(u32)=nullptr;
    void (*call_1a9f8)()=nullptr;
    void (*call_33971)(u32,u32,u32)=nullptr;
    void (*dos_int21_13)(u32)=nullptr;
    u32 (*call_339a2_local)(u32,u32,u8*)=nullptr;
    u32 (*call_339cf_local)(u8*)=nullptr;
    u32 (*call_3127c)(u32)=nullptr;
    void (*call_31380)(u32)=nullptr;
    u32 (*call_312c8)(u32)=nullptr;
    u32 (*call_312b4)(u32)=nullptr;
    u32 (*call_31304)(u32,u32,u32)=nullptr;
};

static u32 app10_read_local_u32(Application10Boundary& b,u32 h) noexcept {
    u8 raw[4]{};
    if(b.call_312ec_local)(void)b.call_312ec_local(raw,4u,h);
    return load_le32(raw);
}

static u16 app10_read_local_u16(Application10Boundary& b,u32 h) noexcept {
    u8 raw[2]{};
    if(b.call_312ec_local)(void)b.call_312ec_local(raw,2u,h);
    return load_le16(raw);
}

static u8 app10_read_local_u8(Application10Boundary& b,u32 h) noexcept {
    u8 raw=0u;
    if(b.call_312ec_local)(void)b.call_312ec_local(&raw,1u,h);
    return raw;
}

// 06CAD is a backward-jump shared tail of 07938, not an independently called
// routine. EBX and ESI both carry the already-open save handle at entry.
static u32 app10_load_tail_06cad(Machine& m,Application10Boundary& b,Application09Boundary& a9,u32 h) noexcept {
    if(b.call_3131c)(void)b.call_3131c(0x32u,0u,h);
    auto read_guest=[&](u32 ptr,u32 n)->u32{return b.call_312ec?b.call_312ec(ptr,n,h):0u;};

    (void)read_guest(0x22200u,0x20u);
    const u32 saved_level=app10_read_local_u32(b,h);
    (void)read_guest(0x255D0u,4u);
    (void)read_guest(0x255D4u,4u);
    const u32 saved_mode=app10_read_local_u32(b,h);
    if(b.call_1c0c0)b.call_1c0c0(saved_mode);
    if(*dg(m,0xC02Fu)!=0u){store_le16(dg(m,0x255D0u),0x8000u);store_le16(dg(m,0x255D4u),0x8000u);}

    (void)read_guest(0xF03Cu,2u);
    (void)read_guest(0xF03Eu,2u);
    (void)read_guest(0xFD08u,4u);
    (void)read_guest(0x6DAEu,2u);
    const u32 runtime_state=app10_read_local_u32(b,h);
    if(b.call_30738)b.call_30738(runtime_state);
    (void)read_guest(0xCF4Cu,4u);
    const u16 word_state=app10_read_local_u16(b,h);
    const u8 byte_state=app10_read_local_u8(b,h);
    if(b.call_1a344)b.call_1a344(static_cast<s16>(word_state),byte_state);

    (void)read_guest(0x218E0u,0x920u);
    const u32 record_count=app10_read_local_u32(b,h);
    (void)read_guest(0x248ECu,record_count*0x16u);
    for(u32 off=0u;off<0x920u;off+=0x248u){
        if((load_le16(dg(m,0x218F0u+off))&0x8000u)!=0u)continue;
        const s8 a=static_cast<s8>(*dg(m,0x218F5u+off));
        const s8 c=static_cast<s8>(*dg(m,0x218F6u+off));
        const s8 d=static_cast<s8>(*dg(m,0x218F7u+off));
        if(a==1)*dg(m,0x21A11u+off)=static_cast<u8>(*dg(m,0x21A11u+off)|4u);
        if(a==4||c==4||d==4)*dg(m,0x21A11u+off)=static_cast<u8>(*dg(m,0x21A11u+off)|2u);
        if(a==3||c==3||d==3)*dg(m,0x21A11u+off)=static_cast<u8>(*dg(m,0x21A11u+off)|1u);
    }

    (void)read_guest(0xD8A8u,8u);
    const u32 elapsed_state=app10_read_local_u32(b,h);
    if(b.call_1e74c)b.call_1e74c(elapsed_state);
    for(u32 i=0u;i<0x3Eu;++i){
        const u32 d=0x7330u+i*10u;
        (void)read_guest(load_le32(dg(m,d+5u)),static_cast<u32>(*dg(m,d))*2u);
        (void)read_guest(d+9u,1u);
    }
    const u32 state_31edc=app10_read_local_u32(b,h);
    const u32 state_31f38=app10_read_local_u32(b,h);
    if(read_guest(0x12990u,0x0Fu)==0x0Fu&&b.call_30cd4)b.call_30cd4(0x67ECu,0x12990u,0x0Fu);

    const u32 temp=b.call_3104c?b.call_3104c(0x248u):0u;
    for(u32 i=0u;i<10u;++i){
        const u32 n=read_guest(temp,0x248u);
        if(n==0x248u&&b.call_30cd4)b.call_30cd4(0xA808u+i*0x248u,temp,n);
    }
    const u32 tail_n=read_guest(temp,0x10u);
    if(tail_n==0x10u&&b.call_30cd4)b.call_30cd4(0x6FA0u,temp,tail_n);
    if(b.call_310b0)b.call_310b0(temp);
    if(b.call_312dc)b.call_312dc(h);

    for(u32 i=0u;i<4u;++i)*dg(m,0xCF5Au+i)=1u;
    if(b.call_31edc)b.call_31edc(state_31edc);
    if(b.call_31f38)b.call_31f38(state_31f38);
    store_le32(dg(m,0x6F90u),0u);store_le32(dg(m,0x6F94u),0u);
    store_le32(dg(m,0x6F98u),0u);store_le32(dg(m,0x6F9Cu),0u);
    store_le32(dg(m,0xC6C8u),0xFFFFFFFFu);
    if(b.call_181b4)b.call_181b4(saved_level);
    store_le32(dg(m,0xBF44u),0x270Fu);
    store_le32(dg(m,0xC038u),record_count);
    function_06c68(m,a9);
    return 0u;
}
static_assert(0x0706Eu-0x06CADu==961u);

// Ten 50-byte labels/resources backed by numbered files 01..10. Missing files
// fall back to the C string pointed to by 60EC.
void function_07070(Machine& m,Application10Boundary& b,u32 image) noexcept {
    u32 sprite=0u,x=0x15u,handle_off=0u,text_y=0x1Du;
    u32 buf=0x6461u;
    for(s32 i=0;i<10;++i){
        buf+=0x32u;
        const s32 n=i+1;
        *dg(m,0x6690u)=static_cast<u8>(n/10+0x30);
        *dg(m,0x6691u)=static_cast<u8>(n%10+0x30);
        const u32 h=b.call_312a0?b.call_312a0(0x668Cu):0xFFFFFFFFu;
        if(h==0xFFFFFFFFu){app09_copy_cstr(m,buf,load_le32(dg(m,0x60ECu)));}
        else {if(b.call_312ec)(void)b.call_312ec(buf,0x32u,h);if(b.call_312dc)b.call_312dc(h);}
        const u32 resource=b.call_30070?b.call_30070(text_y,0x64u,0x0Fu,0xD8u,0xFFu):0u;
        store_le32(dg(m,0x12B30u+handle_off),resource);
        if(b.call_2975c)b.call_2975c(sprite,x,0x14u,image,0u);
        if(b.call_3060c)b.call_3060c(text_y,0x64u,buf,1u);
        sprite+=2u;x+=0x25u;handle_off+=4u;text_y+=0x25u;
    }
}
static_assert(0x0719Cu-0x07070u==300u);

void function_0719c(Machine& m,Application10Boundary& b) noexcept {
    for(u32 off=0u;off<0x28u;off+=4u)if(b.call_30040)b.call_30040(load_le32(dg(m,0x12B30u+off)));
}
static_assert(0x071B8u-0x0719Cu==28u);

void function_071b8(Machine& m,Application10Boundary& b,u32 image) noexcept {
    if(b.call_291b0)b.call_291b0(*dg(m,0xC050u));
    for(u32 x=0u;x<0x190u;x+=0x10u){
        for(u32 y=0u;y<0x140u;y+=8u){
            const u32 r=b.call_30ca2?b.call_30ca2():0u;
            if(b.call_2975c)b.call_2975c((r*4u)>>15,x,y,image+0x35u,0u);
        }
    }
    for(u32 y=8u;y<0x138u;y+=8u){
        u32 r=b.call_30ca2?b.call_30ca2():0u;
        if(b.call_2975c)b.call_2975c(((r*2u)>>15)+9u,0u,y,image+0x35u,0u);
        r=b.call_30ca2?b.call_30ca2():0u;
        if(b.call_2975c)b.call_2975c(((r*2u)>>15)+5u,0x180u,y,image+0x35u,0u);
    }
    for(u32 x=0x10u;x<0x180u;x+=0x10u){
        u32 r=b.call_30ca2?b.call_30ca2():0u;
        if(b.call_2975c)b.call_2975c(((r*2u)>>15)+0x0Cu,x,0u,image+0x35u,0u);
        r=b.call_30ca2?b.call_30ca2():0u;
        if(b.call_2975c)b.call_2975c(((r*2u)>>15)+0x0Eu,x,0x138u,image+0x35u,0u);
    }
    if(b.call_2975c){
        b.call_2975c(8u,0u,0u,image+0x35u,0u);
        b.call_2975c(0x0Bu,0u,0x138u,image+0x35u,0u);
        b.call_2975c(4u,0x180u,0u,image+0x35u,0u);
        b.call_2975c(7u,0x180u,0x138u,image+0x35u,0u);
    }
    function_07070(m,b,image);
    if(b.call_1a510)b.call_1a510(load_le32(dg(m,0xC050u)));
    *dg(m,0xC050u)=static_cast<u8>(*dg(m,0xC050u)^1u);
    if(b.call_1a9f8)b.call_1a9f8();
    if(b.call_291b0)b.call_291b0(*dg(m,0xC050u));
}
static_assert(0x07376u-0x071B8u==446u);

void function_07378(Machine& m,Application10Boundary& b) noexcept {
    *dg(m,0x12B70u)=0u;
    if(b.call_33971)b.call_33971(0x12B71u,0x3B0u,0x0Bu);
    if(b.dos_int21_13)b.dos_int21_13(0x12B70u);
}
static_assert(0x0739Du-0x07378u==37u);

// Copy every wildcard match under src_root into dst_root. The DOS find-state is
// the exact 2Ch-byte local block; its filename field begins at +1Eh.
[[nodiscard]] u32 function_073a0(Machine& m,Application10Boundary& b,u32 src_root,u32 dst_root) noexcept {
    u8 find_state[0x2C]{};
    app09_copy_cstr(m,0x128F0u,src_root);
    app09_append_cstr(m,0x128F0u,0x3BCu);
    if(!b.call_339a2_local||b.call_339a2_local(0x128F0u,0u,find_state)!=0u)return 0u;
    for(;;){
        app09_copy_cstr(m,0x12850u,src_root);
        app09_append_cstr(m,0x12850u,0x398u);
        u32 end=0x12850u;while(*dg(m,end)!=0u)++end;
        for(u32 i=0u;;++i){const u8 c=find_state[0x1Eu+i];*dg(m,end+i)=c;if(c==0u)break;}
        const u32 size=b.call_3127c?b.call_3127c(0x12850u):0u;
        const u32 in=b.call_312a0?b.call_312a0(0x12850u):0xFFFFFFFFu;
        if(in==0xFFFFFFFFu)return 1u;
        const u32 buf=b.call_3104c?b.call_3104c(size):0u;
        if(b.call_312ec)(void)b.call_312ec(buf,size,in);
        if(b.call_312dc)b.call_312dc(in);

        app09_copy_cstr(m,0x127B0u,dst_root);
        app09_append_cstr(m,0x127B0u,0x398u);
        end=0x127B0u;while(*dg(m,end)!=0u)++end;
        for(u32 i=0u;;++i){const u8 c=find_state[0x1Eu+i];*dg(m,end+i)=c;if(c==0u)break;}
        if(b.call_31380)b.call_31380(0x127B0u);
        const u32 probe=b.call_312c8?b.call_312c8(0x127B0u):0u;
        if(b.call_312dc)b.call_312dc(probe);
        const u32 out=b.call_312b4?b.call_312b4(0x127B0u):0xFFFFFFFFu;
        if(out==0xFFFFFFFFu){if(b.call_310b0)b.call_310b0(buf);return 1u;}
        const u32 wrote=b.call_31304?b.call_31304(buf,size,out):0u;
        if(wrote!=size){if(b.call_310b0)b.call_310b0(buf);if(b.call_312dc)b.call_312dc(out);return 1u;}
        if(b.call_312dc)b.call_312dc(out);
        if(b.call_310b0)b.call_310b0(buf);
        if(!b.call_339cf_local||b.call_339cf_local(find_state)!=0u)return 0u;
    }
}
static_assert(0x075C9u-0x073A0u==553u);

// 07938 opens the constructed save pathname and jumps backward to 06CAD on
// success. The C++ helper above preserves that shared-tail ownership.
[[nodiscard]] u32 function_07938(Machine& m,Application10Boundary& b,Application09Boundary& a9) noexcept {
    *dg(m,0xD681u)=0u;*dg(m,0xD682u)=0u;
    app09_copy_cstr(m,0x12990u,0xC020u);
    app09_append_cstr(m,0x12990u,0x398u);
    app09_append_cstr(m,0x12990u,0x39Cu);
    const u32 h=b.call_312a0?b.call_312a0(0x12990u):0xFFFFFFFFu;
    if(h==0xFFFFFFFFu)return 1u;
    return app10_load_tail_06cad(m,b,a9,h);
}
static_assert(0x079E0u-0x07938u==168u);

// -----------------------------------------------------------------------------
// Batch 11: 075CC..0808A filename editing, save/load slot workflows,
// confirmation UI, and modal message helper. 0808C..080AB is a CS table.
// -----------------------------------------------------------------------------

struct Application11Boundary {
    u32 (*call_30aec)()=nullptr;
    s32 (*call_30288)(u32,u32)=nullptr;
    void (*call_30354)(s32,u32)=nullptr;
    void (*call_30d30)(u32,u32,u32)=nullptr;
    void (*call_171fc)(u32)=nullptr;
    void (*call_33a38)(u32)=nullptr;
    u32 (*call_33a5a)(u32)=nullptr;
    void (*call_1c21c)(u32)=nullptr;
    void (*call_2f580)(u32)=nullptr;
    void (*call_30508)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_1c40c)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_13fd4)(u32,u32,u32,u32,u32,u32,u32)=nullptr;
};

static u32 app11_strlen(Machine& m,u32 p) noexcept {
    u32 n=0u;while(*dg(m,p+n)!=0u)++n;return n;
}
static void app11_slot_digits(Machine& m,u32 one_based) noexcept {
    *dg(m,0x6690u)=static_cast<u8>(one_based/10u+0x30u);
    *dg(m,0x6691u)=static_cast<u8>(one_based%10u+0x30u);
    *dg(m,0x6692u)=0u;
}

// Edit one 49-character slot label in the global 12760h scratch buffer.
// The first accepted printable key clears the inherited label; backspace or an
// invalid first key consumes that first-edit state without clearing it.
[[nodiscard]] u32 function_075cc(Machine& m,Application11Boundary& b,Application09Boundary& a9,
                                  u32 text,u32 slot) noexcept {
    if(a9.call_291b0)a9.call_291b0(*dg(m,0xC050u));
    app09_copy_cstr(m,0x12760u,text);
    const u32 y=slot*0x25u+0x1Du;
    const u32 handle=load_le32(dg(m,0x12B30u+slot*4u));
    bool first_edit=true,redraw=true;
    for(;;) {
        if(redraw) {
            if(a9.call_2a380)a9.call_2a380(0u,y,0x64u,handle);
            if(a9.call_3060c)a9.call_3060c(y,0x64u,0x12760u,1u);
            if(b.call_30354)b.call_30354(0x7Cu,1u);
            if(a9.call_1a8c8)a9.call_1a8c8(load_le32(dg(m,0xC050u)),y,0x64u,0x0Au,0xD8u);
        }
        const u32 key=b.call_30aec?b.call_30aec():0u;
        if(key==0x1Bu)return 0u;
        if(key==0x0Du){app09_copy_cstr(m,text,0x12760u);return 1u;}
        redraw=false;
        if(key==8u) {
            const u32 n=app11_strlen(m,0x12760u);
            if(n!=0u){*dg(m,0x12760u+n-1u)=0u;redraw=true;}
        } else {
            const u32 n=app11_strlen(m,0x12760u);
            if(n<0x31u) {
                const s32 width=b.call_30288?b.call_30288(0x12760u,1u):0;
                if(width<0xD2) {
                    const u8 idx=static_cast<u8>(static_cast<u8>(key)+1u);
                    const u8 cls=*dg(m,0x165Cu+idx);
                    if((cls&0xE0u)!=0u||(cls&2u)!=0u||(cls&4u)!=0u) {
                        if(first_edit) {
                            if(b.call_30d30)b.call_30d30(0x12760u,0u,0x50u);
                            else for(u32 i=0;i<0x50u;++i)*dg(m,0x12760u+i)=0u;
                        }
                        const u32 end=app11_strlen(m,0x12760u);
                        *dg(m,0x12760u+end)=static_cast<u8>(key);
                        redraw=true;
                    }
                }
            }
        }
        first_edit=false;
    }
}
static_assert(0x0779Fu-0x075CCu==467u);

void function_0803c(Machine& m,Application11Boundary& b,Application09Boundary& a9,u32 text) noexcept;

// Save the live game to slot 00, create/validate its directory, then copy the
// working save tree into it. Failure paths intentionally use two different
// message mechanisms, matching the binary.
void function_077a0(Machine& m,Application11Boundary& b,Application10Boundary& a10,
                    Application09Boundary& a9) noexcept {
    *dg(m,0xA79Du)=0u;
    if(b.call_171fc)b.call_171fc(load_le32(dg(m,0xC6C8u)));
    *dg(m,0xA79Du)=1u;
    const u32 save=function_066e8(m,a9,0u);
    if(save!=0u){function_0803c(m,b,a9,load_le32(dg(m,0x60B8u)));return;}
    app11_slot_digits(m,0u);
    if(b.call_33a38)b.call_33a38(0x668Cu);
    if(b.call_33a5a&&b.call_33a5a(0x668Cu)!=0u){if(b.call_1c21c)b.call_1c21c(load_le32(dg(m,0x60B4u)));return;}
    function_07378(m,a10);
    if(b.call_33a5a&&b.call_33a5a(0x3C4u)!=0u&&b.call_2f580)b.call_2f580(load_le32(dg(m,0x60B0u)));
    if(function_073a0(m,a10,0x668Cu,0xC020u)==1u){if(b.call_1c21c)b.call_1c21c(load_le32(dg(m,0x60B4u)));return;}
    *dg(m,0x6692u)=0x5Cu;
}
static_assert(0x07871u-0x077A0u==209u);

// Prepare slot-00 loading. The 07938 routine is entered by fall-through in the
// original; the structured call here preserves its side effects while accounting
// only for the 07874..07937 prefix in this batch.
void function_07874(Machine& m,Application11Boundary& b,Application10Boundary& a10,
                    Application09Boundary& a9) noexcept {
    app11_slot_digits(m,0u);
    if(b.call_33a5a&&b.call_33a5a(0x668Cu)!=0u){if(b.call_1c21c)b.call_1c21c(load_le32(dg(m,0x60BCu)));return;}
    if(b.call_33a5a&&b.call_33a5a(0x3C4u)!=0u&&b.call_2f580)b.call_2f580(load_le32(dg(m,0x60B0u)));
    if(b.call_33a5a&&b.call_33a5a(0xC020u)!=0u){if(b.call_1c21c)b.call_1c21c(load_le32(dg(m,0x60BCu)));return;}
    function_07378(m,a10);
    if(b.call_33a5a&&b.call_33a5a(0x3C4u)!=0u&&b.call_2f580)b.call_2f580(load_le32(dg(m,0x60B0u)));
    if(function_073a0(m,a10,0xC020u,0x668Cu)==1u&&b.call_2f580)b.call_2f580(load_le32(dg(m,0x60C0u)));
    *dg(m,0x6692u)=0x5Cu;
    (void)function_07938(m,a10,a9);
}
static_assert(0x07938u-0x07874u==196u);

static bool app11_slot_hit(Machine& m,s32& slot,u32& x) noexcept {
    const s32 mx=static_cast<s16>(load_le16(dg(m,0x9E31u)));
    const s32 my=static_cast<s16>(load_le16(dg(m,0x9E33u)));
    if(mx<0x15||mx>=0x17B||my<0x14||my>=0x5C)return false;
    const s32 d=mx-0x15;
    slot=d/0x25;
    if(d%0x25>0x19)return false;
    x=static_cast<u32>(slot*0x25+0x15);
    return true;
}
static void app11_highlight_slot(Machine& m,Application09Boundary& a9,s32 slot,u32 x,u32 image) noexcept {
    if(a9.call_291b0)a9.call_291b0(*dg(m,0xC050u));
    if(a9.call_2975c)a9.call_2975c(static_cast<u32>(slot*2+1),x,0x14u,image,0u);
    if(a9.call_1a8c8)a9.call_1a8c8(load_le32(dg(m,0xC050u)),x,0x14u,0x19u,0x48u);
}

// Interactive save-slot selector. Right-click returns 2 immediately without
// releasing the ten background handles, exactly as encoded.
[[nodiscard]] u32 function_079e0(Machine& m,Application11Boundary& b,Application10Boundary& a10,
                                  Application09Boundary& a9) noexcept {
    const u32 image=load_le32(dg(m,0x258DCu))+0x41u;
    function_071b8(m,a10,image);
    u32 result=0u;
    for(;;) {
        if(a9.call_1a1f4)while(a9.call_1a1f4()!=0u){}
        if(a9.call_1a1f4)while(a9.call_1a1f4()==0u){}
        if((load_le16(dg(m,0x9E35u))&2u)!=0u)return 2u;
        s32 slot=0;u32 x=0u;if(!app11_slot_hit(m,slot,x))continue;
        app11_highlight_slot(m,a9,slot,x,image);
        const u32 one=static_cast<u32>(slot+1);
        if(function_075cc(m,b,a9,0x6461u+one*0x32u,static_cast<u32>(slot))==0u)break;
        if(function_066e8(m,a9,one)!=0u){result=1u;break;}
        app11_slot_digits(m,one);
        if(b.call_33a38)b.call_33a38(0x668Cu);
        if(b.call_33a5a&&b.call_33a5a(0x668Cu)!=0u){result=1u;break;}
        function_07378(m,a10);
        if(b.call_33a5a&&b.call_33a5a(0x3C4u)!=0u&&b.call_2f580)b.call_2f580(load_le32(dg(m,0x60B0u)));
        result=function_073a0(m,a10,0x668Cu,0xC020u);
        *dg(m,0x6692u)=0x5Cu;
        break;
    }
    function_0719c(m,a10);
    return result;
}
static_assert(0x07B94u-0x079E0u==436u);

// Interactive load-slot selector. Copy failure reports 60C0h and returns to
// polling; a missing working save path returns 1. Right-click again returns 2
// without releasing the ten panel handles.
[[nodiscard]] u32 function_07b94(Machine& m,Application11Boundary& b,Application10Boundary& a10,
                                  Application09Boundary& a9) noexcept {
    const u32 image=load_le32(dg(m,0x258DCu))+0x64u;
    function_071b8(m,a10,image);
    for(;;) {
        if(a9.call_1a1f4)while(a9.call_1a1f4()!=0u){}
        if(a9.call_1a1f4)while(a9.call_1a1f4()==0u){}
        if((load_le16(dg(m,0x9E35u))&2u)!=0u)return 2u;
        s32 slot=0;u32 x=0u;if(!app11_slot_hit(m,slot,x))continue;
        app11_highlight_slot(m,a9,slot,x,image);
        const u32 one=static_cast<u32>(slot+1);
        app11_slot_digits(m,one);
        if(b.call_33a5a&&b.call_33a5a(0xC020u)!=0u){function_0719c(m,a10);return 1u;}
        function_07378(m,a10);
        if(b.call_33a5a&&b.call_33a5a(0x3C4u)!=0u&&b.call_2f580)b.call_2f580(load_le32(dg(m,0x60B0u)));
        if(function_073a0(m,a10,0x668Cu,0xC020u)==1u){
            if(b.call_2f580)b.call_2f580(load_le32(dg(m,0x60C0u)));
            continue;
        }
        *dg(m,0x6692u)=0x5Cu;
        const u32 result=function_07938(m,a10,a9);
        function_0719c(m,a10);
        return result;
    }
}
static_assert(0x07D43u-0x07B94u==431u);

static u32 app11_random(Application09Boundary& a9) noexcept {return a9.call_30ca2?a9.call_30ca2():0u;}
static void app11_choice_finish(Machine& m,Application09Boundary& a9,u32 sprite,u32 y) noexcept {
    if(a9.call_2975c)a9.call_2975c(sprite,0xC8u,y,load_le32(dg(m,0x258DCu))+0x63u,0u);
    if(a9.call_1a510)a9.call_1a510(load_le32(dg(m,0xC050u)));
    *dg(m,0xC050u)=static_cast<u8>(*dg(m,0xC050u)^1u);
    if(a9.call_291b0)a9.call_291b0(*dg(m,0xC050u));
}

// Randomized confirmation screen. The upper button returns 0; the lower button
// returns 1. X accepts both C8h and E0h endpoints, while Y uses half-open ranges.
[[nodiscard]] u32 function_07d44(Machine& m,Application11Boundary& b,Application09Boundary& a9,u32 text) noexcept {
    if(a9.call_291b0)a9.call_291b0(*dg(m,0xC050u));
    const u32 image=load_le32(dg(m,0x258DCu))+0x35u;
    for(u32 y=0xA0u;y<0xF0u;y+=0x10u)for(u32 x=0x20u;x<0x120u;x+=8u)
        if(a9.call_2975c)a9.call_2975c((app11_random(a9)*4u)>>15,y,x,image,0u);
    for(u32 x=0x20u;x<0x118u;x+=8u){
        if(a9.call_2975c)a9.call_2975c(((app11_random(a9)*2u)>>15)+9u,0xA0u,x,image,0u);
        if(a9.call_2975c)a9.call_2975c(((app11_random(a9)*2u)>>15)+5u,0xE0u,x,image,0u);
    }
    for(u32 y=0xB0u;y<0xE0u;y+=0x10u){
        if(a9.call_2975c)a9.call_2975c(((app11_random(a9)*2u)>>15)+0x0Cu,y,0x20u,image,0u);
        if(a9.call_2975c)a9.call_2975c(((app11_random(a9)*2u)>>15)+0x0Eu,y,0x118u,image,0u);
    }
    if(a9.call_2975c){
        a9.call_2975c(8u,0xA0u,0x20u,image,0u);a9.call_2975c(0x0Bu,0xA0u,0x118u,image,0u);
        a9.call_2975c(4u,0xE0u,0x20u,image,0u);a9.call_2975c(7u,0xE0u,0x118u,image,0u);
    }
    if(b.call_30508)b.call_30508(0xAAu,0x20u,0x100u,text,1u);
    const u32 buttons=load_le32(dg(m,0x258DCu))+0x63u;
    if(a9.call_2975c){a9.call_2975c(0u,0xC8u,0x60u,buttons,0u);a9.call_2975c(2u,0xC8u,0xB0u,buttons,0u);}
    if(a9.call_1a510)a9.call_1a510(load_le32(dg(m,0xC050u)));
    *dg(m,0xC050u)=static_cast<u8>(*dg(m,0xC050u)^1u);
    if(a9.call_1a9f8)a9.call_1a9f8();
    if(a9.call_291b0)a9.call_291b0(*dg(m,0xC050u));
    for(;;){
        if(a9.call_1a1f4)while(a9.call_1a1f4()!=0u){}
        if(a9.call_1a1f4)while(a9.call_1a1f4()==0u){}
        const s32 x=static_cast<s16>(load_le16(dg(m,0x9E31u)));
        if(x<0xC8||x>0xE0)continue;
        const s32 y=static_cast<s16>(load_le16(dg(m,0x9E33u)));
        if(y>=0x60&&y<0x90){app11_choice_finish(m,a9,1u,0x60u);return 0u;}
        if(y>=0xB0&&y<0xE0){app11_choice_finish(m,a9,3u,0xB0u);return 1u;}
    }
}
static_assert(0x0803Cu-0x07D44u==760u);

void function_0803c(Machine& m,Application11Boundary& b,Application09Boundary& a9,u32 text) noexcept {
    if(a9.call_1a9f8)a9.call_1a9f8();
    if(b.call_1c40c)b.call_1c40c(load_le32(dg(m,0xC050u)),0x0Fu,0x40u,0x32u,0xC0u);
    if(b.call_13fd4)b.call_13fd4(0u,0x14u,0x46u,0x12u,0xB4u,text,1u);
    if(a9.call_1a510)a9.call_1a510(load_le32(dg(m,0xC050u)));
    *dg(m,0xC050u)=static_cast<u8>(*dg(m,0xC050u)^1u);
}
static_assert(0x0808Bu-0x0803Cu==79u);
// -----------------------------------------------------------------------------
// Batch 12: 080AC..08CD7 left-panel dispatcher, status expirations,
// timed-effect sweeper, moving-record constructor, and record compaction.
// 0808C..080AB and 086A0..086C3 are directly decoded CS/data tables.
// -----------------------------------------------------------------------------

struct Application12Boundary {
    u32 (*call_19ccc)(u32)=nullptr;
    u32 (*call_19cf4)()=nullptr;
    u32 (*call_19cc4)()=nullptr;
    void (*call_1a9f8)()=nullptr;
    u32 (*call_1a1f4)()=nullptr;
    void (*call_291b0)(u32)=nullptr;
    u32 (*call_30070)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_30040)(u32)=nullptr;
    void (*call_2975c)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_30508)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_1a8c8)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_1a344)(s32,u32)=nullptr;
    u32 (*call_05c84)()=nullptr;
    u32 (*call_05d34)()=nullptr;
    void (*call_1f8fc)()=nullptr;
    void (*call_325b0)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_322ec)(u32,u32,u32,u32,u32,u32)=nullptr;
    void (*call_03634)(u32)=nullptr;
    void (*call_0461c)()=nullptr;
    u32 (*call_07b94)()=nullptr;
    void (*call_0803c)(u32)=nullptr;
    void (*call_171fc)(u32)=nullptr;
    u32 (*call_079e0)()=nullptr;
    u32 (*call_07d44)(u32)=nullptr;
    void (*call_205b4)(u32)=nullptr;
    void (*call_06548)()=nullptr;
    void (*call_05dd0)()=nullptr;
    void (*call_2939c)(u32,u32,u32,u32)=nullptr;
    void (*call_22d70)()=nullptr;

    void (*call_24a9c)(u32,u32)=nullptr;
    void (*call_3210f)(u32,u32,u32)=nullptr;
    void (*call_1c368)(u32,u32)=nullptr;
    void (*call_1c21c)(u32)=nullptr;
    void (*call_31f38)(u32)=nullptr;
    void (*call_1202c)(u32,u32)=nullptr;
    u32 (*call_12098)(u32)=nullptr;
    u32 (*call_1c328)(u32,u32,u32,u32)=nullptr;

    void (*call_e578)(u32)=nullptr;
    u32 (*call_30ca2)()=nullptr;
    void (*call_28110)(u32,u32,u32,u32)=nullptr;
    u32 (*call_2c9dc)(u32,u32)=nullptr;
    void (*call_22e90)(u32,u32)=nullptr;
    u32 (*call_1f1c8)()=nullptr;
    void (*call_1ea04)()=nullptr;
    void (*call_1ea78)()=nullptr;
    void (*call_2f580)(u32)=nullptr;
    void (*call_30cd4)(u32,u32,u32)=nullptr;
};

static s32 app12_signed_row(Machine& m) noexcept {
    return static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x9E33u))))/0x28;
}

static void app12_draw_menu_row(Machine& m,Application12Boundary& b,s32 row,u32 sprite) noexcept {
    const u32 y=static_cast<u32>(row*0x28);
    if(b.call_2975c)b.call_2975c(sprite,0u,y,load_le32(dg(m,0x258DCu))+0x49u,0u);
    if(b.call_30508)b.call_30508(0x1Bu,y,0x28u,load_le32(dg(m,0x60F0u+static_cast<u32>(row)*4u)),1u);
}

static void app12_update_menu_row(Machine& m,Application12Boundary& b,s32 row) noexcept {
    if(b.call_1a8c8)b.call_1a8c8(load_le32(dg(m,0xC050u)),0u,static_cast<u32>(row*0x28),0x40u,0x28u);
}

static void app12_refresh_menu_capture(Machine& m,Application12Boundary& b,u32& capture,bool overlay) noexcept {
    if(b.call_1f8fc)b.call_1f8fc();
    if(b.call_291b0)b.call_291b0(*dg(m,0xC050u));
    if(b.call_30040)b.call_30040(capture);
    capture=b.call_30070?b.call_30070(0u,0u,0x40u,0x140u,0xFFu):0u;
    if(overlay) {
        if(b.call_325b0)b.call_325b0(0xC054u,0u,load_le32(dg(m,0xC034u)),0x390u,0x100u);
        if(b.call_322ec)b.call_322ec(0xC054u,0u,0xFFu,0u,0u,0u);
    }
}

// 080AC..0869E: eight-way left-panel dispatcher using the already-proven
// 0808C..080AB CS table. The two consecutive 1A1F4 polls before accepting a
// press, signed IDIV row selection, and per-case capture refresh asymmetry are
// retained exactly.
void function_080ac(Machine& m,Application12Boundary& b) noexcept {
    if(static_cast<s16>(load_le16(dg(m,0x9E31u)))>=0x40) {
        store_le16(dg(m,0xA6C2u),0u);
        return;
    }

    *dg(m,0xA79Du)=1u;
    if(b.call_19ccc)(void)b.call_19ccc(0u);
    if(b.call_1a9f8)b.call_1a9f8();
    const u8 saved_d680=*dg(m,0xD680u);
    *dg(m,0xD680u)=3u;
    if(b.call_291b0)b.call_291b0(*dg(m,0xC050u));
    u32 capture=b.call_30070?b.call_30070(0u,0u,0x40u,0x140u,0xFFu):0u;
    bool redraw=true;
    s32 selected=0;

    for(;;) {
        if(static_cast<s16>(load_le16(dg(m,0x9E31u)))>=0x40)break;

        if(redraw) {
            selected=app12_signed_row(m);
            if(b.call_291b0)b.call_291b0(*dg(m,0xC050u));
            for(s32 row=0;row<8;++row)app12_draw_menu_row(m,b,row,row==selected?1u:0u);
            if(b.call_1a8c8)b.call_1a8c8(load_le32(dg(m,0xC050u)),0u,0u,0x40u,0x140u);
            redraw=false;
        }

        for(;;) {
            const u32 first=b.call_1a1f4?b.call_1a1f4():0u;
            if(first!=0u)break;
            if(static_cast<s16>(load_le16(dg(m,0x9E31u)))>=0x40)break;
            const s32 next=app12_signed_row(m);
            if(next==selected)continue;
            const s32 old=selected;
            selected=next;
            if(b.call_291b0)b.call_291b0(*dg(m,0xC050u));
            app12_draw_menu_row(m,b,old,0u);
            app12_draw_menu_row(m,b,selected,1u);
            app12_update_menu_row(m,b,old);
            app12_update_menu_row(m,b,selected);
        }

        // 82B2 performs a second poll even after the first poll reported a press.
        if(!b.call_1a1f4||b.call_1a1f4()==0u)continue;

        const s32 old=selected;
        selected=app12_signed_row(m);
        if(b.call_291b0)b.call_291b0(*dg(m,0xC050u));
        app12_draw_menu_row(m,b,old,0u);
        app12_draw_menu_row(m,b,selected,2u);
        app12_update_menu_row(m,b,old);
        app12_update_menu_row(m,b,selected);

        const s32 action=app12_signed_row(m);
        if(static_cast<u32>(action)>7u)continue;

        switch(action) {
            case 0:
                if(b.call_0461c)b.call_0461c();
                if(b.call_1a1f4)while(b.call_1a1f4()!=0u){}
                continue;
            case 1:
            case 2: {
                *dg(m,0x6460u)=0u;
                const s32 saved_word=static_cast<s16>(load_le16(dg(m,0x8600u)));
                const u8 saved_byte=*dg(m,0x8602u);
                if(b.call_1a344)b.call_1a344(-1,0u);
                const u32 changed=(action==1)
                    ? (b.call_05c84?b.call_05c84():0u)
                    : (b.call_05d34?b.call_05d34():0u);
                if(changed!=0u) {
                    *dg(m,0xCF58u)=0u;
                    app12_refresh_menu_capture(m,b,capture,true);
                    redraw=true;
                }
                if(b.call_1a344)b.call_1a344(saved_word,saved_byte);
                continue;
            }
            case 3:
                redraw=true;
                if(b.call_03634)b.call_03634(0u);
                continue;
            case 4: {
                const u32 r=b.call_07b94?b.call_07b94():0u;
                if(r==1u&&b.call_0803c)b.call_0803c(load_le32(dg(m,0x60C4u)));
                app12_refresh_menu_capture(m,b,capture,false);
                redraw=true;
                continue;
            }
            case 5: {
                *dg(m,0xA79Du)=0u;
                if(b.call_171fc)b.call_171fc(load_le32(dg(m,0xC6C8u)));
                *dg(m,0xA79Du)=1u;
                const u32 r=b.call_079e0?b.call_079e0():0u;
                if(r==1u&&b.call_0803c)b.call_0803c(load_le32(dg(m,0x60C8u)));
                app12_refresh_menu_capture(m,b,capture,false);
                redraw=true;
                continue;
            }
            case 6: {
                const u32 r=b.call_07d44?b.call_07d44(load_le32(dg(m,0x60CCu))):0u;
                if(r==0u) {
                    app12_refresh_menu_capture(m,b,capture,false);
                    redraw=true;
                } else if(b.call_205b4)b.call_205b4(0xFFFFFFFFu);
                continue;
            }
            case 7:
                if(b.call_06548)b.call_06548();
                if(b.call_05dd0)b.call_05dd0();
                app12_refresh_menu_capture(m,b,capture,false);
                redraw=true;
                continue;
        }

    }

    if(b.call_291b0)b.call_291b0(*dg(m,0xC050u));
    if(b.call_2939c)b.call_2939c(0u,0u,0u,capture);
    if(b.call_30040)b.call_30040(capture);
    if(b.call_1a8c8)b.call_1a8c8(load_le32(dg(m,0xC050u)),0u,0u,0x40u,0x140u);
    if(b.call_22d70)b.call_22d70();
    *dg(m,0xA79Du)=0u;
    store_le16(dg(m,0xA6C2u),0u);
    *dg(m,0xD680u)=saved_d680;
    if(b.call_19cf4)(void)b.call_19cf4();
    store_le16(dg(m,0x1457Cu),0u);
}
static_assert(0x0869Fu-0x080ACu==1523u);

// 086C4..0871A: compare two per-slot words against one 67CAh table word and
// notify 24A9C independently for each match.
void function_086c4(Machine& m,Application12Boundary& b,u32 slot,u32 index) noexcept {
    const u32 base=slot*0x248u;
    const u16 wanted=load_le16(dg(m,0x67CAu+index*2u));
    if(load_le16(dg(m,0x2193Cu+base))==wanted&&b.call_24a9c)b.call_24a9c(slot,0u);
    if(load_le16(dg(m,0x2193Eu+base))==wanted&&b.call_24a9c)b.call_24a9c(slot,1u);
}
static_assert(0x0871Bu-0x086C4u==87u);

// 0871C..0893B: expire one 21A12h status bit, format its message, clear its
// deadline, then perform the address-distinct bit-specific side effect.
void function_0871c(Machine& m,Application12Boundary& b,u32 slot,u32 bit) noexcept {
    const u32 base=slot*0x248u;
    const u32 mask=1u<<(bit&31u);
    store_le32(dg(m,0x21A12u+base),load_le32(dg(m,0x21A12u+base))^mask);
    if(b.call_3210f)b.call_3210f(0x759Cu,load_le32(dg(m,0x669Cu)),0x218E0u+base);
    const u32 end=0x759Cu+app11_strlen(m,0x759Cu);
    if(b.call_1c368)b.call_1c368(end,bit+0x24Cu);
    app09_append_cstr(m,0x759Cu,load_le32(dg(m,0x66A0u)));
    if(b.call_1c21c)b.call_1c21c(0x759Cu);
    *dg(m,0xCF5Au+slot)=1u;
    store_le32(dg(m,0x21A16u+base+bit*4u),0u);

    switch(bit) {
        case 1u:
        case 10u:
            if(b.call_31f38)b.call_31f38(load_le32(dg(m,0xF91Cu)));
            break;
        case 6u:
            store_le32(dg(m,0x21AD2u+base),load_le32(dg(m,0x21AD2u+base))-1u);
            break;
        case 11u:
            store_le32(dg(m,0x21ACAu+base),0u);
            break;
        case 12u:
            if(b.call_1202c)b.call_1202c(slot,0u-load_le32(dg(m,0x21A03u+base)));
            break;
        case 13u:
            *dg(m,0x21915u+base)=static_cast<u8>(*dg(m,0x21915u+base)-*dg(m,0x21A07u+base));
            break;
        case 17u:
            store_le32(dg(m,0xFD08u),load_le32(dg(m,0xFD08u))-1u);
            break;
        case 18u:
            store_le16(dg(m,0xF03Cu),static_cast<u16>(load_le16(dg(m,0xF03Cu))-1u));
            break;
        case 20u:
            for(u32 i=0;i<4u;++i)*dg(m,0xCF5Au+i)=1u;
            store_le16(dg(m,0x6DAEu),static_cast<u16>(load_le16(dg(m,0x6DAEu))-1u));
            break;
        case 21u:
            for(u32 i=0;i<4u;++i)*dg(m,0xCF5Au+i)=1u;
            for(u32 off=0u;off<0x920u;off+=0x248u)
                *dg(m,0x21A14u+off)=static_cast<u8>(*dg(m,0x21A14u+off)&0xDFu);
            break;
        case 23u:
            if(b.call_1202c)b.call_1202c(slot,0u-load_le32(dg(m,0x21A9Eu+base)));
            break;
        case 25u:
            store_le16(dg(m,0xF03Eu),static_cast<u16>(load_le16(dg(m,0xF03Eu))-1u));
            break;
        default:
            break;
    }
}
static_assert(0x0893Cu-0x0871Cu==544u);

// 0893C..08A7C: sweep four party slots for nine event deadlines, one refresh
// deadline, and 26 status-bit deadlines. All deadline comparisons are signed.
void function_0893c(Machine& m,Application12Boundary& b) noexcept {
    const u32 now=b.call_19cc4?b.call_19cc4():0u;
    for(u32 slot=0u,base=0u;slot<4u;++slot,base+=0x248u) {
        if(static_cast<u32>(load_le16(dg(m,0x218F0u+base)))>=0x4000u)continue;

        if(load_le32(dg(m,0x21AA2u+base))!=0u) {
            for(u32 event=0u;event<9u;++event) {
                const u32 d=0x21AA6u+base+event*4u;
                const u32 deadline=load_le32(dg(m,d));
                if(deadline!=0u&&static_cast<s32>(now)>=static_cast<s32>(deadline)) {
                    store_le32(dg(m,0x21AA2u+base),load_le32(dg(m,0x21AA2u+base))^(1u<<(event&31u)));
                    *dg(m,0xCF5Au+slot)=1u;
                    store_le32(dg(m,d),0u);
                    function_086c4(m,b,slot,event);
                }
            }
        }

        const u32 refresh=load_le32(dg(m,0x21ACEu+base));
        if(refresh!=0u&&static_cast<s32>(now)>=static_cast<s32>(refresh)) {
            if(b.call_1202c)b.call_1202c(slot,1u);
            store_le32(dg(m,0x21ACEu+base),0u);
            const u32 current=b.call_12098?b.call_12098(slot):0u;
            if(current!=static_cast<u32>(*dg(m,0x21912u+base))) {
                const u32 factor=b.call_1c328?b.call_1c328(2u,4u,0u,0u):0u;
                const u32 fresh=b.call_19cc4?b.call_19cc4():0u;
                store_le32(dg(m,0x21ACEu+base),fresh+factor*0x2A30u);
            }
        }

        if(load_le32(dg(m,0x21A12u+base))!=0u) {
            for(u32 bit=0u;bit<26u;++bit) {
                const u32 deadline=load_le32(dg(m,0x21A16u+base+bit*4u));
                if(static_cast<s32>(deadline)>0&&static_cast<s32>(now)>=static_cast<s32>(deadline))
                    function_0871c(m,b,slot,bit);
            }
        }
    }
}
static_assert(0x08A7Du-0x0893Cu==321u);

constexpr u32 kMovingRecordSoundOffsets[9]={
    0x1Bu,0x36u,0x2Bu,0x0Du,0x1Du,0x3Au,0x38u,0x38u,0x1Bu
};

static u32 app12_abs_bits(u32 v) noexcept {
    return (v&0x80000000u)!=0u ? 0u-v : v;
}

// 08A80..08C87: create one 16h-byte moving record. Packed coordinates are
// passed as dwords, but the tile calculation intentionally consumes their high
// words with 16-bit IMUL/truncation. The sound-table index has no encoded bounds.
void function_08a80(Machine& m,Application12Boundary& b,u32 type,s32 angle,s32 owner,
                     u32 packed_x,u32 packed_y) noexcept {
    if(b.call_e578)b.call_e578(static_cast<u32>(owner));
    if(static_cast<s32>(load_le32(dg(m,0xC038u)))>=0x96)return;

    const u32 rec=0x248ECu+load_le32(dg(m,0xC038u))*0x16u;
    store_le32(dg(m,rec),packed_x);
    store_le32(dg(m,rec+4u),packed_y);
    store_le32(dg(m,rec+8u),static_cast<u32>(angle));
    const u16 tile=static_cast<u16>(
        static_cast<u16>(static_cast<u32>(static_cast<u16>(packed_x>>16))*load_le16(dg(m,0x25619u)))+
        static_cast<u16>(packed_y>>16));
    store_le16(dg(m,rec+0x0Cu),tile);
    store_le16(dg(m,rec+0x0Eu),static_cast<u16>(type));
    store_le16(dg(m,rec+0x10u),0u);

    u16 flags=static_cast<u16>(load_le16(dg(m,rec+0x14u))&0xC43Fu);
    const u16 owner_bits=static_cast<u16>((static_cast<u8>(owner)&7u)<<11);
    flags=static_cast<u16>(flags|owner_bits);
    store_le16(dg(m,rec+0x14u),flags);

    if(type==8u) {
        const u32 r=b.call_30ca2?b.call_30ca2():0u;
        store_le16(dg(m,rec+0x12u),static_cast<u16>(((r*10u)>>15)+0x14u));
    }

    store_le32(dg(m,0xC038u),load_le32(dg(m,0xC038u))+1u);
    *dg(m,rec+0x14u)=static_cast<u8>(*dg(m,rec+0x14u)&0xF9u);

    bool play=owner>=0&&owner<4;
    if(!play) {
        if((*dg(m,0x198E0u+static_cast<u32>(tile)*2u)&0x40u)!=0u)play=true;
        else {
            const u32 dx=app12_abs_bits(load_le32(dg(m,rec))-load_le32(dg(m,0x255D4u)));
            if(static_cast<s32>(dx)<10) {
                const u32 dy=app12_abs_bits(load_le32(dg(m,rec+4u))-load_le32(dg(m,0x255D0u)));
                if(static_cast<s32>(dy)<10)play=true;
            }
        }
    }
    if(play&&b.call_28110)
        b.call_28110(load_le32(dg(m,0x258D4u))+kMovingRecordSoundOffsets[type],0xFFu,0u,1u);

    angle=static_cast<s32>(static_cast<u32>(angle)+0x16u);
    if(angle>=0x168)angle-=0x168;
    if(angle<0)angle+=0x168;
    const s32 quotient=angle/0x2D;
    const u32 orientation=static_cast<u32>(8-quotient)&7u;
    *dg(m,rec+0x14u)=static_cast<u8>(*dg(m,rec+0x14u)&0xC7u);
    store_le16(dg(m,rec+0x14u),static_cast<u16>(load_le16(dg(m,rec+0x14u))|
                                                static_cast<u16>(orientation<<3)));

    if(type!=8u) {
        for(;;) {
            const u32 hit=b.call_2c9dc?b.call_2c9dc(load_le32(dg(m,rec)),load_le32(dg(m,rec+4u))):0u;
            if(hit==0u)break;
            if(b.call_22e90)b.call_22e90(rec,0u);
            if((*dg(m,rec+0x14u)&6u)!=0u)break;
        }
    }

    if((*dg(m,rec+0x14u)&6u)!=0u) {
        if(b.call_19ccc)(void)b.call_19ccc(0u);
        const u8 saved=*dg(m,0xBF38u);
        *dg(m,0xBF38u)=0u;
        if(b.call_1f1c8)(void)b.call_1f1c8();
        if(b.call_1ea04)b.call_1ea04();
        if(b.call_1ea78)b.call_1ea78();
        *dg(m,0xBF38u)=saved;
        if(b.call_19cf4)(void)b.call_19cf4();
    }

    if(owner>=0&&owner<4) {
        const u32 base=static_cast<u32>(owner)*0x248u;
        store_le32(dg(m,0x21A2Au+base),0u);
        store_le32(dg(m,0x21A3Au+base),0u);
        store_le16(dg(m,0x21A12u+base),static_cast<u16>(load_le16(dg(m,0x21A12u+base))&0xFDDFu));
        *dg(m,0xCF5Au+static_cast<u32>(owner))=1u;
    }
}
static_assert(0x08C88u-0x08A80u==520u);

// 08C88..08CD7: compact the 16h-byte moving-record array. An out-of-range
// signed index reports 66A4h but deliberately continues into the copy anyway.
void function_08c88(Machine& m,Application12Boundary& b,s32 index) noexcept {
    if(index>=static_cast<s32>(load_le32(dg(m,0xC038u)))&&b.call_2f580)
        b.call_2f580(load_le32(dg(m,0x66A4u)));
    const u32 i=static_cast<u32>(index);
    const u32 count=(load_le32(dg(m,0xC038u))-1u-i)*0x16u;
    if(b.call_30cd4)b.call_30cd4(0x248ECu+i*0x16u,0x248ECu+(i+1u)*0x16u,count);
    store_le32(dg(m,0xC038u),load_le32(dg(m,0xC038u))-1u);
}
static_assert(0x08CD8u-0x08C88u==80u);

} // namespace code2
