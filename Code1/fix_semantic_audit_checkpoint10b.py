from pathlib import Path

cpp_path=Path('Code1/Code1.cpp')
s=cpp_path.read_text()

helper_marker='static void b17_put(LowGraphics13Boundary&b,u32 addr,u8 value)noexcept{(void)lg13r8(b,addr);lg13w8(b,addr,value);}\n'
assert helper_marker in s
helpers=helper_marker+'''static bool b17_neg32(u32 v)noexcept{return (v&0x80000000u)!=0u;}\nstatic bool b17_sgt32(u32 a,u32 b)noexcept{const bool sa=b17_neg32(a),sb=b17_neg32(b);return sa==sb?a>b:(!sa&&sb);}\nstatic void b17_step_plane4(LowGraphics13Boundary&b,u8&mask,u32&addr)noexcept{mask=static_cast<u8>((static_cast<u32>(mask)<<1)&0x0fu);if(mask==0u){mask=1u;++addr;}b17_mask(b,mask);}\n'''
if 'static bool b17_neg32' not in s:
    s=s.replace(helper_marker,helpers,1)

start=s.index('void function_a05c(')
assert_start=s.index('static_assert(0xA21Au-0xA05Cu==446u);',start)
end=assert_start+len('static_assert(0xA21Au-0xA05Cu==446u);')
new=r'''void function_a05c(Machine&m,LowGraphics13Boundary&b,s32 a0,s32 b0,s32 a1,s32 b1,u32 color)noexcept{
 if(b.out16)b.out16(0x3ceu,static_cast<u16>(0x0003u|(static_cast<u16>(*dgroup_ptr(m,dgroup(0x2b28u)))<<8)));
 u32 r0=static_cast<u32>(a0),c0=static_cast<u32>(b0),r1=static_cast<u32>(a1),c1=static_cast<u32>(b1);
 u32 pitch=load_le32(dgroup_ptr(m,dgroup(0x2b41u)));const u8 pixel=static_cast<u8>(color);
 u32 dc=c1-c0;
 if(dc==0u){
  u32 dr=r1-r0;if(b17_neg32(dr)){dr=0u-dr;r0=r1;}
  u32 count=dr+1u;const PlanarAddress13 p=function_aea8(m,r0,c0);u32 addr=p.ebx;const u8 mask=static_cast<u8>(1u<<(p.ecx&3u));b17_mask(b,mask);
  do{b17_put(b,addr,pixel);addr+=pitch;}while(--count!=0u);
  if(b.out16)b.out16(0x3ceu,0x0003u);return;
 }
 if(b17_neg32(dc)){dc=0u-dc;u32 t=c0;c0=c1;c1=t;t=r0;r0=r1;r1=t;}
 u32 dr=r1-r0;
 if(dr==0u){
  const PlanarAddress13 p=function_aea8(m,r0,c0);u32 addr=p.ebx;u8 left=static_cast<u8>(0x0fu<<(p.ecx&3u));
  const u32 end_shift=(c1&3u)^3u;u8 right=static_cast<u8>(~sar8_exact_9eb0(0xf0u,end_shift));u32 bytes=(c1>>2)-(c0>>2);
  if((left&0x80u)==0u){
   if(bytes==0u)right=static_cast<u8>(right&left);
   else{b17_mask(b,left);b17_put(b,addr,pixel);++addr;--bytes;if(bytes!=0u){b17_mask(b,0xffu);do{b17_put(b,addr,pixel);++addr;}while(--bytes!=0u);}}
  }else{b17_mask(b,0xffu);do{b17_put(b,addr,pixel);++addr;}while(--bytes!=0u);}
  b17_mask(b,right);b17_put(b,addr,pixel);
  if(b.out16)b.out16(0x3ceu,0x0003u);return;
 }
 if(b17_neg32(dr)){dr=0u-dr;pitch=0u-pitch;}
 const bool row_major=b17_sgt32(dr,dc);u32 major=row_major?dr:dc,minor=row_major?dc:dr;
 const u32 twice=minor<<1;u32 err=twice-major;const u32 diagonal=err-major;u32 count=major+1u;
 const PlanarAddress13 p=function_aea8(m,r0,c0);u32 addr=p.ebx;u8 mask=static_cast<u8>(1u<<(p.ecx&3u));b17_mask(b,mask);
 const u32 scratch_hi=m.cpu.eax&0xffff0000u;store_le32(dgroup_ptr(m,dgroup(0x2b51u)),scratch_hi|(static_cast<u32>(mask)<<8)|2u);
 do{
  b17_put(b,addr,pixel);
  if(row_major){
   addr+=pitch;
   if(b17_neg32(err))err+=twice;
   else{err+=diagonal;b17_step_plane4(b,mask,addr);store_le32(dgroup_ptr(m,dgroup(0x2b51u)),scratch_hi|(static_cast<u32>(mask)<<8)|2u);}
  }else{
   b17_step_plane4(b,mask,addr);store_le32(dgroup_ptr(m,dgroup(0x2b51u)),scratch_hi|(static_cast<u32>(mask)<<8)|2u);
   if(b17_neg32(err))err+=twice;else{err+=diagonal;addr+=pitch;}
  }
 }while(--count!=0u);
 if(b.out16)b.out16(0x3ceu,0x0003u);
}
static_assert(0xA21Au-0xA05Cu==446u);'''
s=s[:start]+new+s[end:]
cpp_path.write_text(s)

audit_path=Path('Code1/FINAL_BINARY_AUDIT.md')
audit=audit_path.read_text()
section='''\n\n## Semantic audit checkpoint 10b — `A05C..A219`\n\nFresh direct disassembly of the immutable original `CODE.1` reconciled the complete planar line rasterizer. The prior lift rotated the sequencer plane mask through eight bits; the encoded loop is four-plane VGA progression (`1,2,4,8,1`) and advances the byte address on the `8 -> 1` wrap. `A05C` now preserves that four-plane step in both Bresenham branches and models the horizontal-span path directly with the encoded left/full/right map masks rather than expanding it into per-pixel writes.\n\nThe coordinate differences, negations, error accumulator, diagonal increment, and loop counters are now carried as modulo-32-bit values with explicit sign-bit tests, matching x86 `SUB/NEG/JNS/JLE/DEC/JNE` behavior without C++ signed-overflow undefined behavior. The `2B51` scratch map-mask word used by the encoded `XCHG` sequence is also preserved during general-line rasterization. Together with checkpoint 10a, the sequential semantic audit is now complete through `A219`; `A21A..A21B` is alignment. Object accounting remains **69,361 translated instructions + 1,041 proven non-code/data = 70,402 / 70,402**, with **0 unresolved bytes**.\n'''
if '## Semantic audit checkpoint 10b — `A05C..A219`' not in audit:
    audit+=section
audit_path.write_text(audit)
print('checkpoint 10b patch applied')
