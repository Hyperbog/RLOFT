// Batch 111: 35286..353A1 DOS/runtime helper cluster before embedded startup data.
struct Runtime111Boundary {
    // INT 21h/AH=36h. Return true when the interrupt completes; AX=FFFFh still denotes DOS failure.
    bool (*int21_36)(u8,u16&,u16&,u16&,u16&)=nullptr; // drive -> DX,BX,AX,CX
    u32 (*call_37cb2)()=nullptr;
    void (*call_1025c)()=nullptr;
    void (*call_10260)()=nullptr;
    void (*call_10264)()=nullptr;
    void (*tail_3562f)(u32)=nullptr;
    s32 (*call_39526)(Machine&,u32,u32,u32)=nullptr;
    void (*tail_35418)(Machine&)=nullptr;
};

u32 function_35286(Runtime111Boundary& b,u32 drive,u16* out) noexcept {
    u16 dx=0u,bx=0u,ax=0xFFFFu,cx=0u;
    const bool completed=b.int21_36?b.int21_36(static_cast<u8>(drive),dx,bx,ax,cx):false;
    if(completed && ax!=0xFFFFu){
        if(out){out[0]=dx;out[1]=bx;out[2]=ax;out[3]=cx;}
        return 0u;
    }
    return b.call_37cb2?b.call_37cb2():0u;
}

void function_352bd() noexcept {}

void function_352d8(Runtime111Boundary& b,u32 arg) noexcept {
    if(b.call_10260)b.call_10260();
    if(b.call_10264)b.call_10264();
    if(b.tail_3562f)b.tail_3562f(arg);
}

void function_352be(Runtime111Boundary& b,u32 arg) noexcept {
    if(b.call_1025c)b.call_1025c();
    if(b.call_10260)b.call_10260();
    function_352d8(b,arg);
}

u32 function_352ee(Machine& m,Runtime111Boundary& b,u32 query) noexcept {
    u32 list=load_le32(dg(m,0x12688u));
    if(list==0u || query==0u)return 0u;
    u32 length=0u;
    while(*dg(m,query+length)!=0u)++length;
    for(;;list+=4u){
        const u32 entry=load_le32(dg(m,list));
        if(entry==0u)return 0u;
        const s32 cmp=b.call_39526?b.call_39526(m,entry,query,length):1;
        if(cmp==0 && *dg(m,entry+length)==0x3Du)return entry+length+1u;
    }
}

static bool app111_ctype(Machine& m,u8 ch,u8 mask) noexcept {
    const u8 index=static_cast<u8>(ch+1u);
    return (*dg(m,0x165Cu+static_cast<u32>(index))&mask)!=0u;
}

u32 function_35349(Machine& m,u32 text) noexcept {
    u32 p=text;
    while(app111_ctype(m,*dg(m,p),0x02u))++p;
    const u8 sign=*dg(m,p);
    if(sign==0x2Bu || sign==0x2Du)++p;
    u32 value=0u;
    while(app111_ctype(m,*dg(m,p),0x20u)){
        value=value*10u+static_cast<u32>(*dg(m,p))-0x30u;
        ++p;
    }
    if(sign==0x2Du)value=0u-value;
    return value;
}

void function_353a0(Machine& m,Runtime111Boundary& b) noexcept {
    if(b.tail_35418)b.tail_35418(m);
}

static_assert(0x352BDu-0x35286u==55u);
static_assert(0x352BEu-0x352BDu==1u);
static_assert(0x352D8u-0x352BEu==26u);
static_assert(0x352EEu-0x352D8u==22u);
static_assert(0x35349u-0x352EEu==91u);
static_assert(0x353A0u-0x35349u==87u);
static_assert(0x353A2u-0x353A0u==2u);
// 353A2..35417 is 118 bytes of embedded runtime data skipped by 353A0's JMP 35418.
