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
constexpr u32 kVerifiedTranslatedBytes = 15532u;
constexpr u32 kVerifiedNonCodeBytes = 94u;

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
static_assert(kVerifiedTranslatedBytes == 4017u +
              (0x006B9u-0x002A8u) +
              (0x29A84u-0x29930u) + (0x29A9Fu-0x29A84u) +
              (0x29AF0u-0x29AA4u) + (0x29B19u-0x29AF0u) +
              (0x29C9Eu-0x29B40u) + (0x29CE9u-0x29CA0u) +
              (0x29EAAu-0x29CECu) +
              (0x2A20Cu-0x2A1E8u) + (0x2A256u-0x2A20Cu) +
              (0x2A305u-0x2A258u) + (0x2A380u-0x2A308u) +
              (0x2A478u-0x2A380u) + (0x2A584u-0x2A478u) +
              (0x2A6B0u-0x2A584u) + (0x2A89Fu-0x2A6B0u) +
              (0x2AA9Bu-0x2A8A0u) +
              (0x006FDu-0x006C0u) + (0x00792u-0x00700u) +
              (0x007E2u-0x00794u) + (0x00839u-0x007E4u) +
              (0x00931u-0x0083Cu) + (0x009B0u-0x00934u) +
              (0x00B6Cu-0x009B0u) + (0x00BFAu-0x00B6Cu) +
              (0x00CB0u-0x00BFCu) + (0x010C3u-0x00CB0u) +
              (0x011A5u-0x010C4u) +
              (0x01227u-0x011A8u) + (0x01399u-0x01228u) +
              (0x01557u-0x0139Cu) + (0x01831u-0x01558u) +
              (0x01C08u-0x01834u) + (0x01C28u-0x01C08u) +
              (0x01C4Du-0x01C28u) + (0x01CA8u-0x01C50u) +
              (0x01CE2u-0x01CA8u) + (0x01F6Bu-0x01CE4u) +
              (0x02079u-0x01F6Cu) + (0x021D8u-0x0207Cu));

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

} // namespace code2
