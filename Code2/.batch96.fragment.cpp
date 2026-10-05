// Batch 96: 339A2..33A37 DOS find-first/find-next wrappers and mode-9 DTA shuttles.
struct Runtime96DosResult {
    u32 eax=0u;
    bool carry=false;
};
struct Runtime96DtaAddress {
    u16 es=0u;
    u32 ebx=0u;
};
struct Runtime96Boundary {
    u16 ds_selector=0u;
    void (*set_dta)(u16,u32)=nullptr;
    Runtime96DosResult (*find_first)(u16,u32,u16)=nullptr;
    Runtime96DosResult (*find_next)()=nullptr;
    Runtime96DtaAddress (*get_dta)()=nullptr;
    void (*rep_movsb)(u16,u32,u16,u32,u32)=nullptr;
    void (*record_dos_error)(u16)=nullptr;
};

static u32 app96_status(Runtime96Boundary& b,const Runtime96DosResult& r) noexcept {
    if(!r.carry)return 0u;
    const u16 error=static_cast<u16>(r.eax);
    if(b.record_dos_error)b.record_dos_error(error);
    // 3908C discards 390D7's EAX return by POP EAX and returns the original low-16 error.
    return static_cast<u32>(error);
}

void function_339f2(Machine& m,Runtime96Boundary& b,u32 edx) noexcept {
    if(*dg(m,0x12662u)!=9u)return;
    const Runtime96DtaAddress dta=b.get_dta?b.get_dta():Runtime96DtaAddress{};
    // Encoded REP MOVSB is DS:EBX -> ES:EDX after AH=2F. Keep selectors explicit.
    if(b.rep_movsb)b.rep_movsb(dta.es,edx,b.ds_selector,dta.ebx,0x2Bu);
}

void function_33a15(Machine& m,Runtime96Boundary& b,u32 edx) noexcept {
    if(*dg(m,0x12662u)!=9u)return;
    const Runtime96DtaAddress dta=b.get_dta?b.get_dta():Runtime96DtaAddress{};
    // Encoded REP MOVSB is DS:EDX -> ES:EBX after AH=2F. Keep selectors explicit.
    if(b.rep_movsb)b.rep_movsb(dta.es,dta.ebx,b.ds_selector,edx,0x2Bu);
}

u32 function_339a2(Machine& m,Runtime96Boundary& b,u32 path,u32 attributes,u32 dta) noexcept {
    if(b.set_dta)b.set_dta(b.ds_selector,dta);
    const Runtime96DosResult r=b.find_first?
        b.find_first(b.ds_selector,path,static_cast<u16>(attributes)):Runtime96DosResult{};
    const u32 status=app96_status(b,r);
    function_339f2(m,b,dta);
    return status;
}

u32 function_339cf(Machine& m,Runtime96Boundary& b,u32 dta) noexcept {
    if(b.set_dta)b.set_dta(b.ds_selector,dta);
    function_33a15(m,b,dta);
    const Runtime96DosResult r=b.find_next?b.find_next():Runtime96DosResult{};
    const u32 status=app96_status(b,r);
    function_339f2(m,b,dta);
    return status;
}

u32 function_339ef() noexcept { return 0u; }
static_assert(0x33A38u-0x339A2u==150u);
