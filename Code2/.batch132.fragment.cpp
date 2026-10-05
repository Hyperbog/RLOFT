
// Batch 132: object-relative 38106..384A9 (932 translated bytes).
struct Runtime132Boundary {
    u32 (*descriptor_flags)(u32)=nullptr; void (*set_runtime_error)(u32)=nullptr; void (*translate_dos_error)(u16)=nullptr;
    s32 (*dos_read)(u32,u32,u32)=nullptr; s32 (*dos_write_mem)(u32,u32,u32)=nullptr; s32 (*dos_write_bytes)(u32,const u8*,u32)=nullptr;
    s32 (*dos_seek)(u32,s32,u8)=nullptr; s32 (*dos_delete)(u32)=nullptr; u32 (*runtime_version)()=nullptr; u32 (*runtime_version_old)()=nullptr;
    u8 (*read8)(u32)=nullptr; void (*write8)(u32,u8)=nullptr; u8 (*get_default_drive)()=nullptr; u8 (*set_default_drive)(u8)=nullptr;
};
static s32 function_38106(Runtime132Boundary& r,u32 h,u32 dst,u32 n){
 u32 f=r.descriptor_flags?r.descriptor_flags(h):0u; if(!f){if(r.set_runtime_error)r.set_runtime_error(4);return -1;} if(!(f&1)){if(r.set_runtime_error)r.set_runtime_error(6);return -1;}
 if(f&0x40){s32 q=r.dos_read?r.dos_read(h,dst,n):-1;if(q<0){if(r.translate_dos_error)r.translate_dos_error(static_cast<u16>(q));return -1;}return q;}
 u32 rem=n,out=dst; s32 logical=0; while(rem){s32 q=r.dos_read?r.dos_read(h,out,rem):-1;if(q<0){if(r.translate_dos_error)r.translate_dos_error(static_cast<u16>(q));return -1;}if(!q)break;
  u32 raw=static_cast<u32>(q),i=0,p=0; while(i<raw){u8 c=r.read8?r.read8(out+i):0;if(c==0x1a){s32 back=static_cast<s32>(i)-q+1;if(r.dos_seek)(void)r.dos_seek(h,back,1);return logical;}if(c!=0x0d){if(r.write8)r.write8(out+p,c);++p;++logical;}++i;} out+=p;rem-=p;if(f&0x20)break;}
 return logical;
}
static s32 function_3821f(Runtime132Boundary& r,u32 h,u32 src,u32 n){
 u32 f=r.descriptor_flags?r.descriptor_flags(h):0u;if(!f){if(r.set_runtime_error)r.set_runtime_error(4);return -1;}if(!(f&2)){if(r.set_runtime_error)r.set_runtime_error(6);return -1;}
 if(f&0x80){s32 q=r.dos_seek?r.dos_seek(h,0,2):-1;if(q<0){if(r.translate_dos_error)r.translate_dos_error(static_cast<u16>(q));return -1;}}
 if(f&0x40){s32 q=r.dos_write_mem?r.dos_write_mem(h,src,n):-1;if(q<0){if(r.translate_dos_error)r.translate_dos_error(static_cast<u16>(q));return -1;}if(static_cast<u32>(q)!=n&&r.set_runtime_error)r.set_runtime_error(12);return q;}
 u32 chunk=0x200;u32 v=r.runtime_version?r.runtime_version():0x230;if(v<0xb0&&r.runtime_version_old)(void)r.runtime_version_old();if(v<0x230)chunk=0x80;
 u8 b[0x200]{};u32 in=0,committed=0,used=0;auto flush=[&](u32 len)->s32{s32 q=r.dos_write_bytes?r.dos_write_bytes(h,b,len):-1;if(q<0){if(r.translate_dos_error)r.translate_dos_error(static_cast<u16>(q));return -1;}if(static_cast<u32>(q)!=len){if(r.set_runtime_error)r.set_runtime_error(12);return q;}return q;};
 while(in<n){u8 c=r.read8?r.read8(src+in):0;if(c==0x0a){b[used++]=0x0d;if(used==chunk){s32 q=flush(chunk);if(q<0)return -1;if(static_cast<u32>(q)!=chunk)return static_cast<s32>(committed+static_cast<u32>(q));committed=in;used=0;}}
  b[used++]=c;++in;if(used==chunk){s32 q=flush(chunk);if(q<0)return -1;if(static_cast<u32>(q)!=chunk)return static_cast<s32>(committed+static_cast<u32>(q));committed=in;used=0;}}
 if(used){s32 q=flush(used);if(q<0)return -1;if(static_cast<u32>(q)!=used)return static_cast<s32>(committed+static_cast<u32>(q));}return static_cast<s32>(n);
}
static s32 function_383ed(Runtime132Boundary&r,u32 h,s32 o,u32 w){return r.dos_seek?r.dos_seek(h,o,static_cast<u8>(w)):-1;}
static s32 function_38407(Runtime132Boundary&r,u32 h,s32 o,u8 w){s32 q=r.dos_seek?r.dos_seek(h,o,w):-1;if(q<0){if(r.translate_dos_error)r.translate_dos_error(static_cast<u16>(q));return -1;}return q;}
static s32 function_38452(Runtime132Boundary&r,u32 p){s32 q=r.dos_delete?r.dos_delete(p):-1;if(q<0){if(r.translate_dos_error)r.translate_dos_error(static_cast<u16>(q));return q;}return 0;}
static u16 function_38474(Runtime132Boundary&r,u32 p){u32 d=static_cast<u32>(r.get_default_drive?r.get_default_drive():0)+1;if(r.write8){r.write8(p,static_cast<u8>(d));r.write8(p+1,0);r.write8(p+2,0);r.write8(p+3,0);}return 0;}
static u32 function_3848c(Runtime132Boundary&r,u32 d,u32 p){u8 n=r.set_default_drive?r.set_default_drive(static_cast<u8>(d-1)):0;if(r.write8){r.write8(p,n);r.write8(p+1,0);r.write8(p+2,0);r.write8(p+3,0);}return 0;}
