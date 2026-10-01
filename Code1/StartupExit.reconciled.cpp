/* CODE.1 startup reconciliation supplement.
 * Authority: CODE.1 SHA-256 fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed
 * This file records exact semantics discovered while hardening StartupNext.staging.cpp.
 * It is NOT separately countable coverage; all addresses overlap that staging range.
 */
#include <cstdint>
namespace code1_startup_reconciled {
using u8=std::uint8_t; using u16=std::uint16_t; using u32=std::uint32_t;

struct DosRegs { u32 eax=0,ebx=0,ecx=0,edx=0; u16 ds=0,es=0; };
struct Boundary {
 u8 (*read8)(u16,u32)=nullptr;
 void (*int21)(DosRegs&)=nullptr;
 void (*runtime_call)(u32)=nullptr;
};

/* CE11..CE1F.
 * At CE11 ESI points at the second NUL terminating the environment block.
 * CE16 LODSB consumes that NUL, then CE17 and CE18 each INC ESI. Therefore
 * the executable-path source begins exactly THREE bytes beyond the second NUL,
 * not two. CE19 tests the source byte before CE1C MOVSB; MOVSB still copies the
 * terminating NUL because JNE consumes the flags produced by the preceding CMP.
 */
void copy_environment_executable_tail(Boundary& b,u16 env_selector,u32& esi,u8* dgroup,u32& edi) noexcept {
 esi += 3u;
 if(!b.read8||!dgroup)return;
 for(;;){
  const u8 tested=b.read8(env_selector,esi);
  dgroup[edi++]=b.read8(env_selector,esi++); // CE1C MOVSB executes even when tested==0.
  if(tested==0)break;
 }
}

/* CBF0..CC18 is the common termination trampoline used by FCE2, D07C, 97E0,
 * 5085 and 50A4. Each caller has already PUSHed its exit value before JMP CBF0.
 * CBF0 establishes EBP, runs callbacks through [3088]/[308C], then pushes
 * [EBP+4] (the original exit value), creates a second frame, runs [308C]/[3090],
 * pushes that same exit value again, and JMPs CE8B. Consequently CE8B's first
 * POP EAX retrieves the exit value exactly; it is not a conventional call.
 */
struct ExitTrace { u32 exit_value=0; bool terminated=false; };
[[nodiscard]] ExitTrace function_ce8b(Boundary& b,u32 stacked_exit,u16 ds,u16 es) noexcept {
 DosRegs r{}; r.eax=stacked_exit; r.ds=ds; r.es=es;
 /* CEB3 PUSH EAX / CEB4 CALL FD2B / CEB9 POP EAX: FD2B may clobber registers,
  * but EAX is restored from the stack before AH is changed to 4Ch. */
 if(b.runtime_call)b.runtime_call(0xfd2b);
 r.eax=stacked_exit;
 r.eax=(r.eax&0xffff00ffu)|0x00004c00u; // CEBA MOV AH,4Ch; AL remains exit status.
 if(b.int21)b.int21(r);                 // CEBC INT 21h; expected not to return.
 return {stacked_exit,true};
}

/* CE8E alternate fatal entry.
 * Stack on entry: [ESP]=message pointer, [ESP+4]=exit value. This is proved by
 * 10C4D: PUSH 1; PUSH 390h; JMP CE8E. 10479 reaches the same entry through the
 * stack-check trampoline with EAX=55B2h and EDX=1 prepared by that runtime path.
 * CE8E pops message into EAX and exit value into EDX, saves both, opens "con"
 * (CC92) for write with AX=3D01h, places the returned handle in BX, scans the
 * message through its NUL, writes exactly strlen(message) bytes with AH=40h,
 * restores the saved exit value into EAX, then falls into CE8B's FD2B/4Ch tail.
 */
[[nodiscard]] ExitTrace function_ce8e(Boundary& b,u32 message,u32 exit_value,u16 ds,u16 es) noexcept {
 DosRegs r{}; r.ds=ds; r.es=es;
 r.eax=message;                         // CE8E POP EAX
 r.edx=exit_value;                      // CE8F POP EDX
 r.edx=0x0000cc92u;                     // CE92 MOV EDX,CC92
 r.eax=(r.eax&0xffff0000u)|0x3d01u;     // CE97 MOV AX,3D01h
 if(b.int21)b.int21(r);                 // CE9B open CON for writing
 const u16 handle=static_cast<u16>(r.eax);
 r.edx=message;                         // CEA0 POP EDX restores message pointer
 u32 esi=message;
 if(b.read8){while(b.read8(ds,esi++)!=0){}}
 r.ecx=esi-message-1u;                  // CEA9..CEAD computes strlen
 r.ebx=(r.ebx&0xffff0000u)|handle;      // CE9D MOV BX,AX
 /* Final LODSB read the NUL, so AL=0. CEAE MOV AH,40h gives AX=4000h. */
 r.eax=(r.eax&0xffff0000u)|0x4000u;
 if(b.int21)b.int21(r);                 // write message; DS:EDX, BX handle, CX length
 return function_ce8b(b,exit_value,ds,es);
}

/* These are overlapping reconciliation facts, not additional bytes. */
constexpr u32 kEnvironmentTailStart=0xCE11u;
constexpr u32 kCommonExitStart=0xCE8Bu;
constexpr u32 kFatalExitStart=0xCE8Eu;
constexpr u32 kCommonTerminationTrampoline=0xCBF0u;
} // namespace code1_startup_reconciled
