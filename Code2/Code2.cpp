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
constexpr u32 kVerifiedTranslatedBytes = 103537u;
constexpr u32 kVerifiedNonCodeBytes = 2143u;

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
static_assert(kVerifiedTranslatedBytes == 99711u +
              (0x17838u-0x171FCu) + (0x179A2u-0x17838u) +
              (0x181B4u-0x17A68u));

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

// -----------------------------------------------------------------------------
// Batch 13: 08CF4..094DC moving-record collision/effect resolution, party-hit
// propagation, proximity-gated sound playback, and pointer-selected angle input.
// 08CD8..08CF3 and 0934C..09367 are directly decoded CS dispatch tables.
// -----------------------------------------------------------------------------

struct Application13Boundary {
    u32 (*call_1c328)(u32,u32,u32,u32)=nullptr;
    void (*call_e284)(u32,u32,u32,u32,u32)=nullptr;
    u32 (*call_23f50)(u32)=nullptr;
    s32 (*call_14a60)(u32)=nullptr;
    u32 (*call_f2d4)(u32,u32)=nullptr;
    u32 (*call_cf2c)(u32,u32)=nullptr;
    void (*call_cd44)(u32,u32)=nullptr;
    void (*call_28110)(u32,u32,u32,u32)=nullptr;
    void (*call_1c21c)(u32)=nullptr;
    void (*call_1c258)()=nullptr;
    void (*call_1a9f8)()=nullptr;
    u32 (*call_1a1f4)()=nullptr;
    void (*call_1c2cc)(u32)=nullptr;
    void (*call_22d70)()=nullptr;
};

static u32 app13_owner_multiplier(Machine& m,u32 moving,bool owner7_is_five) noexcept {
    const u32 owner=(load_le16(dg(m,moving+0x14u))>>11)&7u;
    if(owner==4u)return 3u;
    if(owner7_is_five&&owner==7u)return 5u;
    const u32 base=owner*0x248u;
    u32 result=1u;
    if(static_cast<s8>(*dg(m,0x218F5u+base))==4)
        result=static_cast<u32>(static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x21900u+base)))));
    if(static_cast<s8>(*dg(m,0x218F6u+base))==4)
        result=static_cast<u32>(static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x21902u+base)))));
    if(static_cast<s8>(*dg(m,0x218F7u+base))==4)
        result=static_cast<u32>(static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x21904u+base)))));
    return result;
}

static bool app13_mode_filter(Machine& m,u32 moving_type,u32 candidate_def) noexcept {
    const u8 f=*dg(m,candidate_def+0x32u);
    switch(moving_type) {
        case 0u:return (f&0x21u)!=0u;
        case 1u:return (f&0x03u)!=0u;
        case 2u:return (f&0x09u)!=0u;
        case 3u:return (f&0x09u)!=0u;
        case 4u:return (f&0x21u)!=0u;
        case 5u:return (f&0x01u)!=0u;
        case 6u:return (f&0x01u)!=0u;
        default:return false;
    }
}

// 08CF4..0906C: scan active 16h-byte world records whose type is below 22h or
// exactly 50h and whose radius box contains the moving record. A geometric hit
// clears the result even when the mode-specific type filter suppresses E284.
[[nodiscard]] u32 function_08cf4(Machine& m,Application13Boundary& b,u32 moving,u32 mode) noexcept {
    const u32 moving_type=load_le16(dg(m,moving+0x0Eu));
    const u32 cfg=0x66C8u+moving_type*4u;
    const u32 multiplier=mode!=0u?app13_owner_multiplier(m,moving,false):1u;
    u32 result=1u;
    const s32 count=static_cast<s16>(load_le16(dg(m,0x258A2u)));
    for(s32 i=0;i<count;++i) {
        const u32 rec=0x14D80u+static_cast<u32>(i)*0x16u;
        if((*dg(m,rec+0x0Bu)&2u)!=0u)continue;
        const u32 candidate_type=load_le16(dg(m,rec+0x0Eu));
        if(candidate_type>=0x22u&&candidate_type!=0x50u)continue;
        const u32 candidate_def=load_le32(dg(m,0x22230u))+candidate_type*0x45u;
        const u32 radius=load_le16(dg(m,candidate_def+0x16u));
        const u32 cy=load_le32(dg(m,rec+4u)),cx=load_le32(dg(m,rec));
        const u32 my=load_le32(dg(m,moving+4u)),mx=load_le32(dg(m,moving));
        if(my<cy-radius||cy+radius<my||mx<cx-radius||cx+radius<mx)continue;
        result=0u;

        if(mode!=0u) {
            if(!app13_mode_filter(m,moving_type,candidate_def))continue;
            u32 amount=b.call_1c328?b.call_1c328(*dg(m,candidate_def),*dg(m,candidate_def+1u),*dg(m,cfg+2u),0u):0u;
            if(*dg(m,cfg+3u)!=0u)amount*=multiplier;
            if(b.call_e284)b.call_e284(rec,candidate_def,static_cast<u32>(i),amount,0u);
            continue;
        }

        const u32 moving_def=load_le32(dg(m,0x255D8u))+moving_type*0x41u;
        if(moving_type==0x5Au) {
            const u32 amount=b.call_1c328?b.call_1c328(*dg(m,moving_def+0x38u),*dg(m,moving_def+0x39u),*dg(m,moving_def+0x3Au),0u):0u;
            if(b.call_e284)b.call_e284(rec,candidate_def,static_cast<u32>(i),amount,1u);
            if((*dg(m,candidate_def+0x32u)&3u)==0u&&b.call_e284)
                b.call_e284(rec,candidate_def,static_cast<u32>(i),0x14u,0u);
            *dg(m,moving+0x15u)=static_cast<u8>((*dg(m,moving+0x15u)&0x3Fu)|0x40u);
            continue;
        }

        if(!b.call_23f50||b.call_23f50(moving_type)==0u||*dg(m,moving_def+0x38u)==0u)continue;
        u32 amount=b.call_1c328?b.call_1c328(*dg(m,moving_def+0x38u),*dg(m,moving_def+0x39u),*dg(m,moving_def+0x3Au),*dg(m,moving_def+0x37u)):0u;
        if(moving_type==0x58u&&(*dg(m,candidate_def+0x3Au)&0x20u)!=0u) {
            *dg(m,moving+0x15u)=static_cast<u8>((*dg(m,moving+0x15u)&0x3Fu)|0x40u);
            amount=static_cast<u32>(static_cast<s32>(static_cast<s16>(load_le16(dg(m,rec+0x12u)))));
        }
        if(b.call_e284)b.call_e284(rec,candidate_def,static_cast<u32>(i),amount,1u);
    }
    return result;
}
static_assert(0x0906Du-0x08CF4u==889u);

static u32 app13_half_signed(u32 v) noexcept {
    return static_cast<u32>(static_cast<s32>(v)/2);
}

// 09070..09349: apply one moving record against the four party slots. In mode
// zero, types 57h/59h clear the encoded bit and return before the proximity box.
void function_09070(Machine& m,Application13Boundary& b,u32 moving,u32 mode) noexcept {
    const u32 moving_type=load_le16(dg(m,moving+0x0Eu));
    u32 cfg=0u,multiplier=1u;
    if(mode==0u&&(moving_type==0x59u||moving_type==0x57u)) {
        *dg(m,moving+0x15u)=static_cast<u8>(*dg(m,moving+0x15u)&0xFDu);
        return;
    }
    if(mode!=0u) {
        cfg=0x66C8u+moving_type*4u;
        multiplier=app13_owner_multiplier(m,moving,true);
    }

    const u32 p0=load_le32(dg(m,0x255D0u)),p4=load_le32(dg(m,0x255D4u));
    const u32 v4=load_le32(dg(m,moving+4u)),v0=load_le32(dg(m,moving));
    if(v4<p0-0x8FA0u||p0+0x8FA0u<v4||v0<p4-0x8FA0u||p4+0x8FA0u<v0)return;

    for(u32 slot=0u,base=0u;slot<4u;++slot,base+=0x248u) {
        if((load_le16(dg(m,0x218F0u+base))&0xF240u)!=0u)continue;
        if(b.call_14a60&&b.call_14a60(0x510u)==static_cast<s32>(slot))continue;

        if(mode!=0u) {
            u32 amount=b.call_1c328?b.call_1c328(*dg(m,cfg),*dg(m,cfg+1u),*dg(m,cfg+2u),0u):0u;
            if(*dg(m,cfg+3u)!=0u)amount*=multiplier;
            u32 flavor=0u;
            bool stronger=false;
            if(moving_type==0u) {
                stronger=(b.call_f2d4&&b.call_f2d4(slot,0x44u)!=0u)||((*dg(m,0x21A14u+base)&8u)!=0u);
            } else if(moving_type==1u) {
                stronger=static_cast<s16>(load_le16(dg(m,0x2193Cu+base)))==0x67||
                         static_cast<s16>(load_le16(dg(m,0x2193Eu+base)))==0x67;
            }
            if(stronger) {
                const u32 middle=static_cast<u32>(*dg(m,cfg+1u))-2u;
                amount=b.call_1c328?b.call_1c328(*dg(m,cfg),middle,*dg(m,cfg+2u),0u):0u;
                amount*=multiplier;
                flavor=2u;
            }
            if(b.call_cf2c&&b.call_cf2c(slot,flavor)!=0u)amount=app13_half_signed(amount);
            if(amount!=0u&&b.call_cd44)b.call_cd44(slot,amount);
        } else {
            const u32 def=load_le32(dg(m,0x255D8u))+moving_type*0x41u;
            u32 amount=0u;
            if(*dg(m,def+0x38u)!=0u)
                amount=b.call_1c328?b.call_1c328(*dg(m,def+0x38u),*dg(m,def+0x39u),*dg(m,def+0x3Au),0u):0u;
            else
                amount=b.call_1c328?b.call_1c328(1u,2u,0u,0u):0u;
            if(b.call_cd44)b.call_cd44(slot,amount);
            *dg(m,moving+0x15u)=static_cast<u8>(*dg(m,moving+0x15u)&0xFDu);
        }
    }
}
static_assert(0x0934Au-0x09070u==730u);

// 09368..09417: choose one sound offset from the seven-entry 0934C CS table,
// then gate distant playback by orientation bits and tile visibility.
void function_09368(Machine& m,Application13Boundary& b,u32 moving) noexcept {
    const s32 dy=static_cast<s32>(static_cast<s16>(load_le16(dg(m,moving+2u))))-
                 static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x255D6u))));
    const s32 dx=static_cast<s32>(static_cast<s16>(load_le16(dg(m,moving+6u))))-
                 static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x255D2u))));
    const s32 ady=dy<0?-dy:dy,adx=dx<0?-dx:dx;
    s32 sound=-1;
    switch(load_le16(dg(m,moving+0x0Eu))) {
        case 0u:sound=0x1C;break;
        case 1u:sound=0x37;break;
        case 4u:sound=0x1E;break;
        case 6u:sound=0x39;break;
        default:break;
    }
    const u16 phase=static_cast<u16>(load_le16(dg(m,moving+0x14u))&0x3800u);
    if(phase>=0x2000u) {
        const u32 tile=load_le16(dg(m,moving+0x0Cu));
        if((*dg(m,0x198E0u+tile*2u)&0x40u)==0u&&(ady>=10||adx>=10))return;
    }
    if(sound!=-1&&b.call_28110)
        b.call_28110(load_le32(dg(m,0x258D4u))+static_cast<u32>(sound),0xFFu,1u,1u);
}
static_assert(0x09418u-0x09368u==176u);

// 09418..094DC: modal pointer selection. With mode!=0, Y rotates C658 in
// signed thirds around A0h; otherwise the returned value is Y-20h.
[[nodiscard]] s32 function_09418(Machine& m,Application13Boundary& b,u32 mode) noexcept {
    if(*dg(m,0xC02Fu)!=0u)
        return mode!=0u?static_cast<s32>(load_le32(dg(m,0xC658u))):0x80;
    if(b.call_1c21c)b.call_1c21c(load_le32(dg(m,0x66A8u)));
    if(b.call_1c258)b.call_1c258();
    if(b.call_1a9f8)b.call_1a9f8();
    for(;;) {
        if(b.call_1a1f4)while(b.call_1a1f4()!=0u){}
        if(b.call_1a1f4)while(b.call_1a1f4()==0u){}
        const s32 x=static_cast<s16>(load_le16(dg(m,0x9E31u)));
        const s32 y=static_cast<s16>(load_le16(dg(m,0x9E33u)));
        if(x<4||x>=0x104||y<0x20||y>=0x120)continue;
        s32 result=0;
        if(mode!=0u) {
            const s32 q=(y-0xA0)/3;
            result=static_cast<s32>(load_le32(dg(m,0xC658u)))-q;
            if(result<0)result+=0x168;
            if(result>=0x168)result-=0x168;
        } else result=y-0x20;
        if(b.call_1c2cc)b.call_1c2cc(static_cast<u32>(result));
        if(b.call_22d70)b.call_22d70();
        return result;
    }
}
static_assert(0x094DDu-0x09418u==197u);

// -----------------------------------------------------------------------------
// Batch 14: 094E0..09B49 opposite-buffer hit testing, interactive selection,
// effect-slot insertion, and party status/effect application.
// -----------------------------------------------------------------------------

struct Application14Boundary {
    u32 (*call_2acdc)(u32)=nullptr;
    u32 (*call_21bf0)(u32,u32)=nullptr;
    void (*call_1c21c)(u32)=nullptr;
    void (*call_1c258)()=nullptr;
    void (*call_1a9f8)()=nullptr;
    u32 (*call_1a1f4)()=nullptr;
    u32 (*call_30b40)()=nullptr;
    void (*call_1c2cc)()=nullptr;
    void (*call_22d70)()=nullptr;
    void (*call_2195c)(u32,u32,u32,u32,u32,u32,u32)=nullptr;
    u32 (*call_f218)(u32,u32)=nullptr;
    void (*call_f238)(u32,u32,u32)=nullptr;
    void (*call_1ea90)(u32,u32,u32)=nullptr;
    void (*call_28110)(u32,u32,u32,u32)=nullptr;
    void (*call_28168)()=nullptr;
    void (*call_1a8c8)(u32,u32,u32,u32,u32)=nullptr;
    u32 (*call_19cc4)()=nullptr;
};

[[nodiscard]] static u32 app14_toggle_low8(u32 v) noexcept {
    return (v&0xFFFFFF00u)|((v^1u)&0xFFu);
}
[[nodiscard]] static u32 app14_toggle_low16(u32 v) noexcept {
    return (v&0xFFFF0000u)|((v^1u)&0xFFFFu);
}

// 094E0..095BC: search the opposite display buffer from the highest ordering
// value down. The inner search deliberately falls through to record[count] if
// no ordering-word match is found among records 0..count-1.
[[nodiscard]] u32 function_094e0(Machine& m,Application14Boundary& b,s32 x,s32 y) noexcept {
    u32 side=app14_toggle_low8(load_le32(dg(m,0xC050u)));
    s32 order=static_cast<s32>(load_le16(dg(m,0xF038u+side*2u)))-1;
    while(order>=0) {
        u32 scan_side=app14_toggle_low8(load_le32(dg(m,0xC050u)));
        u32 index=0u;
        u32 off=0u;
        const u32 count=load_le16(dg(m,0xF038u+scan_side*2u));
        while(static_cast<s32>(index)<static_cast<s32>(count)) {
            const u32 list=scan_side*0x2260u;
            if(load_le16(dg(m,0x27152u+list+off))==static_cast<u16>(order))break;
            off+=0x16u;
            ++index;
        }
        const u32 rec=0x27148u+scan_side*0x2260u+index*0x16u;
        if(load_le16(dg(m,rec+8u))==0u) {
            const u32 id=load_le16(dg(m,rec+6u));
            const u32 mapped=load_le16(dg(m,0x14D8Eu+id*0x16u));
            if((!b.call_2acdc||b.call_2acdc(mapped)!=0u) &&
               (!b.call_21bf0||b.call_21bf0(rec,0u)!=0u) &&
               x>=static_cast<s32>(load_le16(dg(m,rec+0x0Eu))) &&
               x< static_cast<s32>(load_le16(dg(m,rec+0x10u))) &&
               y>=static_cast<s32>(load_le16(dg(m,rec+0x12u))) &&
               y< static_cast<s32>(load_le16(dg(m,rec+0x14u))))
                return id;
        }
        --order;
    }
    return 0xFFFFFFFFu;
}
static_assert(0x095BDu-0x094E0u==221u);

// 095C0..096DF: require one free opposite-buffer record slot, then poll for a
// selectable rectangle. The first/second 1A1F4 phases and ESC handling are kept
// distinct because the binary loops to different polling addresses.
[[nodiscard]] u32 function_095c0(Machine& m,Application14Boundary& b) noexcept {
    u32 side=app14_toggle_low16(load_le32(dg(m,0xC050u)));
    u32 index=0u;
    u32 off=0u;
    const u32 count=load_le16(dg(m,0xF038u+side*2u));
    while(static_cast<s32>(index)<=static_cast<s32>(count)) {
        if(load_le16(dg(m,0x27150u+side*0x2260u+off))==0u)break;
        off+=0x16u;
        ++index;
    }
    if(index==count) {
        if(b.call_1c21c)b.call_1c21c(load_le32(dg(m,0x66ACu)));
        if(b.call_1c258)b.call_1c258();
        return 0xFFFFFFFFu;
    }

    if(b.call_1c21c)b.call_1c21c(load_le32(dg(m,0x66B0u)));
    s32 selected=-1;
    bool cancel=false;
    if(b.call_1c258)b.call_1c258();
    if(b.call_1a9f8)b.call_1a9f8();
    while(!cancel&&selected==-1) {
        const u32 first=b.call_1a1f4?b.call_1a1f4():0u;
        if(first!=0u) {
            if(b.call_30b40&&b.call_30b40()==0x1Bu)cancel=true;
            continue;
        }
        for(;;) {
            const u32 second=b.call_1a1f4?b.call_1a1f4():0u;
            if(second!=0u)break;
            if(b.call_30b40&&b.call_30b40()==0x1Bu) { cancel=true; break; }
        }
        if(cancel)break;
        if((load_le16(dg(m,0x9E35u))&2u)!=0u)break;
        const s32 px=static_cast<s16>(load_le16(dg(m,0x9E31u)));
        const s32 py=static_cast<s16>(load_le16(dg(m,0x9E33u)));
        if(px>=4&&px<0x104&&py>=0x20&&py<0x120)
            selected=static_cast<s32>(function_094e0(m,b,px,py-0x20));
    }
    if(b.call_1c2cc)b.call_1c2cc();
    if(b.call_22d70)b.call_22d70();
    return static_cast<u32>(selected);
}
static_assert(0x096E0u-0x095C0u==288u);

// 096E0..0978C: insert one of two words into the first -1 word of the 12-slot
// per-party list. If all 12 are occupied, forward the encoded coordinate pair to
// 2195C and return the mode-derived selector.
[[nodiscard]] u32 function_096e0(Machine& m,Application14Boundary& b,
                                  u32 a0,u32 a1,u32 a2,u32 a3,u32 mode,u32 slot) noexcept {
    u32 i=0u;
    u32 p=0x21920u+slot*0x248u;
    while(i<12u&&static_cast<s16>(load_le16(dg(m,p)))!=-1) { ++i; p+=2u; }
    if(i<12u) {
        const u32 which=(mode!=0u)?0u:1u;
        store_le16(dg(m,p),static_cast<u16>((mode!=0u)?a0:a1));
        return which;
    }
    if(mode!=0u) {
        if(b.call_2195c)b.call_2195c(0x80u,a0,a2,load_le32(dg(m,0x255D0u)),load_le32(dg(m,0x255D4u)),1u,0u);
        return 0u;
    }
    if(b.call_2195c)b.call_2195c(0x80u,a1,a3,load_le32(dg(m,0x255D0u)),load_le32(dg(m,0x255D4u)),1u,0u);
    return 1u;
}
static_assert(0x0978Du-0x096E0u==173u);

static bool app14_id_is_class_d(Machine& m,s32 id) noexcept {
    if(id==-1)return false;
    const u32 u=static_cast<u32>(id);
    const u32 off=(u<<6)+u;
    return *dg(m,0x255D8u+off+0x2Fu)==0x0Du;
}
static void app14_redraw_slot(Machine& m,Application14Boundary& b,s32 slot,s32 side) noexcept {
    const s32 rem=slot%2;
    const s32 row=slot/2;
    const u32 base=static_cast<u32>(row*0xD3 + rem*0x35);
    const u32 x0=base+static_cast<u32>(side*0x21+4);
    if(b.call_1a8c8)b.call_1a8c8(load_le32(dg(m,0xC050u)),0x114u,x0,0x20u,0x10u);
    const u32 toggled=app14_toggle_low16(static_cast<u32>(side));
    const u32 x1=base+static_cast<u32>(static_cast<s32>(toggled)*0x21+4);
    if(b.call_1a8c8)b.call_1a8c8(load_le32(dg(m,0xC050u)),0x114u,x1,0x20u,0x10u);
}

// 09790..09B49: apply one party status/equipment effect. The two old words,
// F218-derived values, A5/80 special cases, address-distinct F238 refreshes,
// slot redraw, sound, and optional timed-event installation retain binary order.
[[nodiscard]] u32 function_09790(Machine& m,Application14Boundary& b,
                                  u32 effect,u32 slot,s32 sound_off,u32 mask,
                                  u32 deadline_index,u32 duration_a,u32 duration_b) noexcept {
    const u32 base=slot*0x248u;
    const s32 old0=static_cast<s16>(load_le16(dg(m,0x2193Cu+base)));
    const s32 old1=static_cast<s16>(load_le16(dg(m,0x2193Eu+base)));
    const u32 derived0=(b.call_f218?b.call_f218(slot,0x0Eu):0u)&0xFFu;
    const u32 derived1=(b.call_f218?b.call_f218(slot,0x0Fu):0u)&0xFFu;

    if(mask!=0xFFFFFFFFu&&(load_le32(dg(m,0x21AA2u+base))&mask)!=0u)return 0u;

    u32 result=1u;
    bool first=!(old0>=0x7E&&old0<=0x86);
    bool second=!(old1>=0x7E&&old1<=0x86);
    s32 selected=0;

    if(effect==0xA5u) {
        if(first&&app14_id_is_class_d(m,old0)&&old0!=static_cast<s32>(effect))first=false;
        if(second&&app14_id_is_class_d(m,old1)&&old1!=static_cast<s32>(effect))second=false;
    } else if(effect==0x80u) {
        if(!first||!second)return 0u;
        if(first&&old0!=-1)
            (void)function_096e0(m,b,static_cast<u32>(old0),static_cast<u32>(old1),derived0,derived1,1u,slot);
        if(second&&old1!=-1)
            (void)function_096e0(m,b,static_cast<u32>(old0),static_cast<u32>(old1),derived0,derived1,0u,slot);
        selected=1;
    } else {
        if(old0==-1)selected=0;
        else if(old1==-1)selected=1;
        else selected=static_cast<s32>(function_096e0(m,b,static_cast<u32>(old0),static_cast<u32>(old1),derived0,derived1,first?1u:0u,slot));
    }

    if(selected==1) {
        for(u32 i=0;i<4u;++i)*dg(m,0x6FA0u+slot*4u+i)=0xFFu;
    }

    if(effect==0x80u) {
        store_le16(dg(m,0x2193Cu+base),static_cast<u16>(effect));
        store_le16(dg(m,0x2193Eu+base),static_cast<u16>(effect));
        if(b.call_f238){b.call_f238(slot,0x0Eu,0u);b.call_f238(slot,0x0Fu,0u);}
    } else if(effect==0xA5u) {
        if(!first&&!second)result=0u;
        if(first) {
            store_le16(dg(m,0x2193Cu+base),static_cast<u16>(effect));
            if(b.call_f238)b.call_f238(slot,0x0Eu,0u);
        }
        if(second) {
            store_le16(dg(m,0x2193Eu+base),static_cast<u16>(effect));
            if(b.call_f238)b.call_f238(slot,0x0Fu,0u);
        }
    } else {
        store_le16(dg(m,0x2193Cu+base+static_cast<u32>(selected)*2u),static_cast<u16>(effect));
        if(b.call_f238)b.call_f238(slot,static_cast<u32>(selected+0x0E),0u);
        result=1u;
    }

    if(b.call_1ea90) {
        b.call_1ea90(slot,static_cast<u32>(selected),0xFFFFFFFFu);
        b.call_1ea90(slot,app14_toggle_low8(static_cast<u32>(selected)),0xFFFFFFFFu);
    }
    if(sound_off>=0) {
        if(b.call_28110)b.call_28110(load_le32(dg(m,0x258D4u))+static_cast<u32>(sound_off),0xFFu,0u,1u);
        if(b.call_28168)b.call_28168();
    }
    app14_redraw_slot(m,b,static_cast<s32>(slot),selected);

    if(mask!=0xFFFFFFFFu) {
        store_le32(dg(m,0x21AA2u+base),load_le32(dg(m,0x21AA2u+base))|mask);
        const u32 delta=duration_a*duration_b;
        const u32 now=b.call_19cc4?b.call_19cc4():0u;
        store_le32(dg(m,0x21AA6u+base+deadline_index*4u),now+delta);
    }
    return result;
}
static_assert(0x09B4Au-0x09790u==954u);

// -----------------------------------------------------------------------------
// Batch 15: 09B4C..0A790 party-effect animation/resolution and modal apply flow.
// -----------------------------------------------------------------------------

struct Application15Boundary {
    void (*call_2e608)()=nullptr;
    void (*call_1f494)(u32)=nullptr;
    void (*call_2975c)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_1a8c8)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_28168)()=nullptr;
    void (*call_2e614)(u32,u32)=nullptr;
    u32 (*call_1c328)(u32,u32,u32,u32)=nullptr;
    void (*call_3210f)(u32,u32,u32)=nullptr;
    void (*call_1c21c)(u32)=nullptr;
    u32 (*call_22e40)(u32,u32)=nullptr;
    u32 (*call_1a1f4)()=nullptr;
    void (*call_1c258)()=nullptr;
    void (*call_28110)(u32,u32,u32,u32)=nullptr;
    u32 (*call_19cc4)()=nullptr;
    void (*call_1c2cc)()=nullptr;
    void (*call_22d70)()=nullptr;
};

[[nodiscard]] static s32 app15_panel_base(s32 slot) noexcept {
    return (slot%2)*0x35 + (slot/2)*0xD3;
}

static void app15_draw_effect_frame(Machine& m,Application15Boundary& b,
                                    u32 desc,u8 frame,s32 panel) noexcept {
    const u32 image=load_le32(dg(m,0x258DCu))+load_le32(dg(m,desc));
    if(b.call_2975c)
        b.call_2975c(frame,load_le32(dg(m,desc+4u)),
                     static_cast<u32>(panel)+load_le32(dg(m,desc+8u)),image,0u);
}

// 09B4C..09D8E: animate one descriptor from 66ECh. mode==0 draws the chosen
// party panel only; nonzero mode redraws every active party for each frame.
void function_09b4c(Machine& m,Application15Boundary& b,u32 slot,u32 visual,u32 mode) noexcept {
    const u32 desc=0x66ECu+visual*0x21u;
    const u32 frame_count=*dg(m,desc+0x0Cu);
    const u32 frames=desc+0x0Du;
    const s32 selected_panel=app15_panel_base(static_cast<s32>(slot));

    if(mode==0u) {
        for(u32 i=0u;i<frame_count;++i) {
            if(b.call_2e608)b.call_2e608();
            *dg(m,0xCF5Au+slot)=1u;
            if(b.call_1f494)b.call_1f494(0u);
            const u8 frame=*dg(m,frames+i);
            app15_draw_effect_frame(m,b,desc,frame,selected_panel);
            if(visual==5u)app15_draw_effect_frame(m,b,desc,frame,selected_panel+0x21);
            if(b.call_1a8c8)
                b.call_1a8c8(load_le32(dg(m,0xC050u)),0x113u,
                              static_cast<u32>(selected_panel),0x7Du,0x35u);
            if(b.call_28168)b.call_28168();
            if(b.call_2e614)b.call_2e614(3u,0u);
        }
    } else {
        for(u32 i=0u;i<frame_count;++i) {
            if(b.call_2e608)b.call_2e608();
            const u8 frame=*dg(m,frames+i);
            for(u32 party=0u,off=0u;party<4u;++party,off+=0x248u) {
                if(static_cast<u32>(load_le16(dg(m,0x218F0u+off)))>=0x4000u)continue;
                const s32 panel=app15_panel_base(static_cast<s32>(party));
                *dg(m,0xCF5Au+party)=1u;
                if(b.call_1f494)b.call_1f494(0u);
                app15_draw_effect_frame(m,b,desc,frame,panel);
                if(visual==5u)app15_draw_effect_frame(m,b,desc,frame,panel+0x21);
            }
            if(b.call_1a8c8)b.call_1a8c8(load_le32(dg(m,0xC050u)),0x113u,0u,0x7Du,0x140u);
            if(b.call_28168)b.call_28168();
            if(b.call_2e614)b.call_2e614(3u,0u);
        }
    }

    *dg(m,0xCF5Au+slot)=1u;
    if(b.call_1f494)b.call_1f494(0u);
    if(b.call_1a8c8)
        b.call_1a8c8(load_le32(dg(m,0xC050u)),0x113u,
                      static_cast<u32>(selected_panel),0x7Du,0x35u);
}
static_assert(0x09D8Fu-0x09B4Cu==579u);

static void app15_message_name(Machine& m,Application15Boundary& b,u32 text,u32 slot) noexcept {
    (void)m;
    if(b.call_3210f)b.call_3210f(0x759Cu,text,0x218E0u+slot*0x248u);
    if(b.call_1c21c)b.call_1c21c(0x759Cu);
}

static void app15_add_current_word(Machine& m,u32 slot,u32 amount) noexcept {
    const u32 base=slot*0x248u;
    const u16 old=load_le16(dg(m,0x218FCu+base));
    const u16 sum=static_cast<u16>(old+static_cast<u16>(amount));
    store_le16(dg(m,0x218FCu+base),sum);
    const u16 maximum=load_le16(dg(m,0x218FEu+base));
    if(static_cast<s16>(sum)>static_cast<s16>(maximum))store_le16(dg(m,0x218FCu+base),maximum);
}

// 09D90..0A253: resolve one encoded effect type for party slot and auxiliary
// slot. Return value defaults to one; the message-only failure branches return
// zero exactly where the binary clears EBP.
[[nodiscard]] u32 function_09d90(Machine& m,Application15Boundary& b,
                                  s32 type,u32 slot,u32 auxiliary) noexcept {
    const u32 base=slot*0x248u;
    const u32 aux=auxiliary*0x248u;

    switch(type) {
        case -1: {
            *dg(m,0x218F1u+base)=static_cast<u8>(*dg(m,0x218F1u+base)&0xF3u);
            const u32 amount=b.call_1c328?b.call_1c328(1u,4u,0u,8u):0u;
            app15_add_current_word(m,slot,amount);
            return 1u;
        }
        case 0x51: {
            u32 amount=b.call_1c328?b.call_1c328(1u,8u,0u,0u):0u;
            if((*dg(m,0x21A13u+aux)&0x80u)!=0u) {
                amount=8u;
                *dg(m,0x21A13u+aux)=static_cast<u8>(*dg(m,0x21A13u+aux)&0x7Fu);
            }
            app15_add_current_word(m,slot,amount);
            return 1u;
        }
        case 0x57: {
            const u32 amount=(b.call_22e40?b.call_22e40(1u,auxiliary):0u)*2u;
            app15_add_current_word(m,slot,amount);
            return 1u;
        }
        case 0x64:
            if((load_le16(dg(m,0x218F0u+base))&0x0400u)!=0u)
                *dg(m,0x218F1u+base)=static_cast<u8>(*dg(m,0x218F1u+base)&0xFBu);
            return 1u;
        case 0x68:
            if((load_le16(dg(m,0x218F0u+base))&0x1000u)!=0u)
                *dg(m,0x218F1u+base)=static_cast<u8>(*dg(m,0x218F1u+base)&0xEFu);
            return 1u;
        case 0x6E: {
            u32 amount=b.call_1c328?b.call_1c328(2u,8u,0u,1u):0u;
            if((*dg(m,0x21A13u+aux)&0x80u)!=0u) {
                amount=0x10u;
                *dg(m,0x21A13u+aux)=static_cast<u8>(*dg(m,0x21A13u+aux)&0x7Fu);
            }
            app15_add_current_word(m,slot,amount);
            return 1u;
        }
        case 0x71:
            if((load_le16(dg(m,0x218F0u+base))&0x0800u)!=0u)
                *dg(m,0x218F1u+base)=static_cast<u8>(*dg(m,0x218F1u+base)&0xF7u);
            return 1u;
        case 0x78: {
            u32 amount=b.call_1c328?b.call_1c328(3u,8u,0u,3u):0u;
            if((*dg(m,0x21A13u+aux)&0x80u)!=0u) {
                amount=0x1Bu;
                *dg(m,0x21A13u+aux)=static_cast<u8>(*dg(m,0x21A13u+aux)&0x7Fu);
            }
            app15_add_current_word(m,slot,amount);
            return 1u;
        }
        case 0x7B: {
            const u16 state=load_le16(dg(m,0x218F0u+base));
            if((state&0x4000u)==0u) {
                app15_message_name(m,b,load_le32(dg(m,0x66C4u)),slot);
                return 0u;
            }
            if((state&0x8000u)!=0u)return 1u;
            if(*dg(m,0x218F4u+base)==5u) {
                if(b.call_1c21c)b.call_1c21c(load_le32(dg(m,0x66BCu)));
                return 0u;
            }
            if(load_le16(dg(m,0x21900u+base))==0u ||
               load_le16(dg(m,0x21902u+base))==0u ||
               load_le16(dg(m,0x21904u+base))==0u) {
                app15_message_name(m,b,load_le32(dg(m,0x66B8u)),slot);
                return 0u;
            }
            if(*dg(m,0x21917u+base)!=0u) {
                const s32 delta=static_cast<s32>(*dg(m,0x21917u+base))-
                                static_cast<s32>(*dg(m,0x21916u+base));
                if(delta<5) {
                    store_le16(dg(m,0x218FCu+base),1u);
                    *dg(m,0x218F1u+base)=static_cast<u8>(*dg(m,0x218F1u+base)&0xBFu);
                    *dg(m,0xCF5Au+slot)=1u;
                    *dg(m,0x21917u+base)=static_cast<u8>(*dg(m,0x21917u+base)-1u);
                    app15_message_name(m,b,load_le32(dg(m,0x66B4u)),slot);
                    return 1u;
                }
            }
            app15_message_name(m,b,load_le32(dg(m,0x66B8u)),slot);
            return 0u;
        }
        case 0x82: {
            const u16 maximum=load_le16(dg(m,0x218FEu+base));
            store_le16(dg(m,0x218F0u+base),static_cast<u16>(load_le16(dg(m,0x218F0u+base))&0x01C0u));
            *dg(m,0xCF5Au+slot)=1u;
            store_le16(dg(m,0x218FCu+base),maximum);
            return 1u;
        }
        default:
            return 1u;
    }
}
static_assert(0x0A254u-0x09D90u==1220u);

[[nodiscard]] static bool app15_target_eligible(Machine& m,u32 effect,u32 slot) noexcept {
    const u16 state=load_le16(dg(m,0x218F0u+slot*0x248u));
    switch(effect) {
        case 0x7Bu:return (state&0x8000u)==0u;
        case 0x68u:return (state&0x1000u)!=0u;
        case 0x71u:
        case 0x5Du:return (state&0x0800u)!=0u;
        case 0x64u:return (state&0x0400u)!=0u;
        default:return static_cast<u32>(state)<0x4000u;
    }
}

// 0A254..0A5BB: modal four-party effect application. The eight arguments are
// retained positionally because several values serve unrelated encoded roles.
[[nodiscard]] u32 function_0a254(Machine& m,Application15Boundary& b,
                                  u32 a0,u32 effect,s32 a2,u32 a3,s32 visual,
                                  u32 auxiliary,u32 a6,u32 a7) noexcept {
    for(u32 i=0;i<4u;++i)*dg(m,0xCF5Au+i)=1u;
    const u8 saved_d681=*dg(m,0xD681u);
    *dg(m,0xD681u)=0u;
    const u8 saved_d682=*dg(m,0xD682u);
    *dg(m,0xD682u)=0u;
    if(b.call_1f494)b.call_1f494(1u);
    if(b.call_1c21c)b.call_1c21c(load_le32(dg(m,0x66C0u)));
    if(b.call_1c258)b.call_1c258();

    const u32 timer_delta=a3*a7;
    const u32 deadline_off=effect*4u;
    u32 result=1u;
    bool done=false;

    while(!done) {
        if(b.call_1a1f4)while(b.call_1a1f4()!=0u){}
        if(b.call_1a1f4)while(b.call_1a1f4()==0u){}
        if((load_le16(dg(m,0x9E35u))&2u)!=0u) {
            result=0u;
            break;
        }

        const s32 x=static_cast<s16>(load_le16(dg(m,0x9E31u)));
        const s32 y=static_cast<s16>(load_le16(dg(m,0x9E33u)));
        for(u32 slot=0u;slot<4u;++slot) {
            const u32 roff=slot*8u;
            const s32 left=static_cast<s16>(load_le16(dg(m,0xD684u+roff)));
            const s32 top=static_cast<s16>(load_le16(dg(m,0xD686u+roff)));
            const s32 right=static_cast<s16>(load_le16(dg(m,0xD688u+roff)));
            const s32 bottom=static_cast<s16>(load_le16(dg(m,0xD68Au+roff)));
            if(x<left||x>right||y<top||y>bottom)continue;
            if(!app15_target_eligible(m,effect,slot))break;

            if(visual>=0 && b.call_28110)
                b.call_28110(load_le32(dg(m,0x258D4u))+static_cast<u32>(visual),0xFFu,0u,1u);

            if(a2>=0)function_09b4c(m,b,slot,static_cast<u32>(visual),0u);

            if(a2>=0 && a6==0u) {
                const u32 base=slot*0x248u;
                store_le32(dg(m,0x21A12u+base),load_le32(dg(m,0x21A12u+base))|a0);
                if(a3!=0u) {
                    const u32 now=b.call_19cc4?b.call_19cc4():0u;
                    store_le32(dg(m,0x21A16u+base+deadline_off),now+timer_delta);
                }
                if(a0==0x40u) {
                    const u32 r=b.call_22e40?b.call_22e40(4u,auxiliary):0u;
                    const s32 sr=static_cast<s32>(r);
                    const u32 half=static_cast<u32>(sr/2);
                    const u32 add=b.call_1c328?b.call_1c328(1u,4u,0u,half):0u;
                    store_le32(dg(m,0x21AD2u+base),load_le32(dg(m,0x21AD2u+base))+add);
                }
                if(a0==0x800u) {
                    const u32 add=b.call_1c328?b.call_1c328(1u,8u,0u,0u):0u;
                    store_le32(dg(m,0x21ACAu+base),load_le32(dg(m,0x21ACAu+base))+add);
                }
            } else {
                result=function_09d90(m,b,static_cast<s32>(effect),slot,auxiliary);
            }

            if(effect==0u) {
                const u32 base=slot*0x248u;
                store_le32(dg(m,0x21A16u+base),0u);
                const u32 r=b.call_22e40?b.call_22e40(4u,slot):0u;
                store_le32(dg(m,0x219F7u+base),r+8u);
            }
            done=true;
            break;
        }
    }

    if(b.call_1c2cc)b.call_1c2cc();
    for(u32 i=0;i<4u;++i)*dg(m,0xCF5Au+i)=1u;
    *dg(m,0xD681u)=saved_d681;
    *dg(m,0xD682u)=saved_d682;
    if(saved_d681!=0u)*dg(m,0xD681u)=2u;
    if(saved_d682!=0u)*dg(m,0xD682u)=2u;
    if(b.call_1f494)b.call_1f494(1u);
    if(b.call_22d70)b.call_22d70();
    return result;
}
static_assert(0x0A5BCu-0x0A254u==872u);

// 0A5BC..0A608: save and suppress the two input-state bytes while marking all
// four party panels dirty.
void function_0a5bc(Machine& m,Application15Boundary& b) noexcept {
    for(u32 i=0;i<4u;++i)*dg(m,0xCF5Au+i)=1u;
    store_le32(dg(m,0x12BA0u),static_cast<u32>(*dg(m,0xD681u)));
    *dg(m,0xD681u)=0u;
    store_le32(dg(m,0x12BA4u),static_cast<u32>(*dg(m,0xD682u)));
    *dg(m,0xD682u)=0u;
    if(b.call_1f494)b.call_1f494(1u);
}
static_assert(0x0A609u-0x0A5BCu==77u);

// 0A60C..0A666: restore the saved input-state bytes, normalize nonzero states
// to two, and mark all four party panels dirty.
void function_0a60c(Machine& m,Application15Boundary& b) noexcept {
    *dg(m,0xD681u)=*dg(m,0x12BA0u);
    *dg(m,0xD682u)=*dg(m,0x12BA4u);
    if(*dg(m,0xD681u)!=0u)*dg(m,0xD681u)=2u;
    if(*dg(m,0xD682u)!=0u)*dg(m,0xD682u)=2u;
    for(u32 i=0;i<4u;++i)*dg(m,0xCF5Au+i)=1u;
    if(b.call_1f494)b.call_1f494(1u);
}
static_assert(0x0A667u-0x0A60Cu==91u);

// 0A668..0A790: play the encoded sound, animate visual 2 across all party
// panels, then install the 21A6A timer/21A14 bit on each non-8000h party slot.
void function_0a668(Machine& m,Application15Boundary& b,u32 value) noexcept {
    if(b.call_28110)b.call_28110(load_le32(dg(m,0x258D4u))+0x3Eu,0xFFu,0u,1u);
    for(u32 i=0;i<4u;++i)*dg(m,0xCF5Au+i)=1u;
    const u8 saved_d681=*dg(m,0xD681u);
    *dg(m,0xD681u)=0u;
    const u8 saved_d682=*dg(m,0xD682u);
    *dg(m,0xD682u)=0u;
    store_le32(dg(m,0x12BA0u),saved_d681);
    store_le32(dg(m,0x12BA4u),saved_d682);
    if(b.call_1f494)b.call_1f494(1u);

    function_09b4c(m,b,value,2u,1u);

    *dg(m,0xD681u)=*dg(m,0x12BA0u);
    *dg(m,0xD682u)=*dg(m,0x12BA4u);
    if(*dg(m,0xD681u)!=0u)*dg(m,0xD681u)=2u;
    if(*dg(m,0xD682u)!=0u)*dg(m,0xD682u)=2u;
    for(u32 i=0;i<4u;++i)*dg(m,0xCF5Au+i)=1u;
    if(b.call_1f494)b.call_1f494(1u);

    for(u32 slot=0u,base=0u;slot<4u;++slot,base+=0x248u) {
        if((load_le16(dg(m,0x218F0u+base))&0x8000u)!=0u)continue;
        *dg(m,0xCF5Au+slot)=1u;
        *dg(m,0x21A14u+base)=static_cast<u8>(*dg(m,0x21A14u+base)|0x20u);
        const u32 factor=b.call_22e40?b.call_22e40(3u,value):0u;
        const u32 now=b.call_19cc4?b.call_19cc4():0u;
        store_le32(dg(m,0x21A6Au+base),now+factor*0x438u);
    }
}
static_assert(0x0A791u-0x0A668u==297u);

// -----------------------------------------------------------------------------
// Batch 16: 0A794..0B515 status wrappers, map-object helpers, event clearing,
// nearby-object effects, projectile spawning, and moving-record conversion.
// -----------------------------------------------------------------------------

struct Application16Boundary {
    void (*call_28110)(u32,u32,u32,u32)=nullptr;
    void (*call_1f494)(u32)=nullptr;
    void (*call_09b4c)(s32,u32,u32)=nullptr;
    u32 (*call_22e40)(u32,u32)=nullptr;
    u32 (*call_19cc4)()=nullptr;
    u32 (*call_1202c)(u32,u32)=nullptr;
    u32 (*call_1e8c8)(u32,u32)=nullptr;
    void (*call_20c0c)(u32,u32,u32)=nullptr;
    u32 (*call_20cf4)(u32)=nullptr;
    void (*call_002a8)(s32)=nullptr;
    u32 (*call_1c924)(u32)=nullptr;
    void (*call_1c77c)(u32,u32)=nullptr;
    void (*call_0871c)(u32,u32)=nullptr;
    void (*call_086c4)(u32,u32)=nullptr;
    void (*call_0893c)()=nullptr;
    void (*call_2df2c)(u32*,u32*,u32,u32)=nullptr;
    u32 (*call_2d5d0)(u32,u32,s32)=nullptr;
    void (*call_1f1c8)()=nullptr;
    void (*call_1ea78)()=nullptr;
    u32 (*call_1c328)(u32,u32,u32,u32)=nullptr;
    u32 (*call_2acdc)(u32)=nullptr;
    void (*call_e284)(u32,u32,u32,u32,u32)=nullptr;
    u32 (*call_30ca2)()=nullptr;
    u32 (*call_2c9dc)(u32,u32)=nullptr;
    u32 (*call_1e930)(u32)=nullptr;
    void (*call_28168)()=nullptr;
    void (*call_08c88)(s32)=nullptr;
};

static void app16_dirty_all(Machine& m) noexcept {
    for(u32 i=0;i<4u;++i)*dg(m,0xCF5Au+i)=1u;
}

static void app16_suspend_ui(Machine& m,Application16Boundary& b) noexcept {
    app16_dirty_all(m);
    store_le32(dg(m,0x12BA0u),static_cast<u32>(*dg(m,0xD681u)));
    *dg(m,0xD681u)=0u;
    store_le32(dg(m,0x12BA4u),static_cast<u32>(*dg(m,0xD682u)));
    *dg(m,0xD682u)=0u;
    if(b.call_1f494)b.call_1f494(1u);
}

static void app16_restore_ui(Machine& m,Application16Boundary& b) noexcept {
    *dg(m,0xD681u)=*dg(m,0x12BA0u);
    *dg(m,0xD682u)=*dg(m,0x12BA4u);
    if(*dg(m,0xD681u)!=0u)*dg(m,0xD681u)=2u;
    if(*dg(m,0xD682u)!=0u)*dg(m,0xD682u)=2u;
    app16_dirty_all(m);
    if(b.call_1f494)b.call_1f494(1u);
}

void function_0a794(Machine& m,Application16Boundary& b,u32 slot) noexcept {
    if(b.call_28110)b.call_28110(load_le32(dg(m,0x258D4u))+0x30u,0xFFu,0u,1u);
    app16_suspend_ui(m,b);
    if(b.call_09b4c)b.call_09b4c(static_cast<s32>(slot),2u,1u);
    app16_restore_ui(m,b);
    const u32 target=slot*0x248u;
    for(u32 party=0u,base=0u;party<4u;++party,base+=0x248u) {
        if((load_le16(dg(m,0x218F0u+base))&0x8000u)!=0u)continue;
        *dg(m,0xCF5Au+party)=1u;
        *dg(m,0x21A12u+target)=static_cast<u8>(*dg(m,0x21A12u+target)|0x20u);
    }
}
static_assert(0x0A8A0u-0x0A794u==268u);

u32 function_0a8a0(Machine& m,Application16Boundary& b,u32 slot) noexcept {
    const u32 base=slot*0x248u;
    if((*dg(m,0x21A13u+base)&0x10u)!=0u)return 0u;
    if(b.call_28110)b.call_28110(load_le32(dg(m,0x258D4u))+0x1Au,0xFFu,0u,1u);
    app16_suspend_ui(m,b);
    if(b.call_09b4c)b.call_09b4c(static_cast<s32>(slot),2u,0u);
    app16_restore_ui(m,b);
    *dg(m,0x21A13u+base)=static_cast<u8>(*dg(m,0x21A13u+base)|0x10u);
    const u32 first=b.call_22e40?b.call_22e40(3u,slot):0u;
    const u32 now=b.call_19cc4?b.call_19cc4():0u;
    store_le32(dg(m,0x21A46u+base),now+first*0x438u);
    const s32 rolled=static_cast<s32>(b.call_22e40?b.call_22e40(3u,slot):0u);
    s32 delta=rolled/3+1;
    const s32 current=static_cast<s32>(*dg(m,0x21912u+base));
    const s32 cap=*dg(m,0x218F4u+base)==2u?0x11:0x12;
    if(current+delta>cap)delta=cap-current;
    const u32 applied=b.call_1202c?b.call_1202c(slot,static_cast<u32>(delta)):0u;
    store_le32(dg(m,0x21A03u+base),applied);
    return 1u;
}
static_assert(0x0AA2Du-0x0A8A0u==397u);

u32 function_0aa30(Machine& m,Application16Boundary& b,u32 slot) noexcept {
    const u32 base=slot*0x248u;
    if((*dg(m,0x21A13u+base)&0x20u)!=0u)return 0u;
    if(b.call_28110)b.call_28110(load_le32(dg(m,0x258D4u))+0x19u,0xFFu,0u,1u);
    app16_suspend_ui(m,b);
    if(b.call_09b4c)b.call_09b4c(static_cast<s32>(slot),2u,0u);
    app16_restore_ui(m,b);
    *dg(m,0x21A13u+base)=static_cast<u8>(*dg(m,0x21A13u+base)|0x20u);
    const u32 first=b.call_22e40?b.call_22e40(3u,slot):0u;
    const u32 now=b.call_19cc4?b.call_19cc4():0u;
    store_le32(dg(m,0x21A4Au+base),now+first*0x438u);
    const s32 rolled=static_cast<s32>(b.call_22e40?b.call_22e40(3u,slot):0u);
    s32 delta=rolled/3+1;
    const s32 current=static_cast<s32>(*dg(m,0x21915u+base));
    const u8 cls=*dg(m,0x218F4u+base);
    const s32 cap=(cls==2u||cls==5u)?0x13:0x12;
    if(current+delta>cap)delta=cap-current;
    store_le32(dg(m,0x21A07u+base),static_cast<u32>(delta));
    *dg(m,0x21915u+base)=static_cast<u8>(*dg(m,0x21915u+base)+static_cast<u8>(delta));
    return 1u;
}
static_assert(0x0ABC4u-0x0AA30u==404u);

static void app16_div_remainders(Machine& m,u32 table_index,s32& rx,s32& ry) noexcept {
    const s32 packed=static_cast<s32>(load_le32(dg(m,0x15A2Cu+table_index*0x1Cu)));
    const s32 width=static_cast<s16>(load_le16(dg(m,0x25619u)));
    const s32 height=static_cast<s16>(load_le16(dg(m,0x2561Bu)));
    rx=packed%width;
    ry=packed%height;
}

void function_0abc4(Machine& m,Application16Boundary& b,u32 table_index) noexcept {
    s32 rx=0,ry=0;
    app16_div_remainders(m,table_index,rx,ry);
    if(b.call_28110)b.call_28110(load_le32(dg(m,0x258D4u))+0x49u,0xFFu,0u,1u);
    const u32 obj=b.call_1e8c8?b.call_1e8c8(static_cast<u32>(rx),static_cast<u32>(ry)):0u;
    if(obj!=0u&&*dg(m,obj)==1u)*dg(m,obj+1u)=static_cast<u8>(*dg(m,obj+1u)|0x40u);
}
static_assert(0x0AC2Cu-0x0ABC4u==104u);

void function_0ac2c(Machine& m,Application16Boundary& b,u32 table_index) noexcept {
    s32 rx=0,ry=0;
    app16_div_remainders(m,table_index,rx,ry);
    if(b.call_28110)b.call_28110(load_le32(dg(m,0x258D4u))+0x32u,0xFFu,0u,1u);
    const u32 obj=b.call_1e8c8?b.call_1e8c8(static_cast<u32>(rx),static_cast<u32>(ry)):0u;
    if(obj==0u)return;
    const u8 type=*dg(m,obj);
    if(type==1u) {
        if((*dg(m,obj+1u)&0x40u)!=0u) {
            *dg(m,obj+1u)=static_cast<u8>(*dg(m,obj+1u)&0xBFu);
            return;
        }
        if(*dg(m,obj+6u)==0u)return;
        const u8 saved_bit2=static_cast<u8>((*dg(m,obj+1u)>>2)&1u);
        *dg(m,obj+1u)=static_cast<u8>(*dg(m,obj+1u)|0x04u);
        if(b.call_20c0c)b.call_20c0c(static_cast<u32>(rx),static_cast<u32>(ry),obj);
        *dg(m,obj+1u)=static_cast<u8>((*dg(m,obj+1u)&0xFBu)|(saved_bit2<<2));
        return;
    }
    if(type!=4u)return;
    const u32 active=b.call_20cf4?b.call_20cf4(*dg(m,obj+6u)):0u;
    if(active!=0u) {
        if(b.call_002a8)b.call_002a8(static_cast<s8>(*dg(m,obj+2u)));
        *dg(m,obj+1u)=static_cast<u8>(*dg(m,obj+1u)&0xFBu);
        return;
    }
    const u32 token=b.call_1c924?b.call_1c924(0x320u):0u;
    if(b.call_1c77c)b.call_1c77c(token,0x320u);
}
static_assert(0x0AD24u-0x0AC2Cu==248u);

void function_0ad24(Machine& m,Application16Boundary& b,s32 selected) noexcept {
    const u32 begin=selected==-1?0u:static_cast<u32>(selected);
    const u32 end=selected==-1?4u:static_cast<u32>(selected+1);
    for(u32 slot=begin;slot<end;++slot) {
        const u32 base=slot*0x248u;
        if((load_le16(dg(m,0x218F0u+base))&0x8000u)!=0u)continue;
        for(u32 bit=0;bit<26u;++bit) {
            if((load_le32(dg(m,0x21A12u+base))&(1u<<(bit&31u)))!=0u&&b.call_0871c)
                b.call_0871c(slot,bit);
        }
    }
    for(u32 slot=begin;slot<end;++slot) {
        const u32 base=slot*0x248u;
        if((load_le16(dg(m,0x218F0u+base))&0x8000u)!=0u)continue;
        for(u32 event=0;event<9u;++event) {
            const u32 mask=1u<<(event&31u);
            if((load_le32(dg(m,0x21AA2u+base))&mask)==0u)continue;
            store_le32(dg(m,0x21AA2u+base),load_le32(dg(m,0x21AA2u+base))^mask);
            *dg(m,0xCF5Au+slot)=1u;
            store_le32(dg(m,0x21AA6u+base+event*4u),0u);
            if(b.call_086c4)b.call_086c4(slot,event);
        }
    }
    if(b.call_0893c)b.call_0893c();
}
static_assert(0x0AE2Du-0x0AD24u==265u);

void function_0ae30(Machine& m,Application16Boundary& b) noexcept {
    const u32 saved0=load_le32(dg(m,0x255D0u));
    const u32 saved4=load_le32(dg(m,0x255D4u));
    if(b.call_28110)b.call_28110(load_le32(dg(m,0x258D4u))+0x48u,0xFFu,0u,1u);
    for(;;) {
        u32 d0=0u,d4=0u;
        if(b.call_2df2c)b.call_2df2c(&d0,&d4,0x5000u,load_le32(dg(m,0xC658u)));
        store_le32(dg(m,0x255D0u),load_le32(dg(m,0x255D0u))+d0);
        store_le32(dg(m,0x255D4u),load_le32(dg(m,0x255D4u))+d4);
        const u32 blocked=b.call_2d5d0?b.call_2d5d0(load_le32(dg(m,0x255D4u)),load_le32(dg(m,0x255D0u)),-2):0u;
        if(static_cast<u16>(blocked)==0u)break;
        if(b.call_1f1c8)b.call_1f1c8();
        if(b.call_1ea78)b.call_1ea78();
    }
    store_le32(dg(m,0x255D0u),saved0);
    store_le32(dg(m,0x255D4u),saved4);
    if(b.call_1f1c8)b.call_1f1c8();
    if(b.call_1ea78)b.call_1ea78();
}
static_assert(0x0AED7u-0x0AE30u==167u);

void function_0aed8(Machine& m,Application16Boundary& b) noexcept {
    s32 budget=static_cast<s32>(b.call_1c328?b.call_1c328(4u,0x14u,0u,0u):0u);
    const s32 y0=static_cast<s16>(load_le16(dg(m,0x255D2u)))-1;
    const s32 x0=static_cast<s16>(load_le16(dg(m,0x255D6u)))-1;
    if(b.call_28110)b.call_28110(load_le32(dg(m,0x258D4u))+0x12u,0xFFu,0u,1u);
    const s32 width=static_cast<s16>(load_le16(dg(m,0x25619u)));
    const s32 height=static_cast<s16>(load_le16(dg(m,0x2561Bu)));
    const s32 last=static_cast<s16>(load_le16(dg(m,0x258A2u)));
    for(s32 y=y0;y<y0+2;++y) {
        if(y<0||y>=height)continue;
        for(s32 x=x0;x<x0+2;++x) {
            if(x<0||x>=width)continue;
            const u32 tile=static_cast<u32>(y*width+x);
            for(s32 i=0;i<last;++i) {
                const u32 rec=0x14D80u+static_cast<u32>(i)*0x16u;
                if((*dg(m,rec+0x0Bu)&0x02u)!=0u)continue;
                const u32 type=load_le16(dg(m,rec+0x0Eu));
                if((b.call_2acdc?b.call_2acdc(type):0u)==0u||type==8u)continue;
                if(load_le16(dg(m,rec+0x0Cu))!=tile)continue;
                if(budget<=0)return;
                const u32 desc=load_le32(dg(m,0x22230u))+type*0x45u;
                if((*dg(m,desc+0x31u)&0x80u)!=0u)continue;
                const u32 amount=static_cast<u32>(static_cast<s16>(load_le16(dg(m,rec+0x12u)))+1);
                if(b.call_e284)b.call_e284(rec,desc,static_cast<u32>(i),amount,0u);
                --budget;
            }
        }
    }
}
static_assert(0x0B02Cu-0x0AED8u==340u);

static s32 app16_wrap_angle(s32 angle) noexcept {
    while(angle<0)angle+=0x168;
    while(angle>=0x168)angle-=0x168;
    return angle;
}

static s16 app16_hiword(u32 v) noexcept { return static_cast<s16>(v>>16); }

void function_0b02c(Machine& m,Application16Boundary& b,s32 angle_first,s32 angle_last,
                    s32 max_steps,u32 callback_value,u32 descriptor_mask,u32 duplicate_guard,
                    u32 moving_type,s32 sound_offset,u32 initial_y,u32 initial_x) noexcept {
    const u8 saved_c02f=*dg(m,0xC02Fu);
    *dg(m,0xC02Fu)=0u;
    for(s32 angle=angle_first;angle<=angle_last;++angle) {
        const u32 rnd=b.call_30ca2?b.call_30ca2():0u;
        const u32 orientation_seed=(rnd*3u)>>15;
        const s32 heading=app16_wrap_angle(static_cast<s32>(load_le32(dg(m,0xC658u)))+angle);
        u32 dy=0u,dx=0u;
        if(b.call_2df2c)b.call_2df2c(&dy,&dx,0xA000u,static_cast<u32>(heading));
        u32 x=initial_x;
        u32 y=initial_y;
        while((b.call_2c9dc?b.call_2c9dc(x,y):0u)!=0u) { x+=dx; y+=dy; }
        const s16 height=static_cast<s16>(load_le16(dg(m,0x2561Bu)));
        const s16 width=static_cast<s16>(load_le16(dg(m,0x25619u)));
        if(app16_hiword(x)>height||app16_hiword(y)>width) { *dg(m,0xC02Fu)=saved_c02f; return; }
        bool blocked=false;
        for(s32 step=0;step<=max_steps;++step) {
            const u32 orient=(orientation_seed&7u)<<6;
            x+=dx; y+=dy;
            const s16 hy=app16_hiword(x);
            const s16 hx=app16_hiword(y);
            if(hy>=height||hx>=width)continue;
            const u32 tile=static_cast<u32>(hy*width+hx);
            const u8 tile_flags=*dg(m,0x198E0u+tile*2u);
            if((tile_flags&0x80u)!=0u) {
                if(static_cast<u16>(b.call_1e930?b.call_1e930(tile):0u)!=0u)blocked=true;
            } else if((tile_flags&0x3Fu)!=0u)blocked=true;
            if(blocked)break;

            u32 record_index=load_le32(dg(m,0xC038u));
            if(duplicate_guard!=0u) {
                record_index=0u;
                const u32 count=load_le32(dg(m,0xC038u));
                for(;record_index<count;++record_index) {
                    if(load_le16(dg(m,0x248F8u+record_index*0x16u))==static_cast<u16>(tile))break;
                }
            }
            if(record_index!=load_le32(dg(m,0xC038u)))continue;

            const u32 rec=0x248ECu+record_index*0x16u;
            u32 stored_x=x,stored_y=y;
            if(duplicate_guard!=0u) {
                stored_x=(stored_x&0xFFFF0000u)|0x8000u;
                stored_y=(stored_y&0xFFFF0000u)|0x8000u;
            }
            store_le32(dg(m,rec),stored_x);
            store_le32(dg(m,rec+4u),stored_y);
            store_le16(dg(m,rec+0x0Cu),static_cast<u16>(tile));
            store_le16(dg(m,rec+0x0Eu),static_cast<u16>(moving_type));
            store_le16(dg(m,rec+0x14u),static_cast<u16>((load_le16(dg(m,rec+0x14u))&0xFE3Fu)|static_cast<u16>(orient)));
            if(moving_type==8u) {
                const u32 r=b.call_30ca2?b.call_30ca2():0u;
                const u16 ro=static_cast<u16>((((r*3u)>>15)&7u)<<6);
                store_le16(dg(m,rec+0x14u),static_cast<u16>((load_le16(dg(m,rec+0x14u))&0xFE3Fu)|ro));
            }
            store_le32(dg(m,0xC038u),load_le32(dg(m,0xC038u))+1u);

            const s32 world_last=static_cast<s16>(load_le16(dg(m,0x258A2u)));
            for(s32 i=0;i<=world_last;++i) {
                const u32 wrec=0x14D80u+static_cast<u32>(i)*0x16u;
                if((*dg(m,wrec+0x0Bu)&0x02u)!=0u)continue;
                if(load_le16(dg(m,wrec+0x0Cu))!=static_cast<u16>(tile))continue;
                if((*dg(m,wrec+0x0Bu)&0x10u)==0u)continue;
                if(load_le16(dg(m,wrec+0x12u))==0u)continue;
                const u32 type=load_le16(dg(m,wrec+0x0Eu));
                const u32 desc=load_le32(dg(m,0x22230u))+type*0x45u;
                if(moving_type==0x0Bu) {
                    if(type!=8u||static_cast<s16>(load_le16(dg(m,wrec+0x12u)))>10)continue;
                    store_le16(dg(m,wrec+0x12u),static_cast<u16>(load_le16(dg(m,wrec+0x12u))-2u));
                    if(b.call_e284)b.call_e284(wrec,desc,static_cast<u32>(i),0u,0u);
                } else {
                    if((load_le32(dg(m,desc+0x31u))&descriptor_mask)!=0u)continue;
                    if(b.call_e284)b.call_e284(wrec,desc,static_cast<u32>(i),callback_value,0u);
                }
            }
        }
    }

    u32 sound=0u;
    if(moving_type==8u||moving_type==0u)sound=load_le32(dg(m,0x258BCu))+0x19u;
    else sound=load_le32(dg(m,0x258D4u))+static_cast<u32>(sound_offset);
    if(b.call_28110)b.call_28110(sound,0xFFu,0u,1u);
    if(b.call_1f1c8)b.call_1f1c8();
    if(b.call_1ea78)b.call_1ea78();
    if(b.call_28168)b.call_28168();
    for(u32 i=0;i<3u;++i) {
        if(b.call_1f1c8)b.call_1f1c8();
        if(b.call_1ea78)b.call_1ea78();
    }
    for(;;) {
        const u32 count=load_le32(dg(m,0xC038u));
        u32 i=0u;
        for(;i<count;++i)if(load_le16(dg(m,0x248FAu+i*0x16u))==static_cast<u16>(moving_type))break;
        if(i==count)break;
        if(b.call_08c88)b.call_08c88(static_cast<s32>(i));
        else break;
    }
    *dg(m,0xC02Fu)=saved_c02f;
}
static_assert(0x0B40Eu-0x0B02Cu==994u);

void function_0b410(Machine& m,Application16Boundary& b,u32 index) noexcept {
    const u32 wrec=0x14D80u+index*0x16u;
    const u32 moving_index=load_le32(dg(m,0xC038u));
    const u32 rec=0x248ECu+moving_index*0x16u;
    store_le16(dg(m,rec+0x0Eu),0x0Au);
    store_le32(dg(m,rec+4u),load_le32(dg(m,wrec+4u)));
    store_le32(dg(m,rec),load_le32(dg(m,wrec)));
    store_le16(dg(m,rec+0x0Cu),load_le16(dg(m,wrec+0x0Cu)));
    store_le32(dg(m,0xC038u),moving_index+1u);
    const u32 roll=b.call_1c328?b.call_1c328(6u,8u,0u,0u):0u;
    const u32 type=load_le16(dg(m,wrec+0x0Eu));
    const u32 desc=load_le32(dg(m,0x22230u))+type*0x45u;
    if((*dg(m,desc+0x32u)&0x21u)==0u&&b.call_e284)b.call_e284(wrec,desc,index,roll,0u);
    if(b.call_28110)b.call_28110(load_le32(dg(m,0x258D4u))+0x20u,0xFFu,0u,1u);
    if(b.call_1f1c8)b.call_1f1c8();
    if(b.call_1ea78)b.call_1ea78();
    if(b.call_28168)b.call_28168();
    if(b.call_1f1c8)b.call_1f1c8();
    if(b.call_1ea78)b.call_1ea78();
    if(b.call_28110)b.call_28110(load_le32(dg(m,0x258D4u))+0x21u,0xFFu,0u,1u);
    for(u32 i=0;i<2u;++i) {
        if(b.call_28168)b.call_28168();
        if(b.call_1f1c8)b.call_1f1c8();
        if(b.call_1ea78)b.call_1ea78();
    }
    if(b.call_28168)b.call_28168();
    if(b.call_08c88)b.call_08c88(static_cast<s32>(load_le32(dg(m,0xC038u))-1u));
}
static_assert(0x0B516u-0x0B410u==262u);

// -----------------------------------------------------------------------------
// Batch 17: 0B518..0C6D6 nearby-world effects and 132-way item/effect dispatcher.
// The CS table at 0B740..0B94F is data and is accounted in RESIDUAL_LEDGER.md.
// -----------------------------------------------------------------------------

struct Application17Boundary {
    void (*call_19ccc)(u32)=nullptr;
    void (*call_19cf4)()=nullptr;
    u32 (*call_19cc4)()=nullptr;
    void (*call_e578)(u32)=nullptr;
    u32 (*call_22e40)(u32,u32)=nullptr;
    u32 (*call_1c328)(u32,u32,u32,u32)=nullptr;
    void (*call_0b02c)(s32,s32,s32,u32,u32,u32,u32,s32,u32,u32)=nullptr;
    s32 (*call_095c0)()=nullptr;
    s32 (*call_09418)(u32)=nullptr;
    u32 (*call_0a254)(u32,u32,s32,u32,s32,u32,u32,u32)=nullptr;
    u32 (*call_09790)(u32,u32,s32,u32,u32,u32,u32)=nullptr;
    void (*call_28110)(u32,u32,u32,u32)=nullptr;
    void (*call_31f38)(u32)=nullptr;
    void (*call_1f494)(u32)=nullptr;
    void (*call_09b4c)(s32,u32,u32)=nullptr;
    void (*call_f238)(u32,u32,u32)=nullptr;
    void (*call_0a794)(u32)=nullptr;
    void (*call_0a668)(u32)=nullptr;
    void (*call_08a80)(u32,s32,s32,u32,u32)=nullptr;
    void (*call_0abc4)(u32)=nullptr;
    void (*call_0ac2c)(u32)=nullptr;
    void (*call_0ad24)(s32)=nullptr;
    void (*call_0ae30)()=nullptr;
    void (*call_0aed8)(u32)=nullptr;
    void (*call_1c77c)(u32,u32)=nullptr;
    u32 (*call_0a8a0)(u32)=nullptr;
    u32 (*call_0aa30)(u32)=nullptr;
    void (*call_0b410)(u32)=nullptr;
    void (*call_e284)(u32,u32,u32,u32,u32)=nullptr;
    // Rank >= 9 in 0B518 writes through the encoded rank-indexed stack alias
    // into saved-register / return / caller-frame slots. Normal data should not
    // reach this edge; expose it instead of silently discarding the write.
    void (*write_0b518_outer_frame)(u32,u32)=nullptr;
};

static u32 app17_abs32_bits(u32 value) noexcept {
    const u32 sign=(value&0x80000000u)!=0u?0xFFFFFFFFu:0u;
    return (value^sign)-sign;
}

void function_0b518(Machine& m,Application17Boundary& b,u32 slot,u32 mode) noexcept {
    const u32 actor_base=slot*0x248u;
    if((*dg(m,0x21A13u+actor_base)&0x02u)!=0u)return;

    // 30D30 clears exactly five dwords at ESP+00..10. ESP-4 still contains
    // zero from the first argument of the immediately preceding 1C328 call;
    // rank zero can later overwrite that stale slot through the same alias.
    u32 prelocal=0u;
    u32 scratch[5]={0u,0u,0u,0u,0u};
    u32 threshold=b.call_1c328?b.call_1c328(1u,0x14u,0u,0u):0u;
    u32 offset=0u;
    if(mode!=0u)offset=(b.call_22e40?b.call_22e40(slot,1u):0u)-2u;
    else offset=b.call_22e40?b.call_22e40(slot,3u):0u;
    u32 remaining=b.call_1c328?b.call_1c328(2u,6u,0u,0u):0u;
    u32 counter=0u;
    u32 record_offset=0u;

    auto write_rank_alias=[&](u32 rank,u32 value) noexcept {
        if(rank==0u)prelocal=value;
        else if(rank<=5u)scratch[rank-1u]=value;
        else if(rank==6u)counter=value;
        else if(rank==7u)offset=value;
        else if(rank==8u)threshold=value;
        else if(b.write_0b518_outer_frame)b.write_0b518_outer_frame(rank,value);
    };

    while(static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x258A2u))))>
          static_cast<s32>(counter)) {
        const u32 rec=0x14D80u+record_offset;
        const u32 kind=load_le16(dg(m,rec+0x0Cu));
        if((*dg(m,0x198E0u+kind*2u)&0x40u)!=0u) {
            const u32 type=load_le16(dg(m,rec+0x0Eu));
            const u32 desc=load_le32(dg(m,0x22230u))+type*0x45u;
            if((*dg(m,desc+0x3Au)&0x20u)!=0u) {
                const u32 dx=app17_abs32_bits(load_le32(dg(m,0x255D4u))-load_le32(dg(m,rec)));
                const u32 dy=app17_abs32_bits(load_le32(dg(m,0x255D0u))-load_le32(dg(m,rec+4u)));
                if(static_cast<s32>(dx)<0xA0000 && static_cast<s32>(dy)<0xA0000) {
                    const u32 rank=*dg(m,desc+0x1Du);
                    const u32 table_index=0x0Cu-rank+offset;
                    bool rank_gate=true;
                    if(remaining==0u) {
                        if(rank>=6u)rank_gate=false;
                        else if(rank==0u)rank_gate=prelocal!=0u;
                        else rank_gate=scratch[rank-1u]!=0u;
                    }
                    if(rank_gate) {
                        const s32 code=static_cast<s32>(static_cast<s8>(*dg(m,0x67B2u+table_index)));
                        if(code<0) {
                            --remaining;
                            if(b.call_e284)b.call_e284(rec,desc,counter,
                                static_cast<u32>(static_cast<s32>(static_cast<s16>(load_le16(dg(m,rec+0x12u))))),0u);
                            if(code==-2) {
                                const u32 value=b.call_1c328?b.call_1c328(2u,4u,0u,0u):0u;
                                write_rank_alias(rank,value);
                            }
                        } else {
                            if((load_le32(dg(m,desc+0x31u))&0x04u)!=0u)threshold-=4u;
                            if(code<=static_cast<s32>(threshold)) {
                                --remaining;
                                *dg(m,rec+8u)=static_cast<u8>(*dg(m,rec+8u)|0x10u);
                            }
                        }
                    }
                }
            }
        }
        ++counter;
        record_offset+=0x16u;
    }
}
static_assert(0x0B6F1u-0x0B518u==473u);

void function_0b6f4(Machine& m) noexcept {
    const u32 saved_desc=load_le32(dg(m,0x22230u));
    u32 record_offset=0u;
    for(s32 counter=0,
            count=static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x258A2u))));
        counter<count;++counter,record_offset+=0x16u) {
        const u32 rec=0x14D80u+record_offset;
        if((*dg(m,rec+0x0Bu)&0x02u)!=0u)continue;
        const u32 type=load_le16(dg(m,rec+0x0Eu));
        const u32 desc=saved_desc+type*0x45u;
        if((*dg(m,desc+0x3Au)&0x20u)!=0u)
            *dg(m,rec+9u)=static_cast<u8>(*dg(m,rec+9u)|0x02u);
    }
    store_le32(dg(m,0x22230u),saved_desc);
}
static_assert(0x0B73Du-0x0B6F4u==73u);

static void app17_sound(Machine& m,Application17Boundary& b,u32 offset,u32 tail=1u) noexcept {
    if(b.call_28110)b.call_28110(load_le32(dg(m,0x258D4u))+offset,0xFFu,0u,tail);
}

static u32 app17_apply_0a254(Application17Boundary& b,u32 a0,u32 effect,s32 a2,u32 a3,
                             s32 visual,u32 slot,u32 a6,u32 a7) noexcept {
    return b.call_0a254?b.call_0a254(a0,effect,a2,a3,visual,slot,a6,a7):0u;
}

static u32 app17_apply_09790(Application17Boundary& b,u32 effect,u32 slot,s32 sound_off,
                             u32 mask,u32 index,u32 duration_a,u32 duration_b) noexcept {
    return b.call_09790?b.call_09790(effect,slot,sound_off,mask,index,duration_a,duration_b):0u;
}

static void app17_launch(Machine& m,Application17Boundary& b,u32 type,u32 slot) noexcept {
    const s32 angle=b.call_09418?b.call_09418(1u):0;
    if(b.call_08a80)b.call_08a80(type,angle,static_cast<s32>(slot),
        load_le32(dg(m,0x255D4u)),load_le32(dg(m,0x255D0u)));
}

[[nodiscard]] u32 function_0b950(Machine& m,Application17Boundary& b,u32 command,u32 slot) noexcept {
    u32 result=1u;
    if(b.call_19ccc)b.call_19ccc(0u);
    *dg(m,0xBF38u)=0u;

    if(command<=0x83u) {
        switch(command) {
        case 0x00u:
            result=app17_apply_0a254(b,1u,0u,0,0u,3,slot,0u,0x438u);
            break;
        case 0x01u: {
            if(b.call_e578)b.call_e578(slot);
            const u32 r=b.call_22e40?b.call_22e40(slot,4u):0u;
            const u32 random=b.call_1c328?b.call_1c328(1u,3u,0u,0u):0u;
            if(b.call_0b02c)b.call_0b02c(-0x3C,0x3C,7,random+r*2u,0x2100u,1u,8u,7,
                load_le32(dg(m,0x255D0u)),load_le32(dg(m,0x255D4u)));
            break;
        }
        case 0x02u: {
            const u32 r=(b.call_22e40?b.call_22e40(slot,4u):0u)+3u;
            result=app17_apply_09790(b,0x7Eu,slot,0x0A,1u,0u,r,0x438u);
            break;
        }
        case 0x03u: {
            const u32 base=slot*0x248u;
            *dg(m,0x21A14u+base)=static_cast<u8>(*dg(m,0x21A14u+base)|0x04u);
            const u32 now=b.call_19cc4?b.call_19cc4():0u;
            store_le32(dg(m,0x21A5Eu+base),now+0x438u);
            store_le16(dg(m,0xF03Cu),static_cast<u16>(load_le16(dg(m,0xF03Cu))+1u));
            *dg(m,0xCF5Au+slot)=1u;
            app17_sound(m,b,0x14u);
            break;
        }
        case 0x04u: {
            const u32 base=slot*0x248u;
            *dg(m,0x21A12u+base)=static_cast<u8>(*dg(m,0x21A12u+base)|0x02u);
            const u32 r=b.call_22e40?b.call_22e40(slot,4u):0u;
            const u32 delay=(r*0x2A30u);
            const u32 now=b.call_19cc4?b.call_19cc4():0u;
            store_le32(dg(m,0x21A1Au+base),now+delay);
            *dg(m,0xCF5Au+slot)=1u;
            app17_sound(m,b,0x34u);
            if(b.call_31f38)b.call_31f38(load_le32(dg(m,0xF91Cu)));
            break;
        }
        case 0x05u: app17_launch(m,b,6u,slot); break;
        case 0x06u: {
            const u32 r=b.call_22e40?b.call_22e40(slot,4u):0u;
            result=app17_apply_0a254(b,4u,2u,0,r*5u,0x42,slot,0u,0x438u);
            break;
        }
        case 0x0Au: {
            const s32 angle=b.call_09418?b.call_09418(1u):0;
            if(b.call_e578)b.call_e578(slot);
            const u32 random=b.call_1c328?b.call_1c328(3u,6u,0u,0u):0u;
            const s32 delta=static_cast<s32>(static_cast<u32>(angle)-load_le32(dg(m,0xC658u)));
            if(b.call_0b02c)b.call_0b02c(delta,delta,0x14,random,0x2100u,0u,8u,0,
                load_le32(dg(m,0x255D0u)),load_le32(dg(m,0x255D4u)));
            break;
        }
        case 0x0Bu: {
            const u32 r=(b.call_22e40?b.call_22e40(slot,4u):0u)+3u;
            result=app17_apply_0a254(b,8u,3u,1,r,6,slot,0u,0x438u);
            break;
        }
        case 0x0Cu: {
            app17_sound(m,b,0x2Du);
            for(u32 i=0;i<4u;++i)*dg(m,0xCF5Au+i)=1u;
            store_le32(dg(m,0x12BA0u),static_cast<u32>(*dg(m,0xD681u)));
            store_le32(dg(m,0x12BA4u),static_cast<u32>(*dg(m,0xD682u)));
            *dg(m,0xD682u)=0u;
            *dg(m,0xD681u)=*dg(m,0xD682u);
            if(b.call_1f494)b.call_1f494(1u);
            if(b.call_09b4c)b.call_09b4c(static_cast<s32>(slot),5u,0u);
            *dg(m,0xD681u)=*dg(m,0x12BA0u);
            *dg(m,0xD682u)=*dg(m,0x12BA4u);
            if(*dg(m,0xD681u)!=0u)*dg(m,0xD681u)=2u;
            if(*dg(m,0xD682u)!=0u)*dg(m,0xD682u)=2u;
            for(u32 i=0;i<4u;++i)*dg(m,0xCF5Au+i)=1u;
            if(b.call_1f494)b.call_1f494(1u);
            for(u32 i=0;i<0x15u;++i)if(b.call_f238)b.call_f238(slot,i,1u);
            result=1u;
            *dg(m,0xCF5Au+slot)=1u;
            break;
        }
        case 0x0Du: {
            const s32 index=b.call_09418?b.call_09418(0u):0;
            if(b.call_0ac2c)b.call_0ac2c(static_cast<u32>(index));
            break;
        }
        case 0x0Eu: {
            const s32 index=b.call_09418?b.call_09418(0u):0;
            if(b.call_0abc4)b.call_0abc4(static_cast<u32>(index));
            break;
        }
        case 0x0Fu: app17_launch(m,b,2u,slot); break;
        case 0x14u: app17_sound(m,b,0x16u); if(b.call_0ad24)b.call_0ad24(-1); break;
        case 0x15u: app17_launch(m,b,0u,slot); break;
        case 0x16u: app17_launch(m,b,4u,slot); break;
        case 0x17u: {
            const u32 r=(b.call_22e40?b.call_22e40(slot,4u):0u)+3u;
            result=app17_apply_0a254(b,0x10u,4u,1,r,0x25,slot,0u,0x438u);
            break;
        }
        case 0x18u: {
            const s32 index=b.call_095c0?b.call_095c0():-1;
            if(index==-1)result=0xFFFFFFFFu;
            else {
                app17_sound(m,b,0x43u);
                const u32 rec=0x14D80u+static_cast<u32>(index)*0x16u;
                const u32 type=load_le16(dg(m,rec+0x0Eu));
                const u32 desc=load_le32(dg(m,0x22230u))+type*0x45u;
                if((*dg(m,desc+0x32u)&0x01u)==0u)*dg(m,rec+8u)=static_cast<u8>(*dg(m,rec+8u)|0x80u);
            }
            break;
        }
        case 0x19u:
        case 0x1Au:
        case 0x29u:
        case 0x5Cu: {
            const s32 index=b.call_095c0?b.call_095c0():-1;
            if(index==-1) { result=0xFFFFFFFFu; break; }
            const u32 rec=0x14D80u+static_cast<u32>(index)*0x16u;
            u32 sound=0u,mask=0u,multiplier=0u,roll_kind=0u;
            if(command==0x19u) { sound=0x27u; mask=0x0108u; multiplier=0x870u; roll_kind=4u; }
            else if(command==0x1Au) { sound=0x2Au; mask=0x0102u; multiplier=0x438u; roll_kind=4u; }
            else if(command==0x29u) { sound=0x29u; mask=0x0110u; multiplier=0x438u; roll_kind=4u; }
            else { sound=0x28u; mask=0x0108u; multiplier=0x870u; roll_kind=3u; }
            app17_sound(m,b,sound);
            const u32 type=load_le16(dg(m,rec+0x0Eu));
            const u32 desc=load_le32(dg(m,0x22230u))+type*0x45u;
            if((load_le16(dg(m,desc+0x31u))&mask)!=0u)break;
            *dg(m,rec+8u)=static_cast<u8>(*dg(m,rec+8u)|0x08u);
            u32 duration=b.call_22e40?b.call_22e40(slot,roll_kind):0u;
            if(command==0x1Au)duration+=(b.call_1c328?b.call_1c328(1u,4u,0u,0u):0u);
            store_le16(dg(m,rec+0x10u),static_cast<u16>(duration*multiplier));
            break;
        }
        case 0x1Bu: if(b.call_0a794)b.call_0a794(slot); break;
        case 0x1Cu: app17_launch(m,b,1u,slot); break;
        case 0x1Du: result=app17_apply_09790(b,0x7Fu,slot,0x47,2u,1u,1u,0x2A30u); break;
        case 0x1Eu:
            app17_sound(m,b,0x15u);
            if(b.call_1c77c)b.call_1c77c(slot,0x31Du);
            break;
        case 0x1Fu: {
            const s32 index=b.call_095c0?b.call_095c0():-1;
            if(index==-1) { result=0u; break; }
            if(b.call_e578)b.call_e578(slot);
            const u32 rec=0x14D80u+static_cast<u32>(index)*0x16u;
            const u32 random=b.call_1c328?b.call_1c328(3u,0x0Au,0u,0u):0u;
            if(b.call_0b02c)b.call_0b02c(-0xB4,0xB4,7,random,0x900u,1u,9u,0x2C,
                load_le32(dg(m,rec+4u)),load_le32(dg(m,rec)));
            break;
        }
        case 0x20u: result=app17_apply_0a254(b,0x40u,6u,0,0u,0x45,slot,0u,0x438u); break;
        case 0x21u: if(b.call_0ae30)b.call_0ae30(); break;
        case 0x28u: app17_launch(m,b,3u,slot); break;
        case 0x32u: {
            const u32 r=b.call_22e40?b.call_22e40(slot,4u):0u;
            result=app17_apply_09790(b,0x80u,slot,0x0B,4u,2u,r,0x438u);
            break;
        }
        case 0x33u: if(b.call_0aed8)b.call_0aed8(slot); break;
        case 0x34u: {
            const u32 r=b.call_22e40?b.call_22e40(slot,4u):0u;
            result=app17_apply_09790(b,0x86u,slot,0x33,0x100u,8u,r,0x438u);
            break;
        }
        case 0x35u: {
            const s32 angle=b.call_09418?b.call_09418(1u):0;
            if(b.call_e578)b.call_e578(slot);
            const u32 random=b.call_1c328?b.call_1c328(3u,6u,0u,0u):0u;
            const s32 delta=static_cast<s32>(static_cast<u32>(angle)-load_le32(dg(m,0xC658u)));
            if(b.call_0b02c)b.call_0b02c(delta,delta,0x14,random,0x900u,0u,9u,0x3D,
                load_le32(dg(m,0x255D0u)),load_le32(dg(m,0x255D4u)));
            break;
        }
        case 0x36u: {
            const u32 base=slot*0x248u;
            if((*dg(m,0x21A14u+base)&0x02u)==0u)
                store_le32(dg(m,0xFD08u),load_le32(dg(m,0xFD08u))+1u);
            app17_sound(m,b,0x46u);
            *dg(m,0x21A14u+base)=static_cast<u8>(*dg(m,0x21A14u+base)|0x02u);
            const u32 r=b.call_22e40?b.call_22e40(slot,3u):0u;
            const u32 now=b.call_19cc4?b.call_19cc4():0u;
            store_le32(dg(m,0x21A5Au+base),now+r*0x438u);
            *dg(m,0xCF5Au+slot)=1u;
            break;
        }
        case 0x37u: {
            const u32 r=b.call_22e40?b.call_22e40(slot,4u):0u;
            result=app17_apply_0a254(b,0x80u,7u,0,r*10u,0x18,slot,0u,0x2A30u);
            break;
        }
        case 0x50u: result=app17_apply_0a254(b,0x100u,8u,2,6u,5,slot,0u,0x438u); break;
        case 0x51u: result=app17_apply_0a254(b,0xFFFFFFFFu,0x51u,3,0xFFFFFFFFu,0x10,slot,1u,0u); break;
        case 0x52u: result=app17_apply_09790(b,0x81u,slot,-1,0xFFFFFFFFu,0xFFFFFFFFu,0xFFFFFFFFu,0xFFFFFFFFu); break;
        case 0x53u: {
            const u32 base=slot*0x248u;
            if((*dg(m,0x21A14u+base)&0x10u)==0u)
                store_le16(dg(m,0x6DAEu),static_cast<u16>(load_le16(dg(m,0x6DAEu))+1u));
            app17_sound(m,b,0x13u);
            *dg(m,0x21A14u+base)=static_cast<u8>(*dg(m,0x21A14u+base)|0x10u);
            const u32 now=b.call_19cc4?b.call_19cc4():0u;
            for(u32 i=0;i<4u;++i)*dg(m,0xCF5Au+i)=1u;
            store_le32(dg(m,0x21A66u+base),now+0x2A30u);
            break;
        }
        case 0x54u: result=app17_apply_0a254(b,0x200u,9u,1,6u,0x30,slot,0u,0x438u); break;
        case 0x55u: {
            const u32 base=slot*0x248u;
            *dg(m,0x21A13u+base)=static_cast<u8>(*dg(m,0x21A13u+base)|0x04u);
            const u32 r=(b.call_22e40?b.call_22e40(slot,3u):0u)+10u;
            const u32 now=b.call_19cc4?b.call_19cc4():0u;
            store_le32(dg(m,0x21A3Eu+base),now+r*0x2A30u);
            *dg(m,0xCF5Au+slot)=1u;
            app17_sound(m,b,0x35u);
            if(b.call_31f38)b.call_31f38(load_le32(dg(m,0xF91Cu)));
            break;
        }
        case 0x56u: result=app17_apply_09790(b,0xA5u,slot,0x38,0xFFFFFFFFu,0xFFFFFFFFu,0xFFFFFFFFu,0xFFFFFFFFu); break;
        case 0x57u: result=app17_apply_0a254(b,0xFFFFFFFFu,0x57u,3,0xFFFFFFFFu,0x10,slot,1u,0u); break;
        case 0x58u: {
            const s32 cls=static_cast<s32>(static_cast<s8>(*dg(m,0x218F5u+slot*0x248u)));
            function_0b518(m,b,slot,cls==1?1u:0u);
            result=0xFFFFFFFEu;
            break;
        }
        case 0x5Au: {
            const u32 r=(b.call_22e40?b.call_22e40(slot,3u):0u)+1u;
            result=app17_apply_0a254(b,0x800u,0x0Bu,3,r,2,slot,0u,0x438u);
            break;
        }
        case 0x5Bu: {
            const s32 r=static_cast<s32>(b.call_22e40?b.call_22e40(slot,3u):0u);
            const u32 duration=static_cast<u32>(r/2+4);
            result=app17_apply_09790(b,0x85u,slot,0x1F,0x80u,7u,duration,0x438u);
            break;
        }
        case 0x5Du: {
            const u32 r=b.call_22e40?b.call_22e40(slot,3u):0u;
            result=app17_apply_0a254(b,0x10000u,0x10u,4,r,0x44,slot,0u,0x438u);
            break;
        }
        case 0x5Eu: result=b.call_0a8a0?b.call_0a8a0(slot):0u; break;
        case 0x5Fu: result=b.call_0aa30?b.call_0aa30(slot):0u; break;
        case 0x64u: result=app17_apply_0a254(b,0xFFFFFFFFu,0x64u,4,0xFFFFFFFFu,0x0F,slot,1u,0u); break;
        case 0x65u: app17_sound(m,b,0x17u); if(b.call_0ad24)b.call_0ad24(-1); break;
        case 0x66u:
            app17_sound(m,b,0x3Bu);
            result=app17_apply_0a254(b,0x4000u,0x0Eu,2,0u,0x42,slot,0u,0u);
            break;
        case 0x67u:
            if(b.call_0a668)b.call_0a668(slot);
            *dg(m,0xCF5Au+slot)=1u;
            break;
        case 0x68u: result=app17_apply_0a254(b,0xFFFFFFFFu,0x68u,4,0xFFFFFFFFu,0x41,slot,1u,0u); break;
        case 0x6Eu: result=app17_apply_0a254(b,0xFFFFFFFFu,0x6Eu,3,0xFFFFFFFFu,0x11,slot,1u,0u); break;
        case 0x6Fu: result=app17_apply_09790(b,0x82u,slot,8,0xFFFFFFFFu,0xFFFFFFFFu,0xFFFFFFFFu,0xFFFFFFFFu); break;
        case 0x70u: {
            const u32 base=slot*0x248u;
            *dg(m,0x21A13u+base)=static_cast<u8>(*dg(m,0x21A13u+base)|0x80u);
            *dg(m,0xCF5Au+slot)=1u;
            app17_sound(m,b,0x22u);
            break;
        }
        case 0x71u: result=app17_apply_0a254(b,0xFFFFFFFFu,0x71u,4,0xFFFFFFFFu,0x3C,slot,1u,0u); break;
        case 0x78u: result=app17_apply_0a254(b,0xFFFFFFFFu,0x78u,3,0xFFFFFFFFu,0x0E,slot,1u,0u); break;
        case 0x79u: result=app17_apply_09790(b,0x83u,slot,8,0xFFFFFFFFu,0xFFFFFFFFu,0xFFFFFFFFu,0xFFFFFFFFu); break;
        case 0x7Au: {
            const s32 index=b.call_095c0?b.call_095c0():-1;
            if(index==-1)result=0u;
            else if(b.call_0b410)b.call_0b410(static_cast<u32>(index));
            break;
        }
        case 0x7Bu:
            app17_sound(m,b,0x3Fu);
            result=app17_apply_0a254(b,0x7Bu,0x7Bu,1,0u,0x42,slot,1u,0u);
            break;
        case 0x82u: result=app17_apply_0a254(b,0xFFFFFFFFu,0x82u,3,0xFFFFFFFFu,0x26,slot,1u,0u); break;
        case 0x83u: result=app17_apply_09790(b,0x84u,slot,0x23,0xFFFFFFFFu,0xFFFFFFFFu,0xFFFFFFFFu,0xFFFFFFFFu); break;
        default: break;
        }
    }

    if(b.call_19cf4)b.call_19cf4();
    *dg(m,0xBF38u)=1u;
    return result;
}
static_assert(0x0C6D7u-0x0B950u==3463u);

// -----------------------------------------------------------------------------
// Batch 18: 0C704..0D53D party rotation, checks, damage/level helpers, and
// transition/render support. Tables at 0C6E0, 0CF70, and 0D098 are data.
// -----------------------------------------------------------------------------

struct Application18Boundary {
    u32 (*call_24478)(u32,u32)=nullptr;
    u32 (*call_23e58)(u32)=nullptr;
    u32 (*call_23f50)(u32)=nullptr;
    u32 (*call_fe44)(u32)=nullptr;
    u32 (*call_1c328)(u32,u32,u32,u32)=nullptr;
    s32 (*call_efa0)(u32)=nullptr;
    void (*call_0871c)(u32,u32)=nullptr;
    void (*call_30cd4)(u32,u32,u32)=nullptr;
    u32 (*call_3104c)(u32)=nullptr;
    void (*call_310b0)(u32)=nullptr;
    void (*call_291b0)(u32)=nullptr;
    u32 (*call_30ca2)()=nullptr;
    void (*call_2a078)(u32,u32,u32,u32,u32,u32,u32)=nullptr;
    void (*call_1f1c8)()=nullptr;
    void (*call_1ea78)()=nullptr;
    void (*call_1f494)(u32)=nullptr;
    void (*call_28168)()=nullptr;
    void (*call_1a9f8)()=nullptr;
    void (*call_2e608)()=nullptr;
    void (*call_2e510)(u32,u32,u32,u32)=nullptr;
    void (*call_1ea04)()=nullptr;
    void (*call_28c54)(u32,u32)=nullptr;
    void (*call_1aacc)()=nullptr;
    void (*call_27f24)(u32)=nullptr;
    void (*call_1a288)()=nullptr;
    void (*call_2e614)(u32,u32)=nullptr;
    u32 (*call_1a1f4)()=nullptr;
    void (*call_205b4)(u32)=nullptr;
    u32 (*call_19cc4)()=nullptr;
    void (*call_0ad24)(s32)=nullptr;
    u32 (*call_14a60)(u32)=nullptr;
    u32 (*call_1c1dc)(u32)=nullptr;
    void (*call_3210f)(u32,u32,u32)=nullptr;
    void (*call_1c21c)(u32)=nullptr;
    void (*call_3c04)(u32,u32)=nullptr;
    void (*call_3a78)(u32,u32)=nullptr;
    void (*divide_error)()=nullptr;
};

static s32 app18_sdiv3(s32 v) noexcept { return v/3; }
static s32 app18_sdiv2(s32 v) noexcept { return v/2; }

void function_0c704(Machine& m,Application18Boundary& b) noexcept {
    const u8 old=*dg(m,0xCF68u);
    for(;;) {
        const u8 next=static_cast<u8>((*dg(m,0xCF68u)+1u)&7u);
        *dg(m,0xCF68u)=next;
        const s32 signed_next=static_cast<s32>(next);
        const s32 party=app18_sdiv2(signed_next);
        const s32 side=signed_next%2;
        const u32 base=static_cast<u32>(party)*0x248u;
        bool stop=false;
        if(load_le16(dg(m,0x218F0u+base))<0x2000u) {
            const u32 ok=b.call_24478?b.call_24478(static_cast<u32>(party),static_cast<u32>(side)):0u;
            if(ok!=0u) {
                const s32 item=static_cast<s16>(load_le16(dg(m,0x2193Cu+base+static_cast<u32>(side)*2u)));
                if(item==-1)stop=true;
                else {
                    const u32 a=b.call_23e58?b.call_23e58(static_cast<u32>(item)):0u;
                    if(a!=0u)stop=true;
                    else {
                        const u32 c=b.call_23f50?b.call_23f50(static_cast<u32>(item)):0u;
                        if(c!=0u)stop=true;
                    }
                }
            }
        }
        if(stop||*dg(m,0xCF68u)==old)break;
    }
    *dg(m,0xCF5Au+static_cast<u32>(*dg(m,0xCF68u))/2u)=1u;
    *dg(m,0xCF5Au+static_cast<u32>(old)/2u)=1u;
}
static_assert(0x0C7C1u-0x0C704u==189u);

[[nodiscard]] s32 function_0c7c4(Machine& m,u32 slot) noexcept {
    const u32 base=slot*0x248u;
    const u32 state=*dg(m,0x21913u+base);
    const u32 value=*dg(m,0x2191Fu+base);
    if(state!=0x12u||value==0u)
        return static_cast<s8>(*dg(m,0x6832u+state));
    if(value<=0x32u)return 1;
    if(value<=0x63u)return 2;
    return 3;
}
static_assert(0x0C80Bu-0x0C7C4u==71u);

[[nodiscard]] u32 function_0c80c(Machine& m,Application18Boundary& b,u32 slot,
                                  u32 flags_ptr,u32 stat_ptr,s32 record_index,
                                  u32 use_state_penalty) noexcept {
    if((*dg(m,flags_ptr+8u)&0x08u)!=0u)return 1u;
    s32 difficulty=0;
    s32 status_count=0;
    if(record_index!=-1) {
        const u32 rec=load_le32(dg(m,0x255D8u))+static_cast<u32>(record_index)*65u;
        difficulty=*dg(m,rec+0x37u);
    }
    if(use_state_penalty!=0u)difficulty-=function_0c7c4(m,slot);
    const u32 kind=b.call_fe44?b.call_fe44(slot):0u;
    if(kind==2u)difficulty+=1;
    if(kind==3u)difficulty+=2;
    if(kind==4u)difficulty+=static_cast<s32>(kind);
    const u32 base=slot*0x248u;
    s32 target=static_cast<s32>(*dg(m,0x219EAu+base))-
               static_cast<s8>(*dg(m,stat_ptr+0x21u))+difficulty;
    if((*dg(m,flags_ptr+8u)&0x80u)!=0u)target-=4;
    if((*dg(m,0x21A13u+base)&0x01u)!=0u)++status_count;
    if((*dg(m,0x21A13u+base)&0x08u)!=0u)++status_count;
    if(static_cast<s16>(load_le16(dg(m,0x21948u+base)))==0x54)status_count+=3;
    const s32 roll=static_cast<s32>(b.call_1c328?b.call_1c328(1u,0x14u,static_cast<u32>(status_count),0u):0u);
    if(roll>=0x14)return 1u;
    if(roll==1)return 0u;
    return roll>=target?1u:0u;
}
static_assert(0x0C90Du-0x0C80Cu==257u);

[[nodiscard]] s32 function_0c910(Machine& m,u32 value,u32 desc_ptr,s32 record_index) noexcept {
    const u32 flags=load_le32(dg(m,desc_ptr+0x31u));
    s32 category=9;
    u32 rec=0u;
    if(record_index!=-1) {
        rec=load_le32(dg(m,0x255D8u))+static_cast<u32>(record_index)*65u;
        category=*dg(m,rec+0x2Fu);
        if(*dg(m,rec+0x37u)!=0u)category=11;
    }
    s32 result=static_cast<s32>(value);
    if(category==9) {
        if((flags&0x00008000u)==0u)result=0;
    } else if(category==10) {
        if((flags&0x00004000u)==0u)result=0;
    } else if(category==11) {
        // MOVZX at C975/C988 makes the encoded signed -2 comparisons
        // asymmetric: bit 17 always clears, while bit 18 cannot clear.
        if((flags&0x00020000u)!=0u)result=0;
    }
    if(result<1)result=1;
    return result;
}
static_assert(0x0C99Fu-0x0C910u==143u);

[[nodiscard]] u32 function_0c9a0(Machine& m,Application18Boundary& b,u32 slot,
                                  u32 stat_ptr,u32 flags_ptr) noexcept {
    const s32 initial=static_cast<s32>(b.call_1c328?b.call_1c328(1u,0x14u,0u,0u):0u);
    if(initial==0x14)return 1u;
    if(initial==1)return 0u;
    const s32 modifier=b.call_efa0?b.call_efa0(slot):0;
    const u32 base=slot*0x248u;
    s32 roll=initial;
    const u32 bits=load_le32(dg(m,0x21A12u+base));
    if((bits&0x40u)!=0u) {
        if(b.call_0871c)b.call_0871c(slot,6u);
        return 0u;
    }
    if((bits&0x20u)!=0u)roll-=2;
    if((bits&0x100u)!=0u)--roll;
    if((bits&0x08u)!=0u)roll-=2;
    s32 target=static_cast<s32>(*dg(m,stat_ptr+0x22u))-modifier;
    if((*dg(m,flags_ptr+8u)&0x80u)!=0u)target+=4;
    return roll>=target?1u:0u;
}
static_assert(0x0CA36u-0x0C9A0u==150u);

void function_0ca38(Machine& m,Application18Boundary& b,u32 slot) noexcept {
    const u32 base=slot*0x248u;
    for(u32 off=0u;off<0x2Au;off+=2u) {
        const s32 item=static_cast<s16>(load_le16(dg(m,0x21920u+base+off)));
        if(item==-1)continue;
        const s32 count=static_cast<s16>(load_le16(dg(m,0x258A4u)));
        if(b.call_30cd4)b.call_30cd4(0x1762Eu,0x17618u,static_cast<u32>(count)*0x16u);
        const u16 y=static_cast<u16>(static_cast<u16>(load_le16(dg(m,0x255D6u)))*
                                     static_cast<u16>(load_le16(dg(m,0x25619u))));
        store_le16(dg(m,0x17626u),static_cast<u16>(item));
        store_le32(dg(m,0x17618u),load_le32(dg(m,0x255D4u)));
        store_le32(dg(m,0x1761Cu),load_le32(dg(m,0x255D0u)));
        store_le16(dg(m,0x1762Cu),static_cast<u16>(load_le16(dg(m,0x1762Cu))&0xFE07u));
        const u16 final_y=static_cast<u16>(y+load_le16(dg(m,0x255D2u)));
        store_le16(dg(m,0x17624u),final_y);
        if(count<0x64)store_le16(dg(m,0x258A4u),static_cast<u16>(count+1));
    }
}
static_assert(0x0CADAu-0x0CA38u==162u);

void function_0cadc(Machine& m,Application18Boundary& b) noexcept {
    const u32 buf=b.call_3104c?b.call_3104c(0xE10u):0u;
    if(b.call_291b0)b.call_291b0(*dg(m,0xC050u));
    for(u32 i=0u;i<400u;++i) {
        const u32 rec=buf+i*9u;
        store_le32(dg(m,rec),0u);
        const u32 r=b.call_30ca2?b.call_30ca2():0u;
        store_le32(dg(m,rec+4u),static_cast<u32>(r*15u)>>15u);
        store_le32(dg(m,rec+4u),load_le32(dg(m,rec+4u))+10u);
        *dg(m,rec+8u)=0u;
    }
    u32 done=0u;
    while(done<400u) {
        for(u32 i=0u;i<400u;++i) {
            const u32 rec=buf+i*9u;
            if(*dg(m,rec+8u)!=0u)continue;
            u32 delay=load_le32(dg(m,rec+4u));
            if(delay!=0u) {
                store_le32(dg(m,rec+4u),delay-1u);
                continue;
            }
            const u32 r=b.call_30ca2?b.call_30ca2():0u;
            store_le32(dg(m,rec+4u),static_cast<u32>(r*5u)>>15u);
            if(b.call_291b0)b.call_291b0(*dg(m,0xC050u));
            const u32 pos=load_le32(dg(m,rec));
            const u32 page=load_le32(dg(m,0xC050u))^1u;
            if(b.call_2a078)b.call_2a078(page,i,pos,i,pos,1u,4u);
            if(b.call_291b0)b.call_291b0(static_cast<u8>(*dg(m,0xC050u)^1u));
            store_le32(dg(m,rec),pos+4u);
            if(load_le32(dg(m,rec))>=0x140u) {
                ++done;
                *dg(m,rec+8u)=1u;
            }
        }
    }
    if(b.call_310b0)b.call_310b0(buf);
}
static_assert(0x0CBC6u-0x0CADCu==234u);

void function_0cbc8(Machine& m,Application18Boundary& b) noexcept {
    static constexpr u32 offsets[9]={0u,1u,0u,1u,0u,2u,3u,4u,5u};
    if(b.call_1f1c8)b.call_1f1c8();
    if(b.call_1ea78)b.call_1ea78();
    if(b.call_1f494)b.call_1f494(1u);
    if(b.call_28168)b.call_28168();
    if(b.call_1a9f8)b.call_1a9f8();
    if(b.call_291b0)b.call_291b0(*dg(m,0xC050u));
    if(b.call_1f494)b.call_1f494(0u);
    for(u32 i=0u;i<9u;) {
        if(b.call_2e608)b.call_2e608();
        const u32 art=load_le32(dg(m,0x258DCu))+0x5Du+offsets[i];
        if(b.call_2e510)b.call_2e510(4u,0x20u,art,0u);
        if(b.call_1ea04)b.call_1ea04();
        if(i==0u&&b.call_28c54)b.call_28c54(0x11u,1u);
        if(b.call_1ea78)b.call_1ea78();
        if(i==6u) {
            if(b.call_291b0)b.call_291b0(*dg(m,0xC050u));
            const u32 art2=load_le32(dg(m,0x258DCu))+0x5Du+offsets[7];
            if(b.call_2e510)b.call_2e510(4u,0x20u,art2,0u);
            if(b.call_1ea04)b.call_1ea04();
            if(b.call_1aacc)b.call_1aacc();
            if(b.call_27f24)b.call_27f24(load_le32(dg(m,0x258D0u))+1u);
            ++i;
            function_0cadc(m,b);
            if(b.call_1a288)b.call_1a288();
        } else if(b.call_2e614)b.call_2e614(0x12u,0u);
        ++i;
    }
    if(b.call_1a1f4)while(b.call_1a1f4()!=0u){}
    if(b.call_1a1f4)while(b.call_1a1f4()==0u){}
    if(b.call_205b4)b.call_205b4(0x0Cu);
}
static_assert(0x0CCF1u-0x0CBC8u==297u);

void function_0ccf4(Machine& m,Application18Boundary& b) noexcept {
    constexpr u32 bases[4]={0x218F0u,0x21B38u,0x21D80u,0x21FC8u};
    for(u32 addr:bases)
        if((load_le16(dg(m,addr))&0xD040u)==0u)return;
    // Encoded control flow is a backward JMP into 0CBC8's entry.
    function_0cbc8(m,b);
}
static_assert(0x0CD41u-0x0CCF4u==77u);

void function_0cd44(Machine& m,Application18Boundary& b,u32 slot,u32 amount) noexcept {
    u32 base=slot*0x248u;
    store_le32(dg(m,0x21A92u+base),amount);
    const u32 now=b.call_19cc4?b.call_19cc4():0u;
    store_le32(dg(m,0x21A86u+base),now+0x36u);
    u32 remaining=amount;
    if(load_le32(dg(m,0x219F7u+base))!=0u) {
        const u32 v=load_le32(dg(m,0x219F7u+base))-remaining;
        store_le32(dg(m,0x219F7u+base),v);
        if(static_cast<s32>(v)<=0) {
            store_le32(dg(m,0x219F7u+base),0u);
            *dg(m,0x21A12u+base)=static_cast<u8>(*dg(m,0x21A12u+base)&0xFEu);
        }
    }
    if(load_le32(dg(m,0x21ACAu+base))!=0u) {
        const u32 v=load_le32(dg(m,0x21ACAu+base));
        if(static_cast<s32>(remaining)>=static_cast<s32>(v)) { remaining-=v; store_le32(dg(m,0x21ACAu+base),0u); }
        else { store_le32(dg(m,0x21ACAu+base),v-remaining); remaining=0u; }
    }
    if(load_le32(dg(m,0x219F3u+base))!=0u) {
        const u32 v=load_le32(dg(m,0x219F3u+base));
        if(static_cast<s32>(remaining)>=static_cast<s32>(v)) { remaining-=v; store_le32(dg(m,0x219F3u+base),0u); }
        else { store_le32(dg(m,0x219F3u+base),v-remaining); remaining=0u; }
    }
    const u16 hp=static_cast<u16>(load_le16(dg(m,0x218FCu+base))-static_cast<u16>(remaining));
    store_le16(dg(m,0x218FCu+base),hp);
    *dg(m,0xCF5Au+slot)=1u;
    if(static_cast<s16>(hp)<=0) {
        store_le16(dg(m,0x218FCu+base),0u);
        *dg(m,0x218F1u+base)=static_cast<u8>((*dg(m,0x218F1u+base)|0x40u)&0xE3u);
        if(b.call_0ad24)b.call_0ad24(static_cast<s32>(slot));
        const s32 pair=app18_sdiv2(static_cast<s32>(slot));
        if(*dg(m,0xD681u+static_cast<u32>(pair))!=0u&&
           load_le32(dg(m,0xD8A8u+static_cast<u32>(pair)*4u))==slot) {
            *dg(m,0xCF5Au+static_cast<u32>(pair)*2u)=1u;
            *dg(m,0xD681u+static_cast<u32>(pair))=0u;
            *dg(m,0xCF5Bu+static_cast<u32>(pair)*2u)=1u;
        }
        const u32 chosen=b.call_14a60?b.call_14a60(0x578u):0u;
        if(chosen==slot) {
            const u32 found=b.call_1c1dc?b.call_1c1dc(0x46u):0u;
            if(found==0u)store_le32(dg(m,0xBF44u),0x6Au);
        }
    }
    function_0ccf4(m,b);
}
static_assert(0x0CEE4u-0x0CD44u==416u);

[[nodiscard]] u32 function_0cee4(Machine& m,Application18Boundary& b,u32 slot) noexcept {
    const u32 base=slot*0x248u;
    const s32 cls=static_cast<s8>(*dg(m,0x218F5u+base));
    const s32 level=static_cast<s16>(load_le16(dg(m,0x21900u+base)));
    const u32 off=static_cast<u32>(cls*0x5C+level*4);
    const s32 target=static_cast<s32>(load_le32(dg(m,0x6888u+off)));
    const s32 roll=static_cast<s32>(b.call_1c328?b.call_1c328(1u,0x14u,0u,0u):0u);
    return roll>=target?1u:0u;
}
static_assert(0x0CF2Au-0x0CEE4u==70u);

[[nodiscard]] u32 function_0cf2c(Machine& m,Application18Boundary& b,u32 slot,s32 bonus) noexcept {
    const u32 base=slot*0x248u;
    const s32 cls=static_cast<s8>(*dg(m,0x218F5u+base));
    const s32 level=static_cast<s16>(load_le16(dg(m,0x21900u+base)));
    const u32 off=static_cast<u32>(cls*0x5C+level*4);
    const s32 target=static_cast<s32>(load_le32(dg(m,0x6AB0u+off))+static_cast<u32>(bonus));
    const s32 roll=static_cast<s32>(b.call_1c328?b.call_1c328(1u,0x14u,0u,0u):0u);
    return roll>=target?1u:0u;
}
static_assert(0x0CF6Du-0x0CF2Cu==65u);

[[nodiscard]] s32 function_0cf88(Machine& m,Application18Boundary& b,u32 slot) noexcept {
    const u32 base=slot*0x248u;
    s32 sum=0;
    s32 count=0;
    for(;count<3;++count) {
        const s32 level=static_cast<s16>(load_le16(dg(m,0x21900u+base+static_cast<u32>(count)*2u)));
        const s32 cls=static_cast<s8>(*dg(m,0x218F5u+base+static_cast<u32>(count)));
        if(cls==-1)break;
        const s32 base_mod=static_cast<s8>(*dg(m,0x6821u+static_cast<u32>(cls)));
        const s32 q=app18_sdiv3(cls);
        s32 mod=static_cast<s8>(*dg(m,0x67FAu+*dg(m,0x21917u+base)+static_cast<u32>(q*0x13)));
        for(s32 n=1;n<=level;++n) {
            const s32 before=base_mod+mod;
            const s32 after10=mod+3;
            const s32 add=n<10?before:after10;
            sum+=add;
            mod+=2;
        }
    }
    if(count==0) {
        if(b.divide_error)b.divide_error();
        return 0;
    }
    return sum/count;
}
static_assert(0x0D097u-0x0CF88u==271u);

void function_0d0b0(Machine& m,Application18Boundary& b,u32 slot,s32 cls,s32 old_level) noexcept {
    const s32 cap=function_0cf88(m,b,slot);
    const s32 max_roll=static_cast<s8>(*dg(m,0x6821u+static_cast<u32>(cls)));
    const s32 roll=static_cast<s32>(b.call_1c328?b.call_1c328(1u,static_cast<u32>(max_roll),0u,0u):0u);
    const u32 base=slot*0x248u;
    const s32 q=app18_sdiv3(cls);
    const s32 mod=static_cast<s8>(*dg(m,0x67FAu+*dg(m,0x21917u+base)+static_cast<u32>(q*0x13)));
    s32 loss=0;
    switch(static_cast<u32>(cls)) {
        case 0u: case 1u: case 2u: loss=old_level<10?roll+mod:mod+3; break;
        case 3u: loss=old_level<10?roll+mod:mod+2; break;
        case 4u: loss=old_level<11?roll+mod:mod+1; break;
        case 5u: loss=old_level<11?roll+mod:mod+2; break;
        default: loss=0; break;
    }
    if(loss<1)loss=1;
    const u16 old_max=load_le16(dg(m,0x218FEu+base));
    u16 new_max=static_cast<u16>(old_max-static_cast<u16>(loss));
    if(static_cast<s16>(new_max)<1)new_max=1u;
    if(static_cast<s16>(new_max)>cap)new_max=static_cast<u16>(cap);
    store_le16(dg(m,0x218FEu+base),new_max);
    if(static_cast<s16>(load_le16(dg(m,0x218FCu+base)))>static_cast<s16>(new_max))
        store_le16(dg(m,0x218FCu+base),new_max);
}
static_assert(0x0D1DFu-0x0D0B0u==303u);

void function_0d1e0(Machine& m,Application18Boundary& b,u32 slot) noexcept {
    const u32 base=slot*0x248u;
    s32 track=0;
    const s32 cls1=static_cast<s8>(*dg(m,0x218F6u+base));
    if(cls1!=-1) {
        const s32 cls2=static_cast<s8>(*dg(m,0x218F7u+base));
        if(cls2==-1) {
            const u32 r=b.call_30ca2?b.call_30ca2():0u;
            track=static_cast<s32>((static_cast<u32>(r*2u))>>15u);
            const s16 l1=static_cast<s16>(load_le16(dg(m,0x21902u+base)));
            const s16 l0=static_cast<s16>(load_le16(dg(m,0x21900u+base)));
            if(l1>l0)track=1;
            if(l0>l1)track=0;
        } else {
            const u32 r=b.call_30ca2?b.call_30ca2():0u;
            track=static_cast<s32>((static_cast<u32>(r*3u))>>15u);
            const s16 l2=static_cast<s16>(load_le16(dg(m,0x21904u+base)));
            const s16 l0=static_cast<s16>(load_le16(dg(m,0x21900u+base)));
            const s16 l1=static_cast<s16>(load_le16(dg(m,0x21902u+base)));
            if(l2>l0&&l2>l1)track=2;
            if(l1>l0&&l1>l2)track=1;
            if(l0>l1&&l0>l2)track=0;
        }
    }
    const u32 level_addr=0x21900u+base+static_cast<u32>(track)*2u;
    const s32 old_level=static_cast<s16>(load_le16(dg(m,level_addr)));
    if(old_level<=1) {
        if(old_level==1) {
            store_le16(dg(m,0x218FCu+base),0u);
            store_le16(dg(m,level_addr),0u);
            *dg(m,0x218F1u+base)=static_cast<u8>((*dg(m,0x218F1u+base)|0x40u)&0xE3u);
            const s32 pair=app18_sdiv2(static_cast<s32>(slot));
            if(*dg(m,0xD681u+static_cast<u32>(pair))!=0u&&
               load_le32(dg(m,0xD8A8u+static_cast<u32>(pair)*4u))==slot) {
                *dg(m,0xCF5Au+static_cast<u32>(pair)*2u)=1u;
                *dg(m,0xD681u+static_cast<u32>(pair))=0u;
                *dg(m,0xCF5Bu+static_cast<u32>(pair)*2u)=1u;
            }
        }
        return;
    }
    store_le16(dg(m,level_addr),static_cast<u16>(old_level-1));
    const s32 cls=static_cast<s8>(*dg(m,0x218F5u+base+static_cast<u32>(track)));
    const s32 level=static_cast<s16>(load_le16(dg(m,level_addr)));
    const u32 table=static_cast<u32>(cls*40+level*2);
    const s32 lo=load_le16(dg(m,0x7D9Au+table));
    const s32 hi=load_le16(dg(m,0x7D9Cu+table));
    const s32 midpoint=lo*1000+((hi-lo)*1000)/2;
    store_le32(dg(m,0x21906u+base+static_cast<u32>(track)*4u),static_cast<u32>(midpoint));

    const s32 c0=static_cast<s8>(*dg(m,0x218F5u+base));
    const s32 c1=static_cast<s8>(*dg(m,0x218F6u+base));
    const s32 c2=static_cast<s8>(*dg(m,0x218F7u+base));
    const u32 l0=static_cast<u32>(static_cast<s16>(load_le16(dg(m,0x21900u+base))));
    const u32 l1=static_cast<u32>(static_cast<s16>(load_le16(dg(m,0x21902u+base))));
    const u32 l2=static_cast<u32>(static_cast<s16>(load_le16(dg(m,0x21904u+base))));
    if(c0==1&&b.call_3c04)b.call_3c04(slot,l0);
    if(c0==4&&b.call_3a78)b.call_3a78(slot,l0);
    if(c1==4&&b.call_3a78)b.call_3a78(slot,l1);
    if(c2==4&&b.call_3a78)b.call_3a78(slot,l2);
    if(c0==3&&b.call_3c04)b.call_3c04(slot,l0);
    if(c1==3&&b.call_3c04)b.call_3c04(slot,l1);
    if(c2==3&&b.call_3c04)b.call_3c04(slot,l2);

    *dg(m,0xCF5Au+slot)=1u;
    const s32 pair=app18_sdiv2(static_cast<s32>(slot));
    if(*dg(m,0xD681u+static_cast<u32>(pair))!=0u&&
       load_le32(dg(m,0xD8A8u+static_cast<u32>(pair)*4u))==slot)
        *dg(m,0xD681u+static_cast<u32>(pair))=2u;
    if(b.call_3210f)b.call_3210f(0x759Cu,load_le32(dg(m,0x67DCu)),0x218E0u+base);
    if(b.call_1c21c)b.call_1c21c(0x759Cu);
    function_0d0b0(m,b,slot,cls,old_level);
}
static_assert(0x0D4E8u-0x0D1E0u==776u);

[[nodiscard]] u32 function_0d4e8(Machine& m,u32 slot,u32 desc_ptr) noexcept {
    const u32 base=slot*0x248u;
    if(load_le16(dg(m,0x218F0u+base))>=0x4000u)return 1u;
    if((*dg(m,0x21A12u+base)&0x20u)!=0u)return 1u;
    if((*dg(m,0x21A13u+base)&0x02u)!=0u&&
       *dg(m,desc_ptr+0x1Du)<=4u&&(*dg(m,desc_ptr+0x3Au)&0x20u)!=0u)return 1u;
    return 0u;
}
static_assert(0x0D53Eu-0x0D4E8u==86u);

// -----------------------------------------------------------------------------
// Batch 19: 0D550..0E577 record/effect dispatch, party progression helpers,
// map-cell cleanup/spawn support, and world-record death/cleanup handling.
// CS tables at 0DC64 and 0E008 are data in RESIDUAL_LEDGER.md.
// -----------------------------------------------------------------------------

struct Application19Boundary {
    u32 (*call_cee4)(u32)=nullptr;
    void (*call_cd44)(u32,u32)=nullptr;
    void (*call_28110)(u32,u32,u32,u32)=nullptr;
    void (*call_08a80)(u32,s32,s32,u32,u32)=nullptr;
    u32 (*call_30700)()=nullptr;
    void (*call_30cc4)(u32)=nullptr;
    u32 (*call_d4e8)(u32,u32)=nullptr;
    u32 (*call_14a60)(u32)=nullptr;
    u32 (*call_c9a0)(u32,u32,u32)=nullptr;
    u32 (*call_1c328)(u32,u32,u32,u32)=nullptr;
    void (*call_d1e0)(u32)=nullptr;
    u32 (*call_1202c)(u32,u32)=nullptr;
    u32 (*call_19cc4)()=nullptr;
    u32 (*call_f2d4)(u32,u32)=nullptr;
    void (*call_cbc8)()=nullptr;
    void (*call_3210f)(u32,u32,u32)=nullptr;
    void (*call_1c21c)(u32)=nullptr;
    s32 (*call_50dc)(u32,u32)=nullptr;
    void (*call_3ea4)(u32,u32)=nullptr;
    void (*call_3de4)(u32,u32)=nullptr;
    void (*call_1c124)(u32)=nullptr;
    void (*call_290d0)(u32)=nullptr;
    u32 (*call_30ca2)()=nullptr;
    u32 (*call_2acdc)(u32)=nullptr;
    u32 (*call_1c1dc)(u32)=nullptr;
    void (*call_1f1c8)()=nullptr;
    void (*call_1ea78)()=nullptr;
    void (*call_16524)(u32)=nullptr;
    void (*call_2195c)(u32,u32,u32,u32,u32,u32,u32)=nullptr;
    void (*call_002a8)(s32)=nullptr;
    void (*call_28dcc)(u32)=nullptr;
    void (*call_28ef0)()=nullptr;
    void (*call_28e58)(u32,u32,u32)=nullptr;
    void (*divide_error)()=nullptr;
};

static u32 app19_abs32(u32 value) noexcept {
    const u32 sign=(value&0x80000000u)!=0u?0xFFFFFFFFu:0u;
    return (value^sign)-sign;
}
static u32 app19_dir_extract(u16 flags) noexcept {
    u32 eax=flags;
    eax<<=10u;
    u16 ax=static_cast<u16>(eax);
    ax=static_cast<u16>(ax>>13u);
    return static_cast<u32>(ax);
}
static s32 app19_sar3(s32 value) noexcept {
    if(value>=0)return value/8;
    const u32 magnitude=static_cast<u32>(-(value+1))+1u;
    return -static_cast<s32>((magnitude+7u)/8u);
}
static void app19_sound(Application19Boundary& b,u32 sound) noexcept {
    if(b.call_28110)b.call_28110(sound,0xFFu,0u,1u);
}

void function_0e284(Machine& m,Application19Boundary& b,u32 record,u32 descriptor,
                     u32 world_index,u32 amount,u32 mode) noexcept;

void function_0d550(Machine& m,Application19Boundary& b,u32 record,u32 descriptor,
                     u32 world_index) noexcept {
    const u32 type=load_le16(dg(m,record+0x0Eu));
    if(type==0x0Fu) {
        for(u32 slot=0,base=0;slot<4u;++slot,base+=0x248u) {
            if(load_le16(dg(m,0x218F0u+base))>=0x4000u)continue;
            if((b.call_cee4?b.call_cee4(slot):0u)!=0u)continue;
            const u32 value=static_cast<u32>(static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x218FCu+base)))))+
                            load_le32(dg(m,0x21ACAu+base))+load_le32(dg(m,0x219F3u+base));
            if(b.call_cd44)b.call_cd44(slot,value);
        }
        app19_sound(b,load_le32(dg(m,0x258C0u))+load_le16(dg(m,descriptor+0x3Fu)));
        return;
    }

    if(type==8u) {
        const u16 flags=load_le16(dg(m,record+0x14u));
        if((flags&0xC000u)==0x8000u) {
            const u32 dir=app19_dir_extract(flags);
            s32 angle=static_cast<s32>((8u-dir)*45u);
            if(angle>=360)angle-=360;
            if(b.call_08a80)b.call_08a80(0u,angle,-1,load_le32(dg(m,record)),load_le32(dg(m,record+4u)));
            return;
        }
    }

    const u32 dy=app19_abs32(load_le32(dg(m,0x255D0u))-load_le32(dg(m,record+4u)));
    const u32 dx=app19_abs32(load_le32(dg(m,0x255D4u))-load_le32(dg(m,record)));
    if(static_cast<s32>(dy)>0x15000||static_cast<s32>(dx)>0x15000)return;

    const u32 dir=app19_dir_extract(load_le16(dg(m,record+0x14u)));
    const s32 view=static_cast<s32>(load_le32(dg(m,0xC658u)))/45;
    u32 slot=static_cast<u32>(view+static_cast<s32>(dir))&7u;
    slot=*dg(m,0x687Eu+slot);
    const u32 token=b.call_30700?b.call_30700():0u;
    if(b.call_30cc4)b.call_30cc4(token);
    const u32 selector=slot-4u;
    if(selector<=3u) {
        const u32 r=b.call_30ca2?b.call_30ca2():0u;
        slot=(r*2u)>>15u;
        if(selector==0u||selector==2u)slot+=1u;
    }

    const u32 original_slot=slot;
    if(*dg(m,0x258A8u)!=0xFFu) {
        for(;;) {
            if((b.call_d4e8?b.call_d4e8(slot,descriptor):0u)==0u)break;
            slot=static_cast<u32>((static_cast<s32>(slot)+3)%4);
            if(slot==original_slot) {
                app19_sound(b,load_le32(dg(m,0x258BCu))+9u);
                return;
            }
        }
    } else {
        if(load_le16(dg(m,0x218F0u))>=0x4000u&&load_le16(dg(m,0x21B38u))>=0x4000u&&
           static_cast<s32>(slot)<2)slot+=2u;
        if((b.call_d4e8?b.call_d4e8(slot,descriptor):0u)!=0u) {
            slot^=1u;
            if((b.call_d4e8?b.call_d4e8(slot,descriptor):0u)!=0u) {
                app19_sound(b,load_le32(dg(m,0x258BCu))+9u);
                return;
            }
        }
    }

    const u32 selected=b.call_14a60?b.call_14a60(0x584u):0u;
    const bool selected_matches=selected==slot;
    if((b.call_c9a0?b.call_c9a0(slot,descriptor,record):0u)==0u) {
        const u32 desc=load_le32(dg(m,0x22230u))+type*0x45u;
        if(b.call_3210f)b.call_3210f(0x759Cu,load_le32(dg(m,0x67E0u)),desc);
        if(b.call_1c21c)b.call_1c21c(0x759Cu);
        app19_sound(b,load_le32(dg(m,0x258BCu))+9u);
        return;
    }

    const u32 roll=b.call_1c328?b.call_1c328(*dg(m,descriptor+0x1Eu),*dg(m,descriptor+0x1Fu),
                                               *dg(m,descriptor+0x20u),0u):0u;
    bool force_cleanup=false;
    const u32 actor=slot*0x248u;
    if((*dg(m,descriptor+0x3Au)&0x20u)!=0u&&(*dg(m,0x21A13u+actor)&0x40u)!=0u)force_cleanup=true;

    if(!selected_matches) {
        if((*dg(m,descriptor+0x3Au)&0x02u)!=0u) {
            if((*dg(m,0x21A13u+actor)&0x40u)!=0u)force_cleanup=true;
            else if((b.call_cee4?b.call_cee4(slot):0u)==0u&&b.call_d1e0)b.call_d1e0(slot);
        }
        if((*dg(m,descriptor+0x3Au)&0x04u)!=0u) {
            bool apply=true;
            if(type==8u&&((load_le16(dg(m,record+0x14u))&0xC000u)!=0x4000u))apply=false;
            if(apply) {
                if((*dg(m,0x21A13u+actor)&0x40u)!=0u)force_cleanup=true;
                else if((b.call_cee4?b.call_cee4(slot):0u)==0u) {
                    if(b.call_1202c)(void)b.call_1202c(slot,0xFFFFFFFFu);
                    const u32 r=b.call_1c328?b.call_1c328(2u,4u,0u,0u):0u;
                    const u32 delay=r*672u;
                    const u32 now=b.call_19cc4?b.call_19cc4():0u;
                    store_le32(dg(m,0x21ACEu+actor),now+delay);
                }
            }
        }
    }

    if((*dg(m,descriptor+0x39u)&0x20u)!=0u) {
        bool eligible=true;
        if(type==0x11u&&*dg(m,0x218F4u+actor)==5u)eligible=false;
        if(static_cast<s16>(load_le16(dg(m,0x2193Cu+actor)))==0x86||
           static_cast<s16>(load_le16(dg(m,0x2193Eu+actor)))==0x86)eligible=false;
        if((b.call_f2d4?b.call_f2d4(slot,0x45u):0u)!=0u)eligible=false;
        if((b.call_cee4?b.call_cee4(slot):0u)!=0u)eligible=false;
        if(eligible) {
            *dg(m,0x218F1u+actor)=static_cast<u8>(*dg(m,0x218F1u+actor)|0x10u);
            const u32 pair=slot/2u;
            if(*dg(m,0xD681u+pair)!=0u&&slot==load_le32(dg(m,0xD8A8u+pair*4u))) {
                *dg(m,0xCF5Au+pair*2u)=1u;
                *dg(m,0xD681u+pair)=0u;
                *dg(m,0xCF5Bu+pair*2u)=1u;
            }
        }
    }
    if((*dg(m,descriptor+0x39u)&0x01u)!=0u&&(b.call_cee4?b.call_cee4(slot):0u)==0u)
        *dg(m,0x218F1u+actor)=static_cast<u8>(*dg(m,0x218F1u+actor)|0x08u);
    if((*dg(m,descriptor+0x3Au)&0x10u)!=0u&&static_cast<s8>(*dg(m,0x218F5u+actor))!=1&&
       (b.call_cee4?b.call_cee4(slot):0u)==0u)
        *dg(m,0x218F1u+actor)=static_cast<u8>(*dg(m,0x218F1u+actor)|0x04u);

    if(!selected_matches||static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x218FCu+actor))))-
                         static_cast<s32>(roll)>10) {
        if(b.call_cd44)b.call_cd44(slot,roll);
    }

    if(force_cleanup) {
        const u32 random=b.call_1c328?b.call_1c328(2u,6u,0u,0u):0u;
        function_0e284(m,b,record,descriptor,world_index,random,0u);
        store_le32(dg(m,0x21A4Eu+actor),0u);
        *dg(m,0x21A13u+actor)=static_cast<u8>(*dg(m,0x21A13u+actor)&0xBFu);
    }

    *dg(m,0xCF5Au+slot)=1u;
    app19_sound(b,load_le32(dg(m,0x258C0u))+load_le16(dg(m,descriptor+0x3Fu)));
    const u16 alive_mask=0xD040u;
    if((load_le16(dg(m,0x218F0u))&alive_mask)!=0u&&
       (load_le16(dg(m,0x21B38u))&alive_mask)!=0u&&
       (load_le16(dg(m,0x21D80u))&alive_mask)!=0u&&
       (load_le16(dg(m,0x21FC8u))&alive_mask)!=0u) {
        if(b.call_cbc8)b.call_cbc8();
    }
}
static_assert(0x0DACCu-0x0D550u==1404u);

[[nodiscard]] u32 function_0dacc(Machine& m,u32 mode) noexcept {
    const u32 bank=load_le32(dg(m,0xC050u))^1u;
    const s32 count=static_cast<s32>(load_le16(dg(m,0xF038u+bank*2u)));
    for(s32 rank=count-1;rank>=0;--rank) {
        const u32 base=0x27148u+bank*0x2260u;
        s32 i=0;
        for(;i<count;++i) {
            if(load_le16(dg(m,0x27152u+bank*0x2260u+static_cast<u32>(i)*0x16u))==static_cast<u16>(rank))break;
        }
        const u32 rec=base+static_cast<u32>(i)*0x16u;
        if(load_le16(dg(m,rec+8u))!=0u)continue;
        const u32 world_index=load_le16(dg(m,rec+6u));
        if((*dg(m,0x14D8Bu+world_index*0x16u)&0x02u)!=0u)continue;
        const u32 value=load_le16(dg(m,rec));
        if(mode==0u&&value>0x80u)continue;
        if(mode==1u&&value<=0x80u)continue;
        const s32 scaled=app19_sar3(static_cast<s32>(load_le16(dg(m,rec+2u)))-0x7D00);
        if(scaled<0x3B)continue;
        if((*dg(m,0x14D8Bu+world_index*0x16u)&0x10u)!=0u)return world_index;
    }
    return 0xFFFFFFFFu;
}
static_assert(0x0DB94u-0x0DACCu==200u);

[[nodiscard]] u32 function_0db94(Machine& m,u32 flags) noexcept {
    const u32 first=(flags&1u)!=0u?1u:0u;
    u32 result=function_0dacc(m,first);
    if(result==0xFFFFFFFFu)result=function_0dacc(m,first^1u);
    return result;
}
static_assert(0x0DBD3u-0x0DB94u==63u);

[[nodiscard]] s32 function_0dbd4(Machine& m,Application19Boundary& b,u32 slot) noexcept {
    const u32 base=slot*0x248u;
    s32 tracks=1;
    if(static_cast<s8>(*dg(m,0x218F6u+base))!=-1)tracks=2;
    if(static_cast<s8>(*dg(m,0x218F7u+base))!=-1)++tracks;
    s32 best=0;
    u32 best_index=m.cpu.edi;
    for(s32 i=0;i<tracks;++i) {
        const s32 level=static_cast<s16>(load_le16(dg(m,0x21900u+base+static_cast<u32>(i)*2u)));
        if(level>best) { best=level; best_index=static_cast<u32>(i); }
    }
    const s32 cls=static_cast<s8>(*dg(m,0x218F5u+base+best_index));
    const u32 divisor=*dg(m,static_cast<u32>(0x6828+cls*2));
    const u32 multiplier=*dg(m,static_cast<u32>(0x6827+cls*2));
    if(divisor==0u) { if(b.divide_error)b.divide_error(); return 0; }
    const s32 q=best/static_cast<s32>(divisor);
    return 20-q*static_cast<s32>(multiplier);
}
static_assert(0x0DC64u-0x0DBD4u==144u);

void function_0dc7c(Machine& m,Application19Boundary& b,u32 slot,u32 track,u32 level) noexcept {
    const u32 base=slot*0x248u;
    const s32 cls=static_cast<s8>(*dg(m,0x218F5u+base+track));
    const s32 random_arg=static_cast<s8>(*dg(m,static_cast<u32>(0x6821+cls)));
    const s32 random=static_cast<s32>(b.call_1c328?b.call_1c328(1u,static_cast<u32>(random_arg),0u,0u):0u);
    s32 random_min=random<1?1:random;
    const s32 group=cls/3;
    const s32 state=*dg(m,0x21917u+base);
    const s32 table=static_cast<s8>(*dg(m,static_cast<u32>(0x67FA+group*0x13+state)));
    s32 gain=static_cast<s32>(m.cpu.esi);
    const s32 sum=random_min+table;
    switch(cls) {
    case 0: case 1: case 2: gain=static_cast<s32>(level)<10?sum:table+3; break;
    case 3: gain=static_cast<s32>(level)<10?sum:table+2; break;
    case 4: gain=static_cast<s32>(level)<11?sum:table+1; break;
    case 5: gain=static_cast<s32>(level)<11?sum:table+2; break;
    default: break;
    }
    if(gain<1)gain=1;
    if(static_cast<s8>(*dg(m,0x218F6u+base))!=-1) {
        if(static_cast<s8>(*dg(m,0x218F7u+base))!=-1)gain/=3;
        else gain/=2;
    }
    store_le16(dg(m,0x218FEu+base),static_cast<u16>(load_le16(dg(m,0x218FEu+base))+static_cast<u16>(gain)));
    store_le16(dg(m,0x218FCu+base),static_cast<u16>(load_le16(dg(m,0x218FCu+base))+static_cast<u16>(gain)));
}
static_assert(0x0DDA1u-0x0DC7Cu==293u);

void function_0dda4(Machine& m,Application19Boundary& b,s32 points) noexcept {
    s32 active=0;
    for(u32 base=0;base<0x920u;base+=0x248u)
        if((load_le16(dg(m,0x218F0u+base))&0xF240u)==0u)++active;
    if(active==0) { if(b.divide_error)b.divide_error(); return; }
    const s32 share_num=static_cast<s32>(static_cast<u32>(active/2)+static_cast<u32>(points));
    s32 share=share_num/active;
    if(share==0)share=1;

    for(u32 slot=0;slot<4u;++slot) {
        const u32 base=slot*0x248u;
        if((load_le16(dg(m,0x218F0u+base))&0xF240u)!=0u)continue;
        s32 tracks=1;
        if(static_cast<s8>(*dg(m,0x218F6u+base))!=-1)tracks=2;
        if(static_cast<s8>(*dg(m,0x218F7u+base))!=-1)++tracks;
        u32 best_track=0u;
        for(s32 track=0;track<tracks;++track) {
            const s32 per_num=static_cast<s32>(static_cast<u32>(tracks/2)+static_cast<u32>(share));
            s32 per=per_num/tracks;
            if(per==0)per=1;
            const u32 track_u=static_cast<u32>(track);
            const u32 xp_off=0x21906u+base+track_u*4u;
            store_le32(dg(m,xp_off),load_le32(dg(m,xp_off))+static_cast<u32>(per));
            const s32 cls=static_cast<s8>(*dg(m,0x218F5u+base+track_u));
            u32 new_level=0u;
            const u32 xp_thousands=load_le32(dg(m,xp_off))/1000u;
            for(u32 off=0;off<0x28u;off+=2u) {
                const u32 threshold=load_le16(dg(m,static_cast<u32>(0x7D9C+cls*0x28+static_cast<s32>(off))));
                if(threshold>xp_thousands)break;
                ++new_level;
            }
            const u32 level_off=0x21900u+base+track_u*2u;
            if(static_cast<s16>(load_le16(dg(m,level_off)))!=static_cast<s32>(new_level)) {
                store_le16(dg(m,level_off),static_cast<u16>(new_level));
                const s32 ok=b.call_50dc?b.call_50dc(0u,slot):-1;
                if(ok>=0) {
                    if(cls==1) { if(b.call_3ea4)b.call_3ea4(slot,new_level); }
                    else { if(b.call_3de4)b.call_3de4(slot,new_level); }
                }
                if(b.call_3210f)b.call_3210f(0x759Cu,load_le32(dg(m,0x67E4u)),0x218E0u+base);
                if(b.call_1c21c)b.call_1c21c(0x759Cu);
                function_0dc7c(m,b,slot,static_cast<u32>(track),new_level);
            }
            if(static_cast<s16>(load_le16(dg(m,level_off)))>
               static_cast<s16>(load_le16(dg(m,0x21900u+base+best_track*2u))))
                best_track=static_cast<u32>(track);
        }
        const s32 value=function_0dbd4(m,b,slot);
        *dg(m,0x219EAu+base)=static_cast<u8>(value);
    }
}
static_assert(0x0E008u-0x0DDA4u==612u);

void function_0e028(Machine& m,Application19Boundary& b,u32 record) noexcept {
    const u32 extracted=app19_dir_extract(load_le16(dg(m,record+0x14u)));
    const s32 x=static_cast<s16>(load_le16(dg(m,record+2u)));
    const s32 y=static_cast<s16>(load_le16(dg(m,record+6u)));
    const s32 transformed=static_cast<s32>(8u-extracted+4u);
    const s32 direction=transformed%8;
    if(b.call_1c124)b.call_1c124(0xB0u);
    s32 nx=x,ny=y;
    switch(direction) {
    case 0: nx=x+1; break;
    case 1: nx=x+1; ny=y-1; break;
    case 2: ny=y-1; break;
    case 3: nx=x-1; ny=y-1; break;
    case 4: nx=x-1; break;
    case 5: nx=x-1; ny=y+1; break;
    case 6: ny=y+1; break;
    case 7: nx=x+1; ny=y+1; break;
    default: break;
    }
    const s32 width=static_cast<s16>(load_le16(dg(m,0x25619u)));
    const u32 tile=static_cast<u32>(width)*static_cast<u32>(nx)+static_cast<u32>(ny);
    *dg(m,0x198E0u+tile*2u)=static_cast<u8>(*dg(m,0x198E0u+tile*2u)&0xC0u);
}
static_assert(0x0E0BCu-0x0E028u==148u);

[[nodiscard]] u32 function_0e0bc(Machine& m) noexcept {
    s32 found=0;
    const s32 count=static_cast<s16>(load_le16(dg(m,0x258A2u)));
    for(s32 i=0;i<count;++i) {
        const u32 rec=0x14D80u+static_cast<u32>(i)*0x16u;
        if((*dg(m,rec+0x0Bu)&0x02u)==0u&&load_le16(dg(m,rec+0x0Eu))==0x18u)++found;
    }
    return found==1?0u:1u;
}
static_assert(0x0E0F5u-0x0E0BCu==57u);

[[nodiscard]] u32 function_0e0f8(Machine& m,Application19Boundary& b,u32 world_index) noexcept {
    const u32 seed=(b.call_30700?b.call_30700():0u)&0xFFFFu;
    if(b.call_290d0)b.call_290d0(seed);
    u32 x=0u,y=0u,tile=0u;
    s32 attempt=0;
    for(;attempt<3;++attempt) {
        const s32 width=static_cast<s16>(load_le16(dg(m,0x25619u)))-2;
        const u32 rx=b.call_30ca2?b.call_30ca2():0u;
        x=((rx*static_cast<u32>(width))>>15u)+1u;
        const s32 height=static_cast<s16>(load_le16(dg(m,0x2561Bu)))-2;
        const u32 ry=b.call_30ca2?b.call_30ca2():0u;
        y=((ry*static_cast<u32>(height))>>15u)+1u;
        const s32 full_width=static_cast<s16>(load_le16(dg(m,0x25619u)));
        tile=y*static_cast<u32>(full_width)+x;
        const u8 cell=*dg(m,0x198E0u+tile*2u);
        if((cell&0x40u)!=0u||(cell&0x3Fu)!=0u)continue;
        const u32 dx=app19_abs32(x-static_cast<u32>(static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x255D2u))))));
        if(static_cast<s32>(dx)<15)continue;
        const u32 dy=app19_abs32(y-static_cast<u32>(static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x255D6u))))));
        if(static_cast<s32>(dy)<15)continue;
        s32 i=0;
        const s32 count=static_cast<s16>(load_le16(dg(m,0x258A2u)));
        for(;i<count;++i) {
            const u32 rec=0x14D80u+static_cast<u32>(i)*0x16u;
            if((*dg(m,rec+0x0Bu)&0x02u)==0u&&load_le16(dg(m,rec+0x0Cu))==static_cast<u16>(tile))break;
        }
        if(i>=count)break;
    }
    if(attempt==3)return 0u;

    const u32 rec=0x14D80u+world_index*0x16u;
    store_le16(dg(m,rec+6u),static_cast<u16>(x));
    const u32 type=load_le16(dg(m,rec+0x0Eu));
    const u32 desc=load_le32(dg(m,0x22230u))+type*0x45u;
    store_le16(dg(m,rec),0x8000u);
    store_le16(dg(m,rec+4u),0x8000u);
    store_le16(dg(m,rec+2u),static_cast<u16>(y));
    store_le16(dg(m,rec+0x0Cu),static_cast<u16>(tile));
    const u32 hp=b.call_1c328?b.call_1c328(*dg(m,desc+0x1Du),6u,0u,0u):0u;
    store_le16(dg(m,rec+0x12u),static_cast<u16>(hp));
    store_le16(dg(m,rec+0x14u),static_cast<u16>(load_le16(dg(m,rec+0x14u))&0xFE39u));
    store_le16(dg(m,rec+0x10u),0u);
    store_le32(dg(m,rec+8u),load_le32(dg(m,desc+0x35u)));
    return 1u;
}
static_assert(0x0E282u-0x0E0F8u==394u);

void function_0e284(Machine& m,Application19Boundary& b,u32 record,u32 descriptor,
                     u32 world_index,u32 amount,u32 mode) noexcept {
    if(*dg(m,descriptor+0x1Cu)!=0u)*dg(m,record+0x14u)=static_cast<u8>(*dg(m,record+0x14u)|0x06u);
    store_le16(dg(m,record+0x10u),mode==0u?2u:1u);

    const u32 type=load_le16(dg(m,record+0x0Eu));
    if(type!=0x0Fu) {
        if(type==8u) {
            const u16 flags=load_le16(dg(m,record+0x14u));
            if((flags&0xC000u)!=0xC000u&&static_cast<s16>(load_le16(dg(m,record+0x12u)))>10) {
                store_le16(dg(m,record+0x12u),static_cast<u16>(load_le16(dg(m,record+0x12u))-static_cast<u16>(amount)));
                if(static_cast<s16>(load_le16(dg(m,record+0x12u)))<=0)store_le16(dg(m,record+0x12u),10u);
            }
        } else {
            store_le16(dg(m,record+0x12u),static_cast<u16>(load_le16(dg(m,record+0x12u))-static_cast<u16>(amount)));
        }
    }
    if((b.call_2acdc?b.call_2acdc(type):0u)!=0u) {
        *dg(m,record+0x0Bu)=static_cast<u8>((*dg(m,record+0x0Bu)&0xCFu)|0x10u);
    }

    if(static_cast<s16>(load_le16(dg(m,record+0x12u)))>0) {
        app19_sound(b,load_le32(dg(m,0x258C0u))+load_le16(dg(m,descriptor+0x41u)));
        return;
    }

    if(type==0x18u) {
        const bool blocked=(b.call_1c1dc?b.call_1c1dc(0x3Bu):0u)!=0u||
                           load_le32(dg(m,0xC6C8u))!=0x19u||function_0e0bc(m)!=0u;
        if(!blocked) {
            const u32 hp=b.call_1c328?b.call_1c328(1u,3u,0u,0u):0u;
            store_le16(dg(m,record+0x12u),static_cast<u16>(hp));
            if(b.call_1c124)b.call_1c124(0x3Bu);
            *dg(m,0xBF38u)=0u;
            if(b.call_1f1c8)b.call_1f1c8();
            if(b.call_1ea78)b.call_1ea78();
            *dg(m,0xBF38u)=1u;
            if(b.call_16524)b.call_16524(1u);
            return;
        }
    } else if(type==0x21u) {
        function_0e028(m,b,record);
    } else if(type==0x12u) {
        if(b.call_1c124)b.call_1c124(0x20u);
    } else if(type==9u) {
        if(b.call_2195c) {
            b.call_2195c(0x6Eu,0x88u,0u,load_le32(dg(m,record+4u)),load_le32(dg(m,record)),1u,0u);
            b.call_2195c(0x78u,0x87u,0u,load_le32(dg(m,record+4u)),load_le32(dg(m,record)),1u,0u);
            b.call_2195c(0x82u,0xE9u,0u,load_le32(dg(m,record+4u)),load_le32(dg(m,record)),1u,0u);
            b.call_2195c(0x8Cu,0x74u,0u,load_le32(dg(m,record+4u)),load_le32(dg(m,record)),1u,0u);
        }
        store_le32(dg(m,0xBF44u),1u);
    } else if(type==0x50u) {
        if(b.call_1c124)b.call_1c124(0x43u);
    } else if(type==8u) {
        if(b.call_1c124)b.call_1c124(0x51u);
        if(b.call_2195c)b.call_2195c(0x80u,0x74u,0u,load_le32(dg(m,record+4u)),load_le32(dg(m,record)),1u,0u);
        if(b.call_002a8) { b.call_002a8(0x1F); b.call_002a8(0x20); b.call_002a8(0x21); }
        if(b.call_28dcc)b.call_28dcc(0x9C4u);
        if(b.call_28ef0)b.call_28ef0();
        if(b.call_28e58)b.call_28e58(*dg(m,0x258B4u),1u,0x9C4u);
    }

    const u32 desc=load_le32(dg(m,0x22230u))+type*0x45u;
    const u16 sound_off=load_le16(dg(m,desc+0x43u));
    app19_sound(b,load_le32(dg(m,0x258BCu))+(sound_off!=0u?sound_off:0u));

    u32 i=0u;
    for(;i<3u;++i) {
        const s32 candidate=static_cast<s16>(load_le16(dg(m,0x258AAu+i*2u)));
        if(static_cast<s32>(type)==candidate&&function_0e0f8(m,b,world_index)!=0u)break;
    }
    if(i==3u)*dg(m,record+0x0Bu)=static_cast<u8>(*dg(m,record+0x0Bu)|0x02u);
    function_0dda4(m,b,static_cast<s32>(load_le16(dg(m,descriptor+0x23u))));
}
static_assert(0x0E578u-0x0E284u==756u);

// -----------------------------------------------------------------------------
// Batch 20: 0E578..0F66B status/effect resolution, modal text/UI support,
// equipment/stat helpers, nested item-list membership/removal, and value sums.
// The CS table at 0E5CC and dword data table at 0EEB0 are non-code/data.
// -----------------------------------------------------------------------------

struct Application20Boundary {
    u32 (*call_0db94)(u32)=nullptr;
    u32 (*call_0c80c)(u32,u32,u32,s32,u32)=nullptr;
    u32 (*call_23eac)(u32,u32)=nullptr;
    u32 (*call_1c328)(u32,u32,u32,u32)=nullptr;
    u32 (*call_22e40)(u32,u32)=nullptr;
    void (*call_28110)(u32,u32,u32,u32)=nullptr;
    s32 (*call_0c910)(u32,u32,s32)=nullptr;
    void (*call_0e284)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_3210f)(u32,u32,u32)=nullptr;
    void (*call_1c21c)(u32)=nullptr;
    u32 (*call_19cc4)()=nullptr;
    void (*call_24b48)(u32,u32,u32,u32)=nullptr;

    u32 (*call_3104c)(u32)=nullptr;
    void (*call_30d30)(u32,u32,u32)=nullptr;
    void (*call_310b0)(u32)=nullptr;
    void (*call_1c368)(u32,u32)=nullptr;
    s32 (*call_30288)(u32,u32)=nullptr;
    void (*call_3060c)(u32,u32,u32,u32)=nullptr;
    void (*call_29cec)(u32,u32,u32,u32,u32)=nullptr;

    void (*call_291b0)(u32)=nullptr;
    void (*call_2975c)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_298f0)(u32,u32,u32,u32,u32,u32)=nullptr;
    void (*call_29b40)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_2a020)(u32)=nullptr;
    void (*call_325b0)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_32404)(u32,u32,u32,u32,u32,u32)=nullptr;
    void (*call_29234)(u32)=nullptr;
    void (*call_27f24)(u32)=nullptr;
    void (*call_322ec)(u32,u32,u32,u32,u32,u32)=nullptr;
    u32 (*call_1a1f4)()=nullptr;
    u32 (*call_30b40)()=nullptr;
    void (*call_2e608)()=nullptr;
    void (*call_2a078)(u32,u32,u32,u32,u32,u32,u32)=nullptr;
    void (*call_2e614)(u32,u32)=nullptr;
    void (*call_28dcc)(u32)=nullptr;
    void (*call_28ef0)()=nullptr;
    void (*call_28ce8)()=nullptr;
    void (*call_20294)()=nullptr;
    void (*call_205b4)(u32)=nullptr;

    u32 (*call_24478)(u32,u32)=nullptr;
    u32 (*call_fe44)(u32)=nullptr;
    void (*call_1a344)(s32,u32)=nullptr;
};

static s32 app20_div2_trunc(s32 value) noexcept { return value/2; }
static u32 app20_shl32(u32 value,u32 count) noexcept {
    count&=31u;
    return value<<count;
}
static u8 app20_shl8(u8 value,u32 count) noexcept {
    count&=31u;
    if(count>=8u)return 0u;
    return static_cast<u8>(static_cast<u32>(value)<<count);
}

void function_0e578(Machine& m,u32 slot) noexcept {
    for(u32 i=0u,base=0u;i<4u;++i,base+=0x248u) {
        if((*dg(m,0x21A12u+base)&0x20u)==0u)continue;
        *dg(m,0xCF5Au+i)=1u;
        *dg(m,0x21A12u+base)=static_cast<u8>(*dg(m,0x21A12u+base)&0xDFu);
    }
    const u32 base=slot*0x248u;
    if((*dg(m,0x21A13u+base)&0x02u)!=0u) {
        store_le32(dg(m,0x21A3Au+base),0u);
        *dg(m,0x21A13u+base)=static_cast<u8>(*dg(m,0x21A13u+base)&0xFDu);
        *dg(m,0xCF5Au+slot)=1u;
    }
}
static_assert(0x0E5CCu-0x0E578u==84u);

[[nodiscard]] u32 function_0e5f0(Machine& m,Application20Boundary& b,
                                  u32 slot,u32 timer_index,s32 effect_index,
                                  s32 supplied_world_index) noexcept {
    s32 world_index=supplied_world_index;
    if(world_index==-1)
        world_index=static_cast<s32>(b.call_0db94?b.call_0db94(slot):0xFFFFFFFFu);

    function_0e578(m,slot);

    if(*dg(m,0x258A8u)==0xFFu&&world_index!=-1&&(slot&2u)!=0u) {
        if(effect_index==-1)world_index=-1;
        else {
            const u32 rec=load_le32(dg(m,0x255D8u))+static_cast<u32>(effect_index)*65u;
            const u8 low=static_cast<u8>(load_le16(dg(m,rec+0x35u)));
            if((low&3u)!=2u)world_index=-1;
        }
    }

    u32 result=0u;
    u32 amount=0u;
    if(world_index!=-1) {
        const u32 world=0x14D80u+static_cast<u32>(world_index)*0x16u;
        const u32 type=load_le16(dg(m,world+0x0Eu));
        const u32 desc=load_le32(dg(m,0x22230u))+type*0x45u;
        result=b.call_0c80c?b.call_0c80c(slot,world,desc,effect_index,1u):0u;

        if(result!=0u) {
            const u32 actor=slot*0x248u;
            const u32 state=*dg(m,0x21913u+actor);
            const u32 state_value=*dg(m,0x2191Fu+actor);
            bool use_state_table=true;
            if(state==0x12u) {
                const bool any=(b.call_23eac&&b.call_23eac(slot,1u)!=0u)||
                               (b.call_23eac&&b.call_23eac(slot,5u)!=0u)||
                               (b.call_23eac&&b.call_23eac(slot,0u)!=0u)||
                               (b.call_23eac&&b.call_23eac(slot,2u)!=0u);
                use_state_table=!any;
            }
            s32 state_term=0;
            if(use_state_table)
                state_term=static_cast<s8>(*dg(m,0x684Bu+state));
            else if(state_value<=0x4Bu)state_term=3;
            else if(state_value<=0x5Au)state_term=4;
            else if(state_value<=0x63u)state_term=5;
            else state_term=6;

            s32 sound_offset=-1;
            const u32 eid=static_cast<u32>(effect_index);
            if(effect_index==-1) {
                amount=b.call_1c328?b.call_1c328(1u,2u,0u,0u):0u;
            } else if(eid>=0x7Eu&&eid<=0x86u) {
                switch(eid) {
                case 0x7Eu: amount=b.call_1c328?b.call_1c328(1u,4u,0u,0u):0u; break;
                case 0x7Fu: {
                    const u32 r=b.call_22e40?b.call_22e40(4u,slot):0u;
                    const s32 half=app20_div2_trunc(static_cast<s32>(r));
                    amount=b.call_1c328?b.call_1c328(static_cast<u32>(half+1),6u,0u,0u):0u;
                    break;
                }
                case 0x80u:
                    sound_offset=0x0C;
                    amount=b.call_1c328?b.call_1c328(2u,6u,0u,0u):0u;
                    break;
                case 0x81u: amount=b.call_1c328?b.call_1c328(1u,8u,0u,0u):0u; break;
                case 0x82u:
                    sound_offset=9;
                    amount=b.call_1c328?b.call_1c328(2u,8u,1u,0u):0u;
                    break;
                case 0x83u:
                    sound_offset=9;
                    amount=b.call_1c328?b.call_1c328(3u,8u,3u,0u):0u;
                    break;
                case 0x84u: {
                    sound_offset=0x24;
                    const u32 r=b.call_1c328?b.call_1c328(1u,4u,0u,0u):0u;
                    if(static_cast<s16>(load_le16(dg(m,world+0x12u)))>static_cast<s32>(r))
                        store_le16(dg(m,world+0x12u),static_cast<u16>(r));
                    amount=0u;
                    break;
                }
                case 0x85u: amount=b.call_1c328?b.call_1c328(1u,4u,4u,2u):0u; break;
                case 0x86u: amount=b.call_1c328?b.call_1c328(1u,10u,0u,0u):0u; break;
                default: break;
                }
            } else {
                const u32 effect=load_le32(dg(m,0x255D8u))+eid*65u;
                amount=b.call_1c328?b.call_1c328(*dg(m,effect+0x38u),*dg(m,effect+0x39u),
                                                  *dg(m,effect+0x3Au),static_cast<u32>(state_term)):0u;
            }

            if(sound_offset!=-1&&b.call_28110)
                b.call_28110(load_le32(dg(m,0x258D4u))+static_cast<u32>(sound_offset),0xFFu,0u,0u);

            if(effect_index==0x4A) {
                if(type==0x17u||type==0x1Fu) {
                    if(result==0x14u) {
                        const u32 effect=load_le32(dg(m,0x255D8u))+0x4Au*65u;
                        const s32 counter_index=static_cast<s8>(*dg(m,effect+0x31u));
                        u8* counter=dg(m,0x67ECu+static_cast<u32>(counter_index));
                        if(*counter!=0u) {
                            amount=static_cast<u32>(static_cast<s32>(static_cast<s16>(load_le16(dg(m,world+0x12u)))));
                            --*counter;
                        }
                    } else {
                        amount=b.call_1c328?b.call_1c328(2u,8u,6u,0u):0u;
                    }
                } else if(result==0x14u) {
                    const u32 effect=load_le32(dg(m,0x255D8u))+0x4Au*65u;
                    const s32 counter_index=static_cast<s8>(*dg(m,effect+0x31u));
                    u8* counter=dg(m,0x67ECu+static_cast<u32>(counter_index));
                    if(*counter!=0u) {
                        amount*=3u;
                        --*counter;
                    }
                }
            } else if(b.call_0c910) {
                amount=static_cast<u32>(b.call_0c910(amount,desc,effect_index));
            }

            if(b.call_0e284)b.call_0e284(world,desc,static_cast<u32>(world_index),amount,0u);
            if(effect_index==0x7F)store_le32(dg(m,0x219F3u+actor),amount);
        } else {
            const u32 type2=load_le16(dg(m,world+0x0Eu));
            const u32 desc2=load_le32(dg(m,0x22230u))+type2*0x45u;
            if(b.call_3210f)b.call_3210f(0x759Cu,load_le32(dg(m,0x67E8u)),desc2);
            if(b.call_1c21c)b.call_1c21c(0x759Cu);
            if(b.call_28110)b.call_28110(load_le32(dg(m,0x258BCu))+9u,0xFFu,1u,1u);
        }
    } else {
        if(b.call_28110)b.call_28110(load_le32(dg(m,0x258BCu))+9u,0xFFu,1u,1u);
    }

    s32 duration=0x12;
    const u32 eid=static_cast<u32>(effect_index);
    if(effect_index!=-1&&(eid<0x7Eu||eid>0x86u)) {
        const u32 effect=load_le32(dg(m,0x255D8u))+eid*65u;
        const u16 packed=load_le16(dg(m,effect+0x35u));
        duration=static_cast<s32>(((packed>>2u)&0x0Fu)*9u);
    }
    const u32 actor=slot*0x248u;
    if((load_le32(dg(m,0x21A12u+actor))&0x01000010u)!=0u)
        duration=app20_div2_trunc(duration);
    if(result!=0u) {
        const u32 now=b.call_19cc4?b.call_19cc4():0u;
        store_le32(dg(m,0x21A7Eu+actor+timer_index*4u),
                   now+static_cast<u32>(app20_div2_trunc(duration)));
        store_le32(dg(m,0x21A8Au+actor+timer_index*4u),amount);
    }
    if(b.call_24b48)b.call_24b48(slot,timer_index,0u,static_cast<u32>(duration));
    return result;
}
static_assert(0x0EA82u-0x0E5F0u==1170u);

void function_0ea90(Machine& m,Application20Boundary& b) noexcept {
    for(u32 off=0u;off<=0x40u;off+=4u) {
        const u32 ptr=b.call_3104c?b.call_3104c(0x50u):0u;
        store_le32(dg(m,0x12BB0u+off),ptr);
        if(b.call_30d30)b.call_30d30(ptr,0u,0x50u);
        else for(u32 i=0;i<0x50u;++i)*dg(m,ptr+i)=0u;
    }
}
static_assert(0x0EABBu-0x0EA90u==43u);

void function_0eabc(Machine& m,Application20Boundary& b) noexcept {
    for(u32 off=0u;off<=0x40u;off+=4u)
        if(b.call_310b0)b.call_310b0(load_le32(dg(m,0x12BB0u+off)));
}
static_assert(0x0EAD8u-0x0EABCu==28u);

void function_0ead8(Machine& m,Application20Boundary& b,s32 value) noexcept {
    const s32 remainder=value%0xC0;
    for(u32 off=0u;off<0x40u;off+=4u) {
        u32 src=load_le32(dg(m,0x12BB4u+off));
        u32 dst=load_le32(dg(m,0x12BB0u+off));
        for(;;) {
            const u8 a=*dg(m,src);
            *dg(m,dst)=a;
            if(a==0u)break;
            const u8 c=*dg(m,src+1u);
            src+=2u;
            *dg(m,dst+1u)=c;
            dst+=2u;
            if(c==0u)break;
        }
    }
    if(b.call_1c368)b.call_1c368(load_le32(dg(m,0x12BF0u)),static_cast<u32>(remainder+0x371));
}
static_assert(0x0EB3Bu-0x0EAD8u==99u);

void function_0eb3c(Machine& m,Application20Boundary& b,s32 value,u32 render) noexcept {
    if(value%16==0)function_0ead8(m,b,value/16+16);
    if(render==0u)return;

    s32 seq=value/16;
    s32 y=0x52-(value%16);
    for(u32 off=0u;off<=0x40u;off+=4u) {
        const u32 ptr=load_le32(dg(m,0x12BB0u+off));
        const s32 width=b.call_30288?b.call_30288(ptr,5u):0;
        const s32 half=app20_div2_trunc(width);
        const s32 x=0xA0-half;
        if(b.call_3060c)b.call_3060c(static_cast<u32>(y),static_cast<u32>(x),ptr,5u);
        const s32 row=seq%0xC0;
        if(*dg(m,0x6CD8u+static_cast<u32>(row))!=0u&&b.call_29cec) {
            const u32 line=static_cast<u32>(y+0x0C);
            b.call_29cec(line,static_cast<u32>(x),line,static_cast<u32>(half+0xA0),0x1Bu);
        }
        ++seq;
        y+=0x10;
    }
}
static_assert(0x0EC19u-0x0EB3Cu==221u);

void function_0ec1c(Machine& m,Application20Boundary& b) noexcept {
    auto draw_pair=[&]() noexcept {
        const u32 page=static_cast<u32>(*dg(m,0xC050u));
        if(b.call_291b0)b.call_291b0(page);
        if(b.call_2975c) {
            b.call_2975c(0u,0u,0u,load_le32(dg(m,0x258DCu))+0x65u,0u);
            b.call_2975c(0u,0xC8u,0u,load_le32(dg(m,0x258DCu))+0x66u,0u);
        }
    };

    draw_pair();
    if(b.call_29b40)b.call_29b40(0xECu,0x53u,0xA0u,0x78u,0u);
    if(b.call_2a020)b.call_2a020(load_le32(dg(m,0xC050u))^1u);
    *dg(m,0xC050u)=static_cast<u8>(*dg(m,0xC050u)^1u);
    if(b.call_325b0)b.call_325b0(0xC054u,0u,0x0Du,0x590u,0x100u);
    if(b.call_32404)b.call_32404(0xC054u,0x90u,0u,0u,0u,0x6Fu);
    if(b.call_29234)b.call_29234(0xC054u);
    if(b.call_325b0)b.call_325b0(0xC054u,0u,0x0Du,0x590u,0x100u);
    draw_pair();
    if(b.call_2a020)b.call_2a020(load_le32(dg(m,0xC050u))^1u);
    *dg(m,0xC050u)=static_cast<u8>(*dg(m,0xC050u)^1u);
    if(b.call_27f24)b.call_27f24(load_le32(dg(m,0x258D0u))+0x11u);
    if(b.call_322ec)b.call_322ec(0xC054u,0x90u,0xFFu,0u,0u,0u);

    s32 phase=0xAF0;
    function_0ea90(m,b);
    for(;;) {
        if((b.call_1a1f4?b.call_1a1f4():0u)!=0u)break;
        if((b.call_30b40?b.call_30b40():0u)!=0u)break;
        if(b.call_2e608)b.call_2e608();
        draw_pair();
        function_0eb3c(m,b,phase,1u);
        if(b.call_2a078)b.call_2a078(load_le32(dg(m,0xC050u))^1u,0x52u,0u,0x52u,0u,0xECu,0x140u);
        phase=(phase+1)%0xC00;
        function_0eb3c(m,b,phase,0u);
        phase=(phase+1)%0xC00;
        if(b.call_2e614)b.call_2e614(1u,0u);
    }
    function_0eabc(m,b);
    if(b.call_28dcc)b.call_28dcc(0x9C4u);
    if(b.call_322ec)b.call_322ec(0xC054u,0u,0xFFu,0u,0u,0u);
    if(b.call_28ef0)b.call_28ef0();
    if(b.call_28ce8)b.call_28ce8();
    if(b.call_20294)b.call_20294();
    if(b.call_205b4)b.call_205b4(0u);
}
static_assert(0x0EEA5u-0x0EC1Cu==649u);

[[nodiscard]] u32 function_0eee0(Machine& m,u32 slot) noexcept {
    const u32 base=slot*0x248u;
    const s32 item=static_cast<s16>(load_le16(dg(m,0x21940u+base)));
    if(item==-1)return 0u;
    const u32 rec=load_le32(dg(m,0x255D8u))+static_cast<u32>(item)*65u;
    return *dg(m,rec+0x2Fu)==4u?1u:0u;
}
static_assert(0x0EF14u-0x0EEE0u==52u);

[[nodiscard]] s32 function_0ef14(Machine& m,u32 slot) noexcept {
    const u32 base=slot*0x248u;
    s32 value=static_cast<s16>(load_le16(dg(m,0x218FCu+base)));
    value=static_cast<s32>(static_cast<u32>(value)+load_le32(dg(m,0x21ACAu+base))+
                           load_le32(dg(m,0x219F3u+base)));
    return value;
}
static_assert(0x0EF32u-0x0EF14u==30u);

[[nodiscard]] s32 function_0ef34(Machine& m,s32 record_index) noexcept {
    if(record_index==-1)return 0;
    const u32 rec=load_le32(dg(m,0x255D8u))+static_cast<u32>(record_index)*65u;
    const u32 kind=*dg(m,rec+0x2Fu);
    if(kind>=2u&&kind<=8u)return -static_cast<s32>(*dg(m,rec+0x37u));
    return 0;
}
static_assert(0x0EF6Du-0x0EF34u==57u);

[[nodiscard]] s32 function_0ef70(Machine& m,s32 record_index) noexcept {
    if(record_index==-1)return 0;
    const u32 rec=load_le32(dg(m,0x255D8u))+static_cast<u32>(record_index)*65u;
    return static_cast<s8>(*dg(m,rec+0x34u))+function_0ef34(m,record_index);
}
static_assert(0x0EF9Du-0x0EF70u==45u);

[[nodiscard]] u32 function_0f2d4(Machine& m,u32 slot,s32 record_index) noexcept;

[[nodiscard]] s32 function_0efa0(Machine& m,Application20Boundary& b,u32 slot) noexcept {
    const u32 base=slot*0x248u;
    s32 value=static_cast<s8>(*dg(m,0x218FBu+base));
    const s32 primary=static_cast<s16>(load_le16(dg(m,0x21940u+base)));
    value+=function_0ef70(m,primary);

    if(static_cast<s16>(load_le16(dg(m,0x21948u+base)))==0x51&&function_0eee0(m,slot)==0u) {
        if(value>5)value=5;
        value+=function_0ef34(m,static_cast<s16>(load_le16(dg(m,0x21940u+base))));
    }

    value+=function_0ef70(m,static_cast<s16>(load_le16(dg(m,0x21942u+base))));
    const u32 stat=*dg(m,0x21915u+base);
    value+=static_cast<s8>(*dg(m,0x6FAFu+stat));

    const s32 left=static_cast<s16>(load_le16(dg(m,0x2193Cu+base)));
    if(left!=-1&&(b.call_24478?b.call_24478(slot,0u):0u)!=0u)value+=function_0ef70(m,left);
    const s32 right=static_cast<s16>(load_le16(dg(m,0x2193Eu+base)));
    if(right!=-1&&(b.call_24478?b.call_24478(slot,1u):0u)!=0u)value+=function_0ef70(m,right);

    const u32 cls=b.call_fe44?b.call_fe44(slot):0u;
    if(cls==3u)++value;
    if(cls==4u)value+=3;

    const u32 flags=load_le32(dg(m,0x21A12u+base));
    if((flags&1u)!=0u&&value>6&&function_0eee0(m,slot)==0u)value=6;
    if((flags&0x80u)!=0u)value-=2;
    if(function_0f2d4(m,slot,0x46)!=0u)--value;
    return value;
}
static_assert(0x0F167u-0x0EFA0u==455u);

[[nodiscard]] u32 function_0f168(u32 value) noexcept {
    switch(value) {
    case 0x006u: case 0x109u: case 0x10Eu: case 0x120u: case 0x127u:
    case 0x131u: case 0x134u: case 0x135u: case 0x13Bu: case 0x13Du:
    case 0x141u: case 0x146u: case 0x147u: return 1u;
    default: return 0u;
    }
}
static_assert(0x0F216u-0x0F168u==174u);

[[nodiscard]] u32 function_0f218(Machine& m,u32 slot,u32 bit) noexcept {
    const u32 base=slot*0x248u;
    const u32 shift=bit&31u;
    return (load_le32(dg(m,0x21A0Bu+base))&app20_shl32(1u,shift))>>shift;
}
static_assert(0x0F236u-0x0F218u==30u);

void function_0f238(Machine& m,u32 slot,u32 bit,u32 value) noexcept {
    const u32 base=slot*0x248u;
    const u32 shift=bit&31u;
    if(static_cast<s16>(load_le16(dg(m,0x21920u+base+bit*2u)))==-1)return;
    u32 flags=load_le32(dg(m,0x21A0Bu+base));
    flags&=~app20_shl32(1u,shift);
    flags|=app20_shl32(value,shift);
    store_le32(dg(m,0x21A0Bu+base),flags);
}
static_assert(0x0F271u-0x0F238u==57u);

[[nodiscard]] u32 function_0f274(Machine& m,u32 list,u32 bit) noexcept {
    const u32 shift=bit&31u;
    const u32 mask=app20_shl32(1u,shift);
    return (static_cast<u32>(*dg(m,0x7339u+list*10u))&mask)>>shift;
}
static_assert(0x0F290u-0x0F274u==28u);

void function_0f290(Machine& m,u32 list,u32 bit,u32 value) noexcept {
    const u32 shift=bit&31u;
    u8& flags=*dg(m,0x7339u+list*10u);
    flags=static_cast<u8>(flags&static_cast<u8>(~app20_shl8(1u,shift)));
    flags=static_cast<u8>(flags|app20_shl8(static_cast<u8>(value),shift));
}
static_assert(0x0F2B2u-0x0F290u==34u);

[[nodiscard]] u32 function_0f2b4(u32 value) noexcept {
    return value==0x10u||value==0x5Fu?1u:0u;
}
static_assert(0x0F2D1u-0x0F2B4u==29u);

[[nodiscard]] u32 function_0f2d4(Machine& m,u32 slot,s32 record_index) noexcept {
    const u32 base=slot*0x248u;
    if(static_cast<s16>(load_le16(dg(m,0x21938u+base)))==record_index)return 1u;
    if(static_cast<s16>(load_le16(dg(m,0x2193Au+base)))==record_index)return 1u;
    return 0u;
}
static_assert(0x0F305u-0x0F2D4u==49u);

void function_0f308(Machine& m,Application20Boundary& b,u32 a0,u32 a1,u32 a2,s32 record_index) noexcept {
    bool alternate=false;
    if(load_le16(dg(m,0x6DAEu))!=0u) {
        const u32 rec=load_le32(dg(m,0x255D8u))+static_cast<u32>(record_index)*65u;
        alternate=*dg(m,rec+0x37u)!=0u||load_le16(dg(m,rec+0x32u))!=0u;
    }
    if(alternate) {
        // 298F0 receives the same first four encoded arguments plus mode 1 and D470h.
        if(b.call_298f0)b.call_298f0(0u,a0,a1,a2,1u,0xD470u);
    } else if(b.call_2975c) {
        b.call_2975c(0u,a0,a1,a2,1u);
    }
}
static_assert(0x0F368u-0x0F308u==96u);

[[nodiscard]] u32 function_0f368(Machine& m,u32 slot) noexcept {
    const u32 cls=*dg(m,0x218F4u+slot*0x248u);
    return cls==2u||cls==3u||cls==4u?0u:1u;
}
static_assert(0x0F38Fu-0x0F368u==39u);

[[nodiscard]] u32 function_0f390(Machine& m,s32 root,s32 target) noexcept {
    if(static_cast<s16>(root)==static_cast<s16>(target))return 1u;
    const s32 idx=static_cast<s16>(root);
    const u32 rec=load_le32(dg(m,0x255D8u))+static_cast<u32>(idx)*65u;
    if(*dg(m,rec+0x2Fu)!=1u)return 0u;
    const s32 list=static_cast<s8>(*dg(m,rec+0x31u));
    const u32 lbase=static_cast<u32>(list)*10u;
    const u32 ptr=load_le32(dg(m,0x7335u+lbase));
    const u32 count=*dg(m,0x7330u+lbase);
    for(u32 i=0;i<count;++i) {
        const s32 child=static_cast<s16>(load_le16(dg(m,ptr+i*2u)));
        if(child==-1)continue;
        if(function_0f390(m,child,target)!=0u)return 1u;
    }
    return 0u;
}
static_assert(0x0F40Cu-0x0F390u==124u);

[[nodiscard]] u32 function_0f40c(Machine& m,s32 root,s32 target) noexcept {
    const s32 idx=static_cast<s16>(root);
    const u32 rec=load_le32(dg(m,0x255D8u))+static_cast<u32>(idx)*65u;
    if(*dg(m,rec+0x2Fu)!=1u)return 0u;
    const s32 list=static_cast<s8>(*dg(m,rec+0x31u));
    const u32 lbase=static_cast<u32>(list)*10u;
    const u32 ptr=load_le32(dg(m,0x7335u+lbase));
    const u32 count=*dg(m,0x7330u+lbase);
    for(u32 i=0;i<count;++i) {
        const s32 child=static_cast<s16>(load_le16(dg(m,ptr+i*2u)));
        if(child==-1)continue;
        if(static_cast<s16>(child)==static_cast<s16>(target)) {
            store_le16(dg(m,ptr+i*2u),0xFFFFu);
            function_0f290(m,static_cast<u32>(list),i,0u);
            return 1u;
        }
        if(function_0f40c(m,child,target)!=0u)return 1u;
    }
    return 0u;
}
static_assert(0x0F4ADu-0x0F40Cu==161u);

[[nodiscard]] u32 function_0f4b0(Machine& m,s32 target) noexcept {
    const s32 special=static_cast<s16>(load_le16(dg(m,0x8600u)));
    if(special!=-1&&function_0f390(m,special,target)!=0u)return 1u;
    for(u32 slot=0u,base=0u;slot<4u;++slot,base+=0x248u) {
        if((load_le16(dg(m,0x218F0u+base))&0x8000u)!=0u)continue;
        for(u32 i=0;i<0x15u;++i) {
            const s32 item=static_cast<s16>(load_le16(dg(m,0x21920u+base+i*2u)));
            if(item==-1)continue;
            if(function_0f390(m,item,target)!=0u)return 1u;
        }
    }
    return 0u;
}
static_assert(0x0F53Du-0x0F4B0u==141u);

void function_0f540(Machine& m,Application20Boundary& b,s32 target) noexcept {
    const s32 special=static_cast<s16>(load_le16(dg(m,0x8600u)));
    if(special!=-1) {
        if(special==target) {
            if(b.call_1a344)b.call_1a344(-1,0u);
            return;
        }
        if(function_0f40c(m,special,target)!=0u)return;
    }
    for(u32 slot=0u,base=0u;slot<4u;++slot,base+=0x248u) {
        if((load_le16(dg(m,0x218F0u+base))&0x8000u)!=0u)continue;
        for(u32 i=0;i<0x15u;++i) {
            const s32 item=static_cast<s16>(load_le16(dg(m,0x21920u+base+i*2u)));
            if(item==-1)continue;
            if(item==target) {
                store_le16(dg(m,0x21920u+base+i*2u),0xFFFFu);
                function_0f238(m,slot,i,0u);
                break;
            }
            if(function_0f40c(m,item,target)!=0u)return;
        }
    }
}
static_assert(0x0F5F7u-0x0F540u==183u);

[[nodiscard]] u32 function_0f5f8(Machine& m,s32 list) noexcept {
    u32 total=0u;
    if(list==0x14) {
        total=*dg(m,load_le32(dg(m,0x255D8u))+0x1480u);
    } else {
        const u32 lbase=static_cast<u32>(list)*10u;
        const u32 ptr=load_le32(dg(m,0x7335u+lbase));
        const u32 count=*dg(m,0x7330u+lbase);
        for(u32 i=0;i<count;++i) {
            const s32 item=static_cast<s16>(load_le16(dg(m,ptr+i*2u)));
            if(item==-1)continue;
            const u32 rec=load_le32(dg(m,0x255D8u))+static_cast<u32>(item)*65u;
            total+=*dg(m,rec+0x30u);
            if(*dg(m,rec+0x2Fu)==1u)
                total+=function_0f5f8(m,static_cast<s8>(*dg(m,rec+0x31u)));
        }
    }
    return static_cast<u16>(total);
}
static_assert(0x0F66Cu-0x0F5F8u==116u);

// -----------------------------------------------------------------------------
// Batch 21: 0F66C..10155 item-description presentation, inventory burden/rank
// helpers, character panel rendering, and page-display wrappers.
// The ten-entry CS table at 0F768 is data in RESIDUAL_LEDGER.md.
// -----------------------------------------------------------------------------

struct Application21Boundary {
    Application20Boundary* app20=nullptr;
    u32 (*call_30288)(u32,u32)=nullptr;
    u32 (*call_30ca2)()=nullptr;
    void (*call_3975c)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_40508)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_3932c)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_2c368)(u32,u32)=nullptr;
    u32 (*call_3e6f0)(s32)=nullptr;
    u32 (*call_3e70c)(s32)=nullptr;
    void (*call_40cc4)(u32)=nullptr;
    u32 (*call_40700)()=nullptr;
    void (*call_3e510)(u32,u32,u32,u32)=nullptr;
    u32 (*call_2f3fc)(u32)=nullptr;
    void (*call_398f0)(u32,u32,u32,u32,u32,u32)=nullptr;
    void (*call_39b40)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_2a5b0)(u32,s32,u32)=nullptr;
    void (*call_2a724)(u32,s32,u32,u32,u32,u32,u32)=nullptr;
};

static void app21_copy_cstr(Machine& m,u32 dst,u32 src) noexcept {
    for(;;){const u8 c=*dg(m,src++);*dg(m,dst++)=c;if(c==0u)break;}
}
static u32 app21_end(Machine& m,u32 dst) noexcept {
    while(*dg(m,dst)!=0u)++dst;
    return dst;
}
static void app21_append_cstr(Machine& m,u32 dst,u32 src) noexcept {
    app21_copy_cstr(m,app21_end(m,dst),src);
}
static void app21_resource(Machine& m,Application21Boundary& b,u32 dst,u32 id) noexcept {
    if(b.call_2c368)b.call_2c368(app21_end(m,dst),id);
}
static void app21_number(Machine& m,Application21Boundary& b,u32 dst,s32 value) noexcept {
    if(b.call_3e6f0)app21_append_cstr(m,dst,b.call_3e6f0(value));
}
static void app21_weight(Machine& m,Application21Boundary& b,u32 dst,s32 value) noexcept {
    if(b.call_3e70c)app21_append_cstr(m,dst,b.call_3e70c(value));
}

void function_0f66c(Machine& m,Application21Boundary& b,u32 text) noexcept {
    const s32 width=b.call_30288?static_cast<s32>(b.call_30288(text,1u)):0;
    s32 left=0xA0-(((width+0x10)/2+3)&~3);
    if(((left+4)%8)==0)left-=4;
    if(((left+4)%0x50)==0)left-=8;
    const s32 right=0x140-left, limit=0x138-left; s32 inner=left+4;
    for(s32 x=left;x<right;x+=8) {
        u32 tile;
        if(x==left)tile=0u;
        else if(x>=limit)tile=5u;
        else if((inner%0x50)==0)tile=1u;
        else tile=2u+((b.call_30ca2?b.call_30ca2():0u)*3u>>15u);
        if(b.call_3975c)b.call_3975c(tile,0x62u,static_cast<u32>(x),3u,2u);
        inner+=8;
    }
    if(b.call_40508)b.call_40508(0x65u,0u,0x140u,text,1u);
}
static_assert(0x0F767u-0x0F66Cu==251u);

void function_0f790(Machine& m,Application21Boundary& b,s32 item,u32 detailed) noexcept {
    if(b.call_3932c)b.call_3932c(load_le32(dg(m,0x7F68u)),0x62u,0u,0x10u,0x140u);
    if(*dg(m,0x6DACu)!=0u){function_0f66c(m,b,0x759Cu);return;}
    if(item==-1)return;
    const u32 rec=load_le32(dg(m,0x255D8u))+static_cast<u32>(item)*65u;
    app21_copy_cstr(m,0x759Cu,rec);
    if(*dg(m,rec+0x37u)!=0u&&detailed!=0u){
        app21_append_cstr(m,0x759Cu,0x610u);
        app21_number(m,b,0x759Cu,*dg(m,rec+0x37u));
    }
    app21_append_cstr(m,0x759Cu,0x614u);
    const u32 type=*dg(m,rec+0x2Fu);
    if(type>=9u&&type<=11u){
        const u32 a=*dg(m,rec+0x38u), c=*dg(m,rec+0x39u), d=*dg(m,rec+0x3Au);
        app21_resource(m,b,0x759Cu,0x3Eu); app21_append_cstr(m,0x759Cu,0x618u);
        app21_number(m,b,0x759Cu,static_cast<s32>(a+d)); app21_append_cstr(m,0x759Cu,0x61Cu);
        app21_number(m,b,0x759Cu,static_cast<s32>(a*c+d));
        const u32 a2=*dg(m,rec+0x3Bu), c2=*dg(m,rec+0x3Cu), d2=*dg(m,rec+0x3Du);
        app21_append_cstr(m,0x759Cu,0x620u); app21_number(m,b,0x759Cu,static_cast<s32>(a2+d2));
        app21_append_cstr(m,0x759Cu,0x61Cu); app21_number(m,b,0x759Cu,static_cast<s32>(a2*c2+d2));
        app21_append_cstr(m,0x759Cu,0x614u);
    } else if(type>=2u&&type<=4u){
        app21_resource(m,b,0x759Cu,0x40u); app21_append_cstr(m,0x759Cu,0x618u);
        app21_number(m,b,0x759Cu,-static_cast<s32>(static_cast<s8>(*dg(m,rec+0x34u))));
        app21_append_cstr(m,0x759Cu,0x614u);
    }

    app21_resource(m,b,0x759Cu,0x3Du);
    u16 total=*dg(m,rec+0x30u);
    if(type==1u)total=static_cast<u16>(total+function_0f5f8(m,static_cast<u32>(static_cast<s8>(*dg(m,rec+0x31u)))));
    app21_weight(m,b,0x759Cu,static_cast<s32>(total/2u));
    if((total&1u)!=0u)app21_append_cstr(m,0x759Cu,0x624u);

    if(type==1u){
        const u32 g=static_cast<u32>(static_cast<s8>(*dg(m,rec+0x31u)))*10u;
        const u32 v=*dg(m,0x7332u+g);
        if(v!=0x7Fu){
            app21_append_cstr(m,0x759Cu,0x614u); app21_resource(m,b,0x759Cu,0x41u);
            app21_append_cstr(m,0x759Cu,0x618u); app21_number(m,b,0x759Cu,static_cast<s32>(v));
        }
    } else if((*dg(m,rec+0x31u)!=0u||item==0x4A)&&detailed!=0u){
        app21_append_cstr(m,0x759Cu,load_le32(dg(m,0x6D98u)));
        app21_number(m,b,0x759Cu,*dg(m,0x67ECu+static_cast<u32>(static_cast<s8>(*dg(m,rec+0x31u)))));
    }
    if(b.call_40cc4)b.call_40cc4(static_cast<u32>(item));
    function_0f66c(m,b,0x759Cu);
    const u32 token=b.call_40700?b.call_40700():0u;
    if(b.call_40cc4)b.call_40cc4(token);
}
static_assert(0x0FD84u-0x0F790u==1524u);

[[nodiscard]] u32 function_0fd84(Machine& m,u32 slot) noexcept {
    const u32 base=slot*0x248u; u32 sum=0u;
    for(u32 off=0u;off<0x2Au;off+=2u){
        const s32 item=static_cast<s16>(load_le16(dg(m,0x21920u+base+off)));
        if(item==-1)continue;
        const u32 rec=load_le32(dg(m,0x255D8u))+static_cast<u32>(item)*65u;
        sum+=*dg(m,rec+0x30u);
        if(*dg(m,rec+0x2Fu)==1u)sum+=function_0f5f8(m,static_cast<u32>(static_cast<s8>(*dg(m,rec+0x31u))));
    }
    return sum/2u;
}
static_assert(0x0FDE3u-0x0FD84u==95u);

[[nodiscard]] u32 function_0fde4(Machine& m,u32 slot) noexcept {
    const u32 base=slot*0x248u; s32 index=*dg(m,0x21912u+base); const u32 meter=*dg(m,0x2191Eu+base);
    if(meter>=1u&&meter<=0x32u)index+=1;
    if(meter>=0x33u&&meter<=0x4Bu)index+=2;
    if(meter>=0x4Cu&&meter<=0x5Au)index+=3;
    if(meter>=0x5Bu&&meter<=0x63u)index+=4;
    if(meter==0x64u)index+=5;
    index-=2;
    return load_le16(dg(m,0x7E94u+static_cast<u32>(index*10)));
}
static_assert(0x0FE42u-0x0FDE4u==94u);

[[nodiscard]] u32 function_0fe44(Machine& m,u32 slot) noexcept {
    const u32 base=slot*0x248u; s32 state=*dg(m,0x21913u+base);
    if(state==0x12){
        const u32 meter=*dg(m,0x2191Fu+base);
        if(meter>=1u&&meter<=0x32u)state+=1;
        if(meter>=0x33u&&meter<=0x4Bu)state+=2;
        if(meter>=0x4Cu&&meter<=0x5Au)state+=3;
        if(meter>=0x5Bu&&meter<=0x63u)state+=4;
        if(meter==0x64u)state+=5;
    }
    const u32 burden=function_0fd84(m,slot); const s32 row=state-2;
    for(u32 level=0;level<5u;++level)
        if(static_cast<s32>(burden)<=static_cast<s32>(load_le16(dg(m,0x7E8Cu+static_cast<u32>(row*10)+level*2u))))return level;
    return 4u;
}
static_assert(0x0FECFu-0x0FE44u==139u);

void function_0fed0(Machine& m,Application21Boundary& b,u32 slot) noexcept {
    const u32 rank=function_0fe44(m,slot);
    if(b.call_3975c)b.call_3975c(rank,0x10u,load_le32(dg(m,0x7F6Cu))+0x44u,0x11u,2u);
    if(b.call_3975c)b.call_3975c(0u,0x10u,load_le32(dg(m,0x7F6Cu))+0x38u,0x10u,2u);
    s32 value=0;
    if(b.app20)value=function_0efa0(m,*b.app20,slot);
    const u32 text=b.call_3e6f0?b.call_3e6f0(value):0u;
    if(b.call_40508)b.call_40508(0x16u,load_le32(dg(m,0x7F6Cu))+0x38u,0x0Eu,text,0u);
}
static_assert(0x0FF3Bu-0x0FED0u==107u);

void function_0ff3c(Machine& m,Application21Boundary& b,u32 slot) noexcept {
    const u32 base=slot*0x248u;
    if(b.call_3e510)b.call_3e510(0u,load_le32(dg(m,0x7F6Cu)),0u,2u);
    if(b.call_40508)b.call_40508(2u,load_le32(dg(m,0x7F6Cu)),0x50u,0x218E0u+base,1u);
    const u32 portrait=b.call_2f3fc?b.call_2f3fc(slot):0u;
    s32 graphic=static_cast<s8>(*dg(m,0x218F2u+base))+
                static_cast<s32>(*dg(m,0x218F3u+base))*0x17+
                static_cast<s32>(load_le32(dg(m,0x258E4u)));
    if((load_le16(dg(m,0x218F0u+base))&0x0100u)!=0u)graphic+=0x2E;
    if(b.call_398f0)b.call_398f0(0u,0x10u,static_cast<u32>(graphic),1u,
                                 load_le32(dg(m,0x7F6Cu))+3u,0xCF70u+(portrait<<8u));
    if((load_le16(dg(m,0x218F0u+base))&0x4000u)==0u){
        const s32 hp=function_0ef14(m,slot);
        const s32 maxhp=static_cast<s16>(load_le16(dg(m,0x218FEu+base)));
        if(maxhp!=0){
            const s32 amount=(hp*80+maxhp/2)/maxhp;
            const u32 dst=load_le32(dg(m,0x7F6Cu))+0x2Fu;
            if(amount>0x50){if(b.call_39b40)b.call_39b40(0x10u,dst,0x50u,4u,0x1Bu);}
            else {
                const u32 color=amount>0x14?0x43u:0x57u;
                if(b.call_39b40)b.call_39b40(static_cast<u32>(0x60-amount),dst,static_cast<u32>(amount),4u,color);
            }
        }
    }
    function_0fed0(m,b,slot);
    if(load_le32(dg(m,0x21A12u+base))!=0u&&b.call_3975c)
        b.call_3975c(0u,0x2Au,load_le32(dg(m,0x7F6Cu))+0x44u,0x13u,2u);
    if((load_le16(dg(m,0x218F0u+base))&0x1F00u)!=0u&&b.call_3975c)
        b.call_3975c(0u,0x2Au,load_le32(dg(m,0x7F6Cu))+0x38u,0x12u,2u);
}
static_assert(0x100C5u-0x0FF3Cu==393u);

void function_100c8(Machine& m,Application21Boundary& b,s32 id,u32 mode) noexcept {
    if(id>=0xA7&&id<=0xC9)store_le32(dg(m,0xBF44u),0x3Au);
    if(b.call_2a5b0)b.call_2a5b0(load_le32(dg(m,0xC050u)),static_cast<s16>(id),static_cast<u8>(mode));
    *dg(m,0xC050u)^=1u;
}
static_assert(0x10106u-0x100C8u==62u);
void function_10108(Machine& m,Application21Boundary& b,s32 id,u32 mode,u32 a,u32 c,u32 d,u32 e) noexcept {
    if(id>=0xA7&&id<=0xC9)store_le32(dg(m,0xBF44u),0x3Au);
    if(b.call_2a724)b.call_2a724(load_le32(dg(m,0xC050u)),static_cast<s16>(id),static_cast<u8>(mode),a,c,d,e);
    *dg(m,0xC050u)^=1u;
}
static_assert(0x10156u-0x10108u==78u);

// -----------------------------------------------------------------------------
// Batch 22: 10158..10FA8 character-sheet text/stat presentation, equipment
// grid rendering, and orientation-aware inventory/equipment panel drawing.
// -----------------------------------------------------------------------------

struct Application22Boundary {
    Application20Boundary* app20=nullptr;
    Application21Boundary* app21=nullptr;
    void (*call_405a8)(u32,u32,u32,u32)=nullptr;
    void (*call_4060c)(u32,u32,u32,u32)=nullptr;
    u32 (*call_3f29c)(u32,u32)=nullptr;
    void (*call_3e404)(u32,u32)=nullptr;
    void (*call_4210f3)(u32,u32,u32)=nullptr;
    void (*call_4210f4)(u32,u32,u32,u32)=nullptr;
    s32 (*call_efa0)(u32)=nullptr;
    void (*call_4059c)(u32)=nullptr;
    void (*call_403c8)(u32,u32)=nullptr;
    void (*call_3a258)(u32,u32)=nullptr;
    void (*divide_error)()=nullptr;
};

static void app22_clear_text(Machine& m) noexcept {
    for(u32 i=0u;i<0x100u;++i)*dg(m,0x759Cu+i)=0u;
}
static void app22_resource(Machine& m,Application22Boundary& b,u32 id) noexcept {
    if(b.app21)app21_resource(m,*b.app21,0x759Cu,id);
}
static void app22_append(Machine& m,u32 src) noexcept {
    app21_append_cstr(m,0x759Cu,src);
}
static void app22_number(Machine& m,Application22Boundary& b,s32 value) noexcept {
    if(b.app21)app21_number(m,*b.app21,0x759Cu,value);
}
static u32 app22_weight(Application22Boundary& b,s32 value) noexcept {
    return b.app21&&b.app21->call_3e70c?b.app21->call_3e70c(value):0u;
}
static void app22_draw_40508(Machine& m,Application22Boundary& b,u32 x) noexcept {
    if(b.app21&&b.app21->call_40508)
        b.app21->call_40508(x,load_le32(dg(m,0x7F6Cu)),0x50u,0x759Cu,1u);
}

void function_10158(Machine& m,Application22Boundary& b,u32 slot) noexcept {
    Application21Boundary* a=b.app21;
    const u32 dst=load_le32(dg(m,0x7F6Cu));
    const u32 actor=slot*0x248u;
    if(a&&a->call_3975c)a->call_3975c(0u,0x45u,dst+0x38u,0x24u,2u);

    *dg(m,0x6DACu)=0u;
    if(a&&a->call_2c368)a->call_2c368(0x759Cu,0x3Au);
    app22_draw_40508(m,b,0x126u);
    if(a&&a->call_2c368)a->call_2c368(0x759Cu,0x3Bu);
    app22_draw_40508(m,b,0x15Bu);

    if(a&&a->call_2c368)a->call_2c368(0x759Cu,static_cast<u32>(static_cast<s8>(*dg(m,0x218F5u+actor))+0x15));
    for(u32 i=1u;i<3u;++i) {
        const s32 cls=static_cast<s8>(*dg(m,0x218F5u+actor+i));
        if(cls<0)continue;
        app22_append(m,0x620u);
        app22_resource(m,b,static_cast<u32>(cls+0x15));
    }
    app22_draw_40508(m,b,0x7Bu);

    if(a&&a->call_2c368)a->call_2c368(0x759Cu,0x3Cu);
    app22_append(m,0x628u);
    app22_number(m,b,static_cast<s16>(load_le16(dg(m,0x21900u+actor))));
    const s32 v1=static_cast<s16>(load_le16(dg(m,0x21902u+actor)));
    if(v1>0){app22_append(m,0x620u);app22_number(m,b,v1);}
    const s32 v2=static_cast<s16>(load_le16(dg(m,0x21904u+actor)));
    if(v2>0){app22_append(m,0x620u);app22_number(m,b,v2);}
    app22_draw_40508(m,b,0x88u);

    if(a&&a->call_2c368)a->call_2c368(0x759Cu,static_cast<u32>(*dg(m,0x218FAu+actor))+0x1Du);
    app22_draw_40508(m,b,0x95u);
    if(a&&a->call_2c368)a->call_2c368(0x759Cu,static_cast<u32>(*dg(m,0x218F4u+actor))+0x0Fu);
    app22_append(m,0x618u);
    app22_resource(m,b,static_cast<u32>(*dg(m,0x218F3u+actor))+0x1Bu);
    app22_draw_40508(m,b,0xA2u);

    for(u32 i=0u,id=0x2Fu,y=0xB8u;i<6u;++i,++id,y+=0x0Cu) {
        if(a&&a->call_2c368)a->call_2c368(0x759Cu,id);
        if(b.call_4060c)b.call_4060c(y,dst+0x0Cu,0x759Cu,1u);
    }

    app22_clear_text(m);
    *dg(m,0x75B7u)=0x45u;
    if(b.call_3f29c&&b.call_3e404) {
        const u32 h=b.call_3f29c(1u,0u);
        if(h!=0u)b.call_3e404(0x759Cu,load_le32(dg(m,h)));
    }
    for(u32 i=0u,y=0xB8u;i<6u;++i,y+=0x0Cu) {
        const u32 state=*dg(m,0x21913u+actor+i*2u);
        if(i==0u&&state==0x12u&&*dg(m,0x2191Fu+actor)!=0u) {
            const u32 rem=static_cast<u32>(*dg(m,0x2191Fu+actor))%100u;
            if(b.call_4210f4)b.call_4210f4(0x759Cu,0x62Cu,state,rem);
        } else if(b.call_4210f3)b.call_4210f3(0x759Cu,0x638u,state);
        if(b.call_405a8)b.call_405a8(y,dst+0x2Cu,0x759Cu,1u);
    }

    app22_clear_text(m);
    *dg(m,0x75E1u)=0x1Bu;
    if(b.call_3f29c&&b.call_3e404) {
        const u32 h=b.call_3f29c(1u,0u);
        if(h!=0u)b.call_3e404(0x759Cu,load_le32(dg(m,h)));
    }
    for(u32 i=0u,y=0xB8u;i<6u;++i,y+=0x0Cu) {
        const u32 state=*dg(m,0x21912u+actor+i*2u);
        if(i==0u&&*dg(m,0x2191Eu+actor)!=0u) {
            const u32 rem=static_cast<u32>(*dg(m,0x2191Eu+actor))%100u;
            if(b.call_4210f4)b.call_4210f4(0x759Cu,0x62Cu,state,rem);
        } else if(b.call_4210f3)b.call_4210f3(0x759Cu,0x638u,state);
        if(b.call_405a8)b.call_405a8(y,dst+0x43u,0x759Cu,1u);
    }

    if(a&&a->call_2c368)a->call_2c368(0x759Cu,0x36u);
    app22_append(m,0x618u);
    app22_number(m,b,b.call_efa0?b.call_efa0(slot):0);
    app22_draw_40508(m,b,0x104u);

    if(a&&a->call_2c368)a->call_2c368(0x759Cu,0x39u);
    app22_append(m,0x618u);
    const s32 current=static_cast<s16>(load_le16(dg(m,0x218FCu+actor)));
    const s32 maximum=static_cast<s16>(load_le16(dg(m,0x218FEu+actor)));
    app22_number(m,b,current);
    app22_append(m,0x620u);
    app22_number(m,b,maximum);
    const s32 width=a&&a->call_30288?static_cast<s32>(a->call_30288(0x759Cu,1u)):0;
    const u32 centered=static_cast<u32>(static_cast<s32>(dst+0x28u)-width/2);
    app22_resource(m,b,0x39u);
    app22_append(m,0x618u);
    if(b.call_4060c)b.call_4060c(0x112u,centered,0x759Cu,1u);

    app22_clear_text(m);
    const s32 hp=static_cast<s16>(load_le16(dg(m,0x218FCu+actor)))+
                 static_cast<s32>(load_le32(dg(m,0x21ACAu+actor)))+
                 static_cast<s32>(load_le32(dg(m,0x219F3u+actor)));
    const s32 hpmax=static_cast<s16>(load_le16(dg(m,0x218FEu+actor)));
    s32 ratio=0;
    if(hpmax==0) { if(b.divide_error)b.divide_error(); return; }
    ratio=static_cast<s32>(static_cast<u32>(hp)*80u+static_cast<u32>(hpmax/2))/hpmax;
    *dg(m,0x75B7u)=static_cast<u8>(ratio>0x50?0x1Bu:0x43u);
    if(b.call_4059c)b.call_4059c(0x759Cu);
    if(b.call_403c8&&a&&a->call_3e6f0)b.call_403c8(a->call_3e6f0(current),1u);
    if(b.call_4059c)b.call_4059c(0u);
    if(b.call_403c8)b.call_403c8(0x620u,1u);
    if(b.call_403c8&&a&&a->call_3e6f0)b.call_403c8(a->call_3e6f0(maximum),1u);

    u32 tracks=0u;
    for(u32 i=0u;i<3u;++i)if(static_cast<s8>(*dg(m,0x218F5u+actor+i))>=0)++tracks;
    s32 y=0x145-static_cast<s32>((tracks*0x0Bu)/2u);
    s32 y2=y+0x35;
    for(u32 i=0u;i<tracks;++i,y+=0x0B,y2+=0x0B) {
        const s32 cls=static_cast<s8>(*dg(m,0x218F5u+actor+i));
        if(a&&a->call_2c368)a->call_2c368(0x759Cu,static_cast<u32>(cls+0x15));
        if(b.call_405a8)b.call_405a8(static_cast<u32>(y),dst+0x22u,0x759Cu,1u);
        if(b.call_405a8)b.call_405a8(static_cast<u32>(y2),dst+0x22u,0x759Cu,1u);
        const u32 xp=load_le32(dg(m,0x21906u+actor+i*4u));
        const u32 xp_text=app22_weight(b,static_cast<s32>(xp));
        if(b.call_405a8)b.call_405a8(static_cast<u32>(y),dst+0x43u,xp_text,1u);
        const s32 level=static_cast<s16>(load_le16(dg(m,0x21900u+actor+i*2u)));
        const u32 need_base=load_le16(dg(m,0x7D9Cu+static_cast<u32>(cls)*0x28u+static_cast<u32>(level)*2u))*1000u;
        const u32 need=need_base-xp;
        const u32 need_text=app22_weight(b,static_cast<s32>(need));
        if(b.call_405a8)b.call_405a8(static_cast<u32>(y2),dst+0x43u,need_text,1u);
    }
}
static_assert(0x10A6Du-0x10158u==2325u);

void function_10a70(Machine& m,Application22Boundary& b,u32 panel,u32 mode) noexcept {
    if(!b.app21)return;
    const s32 x0=static_cast<s8>(*dg(m,0x6FA0u+panel*4u));
    const s32 y0=static_cast<s8>(*dg(m,0x6FA1u+panel*4u));
    const u32 dst=load_le32(dg(m,0x7F6Cu));
    if(mode==1u||mode==2u) {
        const u32 x=static_cast<u32>(x0*2+0x14+static_cast<s32>(mode)-1);
        if(b.app21->call_3e510)b.app21->call_3e510(0xC4u,dst,x,2u);
        return;
    }
    if(mode!=3u)return;
    const s32 table=y0*10;
    if(b.app21->call_3e510)b.app21->call_3e510(0xCEu,dst,static_cast<u32>(x0+9),2u);
    const u32 base=panel*0x50u;
    const u32 list=load_le32(dg(m,0x7335u+static_cast<u32>(table)));
    const u32 count=*dg(m,0x7330u+static_cast<u32>(table));
    const u32 cols=*dg(m,0x7331u+static_cast<u32>(table));
    for(u32 i=0u;i<count;++i) {
        const u32 row=cols?i/cols:0u;
        const u32 col=cols?i%cols:0u;
        const u32 draw_y=base+*dg(m,0x7333u+static_cast<u32>(table))+col*0x12u;
        const u32 draw_x=0xDBu+row*0x22u;
        if(b.app21->call_39b40)b.app21->call_39b40(draw_x,draw_y,0x20u,0x10u,0u);
        const s32 item=static_cast<s16>(load_le16(dg(m,list+i*2u)));
        if(item==-1)continue;
        const u32 rec=load_le32(dg(m,0x255D8u))+static_cast<u32>(item)*0x41u;
        const s32 graphic=static_cast<s16>(load_le16(dg(m,rec+0x2Du)));
        if(b.app20)function_0f308(m,*b.app20,draw_x,draw_y,static_cast<u32>(graphic),item);
    }
}
static_assert(0x10BBCu-0x10A70u==332u);

static void app22_draw_item(Machine& m,Application22Boundary& b,u32 x,u32 dst,s32 item,s32 graphic) noexcept {
    if(b.app20)function_0f308(m,*b.app20,x,dst,static_cast<u32>(graphic),item);
}
static s32 app22_item_graphic(Machine& m,s32 item) noexcept {
    const u32 rec=load_le32(dg(m,0x255D8u))+static_cast<u32>(item)*0x41u;
    return static_cast<s16>(load_le16(dg(m,rec+0x2Du)));
}

void function_10bbc(Machine& m,Application22Boundary& b,u32 slot) noexcept {
    if(!b.app21)return;
    const u32 actor=slot*0x248u;
    const u32 side=function_0f368(m,slot)^1u;
    const u32 dst=load_le32(dg(m,0x7F6Cu));
    const u32 frame=static_cast<u32>(*dg(m,0x218F3u+actor))+side*2u+5u;
    if(b.app21->call_3e510)b.app21->call_3e510(0x72u,dst,frame,2u);

    if(static_cast<s16>(load_le16(dg(m,0x21946u+actor)))>=0) {
        const s32 graphic=static_cast<s16>(load_le16(dg(m,load_le32(dg(m,0x255D8u))+0x1B3u)))+
                          static_cast<s32>(load_le32(dg(m,0x258D8u)));
        app22_draw_item(m,b,0x72u+side*0x15u,dst,6,graphic);
    }

    const struct {u32 off;u32 mul;bool add_side;} simple[]={{0x21940u,0u,true},{0x21944u,0x13u,false},{0x21942u,0x14u,false}};
    for(const auto& s:simple) {
        const s32 item=static_cast<s16>(load_le16(dg(m,s.off+actor)));
        if(item<0)continue;
        s32 graphic=app22_item_graphic(m,item)+static_cast<s32>(load_le32(dg(m,0x258D8u)));
        if(s.add_side)graphic+=static_cast<s32>(side);
        app22_draw_item(m,b,0x72u+side*s.mul,dst,item,graphic);
    }

    const s32 item48=static_cast<s16>(load_le16(dg(m,0x21948u+actor)));
    if(item48>=0) {
        const s32 graphic=app22_item_graphic(m,item48)+static_cast<s32>(load_le32(dg(m,0x258D8u)));
        u32 sprite=b.call_3f29c?b.call_3f29c(static_cast<u32>(graphic),1u):0u;
        const u32 x=0x72u+side*0x12u;
        app22_draw_item(m,b,x,dst,item48,graphic);
        if(b.call_3a258)b.call_3a258(0u,sprite);
        const u32 width=sprite?static_cast<u32>(*dg(m,sprite+6u))*8u:0u;
        app22_draw_item(m,b,x,dst+0x50u-width,item48,graphic);
        if(b.call_3a258)b.call_3a258(0u,sprite);
    }

    for(u32 k=0u;k<2u;++k) {
        const u32 off=k==0u?0x21938u:0x2193Au;
        const u32 dstdelta=k==0u?3u:0x3Du;
        const s32 item=static_cast<s16>(load_le16(dg(m,off+actor)));
        if(item<0)continue;
        if(b.app21->call_39b40)b.app21->call_39b40(0xF8u,dst+dstdelta,0x20u,0x10u,0u);
        const s32 graphic=app22_item_graphic(m,item);
        app22_draw_item(m,b,0xF8u,dst+dstdelta,item,graphic);
    }

    const s32 item3c=static_cast<s16>(load_le16(dg(m,0x2193Cu+actor)));
    if(item3c>=0) {
        const u32 x=0x72u+side*0x12u;
        if(function_0f168(static_cast<u32>(item3c))!=0u) {
            const s32 graphic=static_cast<s16>(load_le16(dg(m,load_le32(dg(m,0x255D8u))+0x1B3u)))+
                              static_cast<s32>(load_le32(dg(m,0x258D8u)))+1;
            app22_draw_item(m,b,x,dst,6,graphic);
        } else {
            const s32 graphic=app22_item_graphic(m,item3c)+static_cast<s32>(load_le32(dg(m,0x258D8u)));
            app22_draw_item(m,b,x,dst,item3c,graphic);
        }
    }

    const s32 item3e=static_cast<s16>(load_le16(dg(m,0x2193Eu+actor)));
    if(item3e>=0) {
        const s32 panel=static_cast<s8>(*dg(m,0x6FA0u+slot*4u));
        if(panel!=-1) {
            function_10a70(m,b,slot,*dg(m,0x6FA2u+slot*4u));
        } else {
            s32 graphic;
            s32 item_arg;
            if(function_0f168(static_cast<u32>(item3e))!=0u) {
                graphic=static_cast<s16>(load_le16(dg(m,load_le32(dg(m,0x255D8u))+0x1B3u)))+
                        static_cast<s32>(load_le32(dg(m,0x258D8u)))+1;
                item_arg=6;
            } else {
                graphic=app22_item_graphic(m,item3e)+static_cast<s32>(load_le32(dg(m,0x258D8u)));
                item_arg=item3e;
            }
            const u32 sprite=b.call_3f29c?b.call_3f29c(static_cast<u32>(graphic),1u):0u;
            if(b.call_3a258)b.call_3a258(0u,sprite);
            const u32 width=sprite?static_cast<u32>(*dg(m,sprite+6u))*8u:0u;
            app22_draw_item(m,b,0x72u+side*0x12u,dst+0x4Fu-width,item_arg,graphic);
            if(b.call_3a258)b.call_3a258(0u,sprite);
        }
    }
}
static_assert(0x10FA9u-0x10BBCu==1005u);

// -----------------------------------------------------------------------------
// Batch 23: 10FAC..11C2B panel lifecycle/redraw, item-grid presentation,
// equipment/container compatibility, nested-placement UI, and item swapping.
// CS tables at 113F4 and 11C2C are data in RESIDUAL_LEDGER.md.
// -----------------------------------------------------------------------------

struct Application23Boundary {
    Application20Boundary* app20=nullptr;
    Application21Boundary* app21=nullptr;
    Application22Boundary* app22=nullptr;
    void (*call_2e374)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_292bc)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_22d70)()=nullptr;
    void (*call_1c124)(u32)=nullptr;
    void (*call_4210f2)(u32,u32)=nullptr;
    s32 (*call_50dc)(u32,u32)=nullptr;
    void (*call_03a78)(u32,s32)=nullptr;
    u32 (*read_11630_stale_offset)()=nullptr;
};

static u8 app23_shl8(u8 value,u32 count) noexcept {
    count&=31u;
    if(count>=8u)return 0u;
    return static_cast<u8>(static_cast<u32>(value)<<count);
}
static u32 app23_item_rec(Machine& m,s32 item) noexcept {
    return load_le32(dg(m,0x255D8u))+static_cast<u32>(item*65);
}
static bool app23_special_item_range(s32 item) noexcept {
    const u32 v=static_cast<u32>(item);
    return v>=0xA7u&&v<=0xC9u;
}
static void app23_mark_special(Machine& m,s32 item) noexcept {
    if(app23_special_item_range(item))store_le32(dg(m,0xBF44u),0x3Au);
}
static void app23_select_page(Machine& m,Application23Boundary& b) noexcept {
    if(b.app20&&b.app20->call_291b0)b.app20->call_291b0(*dg(m,0xC050u));
}
static void app23_draw_cursor_item(Machine& m,Application23Boundary& b,s32 item,u32 variant) noexcept {
    app23_mark_special(m,item);
    if(b.app21&&b.app21->call_2a5b0)
        b.app21->call_2a5b0(load_le32(dg(m,0xC050u)),static_cast<u32>(item),static_cast<u8>(variant));
    *dg(m,0xC050u)^=1u;
}

void function_10fac(Machine& m,Application23Boundary& b,u32 panel,u32 xmeta,u32 ymeta,u32 zmeta) noexcept {
    const u32 state=panel*4u;
    *dg(m,0x6FA0u+state)=static_cast<u8>(xmeta);
    *dg(m,0x6FA1u+state)=static_cast<u8>(ymeta);
    *dg(m,0x6FA3u+state)=static_cast<u8>(zmeta);
    const u32 first=xmeta*2u+0x14u;
    for(u32 i=first;i<first+4u;++i)
        if(b.app22&&b.app22->call_3f29c)(void)b.app22->call_3f29c(i,2u);
    const u32 dst=panel*0x50u;
    for(u32 mode=0u;mode<4u;++mode) {
        app23_select_page(m,b);
        *dg(m,0x6FA2u+state)=static_cast<u8>(mode);
        if(b.app22)function_10bbc(m,*b.app22,panel);
        const s32 item=static_cast<s16>(load_le16(dg(m,0x8600u)));
        const u32 variant=*dg(m,0x8602u);
        app23_mark_special(m,item);
        if(b.app21&&b.app21->call_2a724)
            b.app21->call_2a724(load_le32(dg(m,0xC050u)),static_cast<u32>(item),variant,
                                0xC4u,dst,0x64u,0x50u);
        *dg(m,0xC050u)^=1u;
    }
}
static_assert(0x110B4u-0x10FACu==264u);

void function_110b4(Machine& m,Application23Boundary& b,u32 panel) noexcept {
    const u32 dst=panel*0x50u;
    const u32 state=panel*4u;
    for(s32 mode=3;mode>=0;--mode) {
        app23_select_page(m,b);
        *dg(m,0x6FA2u+state)=static_cast<u8>(mode);
        if(b.app22)function_10bbc(m,*b.app22,panel);
        const s32 item=static_cast<s16>(load_le16(dg(m,0x8600u)));
        const u32 variant=*dg(m,0x8602u);
        app23_mark_special(m,item);
        if(b.app21&&b.app21->call_2a724)
            b.app21->call_2a724(load_le32(dg(m,0xC050u)),static_cast<u32>(item),variant,
                                0xC4u,dst,0x64u,0x50u);
        *dg(m,0xC050u)^=1u;
    }
    *dg(m,0x6FA0u+state)=0xFFu;
    *dg(m,0x6FA1u+state)=0xFFu;
    *dg(m,0x6FA3u+state)=0xFFu;
}
static_assert(0x1118Bu-0x110B4u==215u);

void function_1118c(Machine& m,Application23Boundary& b,u32 slot) noexcept {
    const u32 dst=load_le32(dg(m,0x7F6Cu));
    if(b.app21&&b.app21->call_3975c)b.app21->call_3975c(0u,0x45u,dst+0x38u,1u,2u);
    if(b.app21&&b.app21->call_3e510)b.app21->call_3e510(0x128u,dst,4u,2u);
    const u32 actor=slot*0x248u;
    for(u32 i=0u;i<12u;++i) {
        const s32 item=static_cast<s16>(load_le16(dg(m,0x21920u+actor+i*2u)));
        if(item==-1)continue;
        const u32 rec=app23_item_rec(m,item);
        const s32 sprite=static_cast<s16>(load_le16(dg(m,rec+0x2Du)));
        if(item==0xA6) {
            if(b.app21&&b.app21->call_3975c)
                b.app21->call_3975c(0u,0x127u,dst,static_cast<u32>(sprite),1u);
            continue;
        }
        const s32 q=static_cast<s32>(i)/4;
        const s32 r=static_cast<s32>(i)%4;
        const u32 x=static_cast<u32>(0x129+q*0x22);
        const u32 y=static_cast<u32>(static_cast<s32>(dst)+5+r*0x12);
        if(b.app20)function_0f308(m,*b.app20,x,y,static_cast<u32>(sprite),static_cast<u32>(item));
    }
    if(b.app22)function_10bbc(m,*b.app22,slot);
}
static_assert(0x11268u-0x1118Cu==220u);

void function_11268(Machine& m,Application23Boundary& b,u32 slot) noexcept {
    const u32 actor=slot*0x248u;
    const u32 dst=load_le32(dg(m,0x7F6Cu));
    if((load_le16(dg(m,0x218F0u+actor))&0x8000u)!=0u) {
        if(b.app21&&b.app21->call_3e510)b.app21->call_3e510(0u,dst,2u,2u);
    } else {
        if(b.app21)function_0ff3c(m,*b.app21,slot);
        if(load_le32(dg(m,0x6F90u+slot*4u))!=0u) {
            if(b.app22)function_10158(m,*b.app22,slot);
        } else function_1118c(m,b,slot);
    }
    if(slot==load_le32(dg(m,0x7F70u))&&b.call_2e374)
        b.call_2e374(0u,slot*0x50u,0x190u,0x50u,0x1Au);
}
static_assert(0x112EAu-0x11268u==130u);

void function_112ec(Machine& m,Application23Boundary& b) noexcept {
    app23_select_page(m,b);
    for(u32 slot=0u;slot<4u;++slot) {
        store_le32(dg(m,0x7F6Cu),slot*0x50u);
        function_11268(m,b,slot);
    }
    const u32 buffer=b.app20&&b.app20->call_3104c?b.app20->call_3104c(0x1400u):0u;
    store_le32(dg(m,0x7F68u),buffer);
    if(b.call_292bc)b.call_292bc(buffer,0x62u,0u,0x10u,0x140u);
    const s32 item=static_cast<s16>(load_le16(dg(m,0x8600u)));
    const u32 variant=*dg(m,0x8602u);
    if(b.app21)function_0f790(m,*b.app21,item,variant);
    app23_draw_cursor_item(m,b,item,variant);
    if(b.call_22d70)b.call_22d70();
    *dg(m,0xD680u)=1u;
}
static_assert(0x113A9u-0x112ECu==189u);

[[nodiscard]] u32 function_113ac(u32 outer,u32 inner) noexcept {
    if(outer==0u)return inner==3u||inner==5u?1u:0u;
    if(outer==1u)return inner==1u||inner==6u||inner==2u?0u:1u;
    if(outer==2u)return inner==2u?0u:1u;
    return 0u;
}
static_assert(0x113F2u-0x113ACu==70u);

[[nodiscard]] u32 function_11404(Machine& m,Application23Boundary& b,u32 slot,u32 index,s32 candidate,u32 variant) noexcept {
    const u32 actor=slot*0x248u;
    const s32 current=static_cast<s16>(load_le16(dg(m,0x21920u+actor+index*2u)));
    const u32 current_rec=app23_item_rec(m,current);
    const s32 group=static_cast<s8>(*dg(m,current_rec+0x31u));
    const s32 goff_s=group*10;
    const u32 goff=static_cast<u32>(goff_s);
    const u32 list=load_le32(dg(m,0x7335u+goff));
    const u32 mode=*dg(m,0x7334u+goff);

    u32 candidate_rec=0u;
    if(candidate!=-1) {
        candidate_rec=app23_item_rec(m,candidate);
        const u32 type=*dg(m,candidate_rec+0x2Fu);
        if(mode>=3u&&mode<=6u) {
            static constexpr u8 expected[4]={0x0Du,0x0Eu,0x0Cu,0x0Fu};
            if(type!=expected[mode-3u])return 0u;
        }
        if(mode<2u&&(*dg(m,candidate_rec+0x35u)&0x03u)!=0u)return 0u;
        if(type==1u&&candidate!=0x50) {
            const s32 nested_group=static_cast<s8>(*dg(m,candidate_rec+0x31u));
            const u32 nested_mode=*dg(m,0x7334u+static_cast<u32>(nested_group*10));
            if(function_113ac(mode,nested_mode)==0u)return 0u;
        }
    }
    if(mode==4u&&(candidate==0x114||candidate==0x115||candidate==0x128))return 0u;

    const u32 count=*dg(m,0x7330u+goff);
    u32 free_index=0u;
    for(;free_index<count;++free_index)
        if(static_cast<s16>(load_le16(dg(m,list+free_index*2u)))==-1)break;
    if(free_index==count)return 0u;

    s32 total=static_cast<s32>(function_0f5f8(m,static_cast<u32>(group)))-static_cast<s32>(*dg(m,current_rec+0x30u));
    if(candidate!=-1) {
        if(*dg(m,candidate_rec+0x2Fu)==1u)
            total+=static_cast<s32>(function_0f5f8(m,static_cast<u32>(static_cast<s8>(*dg(m,candidate_rec+0x31u)))));
        total+=static_cast<s32>(*dg(m,candidate_rec+0x30u));
    }
    if(total>static_cast<s32>(*dg(m,0x7332u+goff))*2)return 0u;

    app23_select_page(m,b);
    store_le16(dg(m,list+free_index*2u),static_cast<u16>(candidate));
    const u8 bit=static_cast<u8>(1u<<static_cast<u8>(free_index));
    u8 flags=*dg(m,0x7339u+goff);
    flags=static_cast<u8>(flags&static_cast<u8>(~bit));
    flags=static_cast<u8>(flags|app23_shl8(static_cast<u8>(variant),free_index));
    *dg(m,0x7339u+goff)=flags;
    return 1u;
}
static_assert(0x1162Eu-0x11404u==554u);

[[nodiscard]] u32 function_11630(Machine& m,Application23Boundary& b,u32 slot,s32 item) noexcept {
    s32 ranges[12]{};
    for(u32 i=0u;i<12u;++i)ranges[i]=static_cast<s32>(load_le32(dg(m,0xEEB0u+i*4u)));
    s32 group=0;
    s32 offset=0;
    bool found=false;
    for(;group<6;++group) {
        if(item<=ranges[group*2]) { offset=item-ranges[group*2+1]; found=true; break; }
    }
    if(!found)offset=b.read_11630_stale_offset?static_cast<s32>(b.read_11630_stale_offset()):0;
    const u32 actor=slot*0x248u;
    const u32 gbase=static_cast<u32>(group*10);
    u32 used=0u;
    for(u32 i=0u;i<10u;++i)
        if((*dg(m,0x2194Au+actor+gbase+i)&0x0Fu)!=0x0Fu)++used;
    const u32 limit=*dg(m,0x6153u+*dg(m,0x21919u+actor));
    const u32 target=0x2194Au+actor+gbase+static_cast<u32>(offset);
    u32 message=0u;
    if(used==limit)message=load_le32(dg(m,0x6D9Cu));
    else if((*dg(m,target)&0x0Fu)!=0x0Fu)message=load_le32(dg(m,0x6DA4u));
    else {
        if(b.app21&&b.app21->call_2c368)b.app21->call_2c368(0x759Cu,gbase+0x42u+static_cast<u32>(offset));
        u32 dst=0x759Cu; while(*dg(m,dst)!=0u)++dst;
        u32 src=load_le32(dg(m,0x6DA0u));
        for(;;){const u8 c=*dg(m,src++);*dg(m,dst++)=c;if(c==0u)break;}
        *dg(m,0x6DACu)=1u;
        const u32 token=b.app21&&b.app21->call_40700?b.app21->call_40700():0u;
        store_le32(dg(m,0x6DA8u),token+0x36u);
        if(b.app21)function_0f790(m,*b.app21,static_cast<s16>(load_le16(dg(m,0x8600u))),*dg(m,0x8602u));
        *dg(m,target)=0u;
        return 1u;
    }
    if(b.call_4210f2)b.call_4210f2(0x759Cu,message);
    *dg(m,0x6DACu)=1u;
    const u32 token=b.app21&&b.app21->call_40700?b.app21->call_40700():0u;
    store_le32(dg(m,0x6DA8u),token+0x36u);
    if(b.app21)function_0f790(m,*b.app21,static_cast<s16>(load_le16(dg(m,0x8600u))),*dg(m,0x8602u));
    return 0u;
}
static_assert(0x117A0u-0x11630u==368u);

void function_117a0(Machine& m,Application23Boundary& b,u32 slot,s32 index,u32 allow_nested) noexcept {
    const u32 actor=slot*0x248u;
    const u32 slot_addr=actor+static_cast<u32>(index*2);
    const s32 current=static_cast<s16>(load_le16(dg(m,0x21920u+slot_addr)));
    const u32 shift=static_cast<u32>(index)&31u;
    const u32 old_variant=(load_le32(dg(m,0x21A0Bu+actor))>>shift)&1u;
    s32 selected=static_cast<s16>(load_le16(dg(m,0x8600u)));
    const u32 selected_variant=*dg(m,0x8602u);
    if(static_cast<u32>(current)>=0x7Eu&&static_cast<u32>(current)<=0x86u)return;

    const u32 had_48=function_0f2d4(m,slot,0x48u);
    if(current==0xA6&&app23_special_item_range(selected)) {
        if(function_11630(m,b,slot,selected)!=0u) {
            if(b.app20&&b.app20->call_1a344)b.app20->call_1a344(-1,0u);
            if(b.app21)function_0f790(m,*b.app21,-1,0u);
        }
        return;
    }
    if(current==0x35&&static_cast<u32>(selected)>=0x61u&&static_cast<u32>(selected)<=0x64u) {
        if(b.call_1c124)b.call_1c124(static_cast<u32>(selected-0x61));
        if(b.app20&&b.app20->call_1a344)b.app20->call_1a344(-1,0u);
        if(b.app21)function_0f790(m,*b.app21,-1,0u);
        return;
    }

    if(!(selected==-1&&current==-1)) {
        app23_select_page(m,b);
        if(selected!=-1&&current!=-1&&*dg(m,app23_item_rec(m,current)+0x2Fu)==1u&&allow_nested!=0u) {
            if(function_11404(m,b,slot,static_cast<u32>(index),selected,selected_variant)!=0u) {
                if(b.app21)function_0fed0(m,*b.app21,slot);
                if(b.app21)function_0f790(m,*b.app21,-1,0u);
                app23_draw_cursor_item(m,b,-1,0u);
                return;
            }
        }

        store_le16(dg(m,0x21920u+slot_addr),static_cast<u16>(selected));
        function_0f238(m,slot,static_cast<u32>(index),selected_variant);
        if(index<12) {
            const s32 q=index/4;
            const s32 r=index%4;
            const u32 x=static_cast<u32>(0x129+q*0x22);
            const u32 y=static_cast<u32>(static_cast<s32>(load_le32(dg(m,0x7F6Cu)))+5+r*0x12);
            if(b.app21&&b.app21->call_39b40)b.app21->call_39b40(x,y,0x20u,0x10u,0u);
            if(selected!=-1&&b.app20) {
                const s32 sprite=static_cast<s16>(load_le16(dg(m,app23_item_rec(m,selected)+0x2Du)));
                const s32 stored=static_cast<s16>(load_le16(dg(m,0x21920u+slot_addr)));
                function_0f308(m,*b.app20,x,y,static_cast<u32>(sprite),static_cast<u32>(stored));
            }
        } else if(b.app22)function_10bbc(m,*b.app22,slot);

        if(b.app21)function_0fed0(m,*b.app21,slot);
        if(b.app21)function_0f790(m,*b.app21,current,old_variant);
        app23_draw_cursor_item(m,b,current,current==-1?0u:old_variant);
    }

    if(function_0f2d4(m,slot,0x48u)==0u&&had_48==1u&&b.call_50dc) {
        const s32 first=b.call_50dc(1u,slot);
        if(first!=0) {
            const s32 second=b.call_50dc(1u,slot);
            const s32 level=static_cast<s16>(load_le16(dg(m,0x21900u+actor+static_cast<u32>(second*2))));
            if(b.call_03a78)b.call_03a78(slot,level);
        }
    }
    const s32 hand0=static_cast<s16>(load_le16(dg(m,0x2193Cu+actor)));
    const s32 hand1=static_cast<s16>(load_le16(dg(m,0x2193Eu+actor)));
    if(hand0==0x67||hand1==0x67)*dg(m,0x21A14u+actor)=static_cast<u8>(*dg(m,0x21A14u+actor)|0x40u);
    else *dg(m,0x21A14u+actor)=static_cast<u8>(*dg(m,0x21A14u+actor)&0xBFu);
}
static_assert(0x11AD8u-0x117A0u==824u);

void function_11ad8(Machine& m,Application23Boundary& b,u32 slot,u32 flags) noexcept {
    if((flags&2u)!=0u) {
        u32 mode=load_le32(dg(m,0x6F90u+slot*4u));
        mode=(mode&0xFFFF0000u)|static_cast<u16>(static_cast<u16>(mode)^1u);
        app23_select_page(m,b);
        for(u32 i=0u;i<4u;++i) {
            store_le32(dg(m,0x6F90u+i*4u),mode);
            store_le32(dg(m,0x7F6Cu),i*0x50u);
            function_11268(m,b,i);
        }
    } else {
        *dg(m,0x6F90u+slot*4u)^=1u;
        app23_select_page(m,b);
        function_11268(m,b,slot);
    }
    const s32 item=static_cast<s16>(load_le16(dg(m,0x8600u)));
    const u32 variant=*dg(m,0x8602u);
    if(b.app21)function_0f790(m,*b.app21,item,variant);
    if((flags&2u)!=0u)app23_draw_cursor_item(m,b,item,variant);
    else {
        app23_mark_special(m,item);
        if(b.app21&&b.app21->call_2a724)
            b.app21->call_2a724(load_le32(dg(m,0xC050u)),static_cast<u32>(item),variant,
                                0u,load_le32(dg(m,0x7F6Cu)),0x190u,0x50u);
        *dg(m,0xC050u)^=1u;
    }
}
static_assert(0x11C20u-0x11AD8u==328u);

[[nodiscard]] u32 function_11c20(u32 flags) noexcept { return (flags&2u)==0u?1u:0u; }
static_assert(0x11C2Cu-0x11C20u==12u);
// -----------------------------------------------------------------------------
// Batch 24: 11C3C..12985 container-grid placement, inventory/equipment hit
// testing, party-slot swapping, and status-list modal presentation.
// The six-entry CS table at 12168 and compiler separators are non-code/data.
// -----------------------------------------------------------------------------

struct Application24Boundary {
    Application20Boundary* app20=nullptr;
    Application21Boundary* app21=nullptr;
    Application22Boundary* app22=nullptr;
    Application23Boundary* app23=nullptr;
    void (*call_1a5b0)(u32,u32,u32)=nullptr;
    u32 (*call_23eac)(u32,u32)=nullptr;
    u32 (*call_1a8c8)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_30cd4)(u32,u32,u32)=nullptr;
    void (*call_1a724)(u32,u32,u32,u32,u32,u32,u32)=nullptr;
};

static u32 app24_item_rec(Machine& m,s32 item) noexcept {
    return load_le32(dg(m,0x255D8u))+static_cast<u32>(item*65);
}
static s32 app24_div(s32 n,s32 d) noexcept { return n/d; }
static u32 app24_grid_bit(u8 bits,s32 index) noexcept {
    const u32 count=static_cast<u32>(index)&31u;
    if(count>=8u)return 0u;
    return (static_cast<u32>(bits)>>count)&1u;
}
static void app24_draw_cursor(Machine& m,Application24Boundary& b,s32 item,u32 variant) noexcept {
    app23_mark_special(m,item);
    if(b.call_1a5b0)b.call_1a5b0(load_le32(dg(m,0xC050u)),static_cast<u32>(item),variant);
    *dg(m,0xC050u)^=1u;
}

[[nodiscard]] u32 function_11c3c(Machine& m,Application24Boundary& b,u32 panel,s32 x,s32 y,u32 flags) noexcept {
    if((flags&2u)!=0u||load_le32(dg(m,0x6F90u+panel*4u))!=0u)return 0u;
    const s32 group=static_cast<s8>(*dg(m,0x6FA1u+panel*4u));
    const s32 g10=group*10;
    const u32 list=load_le32(dg(m,static_cast<u32>(static_cast<s32>(0x7335)+g10)));
    const s32 top=*dg(m,static_cast<u32>(static_cast<s32>(0x7333)+g10));
    const s32 width=*dg(m,static_cast<u32>(static_cast<s32>(0x7331)+g10));
    if(y<top||y>top+width*0x12)return 1u;

    const s32 selected=static_cast<s16>(load_le16(dg(m,0x8600u)));
    const u32 selected_variant=*dg(m,0x8602u);
    const u8 kind=*dg(m,static_cast<u32>(static_cast<s32>(0x7334)+g10));
    if(selected!=-1&&kind>=3u&&kind<=6u) {
        const u8 expected[4]={0x0Du,0x0Eu,0x0Cu,0x0Fu};
        if(*dg(m,app24_item_rec(m,selected)+0x2Fu)!=expected[kind-3u])return 1u;
    }
    if(kind<2u&&selected!=-1) {
        const u8 low=static_cast<u8>(load_le16(dg(m,app24_item_rec(m,selected)+0x35u)))&3u;
        if(low>=2u)return 1u;
    }
    if(selected!=-1) {
        const u32 rec=app24_item_rec(m,selected);
        if(*dg(m,rec+0x2Fu)==1u) {
            const s32 nested=static_cast<s8>(*dg(m,rec+0x31u));
            if(*dg(m,static_cast<u32>(static_cast<s32>(0x7334)+nested*10))==kind)return 1u;
        }
    }

    const s32 column=app24_div(x-0xDB,0x22);
    const s32 row=app24_div(y-top,0x12);
    if(row>=width)return 1u;
    const s32 index=column*width+row;
    const s32 count=*dg(m,static_cast<u32>(static_cast<s32>(0x7330)+g10));
    if(index>=count)return 1u;
    const u32 entry=list+static_cast<u32>(index*2);
    const s32 old_item=static_cast<s16>(load_le16(dg(m,entry)));
    const u32 old_variant=app24_grid_bit(*dg(m,static_cast<u32>(static_cast<s32>(0x7339)+g10)),index);
    if(selected==-1&&old_item==-1)return 1u;

    u32 burden=function_0f5f8(m,static_cast<u32>(group));
    const s32 anchor=static_cast<s8>(*dg(m,0x6FA3u+panel*4u));
    burden-=*dg(m,app24_item_rec(m,anchor)+0x30u);
    if(old_item!=-1) {
        const u32 rec=app24_item_rec(m,old_item);
        if(*dg(m,rec+0x2Fu)==1u)burden-=function_0f5f8(m,static_cast<u32>(static_cast<s8>(*dg(m,rec+0x31u))));
        burden-=*dg(m,rec+0x30u);
    }
    if(selected!=-1) {
        const u32 rec=app24_item_rec(m,selected);
        if(*dg(m,rec+0x2Fu)==1u)burden+=function_0f5f8(m,static_cast<u32>(static_cast<s8>(*dg(m,rec+0x31u))));
        burden+=*dg(m,rec+0x30u);
    }
    const u32 capacity=static_cast<u32>(*dg(m,static_cast<u32>(static_cast<s32>(0x7332)+g10)))*2u;
    if(static_cast<s32>(burden)>static_cast<s32>(capacity))return 1u;

    if(b.app23)app23_select_page(m,*b.app23);
    store_le16(dg(m,entry),static_cast<u16>(selected));
    const u32 shift=static_cast<u32>(index)&31u;
    if(shift<8u) {
        u8& bits=*dg(m,static_cast<u32>(static_cast<s32>(0x7339)+g10));
        bits=static_cast<u8>(bits&~static_cast<u8>(1u<<shift));
        bits=static_cast<u8>(bits|static_cast<u8>((selected_variant&1u)<<shift));
    }
    if(b.app22)function_10bbc(m,*b.app22,panel);
    if(b.app21)function_0f790(m,*b.app21,old_item,old_item==-1?0u:old_variant);
    app24_draw_cursor(m,b,old_item,old_item==-1?0u:old_variant);
    return 1u;
}
static_assert(0x11FAEu-0x11C3Cu==882u);

[[nodiscard]] u32 function_11fb0(Machine& m,Application24Boundary& b,u32 panel,s32 x,s32 y,u32 flags) noexcept {
    if((flags&2u)!=0u||load_le32(dg(m,0x6F90u+panel*4u))!=0u)return 0u;
    const s32 a=app24_div(x-0x129,0x22);
    const s32 c=app24_div(y-5,0x12);
    const s32 index=(a%0x20)*4+(c%0x10);
    if(b.app23)function_117a0(m,*b.app23,panel,index,1u);
    return 1u;
}
static_assert(0x1202Au-0x11FB0u==122u);

[[nodiscard]] s32 function_1202c(Machine& m,u32 slot,s32 amount) noexcept {
    const u32 actor=slot*0x248u;
    if(static_cast<s16>(load_le16(dg(m,0x21948u+actor)))==0x54) {
        const s32 old=static_cast<s32>(load_le32(dg(m,0x21A96u+actor)));
        if(old+amount>0x14)amount=0x14-old;
        store_le32(dg(m,0x21A96u+actor),load_le32(dg(m,0x21A96u+actor))+static_cast<u32>(amount));
    } else {
        const s32 old=*dg(m,0x21913u+actor);
        if(old+amount>0x14)amount=0x14-old;
        *dg(m,0x21913u+actor)=static_cast<u8>(*dg(m,0x21913u+actor)+static_cast<u8>(amount));
    }
    return amount;
}
static_assert(0x12097u-0x1202Cu==107u);

[[nodiscard]] u32 function_12098(Machine& m,u32 slot) noexcept {
    const u32 actor=slot*0x248u;
    if(static_cast<s16>(load_le16(dg(m,0x21948u+actor)))==0x54)return load_le32(dg(m,0x21A96u+actor));
    return *dg(m,0x21913u+actor);
}
static_assert(0x120BBu-0x12098u==35u);

void function_120bc(Machine& m,u32 slot,s32 old_item,s32 new_item) noexcept {
    const u32 actor=slot*0x248u;
    if(old_item==0x54)*dg(m,0x21913u+actor)=*dg(m,0x21A96u+actor);
    if(new_item==0x54) {
        const u32 old=*dg(m,0x21913u+actor);
        *dg(m,0x21913u+actor)=0x14u;
        store_le32(dg(m,0x21A96u+actor),old);
    }
    if(old_item==0x53)--*dg(m,0x21915u+actor);
    if(new_item==0x53)++*dg(m,0x21915u+actor);
}
static_assert(0x12125u-0x120BCu==105u);

[[nodiscard]] u32 function_12128(u32 item) noexcept {
    return item==0x10u||item==0x5Fu||item==0x0Cu||item==0xECu||item==0xEEu?1u:0u;
}
static_assert(0x12165u-0x12128u==61u);

[[nodiscard]] u32 function_12180(Machine& m,Application24Boundary& b,u32 slot,s32 x,s32 y,u32 flags) noexcept {
    if((flags&2u)!=0u||load_le32(dg(m,0x6F90u+slot*4u))!=0u)return 0u;
    const u32 actor=slot*0x248u;
    const s32 cursor=static_cast<s16>(load_le16(dg(m,0x8600u)));
    s32 target=-1;
    if(cursor!=-1) {
        const u8 type=*dg(m,app24_item_rec(m,cursor)+0x2Fu);
        switch(type) {
            case 3u: target=0x11; break;
            case 4u: {
                target=0x10;
                if(static_cast<s8>(*dg(m,0x218F5u+actor))==4&&static_cast<s8>(*dg(m,0x218F6u+actor))==-1)target=-1;
                const u32 gate=b.call_23eac?b.call_23eac(slot,5u):0u;
                if(gate!=0u&&function_12128(static_cast<u32>(cursor))==0u)target=-1;
                break;
            }
            case 5u: target=y<=0x28?0x0C:0x0D; break;
            case 6u: target=0x12; break;
            case 7u: target=0x14; break;
            case 8u: target=0x10; break;
            default:
                if(y<=0x28)target=function_0f168(static_cast<u32>(cursor))!=0u?0x13:0x0E;
                else target=0x0F;
                break;
        }
    } else {
        const u32 table=load_le32(dg(m,0x12C00u));
        for(u32 off=0u;off<0xF0u;off+=0x18u) {
            const u32 e=table+off;
            if(x<static_cast<s32>(load_le32(dg(m,e+8u)))||x>static_cast<s32>(load_le32(dg(m,e+0x10u))))continue;
            if(y<static_cast<s32>(load_le32(dg(m,e+0x0Cu)))||y>static_cast<s32>(load_le32(dg(m,e+0x14u))))continue;
            if((load_le32(dg(m,e+4u))&flags)==0u)continue;
            const s32 index=static_cast<s32>(load_le32(dg(m,e)));
            if(static_cast<s16>(load_le16(dg(m,0x21920u+actor+static_cast<u32>(index*2))))==-1)continue;
            target=index;
            break;
        }
    }
    if(target==-1)return 0u;
    const s32 existing=static_cast<s16>(load_le16(dg(m,0x21920u+actor+static_cast<u32>(target*2))));
    if(static_cast<u32>(existing)>=0x7Eu&&static_cast<u32>(existing)<=0x86u)return 0u;

    if(target==0x0E||target==0x0F) {
        if(target==0x0F&&existing!=-1&&*dg(m,app24_item_rec(m,existing)+0x2Fu)==1u&&b.app23)
            function_110b4(m,*b.app23,slot);
        if(b.app23)function_117a0(m,*b.app23,slot,target,0u);
        if(target==0x0F&&cursor!=-1) {
            const u32 rec=app24_item_rec(m,cursor);
            if(*dg(m,rec+0x2Fu)==1u&&b.app23) {
                const s32 nested=static_cast<s8>(*dg(m,rec+0x31u));
                const u32 kind=*dg(m,static_cast<u32>(static_cast<s32>(0x7334)+nested*10));
                function_10fac(m,*b.app23,slot,kind,static_cast<u32>(nested),static_cast<u32>(cursor));
            }
        }
    } else {
        if(target==0x14)function_120bc(m,slot,existing,cursor);
        if(b.app23)function_117a0(m,*b.app23,slot,target,1u);
    }
    return 1u;
}
static_assert(0x1246Fu-0x12180u==751u);

static u16 app24_replace_actor_bits(u16 word,u32 from,u32 to) noexcept {
    const u32 encoded=(word>>11u)&7u;
    if(encoded==static_cast<u16>(from))return static_cast<u16>((word&0xC7FFu)|((to&7u)<<11u));
    if(encoded==static_cast<u16>(to))return static_cast<u16>((word&0xC7FFu)|((from&7u)<<11u));
    return word;
}

void function_12470(Machine& m,Application24Boundary& b,u32 source) noexcept {
    if(b.app23)app23_select_page(m,*b.app23);
    store_le32(dg(m,0x7F70u),source);
    if(b.app23)function_11268(m,*b.app23,source);
    const u32 source_dst=source*0x50u;
    if(b.call_1a8c8)b.call_1a8c8(load_le32(dg(m,0xC050u)),0u,source_dst,0x190u,0x50u);
    while(b.app20&&b.app20->call_1a1f4&&b.app20->call_1a1f4()!=0u){}
    while(b.app20&&b.app20->call_1a1f4&&b.app20->call_1a1f4()==0u){}
    const s32 target=app24_div(static_cast<s16>(load_le16(dg(m,0x9E33u))),0x50);
    store_le32(dg(m,0x7F70u),0xFFFFFFFFu);
    if(target==static_cast<s32>(source)) {
        if(b.app23)function_11268(m,*b.app23,source);
        if(b.call_1a8c8)b.call_1a8c8(load_le32(dg(m,0xC050u)),0u,source_dst,0x190u,0x50u);
        if(b.app23&&b.app23->call_22d70)b.app23->call_22d70();
        return;
    }

    const u32 target_u=static_cast<u32>(target);
    const u32 temp=b.app20&&b.app20->call_3104c?b.app20->call_3104c(0x248u):0u;
    const u32 source_actor=0x218E0u+source*0x248u;
    const u32 target_actor=0x218E0u+target_u*0x248u;
    if(b.call_30cd4) {
        b.call_30cd4(temp,source_actor,0x248u);
        b.call_30cd4(source_actor,target_actor,0x248u);
        b.call_30cd4(target_actor,temp,0x248u);
    }
    if(b.app20&&b.app20->call_310b0)b.app20->call_310b0(temp);

    const u32 so=source*4u, to=target_u*4u;
    const u32 sv=load_le32(dg(m,0x6F90u+so)), tv=load_le32(dg(m,0x6F90u+to));
    store_le32(dg(m,0x6F90u+so),tv); store_le32(dg(m,0x6F90u+to),sv);
    for(u32 i=0u;i<4u;++i) {
        const u8 a=*dg(m,0x6FA0u+so+i), c=*dg(m,0x6FA0u+to+i);
        *dg(m,0x6FA0u+so+i)=c; *dg(m,0x6FA0u+to+i)=a;
    }

    store_le32(dg(m,0x7F6Cu),source*0x50u);
    if(b.app23)function_11268(m,*b.app23,source);
    if(b.call_1a8c8)b.call_1a8c8(load_le32(dg(m,0xC050u)),0u,source*0x50u,0x190u,0x50u);
    store_le32(dg(m,0x7F6Cu),target_u*0x50u);
    if(b.app23)function_11268(m,*b.app23,target_u);
    if(b.call_1a8c8)b.call_1a8c8(load_le32(dg(m,0xC050u)),0u,target_u*0x50u,0x190u,0x50u);

    const s32 count1=static_cast<s32>(load_le32(dg(m,0xC038u)));
    if(count1>0)for(s32 i=0;i<count1;++i) {
        const u32 p=0x24900u+static_cast<u32>(i)*0x16u;
        store_le16(dg(m,p),app24_replace_actor_bits(load_le16(dg(m,p)),source,target_u));
    }
    const s32 count2=static_cast<s16>(load_le16(dg(m,0x258A4u)));
    for(s32 i=0;i<count2;++i) {
        const u32 p=0x1762Cu+static_cast<u32>(i)*0x16u;
        store_le16(dg(m,p),app24_replace_actor_bits(load_le16(dg(m,p)),source,target_u));
    }
    if(b.app23&&b.app23->call_22d70)b.app23->call_22d70();
}
static_assert(0x12723u-0x12470u==691u);

void function_12724(Machine& m,Application24Boundary& b,u32 slot) noexcept {
    const u32 actor=slot*0x248u;
    *dg(m,0x6DACu)=0u;
    if(load_le32(dg(m,0x21A12u+actor))==0u)return;

    if(b.app23)app23_select_page(m,*b.app23);
    if(b.app21)function_0ff3c(m,*b.app21,slot);
    if(b.app21&&b.app21->call_3e510)b.app21->call_3e510(0x73u,load_le32(dg(m,0x7F6Cu)),0x25u,2u);
    if(load_le32(dg(m,0x6F90u+slot*4u))==0u&&b.app21&&b.app21->call_3975c)
        b.app21->call_3975c(0u,0x45u,load_le32(dg(m,0x7F6Cu))+0x38u,1u,2u);

    u32 bit=1u;
    u32 resource=0x24Cu;
    u32 sprite_index=2u;
    u32 y_sprite=0x88u;
    u32 y_text=0x8Cu;
    u32 shown=0u;
    while(shown<14u&&bit<=0x02000000u) {
        if((load_le32(dg(m,0x21A12u+actor))&bit)!=0u) {
            if(b.app20&&b.app20->call_1c368)b.app20->call_1c368(0x759Cu,resource);
            if(b.app20&&b.app20->call_2975c)b.app20->call_2975c(sprite_index,y_sprite,load_le32(dg(m,0x7F6Cu))+7u,
                                                                 load_le32(dg(m,0x258DCu))+0x4Du,0u);
            if(b.app20&&b.app20->call_3060c)b.app20->call_3060c(y_text,load_le32(dg(m,0x7F6Cu))+0x10u,0x759Cu,1u);
            ++shown;
        }
        ++resource; ++sprite_index; y_sprite+=0x12u; y_text+=0x12u; bit+=bit;
    }

    const s32 item=static_cast<s16>(load_le16(dg(m,0x8600u)));
    const u32 variant=*dg(m,0x8602u);
    if(b.app21)function_0f790(m,*b.app21,item,variant);
    app23_mark_special(m,item);
    if(b.call_1a724)b.call_1a724(load_le32(dg(m,0xC050u)),static_cast<u32>(item),variant,
                                  0x73u,slot*0x50u,0x11Du,0x50u);
    *dg(m,0xC050u)^=1u;
    while(b.app20&&b.app20->call_1a1f4&&b.app20->call_1a1f4()!=0u){}
    while(b.app20&&b.app20->call_1a1f4&&b.app20->call_1a1f4()==0u){}
    if(b.app23)app23_select_page(m,*b.app23);
    if(b.app23)function_11268(m,*b.app23,slot);

    const s32 item2=static_cast<s16>(load_le16(dg(m,0x8600u)));
    const u32 variant2=*dg(m,0x8602u);
    if(b.app21)function_0f790(m,*b.app21,item2,variant2);
    app23_mark_special(m,item2);
    if(b.call_1a724)b.call_1a724(load_le32(dg(m,0xC050u)),static_cast<u32>(item2),variant2,
                                  0u,load_le32(dg(m,0x7F6Cu)),0x190u,0x50u);
    *dg(m,0xC050u)^=1u;
    while(b.app20&&b.app20->call_1a1f4&&b.app20->call_1a1f4()!=0u){}
    if(b.app23&&b.app23->call_22d70)b.app23->call_22d70();
}
static_assert(0x12986u-0x12724u==610u);

// -----------------------------------------------------------------------------
// Batch 25: 12988..139AA modal character panels, pointer hit routing, main
// inventory loop, compact stream decoders, and ten-way cinematic packet dispatch.
// The CINE00.ANM literal and CS table at 1359C are data in RESIDUAL_LEDGER.md.
// -----------------------------------------------------------------------------

struct Application25Boundary {
    Application21Boundary* app21=nullptr;
    Application23Boundary* app23=nullptr;
    Application24Boundary* app24=nullptr;
    void (*call_291b0)(u32)=nullptr;
    void (*call_291c8)(u32)=nullptr;
    void (*call_2e510)(u32,u32,u32,u32)=nullptr;
    void (*call_2975c)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_1c368)(u32,u32)=nullptr;
    u32 (*call_2e6f0)(s32)=nullptr;
    void (*call_3060c)(u32,u32,u32,u32)=nullptr;
    void (*call_305a8)(u32,u32,u32,u32)=nullptr;
    void (*call_30508)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_1a724)(u32,u32,u32,u32,u32,u32,u32)=nullptr;
    u32 (*call_1a1f4)()=nullptr;
    void (*call_22d70)()=nullptr;
    void (*call_310b0)(u32)=nullptr;
    void (*call_1f8fc)()=nullptr;
    void (*call_1ae30)()=nullptr;
    void (*call_1a8c8)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_1a344)(s32,u32)=nullptr;
    void (*call_19ccc)(u32)=nullptr;
    void (*call_19cf4)()=nullptr;
    u32 (*call_30b40)()=nullptr;
    u32 (*call_30700)()=nullptr;
    void (*call_2195c)(u32,u32,u32,u32,u32,u32,u32)=nullptr;
    void (*call_22d8c)()=nullptr;
    void (*call_30cd4)(u32,u32,u32)=nullptr;
    u32 (*call_3131c)(u32,u32,u32)=nullptr;
    void (*call_27a90)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_29aa4)(u32,u32,u32)=nullptr;
    void (*call_29b40)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_30d30)(u32,u32,u32)=nullptr;
    void (*call_29234)(u32)=nullptr;
    void (*call_27bd8)(u32)=nullptr;
    void (*call_27aec)(u32)=nullptr;
    void (*call_27b84)(u32)=nullptr;
    void (*call_28dcc)(u32)=nullptr;
    void (*call_28ef0)()=nullptr;
    void (*call_29008)(u32)=nullptr;
    void (*call_28f58)(u32)=nullptr;
    void (*call_28e58)(s32,s32,u32)=nullptr;
    void (*call_28110)(u32,u32,u32,u32)=nullptr;
    void (*call_146b8)(s32)=nullptr;
    void (*call_312ec)(u32,u32,u32)=nullptr;
    u32 (*read_312ec_value)(u32,u32)=nullptr;
    void (*call_13fd4)(u32,u32,u32,u32,u32,u32,u32)=nullptr;
    void (*call_2a020)(u32)=nullptr;
    void (*call_28168)()=nullptr;
    void (*call_2e608)()=nullptr;
    void (*call_2f868)()=nullptr;
    void (*call_2e614)(u32,u32)=nullptr;
};

static void app25_select_page(Machine& m,Application25Boundary& b) noexcept {
    if(b.call_291b0)b.call_291b0(*dg(m,0xC050u));
}
static void app25_copy(Machine& m,Application25Boundary& b,u32 dst,u32 src,u32 n) noexcept {
    if(b.call_30cd4){b.call_30cd4(dst,src,n);return;}
    for(u32 i=0u;i<n;++i)*dg(m,dst+i)=*dg(m,src+i);
}
static void app25_draw_cursor(Machine& m,Application25Boundary& b,s32 item,u32 variant,
                              u32 x,u32 dst,u32 w,u32 h) noexcept {
    app23_mark_special(m,item);
    if(b.call_1a724)b.call_1a724(load_le32(dg(m,0xC050u)),static_cast<u32>(item),variant,x,dst,w,h);
    *dg(m,0xC050u)^=1u;
}
static void app25_wait_release(Application25Boundary& b) noexcept {
    while(b.call_1a1f4&&b.call_1a1f4()!=0u){}
}
static void app25_wait_press(Application25Boundary& b) noexcept {
    while(b.call_1a1f4&&b.call_1a1f4()==0u){}
}

void function_12988(Machine& m,Application25Boundary& b,u32 slot) noexcept {
    *dg(m,0x6DACu)=0u;
    app25_select_page(m,b);
    if(b.app21)function_0ff3c(m,*b.app21,slot);
    const u32 dst=load_le32(dg(m,0x7F6Cu));
    if(b.call_2e510)b.call_2e510(0x73u,dst,0x25u,2u);
    if(load_le32(dg(m,0x6F90u+slot*4u))==0u&&b.call_2975c)
        b.call_2975c(0u,0x45u,dst+0x38u,1u,2u);

    const u32 burden=function_0fd84(m,slot);
    const u32 rank=function_0fe44(m,slot);
    const u32 limit=function_0fde4(m,slot);
    if(b.call_1c368)b.call_1c368(0x759Cu,0x266u);
    if(b.call_3060c)b.call_3060c(0x88u,dst+7u,0x759Cu,1u);
    if(b.call_305a8)b.call_305a8(0x88u,dst+0x45u,b.call_2e6f0?b.call_2e6f0(static_cast<s32>(limit)):0u,1u);
    if(b.call_1c368)b.call_1c368(0x759Cu,0x267u);
    if(b.call_3060c)b.call_3060c(0x96u,dst+7u,0x759Cu,1u);
    if(b.call_305a8)b.call_305a8(0x96u,dst+0x45u,b.call_2e6f0?b.call_2e6f0(static_cast<s32>(burden)):0u,1u);
    if(b.call_1c368)b.call_1c368(0x759Cu,rank+0x268u);
    if(b.call_30508)b.call_30508(0xA4u,dst+6u,0x3Cu,0x759Cu,1u);

    s32 item=static_cast<s16>(load_le16(dg(m,0x8600u)));
    u32 variant=*dg(m,0x8602u);
    if(b.app21)function_0f790(m,*b.app21,item,variant);
    app25_draw_cursor(m,b,item,variant,0x73u,slot*0x50u,0x11Du,0x50u);
    app25_wait_release(b);
    app25_wait_press(b);
    app25_select_page(m,b);
    if(b.app23)function_11268(m,*b.app23,slot);

    item=static_cast<s16>(load_le16(dg(m,0x8600u)));
    variant=*dg(m,0x8602u);
    if(b.app21)function_0f790(m,*b.app21,item,variant);
    app25_draw_cursor(m,b,item,variant,0u,load_le32(dg(m,0x7F6Cu)),0x190u,0x50u);
    app25_wait_release(b);
    if(b.call_22d70)b.call_22d70();
}
static_assert(0x12C1Du-0x12988u==661u);

void function_12c20(Machine& m,Application25Boundary& b,u32 slot) noexcept {
    const u32 actor=slot*0x248u;
    *dg(m,0x6DACu)=0u;
    if((load_le16(dg(m,0x218F0u+actor))&0x1F00u)==0u)return;
    app25_select_page(m,b);
    if(b.app21)function_0ff3c(m,*b.app21,slot);
    const u32 dst=load_le32(dg(m,0x7F6Cu));
    if(b.call_2e510)b.call_2e510(0x73u,dst,0x25u,2u);
    if(load_le32(dg(m,0x6F90u+slot*4u))==0u&&b.call_2975c)
        b.call_2975c(0u,0x45u,dst+0x38u,1u,2u);

    u32 bit=0x100u, resource=0x28Du, sprite=0x1Cu, y_sprite=0x88u, y_text=0x8Cu;
    while(bit<=0x1000u) {
        if((load_le16(dg(m,0x218F0u+actor))&bit)!=0u) {
            if(b.call_1c368)b.call_1c368(0x759Cu,resource);
            if(b.call_2975c)b.call_2975c(sprite,y_sprite,dst+7u,load_le32(dg(m,0x258DCu))+0x4Du,0u);
            if(b.call_3060c)b.call_3060c(y_text,dst+0x10u,0x759Cu,1u);
            y_sprite+=0x12u; y_text+=0x12u;
        }
        ++resource; bit+=bit; ++sprite;
    }

    s32 item=static_cast<s16>(load_le16(dg(m,0x8600u)));
    u32 variant=*dg(m,0x8602u);
    if(b.app21)function_0f790(m,*b.app21,item,variant);
    app25_draw_cursor(m,b,item,variant,0x73u,slot*0x50u,0x11Du,0x50u);
    app25_wait_release(b);
    app25_wait_press(b);
    app25_select_page(m,b);
    if(b.app23)function_11268(m,*b.app23,slot);

    item=static_cast<s16>(load_le16(dg(m,0x8600u)));
    variant=*dg(m,0x8602u);
    if(b.app21)function_0f790(m,*b.app21,item,variant);
    app25_draw_cursor(m,b,item,variant,0u,load_le32(dg(m,0x7F6Cu)),0x190u,0x50u);
    app25_wait_release(b);
    if(b.call_22d70)b.call_22d70();
}
static_assert(0x12E6Fu-0x12C20u==591u);

void function_12e70(Machine& m,Application25Boundary& b,s32 x,s32 y,u32 flags) noexcept {
    s32 slot=y/0x50;
    const u32 slotu=static_cast<u32>(slot);
    store_le32(dg(m,0x7F6Cu),slotu*0x50u);
    y-=slot*0x50;
    const u32 actor=slotu*0x248u;
    if((load_le16(dg(m,0x218F0u+actor))&0x8000u)!=0u)goto cleanup;

    store_le32(dg(m,0x12C00u),function_0f368(m,slotu)==0u?0x6EA0u:0x6DB0u);
    if(static_cast<s8>(*dg(m,0x6FA0u+slotu*4u))!=-1&&x>=0xDB&&x<=0x11F&&y>=5&&y<=0x4D) {
        if(b.app24&&function_11c3c(m,*b.app24,slotu,x,y,flags)!=0u)return;
    }
    if(x<0x10) {
        if(b.app24)function_12470(m,*b.app24,slotu);
        flags=0u;
        store_le32(dg(m,0x7F6Cu),slotu*0x50u);
    }
    if(x>=0x10&&x<=0x5F&&y>=3&&y<=0x32&&(flags&1u)!=0u) {
        if((flags&2u)==0u)return;
        goto cleanup;
    }
    if(x>=0x45&&x<=0x60&&y>=0x39&&y<=0x4D) {
        if(b.app23)function_11ad8(m,*b.app23,slotu,flags);
        return;
    }
    if(x>=0x129&&x<=0x18C&&y>=5&&y<=0x4A&&(flags&1u)!=0u) {
        if(b.app24&&function_11fb0(m,*b.app24,slotu,x,y,flags)!=0u)return;
        goto cleanup;
    }
    if(x>=0x6E&&x<=0x123&&y>=2&&y<=0x4D&&(flags&1u)!=0u) {
        if(b.app24&&function_12180(m,*b.app24,slotu,x,y,flags)!=0u)return;
        goto cleanup;
    }
    if(x>=0x10&&x<=0x27&&y>=0x44&&y<=0x4D&&(flags&1u)!=0u) {
        function_12988(m,b,slotu);
        goto cleanup;
    }
    if(x>=0x2B&&x<=0x40&&y>=0x39&&y<=0x42&&(flags&1u)!=0u) {
        function_12c20(m,b,slotu);
        goto cleanup;
    }
    if(x>=0x2B&&x<=0x40&&y>=0x44&&y<=0x4D&&(flags&1u)!=0u) {
        if(b.app24)function_12724(m,*b.app24,slotu);
    }
cleanup:
    if((flags&2u)!=0u) {
        if(b.call_310b0)b.call_310b0(load_le32(dg(m,0x7F68u)));
        if(b.call_1f8fc)b.call_1f8fc();
        if(b.call_1ae30)b.call_1ae30();
    }
}
static_assert(0x1309Fu-0x12E70u==559u);

void function_130a0(Machine& m,Application25Boundary& b) noexcept {
    s32 previous=-1;
    s32 selected=0;
    for(;;) {
        const u32 event=b.call_1a1f4?b.call_1a1f4():1u;
        if(event!=0u) {
            if((event&1u)!=0u&&b.call_1a344)b.call_1a344(selected,0u);
            return;
        }
        selected=static_cast<s16>(load_le16(dg(m,0x9E33u)));
        if(selected==previous)continue;
        const s32 maximum=static_cast<s32>(load_le32(dg(m,0x258E8u)));
        if(selected>maximum)selected=maximum;
        app25_select_page(m,b);
        if(b.app21)function_0f790(m,*b.app21,selected,0u);
        if(b.call_1a8c8)b.call_1a8c8(load_le32(dg(m,0xC050u)),0x62u,0u,0x10u,0x140u);
        previous=selected;
    }
}
static_assert(0x13118u-0x130A0u==120u);

void function_13118(Machine& m,Application25Boundary& b) noexcept {
    store_le32(dg(m,0x6DA8u),0u);
    *dg(m,0x6DACu)=0u;
    if(b.call_19ccc)b.call_19ccc(0u);
    if(b.app23)function_112ec(m,*b.app23);
    for(;;) {
        if(*dg(m,0xD680u)!=1u) { if(b.call_19cf4)b.call_19cf4(); return; }
        const u32 key=b.call_30b40?b.call_30b40():0u;
        if(key==0x44u) {
            const s32 item=static_cast<s16>(load_le16(dg(m,0x8600u)));
            if(item!=-1&&b.call_2195c)
                b.call_2195c(0x80u,static_cast<u32>(item),*dg(m,0x8602u),
                              load_le32(dg(m,0x255D0u)),load_le32(dg(m,0x255D4u)),1u,1u);
        }
        if(*dg(m,0x6DACu)!=0u) {
            const u32 now=b.call_30700?b.call_30700():0u;
            if(now>load_le32(dg(m,0x6DA8u))) {
                store_le32(dg(m,0x6DA8u),0u); *dg(m,0x6DACu)=0u;
                if(b.app21)function_0f790(m,*b.app21,static_cast<s16>(load_le16(dg(m,0x8600u))),*dg(m,0x8602u));
            }
        }
        if(b.call_22d8c)b.call_22d8c();
    }
}
static_assert(0x131CDu-0x13118u==181u);

void function_131dc(Machine& m,Application25Boundary& b) noexcept {
    const s32 count=static_cast<s16>(load_le16(dg(m,0x258A6u)));
    for(s32 i=0;i<count;++i) {
        const u32 p=0x22238u+static_cast<u32>(i)*0x16u;
        if(static_cast<s16>(load_le16(dg(m,p+2u)))!=0x1F)continue;
        if(static_cast<s16>(load_le16(dg(m,p+6u)))!=0x0D)continue;
        if(load_le16(dg(m,p+0x0Eu))!=0x3Cu)continue;
        const u32 bytes=static_cast<u32>(count-i-1)*0x16u;
        app25_copy(m,b,p,p+0x16u,bytes);
        store_le16(dg(m,0x258A6u),static_cast<u16>(load_le16(dg(m,0x258A6u))-1u));
        return;
    }
}
static_assert(0x13245u-0x131DCu==105u);

[[nodiscard]] u32 function_13248(Machine& m,Application25Boundary& b) noexcept {
    const u32 base=b.call_3131c?b.call_3131c(0u,1u,load_le32(dg(m,0x7F7Cu))):0u;
    return base-0x1F400u+load_le32(dg(m,0x7F88u));
}
static_assert(0x13266u-0x13248u==30u);

[[nodiscard]] u32 function_13268(Machine& m) noexcept {
    const u32 pos=load_le32(dg(m,0x7F88u));
    store_le32(dg(m,0x7F88u),pos+1u);
    const u8 value=*dg(m,load_le32(dg(m,0x7F84u))+pos);
    return (pos&0xFFFFFF00u)|value;
}
static_assert(0x13282u-0x13268u==26u);

[[nodiscard]] u32 function_13284(Machine& m) noexcept {
    const u32 p=load_le32(dg(m,0x7F84u))+load_le32(dg(m,0x7F88u));
    store_le32(dg(m,0x7F88u),load_le32(dg(m,0x7F88u))+2u);
    return (p&0xFFFF0000u)|load_le16(dg(m,p));
}
static_assert(0x1329Au-0x13284u==22u);

[[nodiscard]] u32 function_1329c(Machine& m) noexcept {
    const u32 p=load_le32(dg(m,0x7F84u))+load_le32(dg(m,0x7F88u));
    store_le32(dg(m,0x7F88u),load_le32(dg(m,0x7F88u))+4u);
    return load_le32(dg(m,p));
}
static_assert(0x132B1u-0x1329Cu==21u);

void function_132b4(Machine& m,Application25Boundary& b,u32 dst,u32 n) noexcept {
    app25_copy(m,b,dst,load_le32(dg(m,0x7F84u))+load_le32(dg(m,0x7F88u)),n);
    store_le32(dg(m,0x7F88u),load_le32(dg(m,0x7F88u))+n);
}
static_assert(0x132DBu-0x132B4u==39u);

void function_132dc(Machine& m,Application25Boundary& b) noexcept {
    app25_copy(m,b,0xC355u,load_le32(dg(m,0x7F84u))+load_le32(dg(m,0x7F88u)),0x300u);
    *dg(m,0x7F8Cu)=1u;
    store_le32(dg(m,0x7F88u),load_le32(dg(m,0x7F88u))+0x300u);
}
static_assert(0x1330Cu-0x132DCu==48u);

void function_1330c(Machine& m,Application25Boundary& b) noexcept {
    for(u32 row=0u;row<0x190u;++row) {
        u32 produced=0u;
        while(produced<0x140u) {
            u32 pos=load_le32(dg(m,0x7F88u));
            const u32 base=load_le32(dg(m,0x7F84u));
            const u8 tag=*dg(m,base+pos);
            ++pos;
            if(tag<0x80u) {
                const u32 n=static_cast<u32>(tag)+1u;
                store_le32(dg(m,0x7F88u),pos);
                app25_copy(m,b,0x759Cu+produced,base+pos,n);
                pos=load_le32(dg(m,0x7F88u))+n;
                produced+=n;
            } else if(tag>0x80u) {
                const u32 n=0x101u-static_cast<u32>(tag);
                const u8 value=*dg(m,base+pos);
                ++pos;
                for(u32 i=0u;i<n;++i)*dg(m,0x759Cu+produced+i)=value;
                produced+=n;
            }
            store_le32(dg(m,0x7F88u),pos);
        }
        if(b.call_27a90)b.call_27a90(0x759Cu,row,0u,0x50u,load_le32(dg(m,0xC050u)));
    }
}
static_assert(0x133CBu-0x1330Cu==191u);

void function_133cc(Machine& m,Application25Boundary& b,u32 page) noexcept {
    if(b.call_291b0)b.call_291b0(static_cast<u8>(page));
    u32 pos=load_le32(dg(m,0x7F88u));
    const u32 base=load_le32(dg(m,0x7F84u));
    u32 outer=load_le16(dg(m,base+pos)); pos+=2u;
    s32 y=0;
    while(outer--!=0u) {
        const u32 dy=load_le16(dg(m,base+pos));
        u32 inner=load_le16(dg(m,base+pos+2u)); pos+=4u;
        y+=static_cast<s32>(dy);
        s32 x=0;
        while(inner--!=0u) {
            const u32 dx=load_le16(dg(m,base+pos));
            const u32 reps=load_le16(dg(m,base+pos+2u)); pos+=4u;
            x+=static_cast<s32>(dx);
            store_le32(dg(m,0x7F88u),pos);
            for(u32 i=0u;i<reps;++i) {
                const u8 value=static_cast<u8>(function_13268(m));
                if(b.call_29aa4)b.call_29aa4(static_cast<u32>(x),static_cast<u32>(y),value);
                ++x;
            }
            pos=load_le32(dg(m,0x7F88u));
        }
    }
    store_le32(dg(m,0x7F88u),pos);
}
static_assert(0x13488u-0x133CCu==188u);

void function_13488(Machine& m,Application25Boundary& b) noexcept {
    const u32 handle=load_le32(dg(m,0x7F7Cu));
    if(load_le32(dg(m,0x7F98u))!=0u) {
        const u32 left=load_le32(dg(m,0x7F9Cu))-1u;
        store_le32(dg(m,0x7F9Cu),left);
        if(left==0u) {
            if(b.call_3131c)b.call_3131c(load_le32(dg(m,0x7FB0u)),0u,handle);
            store_le32(dg(m,0x7F88u),0x1F400u);
            store_le32(dg(m,0x7F98u),0u);
            store_le32(dg(m,0x7F94u),0u);
            return;
        }
        store_le32(dg(m,0x7F88u),load_le32(dg(m,0x7F88u))+6u);
        function_133cc(m,b,load_le32(dg(m,0xC050u)));
        if(b.call_3131c)b.call_3131c(load_le32(dg(m,0x7FA0u)),0u,handle);
        store_le32(dg(m,0x7F88u),0x1F400u);
        return;
    }

    const u32 base=load_le32(dg(m,0x7F84u));
    u32 pos=load_le32(dg(m,0x7F88u));
    const u32 frames=load_le16(dg(m,base+pos)); pos+=2u;
    store_le32(dg(m,0x7F9Cu),frames);
    const u32 rel=load_le32(dg(m,base+pos)); pos+=4u;
    const u32 target=load_le32(dg(m,0x7FACu))+rel;
    store_le32(dg(m,0x7FA0u),target);
    store_le32(dg(m,0x7F88u),pos);
    function_133cc(m,b,load_le32(dg(m,0xC050u)));
    const u32 filepos=b.call_3131c?b.call_3131c(0u,1u,handle):0u;
    store_le32(dg(m,0x7FB0u),filepos-0x1F400u+load_le32(dg(m,0x7F88u)));
    if(b.call_3131c)b.call_3131c(target,0u,handle);
    store_le32(dg(m,0x7F88u),0x1F400u);
    store_le32(dg(m,0x7F98u),1u);
}
static_assert(0x13599u-0x13488u==273u);

void function_135c4(Machine& m,Application25Boundary& b) noexcept {
    const u32 handle=load_le32(dg(m,0x7F7Cu));
    const u32 opcode=b.read_312ec_value?b.read_312ec_value(handle,2u)&0xFFFFu:0u;
    const u32 size=b.read_312ec_value?b.read_312ec_value(handle,4u):0u;
    if(b.call_312ec)b.call_312ec(load_le32(dg(m,0x7F84u)),size,handle);
    store_le32(dg(m,0x7F88u),0u);
    if(opcode>9u)return;
    switch(opcode) {
        case 0u:
            function_132b4(m,b,0xC355u,0x300u); *dg(m,0x7F8Cu)=1u; return;
        case 1u:
            if(b.call_29b40)b.call_29b40(0u,0u,0x190u,0x140u,0u);
            if(b.call_30d30)b.call_30d30(0xC355u,0u,0x300u);
            if(b.call_29234)b.call_29234(0xC355u);
            return;
        case 2u:
            if(b.call_27bd8)b.call_27bd8(load_le32(dg(m,0xC050u)));
            return;
        case 3u:
            *dg(m,0x7F80u)=1u;
            if(b.call_27aec)b.call_27aec(load_le32(dg(m,0xC050u)));
            return;
        case 4u:
            function_13488(m,b);
            return;
        case 5u: {
            const u32 base=load_le32(dg(m,0x7F84u));
            s32 first=static_cast<s16>(load_le16(dg(m,base)));
            const s32 second=static_cast<s16>(load_le16(dg(m,base+2u)));
            store_le32(dg(m,0x7F88u),4u);
            if(first==0x15&&load_le32(dg(m,0x7F78u))==0x0Bu)first=0x30;
            if(b.call_28dcc)b.call_28dcc(0x9C4u);
            if(b.call_28ef0)b.call_28ef0();
            if(b.call_29008)b.call_29008(0x64u);
            if(b.call_28f58)b.call_28f58(0x64u);
            if(b.call_28e58)b.call_28e58(first,second+1,0x9C4u);
            return;
        }
        case 6u: {
            const s32 value=static_cast<s16>(load_le16(dg(m,load_le32(dg(m,0x7F84u)))));
            store_le32(dg(m,0x7F88u),2u);
            if(value!=3&&value!=0x0F&&value!=0x88&&b.call_28110)
                b.call_28110(static_cast<u32>(value)+load_le32(dg(m,0x258D0u)),0xFFu,0u,1u);
            return;
        }
        case 7u: {
            const u32 base=load_le32(dg(m,0x7F84u));
            const u16 mode=load_le16(dg(m,base));
            const u16 value=load_le16(dg(m,base+2u));
            store_le32(dg(m,0x7F88u),4u); store_le32(dg(m,0x7FA8u),value);
            if(mode==0u) {
                if(b.call_29b40)b.call_29b40(0x140u,0u,0x50u,0x140u,0u);
                store_le32(dg(m,0x7FA4u),0u);
            } else {
                if(b.call_1c368)b.call_1c368(0x759Cu,static_cast<u32>(static_cast<s16>(mode)+0x324));
                store_le32(dg(m,0x7FA4u),1u);
            }
            return;
        }
        case 8u:
            *dg(m,0x7F80u)=1u;
            if(b.call_27b84)b.call_27b84(load_le32(dg(m,0xC050u)));
            return;
        case 9u: {
            const s32 value=static_cast<s16>(load_le16(dg(m,load_le32(dg(m,0x7F84u)))));
            store_le32(dg(m,0x7F88u),2u);
            if(b.call_146b8)b.call_146b8(value);
            return;
        }
    }
}
static_assert(0x13832u-0x135C4u==622u);

[[nodiscard]] u32 function_13834(Machine& m,Application25Boundary& b) noexcept {
    const u32 handle=load_le32(dg(m,0x7F7Cu));
    const u16 packets=static_cast<u16>(b.read_312ec_value?b.read_312ec_value(handle,2u):0u);
    if(b.call_312ec)b.call_312ec(0x7F90u,2u,handle);
    if(b.call_291b0)b.call_291b0(*dg(m,0xC050u));
    if(b.call_291c8)b.call_291c8(static_cast<u8>(*dg(m,0xC050u)^1u));
    if(packets==0u)return 0u;

    u16 iteration=0u;
    for(;;) {
        if(b.call_2e608)b.call_2e608();
        if(b.call_30700)(void)b.call_30700();
        const u16 commands=static_cast<u16>(b.read_312ec_value?b.read_312ec_value(handle,2u):0u);
        for(u16 i=0u;i<commands;++i) {
            function_135c4(m,b);
            if(b.call_2f868)b.call_2f868();
        }
        if(load_le32(dg(m,0x7FA4u))!=0u) {
            if(b.call_29b40)b.call_29b40(0x140u,0u,0x50u,0x140u,0u);
            if(b.call_13fd4)b.call_13fd4(1u,0x140u,0u,0x10u,0x140u,0x759Cu,load_le32(dg(m,0x7FA8u)));
        }
        if(b.call_2a020) {
            u32 page=load_le32(dg(m,0xC050u));
            page=(page&0xFFFFFF00u)|((page&0xFFu)^1u);
            b.call_2a020(page);
        }
        if(b.call_28168)b.call_28168();
        if(*dg(m,0x7F8Cu)!=0u) {
            if(b.call_29234)b.call_29234(0xC355u);
            *dg(m,0x7F8Cu)=0u;
        }
        if(b.call_2e614)b.call_2e614(load_le32(dg(m,*dg(m,0x7F80u)!=0u?0x12C10u:0x7F90u)),0u);
        if(b.call_2f868)b.call_2f868();
        if((b.call_30b40?b.call_30b40():0u)==0x1Bu)return 1u;
        ++iteration;
        store_le32(dg(m,0x7F94u),1u);
        if(iteration>=packets)return 0u;
    }
}
static_assert(0x139ABu-0x13834u==375u);

// -----------------------------------------------------------------------------
// Batch 26: 139AC..148ED cinematic file/state setup, wrapped text rendering,
// four-choice preprocessing/presentation, script condition selection, and
// interactive cinematic choice preview.  The CS table at 13C64 and compiler
// separators/padding are non-code/data in RESIDUAL_LEDGER.md.
// -----------------------------------------------------------------------------

struct Application26Boundary {
    Application25Boundary* app25=nullptr;
    s32 (*call_312a0)(u32)=nullptr;
    void (*call_325b0)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_2f580)(u32)=nullptr;
    u32 (*call_29048)()=nullptr;
    s32 (*call_29050)()=nullptr;
    u32 (*call_29098)()=nullptr;
    u32 (*call_290a0)()=nullptr;
    void (*call_1aacc)()=nullptr;
    u32 (*call_3104c)(u32)=nullptr;
    u32 (*read_13ad8_stale_local)(u32)=nullptr;
    void (*call_312dc)(u32)=nullptr;
    void (*call_ec1c)()=nullptr;
    void (*call_20294)()=nullptr;
    void (*call_27ff0)()=nullptr;
    void (*call_28ce8)()=nullptr;
    void (*call_1a288)()=nullptr;
    void (*call_1c124)(u32)=nullptr;
    void (*call_181b4)(u32)=nullptr;
    void (*call_15050)(u32)=nullptr;
    u32 (*call_1c1dc)(u32)=nullptr;
    s32 (*call_30318)(u32,u32)=nullptr;
    void (*call_3059c)(u32)=nullptr;
    void (*call_30354)(u32,u32)=nullptr;
    u32 (*call_2ec74)(u32)=nullptr;
    void (*call_31334)(u32,u32)=nullptr;
    void (*call_280dc)(u32)=nullptr;
    void (*call_27f24)(u32)=nullptr;
    u32 (*call_280e8)()=nullptr;
    void (*call_16034)()=nullptr;
};

static void app26_copy_cstr(Machine& m,u32 dst,u32 src) noexcept {
    for(;;){const u8 c=*dg(m,src++);*dg(m,dst++)=c;if(c==0u)break;}
}
static void app26_append_cstr(Machine& m,u32 dst,u32 src) noexcept {
    while(*dg(m,dst)!=0u)++dst;
    app26_copy_cstr(m,dst,src);
}

[[nodiscard]] s32 function_139ac(Machine& m,Application26Boundary& b,s32 number) noexcept {
    u8 name[11]{};
    for(u32 i=0u;i<11u;++i)name[i]=*dg(m,0x131D0u+i);
    const s32 q=number/10;
    const s32 r=number%10;
    name[4]=static_cast<u8>(q+0x30);
    name[5]=static_cast<u8>(r+0x30);

    app26_copy_cstr(m,0x759Cu,0xF061u);
    app26_append_cstr(m,0x759Cu,0x650u);
    u32 end=0x759Cu; while(*dg(m,end)!=0u)++end;
    for(u32 i=0u;i<11u;++i){*dg(m,end+i)=name[i];if(name[i]==0u)break;}

    s32 handle=b.call_312a0?b.call_312a0(0x759Cu):-1;
    if(handle==-1) {
        if(b.call_325b0)b.call_325b0(0xC054u,0u,0u,0x654u,0x100u);
        if(b.app25&&b.app25->call_29234)b.app25->call_29234(0xC054u);
        app26_append_cstr(m,0x759Cu,load_le32(dg(m,0x7F74u)));
        if(b.call_2f580)b.call_2f580(0x759Cu);
    }
    if(b.app25&&b.app25->call_3131c)(void)b.app25->call_3131c(0u,0u,static_cast<u32>(handle));
    return handle;
}
static_assert(0x13AD5u-0x139ACu==297u);

void function_13ad8(Machine& m,Application26Boundary& b,u32* mode_io,
                     u32* saved_29050,u32* saved_29048,u32* saved_29098,u32* saved_290a0) noexcept {
    u32 mode=*mode_io;
    if(b.app25&&b.app25->call_19ccc)b.app25->call_19ccc(0u);

    u32 v29048=0u,v29050=0u,v29098=0u,v290a0=0u;
    if(mode!=0x0Cu) {
        v29048=b.call_29048?b.call_29048():0u;
        v29050=static_cast<u32>(b.call_29050?b.call_29050():0);
        v29098=b.call_29098?b.call_29098():0u;
        v290a0=b.call_290a0?b.call_290a0():0u;
        if(b.app25&&b.app25->call_28dcc)b.app25->call_28dcc(0x9C4u);
        store_le32(dg(m,0x12C10u),6u);
    } else {
        // The encoded routine leaves all four locals uninitialized on mode 0Ch
        // and later copies those stale stack dwords to the caller's outputs.
        if(b.read_13ad8_stale_local) {
            v29098=b.read_13ad8_stale_local(0u);
            v290a0=b.read_13ad8_stale_local(1u);
            v29050=b.read_13ad8_stale_local(2u);
            v29048=b.read_13ad8_stale_local(3u);
        }
        store_le32(dg(m,0x12C10u),2u);
    }
    if(b.call_1aacc)b.call_1aacc();

    if(mode==2u) {
        s32 count=0;
        for(u32 actor=0u;actor<0x920u;actor+=0x248u)
            if((load_le16(dg(m,0x218F0u+actor))&0x8000u)==0u)++count;
        mode=static_cast<u32>(static_cast<s32>(mode)+count-1);
    }
    if(mode==0x0Au) {
        s32 count=0;
        for(u32 actor=0u;actor<0x920u;actor+=0x248u) {
            const u16 state=load_le16(dg(m,0x218F0u+actor));
            if(state<0x4000u&&(state&0x0080u)==0u)++count;
        }
        if(count==0)mode=0x0Bu;
    }

    store_le32(dg(m,0x7F78u),mode);
    const s32 handle=function_139ac(m,b,static_cast<s32>(mode));
    store_le32(dg(m,0x7F88u),0x1F400u);
    *dg(m,0x7F8Cu)=0u;
    store_le32(dg(m,0x7F94u),1u);
    store_le32(dg(m,0x7F98u),0u);
    store_le32(dg(m,0x7FA4u),0u);
    store_le32(dg(m,0x7F9Cu),0u);
    store_le32(dg(m,0x7FA0u),0u);
    store_le32(dg(m,0x7FB0u),0u);
    const u32 stream=b.app25&&b.app25->call_3131c?
        b.app25->call_3131c(0u,1u,static_cast<u32>(handle)):0u;
    store_le32(dg(m,0x7F7Cu),static_cast<u32>(handle));
    store_le32(dg(m,0x7FACu),stream);
    store_le32(dg(m,0x7F84u),b.call_3104c?b.call_3104c(0x1F400u):0u);

    *mode_io=mode;
    *saved_29050=v29050;
    *saved_29048=v29048;
    *saved_29098=v29098;
    *saved_290a0=v290a0;
}
static_assert(0x13C62u-0x13AD8u==394u);

void function_13c88(Machine& m,Application26Boundary& b,u32 mode,s32 saved_29050,
                     u32 saved_29048,u32 saved_29098,u32 saved_290a0) noexcept {
    if(b.app25&&b.app25->call_310b0)b.app25->call_310b0(load_le32(dg(m,0x7F84u)));
    if(b.call_312dc)b.call_312dc(load_le32(dg(m,0x7F7Cu)));
    if((mode==0x0Au||mode==0x0Bu)&&b.call_ec1c)b.call_ec1c();
    if(b.call_27ff0)b.call_27ff0();
    if(b.app25&&b.app25->call_28dcc)b.app25->call_28dcc(0x9C4u);
    if(b.call_20294)b.call_20294();
    if(b.app25&&b.app25->call_30d30)b.app25->call_30d30(0xC054u,0u,0x300u);
    if(b.app25&&b.app25->call_29234)b.app25->call_29234(0xC054u);
    if(b.call_1a288)b.call_1a288();

    s32 restore=saved_29050;
    switch(mode) {
        case 1u:
            if(b.call_1c124)b.call_1c124(0x2Bu);
            break;
        case 2u: case 3u: case 4u: case 5u:
            store_le16(dg(m,0x255D2u),0x16u); store_le16(dg(m,0x255D6u),0x0Du);
            store_le16(dg(m,0x255D4u),0x8000u); store_le16(dg(m,0x255D0u),0x8000u);
            store_le32(dg(m,0xC658u),0u);
            if(b.app25&&b.app25->call_28ef0)b.app25->call_28ef0();
            if(b.call_28ce8)b.call_28ce8();
            restore=-2;
            if(b.call_181b4)b.call_181b4(0x0Cu);
            break;
        case 6u:
            store_le16(dg(m,0x255D2u),0x17u); store_le16(dg(m,0x255D6u),0x17u);
            store_le16(dg(m,0x255D4u),0x8000u); store_le16(dg(m,0x255D0u),0x8000u);
            store_le32(dg(m,0xC658u),0x10Eu);
            if(b.app25&&b.app25->call_28ef0)b.app25->call_28ef0();
            if(b.call_28ce8)b.call_28ce8();
            restore=-2;
            if(b.call_181b4)b.call_181b4(0x0Cu);
            break;
        case 7u:
            store_le16(dg(m,0x255D2u),2u); store_le16(dg(m,0x255D6u),0x41u);
            store_le16(dg(m,0x255D4u),0x8000u); store_le16(dg(m,0x255D0u),0x8000u);
            store_le32(dg(m,0xC658u),0x10Eu);
            if(b.app25&&b.app25->call_28ef0)b.app25->call_28ef0();
            if(b.call_28ce8)b.call_28ce8();
            if(b.call_181b4)b.call_181b4(0x0Bu);
            if(b.call_1c124)b.call_1c124(0x25u);
            if(b.call_15050)b.call_15050(0x6Eu);
            restore=-2;
            if(b.app25)function_131dc(m,*b.app25);
            break;
        case 9u:
            store_le16(dg(m,0x255D2u),0x15u); store_le16(dg(m,0x255D6u),0x2Bu);
            store_le32(dg(m,0xC658u),0xB4u);
            break;
        default: break;
    }

    if(mode==0u||mode==0x0Cu) {
        if((b.call_29050?b.call_29050():-1)!=-1) {
            if(b.app25&&b.app25->call_28dcc)b.app25->call_28dcc(0x9C4u);
            if(b.app25&&b.app25->call_28ef0)b.app25->call_28ef0();
        }
        if(b.app25&&b.app25->call_19cf4)b.app25->call_19cf4();
        return;
    }

    if(b.call_325b0)b.call_325b0(0xC054u,0u,load_le32(dg(m,0xC034u)),0x654u,0x100u);
    if(b.app25&&b.app25->call_29234)b.app25->call_29234(0xC054u);
    if(restore!=-2&&(b.call_29050?b.call_29050():-1)!=-1) {
        if(b.app25&&b.app25->call_28dcc)b.app25->call_28dcc(0x9C4u);
        if(b.app25&&b.app25->call_28ef0)b.app25->call_28ef0();
    }
    if(restore!=-1) {
        if(b.call_28ce8)b.call_28ce8();
        if(b.app25&&b.app25->call_29008)b.app25->call_29008(saved_29098);
        if(b.app25&&b.app25->call_28f58)b.app25->call_28f58(saved_290a0);
        if(b.app25&&b.app25->call_28e58)b.app25->call_28e58(restore,static_cast<s32>(saved_29048),0x9C4u);
    }
    if(b.app25&&b.app25->call_1f8fc)b.app25->call_1f8fc();
    if(b.app25&&b.app25->call_19cf4)b.app25->call_19cf4();
}
static_assert(0x13EF6u-0x13C88u==622u);

[[nodiscard]] u32 function_13ef8(Machine& m,Application26Boundary& b,u32 mode) noexcept {
    if(mode==7u&&b.call_1c1dc&&b.call_1c1dc(0x25u)==1u)return 0u;
    u32 saved50=0u,saved48=0u,saved98=0u,saveda0=0u;
    function_13ad8(m,b,&mode,&saved50,&saved48,&saved98,&saveda0);
    const u32 result=b.app25?function_13834(m,*b.app25):0u;
    function_13c88(m,b,mode,static_cast<s32>(saved50),saved48,saved98,saveda0);
    return result;
}
static_assert(0x13F61u-0x13EF8u==105u);

void function_13f70(Machine& m,Application26Boundary& b,u32 offset) noexcept {
    const u32 handle=b.call_2ec74?b.call_2ec74(9u):0u;
    const u32 address=load_le32(dg(m,0x258CCu))+offset+load_le32(dg(m,0x7FCCu));
    if(b.call_31334)b.call_31334(address,handle);
    const u32 words=b.app25&&b.app25->read_312ec_value?b.app25->read_312ec_value(handle,2u):0u;
    if(b.app25&&b.app25->call_312ec)b.app25->call_312ec(0x13020u,words*2u,handle);
    store_le32(dg(m,0x7FECu),0u);
}
static_assert(0x13FD4u-0x13F70u==100u);

[[nodiscard]] u32 function_13fd4(Machine& m,Application26Boundary& b,u32 mode,s32 y,s32 x,
                                  s32 line_step,s32 max_width,u32 text,u32 font) noexcept {
    u32 lines=0u;
    s32 carry_width=0;
    u32 line=text;
    if(*dg(m,line)==0u)return 0u;
    u32 cut=line;
    bool emit=false;
    for(;;) {
        if(emit) {
            const u8 saved=*dg(m,cut);
            *dg(m,cut)=0u;
            if(mode==1u) {
                if(b.app25&&b.app25->call_30508)b.app25->call_30508(static_cast<u32>(y),0u,0x140u,line,font);
            } else if(b.app25&&b.app25->call_3060c) {
                b.app25->call_3060c(static_cast<u32>(y),static_cast<u32>(x),line,font);
            }
            ++lines;
            if(saved==0u)return lines;
            y+=line_step;
            emit=false;
            carry_width=0;
            line=cut+1u;
            *dg(m,cut)=saved;
            while(static_cast<s8>(*dg(m,line))==0x20)++line;
        }

        s32 word_width=0;
        u32 p=line;
        s32 prior_width=0;
        for(;;) {
            prior_width+=word_width;
            cut=p;
            word_width=carry_width;
            while(static_cast<s8>(*dg(m,p))!=0x20&&*dg(m,p)!=0u&&*dg(m,p)!=1u) {
                const s32 ch=static_cast<s8>(*dg(m,p++));
                word_width+=b.call_30318?b.call_30318(static_cast<u32>(ch),font):0;
            }
            if(prior_width+word_width>max_width) {
                --cut;
                emit=true;
                break;
            }
            if(*dg(m,p)==0u||*dg(m,p)==1u) {
                cut=p;
                emit=true;
                break;
            }
            carry_width=b.call_30318?b.call_30318(0x20u,font):0;
            ++p;
        }
    }
}
static_assert(0x14116u-0x13FD4u==322u);

void function_14118(Machine& m,Application26Boundary& b,u32 src,u32 dst,u32 size) noexcept {
    if(b.app25&&b.app25->call_30d30)b.app25->call_30d30(dst,0u,size);
    u32 active=0u;
    for(u32 actor=0u;actor<0x920u;actor+=0x248u)
        if((load_le16(dg(m,0x218F0u+actor))&0xC000u)==0u)++active;
    while(*dg(m,src)!=0u) {
        if(*dg(m,src)!='[') { *dg(m,dst++)=*dg(m,src++); continue; }
        ++src;
        if(active==1u) {
            while(*dg(m,src)!='|')++src;
            ++src;
            while(*dg(m,src)!=']')*dg(m,dst++)=*dg(m,src++);
            ++src;
        } else {
            while(*dg(m,src)!='|')*dg(m,dst++)=*dg(m,src++);
            ++src;
            while(*dg(m,src)!=']')++src;
            ++src;
        }
    }
}
static_assert(0x141B6u-0x14118u==158u);

void function_141b8(Machine& m,Application26Boundary& b) noexcept {
    store_le32(dg(m,0x7FE8u),0u);
    for(u32 i=0u;i<4u;++i) {
        const u16 id=load_le16(dg(m,0x7FD8u+i*4u));
        if(id==0xFFFFu)continue;
        if(b.app25&&b.app25->call_1c368)b.app25->call_1c368(0x759Cu,static_cast<u32>(id)+load_le32(dg(m,0x7FF4u)));
        store_le32(dg(m,0x7FE8u),load_le32(dg(m,0x7FE8u))+1u);
        function_14118(m,b,0x759Cu,0x12C20u+i*0x100u,0x100u);
    }
}
static_assert(0x14217u-0x141B8u==95u);

void function_14218(Machine& m,Application26Boundary& b,s32 selected,u32 show_all) noexcept {
    if(b.app25)app25_select_page(m,*b.app25);
    if(selected==-1&&b.app25&&b.app25->call_2e510)
        b.app25->call_2e510(0x1Eu,0xACu,load_le32(dg(m,0x258CCu))+3u,9u);
    if(b.app25&&b.app25->call_2e510)
        b.app25->call_2e510(0xDCu,0u,load_le32(dg(m,0x258CCu))+2u,9u);

    for(s32 i=0;i<4;++i) {
        if(load_le16(dg(m,0x7FD8u+static_cast<u32>(i)*4u))==0xFFFFu)continue;
        if(b.call_3059c)b.call_3059c(i==selected?0x8000u:0u);
        if(i!=selected&&show_all==0u)continue;
        const u32 y=static_cast<u32>(0xDD+i*0x16);
        const u32 number=b.app25&&b.app25->call_2e6f0?b.app25->call_2e6f0(i+1):0u;
        if(b.app25&&b.app25->call_3060c)b.app25->call_3060c(y,0x17u,number,1u);
        if(b.call_30354)b.call_30354(0x3Au,1u);
        (void)function_13fd4(m,b,0u,static_cast<s32>(y),0x1F,0x0A,0x10A,
                             0x12C20u+static_cast<u32>(i)*0x100u,1u);
    }
    if(b.call_3059c)b.call_3059c(0u);
}
static_assert(0x14309u-0x14218u==241u);

[[nodiscard]] u32 function_1430c(Machine& m) noexcept {
    u32 index=load_le32(dg(m,0x7FECu));
    const u32 actor=0x218E0u+load_le32(dg(m,0x7FFCu))*0x248u;
    bool chosen=false;
    u16 value=0u;
    for(;;) {
        const u32 rec=0x13020u+index*2u;
        const u16 code=load_le16(dg(m,rec));
        if(code==0x21u) {
            if(!chosen)value=load_le16(dg(m,rec+2u));
            index+=2u;
            store_le32(dg(m,0x7FECu),index);
            return value;
        }
        bool match=false;
        switch(code) {
            case 0x00u: match=*dg(m,actor+0x1Au)==0u; break;
            case 0x01u: match=*dg(m,actor+0x1Au)==2u; break;
            case 0x02u: match=*dg(m,actor+0x1Au)==4u; break;
            case 0x03u: match=*dg(m,actor+0x1Au)==1u; break;
            case 0x04u: match=*dg(m,actor+0x1Au)==3u; break;
            case 0x05u: match=*dg(m,actor+0x1Au)==5u; break;
            case 0x06u: match=*dg(m,actor+0x3Du)>0x10u; break;
            case 0x07u: match=*dg(m,actor+0x3Du)<0x0Au; break;
            case 0x08u: match=*dg(m,actor+0x33u)>0x10u; break;
            case 0x09u: match=*dg(m,actor+0x33u)<0x0Au; break;
            case 0x0Au: match=*dg(m,actor+0x39u)>0x10u; break;
            case 0x0Bu: match=*dg(m,actor+0x39u)<0x0Au; break;
            case 0x0Cu: match=*dg(m,actor+0x35u)>0x10u; break;
            case 0x0Du: match=*dg(m,actor+0x35u)<0x0Au; break;
            case 0x0Eu: match=*dg(m,actor+0x37u)>0x10u; break;
            case 0x0Fu: match=*dg(m,actor+0x37u)<0x0Au; break;
            case 0x10u: match=*dg(m,actor+0x3Bu)>0x10u; break;
            case 0x11u: match=*dg(m,actor+0x3Bu)<0x0Au; break;
            case 0x12u: match=*dg(m,actor+0x14u)==0u; break;
            case 0x13u: match=*dg(m,actor+0x14u)==5u; break;
            case 0x14u: match=*dg(m,actor+0x14u)==1u; break;
            case 0x15u: match=*dg(m,actor+0x14u)==4u; break;
            case 0x16u: match=*dg(m,actor+0x14u)==3u; break;
            case 0x17u: match=*dg(m,actor+0x14u)==2u; break;
            case 0x18u: { const u8 v=*dg(m,actor+0x14u); match=v>=2u&&v<=5u; break; }
            case 0x19u: match=*dg(m,actor+0x15u)==0u||*dg(m,actor+0x16u)==0u||*dg(m,actor+0x17u)==0u; break;
            case 0x1Au: match=static_cast<s8>(*dg(m,actor+0x15u))==2||static_cast<s8>(*dg(m,actor+0x16u))==2||static_cast<s8>(*dg(m,actor+0x17u))==2; break;
            case 0x1Bu: match=static_cast<s8>(*dg(m,actor+0x15u))==1||static_cast<s8>(*dg(m,actor+0x16u))==1||static_cast<s8>(*dg(m,actor+0x17u))==1; break;
            case 0x1Cu: match=static_cast<s8>(*dg(m,actor+0x15u))==4||static_cast<s8>(*dg(m,actor+0x16u))==4||static_cast<s8>(*dg(m,actor+0x17u))==4; break;
            case 0x1Du: match=static_cast<s8>(*dg(m,actor+0x15u))==3||static_cast<s8>(*dg(m,actor+0x16u))==3||static_cast<s8>(*dg(m,actor+0x17u))==3; break;
            case 0x1Eu: match=static_cast<s8>(*dg(m,actor+0x15u))==5||static_cast<s8>(*dg(m,actor+0x16u))==5||static_cast<s8>(*dg(m,actor+0x17u))==5; break;
            case 0x1Fu: match=*dg(m,actor+0x13u)==0u; break;
            case 0x20u: match=*dg(m,actor+0x13u)==1u; break;
            default: break;
        }
        if(match&&!chosen){chosen=true;value=load_le16(dg(m,rec+2u));}
        index+=2u;
    }
}
static_assert(0x146B8u-0x1430Cu==940u);

void function_146b8(Application26Boundary& b,u32 value) noexcept {
    if(b.call_280dc)b.call_280dc(0x0Au);
    if(b.call_27f24)b.call_27f24(value);
    if(b.call_280dc)b.call_280dc(6u);
}
static_assert(0x146D9u-0x146B8u==33u);

void function_146dc(Machine& m,Application26Boundary& b) noexcept {
    u32 stream_index=load_le32(dg(m,0x7FECu))+1u;
    const u16 token=load_le16(dg(m,0x1301Eu+stream_index*2u));
    store_le32(dg(m,0x7FECu),stream_index);
    u32 resource=function_1430c(m);
    if(b.app25)app25_select_page(m,*b.app25);
    if(b.app25&&b.app25->call_2e510)
        b.app25->call_2e510(0x1Eu,0xACu,load_le32(dg(m,0x258CCu))+3u,9u);
    resource+=load_le32(dg(m,0x7FF4u));
    if(b.app25&&b.app25->call_1c368)b.app25->call_1c368(0x759Cu,resource);
    (void)function_13fd4(m,b,0u,0x20,0xAF,0x0C,0x7B,0x759Cu,1u);

    const auto clear=[&]() noexcept {
        if(b.app25&&b.app25->call_1a8c8)
            b.app25->call_1a8c8(load_le32(dg(m,0xC050u)),8u,0xA0u,0xD2u,0xA0u);
    };
    const auto wait_release=[&]() noexcept {
        while(b.app25&&b.app25->call_1a1f4&&b.app25->call_1a1f4()!=0u)
            if(b.call_16034)b.call_16034();
    };
    const auto wait_press_or_device=[&]() noexcept {
        while(b.app25&&b.app25->call_1a1f4&&b.app25->call_1a1f4()==0u) {
            if(!b.call_280e8||b.call_280e8()==0u)break;
            if(b.call_16034)b.call_16034();
        }
    };

    if(token==0xFFFFu||*dg(m,0xC02Bu)==0u||load_le32(dg(m,0xEE40u))==0u) {
        clear();
        wait_release();
        while(b.app25&&b.app25->call_1a1f4&&b.app25->call_1a1f4()==0u)
            if(b.call_16034)b.call_16034();
        clear();
        return;
    }

    function_146b8(b,load_le32(dg(m,0x25614u))+load_le32(dg(m,0x7FF8u))+token);
    clear();
    wait_release();
    wait_press_or_device();
    if(b.call_27ff0)b.call_27ff0();

    u16 follow=0xFFFFu;
    if(load_le32(dg(m,0x7FD4u))==9u&&token==2u)follow=0x0Au;
    else if(load_le32(dg(m,0x7FD4u))==0x17u&&token==0u)follow=2u;
    if(follow!=0xFFFFu) {
        function_146b8(b,load_le32(dg(m,0x25614u))+load_le32(dg(m,0x7FF8u))+follow);
        wait_release();
        wait_press_or_device();
        if(b.call_27ff0)b.call_27ff0();
    }
    clear();
}
static_assert(0x148EEu-0x146DCu==530u);

// -----------------------------------------------------------------------------
// Batch 27: 148F0..1548B party-template activation, roster selection/restore,
// script-choice maintenance, world-record flag updates, and recursive item count.
// The ten-entry CS table at 14AB4 and compiler separators are non-code/data.
// -----------------------------------------------------------------------------

struct Application27Boundary {
    Application25Boundary* app25=nullptr;
    Application26Boundary* app26=nullptr;
    u32 (*call_0dbd4)(u32)=nullptr;
    void (*call_03ea4)(u32,s32)=nullptr;
    void (*call_03de4)(u32,s32)=nullptr;
    u32 (*call_27c70)(u32,u32)=nullptr;
    void (*call_1c77c)(u32,u32)=nullptr;
    void (*call_1c160)(u32,u32)=nullptr; // stack argument, live EBX
    void (*call_1c40c)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_1a9f8)()=nullptr;
    void (*call_16400)()=nullptr;
    void (*call_1a510)(u32)=nullptr;
    void (*call_ad24)(s32)=nullptr;
    u32 (*call_f218)(u32,u32,u32,u32,u32,u32)=nullptr;
};

static u16 app27_stream_word(Machine& m) noexcept {
    const u32 index=load_le32(dg(m,0x7FECu));
    const u16 value=load_le16(dg(m,0x13020u+index*2u));
    store_le32(dg(m,0x7FECu),index+1u);
    return value;
}
static u32 app27_variant_bit(Machine& m,u32 bit) noexcept {
    const u32 shift=bit&31u;
    return (load_le32(dg(m,0x14060u))>>shift)&1u;
}

void function_148f0(Machine& m,Application27Boundary& b,u32 slot) noexcept {
    const u32 template_index=app27_stream_word(m);
    const u32 actor=slot*0x248u;
    const u32 dst=0x218E0u+actor;
    const u32 src=0xA808u+template_index*0x248u;
    if(b.app25&&b.app25->call_30cd4)b.app25->call_30cd4(dst,src,0x248u);
    if(b.app25&&b.app25->call_30d30)b.app25->call_30d30(dst+0x10Bu,0u,0x13Du);
    *dg(m,dst+0x10Au)=static_cast<u8>(b.call_0dbd4?b.call_0dbd4(slot):0u);
    *dg(m,dst+0x131u)=0u;

    const s32 cls0=static_cast<s8>(*dg(m,dst+0x15u));
    const s32 cls1=static_cast<s8>(*dg(m,dst+0x16u));
    const s32 cls2=static_cast<s8>(*dg(m,dst+0x17u));
    if(cls0==1) {
        *dg(m,dst+0x131u)=static_cast<u8>(*dg(m,dst+0x131u)|4u);
        if(b.call_03ea4)b.call_03ea4(slot,static_cast<s16>(load_le16(dg(m,dst+0x20u))));
    }
    if(cls0==4||cls1==4||cls2==4) {
        store_le16(dg(m,dst+0x40u),0x00A6u);
        *dg(m,dst+0x131u)=static_cast<u8>(*dg(m,dst+0x131u)|2u);
    }
    const s32 classes[3]={cls0,cls1,cls2};
    for(u32 i=0u;i<3u;++i) {
        if(classes[i]!=3)continue;
        *dg(m,dst+0x131u)=static_cast<u8>(*dg(m,dst+0x131u)|1u);
        if(b.call_03de4)b.call_03de4(slot,static_cast<s16>(load_le16(dg(m,dst+0x20u+i*2u))));
    }
    *dg(m,dst+0x10u)=static_cast<u8>(*dg(m,dst+0x10u)|0x80u);
}
static_assert(0x14A5Eu-0x148F0u==366u);

[[nodiscard]] s32 function_14a60(Machine& m,Application27Boundary& b,u32 value) noexcept {
    u32 actor=0x218E0u;
    for(s32 slot=0;slot<4;++slot,actor+=0x248u) {
        const u32 r=b.call_27c70?b.call_27c70(actor,value):1u;
        if(r==0u&&(load_le16(dg(m,actor+0x10u))&0x0080u)!=0u)return slot;
    }
    return -1;
}
static_assert(0x14AB2u-0x14A60u==82u);

void function_14adc(Machine& m,Application27Boundary& b,u32 slot) noexcept {
    const u32 actor=0x218E0u+slot*0x248u;
    for(u32 i=0u,off=0u;i<10u;++i,off+=0x0Cu) {
        const u32 test=load_le32(dg(m,0x8104u+off));
        if((b.call_27c70?b.call_27c70(test,actor):1u)!=0u)continue;
        const bool bit4000=(load_le16(dg(m,actor+0x10u))&0x4000u)!=0u;
        if(!bit4000) {
            store_le32(dg(m,0xBF2Cu),load_le32(dg(m,0x810Cu+off)));
            if(b.call_1c77c)b.call_1c77c(slot,load_le32(dg(m,0x8108u+off)));
        }
        if(b.call_1c160)b.call_1c160(i+4u,off);
        if(bit4000&&b.app26&&b.app26->call_1c124)b.app26->call_1c124(i+0x0Fu);
        u32 resource=0u;
        switch(i) {
            case 0u: resource=0x19u; break;
            case 1u: resource=0x1Cu; break;
            case 2u: resource=0x11u; break;
            case 3u: resource=0x21u; break;
            case 6u: resource=0x23u; break;
            case 9u: resource=0x0Eu; break;
            default: return;
        }
        if(b.app26&&b.app26->call_1c124)b.app26->call_1c124(resource);
        return;
    }
}
static_assert(0x14BDEu-0x14ADCu==258u);

void function_14be0(Machine& m,Application27Boundary& b,u32 slot) noexcept {
    (void)m;
    const u32 actor=0x218E0u+slot*0x248u;
    u32 candidate=0xA808u;
    for(u32 i=0u;i<10u;++i,candidate+=0x248u) {
        if((b.call_27c70?b.call_27c70(candidate,actor):1u)!=0u)continue;
        if(b.app25&&b.app25->call_30cd4)b.app25->call_30cd4(candidate,actor,0x248u);
        if(b.app25&&b.app25->call_30d30)b.app25->call_30d30(candidate+0x40u,0xFFFFFFFFu,0x2Au);
        return;
    }
}
static_assert(0x14C3Du-0x14BE0u==93u);

void function_14c40(Machine& m,Application27Boundary& b,u32 slot) noexcept {
    const u32 actor=slot*0x248u;
    for(u32 i=0u;i<0x15u;++i) {
        const s32 item=static_cast<s16>(load_le16(dg(m,0x21920u+actor+i*2u)));
        if(item==-1||item==0xA6)continue;
        const u32 variant=b.call_f218?b.call_f218(slot,i,load_le32(dg(m,0x255D0u)),
                                                   load_le32(dg(m,0x255D4u)),1u,0u):0u;
        if(b.app25&&b.app25->call_2195c)
            b.app25->call_2195c(0x80u,static_cast<u32>(item),static_cast<u8>(variant),
                                 load_le32(dg(m,0x255D0u)),load_le32(dg(m,0x255D4u)),1u,0u);
    }
}
static_assert(0x14CA3u-0x14C40u==99u);

[[nodiscard]] s32 function_14ca4(Machine& m,Application27Boundary& b) noexcept {
    const s32 initial=function_14a60(m,b,0x6C8u);
    if(b.app25&&b.app25->call_30d30)b.app25->call_30d30(0x7FBCu,0u,0x10u);
    for(u32 i=0u;i<4u;++i) {
        const u16 state=load_le16(dg(m,0x218F0u+i*0x248u));
        bool disabled=(state&0x0140u)!=0u;
        if(!disabled&&(state&0x0080u)==0u&&(state&0x4000u)==0u)disabled=true;
        if(load_le32(dg(m,0x7FD4u))==0x1Cu&&static_cast<s32>(i)==initial)disabled=true;
        if(disabled)store_le32(dg(m,0x7FBCu+i*4u),1u);
    }
    if(b.call_1a9f8)b.call_1a9f8();
    if(b.call_1c40c)b.call_1c40c(load_le32(dg(m,0xC050u)),0x0Fu,0x40u,0x32u,0xC0u);
    if(b.app26)(void)function_13fd4(m,*b.app26,0u,0x14,0x46,0x12,0xB4,load_le32(dg(m,0x7FB4u)),1u);
    if(b.call_16400)b.call_16400();
    if(b.call_1a510)b.call_1a510(load_le32(dg(m,0xC050u)));
    *dg(m,0xC050u)^=1u;

    s32 selected=0;
    bool chosen=false;
    while(!chosen) {
        while(b.app25&&b.app25->call_1a1f4&&b.app25->call_1a1f4()!=0u){}
        while(b.app25&&b.app25->call_1a1f4&&b.app25->call_1a1f4()==0u){}
        const u16 flags=load_le16(dg(m,0x9E35u));
        if((flags&2u)==0u) {
            const s32 x=static_cast<s16>(load_le16(dg(m,0x9E31u)));
            const s32 y=static_cast<s16>(load_le16(dg(m,0x9E33u)));
            s32 top=0x0D;
            for(s32 i=0;i<4;++i,top+=0x50) {
                if(x<0x136||x>=0x166||y<top||y>=top+0x20)continue;
                if(load_le32(dg(m,0x7FBCu+static_cast<u32>(i)*4u))!=0u)continue;
                selected=i; chosen=true; break;
            }
        }
    }

    const u32 actor=static_cast<u32>(selected)*0x248u;
    if(b.app25&&b.app25->call_30cd4)b.app25->call_30cd4(0x14064u,0x21920u+actor,0x2Au);
    store_le32(dg(m,0x14060u),load_le32(dg(m,0x21A0Bu+actor)));
    if((load_le16(dg(m,0x218F0u+actor))&0x0080u)!=0u) {
        function_14adc(m,b,static_cast<u32>(selected));
        function_14be0(m,b,static_cast<u32>(selected));
    }
    if(b.call_1a510)b.call_1a510(load_le32(dg(m,0xC050u)));
    *dg(m,0xC050u)^=1u;
    if(b.call_1a9f8)b.call_1a9f8();
    if(b.app25&&b.app25->call_22d70)b.app25->call_22d70();
    if(b.app25&&b.app25->call_30d30)b.app25->call_30d30(0x7FBCu,0u,0x10u);
    return selected;
}
static_assert(0x14E99u-0x14CA4u==501u);

void function_14e9c(Machine& m,Application27Boundary& b,u32 slot) noexcept {
    const u32 actor=slot*0x248u;
    for(u32 i=0u;i<12u;++i) {
        const s32 item=static_cast<s16>(load_le16(dg(m,0x14064u+i*2u)));
        if(item==-1||item==0xA6)continue;
        u32 dst=0u;
        for(;dst<12u;++dst)
            if(static_cast<s16>(load_le16(dg(m,0x21920u+actor+dst*2u)))==-1)break;
        const u32 variant=app27_variant_bit(m,i);
        if(dst<12u) {
            store_le16(dg(m,0x21920u+actor+dst*2u),static_cast<u16>(item));
            function_0f238(m,slot,dst,variant);
        } else if(b.app25&&b.app25->call_2195c) {
            b.app25->call_2195c(0x80u,static_cast<u32>(item),variant,
                                 load_le32(dg(m,0x255D0u)),load_le32(dg(m,0x255D4u)),1u,0u);
        }
    }
    for(u32 i=12u;i<0x15u;++i) {
        const s32 item=static_cast<s16>(load_le16(dg(m,0x14064u+i*2u)));
        if(item==-1)continue;
        if(b.app25&&b.app25->call_2195c)
            b.app25->call_2195c(0x80u,static_cast<u32>(item),app27_variant_bit(m,i),
                                 load_le32(dg(m,0x255D0u)),load_le32(dg(m,0x255D4u)),1u,0u);
    }
}
static_assert(0x14FBEu-0x14E9Cu==290u);

void function_14fc0(Machine& m,Application27Boundary& b) noexcept {
    s32 slot=0;
    for(;slot<4;++slot) {
        if((load_le16(dg(m,0x218F0u+static_cast<u32>(slot)*0x248u))&0x8000u)==0u)continue;
        function_148f0(m,b,static_cast<u32>(slot));
        break;
    }
    if(slot!=4)return;
    slot=function_14ca4(m,b);
    if(b.call_ad24)b.call_ad24(slot);
    function_148f0(m,b,static_cast<u32>(slot));
    function_14e9c(m,b,static_cast<u32>(slot));
    const s32 side=slot/2;
    if(*dg(m,0xD681u+static_cast<u32>(side))!=0u&&
       static_cast<s32>(load_le32(dg(m,0xD8A8u+static_cast<u32>(side)*4u)))==slot) {
        *dg(m,0xCF5Au+static_cast<u32>(side)*2u)=1u;
        *dg(m,0xD681u+static_cast<u32>(side))=0u;
        *dg(m,0xCF5Bu+static_cast<u32>(side)*2u)=1u;
    }
}
static_assert(0x1504Du-0x14FC0u==141u);

void function_15050(Machine& m,Application27Boundary& b,u32 raw_item) noexcept {
    u32 item=raw_item;
    if(item==0x90u)item=0x94u;
    if(item==0x78u) {
        if(b.app26&&b.app26->call_1c1dc&&b.app26->call_1c1dc(0x58u)==1u)return;
        if(b.app26&&b.app26->call_1c124)b.app26->call_1c124(0x58u);
    }
    if(item==0x9Bu) {
        if(b.app26&&b.app26->call_1c1dc&&b.app26->call_1c1dc(0x59u)==1u)return;
        if(b.app26&&b.app26->call_1c124)b.app26->call_1c124(0x59u);
    }
    if(static_cast<s16>(load_le16(dg(m,0x8600u)))==-1) {
        if(b.app25&&b.app25->call_1a344)b.app25->call_1a344(static_cast<s16>(item&0xFFFFu),0u);
        return;
    }
    for(u32 slot=0u;slot<4u;++slot) {
        const u32 actor=slot*0x248u;
        if((load_le16(dg(m,0x218F0u+actor))&0x8000u)!=0u)continue;
        for(u32 index=0u;index<12u;++index) {
            const u32 addr=0x21920u+actor+index*2u;
            if(static_cast<s16>(load_le16(dg(m,addr)))!=-1)continue;
            store_le16(dg(m,addr),static_cast<u16>(item));
            function_0f238(m,slot,index,0u);
            return;
        }
    }
    if(b.app25&&b.app25->call_2195c)
        b.app25->call_2195c(0x80u,item,0u,load_le32(dg(m,0x255D0u)),
                             load_le32(dg(m,0x255D4u)),1u,0u);
}
static_assert(0x15143u-0x15050u==243u);

void function_15144(Machine& m,Application27Boundary& b,u32 item) noexcept {
    if(b.app25&&b.app25->call_2195c)
        b.app25->call_2195c(0x80u,item,0u,load_le32(dg(m,0x255D0u)),
                             load_le32(dg(m,0x255D4u)),1u,0u);
}
static_assert(0x15168u-0x15144u==36u);

void function_15168(Machine& m,Application27Boundary& b,u32 prepend) noexcept {
    u32 slot=0u;
    if(prepend==0u) {
        for(;slot<4u;++slot)
            if(load_le16(dg(m,0x7FD8u+slot*4u))==0xFFFFu)break;
        if(slot<4u) {
            store_le16(dg(m,0x7FD8u+slot*4u),app27_stream_word(m));
            store_le16(dg(m,0x7FDAu+slot*4u),app27_stream_word(m));
        }
    } else {
        if(b.app25&&b.app25->call_30cd4)b.app25->call_30cd4(0x7FDCu,0x7FD8u,0x0Cu);
        store_le16(dg(m,0x7FD8u),app27_stream_word(m));
        store_le16(dg(m,0x7FDAu),app27_stream_word(m));
    }
    if(b.app26)function_141b8(m,*b.app26);
}
static_assert(0x1520Bu-0x15168u==163u);

void function_1520c(Machine& m,Application27Boundary& b) noexcept {
    const u16 remove=app27_stream_word(m);
    for(u32 i=0u;i<4u;++i) {
        if(load_le16(dg(m,0x7FD8u+i*4u))!=remove)continue;
        if(i!=3u&&b.app25&&b.app25->call_30cd4)
            b.app25->call_30cd4(0x7FD8u+i*4u,0x7FDCu+i*4u,(3u-i)*4u);
        store_le16(dg(m,0x7FE4u),0xFFFFu);
        store_le32(dg(m,0x7FE8u),load_le32(dg(m,0x7FE8u))-1u);
        break;
    }
    if(b.app26)function_141b8(m,*b.app26);
}
static_assert(0x1527Du-0x1520Cu==113u);

void function_15280(Machine& m) noexcept {
    const u16 token=app27_stream_word(m);
    s32 index=0;
    for(;;++index) {
        const s32 count=static_cast<s16>(load_le16(dg(m,0x258A2u)));
        if(index>=count)break;
        const u32 rec=0x14D80u+static_cast<u32>(index)*0x16u;
        if((*dg(m,rec+0x0Bu)&2u)!=0u)continue;
        const u16 type=load_le16(dg(m,rec+0x0Eu));
        if((token==0xFFFFu&&type==0x18u)||token==type) {
            store_le16(dg(m,rec+0x12u),0u);
            *dg(m,rec+0x0Bu)=static_cast<u8>(*dg(m,rec+0x0Bu)|2u);
        }
    }
    if(token!=0x29u)return;
    index=0;
    for(;;++index) {
        const s32 count=static_cast<s16>(load_le16(dg(m,0x258A2u)));
        if(index>=count)break;
        const u32 rec=0x14D80u+static_cast<u32>(index)*0x16u;
        if((*dg(m,rec+0x0Bu)&2u)!=0u)continue;
        if(load_le16(dg(m,rec+0x0Eu))==0x2Au)
            *dg(m,rec+0x0Bu)=static_cast<u8>(*dg(m,rec+0x0Bu)&0xFDu);
    }
}
static_assert(0x1531Fu-0x15280u==159u);

void function_15320(Machine& m,Application27Boundary& b) noexcept {
    if(!b.app25||!b.app25->call_2975c)return;
    const u32 graphic=load_le32(dg(m,0x258CCu))+5u;
    b.app25->call_2975c(0u,8u,0x0Cu,graphic,9u);
    b.app25->call_2975c(1u,8u,0x74u,graphic,9u);
    b.app25->call_2975c(2u,0xA6u,0x0Cu,graphic,9u);
    b.app25->call_2975c(3u,0xA6u,0x74u,graphic,9u);
}
static_assert(0x1538Bu-0x15320u==107u);

void function_1538c(Machine& m,s32 root,s32 target) noexcept {
    if(static_cast<s16>(root)==static_cast<s16>(target)) {
        store_le32(dg(m,0x14020u),load_le32(dg(m,0x14020u))+1u);
        return;
    }
    const s32 item=static_cast<s16>(root);
    const u32 rec=load_le32(dg(m,0x255D8u))+static_cast<u32>(item*65);
    if(*dg(m,rec+0x2Fu)!=1u)return;
    const s32 group=static_cast<s8>(*dg(m,rec+0x31u));
    const u32 g=static_cast<u32>(group*10);
    const u32 list=load_le32(dg(m,0x7335u+g));
    const u32 count=*dg(m,0x7330u+g);
    for(u32 i=0u;i<count;++i) {
        const s32 child=static_cast<s16>(load_le16(dg(m,list+i*2u)));
        if(child==-1)continue;
        function_1538c(m,child,target);
    }
}
static_assert(0x153F9u-0x1538Cu==109u);

[[nodiscard]] u32 function_153fc(Machine& m) noexcept {
    store_le32(dg(m,0x14020u),0u);
    const s32 cursor=static_cast<s16>(load_le16(dg(m,0x8600u)));
    if(cursor!=-1&&function_0f390(m,cursor,0x75)!=0u)
        store_le32(dg(m,0x14020u),load_le32(dg(m,0x14020u))+1u);
    for(u32 actor=0u;actor<0x920u;actor+=0x248u) {
        if((load_le16(dg(m,0x218F0u+actor))&0x8000u)!=0u)continue;
        for(u32 i=0u;i<0x15u;++i) {
            const s32 item=static_cast<s16>(load_le16(dg(m,0x21920u+actor+i*2u)));
            if(item==-1)continue;
            function_1538c(m,item,0x75);
        }
    }
    return load_le32(dg(m,0x14020u))==0x0Fu?1u:0u;
}
static_assert(0x1548Cu-0x153FCu==144u);

// -----------------------------------------------------------------------------
// Batch 28: 1548C..15E37 42-way cinematic/script dispatcher and execution
// wrappers.  The 42-entry CS table at 1548C and compiler separators are
// non-code/data in RESIDUAL_LEDGER.md.
// -----------------------------------------------------------------------------

struct Application28Boundary {
    Application20Boundary* app20=nullptr;
    Application25Boundary* app25=nullptr;
    Application26Boundary* app26=nullptr;
    Application27Boundary* app27=nullptr;
    void (*call_1c1a0)(u32)=nullptr;
    u32 (*call_30ca2_noargs)()=nullptr;
    u32 (*call_2f29c)(u32,u32)=nullptr;
    void (*call_2e538)(u32)=nullptr;
    void (*call_296c0)(u32,u32,u32,u32)=nullptr;
    void (*call_297d0)(u32,u32,u32,u32)=nullptr;
};

static u16 app28_stream_word(Machine& m) noexcept {
    const u32 index=load_le32(dg(m,0x7FECu));
    const u16 value=load_le16(dg(m,0x13020u+index*2u));
    store_le32(dg(m,0x7FECu),index+1u);
    return value;
}
static void app28_skip_or_jump(Machine& m,bool take) noexcept {
    const u32 index=load_le32(dg(m,0x7FECu));
    if(take)store_le32(dg(m,0x7FECu),load_le16(dg(m,0x13020u+index*2u)));
    else store_le32(dg(m,0x7FECu),index+1u);
}
static u32 app28_bit(u32 index) noexcept { return 1u<<(index&31u); }

[[nodiscard]] u32 function_15534(Machine& m,Application28Boundary& b) noexcept {
    if(*dg(m,0x8101u)!=0u)return 3u;
    const u32 opcode=app28_stream_word(m);
    if(opcode>0x29u)return 0u;

    switch(opcode) {
        case 0x00u: {
            const u32 count=app28_stream_word(m);
            store_le32(dg(m,0x7FE8u),count);
            for(u32 i=0u;i<count;++i)
                store_le16(dg(m,0x7FD8u+i*4u),app28_stream_word(m));
            return 0u;
        }
        case 0x01u: {
            const u32 count=app28_stream_word(m);
            for(u32 i=0u;i<count;++i)
                store_le16(dg(m,0x7FDAu+i*4u),app28_stream_word(m));
            return 0u;
        }
        case 0x02u:
            if(b.app26)function_146dc(m,*b.app26);
            return 0u;
        case 0x03u: {
            const u16 value=app28_stream_word(m);
            const u32 index=app28_stream_word(m);
            store_le16(dg(m,0x7FDAu+index*4u),value);
            return 0u;
        }
        case 0x04u: {
            const u16 value=app28_stream_word(m);
            const u32 index=app28_stream_word(m);
            store_le16(dg(m,0x7FD8u+index*4u),value);
            if(b.app26)function_141b8(m,*b.app26);
            return 0u;
        }
        case 0x05u:
            if(b.app27)function_14fc0(m,*b.app27);
            return 0u;
        case 0x06u: {
            const u32 item=app28_stream_word(m);
            if(b.app27)function_15050(m,*b.app27,item);
            return 0u;
        }
        case 0x07u:
            return 1u;
        case 0x08u: {
            const u16 item=load_le16(dg(m,0x13020u+load_le32(dg(m,0x7FECu))*2u));
            bool take=false;
            if(item==0xFFFFu) {
                store_le32(dg(m,0x7FECu),load_le32(dg(m,0x7FECu))+1u);
                take=function_153fc(m)!=0u;
            } else {
                (void)app28_stream_word(m);
                take=function_0f4b0(m,static_cast<s32>(item))!=0u;
            }
            app28_skip_or_jump(m,take);
            return 0u;
        }
        case 0x09u: {
            const u32 resource=app28_stream_word(m);
            const bool take=b.app26&&b.app26->call_1c1dc&&b.app26->call_1c1dc(resource)!=0u;
            app28_skip_or_jump(m,take);
            return 0u;
        }
        case 0x0Au: {
            const u32 resource=app28_stream_word(m);
            if(b.app26&&b.app26->call_1c124)b.app26->call_1c124(resource);
            return 0u;
        }
        case 0x0Bu: {
            const u32 resource=app28_stream_word(m);
            if(b.app27&&b.app27->call_1c160)b.app27->call_1c160(resource,m.cpu.ebx);
            return 0u;
        }
        case 0x0Cu: {
            const u32 resource=app28_stream_word(m);
            if(b.call_1c1a0)b.call_1c1a0(resource);
            return 0u;
        }
        case 0x0Du: {
            const u32 resource=app28_stream_word(m);
            const bool take=!(b.app26&&b.app26->call_1c1dc&&b.app26->call_1c1dc(resource)!=0u);
            app28_skip_or_jump(m,take);
            return 0u;
        }
        case 0x0Eu:
            return 2u;
        case 0x0Fu: {
            const u32 timer=app28_stream_word(m);
            const u32 target=app28_stream_word(m);
            const u32 period=app28_stream_word(m);
            const u32 base=timer*0x0Cu;
            store_le32(dg(m,0x14024u+base),target);
            store_le32(dg(m,0x14028u+base),period);
            const u32 now=b.app25&&b.app25->call_30700?b.app25->call_30700():0u;
            store_le32(dg(m,0x1402Cu+base),period+now);
            if(target==0xFFFFu)store_le32(dg(m,0x14024u+base),0xFFFFFFFFu);
            return 0u;
        }
        case 0x10u: {
            if(b.app25)app25_select_page(m,*b.app25);
            const u32 p=load_le32(dg(m,0x7FECu));
            const u32 graphic=load_le32(dg(m,0x258CCu))+load_le32(dg(m,0x7FF0u))+
                              load_le16(dg(m,0x13020u+p*2u));
            const u32 x=load_le16(dg(m,0x13022u+p*2u));
            const u32 y=load_le16(dg(m,0x13024u+p*2u));
            if(b.app25&&b.app25->call_2e510)b.app25->call_2e510(x,y,graphic,9u);
            store_le32(dg(m,0x7FECu),p+3u);
            return 0u;
        }
        case 0x11u: {
            if(b.app25)app25_select_page(m,*b.app25);
            if(b.app27)function_15320(m,*b.app27);
            if(*dg(m,0x8102u)==0u) {
                if(b.app27&&b.app27->call_1a510)b.app27->call_1a510(load_le32(dg(m,0xC050u)));
                *dg(m,0xC050u)^=1u;
                if(b.app27&&b.app27->call_1a9f8)b.app27->call_1a9f8();
            } else if(b.app25&&b.app25->call_1a8c8) {
                b.app25->call_1a8c8(load_le32(dg(m,0xC050u)),8u,0xA0u,0xD2u,0xA0u);
            }
            if(*dg(m,0x7FB8u)!=0u) {
                if(b.app25&&b.app25->call_2e614)b.app25->call_2e614(4u,0u);
                if(b.app25&&b.app25->call_2e608)b.app25->call_2e608();
            }
            return 0u;
        }
        case 0x12u: {
            *dg(m,0x7FB8u)=1u;
            const u32 target=app28_stream_word(m);
            const u32 resume=load_le32(dg(m,0x7FECu));
            store_le32(dg(m,0x7FECu),target);
            while(function_15534(m,b)==0u){}
            *dg(m,0x7FB8u)=0u;
            store_le32(dg(m,0x7FECu),resume);
            return 0u;
        }
        case 0x13u:
            if(b.app27)function_15168(m,*b.app27,0u);
            return 0u;
        case 0x14u:
            if(b.app27)function_1520c(m,*b.app27);
            return 0u;
        case 0x15u:
            *dg(m,0x8100u)=1u;
            return 0u;
        case 0x16u:
            function_15280(m);
            return 0u;
        case 0x17u:
            store_le32(dg(m,0x7FF0u),app28_stream_word(m));
            return 0u;
        case 0x18u:
            app28_skip_or_jump(m,true);
            return 0u;
        case 0x19u:
            store_le32(dg(m,0x7FF4u),app28_stream_word(m));
            return 0u;
        case 0x1Au:
            if(b.app27)function_15168(m,*b.app27,1u);
            return 0u;
        case 0x1Bu:
            return 3u;
        case 0x1Cu: {
            const u32 token=app28_stream_word(m);
            if(token!=3u&&token!=0x88u&&b.app26&&b.app26->call_27f24)
                b.app26->call_27f24(token+load_le32(dg(m,0x258C8u)));
            return 0u;
        }
        case 0x1Du: {
            const u32 arg=app28_stream_word(m);
            if(load_le32(dg(m,0x7FD4u))!=0x1Au) {
                if(b.app25&&b.app25->call_28ef0)b.app25->call_28ef0();
                if(b.app25&&b.app25->call_29008)b.app25->call_29008(0x64u);
                if(b.app25&&b.app25->call_28f58)b.app25->call_28f58(0x37u);
                if(b.app25&&b.app25->call_28e58)b.app25->call_28e58(static_cast<s32>(arg),1,0x9C4u);
            }
            return 0u;
        }
        case 0x1Eu: {
            const u32 bit=app28_stream_word(m);
            store_le32(dg(m,0x7FD0u),load_le32(dg(m,0x7FD0u))|app28_bit(bit));
            return 0u;
        }
        case 0x1Fu: {
            const u32 bit=app28_stream_word(m);
            store_le32(dg(m,0x7FD0u),load_le32(dg(m,0x7FD0u))&~app28_bit(bit));
            return 0u;
        }
        case 0x20u: {
            const u32 bit=app28_stream_word(m);
            store_le32(dg(m,0x7FD0u),load_le32(dg(m,0x7FD0u))^app28_bit(bit));
            return 0u;
        }
        case 0x21u: {
            const u32 bit=app28_stream_word(m);
            app28_skip_or_jump(m,(load_le32(dg(m,0x7FD0u))&app28_bit(bit))!=0u);
            return 0u;
        }
        case 0x22u: {
            const u32 bit=app28_stream_word(m);
            app28_skip_or_jump(m,(load_le32(dg(m,0x7FD0u))&app28_bit(bit))==0u);
            return 0u;
        }
        case 0x23u: {
            if(b.app25)app25_select_page(m,*b.app25);
            const u32 p=load_le32(dg(m,0x7FECu));
            const u32 scale=b.call_30ca2_noargs?b.call_30ca2_noargs():0u;
            const u32 scaled=(static_cast<u32>(load_le16(dg(m,0x13022u+p*2u)))*scale)>>15u;
            const u32 graphic=load_le32(dg(m,0x258CCu))+load_le32(dg(m,0x7FF0u))+
                              load_le16(dg(m,0x13020u+p*2u))+scaled;
            const u32 x=load_le16(dg(m,0x13024u+p*2u));
            const u32 y=load_le16(dg(m,0x13026u+p*2u));
            if(b.app25&&b.app25->call_2e510)b.app25->call_2e510(x,y,graphic,9u);
            store_le32(dg(m,0x7FECu),p+4u);
            return 0u;
        }
        case 0x24u: {
            const u32 mode=app28_stream_word(m);
            if(b.app26)(void)function_13ef8(m,*b.app26,mode);
            *dg(m,0x8100u)=1u;
            return 3u;
        }
        case 0x25u: {
            if(b.app25)app25_select_page(m,*b.app25);
            const u32 p=load_le32(dg(m,0x7FECu));
            const u32 graphic=load_le32(dg(m,0x258CCu))+load_le32(dg(m,0x7FF0u))+
                              load_le16(dg(m,0x13020u+p*2u));
            const u32 sprite=b.call_2f29c?b.call_2f29c(graphic,9u):0u;
            if(b.call_2e538)b.call_2e538(sprite);
            const u32 x=load_le16(dg(m,0x13022u+p*2u));
            const u32 y=load_le16(dg(m,0x13024u+p*2u));
            if(b.app25&&b.app25->call_2e510)b.app25->call_2e510(x,y,graphic,9u);
            if(b.call_2e538)b.call_2e538(sprite);
            store_le32(dg(m,0x7FECu),p+3u);
            return 0u;
        }
        case 0x26u: {
            if(b.call_296c0) {
                *dg(m,0xA6BAu)=1u; *dg(m,0xA6BBu)=0u;
                b.call_296c0(8u,0x0Cu,0xCFu,0x9Bu);
            } else { *dg(m,0xA6BAu)=1u; *dg(m,0xA6BBu)=0u; }
            if(b.app25)app25_select_page(m,*b.app25);
            const u32 p=load_le32(dg(m,0x7FECu));
            const u32 graphic=load_le32(dg(m,0x258CCu))+load_le32(dg(m,0x7FF0u))+
                              load_le16(dg(m,0x13020u+p*2u));
            const u32 x=load_le16(dg(m,0x13022u+p*2u));
            const u32 y=load_le16(dg(m,0x13024u+p*2u));
            if(b.call_297d0)b.call_297d0(x,y,graphic,9u);
            if(b.call_296c0)b.call_296c0(0u,0u,0x18Fu,0x13Fu);
            *dg(m,0xA6BAu)=0u; *dg(m,0xA6BBu)=0u;
            store_le32(dg(m,0x7FECu),p+3u);
            return 0u;
        }
        case 0x27u:
            store_le32(dg(m,0x7FF8u),app28_stream_word(m));
            return 0u;
        case 0x28u: {
            const s32 item=static_cast<s32>(app28_stream_word(m));
            if(b.app20)function_0f540(m,*b.app20,item);
            return 0u;
        }
        case 0x29u: {
            const u32 item=app28_stream_word(m);
            if(b.app25&&b.app25->call_2195c)
                b.app25->call_2195c(0x80u,item,0u,load_le32(dg(m,0x255D0u)),
                                     load_le32(dg(m,0x255D4u)),1u,0u);
            return 0u;
        }
    }
    return 0u;
}
static_assert(0x15DF5u-0x15534u==2241u);

void function_15df8(Machine& m,Application28Boundary& b,u32 target) noexcept {
    const u32 saved=load_le32(dg(m,0x7FECu));
    store_le32(dg(m,0x7FECu),target);
    while(function_15534(m,b)==0u){}
    store_le32(dg(m,0x7FECu),saved);
}
static_assert(0x15E21u-0x15DF8u==41u);

void function_15e24(Machine& m,Application28Boundary& b,u32 target) noexcept {
    store_le32(dg(m,0x7FECu),target);
    while(function_15534(m,b)==0u){}
}
static_assert(0x15E37u-0x15E24u==19u);

// -----------------------------------------------------------------------------
// Batch 29: 15E38..16A84 choice presentation/timing, party-panel rendering,
// cinematic mode execution, record conversion, and early resource-table loads.
// 16400 owns the backward blocks at 15F7E..16033; 163C4 falls through into it.
// Compiler separators and the bounded zero padding at 16806..1680F are listed
// as non-code/data in RESIDUAL_LEDGER.md.
// -----------------------------------------------------------------------------

struct Application29Boundary {
    Application28Boundary* app28=nullptr;
    void (*call_1a5b0)(u32,s32,u32)=nullptr;
    s32 (*call_1f3fc)(u32)=nullptr;
    void (*call_298f0)(u32,u32,u32,u32,u32,u32)=nullptr;
    void (*divide_error)()=nullptr;
    u32 (*call_312ec_local)(u8*,u32,u32)=nullptr;
};

void function_16400(Machine& m,Application29Boundary& b) noexcept;

void function_15e38(Machine& m,Application29Boundary& b,u32 choice) noexcept {
    if(!b.app28||!b.app28->app25)return;
    Application25Boundary& a25=*b.app28->app25;
    Application26Boundary* a26=b.app28->app26;

    const u32 random_value=b.app28->call_30ca2_noargs?b.app28->call_30ca2_noargs():0u;
    const s32 limit=static_cast<s32>((random_value*6u)>>15u)+4;
    s32 last=0;
    for(s32 i=0;i<limit;++i) {
        const s32 slot=i%4;
        const u32 actor=static_cast<u32>(slot)*0x248u;
        if((load_le16(dg(m,0x218F0u+actor))&0xD040u)==0u)last=i;
    }
    const s32 selected=last%4;
    if(a25.call_291b0)a25.call_291b0(*dg(m,0xC050u));
    for(s32 i=0;i<4;++i)
        if(i!=selected)store_le32(dg(m,0x7FBCu+static_cast<u32>(i)*4u),1u);

    function_16400(m,b);
    if(a25.call_30d30)a25.call_30d30(0x7FBCu,0u,0x10u);
    if(a25.call_2e510)a25.call_2e510(0xDCu,0u,load_le32(dg(m,0x258CCu))+2u,9u);
    const u32 actor=0x218E0u+static_cast<u32>(selected)*0x248u;
    if(a25.call_30508)a25.call_30508(0xE7u,0u,0x140u,actor,1u);
    if(a26&&a26->call_30354)a26->call_30354(0x3Au,1u);
    if(a26)(void)function_13fd4(m,*a26,0u,0xF6,0x1F,0x0A,0x10A,
                                0x12C20u+(choice<<8u),1u);
    store_le32(dg(m,0x7FFCu),static_cast<u32>(selected));
    if(b.call_1a5b0)b.call_1a5b0(load_le32(dg(m,0xC050u)),
                                  static_cast<s16>(load_le16(dg(m,0x8600u))),
                                  *dg(m,0x8602u));
    *dg(m,0xC050u)^=1u;
    if(a25.call_291b0)a25.call_291b0(*dg(m,0xC050u));
}
static_assert(0x15F7Eu-0x15E38u==326u);

// 15F7E..16033 is the backward continuation of function_16400.
static_assert(0x16034u-0x15F7Eu==182u);

void function_16034(Machine& m,Application29Boundary& b) noexcept {
    if(!b.app28||!b.app28->app25)return;
    Application25Boundary& a25=*b.app28->app25;
    for(u32 off=0u;off<0x3Cu;off+=0x0Cu) {
        const u32 target=load_le32(dg(m,0x14024u+off));
        if(target==0xFFFFFFFFu)continue;
        const u32 now=a25.call_30700?a25.call_30700():0u;
        if(now<=load_le32(dg(m,0x1402Cu+off)))continue;
        const u32 saved=load_le32(dg(m,0x7FECu));
        store_le32(dg(m,0x7FECu),target);
        while(function_15534(m,*b.app28)==0u){}
        store_le32(dg(m,0x7FECu),saved);
        const u32 next=a25.call_30700?a25.call_30700():0u;
        store_le32(dg(m,0x1402Cu+off),next+load_le32(dg(m,0x14028u+off)));
    }
}
static_assert(0x1608Au-0x16034u==86u);

void function_1608c(Machine& m,Application29Boundary& b) noexcept {
    if(!b.app28||!b.app28->app25||!b.app28->app26)return;
    Application25Boundary& a25=*b.app28->app25;
    Application26Boundary& a26=*b.app28->app26;
    function_141b8(m,a26);
    function_14218(m,a26,-1,1u);
    if(b.call_1a5b0)b.call_1a5b0(load_le32(dg(m,0xC050u)),
                                  static_cast<s16>(load_le16(dg(m,0x8600u))),
                                  *dg(m,0x8602u));
    *dg(m,0xC050u)^=1u;
    if(a25.call_291b0)a25.call_291b0(*dg(m,0xC050u));
    *dg(m,0x8102u)=1u;

    const s32 sentinel=-1;
    s32 selected=-1;
    for(;;) {
        if(!a25.call_1a1f4)break;
        while(a25.call_1a1f4()!=0u)function_16034(m,b);
        u32 event=0u;
        while((event=a25.call_1a1f4())==0u) {
            function_16034(m,b);
            const s32 y=static_cast<s16>(load_le16(dg(m,0x9E33u)));
            s32 x=static_cast<s16>(load_le16(dg(m,0x9E31u)));
            s32 hover=sentinel;
            // The encoded signed tests on 9E33 are tautological: a value below
            // 17h cannot simultaneously be greater than 129h.
            if(!(y<0x17&&y>0x129)&&x>=0xDD) {
                const s32 lower=0x12A-(4-static_cast<s32>(load_le32(dg(m,0x7FE8u))))*0x16;
                if(x<lower) {
                    const s32 q=(x-0xDD)/0x16;
                    if(q>=0&&q<=3&&load_le16(dg(m,0x7FD8u+static_cast<u32>(q)*4u))!=0xFFFFu)
                        hover=q;
                }
            }
            if(hover==selected)continue;
            function_14218(m,a26,hover,1u);
            if(a25.call_1a8c8)a25.call_1a8c8(load_le32(dg(m,0xC050u)),0xDCu,0u,0x50u,0x140u);
            selected=hover;
        }
        if((event&2u)!=0u)break;
        if(selected<0||selected>=static_cast<s32>(load_le32(dg(m,0x7FE8u))))continue;

        function_15e38(m,b,static_cast<u32>(selected));
        store_le32(dg(m,0x7FECu),load_le16(dg(m,0x7FDAu+static_cast<u32>(selected)*4u)));
        while(function_15534(m,*b.app28)==0u){}
        if(*dg(m,0x8101u)!=0u||*dg(m,0x8100u)!=0u)break;
        function_16400(m,b);
        function_14218(m,a26,sentinel,1u);
        if(b.call_1a5b0)b.call_1a5b0(load_le32(dg(m,0xC050u)),
                                      static_cast<s16>(load_le16(dg(m,0x8600u))),
                                      *dg(m,0x8602u));
        store_le32(dg(m,0xC050u),load_le32(dg(m,0xC050u))^1u);
        selected=sentinel;
        if(a25.call_291b0)a25.call_291b0(*dg(m,0xC050u));
    }
    if(a25.call_22d70)a25.call_22d70();
}
static_assert(0x1624Bu-0x1608Cu==447u);

void function_1624c(Machine& m,Application29Boundary& b) noexcept {
    if(!b.app28||!b.app28->app25)return;
    Application25Boundary& a25=*b.app28->app25;
    s32 index=0;
    u32 off=0u;
    for(;;) {
        const s32 count=static_cast<s16>(load_le16(dg(m,0x258A6u)));
        if(index>=count)return;
        if(load_le16(dg(m,0x22246u+off))==0x4Eu) {
            static constexpr u32 items[5]={0x52u,0x5Cu,0x5Fu,0x66u,0x69u};
            for(u32 item:items)if(a25.call_2195c)
                a25.call_2195c(0x80u,item,0u,load_le32(dg(m,0x2223Cu+off)),
                                load_le32(dg(m,0x22238u+off)),0u,0u);
            const u32 bytes=static_cast<u32>(count-index+1)*0x16u;
            if(a25.call_30cd4)a25.call_30cd4(0x22238u+off,
                                             0x22238u+static_cast<u32>(index+1)*0x16u,
                                             bytes);
            return;
        }
        ++index;
        off+=0x16u;
    }
}
static_assert(0x1635Cu-0x1624Cu==272u);

void function_1635c(Machine& m,Application29Boundary& b) noexcept {
    if(!b.app28)return;
    u32 result=0u;
    do { result=function_15534(m,*b.app28); } while(result!=2u&&result!=3u);
    if(result!=3u) {
        const u32 saved=load_le32(dg(m,0x7FECu));
        if(b.app28->app27)function_15320(m,*b.app28->app27);
        function_1608c(m,b);
        store_le32(dg(m,0x7FECu),saved);
        do { result=function_15534(m,*b.app28); } while(result!=3u);
    }
    if(load_le32(dg(m,0x7FD4u))==0x13u&&b.app28->app26&&
       b.app28->app26->call_1c1dc&&b.app28->app26->call_1c1dc(0x2Eu)==1u)
        function_1624c(m,b);
}
static_assert(0x163C4u-0x1635Cu==104u);

void function_163c4(Machine& m,Application29Boundary& b) noexcept {
    if(!b.app28||!b.app28->app25)return;
    Application25Boundary& a25=*b.app28->app25;
    if(a25.call_291b0)a25.call_291b0(*dg(m,0xC050u));
    if(a25.call_2e510) {
        a25.call_2e510(0u,0u,load_le32(dg(m,0x258CCu)),9u);
        a25.call_2e510(0xC8u,0u,load_le32(dg(m,0x258CCu))+1u,9u);
    }
    function_16400(m,b);
}
static_assert(0x16400u-0x163C4u==60u);

static s32 app29_sar1(s32 v) noexcept {
    return v/2-((v<0&&(v%2)!=0)?1:0);
}

void function_16400(Machine& m,Application29Boundary& b) noexcept {
    if(!b.app28||!b.app28->app25)return;
    Application25Boundary& a25=*b.app28->app25;
    if(a25.call_291b0)a25.call_291b0(*dg(m,0xC050u));
    s32 ybase=0x0D;
    for(u32 slot=0u;slot<4u;++slot,ybase+=0x50) {
        const u32 actor=slot*0x248u;
        const u16 state=load_le16(dg(m,0x218F0u+actor));
        if(state>=0x8000u)continue;
        s32 graphic=static_cast<s32>(load_le32(dg(m,0x258E4u)))+
                    static_cast<s8>(*dg(m,0x218F2u+actor))+
                    static_cast<s32>(*dg(m,0x218F3u+actor))*0x17;
        if((state&0x0100u)!=0u)graphic+=0x2E;
        s32 portrait=b.call_1f3fc?b.call_1f3fc(slot):0;
        if(load_le32(dg(m,0x7FBCu+slot*4u))!=0u)portrait=0;
        const s32 y=ybase+(slot>=2u?1:0);
        if(b.call_298f0)b.call_298f0(0u,0x136u,static_cast<u32>(y),
                                     static_cast<u32>(graphic),1u,
                                     0xCF70u+(static_cast<u32>(portrait)<<8u));
        if((state&0x4000u)!=0u)continue;
        if(a25.call_29b40)a25.call_29b40(0x136u,static_cast<u32>(y+0x30),0x50u,6u,0u);
        const s32 maxhp=static_cast<s16>(load_le16(dg(m,0x218FEu+actor)));
        const s32 half=app29_sar1(maxhp);
        const u32 wrapped=static_cast<u32>(function_0ef14(m,slot))*0x50u+static_cast<u32>(half);
        const s32 numerator=static_cast<s32>(wrapped);
        if(maxhp==0||(numerator==static_cast<s32>(0x80000000u)&&maxhp==-1)) {
            if(b.divide_error)b.divide_error();
            return;
        }
        const s32 amount=numerator/maxhp;
        const s32 bary=y+0x31;
        if(a25.call_29b40) {
            if(amount<=0x50) {
                const s32 x=0x136+(0x50-amount);
                const u32 color=amount>0x14?0x43u:0x57u;
                a25.call_29b40(static_cast<u32>(x),static_cast<u32>(bary),
                                static_cast<u32>(amount),4u,color);
            } else {
                a25.call_29b40(0x136u,static_cast<u32>(bary),0x50u,4u,0x1Bu);
            }
        }
    }
    if(a25.call_2975c)a25.call_2975c(0u,0x136u,0x8Eu,load_le32(dg(m,0x258CCu))+6u,9u);
    u32 y=0u;
    for(u32 slot=0u;slot<4u;++slot,y+=0x50u) {
        const u32 actor=slot*0x248u;
        if((load_le16(dg(m,0x218F0u+actor))&0x8000u)==0u)continue;
        if(a25.call_2e510)a25.call_2e510(0x134u,y+slot/2u+0x0Bu,
                                         load_le32(dg(m,0x258CCu))+4u,9u);
    }
}
static_assert(0x16524u-0x16400u==292u);

void function_16524(Machine& m,Application29Boundary& b,u32 mode) noexcept {
    if(!b.app28||!b.app28->app25||!b.app28->app26)return;
    Application25Boundary& a25=*b.app28->app25;
    Application26Boundary& a26=*b.app28->app26;
    s32 saved50=-1;
    s32 saved48=-1;
    u32 savedA0=0u;
    u32 saved98=0u;
    if(mode!=0x1Cu) {
        saved50=a26.call_29050?a26.call_29050():-1;
        saved48=static_cast<s32>(a26.call_29048?a26.call_29048():0u);
        savedA0=a26.call_290a0?a26.call_290a0():0u;
        saved98=a26.call_29098?a26.call_29098():0u;
        if(a25.call_28dcc)a25.call_28dcc(0x9C4u);
    }
    const u8 savedBF38=*dg(m,0xBF38u);
    *dg(m,0xBF38u)=0u;
    const u32 saved26=a26.call_1c1dc?a26.call_1c1dc(0x26u):0u;
    if(a25.call_19ccc)a25.call_19ccc(0u);
    *dg(m,0x8100u)=0u;
    store_le32(dg(m,0x7FF0u),0u);
    store_le32(dg(m,0x7FF8u),0u);
    *dg(m,0x8102u)=0u;
    store_le32(dg(m,0x7FD0u),0u);
    *dg(m,0x8101u)=0u;
    *dg(m,0x7FB8u)=0u;
    store_le16(dg(m,0xA6BAu),1u);
    const u8 savedD680=*dg(m,0xD680u);
    *dg(m,0xD680u)=2u;
    if(b.app28->call_296c0)b.app28->call_296c0(0u,0u,0x18Fu,0x13Fu);
    store_le16(dg(m,0xA6BAu),0u);
    store_le16(dg(m,0xA6BEu),0u);
    store_le16(dg(m,0xA6BCu),0u);

    for(u32 off=0u;off<0x3Cu;off+=0x0Cu)store_le32(dg(m,0x14024u+off),0xFFFFFFFFu);
    for(u32 i=0u;i<0x100u;++i)*dg(m,0x8000u+i)=static_cast<u8>(i);
    for(u32 off=0u;off<0x10u;off+=4u)store_le16(dg(m,0x7FD8u+off),0xFFFFu);
    *dg(m,0x801Bu)=0x45u;
    store_le32(dg(m,0x7FD4u),mode);
    function_13f70(m,a26,mode);
    if(a25.call_291b0)a25.call_291b0(*dg(m,0xC050u));
    if(a25.call_2e510) {
        a25.call_2e510(0u,0u,load_le32(dg(m,0x258CCu)),9u);
        a25.call_2e510(0xC8u,0u,load_le32(dg(m,0x258CCu))+1u,9u);
    }
    function_16400(m,b);
    function_1635c(m,b);

    if(mode==0x1Fu&&a26.call_1c1dc&&a26.call_1c1dc(0x3Fu)!=0u) {
        const s32 index=static_cast<s16>(load_le16(dg(m,0x25619u)));
        const u32 addr=static_cast<u32>(static_cast<s32>(0x19972)+index*0xE2);
        *dg(m,addr)=static_cast<u8>(*dg(m,addr)&0xC0u);
    }
    if(mode==0x14u&&saved26==0u&&a26.call_1c1dc&&a26.call_1c1dc(0x26u)==1u) {
        store_le16(dg(m,0x255D2u),0x3Cu); store_le16(dg(m,0x255D6u),4u);
        store_le16(dg(m,0x255D4u),0x8000u); store_le16(dg(m,0x255D0u),0x8000u);
        store_le32(dg(m,0xC658u),0xB4u);
        saved50=-1;
        if(a26.call_181b4)a26.call_181b4(6u);
    }
    if(mode==0x12u&&a26.call_1c1dc&&a26.call_1c1dc(0x2Fu)==0u)
        store_le32(dg(m,0xBF44u),0x16u);
    if(mode==0x20u&&a26.call_1c1dc&&a26.call_1c1dc(0x4Eu)==1u) {
        if(b.app28->app27&&b.app28->app27->call_1c160)
            b.app28->app27->call_1c160(0x4Eu,mode); // live EBX supplies the second argument.
        if(a26.call_1c1dc(0x4Fu)==1u)store_le32(dg(m,0xBF44u),0x4Bu);
    }
    if(mode!=0x1Cu&&saved50!=-1&&saved48!=-1) {
        if(a25.call_28dcc)a25.call_28dcc(0x9C4u);
        if(a25.call_28ef0)a25.call_28ef0();
        if(a25.call_28f58)a25.call_28f58(savedA0);
        if(a25.call_29008)a25.call_29008(saved98);
        if(a25.call_28e58)a25.call_28e58(saved50,saved48,0x9C4u);
    }
    if(mode==0x25u&&b.app28->app27&&function_14a60(m,*b.app28->app27,0x6C8u)!=-1)
        store_le32(dg(m,0xBF44u),0x69u);
    if(a25.call_1f8fc)a25.call_1f8fc();
    if(a25.call_22d70)a25.call_22d70();
    *dg(m,0xBF38u)=savedBF38;
    *dg(m,0xD680u)=savedD680;
    if(a25.call_19cf4)a25.call_19cf4();
}
static_assert(0x16806u-0x16524u==738u);

void function_16810(Machine& m,Application29Boundary& b) noexcept {
    if(!b.app28||!b.app28->app25||!b.app28->app26)return;
    Application25Boundary& a25=*b.app28->app25;
    Application26Boundary& a26=*b.app28->app26;
    const u32 handle=static_cast<u32>(a26.call_312a0?a26.call_312a0(0x6F8u):-1);
    if(a25.call_312ec)a25.call_312ec(0x258F4u,4u,handle);
    const u32 count=load_le32(dg(m,0x258F4u));
    if(count==0u)return; // The encoded zero-count path does not close the handle.
    const u32 bytes=count*0x45u;
    const u32 ptr=a26.call_3104c?a26.call_3104c(bytes):0u;
    store_le32(dg(m,0x22230u),ptr);
    if(a25.call_312ec)a25.call_312ec(ptr,bytes,handle);
    if(a26.call_312dc)a26.call_312dc(handle);
}
static_assert(0x1686Bu-0x16810u==91u);

void function_1686c(Machine& m,Application29Boundary& b,u32 handle) noexcept {
    if(!b.app28||!b.app28->app25||!b.app28->app26)return;
    Application25Boundary& a25=*b.app28->app25;
    Application26Boundary& a26=*b.app28->app26;
    if(a25.call_312ec)a25.call_312ec(0x258E0u,2u,handle);
    const u32 count=load_le32(dg(m,0x258E0u)); // high word is intentionally whatever was already present.
    if(count==0u)return;
    const u32 bytes=count*7u;
    const u32 ptr=a26.call_3104c?a26.call_3104c(bytes):0u;
    store_le32(dg(m,0x255DCu),ptr);
    if(a25.call_312ec)a25.call_312ec(ptr,bytes,handle);
}
static_assert(0x168C7u-0x1686Cu==91u);

void function_168c8(Machine& m,Application29Boundary& b) noexcept {
    if(!b.app28||!b.app28->app25||!b.app28->app26)return;
    Application25Boundary& a25=*b.app28->app25;
    Application26Boundary& a26=*b.app28->app26;
    const u32 handle=static_cast<u32>(a26.call_312a0?a26.call_312a0(0x704u):-1);
    if(a25.call_312ec)a25.call_312ec(0x258E8u,4u,handle);
    const u32 count=load_le32(dg(m,0x258E8u));
    if(count==0u)return; // The encoded zero-count path also leaves this handle open.
    const u32 bytes=count*0x41u;
    const u32 ptr=a26.call_3104c?a26.call_3104c(bytes):0u;
    store_le32(dg(m,0x255D8u),ptr);
    if(a25.call_312ec)a25.call_312ec(ptr,bytes,handle);
    if(a26.call_312dc)a26.call_312dc(handle);
}
static_assert(0x16931u-0x168C8u==105u);

void function_16934(Machine& m,Application29Boundary& b,u32 handle) noexcept {
    if(!b.app28||!b.app28->app25||!b.app28->app26)return;
    Application25Boundary& a25=*b.app28->app25;
    Application26Boundary& a26=*b.app28->app26;
    if(a25.call_312ec)a25.call_312ec(0x258F0u,2u,handle);
    const u32 count=load_le32(dg(m,0x258F0u)); // two-byte read, full-dword use is intentional.
    if(count==0u)return;
    const u32 bytes=count*7u;
    const u32 ptr=a26.call_3104c?a26.call_3104c(bytes):0u;
    store_le32(dg(m,0x22234u),ptr);
    if(a25.call_312ec)a25.call_312ec(ptr,bytes,handle);
}
static_assert(0x1698Fu-0x16934u==91u);

void function_16990(Machine& m,Application29Boundary& b,u32 handle) noexcept {
    if(!b.app28||!b.app28->app25||!b.app28->app26)return;
    Application25Boundary& a25=*b.app28->app25;
    Application26Boundary& a26=*b.app28->app26;
    u8 count_bytes[2]={0u,0u};
    if(b.call_312ec_local)b.call_312ec_local(count_bytes,2u,handle);
    const s32 count=static_cast<s16>(load_le16(count_bytes));
    if(count==0)return;
    const u32 table=a26.call_3104c?a26.call_3104c(static_cast<u32>(count)*4u):0u;
    store_le32(dg(m,0x2B764u),table);
    for(s32 i=0;i<count;++i) {
        const u32 index=static_cast<u32>(i);
        store_le16(dg(m,0x2B750u+index*2u),0u);
        if(a25.call_312ec)a25.call_312ec(0x2B750u+index*2u,2u,handle);
        u8 nested_bytes[4]={0u,0u,0u,0u}; // assembly zeroes the containing dword before the 2-byte read.
        if(b.call_312ec_local)b.call_312ec_local(nested_bytes,2u,handle);
        const u32 nested=load_le32(nested_bytes);
        const u32 bytes=nested*8u;
        const u32 ptr=a26.call_3104c?a26.call_3104c(bytes):0u;
        store_le32(dg(m,table+index*4u),ptr);
        if(a25.call_312ec)a25.call_312ec(ptr,bytes,handle);
    }
}
static_assert(0x16A57u-0x16990u==199u);

[[nodiscard]] u32 function_16a58(Application29Boundary& b,u32 index,u32 handle) noexcept {
    if(!b.app28||!b.app28->app25)return handle;
    Application25Boundary& a25=*b.app28->app25;
    if(a25.call_3131c)(void)a25.call_3131c(index*2u,0u,handle);
    u8 alias[4]={static_cast<u8>(handle),static_cast<u8>(handle>>8u),
                 static_cast<u8>(handle>>16u),static_cast<u8>(handle>>24u)};
    // The original reads the word directly into the stack slot containing the
    // handle argument, so only its low 16 bits are replaced before EAX reload.
    if(b.call_312ec_local)b.call_312ec_local(alias,2u,handle);
    return load_le32(alias);
}
static_assert(0x16A85u-0x16A58u==45u);

// -----------------------------------------------------------------------------
// Batch 30: 16A88..171F8 actor-reset/resource-load helpers and checked writer.
// 16D04 preserves the original partial-word stack-local behavior: every 2-byte
// read starts from a dword preloaded with the file handle, so full-dword stores
// retain the handle's upper 16 bits.  17097..170AA is data/padding (including
// the referenced "levels.dat" string); compiler separators are ledgered.
// -----------------------------------------------------------------------------

struct Application30Boundary {
    Application29Boundary* app29=nullptr;
    void (*call_30d30)(u32,u32,u32)=nullptr;
    u32 (*call_dbd4)(u32)=nullptr;
    void (*call_03ea4)(u32,s32)=nullptr;
    void (*call_03de4)(u32,s32)=nullptr;
    u32 (*call_312a0)(u32)=nullptr;
    u32 (*call_3131c)(u32,u32,u32)=nullptr;
    u32 (*call_312ec)(u32,u32,u32)=nullptr;
    u32 (*call_312ec_local)(u8*,u32,u32)=nullptr;
    void (*call_312dc)(u32)=nullptr;
    void (*call_280dc)(u32)=nullptr;
    void (*call_288e4)(u32)=nullptr;
    void (*call_288f0)(u32)=nullptr;
    u32 (*call_31304)(u32,u32,u32)=nullptr;
    void (*call_2f580)(u32)=nullptr;
};

void function_16a88(Machine& m,u32 slot) noexcept {
    const u32 actor=slot*0x248u;
    for(u32 group=0u;group<5u;++group) {
        const u32 begin=group*4u;
        const u32 end=begin+4u;
        const u32 group_base=actor+group*0x0Au;
        for(u32 i=begin;i<end;++i) {
            const s32 mapped=static_cast<s8>(*dg(m,0x817Cu+i));
            if(mapped!=-1)*dg(m,0x2194Au+group_base+static_cast<u32>(mapped))=0u;
        }
    }
}
static_assert(0x16ADCu-0x16A88u==84u);

void function_16adc(Machine& m,Application30Boundary& b) noexcept {
    store_le16(dg(m,0x218F0u),0u);
    store_le16(dg(m,0x21B38u),0u);
    store_le16(dg(m,0x21D80u),0x8000u);
    store_le16(dg(m,0x21FC8u),0x8000u);
    if(b.call_30d30)b.call_30d30(0x219EBu,0u,0x21B28u-0x219EBu);
    if(b.call_30d30)b.call_30d30(0x21C33u,0u,0x21D70u-0x21C33u);

    for(u32 slot=0u;slot<4u;++slot) {
        const u32 actor=slot*0x248u;
        if(b.call_30d30)b.call_30d30(0x2194Au+actor,0xFFu,0x50u);
        if(b.call_30d30)b.call_30d30(0x2199Au+actor,0xFFu,0x50u);
        store_le32(dg(m,0x219F3u+actor),0u);
        store_le32(dg(m,0x219F7u+actor),0u);
        store_le32(dg(m,0x21A7Eu+actor),0u);
        store_le32(dg(m,0x21A82u+actor),0u);
        store_le32(dg(m,0x21A86u+actor),0u);
        store_le32(dg(m,0x21A0Bu+actor),0u);

        if((load_le16(dg(m,0x218F0u+actor))&0x8000u)!=0u)continue;
        *dg(m,0x218FBu+actor)=0x0Au;
        *dg(m,0x219EAu+actor)=static_cast<u8>(b.call_dbd4?b.call_dbd4(slot):0u);
        *dg(m,0x21A11u+actor)=0u;

        const s32 c0=static_cast<s8>(*dg(m,0x218F5u+actor));
        const s32 c1=static_cast<s8>(*dg(m,0x218F6u+actor));
        const s32 c2=static_cast<s8>(*dg(m,0x218F7u+actor));
        if(c0==1) {
            *dg(m,0x21A11u+actor)=static_cast<u8>(*dg(m,0x21A11u+actor)|4u);
            if(b.call_03ea4)b.call_03ea4(slot,static_cast<s16>(load_le16(dg(m,0x21900u+actor))));
        }
        if(c0==4||c1==4||c2==4) {
            store_le16(dg(m,0x21920u+actor),0x00A6u);
            *dg(m,0x21A11u+actor)=static_cast<u8>(*dg(m,0x21A11u+actor)|2u);
            function_16a88(m,slot);
        }
        const s32 cls[3]={c0,c1,c2};
        for(u32 i=0u;i<3u;++i) {
            if(cls[i]!=3)continue;
            *dg(m,0x21A11u+actor)=static_cast<u8>(*dg(m,0x21A11u+actor)|1u);
            if(b.call_03de4)b.call_03de4(slot,static_cast<s16>(load_le16(dg(m,0x21900u+actor+i*2u))));
        }
    }
}
static_assert(0x16CCDu-0x16ADCu==497u);

void function_16cd0(Machine& m,Application30Boundary& b) noexcept {
    const u32 handle=b.call_312a0?b.call_312a0(0x710u):0u;
    if(b.call_312ec)(void)b.call_312ec(0x218E0u,0x920u,handle);
    if(b.call_312dc)b.call_312dc(handle);
    function_16adc(m,b);
}
static_assert(0x16D03u-0x16CD0u==51u);

static u32 app30_read_handle_word(Application30Boundary& b,u32 handle,u32 offset) noexcept {
    if(b.call_3131c)(void)b.call_3131c(offset,0u,handle);
    u8 local[4]={static_cast<u8>(handle),static_cast<u8>(handle>>8),
                 static_cast<u8>(handle>>16),static_cast<u8>(handle>>24)};
    if(b.call_312ec_local)(void)b.call_312ec_local(local,2u,handle);
    return load_le32(local);
}

void function_16d04(Machine& m,Application30Boundary& b) noexcept {
    if(b.app29) {
        function_16810(m,*b.app29);
        function_168c8(m,*b.app29);
    }
    const u32 handle=b.call_312a0?b.call_312a0(0x718u):0u;

    const u32 v00=app30_read_handle_word(b,handle,0x00u); store_le32(dg(m,0x258DCu),v00);
    const u32 v02=app30_read_handle_word(b,handle,0x02u); store_le32(dg(m,0x258D8u),v02);
    const u32 v04=app30_read_handle_word(b,handle,0x04u); store_le32(dg(m,0x258E4u),v04);
    const u32 v06=app30_read_handle_word(b,handle,0x06u); store_le16(dg(m,0x27100u),static_cast<u16>(v06));
    const u32 v08=app30_read_handle_word(b,handle,0x08u); store_le16(dg(m,0x27102u),static_cast<u16>(v08));
    const u32 v0a=app30_read_handle_word(b,handle,0x0Au); store_le16(dg(m,0x27104u),static_cast<u16>(v0a));
    const u32 v0c=app30_read_handle_word(b,handle,0x0Cu); store_le16(dg(m,0x27106u),static_cast<u16>(v0c));
    const u32 v0e=app30_read_handle_word(b,handle,0x0Eu); store_le32(dg(m,0x258F8u),v0e);
    const u32 v10=app30_read_handle_word(b,handle,0x10u); // also copied to the final 288F0 argument slot.
    const u32 v12=app30_read_handle_word(b,handle,0x12u); store_le32(dg(m,0x258D0u),v12);
    const u32 v14=app30_read_handle_word(b,handle,0x14u); store_le32(dg(m,0x258C0u),v14);
    const u32 v16=app30_read_handle_word(b,handle,0x16u); store_le32(dg(m,0x258BCu),v16);
    const u32 v18=app30_read_handle_word(b,handle,0x18u); store_le32(dg(m,0x258D4u),v18);
    const u32 v1a=app30_read_handle_word(b,handle,0x1Au); store_le32(dg(m,0x258C8u),v1a);
    const u32 v1c=app30_read_handle_word(b,handle,0x1Cu); store_le32(dg(m,0x258ECu),v1c);
    const u32 v1e=app30_read_handle_word(b,handle,0x1Eu); store_le32(dg(m,0x258CCu),v1e);
    const u32 v20=app30_read_handle_word(b,handle,0x20u); store_le32(dg(m,0x25614u),v20);
    const u32 v22=app30_read_handle_word(b,handle,0x22u); store_le16(dg(m,0x7FCCu),static_cast<u16>(v22));

    if(b.app29) {
        function_1686c(m,*b.app29,handle);
        function_16934(m,*b.app29,handle);
        function_16990(m,*b.app29,handle);
    }
    if(b.call_312dc)b.call_312dc(handle);
    if(b.call_280dc)b.call_280dc(6u);
    if(b.call_288e4)b.call_288e4(5u);
    if(b.call_288f0)b.call_288f0(v10);
}
static_assert(0x17097u-0x16D04u==915u);

void function_170ac(Machine& m) noexcept {
    const s32 w=static_cast<s16>(load_le16(dg(m,0x25619u)));
    const s32 w2=w*2;
    store_le16(dg(m,0x27110u),static_cast<u16>(w-1));
    store_le16(dg(m,0x27112u),static_cast<u16>(w+1));
    store_le16(dg(m,0x27114u),static_cast<u16>(-1-w));
    store_le16(dg(m,0x27116u),static_cast<u16>(1-w));
    store_le16(dg(m,0x27130u),static_cast<u16>(w2-2));
    store_le16(dg(m,0x27132u),static_cast<u16>(w2+2));
    store_le16(dg(m,0x27134u),static_cast<u16>(-2-w2));
    store_le16(dg(m,0x27136u),static_cast<u16>(2-w2));
    store_le16(dg(m,0x27108u),0xFFFFu);
    store_le16(dg(m,0x27138u),static_cast<u16>(-1-w2));
    store_le16(dg(m,0x2712Cu),static_cast<u16>(w2));
    store_le16(dg(m,0x2712Eu),static_cast<u16>(w2));
    store_le16(dg(m,0x2713Au),static_cast<u16>(1-w2));
    store_le16(dg(m,0x2710Au),1u);
    store_le16(dg(m,0x2713Eu),static_cast<u16>(w2+1));
    store_le16(dg(m,0x27128u),0xFFFEu);
    store_le16(dg(m,0x2712Au),2u);
    store_le16(dg(m,0x27140u),static_cast<u16>(w-2));
    store_le16(dg(m,0x2710Cu),static_cast<u16>(-w));
    store_le16(dg(m,0x2710Eu),static_cast<u16>(w));
    store_le16(dg(m,0x27142u),static_cast<u16>(w+2));
    store_le16(dg(m,0x27144u),static_cast<u16>(-2-w));
    store_le16(dg(m,0x2713Cu),static_cast<u16>(w2-1));
    store_le16(dg(m,0x27146u),static_cast<u16>(2-w));
}
static_assert(0x171CFu-0x170ACu==291u);

void function_171d0(Machine& m,Application30Boundary& b,u32 src,u32 count,u32 handle) noexcept {
    const u32 wrote=b.call_31304?b.call_31304(src,count,handle):0u;
    if(wrote!=count&&b.call_2f580)b.call_2f580(load_le32(dg(m,0x8190u)));
}
static_assert(0x171F9u-0x171D0u==41u);

// -----------------------------------------------------------------------------
// Batch 31: 171FC..181B3 save serialization, actor state normalization, and
// the 49-way world-record resource gate. 179A2..17A67 is ledgered as the
// compiler separator plus the exact 49-entry CS dispatch table used at 17C50.
// -----------------------------------------------------------------------------

struct Application31Boundary {
    Application04Boundary* app04=nullptr;
    void (*call_31380)(u32)=nullptr;
    u32 (*call_312c8)(u32)=nullptr;
    u32 (*call_312b4)(u32)=nullptr;
    u32 (*call_3131c)(u32,u32,u32)=nullptr;
    u32 (*call_31304)(u32,u32,u32)=nullptr;
    u32 (*call_31304_local)(const u8*,u32,u32)=nullptr;
    void (*call_312dc)(u32)=nullptr;
    void (*call_2f580)(u32)=nullptr;
    u32 (*call_30ca2)()=nullptr;
    u32 (*call_1c1dc)(u32)=nullptr;
    void (*call_1c77c)(u32,u32)=nullptr;
    u32 (*call_2acdc)(u32)=nullptr;
    u32 (*call_1c328)(u32,u32,u32,u32)=nullptr;
};

static void app31_write_error(Machine& m,Application31Boundary& b) noexcept {
    if(b.call_2f580)b.call_2f580(load_le32(dg(m,0x8190u)));
}
static void app31_write_global(Machine& m,Application31Boundary& b,
                               u32 src,u32 count,u32 handle) noexcept {
    const u32 wrote=b.call_31304?b.call_31304(src,count,handle):0u;
    if(wrote!=count)app31_write_error(m,b);
}
static void app31_write_local16(Machine& m,Application31Boundary& b,
                                u16 value,u32 handle) noexcept {
    const u8 bytes[2]={static_cast<u8>(value),static_cast<u8>(value>>8)};
    const u32 wrote=b.call_31304_local?b.call_31304_local(bytes,2u,handle):0u;
    if(wrote!=2u)app31_write_error(m,b);
}
static void app31_write_local32(Machine& m,Application31Boundary& b,
                                u32 value,u32 handle) noexcept {
    const u8 bytes[4]={static_cast<u8>(value),static_cast<u8>(value>>8),
                       static_cast<u8>(value>>16),static_cast<u8>(value>>24)};
    const u32 wrote=b.call_31304_local?b.call_31304_local(bytes,4u,handle):0u;
    if(wrote!=4u)app31_write_error(m,b);
}

void function_171fc(Machine& m,Application31Boundary& b,s32 save_number) noexcept {
    if(save_number==-1||*dg(m,0xA79Du)!=0u)return;

    // The encoded filename digit extraction uses signed IDIV, so negative
    // values other than the -1 sentinel deliberately produce non-digit bytes.
    const s32 hundreds=save_number/100;
    const s32 ones=save_number%10;
    const s32 remainder=save_number-hundreds*100;
    const s32 tens=remainder/10;
    *dg(m,0x81A4u)=static_cast<u8>(hundreds+0x30);
    *dg(m,0x81A5u)=static_cast<u8>(tens+0x30);
    *dg(m,0x81A6u)=static_cast<u8>(ones+0x30);

    if(b.call_31380)b.call_31380(0x819Cu);
    const u32 old_handle=b.call_312c8?b.call_312c8(0x819Cu):0u;
    if(b.call_312dc)b.call_312dc(old_handle);
    const u32 handle=b.call_312b4?b.call_312b4(0x819Cu):0xFFFFFFFFu;
    if(handle==0xFFFFFFFFu&&b.call_2f580)b.call_2f580(load_le32(dg(m,0x8194u)));

    const u32 h0=0x1Du;
    if(b.call_3131c)(void)b.call_3131c(h0,0u,handle);
    app31_write_global(m,b,0x25618u,0x2A4u,handle);

    u32 h1=h0+0x2A4u;
    for(s32 row=0;;++row) {
        const s32 height=static_cast<s16>(load_le16(dg(m,0x2561Bu)));
        if(row>=height)break;
        const s32 width_for_src=static_cast<s16>(load_le16(dg(m,0x25619u)));
        const u32 src=static_cast<u32>(0x198E0 + row*width_for_src*2);
        const s32 width_for_write=static_cast<s16>(load_le16(dg(m,0x25619u)));
        const u32 bytes=static_cast<u32>(width_for_write*2);
        app31_write_global(m,b,src,bytes,handle);
        const s32 width_for_offset=static_cast<s16>(load_le16(dg(m,0x25619u)));
        h1+=static_cast<u32>(width_for_offset*2);
    }

    u32 h2=h1;
    for(s32 index=0;;++index) {
        const s32 width=static_cast<s16>(load_le16(dg(m,0x25619u)));
        if(index>=width)break;
        const u32 ptr=load_le32(dg(m,0xCB3Cu+static_cast<u32>(index)*4u));
        u32 count=0u;
        if(ptr!=0u) {
            for(u32 off=0u;;off+=0x0Au) {
                const u8 a=*dg(m,ptr+off);
                const u8 flags=*dg(m,ptr+off+1u);
                if(a==0u&&(flags&0x10u)==0u)break;
                ++count;
            }
        }
        app31_write_local16(m,b,static_cast<u16>(count),handle);
        h2+=2u;
        if(count!=0u) {
            const u32 bytes=count*0x0Au;
            app31_write_global(m,b,ptr,bytes,handle);
            h2+=bytes;
        }
    }

    app31_write_global(m,b,0xCF50u,1u,handle);
    ++h2;

    for(s32 index=0;;++index) {
        const s32 width=static_cast<s16>(load_le16(dg(m,0x25619u)));
        if(index>=width)break;
        const u32 ptr=load_le32(dg(m,0xCD3Cu+static_cast<u32>(index)*4u));
        u32 count=0u;
        if(ptr!=0u) {
            for(u32 off=0u;*dg(m,ptr+off)!=0u;off+=0x0Au)++count;
        }
        app31_write_local16(m,b,static_cast<u16>(count),handle);
        h2+=2u;
        if(count!=0u) {
            const u32 bytes=count*0x0Au;
            app31_write_global(m,b,ptr,bytes,handle);
            h2+=bytes;
        }
    }

    const s32 world_count_write=static_cast<s16>(load_le16(dg(m,0x258A2u)));
    app31_write_global(m,b,0x14D80u,static_cast<u32>(world_count_write*0x16),handle);
    const s32 world_count_offset=static_cast<s16>(load_le16(dg(m,0x258A2u)));
    const u32 h3=h2+static_cast<u32>(world_count_offset*0x16);

    app31_write_global(m,b,0x258A6u,2u,handle);
    const s32 primary_count=static_cast<s16>(load_le16(dg(m,0x258A6u)));
    app31_write_global(m,b,0x22238u,static_cast<u32>(primary_count*0x16),handle);
    const s32 primary_count_offset=static_cast<s16>(load_le16(dg(m,0x258A6u)));
    const u32 h4=h3+2u+static_cast<u32>(primary_count_offset*0x16);

    const s32 secondary_count=static_cast<s16>(load_le16(dg(m,0x258A4u)));
    const u32 secondary_bytes=static_cast<u32>(secondary_count*0x16);
    app31_write_global(m,b,0x17618u,secondary_bytes,handle);
    const u32 h5=h4+secondary_bytes;

    const u32 cf44=load_le32(dg(m,0xCF44u));
    app31_write_global(m,b,cf44,0x514u,handle);
    const u32 h6=h5+0x514u;

    app31_write_global(m,b,0x25619u,2u,handle);
    app31_write_global(m,b,0x2561Bu,2u,handle);
    if(b.app04)function_01558(m,*b.app04);
    const s32 map_width=static_cast<s16>(load_le16(dg(m,0x25619u)));
    const s32 map_height=static_cast<s16>(load_le16(dg(m,0x2561Bu)));
    app31_write_global(m,b,0x1760u,static_cast<u32>(map_width*map_height),handle);

    const u32 text_bytes=function_01c28(m);
    app31_write_local32(m,b,text_bytes,handle);
    app31_write_global(m,b,0x5760u,text_bytes,handle);

    if(b.call_3131c)(void)b.call_3131c(1u,0u,handle);
    app31_write_local32(m,b,h0,handle);
    app31_write_local32(m,b,h1,handle);
    app31_write_local32(m,b,h2,handle);
    app31_write_local32(m,b,h3,handle);
    app31_write_local32(m,b,h4,handle);
    app31_write_local32(m,b,h5,handle);
    app31_write_local32(m,b,h6,handle);
    if(b.call_312dc)b.call_312dc(handle);
}
static_assert(0x17838u-0x171FCu==1596u);

void function_17838(Machine& m,Application31Boundary& b) noexcept {
    for(u32 slot=0u,actor=0u;slot<4u;++slot,actor+=0x248u) {
        const u16 state=load_le16(dg(m,0x218F0u+actor));
        if((state&0x8000u)!=0u||(state&0x0100u)==0u)continue;
        *dg(m,0x218F0u+actor)=static_cast<u8>(*dg(m,0x218F0u+actor)&0xBFu);
        const bool r37=(b.call_1c1dc?b.call_1c1dc(0x37u):0u)!=0u;
        const u32 level=load_le32(dg(m,0xC6C8u));
        bool excluded=false;
        if(r37) {
            switch(level) {
                case 0x15u: case 0x14u: case 0x0Au: case 0x0Bu: case 0x06u:
                case 0x07u: case 0x12u: case 0x08u: case 0x0Fu: excluded=true; break;
                default: break;
            }
        } else {
            switch(level) {
                case 0x15u: case 0x14u: case 0x0Au: case 0x0Bu: case 0x0Cu:
                case 0x18u: case 0x05u: case 0x12u: case 0x08u: case 0x0Fu:
                    excluded=true; break;
                default: break;
            }
        }
        if(excluded)continue;
        const u32 random=b.call_30ca2?b.call_30ca2():0u;
        const u32 scaled=(random*4u)>>15u;
        store_le32(dg(m,0xBF2Cu),scaled+0x32u);
        if(b.call_1c77c)b.call_1c77c(slot,scaled+0x15Fu);
        *dg(m,0x218F0u+actor)=static_cast<u8>(*dg(m,0x218F0u+actor)|0x40u);
    }
}
static_assert(0x179A2u-0x17838u==362u);

static constexpr u32 kFunction17a68Dispatch[49]={
    0x17C58u,0x18191u,0x17C72u,0x18191u,0x17C8Cu,0x17CB7u,0x17CF5u,
    0x17D32u,0x17F6Au,0x18191u,0x17D4Cu,0x17D89u,0x17DC7u,0x17E05u,
    0x17E42u,0x17E80u,0x18191u,0x17EBEu,0x17ED8u,0x17EF2u,0x17F0Cu,
    0x17F36u,0x18191u,0x17F50u,0x17F6Au,0x18191u,0x18191u,0x17F81u,
    0x18191u,0x17FACu,0x17FEAu,0x18191u,0x18015u,0x1802Cu,0x18069u,
    0x180A6u,0x180E3u,0x180FAu,0x18191u,0x18191u,0x18191u,0x18191u,
    0x18191u,0x18191u,0x18121u,0x18191u,0x18191u,0x18191u,0x1816Eu
};
static_assert(sizeof(kFunction17a68Dispatch)/sizeof(kFunction17a68Dispatch[0])==49u);

static u32 app31_resource(Application31Boundary& b,u32 id) noexcept {
    return b.call_1c1dc?b.call_1c1dc(id):0u;
}

void function_17a68(Machine& m,Application31Boundary& b) noexcept {
    function_17838(m,b);
    for(s32 index=0;;++index) {
        const s32 count=static_cast<s16>(load_le16(dg(m,0x258A2u)));
        if(index>=count)break;
        const u32 rec=0x14D80u+static_cast<u32>(index)*0x16u;
        u16 type=load_le16(dg(m,rec+0x0Eu));
        *dg(m,rec+0x0Bu)=static_cast<u8>(*dg(m,rec+0x0Bu)&0xFDu);

        bool mark=false;
        if((b.call_2acdc?b.call_2acdc(type):0u)!=0u&&
           static_cast<s16>(load_le16(dg(m,rec+0x12u)))<=0)mark=true;

        const u32 high=load_le32(dg(m,0xCF4Cu))>>16u;
        const u32 level=load_le32(dg(m,0xC6C8u));
        switch(type) {
            case 0x00u:
                if((level==0x12u||level==0x1Fu)&&high>4u&&high<0x15u)mark=true;
                break;
            case 0x06u:
                if((level==0x19u||level==0x1Au)&&high>4u&&high<0x15u)mark=true;
                break;
            case 0x08u:
                if(app31_resource(b,0x3Au)==0u)mark=true;
                break;
            case 0x0Fu:
                if(app31_resource(b,0x28u)!=0u&&app31_resource(b,0x29u)!=0u)mark=true;
                break;
            case 0x11u:
                if(level==0x12u&&high>4u&&high<0x15u)mark=true;
                break;
            case 0x15u: case 0x50u:
                if(level==0x18u&&high>4u&&high<0x15u)mark=true;
                break;
            case 0x18u: case 0x1Au:
                if(level==0x0Bu&&high>4u&&high<0x15u)mark=true;
                break;
            default: break;
        }

        if(type>=0x22u&&type<=0x52u&&type!=0x50u) {
            switch(type) {
                case 0x22u: if(app31_resource(b,0x1Du)==1u)mark=true; break;
                case 0x24u: if(app31_resource(b,0x1Eu)==1u)mark=true; break;
                case 0x26u:
                    if(app31_resource(b,0x08u)==1u||app31_resource(b,0x13u)==1u)mark=true;
                    break;
                case 0x27u:
                    if(app31_resource(b,0x0Eu)==1u||app31_resource(b,0x0Du)==1u||
                       app31_resource(b,0x18u)==1u)mark=true;
                    break;
                case 0x28u:
                    if(app31_resource(b,0x0Eu)==0u||app31_resource(b,0x0Du)==1u||
                       app31_resource(b,0x18u)==1u)mark=true;
                    break;
                case 0x29u: if(app31_resource(b,0x2Cu)==1u)mark=true; break;
                case 0x2Au: if(app31_resource(b,0x2Cu)==0u)mark=true; break;
                case 0x2Cu:
                    if(app31_resource(b,0x06u)==1u||app31_resource(b,0x11u)==1u||
                       app31_resource(b,0x30u)==0u)mark=true;
                    break;
                case 0x2Du:
                    if(app31_resource(b,0x04u)==1u||app31_resource(b,0x0Fu)==1u||
                       app31_resource(b,0x19u)==1u)mark=true;
                    break;
                case 0x2Eu:
                    if(app31_resource(b,0x07u)==1u||app31_resource(b,0x12u)==1u||
                       app31_resource(b,0x1Bu)==1u)mark=true;
                    break;
                case 0x2Fu:
                    if(app31_resource(b,0x07u)==1u||app31_resource(b,0x12u)==1u||
                       app31_resource(b,0x1Bu)==0u)mark=true;
                    break;
                case 0x30u:
                    if(app31_resource(b,0x09u)==1u||app31_resource(b,0x45u)==1u||
                       app31_resource(b,0x14u)==1u)mark=true;
                    break;
                case 0x31u:
                    if(app31_resource(b,0x0Au)==1u||app31_resource(b,0x15u)==1u||
                       app31_resource(b,0x23u)==1u)mark=true;
                    break;
                case 0x33u: if(app31_resource(b,0x2Du)==1u)mark=true; break;
                case 0x34u: if(app31_resource(b,0x2Fu)==1u)mark=true; break;
                case 0x35u: if(app31_resource(b,0x2Eu)==1u)mark=true; break;
                case 0x36u:
                    if(app31_resource(b,0x2Fu)==1u||app31_resource(b,0x39u)==0u)mark=true;
                    break;
                case 0x37u: if(app31_resource(b,0x27u)==1u)mark=true; break;
                case 0x39u: if(app31_resource(b,0x2Cu)==1u)mark=true; break;
                case 0x3Au: if(app31_resource(b,0x2Cu)==0u)mark=true; break;
                case 0x3Du:
                    if(app31_resource(b,0x0Cu)==1u||app31_resource(b,0x17u)==1u)mark=true;
                    break;
                case 0x3Fu:
                    if(app31_resource(b,0x05u)==1u||app31_resource(b,0x10u)==1u||
                       app31_resource(b,0x1Cu)==1u)mark=true;
                    break;
                case 0x40u:
                    if(app31_resource(b,0x0Bu)==1u||app31_resource(b,0x16u)==1u)mark=true;
                    break;
                case 0x42u: if(app31_resource(b,0x1Eu)==0u)mark=true; break;
                case 0x43u:
                    if(app31_resource(b,0x04u)==1u||app31_resource(b,0x0Fu)==1u||
                       app31_resource(b,0x19u)==0u)mark=true;
                    break;
                case 0x44u:
                    if(app31_resource(b,0x0Au)==1u||app31_resource(b,0x15u)==1u||
                       app31_resource(b,0x23u)==0u)mark=true;
                    break;
                case 0x45u:
                    if(app31_resource(b,0x05u)==1u||app31_resource(b,0x10u)==1u||
                       app31_resource(b,0x1Cu)==0u)mark=true;
                    break;
                case 0x46u: if(app31_resource(b,0x1Du)==0u)mark=true; break;
                case 0x47u:
                    if(app31_resource(b,0x2Fu)==1u||app31_resource(b,0x39u)==0u)mark=true;
                    break;
                case 0x4Eu:
                    if(app31_resource(b,0x25u)!=0u) {
                        type=1u;
                        store_le16(dg(m,rec+0x0Eu),type);
                        store_le32(dg(m,rec+0x08u),0x10000000u);
                        const u32 desc=load_le32(dg(m,0x22230u))+0x45u;
                        const u32 rank=*dg(m,desc+0x1Du);
                        const u32 roll=b.call_1c328?b.call_1c328(rank,8u,0u,0u):0u;
                        store_le16(dg(m,rec+0x12u),static_cast<u16>(roll));
                    }
                    break;
                case 0x52u:
                    if(app31_resource(b,0x14u)==1u||app31_resource(b,0x09u)==1u)mark=true;
                    break;
                default: break;
            }
        }
        if(mark)*dg(m,rec+0x0Bu)=static_cast<u8>(*dg(m,rec+0x0Bu)|2u);
    }
}
static_assert(0x181B4u-0x17A68u==1868u);

} // namespace code2
