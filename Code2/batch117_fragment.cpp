// Batch 117: verified CODE.2 35A0C..35F86 loader.
// Raw frame storage intentionally preserves byte/word/dword aliasing.
struct Loader35a0cBoundaries {
    u32 (*open_seek)(u32,u32)=nullptr;
    u32 (*load_compact)(u32,u32)=nullptr;
    u32 (*allocate)(u32)=nullptr;
    void (*prepare)(u32,u32,u32)=nullptr;
    u32 (*read_local)(u32,u32,u32,u8*,u32)=nullptr;
    u32 (*read_guest)(u32,u32,u32,u32,u32)=nullptr;
    u32 (*validate_1604)(const u8*,u32)=nullptr;
    void (*close)(u32)=nullptr;
    void (*store_guest32)(u32,u32)=nullptr;
};

[[nodiscard]] u32 function_35a0c(Machine& m,Loader35a0cBoundaries& b,
                                 u32 source,u32 flags,u32 destination) noexcept {
    u8 f[0x2D0]{};
    auto l8=[&](u32 o)->u8{return f[o];};
    auto l16=[&](u32 o)->u16{return load_le16(f+o);};
    auto l32=[&](u32 o)->u32{return load_le32(f+o);};
    auto s32f=[&](u32 o,u32 v){store_le32(f+o,v);};
    auto read_local=[&](u32 h,u32 o,u8* d,u32 n)->u32{return b.read_local?b.read_local(h,o,flags,d,n):o+n;};
    auto fail=[&](u32 h)->u32{if((flags&1u)==0u&&b.close)b.close(h);return 0u;};

    s32f(0x29Cu,load_le32(dg(m,0x35874u)));
    s32f(0x2B4u,0u);
    u32 handle=source;
    if((flags&1u)==0u){handle=b.open_seek?b.open_seek(source,0x200u):0xFFFFFFFFu;if(handle==0xFFFFFFFFu)return 0u;}
    const u32 compact=b.load_compact?b.load_compact(source,flags):0u;
    if((flags&4u)!=0u){destination=b.allocate?b.allocate(compact):0u;if(destination==0u)return fail(handle);}
    const u32 result_destination=destination;
    if(b.prepare)b.prepare(result_destination,0u,compact);
    u32 copy_ptr=result_destination;

    (void)read_local(handle,0x3Cu,f+0x28Cu,4u);
    (void)read_local(handle,l32(0x28Cu),f+0x29Cu,2u); // low-word overwrite only
    if((b.validate_1604?b.validate_1604(f+0x29Cu,0x1604u):0u)!=0u)return fail(handle);
    (void)read_local(handle,l32(0x28Cu),f+0x190u,0xACu);

    u32 outer=0u;
    u32 cursor=l32(0x28Cu)+l32(0x1D0u);
    if(l32(0x1D4u)!=0u){
        do{
            cursor=read_local(handle,cursor,f+0x264u,0x18u);s32f(0x2A8u,cursor);
            u32 inner=0u;u32 inner_cursor=l32(0x28Cu)+l32(0x1D8u);
            if(l32(0x274u)!=0u){
                do{
                    inner_cursor=read_local(handle,inner_cursor,f+0x27Cu,8u);
                    if(inner==0u&&(l8(0x26Cu)&2u)!=0u&&(l8(0x26Cu)&1u)!=0u&&(copy_ptr&0xFu)!=0u)copy_ptr+=0x10u-(copy_ptr&0xFu);
                    const u32 outer4=outer*4u;
                    if(inner==0u)s32f(0x240u+outer4,copy_ptr);
                    s32f(outer4,copy_ptr);
                    const u32 used=copy_ptr-l32(0x240u+outer4);
                    const u32 remaining=l32(0x264u)-used;
                    const u32 described=l16(0x27Cu);
                    const u32 amount=remaining<described?remaining:described;
                    s32f(0x2A4u,amount);
                    const u32 shift=static_cast<u32>(l8(0x1BCu))&31u;
                    const u32 file_off=(l32(0x27Cu)<<shift)+l32(0x210u);
                    if(b.read_guest)(void)b.read_guest(handle,file_off,flags,copy_ptr,amount);
                    copy_ptr+=amount;++inner;
                }while(inner<l32(0x274u));
            }
            ++outer;cursor=l32(0x2A8u);
        }while(outer<l32(0x1D4u));
    }

    u32 relocation_cursor=read_local(handle,l32(0x28Cu)+l32(0x1FCu),f+0x288u,4u);
    u32 group=0u;
    if(l32(0x1A4u)!=0u){
        u32 table_off=0u;
        do{
            relocation_cursor=read_local(handle,relocation_cursor,f+0x284u,4u);
            if(l32(0x288u)!=l32(0x284u)){
                u32 p=l32(0x284u)+l32(0x28Cu)+l32(0x1FCu);
                const u32 end=p;
                do{
                    p=read_local(handle,p,f+0x2CCu,1u);
                    p=read_local(handle,p,f+0x2C4u,1u);
                    if((l8(0x2CCu)&7u)==0u)return fail(handle);
                    if((l8(0x2C4u)&4u)!=0u&&(l8(0x2C4u)&0x20u)==0u)return fail(handle);
                    if((l8(0x2CCu)&0x20u)!=0u)return fail(handle);
                    p=read_local(handle,p,f+0x2C0u,2u);
                    p=read_local(handle,p,f+0x2C8u,1u);
                    p=read_local(handle,p,f+0x2BCu,2u);
                    if(l16(0x2C0u)<=l32(0x1B8u)){
                        const u32 target=l32(table_off)+l16(0x2C0u);
                        const u32 value=l32(0x23Cu+static_cast<u32>(l8(0x2C8u))*4u)+l16(0x2BCu);
                        if(b.store_guest32)b.store_guest32(target,value);
                    }
                }while(p<end);
            }
            s32f(0x288u,l32(0x284u));++group;table_off+=4u;
        }while(group<l32(0x1A4u));
    }
    if((flags&1u)==0u&&b.close)b.close(handle);
    return result_destination;
}
static_assert(0x35F87u-0x35A0Cu==1403u);
