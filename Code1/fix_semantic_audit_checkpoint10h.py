from pathlib import Path
import re

p=Path('Code1/Code1.cpp')
s=p.read_text()

old_comment='// C79F..CBD0 contains position-dependent self-modifying VGA blitters and remains uncounted pending a dedicated instruction-level model.'
new_comment='// C79F is alignment. C7A0..CBD0 is the counted self-modifying scaled-VGA blitter family modeled explicitly below in Batch 18.'
assert old_comment in s
s=s.replace(old_comment,new_comment,1)

scaled5=r'''static void b18_scaled5(Machine&m,LowGraphics13Boundary&b,u32 num,u32 den,u32 image,u32 a,u32 c,bool translate)noexcept{
 const PlanarAddress13 p=function_aea8(m,a,c);u32 dst_base=p.ebx;u8 draw_mask=b18_rol(0x11u,p.ecx),outer_mask=0x11u;u32 src_base=image+9u;const u16 h=lg13r8(b,image+6u),h2=static_cast<u16>(h*2u),outer_total=static_cast<u16>(h2*4u);const u16 h8p1=static_cast<u16>(outer_total+1u),h8m1=static_cast<u16>(outer_total-1u);const u32 src_frac=(static_cast<u32>(h2)<<8)|0x78u;const u32 scale_step=(static_cast<u32>(static_cast<u16>(num))<<8)|0x78u;const u32 inner_total=lg13r16(b,image+2u);const u8 key=lg13r8(b,image+8u);u32 outer_acc=den>>1,outer_count=outer_total;
 do{const u32 outer_src=src_base;outer_acc+=scale_step;while(outer_acc>=den){outer_acc-=den;u32 src=outer_src,dst=dst_base,inner_acc=den>>1,inner_count=inner_total;b18_mask(b,draw_mask);do{inner_acc+=scale_step;if(inner_acc>=den){u8 v=lg13r8(b,src);if(translate)v=*dgroup_ptr(m,dgroup(0x2f88u+v));do{if(v!=key)lg13w8(b,dst,v);dst+=0x50u;inner_acc-=den;}while(inner_acc>=den);}src+=h8p1;}while(--inner_count!=0u);const bool carry=(draw_mask&0x80u)!=0u;draw_mask=b18_rol1(draw_mask);if(carry)++dst_base;}src_base=outer_src+src_frac;const bool carry=(outer_mask&0x80u)!=0u;outer_mask=b18_rol1(outer_mask);if(carry)src_base-=h8m1;}while(--outer_count!=0u);
 if(b.out16)b.out16(0x3c4u,0xff02u);
}
'''
pat5=re.compile(r'static void b18_scaled5\(.*?\n\}\n(?=void function_c7a0)',re.S)
s,n=pat5.subn(scaled5,s,1)
assert n==1,n

scaled7=r'''static void b18_scaled7(Machine&m,LowGraphics13Boundary&b,u32 outer_num,u32 outer_den,u32 inner_num,u32 inner_den,u32 image,u32 a,u32 c,bool translate)noexcept{
 const PlanarAddress13 p=function_aea8(m,a,c);u32 dst_base=p.ebx;u8 draw_mask=b18_rol(0x11u,p.ecx),outer_mask=0x11u;const u8 h=lg13r8(b,image+6u);const u16 h2=static_cast<u16>(static_cast<u16>(h)*2u),outer_total=static_cast<u16>(h2*4u);const u16 frame=lg13r16(b,image),frame_m1=static_cast<u16>(frame-1u),plane_step=static_cast<u16>(frame>>2);const u32 inner_total=lg13r16(b,image+4u);const u8 key=lg13r8(b,image+8u);u32 src_base=image+9u,outer_acc=outer_den>>1,outer_count=outer_total;
 do{const u32 outer_src=src_base;outer_acc+=outer_num;while(outer_acc>=outer_den){outer_acc-=outer_den;u32 src=outer_src,dst=dst_base,inner_acc=inner_den>>1,inner_count=inner_total;b18_mask(b,draw_mask);do{inner_acc+=inner_num;if(inner_acc>=inner_den){const u8 raw=lg13r8(b,src);const u8 v=translate?*dgroup_ptr(m,dgroup(0x2f88u+raw)):raw;do{if(translate||v!=key)lg13w8(b,dst,v);dst+=0x50u;inner_acc-=inner_den;}while(inner_acc>=inner_den);}src+=h2;}while(--inner_count!=0u);const bool carry=(draw_mask&0x80u)!=0u;draw_mask=b18_rol1(draw_mask);if(carry)++dst_base;}src_base=outer_src+plane_step;const bool carry=(outer_mask&0x80u)!=0u;outer_mask=b18_rol1(outer_mask);if(carry)src_base-=frame_m1;}while(--outer_count!=0u);
 if(b.out16)b.out16(0x3c4u,0xff02u);
}
'''
pat7=re.compile(r'static void b18_scaled7\(.*?\n\}\n(?=void function_c9d4)',re.S)
s,n=pat7.subn(scaled7,s,1)
assert n==1,n

old='r.esi+=2u;while(b.read8){u8 c=b.read8(env,r.esi++);'
new='r.esi+=3u;while(b.read8){u8 c=b.read8(env,r.esi++);'
assert s.count(old)==1,s.count(old)
s=s.replace(old,new,1)
p.write_text(s)

a=Path('Code1/FINAL_BINARY_AUDIT.md')
audit=a.read_text()
heading='## Semantic audit checkpoint 10h — `C7A0..CEBF`'
assert heading not in audit
audit += '''\n\n## Semantic audit checkpoint 10h — `C7A0..CEBF`\n\nFresh direct disassembly of the immutable original `CODE.1` reconciled all four position-dependent self-modifying scaled VGA blitters, the translation-table copy/RET callback, termination chain, startup recovery prelude, PSP/environment materializer, and startup exit path through `CEBF`. The blitter patch model itself was confirmed: `C7A0/C8C4` rewrite the low 16 bits of their scale immediates and sequencer-mask byte in place, while `C9D4/CACC` patch the separate outer/inner scaling immediates, plane stride, frame-minus-one, key byte, and map-mask fields exactly as represented by the C++ locals. Translated `C7A0` compares the XLAT result with the source key, `C8C4/C9D4` compare the raw byte, and translated `CACC` writes the table result unconditionally, matching the four encoded variants.\n\nThe previous shared helpers did, however, normalize the patched ECX loops into pre-checked C++ `for` loops. The machine code enters both outer and inner bodies before `DEC ECX/JNE`; a zero 16-bit patched count therefore executes once and then wraps through the full 32-bit ECX cycle rather than becoming a zero-iteration loop. Both shared models now use explicit entry-before-decrement `do/while` counters while preserving all 32-bit accumulator wrap and unsigned `JB/JAE` scale tests. `CBD4`, `CBEF`, and `CBF0..CC18` were rechecked and required no semantic correction; the classified `CC1E..CC95` Watcom copyright/string block remains data reached over by the two-byte `CC1C` jump.\n\nThe startup continuation `CC96..CEBF` was also rechecked directly. `CD97` had regressed on the environment trailer: once the double-NUL is found, the binary executes `LODSB` plus two `INC ESI` instructions, advancing three bytes from the second NUL before copying the executable path. The C++ now restores that exact `ESI += 3` transition rather than `+2`. The PSP command-tail `REPZ SCASB`, zero-fill cap, FCE8/FC98 calls, and CE8B/CE8E shared termination behavior otherwise reconcile without further changes. Object accounting remains **69,361 translated instructions + 1,041 proven non-code/data = 70,402 / 70,402**, with **0 unresolved bytes**. The sequential semantic frontier is now `CEBF`; the next code begins at `CEC0`.\n'''
a.write_text(audit)
