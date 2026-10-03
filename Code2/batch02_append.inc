// -----------------------------------------------------------------------------
// Batch 02: 002A8..009AF application command/record helpers.
// -----------------------------------------------------------------------------

struct Batch02EarlyBoundaries {
    u32 (*call_1e8c8)(u32,u32)=nullptr;
    void (*call_20c0c)(u32,u32,u32)=nullptr;
    void (*call_8a80)(u32,u32,u32,u32,u32)=nullptr;
    void (*call_28110)(u32,u32,u32,u32)=nullptr;
    void (*call_28dcc)(u32)=nullptr;
    void (*call_28ef0)()=nullptr;
    void (*call_28c54)(u32,u32)=nullptr;
    void (*call_13ef8)(u32)=nullptr;
    void (*call_1c1a0)(u32)=nullptr;
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
    u32 (*call_2acdc)(u32)=nullptr;
    u32 (*call_2ad18)(u32)=nullptr;
    u32 (*call_2ade0)(u32)=nullptr;
};

static void batch02_call20c0c(Machine& m,Batch02EarlyBoundaries& b,u32 object,u8 byte2,u8 byte3) noexcept {
    u8* p=dg(m,object);
    const u8 saved=static_cast<u8>((p[1]>>2)&1u);
    p[1]=static_cast<u8>(p[1]|4u);
    if(b.call_20c0c)b.call_20c0c(byte3,byte2,object);
    p[1]=static_cast<u8>((p[1]&0xFBu)|(saved<<2));
}

// 002A8..006B8: process the five-byte commands selected by the 13-entry CS
// table at 00274. Callers discard EAX, so this lift exposes the encoded side
// effects and command termination rather than inventing a semantic result.
void function_002a8(Machine& m,EarlyBoundaries& e,Batch02EarlyBoundaries& b,s32 group) noexcept {
    if(group==-1)return;
    const u32 base=load_le32(dg(m,0xCF44u))+static_cast<u32>(group)*0x1Au;
    u32 command=base;
    s32 index=0;
    bool stop=false;
    while(index<static_cast<s32>(*dg(m,base)) && !stop) {
        const u8 byte1=*dg(m,command+1u);
        const u8 byte2=*dg(m,command+2u);
        const u8 byte3=*dg(m,command+3u);
        const u8 byte4=*dg(m,command+4u);
        const u32 object=b.call_1e8c8?b.call_1e8c8(byte3,byte2):0u;
        switch(byte1) {
        case 0:
            if(object==0u){(void)function_00010(m,static_cast<s32>(byte3),static_cast<s32>(byte2));break;}
            if(*dg(m,object)==1u){if(*dg(m,object+6u)!=0u)batch02_call20c0c(m,b,object,byte2,byte3);}
            else *dg(m,object+1u)=static_cast<u8>(*dg(m,object+1u)|4u);
            break;
        case 1:
            if(object==0u)break;
            if(*dg(m,object)==1u){if(*dg(m,object+6u)==0u)batch02_call20c0c(m,b,object,byte2,byte3);}
            else *dg(m,object+1u)=static_cast<u8>(*dg(m,object+1u)&0xFBu);
            break;
        case 2:
            if(object==0u)break;
            if(*dg(m,object)==1u)batch02_call20c0c(m,b,object,byte2,byte3);
            else if(*dg(m,object)==6u){
                const u8 bit=static_cast<u8>((((*dg(m,object+1u)>>2)&1u)^1u)<<2);
                *dg(m,object+1u)=static_cast<u8>((*dg(m,object+1u)&0xFBu)|bit);
            } else {
                const u8 low=static_cast<u8>((*dg(m,object+1u)&3u)^1u);
                *dg(m,object+1u)=static_cast<u8>((*dg(m,object+1u)&0xFCu)|(low&3u));
            }
            break;
        case 3: {
            const s32 dividend=8-static_cast<s32>(*dg(m,command+5u));
            const s32 offset=(dividend%8)*0x2D;
            const u32 packed2=0x8000u|(static_cast<u32>(byte2)<<16);
            const u32 packed3=0x8000u|(static_cast<u32>(byte3)<<16);
            if(b.call_8a80)b.call_8a80(byte4,static_cast<u32>(offset),4u,packed2,packed3);
            break;
        }
        case 4: {
            const s32 table_index=static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x25619u))))*static_cast<s32>(byte2)+static_cast<s32>(byte3);
            const u32 a=0x198E0u+static_cast<u32>(table_index)*2u;
            *dg(m,a)=static_cast<u8>(*dg(m,a)&0xC0u);
            store_le16(dg(m,a),static_cast<u16>(load_le16(dg(m,a))|static_cast<u16>(byte4&0x3Fu)));
            break;
        }
        case 5:
            if(b.call_28110)b.call_28110(load_le32(dg(m,command+4u)),0xFFu,0u,0u);
            break;
        case 6:
            if(b.call_28dcc)b.call_28dcc(0x9C4u);
            if(b.call_28ef0)b.call_28ef0();
            if(b.call_28c54)b.call_28c54(load_le32(dg(m,command+4u)),1u);
            break;
        case 7:
            if(e.call_1c124)(void)e.call_1c124(byte4);
            break;
        case 8: {
            const u32 page=load_le32(dg(m,0xC050u));
            if(b.call_291b0)b.call_291b0(static_cast<u8>(page)^1u);
            if(b.call_1aacc)b.call_1aacc();
            if(b.call_2a020)b.call_2a020(page);
            if(b.call_1a288)b.call_1a288();
            if(b.call_291b0)b.call_291b0(*dg(m,0xC050u));
            if(b.call_1c40c)b.call_1c40c(load_le32(dg(m,0xC050u)),0x10u,0x30u,0xC8u,0xC8u);
            if(b.call_1c368)b.call_1c368(0x759Cu,byte4);
            if(b.call_13fd4)b.call_13fd4(0u,0x10u,0x30u,0x0Au,0xC8u,0x759Cu,1u);
            if(b.call_1a510)b.call_1a510(load_le32(dg(m,0xC050u)));
            *dg(m,0xC050u)^=1u;
            if(b.call_1a1f4){while(b.call_1a1f4()!=0u){} while(b.call_1a1f4()==0u){} while(b.call_1a1f4()!=0u){}}
            if(b.call_1a5b0)b.call_1a5b0(load_le32(dg(m,0xC050u)),static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x8600u)))),*dg(m,0x8602u));
            *dg(m,0xC050u)^=1u;
            break;
        }
        case 9:
            if(b.call_1c1a0)b.call_1c1a0(byte4);
            break;
        case 10:
            if(e.call_1c160)(void)e.call_1c160(byte4,object);
            break;
        case 11:
            if(byte4==0x0Bu){(void)function_00074(m,e);break;}
            if(byte4==0x0Cu){(void)function_000b8(m,e);break;}
            if(byte4==0x0Du){if(function_001fc(m,e)!=0u&&object!=0u)*dg(m,object+1u)=static_cast<u8>(*dg(m,object+1u)&0xFBu);break;}
            if(b.call_13ef8)b.call_13ef8(byte4);
            stop=true;
            break;
        case 12:
            store_le32(dg(m,0xBF44u),byte4);
            break;
        default:
            break;
        }
        if(!stop){command+=5u;++index;}
    }
}
static_assert(0x006B9u-0x002A8u==1041u);

void function_006c0(Machine& m) noexcept {
    s32 x=static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x255D2u))))-0x18;
    s32 y=static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x255D6u))))-0x18;
    store_le32(dg(m,0x12740u),static_cast<u32>(y));
    if(x<-40)x=-40;
    if(static_cast<s32>(load_le32(dg(m,0x12740u)))<-40)store_le32(dg(m,0x12740u),static_cast<u32>(-40));
    store_le32(dg(m,0x12744u),static_cast<u32>(x));
}
static_assert(0x006FDu-0x006C0u==61u);

[[nodiscard]] s32 function_00700(Machine& m,s32 key6,s32 key2,s32 start) noexcept {
    const u32 base=load_le32(dg(m,0x6048u));
    s32 i=start;
    if(*dg(m,0x5FCBu)!=0u){
        u32 off=static_cast<u32>(i)*0x16u;
        while(i<static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x258A6u))))){
            if(static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x2223Eu+off))))==key6 && static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x2223Au+off))))==key2){store_le32(dg(m,0x6048u),base);return i;}
            off+=0x16u;++i;
        }
    } else {
        u32 off=static_cast<u32>(i)*0x16u;
        while(i<static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x604Cu))))){
            const u32 p=base+off;
            if(static_cast<s32>(static_cast<s16>(load_le16(dg(m,p+6u))))==key6 && static_cast<s32>(static_cast<s16>(load_le16(dg(m,p+2u))))==key2){store_le32(dg(m,0x6048u),base);return i;}
            off+=0x16u;++i;
        }
    }
    store_le32(dg(m,0x6048u),base);return -1;
}
static_assert(0x00792u-0x00700u==146u);

[[nodiscard]] s32 function_00794(Machine& m,s32 key1e,s32 key1a,s32 start) noexcept {
    if(*dg(m,0x5FCBu)==0u)return -1;
    s32 i=start;u32 off=static_cast<u32>(i)*0x16u;
    while(i<static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x258A4u))))){
        if(static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x1761Eu+off))))==key1e && static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x1761Au+off))))==key1a)return i;
        off+=0x16u;++i;
    }
    return -1;
}
static_assert(0x007E2u-0x00794u==78u);

[[nodiscard]] s32 function_007e4(Machine& m,s32 key3e,s32 key3a,s32 start) noexcept {
    s32 i=start;u32 off=static_cast<u32>(i)*0x16u;
    while(i<static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x258A6u))))){
        if(load_le16(dg(m,0x22246u+off))==0x54u && static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x2223Eu+off))))==key3e && static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x2223Au+off))))==key3a)return i;
        off+=0x16u;++i;
    }
    return -1;
}
static_assert(0x00839u-0x007E4u==85u);

[[nodiscard]] s32 function_0083c(Machine& m,Batch02EarlyBoundaries& b,s32 key86,s32 key82,s32 start) noexcept {
    s32 i=start;u32 off=static_cast<u32>(i)*0x16u;
    while(i<static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x258A2u))))){
        bool found=false;
        if((*dg(m,0x14D8Bu+off)&2u)==0u){
            const u32 type=load_le16(dg(m,0x14D8Eu+off));
            if((b.call_2acdc?b.call_2acdc(type):0u)!=0u && static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x14D86u+off))))==key86 && static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x14D82u+off))))==key82){
                if(load_le16(dg(m,0xF03Eu))!=0u)found=true;
                else if(load_le16(dg(m,0xF03Cu))!=0u){const u32 p=load_le32(dg(m,0x22230u))+type*0x45u;found=(*dg(m,p+0x3Au)&0x20u)!=0u;}
                else if((*dg(m,0x198E0u+static_cast<u32>(load_le16(dg(m,0x14D8Cu+off)))*2u)&0x40u)!=0u){
                    s32 d=static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x14D86u+off))))-static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x255D2u))));if(d<0)d=-d;
                    s32 e2=static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x14D82u+off))))-static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x255D6u))));if(e2<0)e2=-e2;
                    found=d<=10&&e2<=10;
                }
            }
        }
        if(found)return i;
        off+=0x16u;++i;
    }
    return -1;
}
static_assert(0x00931u-0x0083Cu==245u);

[[nodiscard]] s32 function_00934(Machine& m,Batch02EarlyBoundaries& b,s32 key86,s32 key82,s32 start) noexcept {
    s32 i=start;u32 off=static_cast<u32>(i)*0x16u;
    while(i<static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x258A2u))))){
        if((*dg(m,0x14D8Bu+off)&2u)==0u){
            const u32 type=load_le16(dg(m,0x14D8Eu+off));
            const bool admitted=(b.call_2ad18&&b.call_2ad18(type)!=0u)||(b.call_2ade0&&b.call_2ade0(type)!=0u);
            if(admitted && static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x14D86u+off))))==key86 && static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x14D82u+off))))==key82)return i;
        }
        off+=0x16u;++i;
    }
    return -1;
}
static_assert(0x009B0u-0x00934u==124u);

// -----------------------------------------------------------------------------
// Batch 02: 29930..29CE8 planar VGA helpers.
// -----------------------------------------------------------------------------
struct Graphics02Boundary {
    u8 (*read8)(u32)=nullptr; u16 (*read16)(u32)=nullptr;
    void (*write8)(u32,u8)=nullptr; void (*write16)(u32,u16)=nullptr;
    u8 (*read_code8)(u32)=nullptr; u8 (*in8)(u16)=nullptr;
    void (*out8)(u16,u8)=nullptr; void (*out16)(u16,u16)=nullptr;
    u8 (*bios_get_mode)()=nullptr; void (*bios_set_mode)(u8)=nullptr;
    void (*sync_2ab98)()=nullptr; void (*call_2abf4)(u32)=nullptr; void (*call_2ac10)(u32)=nullptr;
    void (*translate_2ab38)(u32,u32,u32*,u8*)=nullptr;
    void (*planar_fill)(u32,u32,u32,u8,u8,u8,u32,u8)=nullptr;
    void (*sync_2abc4)(u32)=nullptr;
};
static u8 g02r8(Machine& m,Graphics02Boundary& b,u32 a) noexcept{return b.read8?b.read8(a):*dg(m,a);}
static u16 g02r16(Machine& m,Graphics02Boundary& b,u32 a) noexcept{return b.read16?b.read16(a):load_le16(dg(m,a));}
static void g02w8(Machine& m,Graphics02Boundary& b,u32 a,u8 v) noexcept{if(b.write8)b.write8(a,v);else *dg(m,a)=v;}
static void g02w16(Machine& m,Graphics02Boundary& b,u32 a,u16 v) noexcept{if(b.write16)b.write16(a,v);else store_le16(dg(m,a),v);}

[[nodiscard]] u32 function_29930(Machine& m,Graphics02Boundary& b,u32 mode,u32 out) noexcept {
    store_le32(dg(m,0xEFE1u),mode&0x80u);mode&=0x7Fu;store_le32(dg(m,0xEFE5u),mode);
    store_le32(dg(m,0xEFF5u),0xC8u);store_le32(dg(m,0xEFF9u),0x50u);store_le32(dg(m,0xF001u),0xA0000u);
    store_le32(dg(m,0xEFE9u),0u);*dg(m,0xEFE0u)=0u;store_le32(dg(m,0xF005u),0x400u);store_le32(dg(m,0xEFEDu),4u);store_le32(dg(m,0xEFF1u),2u);
    if(b.sync_2ab98)b.sync_2ab98();
    if(mode==3u)store_le32(dg(m,0xF005u),0x800u);
    const u8 vm=b.bios_get_mode?b.bios_get_mode():0x13u;if(vm!=0x13u&&b.bios_set_mode)b.bios_set_mode(0x13u);
    auto in=[&](u16 p){return b.in8?b.in8(p):0u;};auto o8=[&](u16 p,u8 v){if(b.out8)b.out8(p,v);};auto o16=[&](u16 p,u16 v){if(b.out16)b.out16(p,v);};
    o16(0x3CEu,static_cast<u16>(3u|(static_cast<u16>(*dg(m,0xEFE0u))<<8)));o8(0x3C4u,4u);u8 x=in(0x3C5u);o8(0x3C5u,static_cast<u8>((x&0xF7u)|4u));
    o8(0x3CEu,5u);x=in(0x3CFu);o8(0x3CFu,static_cast<u8>(x&0xEFu));o8(0x3CEu,6u);x=in(0x3CFu);o8(0x3CFu,static_cast<u8>(x&0xFDu));
    if(b.sync_2ab98)b.sync_2ab98();
    o8(0x3D4u,9u);x=in(0x3D5u);o8(0x3D5u,mode==3u?static_cast<u8>(x&0xE0u):0x41u);
    o8(0x3D4u,0x14u);x=in(0x3D5u);o8(0x3D5u,static_cast<u8>(x&0xBFu));o8(0x3D4u,0x17u);x=in(0x3D5u);o8(0x3D5u,static_cast<u8>(x|0x40u));
    if(b.call_2abf4)b.call_2abf4(0u);
    if(b.call_2ac10)b.call_2ac10(0xFFu);
    u32 height=0xC8u;if(mode==3u)height<<=1;store_le32(dg(m,0xEFFDu),height);store_le32(dg(m,0xEFE1u),0u);
    g02w8(m,b,out+1u,2u);g02w16(m,b,out+3u,static_cast<u16>(height));g02w16(m,b,out+5u,0x140u);return 0x0Du;
}
static_assert(0x29A84u-0x29930u==340u);
void function_29a84(Graphics02Boundary& b) noexcept{if(b.sync_2ab98)b.sync_2ab98();if(b.bios_set_mode)b.bios_set_mode(3u);}static_assert(0x29A9Fu-0x29A84u==27u);
void function_29b1c(Machine& m,Graphics02Boundary& b,u32 index) noexcept {static constexpr u8 table[4]={0u,8u,0x10u,0x18u};const u8 v=b.read_code8?b.read_code8(0x29B33u+index):(index<4u?table[index]:0u);*dg(m,0xEFE0u)=v;}
void function_29aa0(Machine& m,Graphics02Boundary& b,u32 index) noexcept{function_29b1c(m,b,index);}static_assert(0x29AA2u-0x29AA0u==2u);
void function_29aa4(Machine& m,Graphics02Boundary& b,u32 x,u32 y,u8 value) noexcept {u32 p=0;u8 plane=0;if(b.translate_2ab38)b.translate_2ab38(x,y,&p,&plane);if(b.out16){b.out16(0x3CEu,static_cast<u16>(4u|(static_cast<u16>(plane)<<8)));b.out16(0x3C4u,static_cast<u16>(2u|(static_cast<u16>(1u<<plane)<<8)));b.out16(0x3CEu,static_cast<u16>(3u|(static_cast<u16>(*dg(m,0xEFE0u))<<8)));}(void)g02r8(m,b,p);g02w8(m,b,p,value);if(b.out16)b.out16(0x3CEu,3u);}static_assert(0x29AF0u-0x29AA4u==76u);
[[nodiscard]] u8 function_29af0(Machine& m,Graphics02Boundary& b,u32 x,u32 y) noexcept{u32 p=0;u8 plane=0;if(b.translate_2ab38)b.translate_2ab38(x,y,&p,&plane);if(b.out16)b.out16(0x3CEu,static_cast<u16>(4u|(static_cast<u16>(plane)<<8)));return g02r8(m,b,p);}static_assert(0x29B19u-0x29AF0u==41u);
static_assert(0x29B33u-0x29B1Cu==23u);
static_assert(0x29B3Eu-0x29B37u==7u);
static u8 sar8_exact_29b40(u8 v,u32 count) noexcept{count&=0x1Fu;if(count==0u)return v;const bool neg=(v&0x80u)!=0u;if(count>=8u)return neg?0xFFu:0u;u8 r=static_cast<u8>(v>>count);if(neg)r=static_cast<u8>(r|static_cast<u8>(0xFFu<<(8u-count)));return r;}
void function_29b40(Machine& m,Graphics02Boundary& b,s32 x,s32 y,s32 w,s32 h,u8 color) noexcept {if(w<=0||h<=0)return;u32 base=0;u8 plane=0;if(b.translate_2ab38)b.translate_2ab38(static_cast<u32>(x),static_cast<u32>(y),&base,&plane);u8 left=static_cast<u8>(0x0Fu<<plane),right=0u;u32 work=static_cast<u32>(h-1),phase=static_cast<u32>(plane^3u),middle=0u;const bool single=work<=phase;work-=phase;if(!single){--work;const u32 rem=work&3u;work-=rem;const u32 shift=rem^3u;right=static_cast<u8>(~sar8_exact_29b40(0xF0u,shift));middle=work>>2;}else{work+=3u;const u32 shift=static_cast<u32>(static_cast<u8>(work)^3u);right=static_cast<u8>(~sar8_exact_29b40(0xF0u,shift));left=static_cast<u8>(left&right);right=0u;middle=0u;}u32 stride=load_le32(dg(m,0xEFF9u))-middle-1u;if(right!=0u)stride=(stride&0xFFFF0000u)|static_cast<u16>(static_cast<u16>(stride)-1u);if(b.planar_fill)b.planar_fill(base,static_cast<u32>(w),middle,left,right,color,stride,*dg(m,0xEFE0u));}static_assert(0x29C9Eu-0x29B40u==350u);
void function_29ca0(Machine& m,Graphics02Boundary& b,u32 src,u32 first,u32 count) noexcept {if(b.out8)b.out8(0x3C8u,static_cast<u8>(first));u32 p=src,n=count;for(;;){if(b.sync_2abc4)b.sync_2abc4(n);if(static_cast<s32>(n)>0xC0){if(b.out8)for(u32 i=0;i<0xC0u;++i)b.out8(0x3C9u,g02r8(m,b,p+i));p+=0xC0u;n-=0xC0u;continue;}if(n!=0u&&b.out8)for(u32 i=0;i<n;++i)b.out8(0x3C9u,g02r8(m,b,p+i));break;}}static_assert(0x29CE9u-0x29CA0u==73u);

// 2A1E8..2A37F image-layout helpers: byte-identical to audited CODE.1 A558..A6EF.
[[nodiscard]] u32 function_2a1e8(u32 frames,u32 rows,u32 bytes_per_row,u32,u32 key) noexcept{u32 v=rows*bytes_per_row;v*=frames;v<<=3;if(key!=0xFFu)v+=frames;return v;}static_assert(0x2A20Cu-0x2A1E8u==36u);
void function_2a20c(Machine& m,Graphics02Boundary& b,u32 frame,u32 count,u32 src,u32 desc,u32 key) noexcept {u32 dst=desc+8u+static_cast<u32>(g02r16(m,b,desc))*frame;if(static_cast<u8>(key)!=0xFFu)g02w8(m,b,dst++,static_cast<u8>(key));const u32 groups=count>>2;u32 left=groups,s=src;do{const u32 base=dst;g02w8(m,b,base,g02r8(m,b,s++));g02w8(m,b,base+groups,g02r8(m,b,s++));g02w8(m,b,base+groups*2u,g02r8(m,b,s++));g02w8(m,b,base+groups*3u,g02r8(m,b,s++));dst=base+1u;}while(--left!=0u);}static_assert(0x2A256u-0x2A20Cu==74u);
void function_2a258(Machine& m,Graphics02Boundary& b,u32 frame,u32 desc) noexcept {u32 payload=desc+8u+static_cast<u32>(g02r16(m,b,desc))*frame;const u16 rows=g02r16(m,b,desc+4u);const u16 plane_bytes=static_cast<u8>(static_cast<u8>(g02r8(m,b,desc+6u))<<1);if((g02r8(m,b,desc+7u)&0x80u)!=0u)++payload;const u16 span=static_cast<u16>(rows*plane_bytes);auto swap_rows=[&](u32 lo,u16 offset){u16 rr=rows;do{u32 hi=lo+static_cast<u32>(offset);u16 cc=plane_bytes;do{const u8 v=g02r8(m,b,lo);const u8 z=g02r8(m,b,hi);g02w8(m,b,lo,z);g02w8(m,b,hi,v);++lo;--hi;}while(--cc!=0u);}while(--rr!=0u);};u16 off0=span;off0=static_cast<u16>(off0<<1);off0=static_cast<u16>(off0+span);u16 tail=plane_bytes;--tail;off0=static_cast<u16>(off0+tail);swap_rows(payload,off0);u16 off1=span;tail=plane_bytes;--tail;off1=static_cast<u16>(off1+tail);swap_rows(payload+static_cast<u32>(span),off1);}static_assert(0x2A305u-0x2A258u==173u);
void function_2a308(Machine& m,Graphics02Boundary& b,u32 frame,u32 desc) noexcept {u32 src=desc+8u+static_cast<u32>(g02r16(m,b,desc))*frame;const u32 rows=g02r16(m,b,desc+4u),width=g02r8(m,b,desc+6u);if((g02r8(m,b,desc+7u)&0x80u)!=0u)++src;u32 bottom=(rows-1u)*width;bottom<<=1;const u32 rowbytes=width<<1;u32 planes=4u;do{const u32 plane_start=src;u32 lo=src,hi=src+bottom;u32 pairs=rows>>1;do{u32 cc=rowbytes;do{const u8 v=g02r8(m,b,lo);const u8 z=g02r8(m,b,hi);g02w8(m,b,lo,z);g02w8(m,b,hi,v);++lo;++hi;}while(--cc!=0u);hi-=rowbytes;hi-=rowbytes;}while(--pairs!=0u);src=plane_start+bottom+rowbytes;}while(--planes!=0u);}static_assert(0x2A380u-0x2A308u==120u);
