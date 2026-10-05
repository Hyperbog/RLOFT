
// Batch 130: errno setters, stream-buffer initialization/flush, and DOS attribute query.
struct Runtime130Boundary {
    void (*errno_set)(u32)=nullptr; void (*dos_errno_set)(u32)=nullptr;
    void (*stream_prepare)(u32)=nullptr; u32 (*alloc)(u32)=nullptr;
    s32 (*stream_write)(u32,u32,u32)=nullptr; s32 (*stream_seek)(u32,s32,u32)=nullptr;
    s32 (*dos_attr)(u32,u32)=nullptr;
};
void function_37cc2(Runtime130Boundary& b,u32 value) noexcept { if(b.errno_set)b.errno_set(value); }
void function_37ca3(Runtime130Boundary& b) noexcept { function_37cc2(b,13u); }
void function_37cae(Runtime130Boundary& b) noexcept { function_37cc2(b,14u); }
static_assert(0x37CCEu-0x37CA3u==43u);
void function_37cce(Runtime130Boundary& b,u32 value) noexcept { if(b.dos_errno_set)b.dos_errno_set(value); }
static_assert(0x37CD5u-0x37CCEu==7u);
void function_37cd5(void (*callback)(u32),u32 value) noexcept { if(callback) callback(value); }
static_assert(0x37CE0u-0x37CD5u==11u);
struct Runtime130Stream { u32 cur=0,pos=0,buffer=0; u8 flags0=0,flags1=0; u16 pad=0; u32 handle=0,capacity=0,local_addr=0; };
void function_37ce0(Runtime130Boundary& b,u32 stream_addr,Runtime130Stream& s) noexcept {
    if(b.stream_prepare)b.stream_prepare(stream_addr);
    if(s.capacity==0u)s.capacity=(s.flags1&2u)?0x86u:((s.flags1&4u)?1u:0x1000u);
    const u32 p=b.alloc?b.alloc(s.capacity):0u; s.buffer=p;
    if(p==0u){s.capacity=1u;s.buffer=s.local_addr;s.flags1=static_cast<u8>((s.flags1&0xF8u)|4u);}else s.flags0|=8u;
    s.cur=s.buffer;s.pos=0u;
}
static_assert(0x37D60u-0x37CE0u==128u);
[[nodiscard]] s32 function_37d60(Runtime130Boundary& b,Runtime130Stream& s) noexcept {
    s32 rc=0;
    if(s.flags1&0x10u){s.flags1&=0xEFu;if((s.flags0&2u)&&s.buffer!=0u&&b.stream_write){const s32 w=b.stream_write(s.handle,s.buffer,s.pos);if(w==-1||static_cast<u32>(w)!=s.pos){s.flags0|=0x20u;rc=-1;}}}
    else if(s.buffer!=0u){s.flags0&=0xEFu;if(!(s.flags1&0x20u)&&s.pos!=0u&&b.stream_seek&&b.stream_seek(s.handle,-static_cast<s32>(s.pos),1u)==-1){s.flags0|=0x20u;rc=-1;}}
    s.pos=0u;s.cur=s.buffer;return rc;
}
static_assert(0x37E24u-0x37D60u==196u);
[[nodiscard]] s32 function_37e39(Runtime130Boundary& b,u32 path,u32 mode) noexcept {return b.dos_attr?b.dos_attr(path,mode):-1;}
[[nodiscard]] s32 function_37e24(Runtime130Boundary& b,u32 path,u32 mode) noexcept {return function_37e39(b,path,mode);}
static_assert(0x37E77u-0x37E24u==83u);
