from pathlib import Path
p=Path('Code1/Code1.cpp')
s=p.read_text()
anchor='// CC1C..CD96: startup recovery scaffolding.'
assert anchor in s and 'function_38a4(' not in s
code=r'''
// 38A4..4976: actor-stat presentation/adjustment and the following two-page presentation loop.
// External renderer/audio/resource routines remain explicit boundaries; guest state stays in DGROUP.
struct ActorStatsBoundaries{
 void(*call_6c60)(s32,s32,s32,s32,u32)=nullptr; void(*call_7df4)(u32)=nullptr;
 void(*call_004c)(u32,u32)=nullptr; void(*call_7e44)(s32,s32,u32,u32)=nullptr;
 void(*call_afee3)(u32,u32,u32)=nullptr; void(*call_afee4)(u32,u32,u32,u32)=nullptr;
 u32(*call_88d8)(s32)=nullptr; void(*call_7e04)(s32,s32,u32,u32)=nullptr;
 void(*call_72a0)(u32,u32)=nullptr; void(*call_b01a)(u32,u32)=nullptr;
 void(*call_56dc)()=nullptr; void(*call_6588)(u32)=nullptr;
 void(*call_55dc)(u32,s32,s32,s32,s32)=nullptr; u32(*call_53b4)()=nullptr;
 u32(*call_7868)(s32,s32,s32,s32,u32)=nullptr; void(*call_9eb0)(s32,s32,s32,s32,u32)=nullptr;
 void(*call_7830)(u32)=nullptr; void(*call_67c8)(u32,s32,s32,u32)=nullptr;
 u32(*call_7f50)()=nullptr; void(*call_61ac)(u32)=nullptr; void(*call_5780)()=nullptr;
 void(*call_a3e8)(u32,s32,s32,s32,s32,s32,u32)=nullptr; void(*call_57dc)()=nullptr;
 u32(*call_954c)(u32,u32)=nullptr; void(*call_a5c8)(u32,u32)=nullptr;
 void(*call_6c98)(s32,s32,s32,s32,u32)=nullptr; void(*call_86c4)(s32,s32,u32,u32)=nullptr;
 void(*call_a390)(u32)=nullptr; void(*call_65a4)(u32)=nullptr; void(*call_87c4)()=nullptr;
 void(*call_87d8)(u32,u32)=nullptr; void(*call_bc1c)(u32,u32,u32)=nullptr;
 void(*call_bfe0)(u32,u32,u32,u32,u32,u32)=nullptr; void(*call_c4e0)(u32)=nullptr;
 void(*call_91d8)(u32,u32)=nullptr;
};
[[nodiscard]]static u32 actor_page_xor1(Machine&m)noexcept{u32 v=load_le32(dgroup_ptr(m,dgroup(0x7138u)));set_low8(v,static_cast<u8>(low8(v)^1u));return v;}

// 38A4..3C54: render the selected actor's six byte stats, secondary derived values,
// class progression counts, and hit-point field into the 55F8 scratch string.
void function_38a4(Machine&m,ActorStatsBoundaries&b,s32 x,s32 y)noexcept{
 if(b.call_6c60)b.call_6c60(0,x,y,0x14,7);x+=4;
 s32 row=0;const u32 ab=actor_base(m);
 for(;row<6;++row){
  if(row==static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x7124u))))){*dgroup_ptr(m,dgroup(0x5a00u))=0x5au;if(b.call_7df4)b.call_7df4(0x59f8u);}
  if(b.call_004c)b.call_004c(0x55f8u,static_cast<u32>(row+0x2f));
  if(b.call_7e44)b.call_7e44(x,y+1,0x55f8u,0);
  const u32 stat_addr=ab+0x682au+static_cast<u32>(row)*2u;const u32 stat=*dgroup_ptr(m,dgroup(stat_addr));
  if(row==0&&*dgroup_ptr(m,dgroup(ab+0x6836u))!=0u){const u32 rem=*dgroup_ptr(m,dgroup(ab+0x6836u))%100u;if(b.call_afee4)b.call_afee4(0x55f8u,0x54u,stat,rem);}
  else if(b.call_afee3)b.call_afee3(0x55f8u,0x4cu,stat);
  if(b.call_7e04)b.call_7e04(x,y+0x2f,0x55f8u,0);
  if(row==static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x7124u))))){*dgroup_ptr(m,dgroup(0x5a00u))=8u;if(b.call_7df4)b.call_7df4(0);}
  x+=0x0a;
 }
 x+=0x0a;
 if(b.call_004c)b.call_004c(0x55f8u,static_cast<u32>(row+0x2f));
 if(b.call_7e44)b.call_7e44(x,y+1,0x55f8u,0);
 {const s32 v=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(ab+0x6810u))));const u32 q=b.call_88d8?b.call_88d8(v):0;if(b.call_7e04)b.call_7e04(x,y+0x2f,q,0);}
 x+=0x0a;
 if(b.call_004c)b.call_004c(0x55f8u,static_cast<u32>(row+0x30));++row;
 if(b.call_7e44)b.call_7e44(x,y+1,0x55f8u,0);
 {const s32 cls=*reinterpret_cast<std::int8_t*>(dgroup_ptr(m,dgroup(ab+0x682cu)));const s32 bonus=*reinterpret_cast<std::int8_t*>(dgroup_ptr(m,dgroup(0x083fu+static_cast<u32>(static_cast<u8>(cls)))));const s32 v=*reinterpret_cast<std::int8_t*>(dgroup_ptr(m,dgroup(ab+0x6813u)))+bonus;const u32 q=b.call_88d8?b.call_88d8(v):0;if(b.call_7e04)b.call_7e04(x,y+0x2f,q,0);}
 x+=0x0a;
 if(b.call_004c)b.call_004c(0x55f8u,static_cast<u32>(row+0x30));row+=2;
 if(static_cast<std::int8_t>(*dgroup_ptr(m,dgroup(ab+0x680eu)))!=-1)if(b.call_004c)b.call_004c(0x55f8u,static_cast<u32>(row+0x2f));
 if(b.call_7e44)b.call_7e44(x,y+1,0x55f8u,0);
 {const s32 v=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(ab+0x6818u))));const u32 q=b.call_88d8?b.call_88d8(v):0;if(b.call_72a0)b.call_72a0(0x55f8u,q);}
 if(static_cast<std::int8_t>(*dgroup_ptr(m,dgroup(ab+0x680eu)))!=-1){if(b.call_b01a)b.call_b01a(0x55f8u,0x60u);const s32 v=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(ab+0x681au))));const u32 q=b.call_88d8?b.call_88d8(v):0;if(b.call_b01a)b.call_b01a(0x55f8u,q);}
 if(static_cast<std::int8_t>(*dgroup_ptr(m,dgroup(ab+0x680fu)))!=-1){if(b.call_b01a)b.call_b01a(0x55f8u,0x60u);const s32 v=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(ab+0x681cu))));const u32 q=b.call_88d8?b.call_88d8(v):0;if(b.call_b01a)b.call_b01a(0x55f8u,q);}
 if(b.call_7e04)b.call_7e04(x,y+0x2f,0x55f8u,0);x+=0x0a;++row;
 if(load_le32(dgroup_ptr(m,dgroup(0x7124u)))==10u){*dgroup_ptr(m,dgroup(0x5a00u))=0x5au;if(b.call_7df4)b.call_7df4(0x59f8u);}
 if(b.call_004c)b.call_004c(0x55f8u,static_cast<u32>(row+0x2f));
 if(b.call_7e44)b.call_7e44(x,y+1,0x55f8u,0);
 {const s32 v=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(ab+0x6816u))));const u32 q=b.call_88d8?b.call_88d8(v):0;if(b.call_7e04)b.call_7e04(x,y+0x2f,q,0);}
 if(load_le32(dgroup_ptr(m,dgroup(0x7124u)))==10u){*dgroup_ptr(m,dgroup(0x5a00u))=8u;if(b.call_7df4)b.call_7df4(0);}
}
static_assert(0x3C55u-0x38A4u==945u);
// 3C55..3C57 is a three-byte LEA alignment instruction.

static void actor_stats_redraw(Machine&m,ActorStatsBoundaries&b)noexcept{
 const u32 page=*dgroup_ptr(m,dgroup(0x7138u));if(b.call_6588)b.call_6588(page);function_38a4(m,b,0x5b,0x88);if(b.call_55dc)b.call_55dc(page,0x5b,0x88,0x7a,0x30);while(b.call_53b4&&b.call_53b4()!=0u){}
}
// 3C58..40FA: interactive stat adjustment. Event 7 increments, event 9 decrements,
// event 8 updates the selected column from mouse X, and event 0 exits and rebuilds the two cached surfaces.
void function_3c58(Machine&m,MidUiBoundaries&ui,ActorStatsBoundaries&b)noexcept{
 store_le32(dgroup_ptr(m,dgroup(0x0924u)),load_le32(dgroup_ptr(m,dgroup(0x0928u))));
 store_le32(dgroup_ptr(m,dgroup(0x092cu)),load_le32(dgroup_ptr(m,dgroup(0x0930u))));
 store_le32(dgroup_ptr(m,dgroup(0x0934u)),load_le32(dgroup_ptr(m,dgroup(0x0938u))));
 store_le32(dgroup_ptr(m,dgroup(0x0928u)),0x2cu);store_le32(dgroup_ptr(m,dgroup(0x0938u)),0x2du);store_le32(dgroup_ptr(m,dgroup(0x0940u)),3u);
 if(ui.call_7e4)ui.call_7e4(6,9);if(b.call_56dc)b.call_56dc();store_le32(dgroup_ptr(m,dgroup(0x7124u)),0xffffffffu);
 bool done=false;
 while(!done){const s32 ev=ui.call_43c?ui.call_43c(m):0;if(ev==0){done=true;continue;}const u32 ab=actor_base(m);const s32 sel=static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x7124u))));
  if(ev==7&&sel!=-1){
   if(sel<6){u8*stat=dgroup_ptr(m,dgroup(ab+0x682au+static_cast<u32>(sel)*2u));if(sel==0){if((*dgroup_ptr(m,dgroup(0x04d4u))&1u)!=0u){if(*stat==0x12u){const u8 cls=*dgroup_ptr(m,dgroup(ab+0x680du));if(cls==0u||cls==1u||cls==2u)++*dgroup_ptr(m,dgroup(ab+0x6836u));}else ++*stat;}else{*stat=0x14u;*dgroup_ptr(m,dgroup(ab+0x6836u))=0x64u;}}else if((*dgroup_ptr(m,dgroup(0x04d4u))&1u)!=0u)++*stat;else *stat=0x14u;}
   else if(sel==10){u16 v=load_le16(dgroup_ptr(m,dgroup(ab+0x6816u)));v=((*dgroup_ptr(m,dgroup(0x04d4u))&1u)!=0u)?static_cast<u16>(v+1u):static_cast<u16>(1000u);store_le16(dgroup_ptr(m,dgroup(ab+0x6816u)),v);}function_3404(m);actor_stats_redraw(m,b);
  }else if(ev==8){const s32 mx=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x73e6u))));const s32 q=(mx-0x60)/10;store_le32(dgroup_ptr(m,dgroup(0x7124u)),(q>=0&&(q<6||q==10))?static_cast<u32>(q):0xffffffffu);actor_stats_redraw(m,b);
  }else if(ev==9&&sel!=-1){
   if(sel<6){u8*stat=dgroup_ptr(m,dgroup(ab+0x682au+static_cast<u32>(sel)*2u));if(sel==0){if((*dgroup_ptr(m,dgroup(0x04d4u))&1u)!=0u){if(*stat==0x12u&&*dgroup_ptr(m,dgroup(ab+0x6836u))!=1u)--*dgroup_ptr(m,dgroup(ab+0x6836u));else --*stat;}else{*stat=0;*dgroup_ptr(m,dgroup(ab+0x6836u))=0;}}else if((*dgroup_ptr(m,dgroup(0x04d4u))&1u)!=0u)--*stat;else *stat=0;}
   else if(sel==10){u16 v=load_le16(dgroup_ptr(m,dgroup(ab+0x6816u)));v=((*dgroup_ptr(m,dgroup(0x04d4u))&1u)!=0u)?static_cast<u16>(v-1u):0u;store_le16(dgroup_ptr(m,dgroup(ab+0x6816u)),v);}function_3404(m);actor_stats_redraw(m,b);
  }
 }
 store_le32(dgroup_ptr(m,dgroup(0x7124u)),0xffffffffu);const u32 page=*dgroup_ptr(m,dgroup(0x7138u));if(b.call_6588)b.call_6588(page);
 const u32 temp=b.call_7868?b.call_7868(0,0,0x7a,0x30,0xff):0u;if(b.call_9eb0)b.call_9eb0(0,0,0x7a,0x30,0xfe);function_38a4(m,b,0,0);
 for(u32 cell:{0x0954u,0x0958u}){const u32 h=load_le32(dgroup_ptr(m,dgroup(cell)));if(h&&b.call_7830)b.call_7830(h);store_le32(dgroup_ptr(m,dgroup(cell)),0u);}
 store_le32(dgroup_ptr(m,dgroup(0x0958u)),b.call_7868?b.call_7868(0,0,0x7a,0x30,0xff):0u);store_le32(dgroup_ptr(m,dgroup(0x0954u)),b.call_7868?b.call_7868(0,0,0x7a,0x30,0xff):0u);
 if(b.call_67c8)b.call_67c8(temp,0,0,0);if(b.call_7830)b.call_7830(temp);
 store_le32(dgroup_ptr(m,dgroup(0x0924u)),load_le32(dgroup_ptr(m,dgroup(0x0928u))));store_le32(dgroup_ptr(m,dgroup(0x092cu)),load_le32(dgroup_ptr(m,dgroup(0x0930u))));store_le32(dgroup_ptr(m,dgroup(0x0934u)),load_le32(dgroup_ptr(m,dgroup(0x0938u))));
 store_le32(dgroup_ptr(m,dgroup(0x0928u)),0x16u);store_le32(dgroup_ptr(m,dgroup(0x0938u)),0x2eu);store_le32(dgroup_ptr(m,dgroup(0x0940u)),0x0eu);if(ui.call_7e4)ui.call_7e4(6,9);
}
static_assert(0x40FBu-0x3C58u==1187u);
// 40FB is one-byte NOP alignment.

// 40FC..44E0: actor-generation presentation controller. The local EBX state is preserved as
// three states: 0=continue event polling, 1=return to the inner presentation loop, 2=leave routine.
void function_40fc(Machine&m,MidUiBoundaries&ui,ActorGenerationBoundaries&gen,EarlyGameBoundaries&rng,ActorStatsBoundaries&b)noexcept{
 const u32 tick=b.call_7f50?b.call_7f50():0u;if(b.call_61ac)b.call_61ac(tick);function_3884(m,rng);store_le32(dgroup_ptr(m,dgroup(0x7124u)),0xffffffffu);
 if(b.call_6588)b.call_6588(*dgroup_ptr(m,dgroup(0x7138u)));u32 temp=b.call_7868?b.call_7868(0,0,0x7a,0x30,0xff):0u;if(b.call_9eb0)b.call_9eb0(0,0,0x7a,0x30,0xfe);function_38a4(m,b,0,0);store_le32(dgroup_ptr(m,dgroup(0x0958u)),b.call_7868?b.call_7868(0,0,0x7a,0x30,0xff):0u);if(b.call_67c8)b.call_67c8(temp,0,0,0);if(b.call_7830)b.call_7830(temp);
 const u32 slots[11]={0xffffffffu,0xffffffffu,0xffffffffu,0xffffffffu,0xffffffffu,0xffffffffu,0x16u,2u,0x2eu,0x0eu,0xffffffffu};function_1464(m,ui,5u,slots,0x2b);
 auto draw_pages=[&](){for(int i=0;i<2;++i){const u32 page=*dgroup_ptr(m,dgroup(0x7138u));if(b.call_6588)b.call_6588(page);if(b.call_67c8)b.call_67c8(load_le32(dgroup_ptr(m,dgroup(0x0958u))),0x5b,0x88,0);if(b.call_6c60)b.call_6c60(0,0x5b,0xbd,0x2e,7);if(gen.call_53e4)gen.call_53e4(page);*dgroup_ptr(m,dgroup(0x7138u))^=1u;}};draw_pages();
 for(;;){s32 state=0;while(state==0){const s32 ev=ui.call_43c?ui.call_43c(m):0;if(ev==0){state=2;if(ui.call_15b0)ui.call_15b0(-1,-1,-1,-1);store_le32(dgroup_ptr(m,dgroup(0x04d0u)),load_le32(dgroup_ptr(m,dgroup(0x04d0u)))-1u);}else if(ev==7){const u32 h=load_le32(dgroup_ptr(m,dgroup(0x0954u)));if(h&&b.call_7830)b.call_7830(h);store_le32(dgroup_ptr(m,dgroup(0x0954u)),load_le32(dgroup_ptr(m,dgroup(0x0958u))));function_3884(m,rng);if(b.call_6588)b.call_6588(*dgroup_ptr(m,dgroup(0x7138u)));temp=b.call_7868?b.call_7868(0,0,0x7a,0x30,0xff):0u;if(b.call_9eb0)b.call_9eb0(0,0,0x7a,0x30,0xfe);function_38a4(m,b,0,0);store_le32(dgroup_ptr(m,dgroup(0x0958u)),b.call_7868?b.call_7868(0,0,0x7a,0x30,0xff):0u);if(b.call_67c8)b.call_67c8(temp,0,0,0);if(b.call_7830)b.call_7830(temp);store_le32(dgroup_ptr(m,dgroup(0x0924u)),load_le32(dgroup_ptr(m,dgroup(0x0928u))));store_le32(dgroup_ptr(m,dgroup(0x0934u)),load_le32(dgroup_ptr(m,dgroup(0x0938u))));store_le32(dgroup_ptr(m,dgroup(0x092cu)),load_le32(dgroup_ptr(m,dgroup(0x0930u))));*dgroup_ptr(m,dgroup(0x0930u))^=1u;if(ui.call_7e4)ui.call_7e4(7,8);state=1;}else if(ev==8){if(ui.call_15b0)ui.call_15b0(-1,-1,-1,-1);function_3138(m,ui,gen);state=2;store_le32(dgroup_ptr(m,dgroup(0x04d0u)),load_le32(dgroup_ptr(m,dgroup(0x04d0u)))+1u);}else if(ev==9){if(b.call_004c)b.call_004c(0x7140u,0x2cu);if(b.call_6588)b.call_6588(*dgroup_ptr(m,dgroup(0x7138u)));if(b.call_6c60)b.call_6c60(0,5,0,0x2a,7);if(gen.call_7d80)gen.call_7d80(5,0,0x140,0x7140u,1);if(b.call_5780)b.call_5780();if(b.call_a3e8)b.call_a3e8(actor_page_xor1(m),5,0,5,0,0x14,0x140);if(b.call_57dc)b.call_57dc();function_3c58(m,ui,b);if(b.call_72a0)b.call_72a0(0x7140u,load_le32(dgroup_ptr(m,dgroup(0x04b8u))));if(b.call_6588)b.call_6588(*dgroup_ptr(m,dgroup(0x7138u)));if(b.call_6c60)b.call_6c60(0,5,0,0x2a,7);if(gen.call_7d80)gen.call_7d80(5,0,0x140,0x7140u,1);if(b.call_5780)b.call_5780();if(b.call_a3e8)b.call_a3e8(actor_page_xor1(m),5,0,5,0,0x14,0x140);if(b.call_57dc)b.call_57dc();state=1;}}
  if(state==1){draw_pages();continue;}break;
 }
 for(u32 cell:{0x0954u,0x0958u}){const u32 h=load_le32(dgroup_ptr(m,dgroup(cell)));if(h&&b.call_7830)b.call_7830(h);store_le32(dgroup_ptr(m,dgroup(cell)),0u);}
}
static_assert(0x44E1u-0x40FCu==997u);
// 44E1..44E3 is a three-byte LEA alignment instruction.

// 44E4..451E: advance the paired local counters; the two encoded wrap pairs are (20,-2) and (0,2).
void function_44e4(s32*a,s32*b)noexcept{s32 sum=*a+*b,step=*b;if(sum==0x18){sum=0x14;step=-2;}if(sum==-4){sum=0;step=2;}*a=sum;*b=step;}
static_assert(0x451Fu-0x44E4u==59u);
// 451F is one-byte NOP alignment.

// 4520..45EE: draw one step of the paired-counter indicator using resource 3C/3D and the fixed 3B marker.
void function_4520(ActorStatsBoundaries&b,s32 value,s32 step)noexcept{
 if(value==0x16){const u32 q=b.call_954c?b.call_954c(0x3du,7u):0u;if(b.call_a5c8)b.call_a5c8(0,q);if(b.call_6c98)b.call_6c98(0,0xe5,value-step,0x3d,7);const u32 r=b.call_954c?b.call_954c(0x3du,7u):0u;if(b.call_a5c8)b.call_a5c8(0,r);}
 else if(value==-2){if(b.call_6c98)b.call_6c98(0,0xe5,value-step,0x3d,7);}
 else{if(step>0){const u32 q=b.call_954c?b.call_954c(0x3cu,7u):0u;if(b.call_a5c8)b.call_a5c8(0,q);}if(b.call_6c98)b.call_6c98(0,0xe5,value,0x3c,7);if(step>0){const u32 q=b.call_954c?b.call_954c(0x3cu,7u):0u;if(b.call_a5c8)b.call_a5c8(0,q);}}
 if(b.call_6c60)b.call_6c60(0,0xe5,0,0x3b,7);
}
static_assert(0x45EFu-0x4520u==207u);
// 45EF..45F8 is the ten-byte local lookup table "678456789:"; 45F9..45FB is LEA alignment.

// 45FC..4976: two-pass presentation loop. The local table is indexed as group*5 + floor(frame/2),
// 44E4 advances the paired indicator counters, and the two 87C4/87D8 loops preserve their exact 10/25 iterations.
void function_45fc(Machine&m,ActorStatsBoundaries&b)noexcept{
 static constexpr u8 table[10]={'6','7','8','4','5','6','7','8','9',':'};s32 value=0,step=2;
 if(b.call_6588)b.call_6588(*dgroup_ptr(m,dgroup(0x7138u)));if(b.call_86c4)b.call_86c4(0,0,0x1b,7);if(b.call_86c4)b.call_86c4(0xc8,0,0x1c,7);
 const u32 panel=b.call_7868?b.call_7868(0xe5,0,0xaa,0x50,0xff):0u;store_le32(dgroup_ptr(m,dgroup(0x7130u)),panel);function_4520(b,0,2);if(b.call_6c60)b.call_6c60(0,0x15f,0x74,0x31,7);if(b.call_a390)b.call_a390(actor_page_xor1(m));if(b.call_65a4)b.call_65a4(*dgroup_ptr(m,dgroup(0x7138u)));*dgroup_ptr(m,dgroup(0x7138u))^=1u;
 for(u32 id=0x31;id<=0x3d;++id)if(b.call_954c)(void)b.call_954c(id,7u);if(b.call_6588)b.call_6588(*dgroup_ptr(m,dgroup(0x7138u)));if(b.call_67c8)b.call_67c8(panel,0xe5,0,0);
 u32 capture=b.call_7868?b.call_7868(0x96,0x8c,0xe2,0x68,0xff):0u;
 for(u32 group=0;group<2u;++group){
  for(s32 frame=0;frame<10;++frame){if(b.call_87c4)b.call_87c4();if(b.call_6588)b.call_6588(*dgroup_ptr(m,dgroup(0x7138u)));if(b.call_67c8)b.call_67c8(capture,0x96,0x8c,0);if(b.call_6c60)b.call_6c60(0,0x96,0x8c,table[group*5u+static_cast<u32>(frame/2)],7);if(b.call_67c8)b.call_67c8(panel,0xe5,0,0);function_4520(b,value,step);if(b.call_65a4)b.call_65a4(*dgroup_ptr(m,dgroup(0x7138u)));*dgroup_ptr(m,dgroup(0x7138u))^=1u;if(b.call_87d8)b.call_87d8(2,0);
   if(group==0u&&frame==0){if(b.call_bc1c)b.call_bc1c(0x18,1,0x9c4u);if(b.call_bfe0)b.call_bfe0(0x64f8u,0,0xffu,0,0,0);}if(frame==1&&b.call_c4e0)b.call_c4e0(load_le32(dgroup_ptr(m,dgroup(0x712cu)))+0x0du);function_44e4(&value,&step);
  }
  for(s32 frame=0;frame<25;++frame){if(b.call_87c4)b.call_87c4();if(b.call_6588)b.call_6588(*dgroup_ptr(m,dgroup(0x7138u)));if(b.call_67c8)b.call_67c8(capture,0x96,0x8c,0);if(b.call_6c60){if(group==0u)b.call_6c60(0,0x15f,0x95,0x32,7);else b.call_6c60(0,0x15f,0xb2,0x33,7);}if(b.call_67c8)b.call_67c8(panel,0xe5,0,0);function_4520(b,value,step);if(b.call_65a4)b.call_65a4(*dgroup_ptr(m,dgroup(0x7138u)));*dgroup_ptr(m,dgroup(0x7138u))^=1u;if(b.call_87d8)b.call_87d8(2,0);function_44e4(&value,&step);if(group==0u&&frame==15&&b.call_c4e0)b.call_c4e0(load_le32(dgroup_ptr(m,dgroup(0x712cu)))+0x0cu);}
  if(b.call_7830)b.call_7830(capture);capture=b.call_7868?b.call_7868(0x96,0x8c,0xd8,0x68,0xff):0u;
 }
 if(b.call_91d8){b.call_91d8(0x32,7);b.call_91d8(0x33,7);for(u32 id=0x34;id<=0x3a;++id)b.call_91d8(id,7);}if(b.call_7830){b.call_7830(capture);b.call_7830(panel);}
}
static_assert(0x4977u-0x45FCu==891u);

'''
s=s.replace(anchor,code+anchor)
p.write_text(s)

pp=Path('Code1/PROGRESS.md');t=pp.read_text();assert '**24,603**' in t
t=t.replace('34.95%25-red','41.03%25-red').replace('**Verified conversion:** 34.95%','**Verified conversion:** 41.03%').replace('**Remaining:** 65.05%','**Remaining:** 58.97%').replace('**24,603**','**28,889**').replace('**34.9465%**','**41.0343%**').replace('displayed as **25.51%**','displayed as **41.03%**').replace('Total integrated verified translated bytes: **24,603**.','Total integrated verified translated bytes: **28,889**.')
needle='- Startup/recovery regions through `CD96`: **259 bytes**.\n';assert needle in t;t=t.replace(needle,'- Actor stat/presentation routines `38A4..3C54`, `3C58..40FA`, `40FC..44E0`, `44E4..451E`, `4520..45EE`, `45FC..4976`: **4,286 bytes**.\n'+needle);pp.write_text(t)
sp=Path('Code1/STAGING_LEDGER.md');sp.write_text(sp.read_text()+'\n- Integrated directly from authoritative disassembly: `38A4..3C54`, `3C58..40FA`, `40FC..44E0`, `44E4..451E`, `4520..45EE`, `45FC..4976` = **4,286 bytes**. `3C55..3C57`, `40FB`, `44E1..44E3`, `451F`, and `45F9..45FB` are alignment; `45EF..45F8` is a ten-byte lookup table and is data.\n')
cp=Path('Code1/DECOMPILATION_CONTINUITY.md');cp.write_text(cp.read_text()+'\n\n## 2026-10-01 actor stat/presentation batch 06\nVerified total **28,889 / 70,402 = 41.03%**. Added **4,286 bytes** at `38A4..3C54`, `3C58..40FA`, `40FC..44E0`, `44E4..451E`, `4520..45EE`, `45FC..4976`. `45EF..45F8` is proven lookup data (`678456789:`); adjacent LEA/NOP alignment is excluded. 3C58 preserves event 7/8/9 stat-edit behavior and the `4D4` bit-0 alternate set/inc/dec semantics. 40FC preserves its EBX three-state control flow. 45FC preserves the 10-frame then 25-frame loops for each of two groups, including 44E4 paired-counter wrapping. Active staging is unchanged.\n')
