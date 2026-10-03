from pathlib import Path

p=Path('Code1/Code1.cpp')
s=p.read_text()

def replace_func(name,end_assert,new):
    global s
    start=s.index('void '+name+'(')
    end=s.index(end_assert,start)+len(end_assert)
    s=s[:start]+new+s[end:]

replace_func('function_a2b0','static_assert(0xA359u-0xA2B0u==169u);',r'''void function_a2b0(Machine&m,LowGraphics13Boundary&b,u32 x,u32 raw_y)noexcept{
 const u32 pitch=load_le32(dgroup_ptr(m,dgroup(0x2b41u)));
 if(static_cast<s16>(raw_y&0xffffu)>=0){
  const PlanarAddress13 s=function_aea8(m,raw_y,0u);const u32 count=x*pitch,dst=load_le32(dgroup_ptr(m,dgroup(0x2b49u)));
  batch15_vga_copy_mode(b,true);if(b.out16)b.out16(0x3c4u,0x0f02u);batch15_copy(b,dst,s.ebx,count);batch15_vga_copy_mode(b,false);
 }else{
  const u32 y=0u-raw_y;u32 src=function_aea8(m,y,0u).ebx,dst=function_aea8(m,load_le32(dgroup_ptr(m,dgroup(0x2b45u))),0u).ebx;--src;--dst;const u32 count=x*pitch;
  batch15_vga_copy_mode(b,true);if(b.out16)b.out16(0x3c4u,0x0f02u);batch15_copy(b,dst,src,count,true);batch15_vga_copy_mode(b,false);
 }
}
static_assert(0xA359u-0xA2B0u==169u);''')

replace_func('function_a3e8','static_assert(0xA473u-0xA3E8u==139u);',r'''void function_a3e8(Machine&m,LowGraphics13Boundary&b,u32 source_page,u32 sy,u32 sx,u32 dy,u32 dx,u32 height,u32 width)noexcept{
 const u8 saved=*dgroup_ptr(m,dgroup(0x2b31u));function_aedc(m,(source_page&0xffu)|(static_cast<u32>(saved)<<8));const PlanarAddress13 s=function_aea8(m,sy,sx);width+=s.ecx;const u32 pitch=load_le32(dgroup_ptr(m,dgroup(0x2b41u)));function_aedc(m,saved);const PlanarAddress13 d=function_aea8(m,dy,dx);u32 src=s.ebx,dst=d.ebx;const u32 row=(width+3u)>>2,skip=pitch-row;
 batch15_vga_copy_mode(b,true);if(b.out16)b.out16(0x3c4u,0x0f02u);u32 rows=height;do{batch15_copy(b,dst,src,row);dst+=row+skip;src+=row+skip;}while(--rows!=0u);batch15_vga_copy_mode(b,false);
}
static_assert(0xA473u-0xA3E8u==139u);''')

replace_func('function_a484','static_assert(0xA4E7u-0xA484u==99u);',r'''void function_a484(Machine&m,LowGraphics13Boundary&b,u32 dst,u32 y,u32 x,u32 height,u32 width)noexcept{
 const PlanarAddress13 p0=function_aea8(m,y,x);const u32 row=(width+p0.ecx+3u)>>2,skip=load_le32(dgroup_ptr(m,dgroup(0x2b41u)))-row;u32 out=dst;
 for(u32 plane=0;plane<4u;++plane){if(b.out16)b.out16(0x3ceu,static_cast<u16>(0x0004u|(plane<<8)));u32 src=p0.ebx,rows=height;do{batch15_copy(b,out,src,row);out+=row;src+=row+skip;}while(--rows!=0u);}
}
static_assert(0xA4E7u-0xA484u==99u);''')

replace_func('function_a4e8','static_assert(0xA555u-0xA4E8u==109u);',r'''void function_a4e8(Machine&m,LowGraphics13Boundary&b,u32 src,u32 y,u32 x,u32 height,u32 width)noexcept{
 const PlanarAddress13 p0=function_aea8(m,y,x);const u32 row=(width+p0.ecx+3u)>>2,skip=load_le32(dgroup_ptr(m,dgroup(0x2b41u)))-row;u32 in=src;
 for(u32 plane=0,mask=1u;plane<4u;++plane,mask<<=1){if(b.out16)b.out16(0x3c4u,static_cast<u16>(0x0002u|(mask<<8)));u32 dst=p0.ebx,rows=height;do{batch15_copy(b,dst,in,row);in+=row;dst+=row+skip;}while(--rows!=0u);}
 if(b.out16)b.out16(0x3c4u,0xff02u);
}
static_assert(0xA555u-0xA4E8u==109u);''')

replace_func('function_a57c','static_assert(0xA5C6u-0xA57Cu==74u);',r'''void function_a57c(Batch12Boundaries&b,u32 frame,u32 count,u32 src,u32 desc,u32 key)noexcept{
 u32 dst=desc+8u+static_cast<u32>(b12r16(b,desc))*frame;if(static_cast<u8>(key)!=0xffu)b12w8(b,dst++,static_cast<u8>(key));const u32 groups=count>>2;u32 left=groups,s=src;
 do{const u32 base=dst;b12w8(b,base,b12r8(b,s++));b12w8(b,base+groups,b12r8(b,s++));b12w8(b,base+groups*2u,b12r8(b,s++));b12w8(b,base+groups*3u,b12r8(b,s++));dst=base+1u;}while(--left!=0u);
}
static_assert(0xA5C6u-0xA57Cu==74u);''')

replace_func('function_a5c8','static_assert(0xA675u-0xA5C8u==173u);',r'''void function_a5c8(Batch12Boundaries&b,u32 frame,u32 desc)noexcept{
 u32 payload=desc+8u+static_cast<u32>(b12r16(b,desc))*frame;const u16 rows=b12r16(b,desc+4u);const u16 plane_bytes=static_cast<u8>(static_cast<u8>(b12r8(b,desc+6u))<<1);if((b12r8(b,desc+7u)&0x80u)!=0u)++payload;const u16 span=static_cast<u16>(rows*plane_bytes);
 auto swap_rows=[&](u32 lo,u16 offset){u16 rr=rows;do{u32 hi=lo+static_cast<u32>(offset);u16 cc=plane_bytes;do{const u8 v=b12r8(b,lo);const u8 z=b12r8(b,hi);b12w8(b,lo,z);b12w8(b,hi,v);++lo;--hi;}while(--cc!=0u);}while(--rr!=0u);};
 u16 off0=span;off0=static_cast<u16>(off0<<1);off0=static_cast<u16>(off0+span);u16 tail=plane_bytes;--tail;off0=static_cast<u16>(off0+tail);swap_rows(payload,off0);
 u16 off1=span;tail=plane_bytes;--tail;off1=static_cast<u16>(off1+tail);swap_rows(payload+static_cast<u32>(span),off1);
}
static_assert(0xA675u-0xA5C8u==173u);''')

replace_func('function_a678','static_assert(0xA6F0u-0xA678u==120u);',r'''void function_a678(Batch12Boundaries&b,u32 frame,u32 desc)noexcept{
 u32 src=desc+8u+static_cast<u32>(b12r16(b,desc))*frame;const u32 rows=b12r16(b,desc+4u),width=b12r8(b,desc+6u);if((b12r8(b,desc+7u)&0x80u)!=0u)++src;u32 bottom=(rows-1u)*width;bottom<<=1;const u32 rowbytes=width<<1;
 u32 planes=4u;do{const u32 plane_start=src;u32 lo=src,hi=src+bottom;u32 pairs=rows>>1;do{u32 cc=rowbytes;do{const u8 v=b12r8(b,lo);const u8 z=b12r8(b,hi);b12w8(b,lo,z);b12w8(b,hi,v);++lo;++hi;}while(--cc!=0u);hi-=rowbytes;hi-=rowbytes;}while(--pairs!=0u);src=plane_start+bottom+rowbytes;}while(--planes!=0u);
}
static_assert(0xA6F0u-0xA678u==120u);''')

s=s.replace('// A678..A6EF: horizontal byte-pair reversal for all four planes of a frame.','// A678..A6EF: vertical row reversal within each of the four planes of a frame.')
p.write_text(s)

a=Path('Code1/FINAL_BINARY_AUDIT.md')
t=a.read_text()
section='''\n\n## Semantic audit checkpoint 10c — `A21C..A6EF`\n\nFresh direct disassembly of the immutable original `CODE.1` reconciled the page-copy, planar read/write, frame-transpose, and in-place frame-transform block through `A6EF`. `A2B0` now passes the encoded zero x-coordinate into `AEA8` in both directions; the first stack argument remains only the unsigned byte-count multiplier. The `A3E8`, `A484`, `A4E8`, and `A57C` loops now preserve the machine code's entry-before-`DEC/JNE` behavior, including zero counters wrapping through their full register width rather than silently becoming zero-iteration C++ loops.\n\n`A5C8` was materially corrected: byte `+6` is doubled through `SHL DL,1` with 8-bit truncation, the row-span multiply truncates through AX, and the two swap passes use the encoded offsets `3*span + plane_bytes - 1` and `span + plane_bytes - 1`. The previous second pass incorrectly addressed one plane beyond the frame. `A678` is not a horizontal row mirror; it swaps top and bottom rows inside each of four planes, using `2*width` bytes per row and advancing each plane by the encoded bottom-row offset plus one row. All arithmetic/counters retain their original unsigned wrap widths. Object accounting remains **69,361 translated instructions + 1,041 proven non-code/data = 70,402 / 70,402**, with **0 unresolved bytes**.\n'''
if '## Semantic audit checkpoint 10c — `A21C..A6EF`' not in t:t+=section
a.write_text(t)
print('checkpoint 10c patch applied')
