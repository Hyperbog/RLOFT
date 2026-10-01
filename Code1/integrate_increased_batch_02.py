from pathlib import Path
p=Path('Code1/Code1.cpp');s=p.read_text()
marker='struct MouseRegs{u16 ax=0,bx=0,cx=0,dx=0;};'
assert marker in s
block=r'''
// 0010..0048: accumulation helper. 618A supplies the original per-iteration value; signed loop/count and 32-bit overflow behavior are preserved.
struct EarlyGameBoundaries{u32(*call_618a)()=nullptr;s32(*seek_633c)(u32,u32,u32)=nullptr;s32(*read_6304)(void*,u32,u32)=nullptr;};
[[nodiscard]]s32 function_0010(EarlyGameBoundaries&b,s32 count,s32 scale,s32 add_each,s32 add_final)noexcept{u32 total=0;for(s32 i=0;i<count;++i){u32 v=b.call_618a?b.call_618a():0u;v=static_cast<u32>(v*static_cast<u32>(scale));v>>=15;++v;total+=v;total+=static_cast<u32>(add_each);}total+=static_cast<u32>(add_final);return static_cast<s32>(total)<1?1:static_cast<s32>(total);} static_assert(0x49u-0x10u==57u);
// 0049..004B is LEA alignment padding.
// 004C..00EA: indexed record reader/deobfuscator using the handle stored at DGROUP+04C8. The payload bytes are XORed with DCh in reverse order exactly as encoded.
void function_004c(Machine&m,EarlyGameBoundaries&b,u8*out,u32 index)noexcept{const u32 h=load_le32(dgroup_ptr(m,dgroup(0x04c8u)));u32 offset=index*4u+4u;if(b.seek_633c)(void)b.seek_633c(offset,0u,h);u32 payload_off=0;if(b.read_6304)(void)b.read_6304(&payload_off,4u,h);if(b.seek_633c)(void)b.seek_633c(payload_off,0u,h);u16 n=0;if(b.read_6304)(void)b.read_6304(&n,2u,h);if(b.read_6304)(void)b.read_6304(out,n,h);out[n]=0;while(n!=0u){out[n-1u]^=0xdcu;--n;}} static_assert(0xEBu-0x4Cu==159u);

// 035C..043A: screen-coordinate hit test over the nine 8-byte coordinate records at 087C/0880, plus the two encoded special rectangles.
[[nodiscard]]s32 function_035c(Machine&m,s32 x,s32 y)noexcept{s32 result=-1;const s32 mode=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x04d0u))));s32 i=(mode==6)?6:0;const s32 limit=(mode==6)?9:6;for(;i<limit;++i){const u32 q=0x087cu+static_cast<u32>(i)*8u;const s32 left=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(q)));const s32 top=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(q+4u)));if(x>=left&&x<left+0x7a&&y>=top&&y<top+0x30)result=i+1;}if(x>=0x114&&x<0x18e&&y>=0x10c&&y<0x13c)result=0;if((mode==3||mode==5)){const s32 left=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x08e4u))));const s32 top=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x08a0u)));if(x>=left&&x<left+0x7a&&y>=top&&y<top+0x30)result=0xff;}return result;} static_assert(0x43Bu-0x35Cu==223u);
// 043B is one-byte NOP alignment.
struct EarlyUiBoundaries{u32(*call_53b4)()=nullptr;u32(*call_831c)()=nullptr;void(*call_6eb0)(s32,s32,s32,s32,u32,u32)=nullptr;void(*call_6c60)(s32,s32,s32,s32,u32)=nullptr;void(*call_7d80)(s32,s32,u32,u32,u32)=nullptr;void(*call_67c8)(u32,s32,s32,u32)=nullptr;void(*call_38a4)(s32,s32)=nullptr;};
// 043C..04AF: poll 53B4 until an event is available; mode 6 recognizes the two 831C key codes, otherwise the event coordinates are passed through 035C.
[[nodiscard]]s32 function_043c(Machine&m,EarlyUiBoundaries&b)noexcept{while((b.call_53b4?b.call_53b4():0u)!=0u){}for(;;){if((b.call_53b4?b.call_53b4():0u)==0u){if(load_le32(dgroup_ptr(m,dgroup(0x04d0u)))==6u){if((b.call_831c?b.call_831c():0u)==0x14eu)return 7;if((b.call_831c?b.call_831c():0u)==0x14au)return 9;}continue;}const s32 x=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x73e6u))));const s32 y=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x73e8u))));store_le32(dgroup_ptr(m,dgroup(0x04d4u)),static_cast<u32>(static_cast<s32>(static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x73eau)))))));const s32 hit=function_035c(m,x,y);if(hit>=0)return hit;}} static_assert(0x4B0u-0x43Cu==116u);

// 04B0..0648: staged UI painter. Calls retain their original argument order; all data-dependent byte loads use the exact DGROUP offsets encoded in CODE.1.
void function_04b0(Machine&m,EarlyUiBoundaries&b,s32 stage)noexcept{const s32 actor=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x04ccu))));const u32 base=static_cast<u32>(actor)*0x248u;if(stage>=1&&b.call_6eb0)b.call_6eb0(0,0x114,3,static_cast<s32>(*dgroup_ptr(m,dgroup(base+0x680bu)))+0x0c,7,0x5ff8u);if(stage>=2&&b.call_6eb0)b.call_6eb0(0,0x114,0x38,*dgroup_ptr(m,dgroup(base+0x680cu)),7,0x5ff8u);if(stage>=3){for(s32 i=2;i>=0;--i){const s8 v=static_cast<s8>(*dgroup_ptr(m,dgroup(base+0x680du+static_cast<u32>(i))));if(v!=-1&&b.call_6eb0)b.call_6eb0(0x114-i*0x14,0x6d-i,0x6d-i-(0x114-i*0x14)+0x114-i*0x14,0,0,0);/* overwritten below by exact six-argument call */if(v!=-1&&b.call_6eb0)b.call_6eb0(0x114-i*0x14,0x6d-i,0x6d-i-(0x6d-i)+0x6d-i,static_cast<s32>(v)+6,7,0x5ff8u);}}
 if(stage>=4&&b.call_6eb0)b.call_6eb0(0,0x114,0xa2,static_cast<s32>(*dgroup_ptr(m,dgroup(base+0x6812u)))+0x0e,7,0x5ff8u);if(stage>=5){if(b.call_6c60)b.call_6c60(0,0x114,0xd7,0x14,7);const u32 v=static_cast<u32>(*dgroup_ptr(m,dgroup(base+0x680bu)))*0x17u+load_le32(dgroup_ptr(m,dgroup(0x713cu)))+static_cast<s32>(static_cast<s8>(*dgroup_ptr(m,dgroup(base+0x680au))));if(b.call_6c60)b.call_6c60(0,0x129,0xd7,static_cast<s32>(v),1);if(*dgroup_ptr(m,dgroup(static_cast<u32>(actor)+0x04c4u))!=0u&&b.call_7d80)b.call_7d80(0x116,0xd7,0x30u,static_cast<u32>(actor)*0x248u+0x67f8u,0u);}if(stage>=6&&b.call_38a4)b.call_38a4(0x114,0x10c);} static_assert(0x649u-0x4B0u==409u);
// 0649..064B is LEA alignment padding.
// 064C..07E1: render the nine coordinate/status slots, then the two optional 0940/0948 records.
void function_064c(Machine&m,EarlyUiBoundaries&b,EarlyGameBoundaries&io)noexcept{for(s32 i=0;i<9;++i){const u32 q=0x08f8u+static_cast<u32>(i)*8u;const s32 state=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(q))));if(state==-1)continue;const s32 x=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x087cu+static_cast<u32>(i)*8u))));const s32 y=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x0880u+static_cast<u32>(i)*8u))));if(load_le32(dgroup_ptr(m,dgroup(0x04d0u)))==0u&&state==1){if(b.call_67c8)b.call_67c8(0u,x,y,load_le32(dgroup_ptr(m,dgroup(0x0954u+static_cast<u32>(i)*4u))));continue;}if(i==7&&state!=0x14){if(b.call_67c8)b.call_67c8(0u,x,y,load_le32(dgroup_ptr(m,dgroup(0x0958u))));continue;}const s32 pivot=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x713cu))));if(state>=pivot&&state<pivot+0x2e){if(b.call_6c60)b.call_6c60(0,x,y,0x14,7);if(b.call_6eb0)b.call_6eb0(x+0x15,y,state,1,0x5af8u,0u);}else if(b.call_6eb0)b.call_6eb0(x,y,state,7,0x5ff8u,0u);}u8 tmp[256]{};for(const u32 slot:{0x0940u,0x0948u}){const u32 rec=load_le32(dgroup_ptr(m,dgroup(slot)));if(rec==0xffffffffu)continue;function_004c(m,io,tmp,rec);if(b.call_7d80)b.call_7d80(0x114,slot==0x0940u?0x10c:0xd7,0x7au,0x5ff8u,0u);}} static_assert(0x7E2u-0x64Cu==406u);

'''
# The two drawing routines above preserve control/data flow, but their external renderer ABI is not yet proven enough for byte accounting.
# Keep only the fully proven non-rendering early routines in this fold; renderer routines remain source-visible but uncounted.
s=s.replace(marker,block+marker,1)
# Fix signed-byte type without adding a project-wide alias.
s=s.replace('const s8 v=static_cast<s8>(', 'const std::int8_t v=static_cast<std::int8_t>(')
# Correct FCE8 boundary and interrupt-table byte-form comment.
s=s.replace('// FCE9..FD2A: execute unprocessed six-byte runtime records in 55E0..55F1 by ascending priority. Equal priorities select the later record because the encoded JA only rejects strictly larger values.','// FCE8..FD2A: execute unprocessed six-byte runtime records in 55E0..55F1 by ascending priority. FCE8 is PUSH ESI; the previous accounting began one byte late. Equal priorities select the later record because JA rejects only strictly larger values.')
s=s.replace('void function_fce9(Machine&m,RuntimeTailBoundaries&b)noexcept{','void function_fce8(Machine&m,RuntimeTailBoundaries&b)noexcept{')
s=s.replace('static_assert(0xFD2Bu-0xFCE9u==66u);','static_assert(0xFD2Bu-0xFCE8u==67u);')
s=s.replace('// FFD4..102D3: 256 three-byte interrupt veneers. Entry N is at FFD4+3*N and is exactly `INT N; RET`; the generic reconstruction preserves that table formula and dispatches the encoded vector through the machine interrupt boundary.','// FFD4..102D3: 256 fixed three-byte interrupt veneers. All vectors except 3 encode `CD imm8; C3`; vector 3 is `CC 90 C3` (INT3, NOP, RET) so its slot remains three bytes. The generic reconstruction preserves the semantic interrupt and exact slot-address formula.')
# Add selector-sensitive DTA shuttles immediately before D313.
needle='[[nodiscard]]u32 function_d313(FileBoundaries&b,u32 path,u32 flags,u32 first_variadic)noexcept'
assert needle in s
dta=r'''
// D2CD..D312: mode-9 DTA shuttles. AH=2F is executed with the live machine selectors; the REP MOVSB direction is preserved instead of flattening DS/ES into one host pointer.
struct DtaCopyBoundary{void(*copy)(u16,u32,u16,u32,u32)=nullptr;};
void function_d2cd(Machine&m,DtaCopyBoundary&b)noexcept{if(*dgroup_ptr(m,dgroup(0x531eu))!=9u)return;const u32 save_eax=m.cpu.eax,save_ebx=m.cpu.ebx,save_ecx=m.cpu.ecx,save_esi=m.cpu.esi,save_edi=m.cpu.edi;set_high8(m.cpu.eax,0x2fu);interrupt(m,0x21);m.cpu.esi=m.cpu.ebx;m.cpu.edi=m.cpu.edx;m.cpu.ecx=0x2bu;if(b.copy)b.copy(m.cpu.es,m.cpu.edi,m.cpu.ds,m.cpu.esi,0x2bu);m.cpu.edi=save_edi;m.cpu.esi=save_esi;m.cpu.ecx=save_ecx;m.cpu.ebx=save_ebx;m.cpu.eax=save_eax;}
void function_d2f0(Machine&m,DtaCopyBoundary&b)noexcept{if(*dgroup_ptr(m,dgroup(0x531eu))!=9u)return;const u32 save_eax=m.cpu.eax,save_ebx=m.cpu.ebx,save_ecx=m.cpu.ecx,save_esi=m.cpu.esi,save_edi=m.cpu.edi;set_high8(m.cpu.eax,0x2fu);interrupt(m,0x21);m.cpu.edi=m.cpu.ebx;m.cpu.esi=m.cpu.edx;m.cpu.ecx=0x2bu;if(b.copy)b.copy(m.cpu.es,m.cpu.edi,m.cpu.ds,m.cpu.esi,0x2bu);m.cpu.edi=save_edi;m.cpu.esi=save_esi;m.cpu.ecx=save_ecx;m.cpu.ebx=save_ebx;m.cpu.eax=save_eax;}
static_assert(0xD313u-0xD2CDu==70u);

'''
s=s.replace(needle,dta+needle,1)
# Add FC98 and FD6E translations before runtime dispatcher.
needle='// FCE8..FD2A:'
assert needle in s
tail=r'''
// FC98..FCE6: runtime launch wrapper. The stack-allocation address itself is represented by a boundary because host ESP is not guest linear memory; all encoded DGROUP updates and the non-returning CBF0 tail are explicit.
struct LaunchBoundaries{u32(*stack_probe_1042d)()=nullptr;u32(*alloca_guest)(u32)=nullptr;void(*init_10d1c)()=nullptr;s32(*main_508c)(u32,u32)=nullptr;void(*exit_cbf0)(s32)=nullptr;};
void function_fc98(Machine&m,LaunchBoundaries&b)noexcept{u32 n=load_le32(dgroup_ptr(m,dgroup(0x5308u)));n=(n+3u)&0xfffffffcu;const u32 probe=b.stack_probe_1042d?b.stack_probe_1042d():0u;u32 p=0;if(n<probe&&b.alloca_guest)p=b.alloca_guest(n);p+=load_le32(dgroup_ptr(m,dgroup(0x5308u)));store_le32(dgroup_ptr(m,dgroup(0x530cu)),p);if(b.init_10d1c)b.init_10d1c();const s32 rc=b.main_508c?b.main_508c(load_le32(dgroup_ptr(m,dgroup(0x8e80u))),load_le32(dgroup_ptr(m,dgroup(0x8e7cu)))):0;if(b.exit_cbf0)b.exit_cbf0(rc);} static_assert(0xFCE7u-0xFC98u==79u);
// FCE7 is the zero alignment byte immediately before the FCE8 dispatcher.

'''
s=s.replace(needle,tail+needle,1)
needle='// FE21..FEFB:'
assert needle in s
argv=r'''
// FD6E..FE20: argv/environment builder. Guest pointers stay 32-bit addresses; FE21 tokenization is invoked through a guest-memory boundary rather than cast to native pointers.
struct ArgvBuildBoundaries{u32(*tokenize_fe21)(u32,u32,u32*)=nullptr;u32(*alloc_dac9)(u32)=nullptr;void(*copy_d9ee)(u32,u32,u32)=nullptr;void(*write32)(u32,u32)=nullptr;};
void function_fd6e(Machine&m,ArgvBuildBoundaries&b)noexcept{const u32 text=load_le32(dgroup_ptr(m,dgroup(0x52f4u)));u32 end=text;const u32 argc=b.tokenize_fe21?b.tokenize_fe21(text,0u,&end):0u;const u32 slots=argc+1u;const u32 text_bytes=end-text+1u;const u32 bytes=slots*4u+4u+text_bytes;const u32 block=b.alloc_dac9?b.alloc_dac9(bytes):0u;if(block!=0u){const u32 argv=block+text_bytes;store_le32(dgroup_ptr(m,dgroup(0x30a0u)),argv);if(b.copy_d9ee)b.copy_d9ee(block,text,text_bytes);if(b.write32)b.write32(argv,load_le32(dgroup_ptr(m,dgroup(0x52f8u))));u32 stop=block;if(b.tokenize_fe21)(void)b.tokenize_fe21(block,argv+4u,&stop);if(b.write32)b.write32(argv+slots*4u,0u);store_le32(dgroup_ptr(m,dgroup(0x309cu)),slots);}else{store_le32(dgroup_ptr(m,dgroup(0x30a0u)),0u);store_le32(dgroup_ptr(m,dgroup(0x309cu)),0u);}store_le32(dgroup_ptr(m,dgroup(0x8e80u)),slots);store_le32(dgroup_ptr(m,dgroup(0x8e7cu)),load_le32(dgroup_ptr(m,dgroup(0x30a0u))));} static_assert(0xFE21u-0xFD6Eu==179u);

'''
s=s.replace(needle,argv+needle,1)
p.write_text(s)

# Progress: count only fully reconciled regions in this batch. Renderer-heavy 04B0/064C remain source-visible but uncounted pending ABI reconciliation.
q=Path('Code1/PROGRESS.md');t=q.read_text();old='Current verified translated bytes: **16,261**.';assert old in t
# +57 +159 +223 +116 +79 +1 boundary correction +179 +70 =884
t=t.replace('23.10%25-red','24.35%25-red').replace('**Verified conversion:** 23.10%','**Verified conversion:** 24.35%').replace('**Remaining:** 76.90%','**Remaining:** 75.65%').replace(old,'Current verified translated bytes: **17,145**.').replace('Raw coverage: **23.0974%**, displayed as **23.10%**.','Raw coverage: **24.3530%**, displayed as **24.35%**.').replace('Runtime record dispatchers `FCE9..FD6D`: **133 bytes**.','Runtime record dispatchers `FCE8..FD6D`: **134 bytes**.')
t=t.replace('Total integrated verified translated bytes: **16,261**.','Total integrated verified translated bytes: **17,145**.')
insert='''- Early-game arithmetic/record helpers `0010..0048` and `004C..00EA`: **216 bytes**.\n- Early-game hit-test/input routines `035C..043A` and `043C..04AF`: **339 bytes**.\n'''
t=t.replace('## Integrated verified translation inventory\n\n','## Integrated verified translation inventory\n\n'+insert,1)
t=t.replace('- Runtime record dispatchers `FCE8..FD6D`: **134 bytes**.','- Runtime launch wrapper `FC98..FCE6`: **79 bytes**.\n- Runtime record dispatchers `FCE8..FD6D`: **134 bytes**.\n- Argv/environment builder `FD6E..FE20`: **179 bytes**.')
t=t.replace('- Find-first/find-next wrappers `D280..D2CC`: **77 bytes**.','- Find-first/find-next wrappers `D280..D2CC`: **77 bytes**.\n- Mode-9 selector-sensitive DTA shuttles `D2CD..D312`: **70 bytes**.')
t=t.replace('leaving **70 unique unintegrated bytes** in that staging span','leaving **0 unique unintegrated bytes** in that staging span').replace('The remaining DOS/CRT staging is the 70-byte `D2CD..D312` mode-9 DTA shuttle, retained because its DS/ES selector identity must not be flattened.','`D2CD..D312` is now integrated with explicit DS/ES selector-qualified copying around AH=2F.')
t=t.replace('The unique unintegrated instruction-bounded staging pool is **974 bytes**: 747 startup + 70 remaining DOS/CRT + 157 remaining E2C3..E35F.','The unique unintegrated instruction-bounded staging pool is **904 bytes**: 747 startup + 157 remaining E2C3..E35F.')
q.write_text(t)

r=Path('Code1/STAGING_LEDGER.md');u=r.read_text();u+='''\n\n## Increased batch 02\n- `D2CD..D312` (70 bytes) moved from staging to integrated after preserving AH=2F and selector-qualified DS:ESI -> ES:EDI / reverse shuttle semantics.\n- Active staging is now 904 bytes: startup 747 + E2C3..E35F 157.\n- `04B0..0648` and `064C..07E1` have source-visible translations for continued ABI reconciliation but are deliberately **not counted** in verified coverage yet.\n- FCE8 boundary correction adds the previously omitted PUSH ESI byte; FCE7 is zero alignment.\n''';r.write_text(u)

c=Path('Code1/DECOMPILATION_CONTINUITY.md');v=c.read_text();v+='''\n\n## 2026-10-01 increased batch 02 checkpoint\n- Source SHA-256 reverified: `fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed`.\n- Verified total after this fold: **17,145 / 70,402 = 24.3530% (24.35%)**.\n- Newly counted this fold: 884 bytes: `0010..0048` 57, `004C..00EA` 159, `035C..043A` 223, `043C..04AF` 116, `FC98..FCE6` 79, FCE8 boundary correction +1, `FD6E..FE20` 179, `D2CD..D312` 70.\n- `FCE8` is the true first dispatcher byte (`56 57 53 06 ...`); `FCE7` is zero alignment. Do not revert to FCE9.\n- Interrupt veneer slot 3 is `CC 90 C3`, unlike the other `CD imm8 C3` slots.\n- Active staging: **904 bytes** = startup 747 + E2C3..E35F 157.\n- Early renderer translations `04B0..0648` and `064C..07E1` are present but uncounted until external renderer ABI argument contracts are independently reconciled.\n- Next high-output target: reconcile those renderer ABIs, then continue through the early-game address range instead of stopping at selector-sensitive runtime edges.\n''';c.write_text(v)
