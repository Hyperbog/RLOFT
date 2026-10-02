from pathlib import Path
cpp=Path('Code1/Code1.cpp')
text=cpp.read_text()
assert 'void function_07e4(' not in text
assert 'void interrupt_80bc(' not in text
assert 'void function_ce8e_exit(' not in text
marker='} // namespace code1'
assert marker in text
block=r'''

// Batch 16: 07E4 transition renderer, deferred interrupt handlers, and shared startup exit tail.
struct Batch16Boundary{
 void(*call_8e3c)(u32,u32)=nullptr;void(*call_6588)(u32)=nullptr;void(*call_86c4)(s32,s32,s32,u32)=nullptr;
 void(*call_67c8)(u32,s32,s32,u32)=nullptr;void(*call_6c60)(u32,s32,s32,u32,u32)=nullptr;void(*call_6eb0)(u32,s32,s32,u32,u32,u32)=nullptr;
 u32(*call_954c)(u32,u32)=nullptr;void(*call_9c44)(u32,u32,u32,u32,u32,u32,s32,s32)=nullptr;void(*call_a05c)(s32,s32,s32,s32,u32)=nullptr;
 void(*call_004c)(u32,u32)=nullptr;void(*call_00ec)(s32,s32,u32,u32,u32,u32,u32)=nullptr;void(*call_55dc)(u32,s32,s32,s32,s32)=nullptr;
 void(*call_53e4)(u32)=nullptr;void(*call_064c)()=nullptr;
};
static bool b16_range(Machine&m,s32 id)noexcept{const s32 base=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x713cu))));return id>=base&&id<base+0x2e;}
static void b16_slide(Batch16Boundary&b,u32 dir,u32 step,u32 height,u32 res,s32 x,s32 y)noexcept{if(b.call_9c44)b.call_9c44(dir,step,height,1u,1u,res,x,y);}

// 07E4..10A1: staggered eleven-phase transition of slots [first,last).
void function_07e4(Machine&m,Batch16Boundary&b,s32 first,s32 last)noexcept{
 u8 counter[9]{};
 if(b.call_8e3c)b.call_8e3c(0x14u,7u);
 for(s32 slot=first;slot<last;++slot){const s32 id=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x08f8u+static_cast<u32>(slot)*8u))));if(id<0)continue;if(load_le32(dgroup_ptr(m,dgroup(0x04d0u)))==0u&&id==1)continue;if(slot==7&&id!=0x14)continue;if(b.call_8e3c)b.call_8e3c(static_cast<u32>(id),b16_range(m,id)?1u:7u);}
 s32 phase=0;
 while(counter[static_cast<u32>(last-1)]!=0x0bu){
  if(b.call_6588)b.call_6588(*dgroup_ptr(m,dgroup(0x7138u)));
  if(load_le32(dgroup_ptr(m,dgroup(0x04d0u)))==0u&&phase<2&&b.call_86c4)b.call_86c4(0xc8,0,0x19,7u);
  for(s32 slot=first;slot<last && (slot-first)<=phase;++slot)++counter[static_cast<u32>(slot)];
  for(s32 slot=first;slot<last;++slot){
   const u32 s=static_cast<u32>(slot),i8=s*8u,i4=s*4u;const u32 k=counter[s];
   s32 x=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x087cu+i8))));s32 y=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x0880u+i8))));
   const s32 old_id=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x08f4u+i8))));const s32 new_id=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x08f8u+i8))));
   if(old_id==new_id){if(new_id==-1)continue;if(slot==7&&new_id!=0x14){if(b.call_67c8)b.call_67c8(0u,x,y,load_le32(dgroup_ptr(m,dgroup(0x0958u))));}else if(b16_range(m,new_id)){if(b.call_6c60)b.call_6c60(0u,x,y,0x14u,7u);if(b.call_6eb0)b.call_6eb0(0u,x+0x15,y,static_cast<u32>(new_id),1u,0x5af8u);}else if(b.call_6eb0)b.call_6eb0(0u,x,y,static_cast<u32>(new_id),7u,0x5ff8u);continue;}
   if(new_id==-1)continue;
   if(k==0u){
    if(load_le32(dgroup_ptr(m,dgroup(0x04d0u)))==0u&&old_id==1){if(b.call_67c8)b.call_67c8(0u,x,y,load_le32(dgroup_ptr(m,dgroup(0x0954u+i4))));}
    else if(b16_range(m,old_id)){if(b.call_6c60)b.call_6c60(0u,x,y,0x14u,7u);if(b.call_6eb0)b.call_6eb0(0u,x+0x15,y,static_cast<u32>(old_id),1u,0x5af8u);}
    else if(slot==7&&old_id!=0x15){if(b.call_67c8)b.call_67c8(0u,x,y,load_le32(dgroup_ptr(m,dgroup(0x0954u))));}
    else if(b.call_6c60)b.call_6c60(0u,x,y,static_cast<u32>(old_id),7u);
    continue;
   }
   if(k<6u){
    if(b.call_6c60)b.call_6c60(0u,x,y,s+0x1eu,7u);const u32 q=6u-k;y+=static_cast<s32>(4u*k);
    if(load_le32(dgroup_ptr(m,dgroup(0x04d0u)))==0u&&old_id==1)b16_slide(b,0u,q,10u,load_le32(dgroup_ptr(m,dgroup(0x0954u+i4))),x,y);
    else if(b16_range(m,old_id)){const u32 a=b.call_954c?b.call_954c(0x14u,7u):0u,c=b.call_954c?b.call_954c(static_cast<u32>(old_id),1u):0u;b16_slide(b,0u,q,10u,a,x,y);b16_slide(b,1u,q,10u,c,x+0x15,y);}
    else if(slot==7&&old_id!=0x15)b16_slide(b,0u,q,10u,load_le32(dgroup_ptr(m,dgroup(0x0954u))),x,y);
    else{const u32 r=b.call_954c?b.call_954c(static_cast<u32>(old_id),7u):0u;b16_slide(b,0u,q,10u,r,x,y);}continue;
   }
   if(k==6u){if(b.call_6c60)b.call_6c60(0u,x,y,s+0x1eu,7u);const u32 color=*dgroup_ptr(m,dgroup(0x63f8u));if(b.call_a05c)b.call_a05c(x,y+0x18,x+0x79,y+0x18,color);continue;}
   if(k<12u){
    const u32 q=12u-k;if(b.call_6c60)b.call_6c60(0u,x,y,s+0x1eu,7u);y+=static_cast<s32>(4u*q);
    if(load_le32(dgroup_ptr(m,dgroup(0x04d0u)))==0u&&new_id==1)b16_slide(b,0u,q,6u,load_le32(dgroup_ptr(m,dgroup(0x0954u+i4))),x,y);
    else if(slot==7&&new_id!=0x14)b16_slide(b,0u,q,6u,load_le32(dgroup_ptr(m,dgroup(0x0958u))),x,y);
    else if(b16_range(m,new_id)){const u32 a=b.call_954c?b.call_954c(0x14u,7u):0u,c=b.call_954c?b.call_954c(static_cast<u32>(new_id),1u):0u;b16_slide(b,0u,q,6u,a,x,y);b16_slide(b,1u,q,6u,c,x+0x15,y);}
    else{const u32 r=b.call_954c?b.call_954c(static_cast<u32>(new_id),7u):0u;b16_slide(b,0u,q,6u,r,x,y);}continue;
   }
   if(load_le32(dgroup_ptr(m,dgroup(0x04d0u)))==0u&&new_id==1){if(b.call_67c8)b.call_67c8(0u,x,y,load_le32(dgroup_ptr(m,dgroup(0x0954u+i4))));}
   else if(slot==7&&new_id!=0x14){if(b.call_67c8)b.call_67c8(0u,x,y,load_le32(dgroup_ptr(m,dgroup(0x0958u))));}
   else if(b16_range(m,new_id)){if(b.call_6c60)b.call_6c60(0u,x,y,0x14u,7u);if(b.call_6eb0)b.call_6eb0(0u,x+0x15,y,static_cast<u32>(new_id),1u,0x5af8u);}
   else if(b.call_6c60)b.call_6c60(0u,x,y,static_cast<u32>(new_id),7u);
  }
  if(phase<2){if(load_le32(dgroup_ptr(m,dgroup(0x0940u)))!=0xffffffffu){if(b.call_004c)b.call_004c(0x55f8u,load_le32(dgroup_ptr(m,dgroup(0x0940u))));if(b.call_00ec)b.call_00ec(0x114,0x10c,0x7a,0x30,0x55f8u,0u,0x5ff8u);}if(load_le32(dgroup_ptr(m,dgroup(0x0948u)))!=0xffffffffu){if(b.call_004c)b.call_004c(0x55f8u,load_le32(dgroup_ptr(m,dgroup(0x0948u))));if(b.call_00ec)b.call_00ec(0x114,0xd7,0x7a,0x30,0x55f8u,0u,0x5ff8u);}if(b.call_55dc)b.call_55dc(load_le32(dgroup_ptr(m,dgroup(0x7138u))),0xec,0,0xa4,0x140);}
  else{if(b.call_53e4)b.call_53e4(load_le32(dgroup_ptr(m,dgroup(0x7138u))));*dgroup_ptr(m,dgroup(0x7138u))^=1u;}
  ++phase;
 }
 if(b.call_6588)b.call_6588(*dgroup_ptr(m,dgroup(0x7138u)));if(b.call_064c)b.call_064c();if(b.call_53e4)b.call_53e4(load_le32(dgroup_ptr(m,dgroup(0x7138u))));*dgroup_ptr(m,dgroup(0x7138u))^=1u;
}
static_assert(0x10A2u-0x07E4u==2238u);

struct Batch16InterruptBoundary{
 void(*enter_saved_frame)()=nullptr;void(*leave_iret_frame)()=nullptr;void(*call_cec0)()=nullptr;void(*call_dde0)(u32)=nullptr;void(*chain_292a)()=nullptr;
 u32(*in_port_dde7)(u32)=nullptr;void(*out_port_ddf2)(u32,u32)=nullptr;void(*call_da18)()=nullptr;void(*call_da1a)()=nullptr;void(*call_7fa8)(u32)=nullptr;u32(*call_802c)(u32)=nullptr;
 void(*call_64dc)(u32)=nullptr;void(*call_cf98)()=nullptr;void(*set_vector_ddfe)(u32,u32,u32)=nullptr;void(*chain_2924)()=nullptr;
};
void interrupt_7f5c(Machine&m,Batch16InterruptBoundary&b)noexcept{if(b.enter_saved_frame)b.enter_saved_frame();if(b.call_cec0)b.call_cec0();const u32 tick=load_le32(dgroup_ptr(m,dgroup(0x2934u)));store_le32(dgroup_ptr(m,dgroup(0x2934u)),tick+1u);if(b.call_dde0)b.call_dde0(tick);if(b.chain_292a)b.chain_292a();if(b.leave_iret_frame)b.leave_iret_frame();}static_assert(0x7F8Au-0x7F5Cu==46u);
void interrupt_80bc(Machine&m,Batch16InterruptBoundary&b)noexcept{if(b.enter_saved_frame)b.enter_saved_frame();if(b.call_cec0)b.call_cec0();u32 key=b.in_port_dde7?b.in_port_dde7(0x60u):0u;u8 p61=static_cast<u8>(b.in_port_dde7?b.in_port_dde7(0x61u):0u);*dgroup_ptr(m,dgroup(0x2918u))=p61;if(p61==0x80u){p61=static_cast<u8>(b.in_port_dde7?b.in_port_dde7(0x61u):0u);*dgroup_ptr(m,dgroup(0x2918u))=static_cast<u8>(p61|0x80u);}if(b.out_port_ddf2){b.out_port_ddf2(0x61u,static_cast<u8>(*dgroup_ptr(m,dgroup(0x2918u))|0x80u));b.out_port_ddf2(0x61u,*dgroup_ptr(m,dgroup(0x2918u)));}if(b.call_da18)b.call_da18();u32 scan=key;if(scan>0x80u){scan&=0x7fu;if(scan==0x2au)*dgroup_ptr(m,dgroup(0x291bu))&=0xfeu;else if(scan==0x36u)*dgroup_ptr(m,dgroup(0x291bu))&=0xfdu;else if(scan==0x38u)*dgroup_ptr(m,dgroup(0x291bu))&=0xfbu;else if(scan==0x1du)*dgroup_ptr(m,dgroup(0x291bu))&=0xf7u;else if(scan>=0x47u&&scan<=0x51u){if(b.call_7fa8)b.call_7fa8(scan);}else *dgroup_ptr(m,dgroup(0x291au))=*dgroup_ptr(m,dgroup(0x2919u));}else{if(scan==0x2au)*dgroup_ptr(m,dgroup(0x291bu))|=1u;else if(scan==0x36u)*dgroup_ptr(m,dgroup(0x291bu))|=2u;else if(scan==0x38u)*dgroup_ptr(m,dgroup(0x291bu))|=4u;else if(scan==0x1du)*dgroup_ptr(m,dgroup(0x291bu))|=8u;else{if(scan>=0x47u&&scan<=0x51u){scan=b.call_802c?b.call_802c(scan):scan;*dgroup_ptr(m,dgroup(0x2919u))=static_cast<u8>(scan);}*dgroup_ptr(m,dgroup(0x291au))=static_cast<u8>(scan);}}const u8 code=*dgroup_ptr(m,dgroup(0x291au));if(code==0x10u&&(*dgroup_ptr(m,dgroup(0x291bu))&4u)){*dgroup_ptr(m,dgroup(0x2b01u))|=1u;*dgroup_ptr(m,dgroup(0x291au))=0u;}else if(code==0x43u)*dgroup_ptr(m,dgroup(0x291au))=0u;else if(code==1u)*dgroup_ptr(m,dgroup(0x2b01u))|=2u;else if(code==0x39u)*dgroup_ptr(m,dgroup(0x291cu))=1u;if(b.out_port_ddf2)b.out_port_ddf2(0x20u,0x20u);if(b.call_da1a)b.call_da1a();if(b.leave_iret_frame)b.leave_iret_frame();}static_assert(0x826Eu-0x80BCu==434u);
void interrupt_8338(Machine&m,Batch16InterruptBoundary&b)noexcept{if(b.enter_saved_frame)b.enter_saved_frame();if(b.call_cec0)b.call_cec0();if(b.call_64dc)b.call_64dc(0xffu);if(b.call_cf98)b.call_cf98();if(b.set_vector_ddfe){b.set_vector_ddfe(9u,load_le32(dgroup_ptr(m,dgroup(0x291eu))),load_le16(dgroup_ptr(m,dgroup(0x2922u))));b.set_vector_ddfe(8u,load_le32(dgroup_ptr(m,dgroup(0x292au))),load_le16(dgroup_ptr(m,dgroup(0x292eu))));}if(b.chain_2924)b.chain_2924();if(b.leave_iret_frame)b.leave_iret_frame();}static_assert(0x8398u-0x8338u==96u);

struct StartupExit16Boundary{u32(*open_3d01)(u32)=nullptr;u8(*read8)(u32)=nullptr;void(*write_40)(u32,u32,u32)=nullptr;void(*call_fd2b)(u32)=nullptr;void(*terminate_4c)(u8)=nullptr;};
[[noreturn]] void function_ce8b_exit(StartupExit16Boundary&b,u32 exit_code){if(b.call_fd2b)b.call_fd2b(exit_code);if(b.terminate_4c)b.terminate_4c(static_cast<u8>(exit_code));for(;;){}}
[[noreturn]] void function_ce8e_exit(StartupExit16Boundary&b,u32 message,u32 exit_code){const u32 h=b.open_3d01?b.open_3d01(0xcc92u):0u;u32 p=message;while((b.read8?b.read8(p):0u)!=0u)++p;if(b.write_40)b.write_40(h,message,p-message);function_ce8b_exit(b,exit_code);}
static_assert(0xCEC0u-0xCE8Bu==53u);
'''
text=text.replace('constexpr u32 kVerifiedTranslatedBytes=16261u;','constexpr u32 kVerifiedTranslatedBytes=66622u;')
text=text.replace(marker,block+'\n'+marker)
cpp.write_text(text)

p=Path('Code1/PROGRESS.md');s=p.read_text();s=s.replace('verified%20conversion-90.56%25-red','verified%20conversion-94.63%25-red').replace('**Verified conversion:** 90.56%','**Verified conversion:** 94.63%').replace('**Remaining:** 9.44%','**Remaining:** 5.37%').replace('Current verified translated bytes: **63,755**.','Current verified translated bytes: **66,622**.').replace('Raw coverage: **90.5585%**, displayed as **90.56%**.','Raw coverage: **94.6308%**, displayed as **94.63%**.')
needle='- Batch 15 actor animation/choice and planar VGA helpers: `15B0..1A00`, `2198..24D0`, `266C..28E1`, selected `A21C..A554`, `A6F0..A8F3`, with documented alignment excluded: **3,886 bytes**.\n'
assert needle in s
s=s.replace(needle,needle+'- Batch 16 transition renderer / interrupt / startup exit: `07E4..10A1`, `7F5C..7F89`, `80BC..826D`, `8338..8397`, `CE8B..CEBF`: **2,867 bytes**.\n')
p.write_text(s)
for name in ['Code1/STAGING_LEDGER.md','Code1/DECOMPILATION_CONTINUITY.md']:
 q=Path(name);t=q.read_text();t+='\n\n## Batch 16 integration checkpoint\nIntegrated **2,867 verified bytes**: the full `07E4..10A1` staggered transition renderer, interrupt handlers `7F5C..7F89`, `80BC..826D`, `8338..8397`, and shared startup exit tail `CE8B..CEBF`. The startup staging pool is now empty. Hardware frame entry/IRET and DOS termination remain explicit machine-boundary callbacks rather than host substitutions. Verified total: **66,622 / 70,402 = 94.63%**.\n'
 q.write_text(t)
print('integrated batch16')
