// Batch 110: 34DE8..35285 scene-preparation scan and quad dispatch.
struct Runtime110Boundary {
    // 1E8C8: lookup the scene record at integer grid coordinates (x,y).
    u32 (*call_1e8c8)(s32,s32)=nullptr;
    // 35798: transform packed 16.16 grid coordinates into two destination words.
    void (*call_35798)(Machine&,u32,u32,u32,u32)=nullptr;
};

static void app110_zero(Machine& m,u32 dst,u32 count) noexcept {
    for(u32 i=0;i<count;++i)*dg(m,dst+i)=0u;
}
static void app110_copy(Machine& m,u32 dst,u32 src,u32 count) noexcept {
    for(u32 i=0;i<count;++i)*dg(m,dst+i)=*dg(m,src+i);
}
static u32 app110_fixed16(s32 v) noexcept {
    return static_cast<u32>(static_cast<u16>(v))<<16;
}
static void app110_add20_word(Machine& m,u32 dst) noexcept {
    store_le16(dg(m,dst),static_cast<u16>(load_le16(dg(m,dst))+0x20u));
}

void function_34de8(Machine& m,Runtime110Boundary& b,
                     Runtime109Boundary& quad,Runtime103Boundary& raster) noexcept {
    constexpr u32 A=0x2B7F0u;
    constexpr u32 B=0x2B870u;
    constexpr u32 AF=0x2B7B0u;
    constexpr u32 BF=0x2B7D0u;
    app110_zero(m,A,0x80u);
    app110_zero(m,B,0x80u);
    app110_zero(m,AF,0x20u);
    app110_zero(m,BF,0x20u);

    s32 min_x=static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x255D2u))))-15;
    s32 max_x=static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x255D2u))))+15;
    s32 min_y=static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x255D6u))))-15;
    s32 max_y=static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x255D6u))))+15;
    if(min_x<0)min_x=0;
    const s32 x_limit=static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x25619u))));
    if(x_limit<max_x)max_x=x_limit;
    if(min_y<0)min_y=0;
    const s32 y_limit=static_cast<s32>(static_cast<s16>(load_le16(dg(m,0x2561Bu))));
    if(y_limit<max_y)max_y=y_limit;

    for(s32 x=min_x;x<max_x;++x){
        bool active=false;
        u32 slot=0u;
        for(s32 y=min_y;y<max_y;++y,++slot){
            const u32 record=b.call_1e8c8?b.call_1e8c8(x,y):0u;
            if(record==0u){active=false;continue;}
            const u8 flags=*dg(m,record+1u);
            if((flags&0x10u)==0u){active=false;continue;}
            if(*dg(m,record)==3u && (flags&0x08u)==0u)continue;

            const u32 curA=A+slot*4u;
            const u32 curB=B+slot*4u;
            const u32 nextA=curA+4u;
            const u32 nextB=curB+4u;

            if(!active){
                active=true;
                if(*dg(m,AF+slot)==0u){
                    if(b.call_35798)b.call_35798(m,app110_fixed16(y),app110_fixed16(x),curA,curA+2u);
                    app110_add20_word(m,curA);
                    *dg(m,AF+slot)=1u;
                }
                if(b.call_35798)b.call_35798(m,app110_fixed16(y),app110_fixed16(x+1),curB,curB+2u);
                app110_add20_word(m,curB);
                *dg(m,BF+slot)=1u;
            }

            if(*dg(m,AF+slot+1u)==0u){
                if(b.call_35798)b.call_35798(m,app110_fixed16(y+1),app110_fixed16(x),nextA,nextA+2u);
                app110_add20_word(m,nextA);
                *dg(m,AF+slot+1u)=1u;
            }
            if(b.call_35798)b.call_35798(m,app110_fixed16(y+1),app110_fixed16(x+1),nextB,nextB+2u);
            app110_add20_word(m,nextB);
            *dg(m,BF+slot+1u)=1u;

            if(load_le16(dg(m,curA+2u))==0u || load_le16(dg(m,nextA+2u))==0u ||
               load_le16(dg(m,curB+2u))==0u || load_le16(dg(m,nextB+2u))==0u)continue;

            u32 coords[4]{
                static_cast<u32>(static_cast<s32>(static_cast<s16>(load_le16(dg(m,curA))))),
                static_cast<u32>(static_cast<s32>(static_cast<s16>(load_le16(dg(m,nextA))))),
                static_cast<u32>(static_cast<s32>(static_cast<s16>(load_le16(dg(m,nextB))))),
                static_cast<u32>(static_cast<s32>(static_cast<s16>(load_le16(dg(m,curB)))))
            };
            u32 indices[4]{
                static_cast<u32>(load_le16(dg(m,curA+2u))),
                static_cast<u32>(load_le16(dg(m,nextA+2u))),
                static_cast<u32>(load_le16(dg(m,nextB+2u))),
                static_cast<u32>(load_le16(dg(m,curB+2u)))
            };
            function_34bb0(m,quad,raster,coords,indices,record);
        }

        app110_copy(m,A,B,0x80u);
        app110_zero(m,B,0x80u);
        app110_copy(m,AF,BF,0x20u);
        app110_zero(m,BF,0x20u);
    }
}

static_assert(0x35286u-0x34DE8u==1182u);
// 35286 is a distinct DOS runtime entry with an external direct caller; no Batch-110 separator bytes are claimed.
