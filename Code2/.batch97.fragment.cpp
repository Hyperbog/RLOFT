// Batch 97: 33A38..33A7B DOS mkdir/chdir wrappers with carry-to-error mapping.
struct Runtime97DosResult {
    u32 eax=0u;
    bool carry=false;
};
struct Runtime97Boundary {
    u16 ds_selector=0u;
    Runtime97DosResult (*make_directory)(u16,u32)=nullptr;
    Runtime97DosResult (*change_directory)(u16,u32)=nullptr;
    void (*record_dos_error)(u16)=nullptr;
};

static u32 app97_path_status(Runtime97Boundary& b,const Runtime97DosResult& r) noexcept {
    if(!r.carry)return 0u;
    const u16 error=static_cast<u16>(r.eax);
    if(b.record_dos_error)b.record_dos_error(error);
    // 33A38/33A5A keep 390D7's EAX return, which is FFFFFFFFh.
    return 0xFFFFFFFFu;
}

u32 function_33a38(Runtime97Boundary& b,u32 path) noexcept {
    const Runtime97DosResult r=b.make_directory?
        b.make_directory(b.ds_selector,path):Runtime97DosResult{};
    return app97_path_status(b,r);
}

u32 function_33a5a(Runtime97Boundary& b,u32 path) noexcept {
    const Runtime97DosResult r=b.change_directory?
        b.change_directory(b.ds_selector,path):Runtime97DosResult{};
    return app97_path_status(b,r);
}

static_assert(0x33A7Cu-0x33A38u==68u);
