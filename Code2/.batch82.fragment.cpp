// Batch 82: 32620..3263A register-call adapter.
struct Application82Boundary {
    u32 (*invoke_register_callback)(u32,u32,u32,u32)=nullptr;
};

// The target is the first stack argument. Before the indirect call the binary
// loads ESI from 255E4, EDI from arg2, and ECX from arg3; EAX is the callback result.
[[nodiscard]] u32 function_32620(Machine& m,Application82Boundary& b,u32 target,u32 edi_arg,u32 ecx_arg) noexcept {
    const u32 esi_arg=load_le32(dg(m,0x255E4u));
    return b.invoke_register_callback?b.invoke_register_callback(target,esi_arg,edi_arg,ecx_arg):0u;
}
static_assert(0x3263Bu-0x32620u==27u);
// 3263B is an unreferenced NOP alignment byte.
