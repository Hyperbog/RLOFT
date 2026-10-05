// -----------------------------------------------------------------------------
// Batch 71: 305A8..30D25 text layout, IRQ/tick/keyboard state, vector lifecycle,
// RNG, and overlap-safe memory move. All addresses are CODE.2 object-relative.
// -----------------------------------------------------------------------------
struct Application71Boundary {
    u32 (*call_2f29c)(u32,u32)=nullptr;
    void (*call_303c8)(u32,u32)=nullptr;
    void (*call_2939c)(u32,u32,u32,u32)=nullptr;
    void (*call_293ec)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_3566c)()=nullptr;
    void (*call_19c80)(u32)=nullptr;
    u8 (*in8)(u16)=nullptr;
    void (*out8)(u16,u8)=nullptr;
    void (*far_chain)(u32,u16)=nullptr;
    void (*call_2f868)()=nullptr;
    void (*call_290f4)(u32)=nullptr;
    void (*call_33b44)()=nullptr;
    void (*get_vector)(u8,u32*,u16*)=nullptr;
    void (*set_vector)(u8,u32,u16)=nullptr;
};

static u8 app71_glyph_signed(Machine& m,u32 font,u8 ch) noexcept {
    const u32 off=font+static_cast<u32>(static_cast<s32>(static_cast<s8>(ch)))+4u;
    return *dg(m,off);
}
static u8 app71_glyph_unsigned(Machine& m,u32 font,u8 ch) noexcept {
    return *dg(m,font+static_cast<u32>(ch)+4u);
}

// 305A8..3060B: right-aligned string draw; glyph indexing is signed-byte here.
void function_305a8(Machine& m,Application71Boundary& b,u32 y,u32 x,u32 text,u32 font_id) noexcept {
    if(text==0u)return;
    const u32 font=b.call_2f29c?b.call_2f29c(font_id,0u):0u;
    store_le32(dg(m,0x2B67Cu),font);
    u32 width=0u;
    for(u32 p=text;*dg(m,p)!=0u;++p)width+=app71_glyph_signed(m,font,*dg(m,p));
    store_le32(dg(m,0x2B680u),x-width);
    store_le32(dg(m,0x2B684u),y);
    if(b.call_303c8)b.call_303c8(text,font_id);
}
// 3060C..3062E: direct-position string draw wrapper.
void function_3060c(Machine& m,Application71Boundary& b,u32 y,u32 x,u32 text,u32 font_id) noexcept {
    store_le32(dg(m,0x2B684u),y); store_le32(dg(m,0x2B680u),x);
    if(b.call_303c8)b.call_303c8(text,font_id);
}
// 30630..306F7: width-bounded draw; this path uses unsigned glyph indices.
void function_30630(Machine& m,Application71Boundary& b,u32 y,u32 x,u32 text,u32 font_id,s32 remaining) noexcept {
    store_le32(dg(m,0x2B684u),y); store_le32(dg(m,0x2B680u),x);
    const u32 font=b.call_2f29c?b.call_2f29c(font_id,0u):0u;
    store_le32(dg(m,0x2B67Cu),font);
    if(text==0u)return;
    u32 p=text;
    remaining=static_cast<s32>(static_cast<u32>(remaining)-app71_glyph_unsigned(m,font,*dg(m,p)));
    while(*dg(m,p)!=0u && remaining>0){
        const u32 ch=*dg(m,p);
        const u32 fy=load_le32(dg(m,0x2B684u)),fx=load_le32(dg(m,0x2B680u));
        const u32 style=load_le32(dg(m,font));
        const u32 mask=load_le32(dg(m,0xF56Cu));
        if(mask!=0u){if(b.call_293ec)b.call_293ec(ch,fy,fx,style,mask);}
        else if(b.call_2939c)b.call_2939c(ch,fy,fx,style);
        store_le32(dg(m,0x2B680u),fx+app71_glyph_unsigned(m,font,*dg(m,p)));
        ++p;
        remaining=static_cast<s32>(static_cast<u32>(remaining)-app71_glyph_unsigned(m,font,*dg(m,p)));
    }
}

[[nodiscard]] u32 function_30700(Machine& m) noexcept { return load_le32(dg(m,0xF58Cu)); }
// 30708..30735: IRQ-8 body; full register/segment save is represented by CpuState restore.
void interrupt_30708(Machine& m,Application71Boundary& b) noexcept {
    const CpuState saved=m.cpu;
    if(b.call_3566c)b.call_3566c();
    const u32 tick=load_le32(dg(m,0xF58Cu)); store_le32(dg(m,0xF58Cu),tick+1u);
    if(b.call_19c80)b.call_19c80(tick);
    if(b.far_chain)b.far_chain(load_le32(dg(m,0xF582u)),load_le16(dg(m,0xF586u)));
    m.cpu=saved;
}
// 30738..30744: encoded CLI/store/STI sequence leaves IF enabled on return.
void function_30738(Machine& m,u32 value) noexcept {
    m.cpu.interrupt_enable=false; store_le32(dg(m,0xF58Cu),value); m.cpu.interrupt_enable=true;
}

// 30748..307C7: remove one scan byte from ordered queue and refresh current/last.
void function_30748(Machine& m,u32 value) noexcept {
    u32 i=0u; u8 count=*dg(m,0xF588u);
    for(;i<count;++i){
        if(static_cast<u32>(*dg(m,0x2B6C4u+i))!=value)continue;
        const u32 n=static_cast<u32>(count)-i-1u;
        for(u32 j=0;j<n;++j)*dg(m,0x2B6C4u+i+j)=*dg(m,0x2B6C4u+i+j+1u);
        count=static_cast<u8>(count-1u); *dg(m,0xF588u)=count; break;
    }
    const u8 active=count?*dg(m,0x2B6C4u+static_cast<u32>(count)-1u):0u;
    *dg(m,0xF572u)=active; *dg(m,0xF571u)=active;
}
static constexpr u32 kApp71DirectionJumpTable[10]={
    0x30857u,0x30843u,0x3086Bu,0x308A7u,0x30876u,
    0x308A7u,0x3088Au,0x308A7u,0x308A7u,0x30895u
};
// 307F0..308B8: move/append scan code and update the encoded direction words.
void function_307f0(Machine& m,u32 value) noexcept {
    u8 count=*dg(m,0xF588u);
    for(u32 i=0;i<count;++i){if(static_cast<u32>(*dg(m,0x2B6C4u+i))==value){function_30748(m,value);count=*dg(m,0xF588u);break;}}
    *dg(m,0x2B6C4u+count)=static_cast<u8>(value); *dg(m,0xF588u)=static_cast<u8>(count+1u);
    const u32 index=value-0x47u;
    if(index<10u){
        switch(index){
            case 0: store_le16(dg(m,0x258FCu),1u); store_le16(dg(m,0x258FEu),0xFFFFu); break;
            case 1: store_le16(dg(m,0x258FCu),3u); store_le16(dg(m,0x258FEu),0xFFFFu); break;
            case 2: store_le16(dg(m,0x258FCu),1u); store_le16(dg(m,0x258FEu),1u); break;
            case 4: store_le16(dg(m,0x258FCu),2u); store_le16(dg(m,0x258FEu),0xFFFFu); break;
            case 6: store_le16(dg(m,0x258FCu),2u); store_le16(dg(m,0x258FEu),1u); break;
            case 9: store_le16(dg(m,0x258FCu),0u); store_le16(dg(m,0x258FEu),1u); break;
            default: break;
        }
    }
    *dg(m,0xF572u)=static_cast<u8>(value); *dg(m,0xF571u)=*dg(m,0xF572u);
}
[[nodiscard]] u32 function_308bc(Machine& m,u32 value) noexcept {
    const u8 count=*dg(m,0xF588u);
    for(u32 i=0;i<count;++i)if(static_cast<u32>(*dg(m,0x2B6C4u+i))==value)return 1u;
    return 0u;
}
static void app71_keyboard_eoi(Application71Boundary& b) noexcept { if(b.out8)b.out8(0x20u,0x20u); }
// 308F8..30AB9: IRQ-9 keyboard state machine. The 19h path is CODE.2-specific.
void interrupt_308f8(Machine& m,Application71Boundary& b) noexcept {
    const CpuState saved=m.cpu;
    if(b.call_3566c)b.call_3566c();
    u32 scan=b.in8?b.in8(0x60u):0u;
    u8 p61=b.in8?b.in8(0x61u):0u; *dg(m,0xF570u)=p61;
    if(p61==0x80u){p61=b.in8?b.in8(0x61u):0u; *dg(m,0xF570u)=static_cast<u8>(p61|0x80u);}
    if(b.out8){b.out8(0x61u,static_cast<u8>(*dg(m,0xF570u)|0x80u)); b.out8(0x61u,*dg(m,0xF570u));}
    if(static_cast<s32>(scan)>0x80){
        scan&=0x7Fu;
        if(scan==0x2Au)*dg(m,0xF573u)&=0xFEu;
        else if(scan==0x36u)*dg(m,0xF573u)&=0xFDu;
        else if(scan==0x38u)*dg(m,0xF573u)&=0xFBu;
        else if(scan==0x1Du)*dg(m,0xF573u)&=0xF7u;
        else if(scan>=0x47u&&scan<=0x51u)function_30748(m,scan);
        else *dg(m,0xF572u)=*dg(m,0xF571u);
    } else {
        if(scan==0x2Au)*dg(m,0xF573u)|=1u;
        else if(scan==0x36u)*dg(m,0xF573u)|=2u;
        else if(scan==0x38u)*dg(m,0xF573u)|=4u;
        else if(scan==0x1Du)*dg(m,0xF573u)|=8u;
        else {if(scan>=0x47u&&scan<=0x51u){function_307f0(m,scan);*dg(m,0xF571u)=static_cast<u8>(scan);}*dg(m,0xF572u)=static_cast<u8>(scan);}
    }
    const u8 code=*dg(m,0xF572u);
    if(code==0x10u&&(*dg(m,0xF573u)&4u)){*dg(m,0xF0B1u)|=1u;*dg(m,0xF572u)=0u;}
    else if(code==0x43u){*dg(m,0xF572u)=0u;app71_keyboard_eoi(b);m.cpu=saved;return;}
    else if(code==0x19u){store_le16(dg(m,0x1457Cu),1u);app71_keyboard_eoi(b);m.cpu=saved;return;}
    else if(code==0x39u)*dg(m,0xF574u)=1u;
    app71_keyboard_eoi(b); m.cpu=saved;
}
[[nodiscard]] u32 function_30abc(Machine& m,u32 mask) noexcept { return static_cast<u32>(*dg(m,0xF573u))&mask; }
[[nodiscard]] u32 function_30ac8(Machine& m) noexcept { const u8 v=*dg(m,0xF574u);*dg(m,0xF574u)=0u;return v; }
[[nodiscard]] u32 function_30ad8(Machine& m,Application71Boundary& b) noexcept { if(b.call_2f868)b.call_2f868();return *dg(m,0xF572u)?1u:0u; }
[[nodiscard]] u32 function_30aec(Machine& m,Application71Boundary& b) noexcept {
    while(*dg(m,0xF572u)==0u){if(!b.call_2f868)return 0u;b.call_2f868();}
    const u8 index=*dg(m,0xF572u); const u32 value=load_le32(dg(m,0xF590u+static_cast<u32>(index)*4u));
    *dg(m,0xF572u)=0u; return value;
}
void function_30b1c(Machine& m) noexcept {*dg(m,0xF588u)=0u;*dg(m,0xF571u)=0u;}
void function_30b2c(Machine& m) noexcept {*dg(m,0xF572u)=*dg(m,0xF571u);*dg(m,0xF574u)=0u;}
[[nodiscard]] u32 function_30b40(Machine& m,Application71Boundary& b) noexcept {
    if(b.call_2f868)b.call_2f868();
    const u8 index=*dg(m,0xF572u); return load_le32(dg(m,0xF590u+static_cast<u32>(index)*4u));
}

// 30B54..30BB3: divide-error cleanup handler, restoring IRQ vectors before far-chain.
void interrupt_30b54(Machine& m,Application71Boundary& b) noexcept {
    const CpuState saved=m.cpu; if(b.call_3566c)b.call_3566c();
    if(b.call_290f4)b.call_290f4(0xFFu);
    if(b.call_33b44)b.call_33b44();
    if(b.set_vector){b.set_vector(9u,load_le32(dg(m,0xF576u)),load_le16(dg(m,0xF57Au)));b.set_vector(8u,load_le32(dg(m,0xF582u)),load_le16(dg(m,0xF586u)));}
    if(b.far_chain)b.far_chain(load_le32(dg(m,0xF57Cu)),load_le16(dg(m,0xF580u)));
    m.cpu=saved;
}
// 30BB4..30C36: install vectors 9, 8, and 0 once.
void function_30bb4(Machine& m,Application71Boundary& b) noexcept {
    if(*dg(m,0xF575u)!=0u)return;
    const u8 vecs[3]={9u,8u,0u}; const u32 handlers[3]={0x308F8u,0x30708u,0x30B54u};
    const u32 offs[3]={0xF576u,0xF582u,0xF57Cu}; const u32 sels[3]={0xF57Au,0xF586u,0xF580u};
    for(u32 i=0;i<3u;++i){u32 off=0u;u16 sel=0u;if(b.get_vector)b.get_vector(vecs[i],&off,&sel);store_le32(dg(m,offs[i]),off);store_le16(dg(m,sels[i]),sel);if(b.set_vector)b.set_vector(vecs[i],handlers[i],m.cpu.cs);}
    *dg(m,0xF575u)=1u;
}
// 30C38..30C90: restore all three saved vectors.
void function_30c38(Machine& m,Application71Boundary& b) noexcept {
    if(*dg(m,0xF575u)==0u)return;
    if(b.set_vector){b.set_vector(9u,load_le32(dg(m,0xF576u)),load_le16(dg(m,0xF57Au)));b.set_vector(8u,load_le32(dg(m,0xF582u)),load_le16(dg(m,0xF586u)));b.set_vector(0u,load_le32(dg(m,0xF57Cu)),load_le16(dg(m,0xF580u)));}
    *dg(m,0xF575u)=0u;
}
[[nodiscard]] u32 function_30c94(Machine& m) noexcept { return *dg(m,0xF575u); }
[[nodiscard]] u32 function_30c9c() noexcept { return 0xF6F4u; }
[[nodiscard]] u32 function_30ca2(Machine& m) noexcept {
    const u32 state=function_30c9c(); if(state==0u)return 0u;
    u32 v=load_le32(dg(m,state));v=v*0x41C64E6Du+0x3039u;store_le32(dg(m,state),v);return(v>>16)&0x7FFFu;
}
void function_30cc4(Machine& m,u32 seed) noexcept { const u32 state=function_30c9c();if(state!=0u)store_le32(dg(m,state),seed); }
// 30CD4..30D25: overlap-safe memmove with CODE.2 guest offsets.
[[nodiscard]] u32 function_30cd4(Machine& m,u32 dst,u32 src,u32 count) noexcept {
    if(dst==src)return dst;
    if(src<dst && src+count>dst){for(u32 i=count;i!=0u;--i)*dg(m,dst+i-1u)=*dg(m,src+i-1u);}
    else {for(u32 i=0;i<count;++i)*dg(m,dst+i)=*dg(m,src+i);}
    return dst;
}

static_assert(
    (0x306F8u-0x305A8u)+(0x30706u-0x30700u)+(0x30736u-0x30708u)+
    (0x30745u-0x30738u)+(0x307C8u-0x30748u)+(0x308B9u-0x307F0u)+
    (0x308F5u-0x308BCu)+(0x30ABAu-0x308F8u)+(0x30AD7u-0x30ABCu)+
    (0x30AEBu-0x30AD8u)+(0x30B19u-0x30AECu)+(0x30B2Bu-0x30B1Cu)+
    (0x30B3Eu-0x30B2Cu)+(0x30C37u-0x30B40u)+(0x30C91u-0x30C38u)+
    (0x30D26u-0x30C94u)==1843u);
