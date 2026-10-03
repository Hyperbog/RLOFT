
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
// original; the structured call here retains its return value while accounting
// only for the 07874..07937 prefix in this batch.
[[nodiscard]] u32 function_07874(Machine& m,Application11Boundary& b,Application10Boundary& a10,
                                  Application09Boundary& a9) noexcept {
    app11_slot_digits(m,0u);
    if(b.call_33a5a&&b.call_33a5a(0x668Cu)!=0u){if(b.call_1c21c)b.call_1c21c(load_le32(dg(m,0x60BCu)));return 1u;}
    if(b.call_33a5a&&b.call_33a5a(0x3C4u)!=0u&&b.call_2f580)b.call_2f580(load_le32(dg(m,0x60B0u)));
    if(b.call_33a5a&&b.call_33a5a(0xC020u)!=0u){if(b.call_1c21c)b.call_1c21c(load_le32(dg(m,0x60BCu)));return 1u;}
    function_07378(m,a10);
    if(b.call_33a5a&&b.call_33a5a(0x3C4u)!=0u&&b.call_2f580)b.call_2f580(load_le32(dg(m,0x60B0u)));
    if(function_073a0(m,a10,0xC020u,0x668Cu)==1u&&b.call_2f580)b.call_2f580(load_le32(dg(m,0x60C0u)));
    *dg(m,0x6692u)=0x5Cu;
    return function_07938(m,a10,a9);
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
