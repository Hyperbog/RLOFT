// Batch 80: 32220..324CA palette transition/copy/fill/interpolation subsystem.
static s32 app80_sar(s32 value,u32 count) noexcept {
    count&=31u; if(count==0u)return value;
    const u32 u=static_cast<u32>(value);
    const u32 fill=(u&0x80000000u)?(~0u<<(32u-count)):0u;
    return static_cast<s32>((u>>count)|fill);
}

void function_32468(Machine& m,u32 dst,s32 start,s32 count,s32 factor,u32 shift,const u8* target) noexcept;

static void app80_restore_slice(Machine& m,u32 dst,u32 src,s32 start,s32 count) noexcept {
    const u32 start_bytes=static_cast<u32>(start)*3u;
    const u32 bytes=static_cast<u32>(count)*3u;
    for(u32 i=0;i<bytes;++i)*dg(m,dst+start_bytes+i)=*dg(m,src+start_bytes+i);
}

void function_32220(Machine& m,Application56Boundary& b,u32 source,s32 start,s32 end,u32 r,u32 g,u32 bl) noexcept {
    const u8 target[3]={static_cast<u8>(r),static_cast<u8>(g),static_cast<u8>(bl)};
    const s32 count=app74_wrap_add(app74_wrap_add(end,-start),1);
    for(u32 i=0;i<0x300u;++i)*dg(m,0xF9D0u+i)=*dg(m,source+i);
    for(s32 factor=7;factor>=0;--factor){
        function_32468(m,0xF9D0u,start,count,factor,3u,target);
        function_29234(b,0xF9D0u);
        app80_restore_slice(m,0xF9D0u,source,start,count);
    }
    function_2f83c(m);
}
static_assert(0x322ECu-0x32220u==204u);

void function_322ec(Machine& m,Application56Boundary& b,u32 source,s32 start,s32 end,u32 r,u32 g,u32 bl) noexcept {
    const u8 target[3]={static_cast<u8>(r),static_cast<u8>(g),static_cast<u8>(bl)};
    const s32 count=app74_wrap_add(app74_wrap_add(end,-start),1);
    for(u32 i=0;i<0x300u;++i)*dg(m,0xF9D0u+i)=*dg(m,source+i);
    for(s32 factor=1;factor<=8;++factor){
        function_32468(m,0xF9D0u,start,count,factor,3u,target);
        function_29234(b,0xF9D0u);
        app80_restore_slice(m,0xF9D0u,source,start,count);
    }
    function_2f844(m);
}
static_assert(0x323B9u-0x322ECu==205u);
// 323B9..323BB is LEA alignment.

void function_323bc(Machine& m,u32 dst,s32 dst_index,u32 src,s32 src_index,s32 count) noexcept {
    const u32 dst_off=static_cast<u32>(dst_index)*3u;
    const u32 src_off=static_cast<u32>(src_index)*3u;
    const u32 bytes=static_cast<u32>(count)*3u;
    for(u32 i=0;i<bytes;++i)*dg(m,dst+dst_off+i)=*dg(m,src+src_off+i);
}
static_assert(0x32404u-0x323BCu==72u);

void function_32404(Machine& m,u32 dst,s32 start,u32 c0,u32 c1,u32 c2,s32 count) noexcept {
    const u8 triple[3]={static_cast<u8>(c0),static_cast<u8>(c1),static_cast<u8>(c2)};
    u32 p=dst+static_cast<u32>(start)*3u;
    if(count<=0)return;
    for(s32 i=0;i<count;++i){for(u32 j=0;j<3u;++j)*dg(m,p+j)=triple[j];p+=3u;}
}
static_assert(0x32468u-0x32404u==100u);

void function_32468(Machine& m,u32 dst,s32 start,s32 count,s32 factor,u32 shift,const u8* target) noexcept {
    u32 p=dst+static_cast<u32>(start)*3u;
    if(count<=0)return;
    const u32 sh=static_cast<u8>(shift)&31u;
    for(s32 row=0;row<count;++row){
        for(u32 j=0;j<3u;++j){
            const s32 base=target[j];
            const s32 current=*dg(m,p+j);
            const s32 delta=app80_sar(app74_wrap_mul(current-base,factor),sh);
            *dg(m,p+j)=static_cast<u8>(app74_wrap_add(base,delta));
        }
        p+=3u;
    }
}
static_assert(0x324CBu-0x32468u==99u);
// 324CB is NOP alignment; 324CC begins the next independent wrapper.
