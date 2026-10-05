// Batch 113: 3562F..35674 shared termination/console-output/DS-restore runtime family.
struct Runtime113Boundary {
    void (*int21)(Machine&)=nullptr;
    void (*call_3962d)(Machine&)=nullptr;
    // 3566C reads the saved DS word through CS, not DGROUP.
    u16 (*read_segment16)(u16,u32)=nullptr;
};

static void app113_set_lo16(u32& r,u16 v) noexcept {
    r=(r&0xFFFF0000u)|static_cast<u32>(v);
}
static void app113_set_hi8(u32& r,u8 v) noexcept {
    r=(r&0xFFFF00FFu)|(static_cast<u32>(v)<<8);
}

// 35655..3566B: common process-termination tail. The PUSH/POP around 3962D
// preserves the full incoming EAX; AH=4Ch then supplies the DOS terminate opcode
// while AL remains the encoded exit status byte.
static void app113_terminate(Machine& m,Runtime113Boundary& b,u32 status) noexcept {
    const u32 saved_eax=status;
    m.cpu.eax=0u;
    m.cpu.edx=0x000000FFu;
    if(b.call_3962d)b.call_3962d(m);
    m.cpu.eax=saved_eax;
    app113_set_hi8(m.cpu.eax,0x4Cu);
    if(b.int21)b.int21(m);
    // 3566A MOV EAX,EAX is an intentional two-byte no-op before the next entry.
}

// 3562F..35631: POP EAX; JMP 35655. `popped_eax` is the dword consumed by
// the address-distinct stack-sharing entry (352E9 also jumps here).
void function_3562f(Machine& m,Runtime113Boundary& b,u32 popped_eax) noexcept {
    app113_terminate(m,b,popped_eax);
}

// 35632..3566B: open "con" for writing, emit the NUL-terminated message,
// then terminate with the second incoming dword as the exit status.
void function_35632(Machine& m,Runtime113Boundary& b,u32 message,u32 exit_code) noexcept {
    const u32 saved_edx=m.cpu.edx=exit_code;             // PUSH EDX
    const u32 saved_eax=m.cpu.eax=message;               // PUSH EAX
    m.cpu.edx=0x00035414u;
    app113_set_lo16(m.cpu.eax,0x3D01u);
    if(b.int21)b.int21(m);                               // INT 21h / AX=3D01h
    app113_set_lo16(m.cpu.ebx,static_cast<u16>(m.cpu.eax));

    m.cpu.edx=saved_eax;                                 // POP EDX: message pointer
    m.cpu.esi=m.cpu.edx;
    u8 al=0u;
    do {
        al=*dg(m,m.cpu.esi++);                           // CLD / LODSB
    } while(al!=0u);
    m.cpu.eax=(m.cpu.eax&0xFFFFFF00u)|static_cast<u32>(al);
    m.cpu.ecx=m.cpu.esi-m.cpu.edx;
    --m.cpu.ecx;
    app113_set_hi8(m.cpu.eax,0x40u);
    if(b.int21)b.int21(m);                               // INT 21h / AH=40h

    m.cpu.eax=saved_edx;                                 // POP EAX: exit status
    app113_terminate(m,b,m.cpu.eax);
}

// 3566C..35674: restore DS from the word that Batch 112 stored through ES at
// code offset 35675. The selector-qualified read is explicit because CS need
// not equal DGROUP.
void function_3566c(Machine& m,Runtime113Boundary& b) noexcept {
    const u16 saved=b.read_segment16?b.read_segment16(m.cpu.cs,0x00035675u):0u;
    m.cpu.ds=saved;
}

static_assert(0x35675u-0x3562Fu==70u);
// 35675..35676 is the runtime saved-DS word; 35677..3567F is zero alignment.
static_assert(0x35680u-0x35675u==11u);
