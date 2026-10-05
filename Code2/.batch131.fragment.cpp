
// Batch 131: DOS open/create descriptor setup and handle wrappers.
struct Runtime131Boundary {
    s32 (*dos_open)(u32 path,u8 access_share)=nullptr;
    s32 (*dos_create)(u32 path,u8 attr)=nullptr;
    s32 (*dos_close)(u32 handle)=nullptr;
    s32 (*dos_zero_write)(u32 handle)=nullptr;
    s32 (*dos_ioctl_get)(u32 handle)=nullptr;
    s32 (*dos_ioctl_set)(u32 handle,u8 info)=nullptr;
    u32 (*descriptor_flags)(u32 handle)=nullptr;       // 3A0C4
    bool (*descriptor_is_device)(u32 handle)=nullptr; // 3A0A8
    void (*descriptor_publish)(u32 handle,u32 flags)=nullptr; // 3A11A
    void (*translate_dos_error)(u16 error)=nullptr;   // 390D7
    void (*set_errno)(u32 value)=nullptr;             // 37CC2
    u8 (*read8)(u32 address)=nullptr;
    u32 create_mask=0;                                // global 12690h
    u32 runtime_mode=0;                               // global 0F905h
};

[[nodiscard]] s32 function_37e99(Runtime131Boundary& b,u32 path,u32 mode,u32 share,u32 create_mode) noexcept {
    while(b.read8 && b.read8(path)==0x20u)++path;
    const u8 access=static_cast<u8>(mode&0x83u);
    const u8 access_share=static_cast<u8>(access|static_cast<u8>(share));
    s32 raw=b.dos_open?b.dos_open(path,access_share):-1;
    s32 handle=raw>=0?raw:-1;

    if((mode&3u)!=0u && handle!=-1 && b.descriptor_is_device && b.descriptor_is_device(static_cast<u32>(handle))){
        if((mode&0x400u)!=0u){
            if(b.dos_close)(void)b.dos_close(static_cast<u32>(handle));
            if(b.set_errno)b.set_errno(7u);
            return -1;
        }
        if((mode&0x40u)!=0u && b.dos_zero_write && b.dos_zero_write(static_cast<u32>(handle))<0){
            if(b.dos_close)(void)b.dos_close(static_cast<u32>(handle));
            if(b.translate_dos_error)b.translate_dos_error(static_cast<u16>(raw));
            return -1;
        }
    }

    if(handle==-1){
        if((mode&0x20u)==0u){if(b.translate_dos_error)b.translate_dos_error(static_cast<u16>(raw));return -1;}
        if(static_cast<u16>(raw)!=2u){if(b.translate_dos_error)b.translate_dos_error(static_cast<u16>(raw));return -1;}
        const u32 cm=create_mode?create_mode:0x180u;
        const u8 attr=static_cast<u8>(((cm^b.create_mask)&0x80u)?0u:1u);
        raw=b.dos_create?b.dos_create(path,attr):-1;
        if(raw<0){if(b.translate_dos_error)b.translate_dos_error(static_cast<u16>(raw));return -1;}
        if(b.dos_close && b.dos_close(static_cast<u32>(raw))<0){if(b.translate_dos_error)b.translate_dos_error(static_cast<u16>(raw));return -1;}
        raw=b.dos_open?b.dos_open(path,access_share):-1;
        if(raw<0){if(b.translate_dos_error)b.translate_dos_error(static_cast<u16>(raw));return -1;}
        handle=raw;
    }

    u32 f=b.descriptor_flags?b.descriptor_flags(static_cast<u32>(handle)):0u;
    f=(f&0xFFFFFF00u)|((f&0xFFu)&0x3Cu);
    if(b.descriptor_is_device && b.descriptor_is_device(static_cast<u32>(handle)))f|=0x2000u; // encoded BH bit 20h
    const u32 rw=mode&0x7Fu;
    if(rw==2u)f|=3u; else if(rw==0u)f|=1u; else if(rw==1u)f|=2u;
    if((mode&0x10u)!=0u)f|=0x80u;
    const u8 mode_hi=static_cast<u8>(mode>>8);
    if((mode_hi&3u)==0u ? (b.runtime_mode==0x200u) : ((mode_hi&2u)!=0u)) f|=0x40u;
    if(b.descriptor_publish)b.descriptor_publish(static_cast<u32>(handle),f);
    return handle;
}

[[nodiscard]] s32 function_37e77(Runtime131Boundary& b,u32 path,u32 mode,u32 create_mode) noexcept { return function_37e99(b,path,mode,0u,create_mode); }
static_assert(0x38075u-0x37E77u==510u);

// 38075..380E0: publish flags and, when BH bit 20h is present, set DOS IOCTL bit 20h.
[[nodiscard]] s32 function_38075(Runtime131Boundary& b,u32 handle) noexcept {
    u32 f=b.descriptor_flags?b.descriptor_flags(handle):0u; f=(f&0xFFFFFF00u)|static_cast<u8>(f|0x40u);
    if(b.descriptor_publish)b.descriptor_publish(handle,f);
    if((f&0x2000u)==0u)return 0;
    const s32 info=b.dos_ioctl_get?b.dos_ioctl_get(handle):-1;
    if(info<0){if(b.translate_dos_error)b.translate_dos_error(static_cast<u16>(info));return -1;}
    if(b.dos_ioctl_set && b.dos_ioctl_set(handle,static_cast<u8>(info|0x20))<0){if(b.translate_dos_error)b.translate_dos_error(static_cast<u16>(info));return -1;}
    return 0;
}
static_assert(0x380E1u-0x38075u==108u);

[[nodiscard]] s32 function_380e1(Runtime131Boundary& b,u32 path,u32 create_mode) noexcept { return function_37e77(b,path,0x62u,create_mode); }
static_assert(0x380F8u-0x380E1u==23u);

struct Runtime131CloseBoundary { s32 (*descriptor_close)(u32)=nullptr; };
[[nodiscard]] s32 function_380f8(Runtime131CloseBoundary& b,u32 handle) noexcept { return b.descriptor_close?b.descriptor_close(handle):-1; }
static_assert(0x38106u-0x380F8u==14u);
