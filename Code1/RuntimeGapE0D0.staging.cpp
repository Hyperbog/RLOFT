/* CODE.1 E0D0..E3B2 runtime continuation staging.
 * Immutable authority: CODE.1 SHA-256
 * fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed
 *
 * Direct raw-binary disassembly established 722 functional instruction bytes.
 * This file is deliberately NOT COUNTED until call contracts and the interrupt
 * trampoline are fully reconciled and folded into Code1.cpp.
 */
#include <cstdint>
namespace code1_runtime_e0d0 {
using u8=std::uint8_t; using u16=std::uint16_t; using u32=std::uint32_t;

struct Boundary {
  u32 (*alloc_71c8)(u32)=nullptr;
  void (*copy_6304)(u32 dst,u32 count,u32 src)=nullptr;
  u32 (*call_7240)(u32)=nullptr;
  void (*call_72d0)(u32)=nullptr;
  void (*call_72e0)()=nullptr;
  void (*call_746c)(u32,u32,u32,u32,u32)=nullptr;
  u8 (*read8)(u32)=nullptr; u16 (*read16)(u32)=nullptr; u32 (*read32)(u32)=nullptr;
  void (*write8)(u32,u8)=nullptr; void (*write16)(u32,u16)=nullptr; void (*write32)(u32,u32)=nullptr;
};

/* E0D0..E173 (164 bytes).
 * Source = dword[arg+5]. Allocate 0x1b + source.dword[16h], copy the fixed
 * 0x1c-byte header field-for-field, then copy source payload length-1 from
 * arg2 to destination+1Ch. The odd +5 source field is encoded and preserved. */
[[nodiscard]] u32 function_e0d0(Boundary& b,u32 arg,u32 payload) noexcept {
  const u32 src=b.read32?b.read32(arg+5u):0u;
  const u32 n=b.read32?b.read32(src+0x16u):0u;
  const u32 dst=b.alloc_71c8?b.alloc_71c8(0x1bu+n):0u;
  if(b.read16&&b.write16) for(u32 o=0;o<=0x14;o+=2)b.write16(dst+o,b.read16(src+o));
  if(b.read32&&b.write32)b.write32(dst+0x16u,b.read32(src+0x16u));
  if(b.read8&&b.write8){b.write8(dst+0x1au,b.read8(src+0x1au));b.write8(dst+0x1bu,b.read8(src+0x1bu));}
  if(b.copy_6304)b.copy_6304(dst+0x1cu,n-1u,payload);
  return dst;
}

/* E174..E186, E1F4..E206, E26C..E27E are three identical 19-byte wrappers
 * around 7240. They remain address-distinct because the binary has three
 * independently callable entries. */
[[nodiscard]] u32 function_e174(Boundary&b,u32 a)noexcept{return b.call_7240?b.call_7240(a):0;}
[[nodiscard]] u32 function_e1f4(Boundary&b,u32 a)noexcept{return b.call_7240?b.call_7240(a):0;}
[[nodiscard]] u32 function_e26c(Boundary&b,u32 a)noexcept{return b.call_7240?b.call_7240(a):0;}

/* E190..E1F0 and E208..E268 are two 97-byte constructor-like routines.
 * Each reads a 16-bit length through 6304, allocates length+2, stores the
 * length word, runs 72D0/72E0, then calls 746C with destination+2 and length.
 * Their first stack argument is not consumed by the encoded body; preserve
 * the ABI position rather than deleting it during later integration. */
[[nodiscard]] u32 function_e190(Boundary&b,u32 /*arg1*/,u32 source)noexcept{
  u16 len=0; const u32 temp=0; // guest local [EBP-8] is represented by len.
  if(b.copy_6304)b.copy_6304(temp,2u,source);
  if(b.read16)len=b.read16(temp);
  const u32 dst=b.alloc_71c8?b.alloc_71c8(u32(len)+2u):0u;
  if(b.write16)b.write16(dst,len);
  if(b.call_72d0)b.call_72d0(source); if(b.call_72e0)b.call_72e0();
  if(b.call_746c)b.call_746c(0u,0u,dst+2u,len,0u);
  return dst;
}
[[nodiscard]] u32 function_e208(Boundary&b,u32 /*arg1*/,u32 source)noexcept{return function_e190(b,0u,source);}

/* E280..E2C2 (67 bytes): install INT 24h handler E2C3. It saves the caller's
 * far callback at 5102:5106, temporarily switches DS to CS, and uses the
 * extender-specific AX=2506h/CL=24h form only when runtime mode 531E is 2..4;
 * otherwise DOS AH=25h/AL=24h is used. Return EAX is explicitly zeroed. */
struct Int24InstallImage {u32 callback_offset=0;u16 callback_selector=0;u8 runtime_mode=0;};
[[nodiscard]] u32 function_e280(const Int24InstallImage&) noexcept { return 0u; }

/* E2C3..E35F (157 bytes) is an IRET interrupt trampoline, not a normal C
 * function. Exact properties retained for integration:
 *  - saves ES,DS,EBP,EDI,ESI,EDX,ECX,EBX,EAX;
 *  - restores saved application DS through CEC0 and sets ES=DS;
 *  - records old SS:ESP at 5100:50FC and old runtime stack top 5300 at 5108;
 *  - switches SS:ESP to DS:8E68 and runtime stack bottom/top to 8A68;
 *  - forms EBX from BP:SI as (BP<<4)+SI with the encoded 16-bit ADD to BX;
 *  - selects callback selector argument from runtime mode (34h or DS);
 *  - far-calls callback 5102:5106 with four pushed dwords;
 *  - LSS restores the original SS:ESP, restores 5300, registers, and IRETs.
 * This region must be modeled as a machine/interrupt boundary; do not replace
 * it with an ordinary host callback.
 */

/* E360..E3B2 (83 bytes): decimal parser using the byte-class table at 03B4.
 * Leading class bit 02h is skipped. Optional +/- is consumed. Digits are
 * recognized by class bit 20h and accumulated in signed-width 32-bit EBX as
 * EBX*10 + byte - '0'. A remembered '-' negates the final dword. */
[[nodiscard]] u32 function_e360(Boundary&b,u32 text)noexcept{
  if(!b.read8)return 0; u32 p=text;
  auto cls=[&](u8 c)->u8{return b.read8(0x3b4u+u32(u8(c+1u)));};
  while((cls(b.read8(p))&0x02u)!=0)++p;
  const u8 sign=b.read8(p); if(sign=='+'||sign=='-')++p;
  u32 v=0; while((cls(b.read8(p))&0x20u)!=0){v=v*10u+u32(b.read8(p))-0x30u;++p;}
  if(sign=='-')v=0u-v; return v;
}

static_assert(0xE174u-0xE0D0u==164u);
static_assert(0xE187u-0xE174u==19u);
static_assert(0xE1F1u-0xE190u==97u);
static_assert(0xE207u-0xE1F4u==19u);
static_assert(0xE269u-0xE208u==97u);
static_assert(0xE27Fu-0xE26Cu==19u);
static_assert(0xE2C3u-0xE280u==67u);
static_assert(0xE360u-0xE2C3u==157u);
static_assert(0xE3B3u-0xE360u==83u);
constexpr u32 kFunctionalInstructionBytes=722u;
/* Excluded boundaries: E0C8..E0CF eight zeros; E187..E18F nine zeros;
 * E1F1..E1F3 and E269..E26B alignment LEAs; E207 single NOP; E27F zero. */
} // namespace code1_runtime_e0d0
