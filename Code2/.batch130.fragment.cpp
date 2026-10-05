
// Batch 130: errno setters, stream-buffer initialization/flush, and DOS attribute query.
struct Runtime130Boundary {
    u32 (*errno_ptr)()=nullptr;
    u32 (*dos_errno_ptr)()=nullptr;
    void (*stream_prepare)(u32)=nullptr;
    i32 (*stream_write)(u32,const u8*,u32)=nullptr;
    i32 (*stream_seek)(u32,i32,u32)=nullptr;
    i32 (*dos_attr)(u32,u32)=nullptr;
};

// 37CA3..37CCD: small errno entry points and common setter. The two public
// wrappers install constants 13 and 14; the common tail writes through the
// runtime errno pointer returned by 39FF0h.
void function_37ca3(Runtime130Boundary& b) noexcept { if(b.errno_ptr) function_write32(b.errno_ptr(),13u); }
void function_37cae(Runtime130Boundary& b) noexcept { if(b.errno_ptr) function_write32(b.errno_ptr(),14u); }
void function_37cc2(Runtime130Boundary& b,u32 value) noexcept { if(b.errno_ptr) function_write32(b.errno_ptr(),value); }
static_assert(0x37CCEu-0x37CA3u==43u);

// 37CCE..37CDF: alternate errno-domain setter and indirect runtime callback.
void function_37cce(Runtime130Boundary& b,u32 value) noexcept { if(b.dos_errno_ptr) function_write32(b.dos_errno_ptr(),value); }
static_assert(0x37CD5u-0x37CCEu==7u);
void function_37cd5(void (*callback)(u32),u32 value) noexcept { if(callback) callback(value); }
static_assert(0x37CE0u-0x37CD5u==11u);

// Minimal reconstruction of the stream fields touched by 37CE0/37D60.
struct Runtime130Stream { u32 cur=0,pos=0,buffer=0; u8 flags0=0,flags1=0; u16 pad=0; u32 handle=0,capacity=0; u8 local[1]{}; };

// 37CE0..37D5F: prepare a stream buffer, choose default capacity from flag bits,
// allocate it, and fall back to the embedded one-byte buffer on allocation failure.
void function_37ce0(Runtime130Boundary& b,Runtime130Stream& s) noexcept {
    if(b.stream_prepare)b.stream_prepare(reinterpret_cast<u32>(&s));
    if(s.capacity==0u) s.capacity=(s.flags1&2u)?0x86u:((s.flags1&4u)?1u:0x1000u);
    const u32 p=function_37b34(*reinterpret_cast<Runtime129Boundary*>(&b),s.capacity);
    s.buffer=p;
    if(p==0u){ s.capacity=1u; s.buffer=reinterpret_cast<u32>(&s.local[0]); s.flags1=static_cast<u8>((s.flags1&0xF8u)|4u); }
    else s.flags0|=8u;
    s.cur=s.buffer; s.pos=0u;
}
static_assert(0x37D60u-0x37CE0u==128u);

// 37D60..37E23: flush/synchronize stream state. Buffered output is written via
// the runtime primitive; input streams seek backward by unread bytes when needed.
[[nodiscard]] i32 function_37d60(Runtime130Boundary& b,Runtime130Stream& s) noexcept {
    i32 rc=0;
    if(s.flags1&0x10u){
        s.flags1&=0xEFu;
        if((s.flags0&2u) && s.buffer!=0u && b.stream_write){
            const i32 w=b.stream_write(s.handle,reinterpret_cast<const u8*>(s.buffer),s.pos);
            if(w==-1 || static_cast<u32>(w)!=s.pos){ s.flags0|=0x20u; rc=-1; }
        }
    } else if(s.buffer!=0u){
        s.flags0&=0xEFu;
        if(!(s.flags1&0x20u) && s.pos!=0u && b.stream_seek){ if(b.stream_seek(s.handle,-static_cast<i32>(s.pos),1u)==-1){s.flags0|=0x20u;rc=-1;} }
    }
    s.pos=0u; s.cur=s.buffer; return rc;
}
static_assert(0x37E24u-0x37D60u==196u);

// 37E24..37E76: DOS INT 21h/AH=43h attribute query wrapper. The encoded path
// rejects a read-only result when caller flag bit 1 requests writable access,
// then maps the carry/error pair through the runtime DOS-error translator.
[[nodiscard]] i32 function_37e39(Runtime130Boundary& b,u32 path,u32 mode) noexcept {
    return b.dos_attr?b.dos_attr(path,mode):-1;
}
[[nodiscard]] i32 function_37e24(Runtime130Boundary& b,u32 path,u32 mode) noexcept { return function_37e39(b,path,mode); }
static_assert(0x37E77u-0x37E24u==83u);
