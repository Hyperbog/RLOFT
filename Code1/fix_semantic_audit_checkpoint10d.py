from pathlib import Path
import re

p=Path('Code1/Code1.cpp')
s=p.read_text()

# A6F0/A7E8: the two routines share layout but not exact byte-loop semantics.
pat=re.compile(r"static u8 rol8_1\(u8 v\)noexcept\{.*?static_assert\(0xA8F4u-0xA7E8u==268u\);",re.S)
new=r'''// A6F0..A7E7: exact four-plane sprite copy.  +4 is the 16-bit row counter;
// +6 is the byte field used to derive each scanline transfer width.
void function_a6f0(Machine&m,LowGraphics13Boundary&b,u32 frame,u32 y,u32 x,u32 image)noexcept{
 const PlanarAddress13 p=function_aea8(m,y,x);u32 dst0=p.ebx;u8 mask=static_cast<u8>(0x11u<<p.ecx);
 const u16 rows=lg13r16(b,image+4u);const u8 half=lg13r8(b,image+6u),flags=lg13r8(b,image+7u);u32 src=image+8u+static_cast<u32>(lg13r16(b,image))*frame;
 const u32 doubled=static_cast<u32>(half)<<1,skip=load_le32(dgroup_ptr(m,dgroup(0x2b41u)))-doubled;
 if((flags&0x80u)==0u){
  for(u32 plane=0;plane<4u;++plane){b17_mask(b,mask);u32 dst=dst0,rc=rows;do{u32 wc=half;while(wc--!=0u){const u16 v=lg13r16(b,src);lg13w16(b,dst,v);src+=2u;dst+=2u;}dst+=skip;}while(--rc!=0u);if(plane!=3u){const bool carry=(mask&0x80u)!=0u;mask=b17_rol1(mask);if(carry)++dst0;}}
 }else{
  const u8 key=lg13r8(b,src++);store_le32(dgroup_ptr(m,dgroup(0x2b51u)),rows);const u32 bytes=static_cast<u8>(doubled);
  for(u32 plane=0;plane<4u;++plane){b17_mask(b,mask);u32 dst=dst0;u16 rc=rows;do{u32 n=bytes;do{const u8 v=lg13r8(b,src++);if(v!=key)lg13w8(b,dst,v);++dst;}while(--n!=0u);dst+=skip;}while(--rc!=0u);if(plane!=3u){const bool carry=(mask&0x80u)!=0u;mask=b17_rol1(mask);if(carry)++dst0;}}
 }
}
static_assert(0xA7E8u-0xA6F0u==248u);
// A7E8..A8F3: subtracting variant.  The nontransparent byte loop doubles the
// low byte of the already-doubled +6 field; the transparent path does not.
void function_a7e8(Machine&m,LowGraphics13Boundary&b,u32 frame,u32 y,u32 x,u32 image,u32 delta)noexcept{
 const PlanarAddress13 p=function_aea8(m,y,x);u32 dst0=p.ebx;u8 mask=static_cast<u8>(0x11u<<p.ecx);const u8 sub=static_cast<u8>(delta);
 const u16 rows=lg13r16(b,image+4u);const u8 half=lg13r8(b,image+6u),flags=lg13r8(b,image+7u);u32 src=image+8u+static_cast<u32>(lg13r16(b,image))*frame;
 const u32 doubled=static_cast<u32>(half)<<1,skip=load_le32(dgroup_ptr(m,dgroup(0x2b41u)))-doubled;
 if((flags&0x80u)==0u){
  const u32 bytes=static_cast<u32>(static_cast<u8>(doubled))<<1;
  for(u32 plane=0;plane<4u;++plane){b17_mask(b,mask);u32 dst=dst0,rc=rows;do{u32 n=bytes;do{lg13w8(b,dst++,static_cast<u8>(lg13r8(b,src++)-sub));}while(--n!=0u);dst+=skip;}while(--rc!=0u);if(plane!=3u){const bool carry=(mask&0x80u)!=0u;mask=b17_rol1(mask);if(carry)++dst0;}}
 }else{
  const u8 key=lg13r8(b,src++);store_le32(dgroup_ptr(m,dgroup(0x2b51u)),rows);const u32 bytes=static_cast<u8>(doubled);
  for(u32 plane=0;plane<4u;++plane){b17_mask(b,mask);u32 dst=dst0,rc=rows;do{u32 n=bytes;do{const u8 v=lg13r8(b,src++);if(v!=key)lg13w8(b,dst,static_cast<u8>(v-sub));++dst;}while(--n!=0u);dst+=skip;}while(--rc!=0u);if(plane!=3u){const bool carry=(mask&0x80u)!=0u;mask=b17_rol1(mask);if(carry)++dst0;}}
 }
}
static_assert(0xA8F4u-0xA7E8u==268u);'''
s,n=pat.subn(new,s,1);assert n==1,n

# A8F4: table translating renderer has its own exact byte counts and 16-bit transparent row loop.
pat=re.compile(r"static void b17_table_sprite\(.*?static_assert\(0xAA20u-0xA8F4u==300u\);",re.S)
new=r'''// A8F4..AA1F: table-translating sprite renderer; the table address is the
// semantic form of the two self-modified XLAT displacement immediates.
void function_a8f4(Machine&m,LowGraphics13Boundary&b,u32 frame,u32 y,u32 x,u32 image,u32 table)noexcept{
 const PlanarAddress13 p=function_aea8(m,y,x);u32 dst0=p.ebx;u8 mask=static_cast<u8>(0x11u<<(p.ecx&3u));
 const u16 rows=lg13r16(b,image+4u);const u8 half=lg13r8(b,image+6u),flags=lg13r8(b,image+7u);u32 src=image+8u+static_cast<u32>(lg13r16(b,image))*frame;
 const u32 doubled=static_cast<u32>(half)<<1,skip=load_le32(dgroup_ptr(m,dgroup(0x2b41u)))-doubled;
 if((flags&0x80u)==0u){
  const u32 bytes=static_cast<u32>(static_cast<u8>(doubled))<<1;
  for(u32 plane=0;plane<4u;++plane){b17_mask(b,mask);u32 dst=dst0,rc=rows;do{u32 n=bytes;do{const u8 v=lg13r8(b,src++);lg13w8(b,dst++,lg13r8(b,table+v));}while(--n!=0u);dst+=skip;}while(--rc!=0u);if(plane!=3u){const bool carry=(mask&0x80u)!=0u;mask=b17_rol1(mask);if(carry)++dst0;}}
 }else{
  const u8 key=lg13r8(b,src++);store_le32(dgroup_ptr(m,dgroup(0x2b51u)),rows);const u32 bytes=static_cast<u8>(doubled);
  for(u32 plane=0;plane<4u;++plane){b17_mask(b,mask);u32 dst=dst0;u16 rc=rows;do{u32 n=bytes;do{const u8 v=lg13r8(b,src++);if(v!=key)lg13w8(b,dst,lg13r8(b,table+v));++dst;}while(--n!=0u);dst+=skip;}while(--rc!=0u);if(plane!=3u){const bool carry=(mask&0x80u)!=0u;mask=b17_rol1(mask);if(carry)++dst0;}}
 }
}
static_assert(0xAA20u-0xA8F4u==300u);'''
s,n=pat.subn(new,s,1);assert n==1,n

# AA20/AC10: preserve their deliberately different leading-plane and row-counter behavior.
pat=re.compile(r"static void b17_clipped_sprite\(.*?static_assert\(0xAE0Bu-0xAC10u==507u\);",re.S)
new=r'''static void b17_clipped_sprite(Machine&m,LowGraphics13Boundary&b,u32 frame,u32 y,u32 x,u32 clip,u32 image,u8 delta,bool subtract)noexcept{
 auto w=[&](u32 o,u32 v){store_le32(dgroup_ptr(m,dgroup(o)),v);};auto r=[&](u32 o){return load_le32(dgroup_ptr(m,dgroup(o)));};
 w(0x2b71u,0u);w(0x2b75u,0u);w(0x2b55u,lg13r32(b,clip)>>2);w(0x2b59u,lg13r32(b,clip+4u)>>2);w(0x2b5du,lg13r32(b,clip+8u));w(0x2b61u,(lg13r32(b,clip+0x0cu)+3u)>>2);
 u32 q=lg13r32(b,clip+0x10u);w(0x2b6du,q&3u);if(q!=0u){w(0x2b71u,1u);q>>=2;}w(0x2b65u,q);q=(lg13r32(b,clip+0x14u)+r(0x2b6du))>>2;if(q==0u){q=1u;if(r(0x2b71u)!=1u){w(0x2b75u,4u-lg13r32(b,clip+0x14u));w(0x2b61u,r(0x2b61u)-1u);}}w(0x2b69u,q);w(0x2b6du,lg13r32(b,clip+0x0cu)&3u);
 const PlanarAddress13 p=function_aea8(m,y,x);u32 dst0=p.ebx;const u32 rot=(p.ecx+4u-r(0x2b6du))&3u;u8 mask=0x11u;if(subtract)mask=static_cast<u8>(mask<<rot);else for(u32 i=0;i<rot;++i)mask=b17_rol1(mask);
 u32 src=image+8u+static_cast<u32>(lg13r16(b,image))*frame;const u8 key=(lg13r8(b,image+7u)&0x80u)?lg13r8(b,src++):0xffu;const u32 plane_stride=static_cast<u32>(lg13r16(b,image))>>2;
 u32 planes=4u,lead=r(0x2b75u);if(subtract){if(lead!=0u){src+=plane_stride;--planes;mask=b17_rol1(mask);}}else{while(lead!=0u){src+=plane_stride;--lead;--planes;mask=b17_rol1(mask);}}
 w(0x2b79u,planes);u32 dst_skip=load_le32(dgroup_ptr(m,dgroup(0x2b41u)))-r(0x2b69u)-r(0x2b65u);b17_mask(b,mask);store_le32(dgroup_ptr(m,dgroup(0x2b51u)),r(0x2b5du));
 for(;;){
  u32 dst=dst0;u32 rows=subtract?static_cast<u16>(r(0x2b5du)):r(0x2b5du);
  do{src+=r(0x2b55u);src+=r(0x2b61u);u32 n=r(0x2b69u);do{const u8 v=lg13r8(b,src++);if(v!=key)lg13w8(b,dst,subtract?static_cast<u8>(v-delta):v);++dst;}while(--n!=0u);const u32 tail=r(0x2b65u);dst+=tail;src+=tail;dst+=dst_skip;}while(--rows!=0u);
  src+=r(0x2b59u);--planes;w(0x2b79u,planes);if(planes==0u)break;const bool carry=(mask&0x80u)!=0u;mask=b17_rol1(mask);
  if(carry){++dst0;if(r(0x2b61u)!=0u){w(0x2b61u,r(0x2b61u)-1u);--dst_skip;--dst0;if(r(0x2b71u)!=0u)w(0x2b65u,r(0x2b65u)+1u);else w(0x2b69u,r(0x2b69u)+1u);}else if(r(0x2b71u)!=0u){w(0x2b65u,r(0x2b65u)+1u);w(0x2b69u,r(0x2b69u)-1u);if(r(0x2b69u)==0u)break;}}
  b17_mask(b,mask);
 }
}
void function_aa20(Machine&m,LowGraphics13Boundary&b,u32 frame,u32 y,u32 x,u32 clip,u32 image)noexcept{b17_clipped_sprite(m,b,frame,y,x,clip,image,0u,false);}static_assert(0xAC0Fu-0xAA20u==495u);
void function_ac10(Machine&m,LowGraphics13Boundary&b,u32 frame,u32 y,u32 x,u32 clip,u32 image,u32 delta)noexcept{b17_clipped_sprite(m,b,frame,y,x,clip,image,static_cast<u8>(delta),true);}static_assert(0xAE0Bu-0xAC10u==507u);'''
s,n=pat.subn(new,s,1);assert n==1,n

# AE0C/AE5C use entry-before-DEC/JNE loops: count==0 wraps rather than skipping.
pat=re.compile(r"void function_ae0c\(.*?static_assert\(0xAE5Au-0xAE0Cu==78u\);",re.S)
new=r'''// AE0C..AE59: translate one frame through a 256-byte XLAT table; transparent key byte is preserved when flag 80h is set.
void function_ae0c(LowGraphics13Boundary&b,u32 frame,u32 table,u32 image)noexcept{u32 count=lg13r16(b,image);u32 p=image+8u+static_cast<u32>(lg13r16(b,image))*frame;if((lg13r8(b,image+7u)&0x80u)!=0u){const u8 key=lg13r8(b,p++);do{const u8 v=lg13r8(b,p);if(v!=key)lg13w8(b,p,lg13r8(b,table+v));++p;}while(--count!=0u);}else do{const u8 v=lg13r8(b,p);lg13w8(b,p,lg13r8(b,table+v));++p;}while(--count!=0u);}static_assert(0xAE5Au-0xAE0Cu==78u);'''
s,n=pat.subn(new,s,1);assert n==1,n
pat=re.compile(r"void function_ae5c\(.*?static_assert\(0xAEA6u-0xAE5Cu==74u\);",re.S)
new=r'''// AE5C..AEA5: register-input variant; ESI=image and EDX=frame are live on entry, BL is the subtraction delta.
void function_ae5c(LowGraphics13Boundary&b,u32 esi_image,u32 edx_frame,u8 delta)noexcept{u32 count=lg13r16(b,esi_image);u32 p=esi_image+8u+static_cast<u32>(lg13r16(b,esi_image))*edx_frame;if((lg13r8(b,esi_image+7u)&0x80u)!=0u){const u8 key=lg13r8(b,p++);do{const u8 v=lg13r8(b,p);if(v!=key)lg13w8(b,p,static_cast<u8>(v-delta));++p;}while(--count!=0u);}else do{lg13w8(b,p,static_cast<u8>(lg13r8(b,p)-delta));++p;}while(--count!=0u);}static_assert(0xAEA6u-0xAE5Cu==74u);'''
s,n=pat.subn(new,s,1);assert n==1,n

p.write_text(s)

a=Path('Code1/FINAL_BINARY_AUDIT.md')
t=a.read_text()
section='''\n\n## Semantic audit checkpoint 10d — `A6F0..AEA5`\n\nFresh direct disassembly of the immutable original `CODE.1` reconciled the three planar sprite renderers, both clipped sprite renderers, and the two in-place image translation helpers. The prior shared sprite abstraction incorrectly treated word `image+4` and byte `image+6` as one normalized width/row layout. `A6F0`, `A7E8`, and `A8F4` now preserve their distinct encoded transfer counts: `A6F0` uses `REP MOVSW` with the original byte field in the opaque path, while the subtracting and XLAT paths derive byte-loop counts from the low byte of the already-doubled field; their transparent paths retain the separate byte count. The original 16- versus 32-bit row counters and entry-before-`DEC/JNE` behavior are also explicit, including the `2B51` scratch write used by transparent paths.\n\n`AA20` and `AC10` no longer share falsely identical clipping control flow. `AA20` loops over all requested leading clipped planes, whereas `AC10` performs only one leading-plane skip when the count is nonzero; `AC10` also truncates the per-plane row counter through `DX` while `AA20` uses full `EDX`. Both routines now preserve the entry-before-decrement row/pixel loops and the encoded `2B51/2B55..2B79` scratch state. `AE0C` and `AE5C` likewise use their encoded entry-before-`DEC/JNE` frame loops rather than a high-level zero-prechecked loop. Object accounting remains **69,361 translated instructions + 1,041 proven non-code/data = 70,402 / 70,402**, with **0 unresolved bytes**. The sequential semantic frontier is now `AEA5`; `AEA6..AEA7` is alignment and the next code begins at `AEA8`.\n'''
if '## Semantic audit checkpoint 10d' not in t:t+=section
a.write_text(t)
