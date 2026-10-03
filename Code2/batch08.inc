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
