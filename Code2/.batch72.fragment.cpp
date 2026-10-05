// -----------------------------------------------------------------------------
// Batch 72: 30D30..314AA runtime memory/DPMI wrappers, allocator recovery,
// buffered character output, DOS/string wrappers, and handle/drive helpers.
// All addresses are CODE.2 object-relative.
// -----------------------------------------------------------------------------
struct Application72Boundary {
    void (*call_379d0)(u32,u32,u32)=nullptr;
    void (*call_37a73)(u8*)=nullptr;
    u32 (*call_33a7c)(u32,u8*,u8*,u8*)=nullptr;
    u32 (*call_37ac2)(u32)=nullptr;
    u32 (*call_37a9e)(u32,u32*)=nullptr;
    u8 (*linear_load8)(u32)=nullptr;
    u32 (*linear_load32)(u32)=nullptr;
    void (*linear_store8)(u32,u8)=nullptr;
    void (*linear_store16)(u32,u16)=nullptr;
    void (*linear_store32)(u32,u32)=nullptr;
    void (*dpmi_0500)(u16,u32*)=nullptr;
    void (*call_290f4)(u32)=nullptr;
    u32 (*call_2f5f4)(u32,u32)=nullptr;
    void (*call_2f580)(u32)=nullptr;
    void (*call_33b44)()=nullptr;
    void (*call_30c38)()=nullptr;
    void (*call_2f540)(u32)=nullptr;
    u32 (*call_37afc)()=nullptr;
    u32 (*call_37b34)(u32)=nullptr;
    u32 (*call_2f0b4)(u32)=nullptr;
    void (*call_2f1e4)()=nullptr;
    void (*call_2e9cc)()=nullptr;
    u32 (*call_37c37)(u32)=nullptr;
    u32 (*call_37cc2)(u32)=nullptr;
    u32 (*call_37ce0)(u32)=nullptr;
    u32 (*call_37d60)(u32)=nullptr;
    void (*dos_ah09)(u32)=nullptr;
    u32 (*call_37e39)(u32,u32)=nullptr;
    s32 (*call_339a2)(u32,u32,u32)=nullptr;
    s32 (*call_339cf)(u32)=nullptr;
    u32 (*call_37e77)(u32,u32)=nullptr;
    u32 (*call_380e1)(u32,u32)=nullptr;
    u32 (*call_380f8)(u32)=nullptr;
    u32 (*call_38106)(u32,u32,u32)=nullptr;
    u32 (*call_3821f)(u32,u32,u32)=nullptr;
    u32 (*call_38407)(u32,u32,u32)=nullptr;
    u32 (*call_38452)(u32)=nullptr;
    u32 (*call_3848c)(u32,u32*)=nullptr;
    u32 (*call_38474)(u32*)=nullptr;
    u32 (*call_384aa)(u32,u32)=nullptr;
    u32 (*call_3855c)(s32)=nullptr;
    s32 (*call_33a5a)(u32)=nullptr;
    u32 (*call_33a38)(u32)=nullptr;
    u32 (*call_3856e)()=nullptr;
};

struct App72Regs { u8 v[0x1Cu]{}; };
static u8 app72_r8(const App72Regs& r,u32 off) noexcept { return r.v[off]; }
static u16 app72_r16(const App72Regs& r,u32 off) noexcept { return load_le16(r.v+off); }
static u32 app72_r32(const App72Regs& r,u32 off) noexcept { return load_le32(r.v+off); }
static void app72_w16(App72Regs& r,u32 off,u16 value) noexcept { store_le16(r.v+off,value); }
static void app72_w32(App72Regs& r,u32 off,u32 value) noexcept { store_le32(r.v+off,value); }
static u8 app72_load8(Machine& m,Application72Boundary& b,u32 addr) noexcept {
    return b.linear_load8?b.linear_load8(addr):*dg(m,addr);
}
static u32 app72_load32(Machine& m,Application72Boundary& b,u32 addr) noexcept {
    return b.linear_load32?b.linear_load32(addr):load_le32(dg(m,addr));
}
static void app72_store8(Machine& m,Application72Boundary& b,u32 addr,u8 value) noexcept {
    if(b.linear_store8)b.linear_store8(addr,value);else *dg(m,addr)=value;
}
static void app72_store16(Machine& m,Application72Boundary& b,u32 addr,u16 value) noexcept {
    if(b.linear_store16)b.linear_store16(addr,value);else store_le16(dg(m,addr),value);
}
static void app72_store32(Machine& m,Application72Boundary& b,u32 addr,u32 value) noexcept {
    if(b.linear_store32)b.linear_store32(addr,value);else store_le32(dg(m,addr),value);
}
static s32 app72_div16_trunc(u32 wrapped) noexcept { return static_cast<s32>(wrapped)/16; }

// 30D30..30D51: Watcom memset entry. The low fill byte is replicated through
// EDX exactly by the three byte/shift operations before the register-call helper.
[[nodiscard]] u32 function_30d30(Application72Boundary& b,u32 dst,u32 fill,u32 count) noexcept {
    const u32 byte=fill&0xFFu;
    const u32 packed=byte|(byte<<8)|(byte<<16)|(byte<<24);
    if(b.call_379d0)b.call_379d0(dst,packed,count);
    return dst;
}
static_assert(0x30D52u-0x30D30u==34u);

// 30D52..30D7F: common software-interrupt adapter. 37A73 initializes the
// private 12-byte call context; 33A7C receives interrupt number, in/out frames,
// and that context in the exact cdecl order.
[[nodiscard]] u32 function_30d52(Application72Boundary& b,u32 intno,u8* in,u8* out) noexcept {
    u8 context[12]{};
    if(b.call_37a73)b.call_37a73(context);
    return b.call_33a7c?b.call_33a7c(intno,in,out,context):0u;
}
static_assert(0x30D80u-0x30D52u==46u);

// 30D80..30DE6: DPMI 0100h wrapper. The paragraph count uses signed truncation
// toward zero after 32-bit wrap of size+10h. Carry returns zero.
[[nodiscard]] u32 function_30d80(Application72Boundary& b,s32 size,u32* packed_segment,u16* selector) noexcept {
    App72Regs in{},out{};
    const s32 paragraphs=app72_div16_trunc(static_cast<u32>(size)+0x10u);
    app72_w32(in,0u,0x100u); app72_w32(in,4u,static_cast<u32>(paragraphs));
    (void)function_30d52(b,0x31u,in.v,out.v);
    if((app72_r8(out,0x18u)&1u)!=0u)return 0u;
    if(selector)*selector=app72_r16(out,0x0Cu);
    if(packed_segment)*packed_segment=app72_r32(out,0u)<<16;
    return app72_r32(out,0u)<<4;
}
static_assert(0x30DE7u-0x30D80u==103u);
// 30DE7 is NOP alignment.

// 30DE8..30E1F: DPMI 0101h free wrapper; encoded SETE/DEC maps clear carry to
// zero and set carry to FFFFFFFFh.
[[nodiscard]] u32 function_30de8(Application72Boundary& b,u32 selector) noexcept {
    App72Regs frame{}; app72_w32(frame,0u,0x101u); app72_w16(frame,0x0Cu,static_cast<u16>(selector));
    (void)function_30d52(b,0x31u,frame.v,frame.v);
    return (app72_r8(frame,0x18u)&1u)==0u?0u:0xFFFFFFFFu;
}
static_assert(0x30E20u-0x30DE8u==56u);

[[nodiscard]] u32 function_30e20(Application72Boundary& b,u32 selector) noexcept {
    return b.call_37ac2?b.call_37ac2(static_cast<u16>(selector)):0u;
}
static_assert(0x30E2Fu-0x30E20u==15u);
// 30E2F is NOP alignment.

// 30E30..30E62: DPMI function 0006h wrapper, returning the encoded high/low
// pair as (dword@+8 << 16) + zero-extended word@+0C.
[[nodiscard]] u32 function_30e30(Application72Boundary& b,u32 selector) noexcept {
    App72Regs frame{}; app72_w16(frame,0u,6u); app72_w16(frame,4u,static_cast<u16>(selector));
    (void)function_30d52(b,0x31u,frame.v,frame.v);
    return (app72_r32(frame,8u)<<16)+static_cast<u32>(app72_r16(frame,0x0Cu));
}
static_assert(0x30E63u-0x30E30u==51u);
// 30E63 is NOP alignment.

// 30E64..30EDF: paragraph allocation plus selector-base query. Negative wrapped
// paragraph counts deliberately pass the signed <= FFFFh check, matching JLE.
[[nodiscard]] u32 function_30e64(Machine& m,Application72Boundary& b,s32 size) noexcept {
    const s32 paragraphs=app72_div16_trunc(static_cast<u32>(size)+0x0Fu);
    if(paragraphs>0xFFFF)return 0u;
    u32 selector_word=0u;
    if(b.call_37a9e && b.call_37a9e(static_cast<u32>(paragraphs),&selector_word)!=0u)return 0u;
    App72Regs frame{}; app72_w16(frame,0u,6u); app72_w16(frame,4u,static_cast<u16>(selector_word));
    (void)function_30d52(b,0x31u,frame.v,frame.v);
    const u32 p=(app72_r32(frame,8u)<<16)+static_cast<u32>(app72_r16(frame,0x0Cu));
    app72_store32(m,b,p+2u,0u); app72_store16(m,b,p,static_cast<u16>(selector_word));
    return p;
}
static_assert(0x30EE0u-0x30E64u==124u);

[[nodiscard]] u32 function_30ee0(Application72Boundary& b,u32 packed) noexcept {
    App72Regs frame{}; app72_w16(frame,0u,0x0502u); app72_w16(frame,0x10u,static_cast<u16>(packed>>16)); app72_w16(frame,0x14u,static_cast<u16>(packed));
    (void)function_30d52(b,0x31u,frame.v,frame.v);
    return (app72_r8(frame,0x18u)&1u)!=0u?1u:0u;
}
static_assert(0x30F1Cu-0x30EE0u==60u);

[[nodiscard]] u32 function_30f1c(Machine& m,Application72Boundary& b,u32 packed) noexcept {
    App72Regs frame{}; app72_w16(frame,0u,0x0501u); app72_w16(frame,4u,static_cast<u16>(packed>>16)); app72_w16(frame,8u,static_cast<u16>(packed));
    (void)function_30d52(b,0x31u,frame.v,frame.v);
    if((app72_r8(frame,0x18u)&1u)!=0u)return 0u;
    const u32 p=(app72_r32(frame,4u)<<16)+static_cast<u32>(app72_r16(frame,8u));
    app72_store16(m,b,p,0u);
    const u32 q=(app72_r32(frame,0x10u)<<16)+static_cast<u32>(app72_r16(frame,0x14u));
    app72_store32(m,b,p+2u,q);
    return p;
}
static_assert(0x30F89u-0x30F1Cu==109u);
// 30F89..30F8B is LEA alignment.

[[nodiscard]] u32 function_30f8c(Application72Boundary& b) noexcept {
    App72Regs out{},in{}; app72_w32(in,0u,0x100u); app72_w32(in,4u,0xFFFFu);
    (void)function_30d52(b,0x31u,in.v,out.v);
    return (app72_r8(out,0x18u)&1u)==0u?(app72_r32(out,4u)<<4):0u;
}
static_assert(0x30FC9u-0x30F8Cu==61u);
// 30FC9..30FCB is LEA alignment.

// 30FCC..31012: DPMI 0500h free-memory query. The binary points ES:EDI at a
// 48-byte stack result block and returns its first dword; the host boundary
// represents that stack-address-dependent interrupt effect directly.
[[nodiscard]] u32 function_30fcc(Machine& m,Application72Boundary& b) noexcept {
    u32 info[12]{};
    if(b.dpmi_0500)b.dpmi_0500(m.cpu.ds,info);
    return info[0];
}
static_assert(0x31013u-0x30FCCu==71u);
// 31013 is NOP alignment.

static void function_311e8(Machine&,Application72Boundary&,u32) noexcept;

// 31014..31042: fatal allocator-recovery cleanup path.
void function_31014(Machine& m,Application72Boundary& b) noexcept {
    if(b.call_290f4)b.call_290f4(0xFFu);
    function_311e8(m,b,0x150Cu);
    if(b.call_33b44)b.call_33b44();
    if(b.call_30c38)b.call_30c38();
    if(b.call_2f540)b.call_2f540(0x0Au);
}
static_assert(0x31043u-0x31014u==47u);
// 31043 is NOP alignment.

// 31044..31048 is an address-distinct tail thunk to 37AFC.
[[nodiscard]] u32 function_31044(Application72Boundary& b) noexcept { return b.call_37afc?b.call_37afc():0u; }
static_assert(0x31049u-0x31044u==5u);
// 31049..3104B is LEA alignment.

// 3104C..310AF: allocation wrapper with three staged recovery actions. The
// one-byte stage counter intentionally stops advancing after stage 2.
[[nodiscard]] u32 function_3104c(Machine& m,Application72Boundary& b,u32 size) noexcept {
    const u32 request=size+0x0Au; u8 stage=0u;
    for(;;){
        const u32 header=b.call_37b34?b.call_37b34(request):0u;
        if(header!=0u){app72_store32(m,b,header+6u,0x12345678u);return header+0x0Au;}
        const u32 probe=b.call_2f0b4?b.call_2f0b4(1u):0u;
        if(probe==0u){
            if(stage==0u){if(b.call_2f1e4)b.call_2f1e4();++stage;}
            else if(stage==1u){if(b.call_2e9cc)b.call_2e9cc();++stage;}
            else if(stage==2u)function_31014(m,b);
        }
    }
}
static_assert(0x310B0u-0x3104Cu==100u);

// 310B0..31106: checked free wrapper. The p==0Ah sentinel is tested after the
// encoded subtraction; freed and corrupt header magics take distinct messages.
void function_310b0(Machine& m,Application72Boundary& b,u32 p) noexcept {
    const u32 header=p-0x0Au;
    if(header==0u){if(b.call_2f5f4)(void)b.call_2f5f4(0x153Cu,0u);return;}
    const u32 magic=app72_load32(m,b,header+6u);
    if(magic==0xABCDEF98u){if(b.call_2f580)b.call_2f580(0x156Cu);return;}
    if(magic!=0x12345678u){if(b.call_2f580)b.call_2f580(0x158Cu);return;}
    app72_store32(m,b,header+6u,0xABCDEF98u);
    if(b.call_37c37)(void)b.call_37c37(header);
}
static_assert(0x31107u-0x310B0u==87u);

// 31107..311D4: buffered single-byte output. Newline may emit an inserted CR;
// flag-byte writes are kept byte-sized even though the later flush test reads
// the full dword at +0C.
[[nodiscard]] u32 function_31107(Machine& m,Application72Boundary& b,u32 ch,u32 stream) noexcept {
    u8 flags=app72_load8(m,b,stream+0x0Cu);
    if((flags&2u)==0u){if(b.call_37cc2)(void)b.call_37cc2(4u);app72_store8(m,b,stream+0x0Cu,static_cast<u8>(flags|0x20u));return 0xFFFFFFFFu;}
    if(app72_load32(m,b,stream+8u)==0u && b.call_37ce0)(void)b.call_37ce0(stream);
    u32 trigger=0x400u;
    if(ch==0x0Au){
        trigger=0x600u; flags=app72_load8(m,b,stream+0x0Cu);
        if((flags&0x40u)==0u){
            app72_store8(m,b,stream+0x0Du,static_cast<u8>(app72_load8(m,b,stream+0x0Du)|0x10u));
            u32 ptr=app72_load32(m,b,stream); app72_store8(m,b,ptr,0x0Du); app72_store32(m,b,stream,ptr+1u);
            const u32 count=app72_load32(m,b,stream+4u)+1u; app72_store32(m,b,stream+4u,count);
            if(count==app72_load32(m,b,stream+0x14u) && b.call_37d60 && b.call_37d60(stream)!=0u)return 0xFFFFFFFFu;
        }
    }
    app72_store8(m,b,stream+0x0Du,static_cast<u8>(app72_load8(m,b,stream+0x0Du)|0x10u));
    u32 ptr=app72_load32(m,b,stream); app72_store8(m,b,ptr,static_cast<u8>(ch)); app72_store32(m,b,stream,ptr+1u);
    const u32 count=app72_load32(m,b,stream+4u)+1u; app72_store32(m,b,stream+4u,count);
    const u32 state=app72_load32(m,b,stream+0x0Cu);
    if(((state&trigger)!=0u || count==app72_load32(m,b,stream+0x14u)) && b.call_37d60 && b.call_37d60(stream)!=0u)return 0xFFFFFFFFu;
    return static_cast<u8>(ch);
}
static_assert(0x311D5u-0x31107u==206u);
// 311D5..311DF is eleven zero bytes with no direct control-flow reference.

[[nodiscard]] u32 function_311e0(Machine& m) noexcept { return load_le32(dg(m,0x046Cu)); }
static_assert(0x311E8u-0x311E0u==8u);

// 311E8..31217: DOS AH=09 string wrapper. Existing '$' bytes are permanently
// converted to '*'; only the appended terminator is restored to zero afterward.
static void function_311e8(Machine& m,Application72Boundary& b,u32 text) noexcept {
    u32 p=text;
    while(*dg(m,p)!=0u){if(*dg(m,p)==0x24u)*dg(m,p)=0x2Au;++p;}
    *dg(m,p)=0x24u;
    if(b.dos_ah09)b.dos_ah09(text);
    *dg(m,p)=0u;
}
static_assert(0x31218u-0x311E8u==48u);

[[nodiscard]] u32 function_31218(Application72Boundary& b,u32 a,u32 word) noexcept {
    const u32 r=b.call_37e39?b.call_37e39(a,static_cast<u16>(word)):1u;
    return r==0u?1u:0u;
}
static_assert(0x31233u-0x31218u==27u);
// 31233 is NOP alignment.

[[nodiscard]] u32 function_31234(Application72Boundary& b,u32 a,u32 word) noexcept {
    const s32 r=b.call_339a2?b.call_339a2(a,static_cast<u16>(word),0x2B6D0u):-1;
    return r==0?0x2B6EEu:0u;
}
static_assert(0x31258u-0x31234u==36u);
[[nodiscard]] u32 function_31258(Application72Boundary& b) noexcept {
    const s32 r=b.call_339cf?b.call_339cf(0x2B6D0u):-1;
    return r==0?0x2B6EEu:0u;
}
static_assert(0x31272u-0x31258u==26u);
// 31272..31273 is MOV EAX,EAX alignment.

// 31274 writes AL only, preserving the incoming upper 24 EAX bits.
[[nodiscard]] u32 function_31274(Machine& m) noexcept {
    m.cpu.eax=(m.cpu.eax&0xFFFFFF00u)|*dg(m,0x2B6E5u); return m.cpu.eax;
}
static_assert(0x3127Au-0x31274u==6u);
// 3127A..3127B is MOV EAX,EAX alignment.

[[nodiscard]] u32 function_3127c(Machine& m,Application72Boundary& b,u32 a) noexcept {
    const s32 r=b.call_339a2?b.call_339a2(a,0u,0x2B6D0u):-1;
    return r==0?load_le32(dg(m,0x2B6EAu)):0xFFFFFFFFu;
}
static_assert(0x3129Fu-0x3127Cu==35u);
// 3129F is NOP alignment.

[[nodiscard]] u32 function_312a0(Application72Boundary& b,u32 a) noexcept { return b.call_37e77?b.call_37e77(a,0x200u):0u; }
static_assert(0x312B2u-0x312A0u==18u);
// 312B2..312B3 is MOV EAX,EAX alignment.
[[nodiscard]] u32 function_312b4(Application72Boundary& b,u32 a) noexcept { return b.call_37e77?b.call_37e77(a,0x202u):0u; }
static_assert(0x312C6u-0x312B4u==18u);
// 312C6..312C7 is MOV EAX,EAX alignment.
[[nodiscard]] u32 function_312c8(Application72Boundary& b,u32 a) noexcept { return b.call_380e1?b.call_380e1(a,0x180u):0u; }
static_assert(0x312DAu-0x312C8u==18u);
// 312DA..312DB is MOV EAX,EAX alignment.
[[nodiscard]] u32 function_312dc(Application72Boundary& b,u32 a) noexcept { return b.call_380f8?b.call_380f8(a):0u; }
static_assert(0x312E9u-0x312DCu==13u);
// 312E9..312EB is LEA alignment.

// These three wrappers preserve the binary's stack-induced argument permutation:
// the downstream call receives (third, first, second).
[[nodiscard]] u32 function_312ec(Application72Boundary& b,u32 a,u32 c,u32 d) noexcept { return b.call_38106?b.call_38106(d,a,c):0u; }
static_assert(0x31301u-0x312ECu==21u);
// 31301..31303 is LEA alignment.
[[nodiscard]] u32 function_31304(Application72Boundary& b,u32 a,u32 c,u32 d) noexcept { return b.call_3821f?b.call_3821f(d,a,c):0u; }
static_assert(0x31319u-0x31304u==21u);
// 31319..3131B is LEA alignment.
[[nodiscard]] u32 function_3131c(Application72Boundary& b,u32 a,u32 c,u32 d) noexcept { return b.call_38407?b.call_38407(d,a,c):0u; }
static_assert(0x31331u-0x3131Cu==21u);
// 31331..31333 is LEA alignment.

// 31334..3137C: seek by count*4+4, read one dword, then seek to that dword.
[[nodiscard]] u32 function_31334(Application72Boundary& b,u32 count,u32 handle) noexcept {
    const u32 delta=count*4u+4u;
    if(b.call_38407)(void)b.call_38407(handle,delta,0u);
    u32 value=0u;
    if(b.call_38106)(void)b.call_38106(handle,static_cast<u32>(reinterpret_cast<std::uintptr_t>(&value)),4u);
    return b.call_38407?b.call_38407(handle,value,0u):0u;
}
static_assert(0x3137Du-0x31334u==73u);
// 3137D..3137F is LEA alignment.

[[nodiscard]] u32 function_31380(Application72Boundary& b,u32 a) noexcept { return b.call_38452?b.call_38452(a):0u; }
static_assert(0x3138Du-0x31380u==13u);
// 3138D..3138F is LEA alignment.

[[nodiscard]] u32 function_31390(Application72Boundary& b,u32 a) noexcept {
    u32 out=0u; if(b.call_3848c)(void)b.call_3848c(a,&out); return out;
}
static_assert(0x313A9u-0x31390u==25u);
// 313A9..313AB is LEA alignment.

[[nodiscard]] u32 function_313ac(Application72Boundary& b) noexcept {
    u32 out=0u; if(b.call_38474)(void)b.call_38474(&out); return out;
}
static_assert(0x313C1u-0x313ACu==21u);
// 313C1..313C3 is LEA alignment.

[[nodiscard]] u32 function_313c4(Application72Boundary& b) noexcept {
    u32 a=0u,bv=0u; if(b.call_38474)(void)b.call_38474(&a); if(b.call_3848c)(void)b.call_3848c(a,&bv); return bv;
}
static_assert(0x313EBu-0x313C4u==39u);
// 313EB is NOP alignment.

[[nodiscard]] u32 function_313ec(Application72Boundary& b) noexcept { return b.call_384aa?b.call_384aa(0x2B6FBu,0x50u):0u; }
static_assert(0x313FCu-0x313ECu==16u);

// 313FC..31482: optional drive-prefix handling around the pathname opener. A
// failed open restores the saved drive; a successful prefixed open leaves the
// selected drive active, exactly as encoded.
[[nodiscard]] s32 function_313fc(Machine& m,Application72Boundary& b,u32 path) noexcept {
    u32 saved_drive=0u; if(b.call_38474)(void)b.call_38474(&saved_drive);
    u32 p=path;
    if(static_cast<s8>(*dg(m,p+1u))==0x3A){
        const s32 ch=static_cast<s8>(*dg(m,p));
        const u32 upper=b.call_3855c?b.call_3855c(ch):static_cast<u32>(ch);
        const u32 drive=upper-0x41u;
        u32 temp=0u; if(b.call_3848c)(void)b.call_3848c(drive,&temp);
        u32 current=0u; if(b.call_38474)(void)b.call_38474(&current);
        if(drive!=current)return -1;
        p+=2u;
    }
    const s32 h=b.call_33a5a?b.call_33a5a(p):-1;
    if(h==-1){u32 temp=0u;if(b.call_3848c)(void)b.call_3848c(saved_drive,&temp);}
    return h;
}
static_assert(0x31483u-0x313FCu==135u);
// 31483 is NOP alignment.

[[nodiscard]] u32 function_31484(Application72Boundary& b,u32 a) noexcept { return b.call_33a38?b.call_33a38(a):0u; }
static_assert(0x31491u-0x31484u==13u);

[[nodiscard]] u32 function_31491(Application72Boundary& b,u32 stream) noexcept {
    if(stream==0u){if(b.call_3856e)(void)b.call_3856e();return 0u;}
    return b.call_37d60?b.call_37d60(stream):0u;
}
static_assert(0x314ABu-0x31491u==26u);
// 314AB..314AF is five zero padding bytes; 314B0 starts the next application routine.

static_assert(
    (0x30D52u-0x30D30u)+(0x30D80u-0x30D52u)+(0x30DE7u-0x30D80u)+
    (0x30E20u-0x30DE8u)+(0x30E2Fu-0x30E20u)+(0x30E63u-0x30E30u)+
    (0x30EE0u-0x30E64u)+(0x30F1Cu-0x30EE0u)+(0x30F89u-0x30F1Cu)+
    (0x30FC9u-0x30F8Cu)+(0x31013u-0x30FCCu)+(0x31043u-0x31014u)+
    (0x31049u-0x31044u)+(0x310B0u-0x3104Cu)+(0x31107u-0x310B0u)+
    (0x311D5u-0x31107u)+(0x311E8u-0x311E0u)+(0x31218u-0x311E8u)+
    (0x31233u-0x31218u)+(0x31258u-0x31234u)+(0x31272u-0x31258u)+
    (0x3127Au-0x31274u)+(0x3129Fu-0x3127Cu)+(0x312B2u-0x312A0u)+
    (0x312C6u-0x312B4u)+(0x312DAu-0x312C8u)+(0x312E9u-0x312DCu)+
    (0x31301u-0x312ECu)+(0x31319u-0x31304u)+(0x31331u-0x3131Cu)+
    (0x3137Du-0x31334u)+(0x3138Du-0x31380u)+(0x313A9u-0x31390u)+
    (0x313C1u-0x313ACu)+(0x313EBu-0x313C4u)+(0x313FCu-0x313ECu)+
    (0x31483u-0x313FCu)+(0x31491u-0x31484u)+(0x314ABu-0x31491u)==1852u);
