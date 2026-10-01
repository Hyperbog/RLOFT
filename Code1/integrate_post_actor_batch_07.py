from pathlib import Path
p=Path('Code1/Code1.cpp'); s=p.read_text(); anchor='// CC1C..CD96: startup recovery scaffolding.'
assert anchor in s and 'function_4978(' not in s
code=r'''
// 4978..59AB: post-generation controller, palette/table preparation, resource bootstrap,
// mouse/cursor page management, and the first 9-byte search-tree insertion helper.
// Calls outside this verified span stay explicit boundaries; all encoded DGROUP offsets are retained.
struct PostActorBoundaries{
 void(*call_c254)(u32,u32,u32,u32,u32)=nullptr; void(*call_mode)(u32)=nullptr;
 void(*call_5a34)(u32)=nullptr; u32(*call_5b64)(u32,u32,u32,u32)=nullptr; void(*call_cbd4)(u32)=nullptr;
 s32(*open_id)(u32)=nullptr; void(*fatal_9814)(u32)=nullptr; void(*close_handle)(u32)=nullptr;
 s32(*seek_handle)(u32,u32,u32)=nullptr; u16(*read_u16_handle)(u32)=nullptr; u32(*read_u32_handle)(u32)=nullptr; void(*read_guest)(Machine&,u32,u32,u32)=nullptr; void(*call_6320)(u32,u32,u32)=nullptr;
 void(*call_b74c)(u32)=nullptr; void(*call_c650)(u32)=nullptr; u32(*alloc_71c8)(u32)=nullptr;
 void(*call_95bc)(u32,u32)=nullptr; void(*call_9724)(u32,u32,u32)=nullptr; void(*call_64dc)(u32)=nullptr;
 void(*call_6618)(u32)=nullptr; void(*call_9ac4)()=nullptr; void(*call_84a0)(u32,u32,u32)=nullptr;
 void(*call_6588)(u32)=nullptr; void(*call_65a4)(u32)=nullptr; s32(*call_53b4)()=nullptr; void(*call_61d0)(u32)=nullptr;
 void(*call_bb80)(u32)=nullptr; void(*call_bcb4)()=nullptr; void(*call_9778)()=nullptr; void(*call_5f4c)()=nullptr;
 void(*terminate_cbf0)(u32)=nullptr; u32(*call_84e0)()=nullptr; void(*call_6c44)(u32)=nullptr;
 void(*call_6bc0)(s32,s32,s32,s32)=nullptr; void(*call_5780)()=nullptr; void(*call_57dc)()=nullptr;
 void(*call_cec8)(u32,u32,u32,u32)=nullptr; u16(*call_84d0)()=nullptr;
 void(*call_d130)(s32,s32)=nullptr; void(*call_d10c)(s32,s32)=nullptr; void(*call_d08c)(s32,s32)=nullptr;
 struct MouseSnapshot{u16 buttons=0;u16 pad=0;u16 y=0;u16 x=0;}; MouseSnapshot(*call_d0b4)()=nullptr;
 u32(*call_954c)(u32,u32)=nullptr; void(*call_d1e8)(u32,u32,u32)=nullptr; void(*call_cf98)()=nullptr;
 void(*call_9ae8)()=nullptr; void(*call_8740)(u32)=nullptr; void(*call_a390)(u32)=nullptr; void(*call_8788)()=nullptr;
 void(*call_a3e8)(u32,s32,s32,s32,s32,s32,s32)=nullptr; void(*call_6744)(u32,s32,s32,u32,u32)=nullptr;
 void(*call_66c0)(u32,s32,s32,u32,u32)=nullptr; void(*call_6d54)(u32,s32,s32,u32)=nullptr;
 void(*call_63a4)(u32)=nullptr; u32(*call_62d8)(u32)=nullptr; u32(*call_62c0)(u32)=nullptr;
};
void function_52ac(Machine&,PostActorBoundaries&)noexcept;
void function_538c(Machine&,PostActorBoundaries&)noexcept;
[[nodiscard]]static s32 div4_toward_zero(s32 v)noexcept{return v/4;}

// 4978..4A09: top-level character-generation controller. Mode 7 wraps back to mode 0;
// a completed pair of actors returns 10, while abandoning with an incomplete pair returns 0.
[[nodiscard]]s32 function_4978(Machine&m,ActorStatsBoundaries&stats,PostActorBoundaries&b)noexcept{
 store_le32(dgroup_ptr(m,dgroup(0x04d0u)),0u);if(b.call_c254)b.call_c254(0x64f8u,0,8,0x64,0x100);function_45fc(m,stats);
 *dgroup_ptr(m,dgroup(0x73eeu))=static_cast<u8>(*dgroup_ptr(m,dgroup(0x7138u))^1u);if(b.call_57dc)b.call_57dc();
 for(;;){u32 mode=load_le32(dgroup_ptr(m,dgroup(0x04d0u)));if(mode==7u){store_le32(dgroup_ptr(m,dgroup(0x04d0u)),0u);continue;}if(b.call_mode)b.call_mode(mode);while(b.call_53b4&&b.call_53b4()!=0u){}if(load_le32(dgroup_ptr(m,dgroup(0x04ccu)))!=0xffffffffu)continue;return (*dgroup_ptr(m,dgroup(0x04c4u))!=0u&&*dgroup_ptr(m,dgroup(0x04c5u))!=0u)?10:0;}
}
static_assert(0x4A0Au-0x4978u==146u); // 4A0A..4A0B is MOV EAX,EAX alignment.

// 4A0C..4AFF: build five 256-entry palette remap rows. Each component moves from the
// 64F8 source palette toward 63 by ((63-component)*row+2)/4 with signed truncation toward zero.
void function_4a0c(Machine&m,PostActorBoundaries&b)noexcept{
 if(b.call_5a34)b.call_5a34(0x64f8u);
 for(s32 row=0;row<5;++row)for(u32 i=0;i<0x100u;++i){const u32 q=0x64f8u+i*3u;const s32 r=*dgroup_ptr(m,dgroup(q)),g=*dgroup_ptr(m,dgroup(q+1u)),bl=*dgroup_ptr(m,dgroup(q+2u));const s32 rr=r+div4_toward_zero((63-r)*row+2),gg=g+div4_toward_zero((63-g)*row+2),bb=bl+div4_toward_zero((63-bl)*row+2);const u32 v=b.call_5b64?b.call_5b64(static_cast<u8>(rr),static_cast<u8>(gg),static_cast<u8>(bb),0x0fu):0u;*dgroup_ptr(m,dgroup(0x5ff8u+static_cast<u32>(row)*0x100u+i))=static_cast<u8>(v);}
}
static_assert(0x4B00u-0x4A0Cu==244u);

// 4B00..4B16: identity byte table at 59F8.
void function_4b00(Machine&m)noexcept{for(u32 i=0;i<0x100u;++i)*dgroup_ptr(m,dgroup(0x59f8u+i))=static_cast<u8>(i);}static_assert(0x4B17u-0x4B00u==23u); // 4B17 NOP alignment.

// 4B18..4C7E: derive the base 256-entry luminance bucket map and four progressively
// brightened rows, then pass the resulting 5*256 bytes through CBD4.
void function_4b18(Machine&m,PostActorBoundaries&b)noexcept{
 for(u32 i=0;i<0x100u;++i){const u32 q=0x64f8u+i*3u;const s32 lum=(static_cast<s32>(*dgroup_ptr(m,dgroup(q)))*300)/1000+(static_cast<s32>(*dgroup_ptr(m,dgroup(q+1u)))*590)/1000+(static_cast<s32>(*dgroup_ptr(m,dgroup(q+2u)))*110)/1000;s32 bucket=0;while(bucket<0x1c&&static_cast<s32>(*dgroup_ptr(m,dgroup(0x64f8u+static_cast<u32>(bucket)*3u)))<=lum)++bucket;--bucket;*dgroup_ptr(m,dgroup(0x5af8u+i))=static_cast<u8>(bucket);}
 for(s32 row=1;row<5;++row)for(u32 i=0;i<0x100u;++i){const u32 src=*dgroup_ptr(m,dgroup(0x5af8u+i));const s32 red=*dgroup_ptr(m,dgroup(0x64f8u+src*3u));const s32 target=red+div4_toward_zero((63-red)*row+2);s32 bucket=0;while(bucket<0x1c&&static_cast<s32>(*dgroup_ptr(m,dgroup(0x64f8u+static_cast<u32>(bucket)*3u)))<=target)++bucket;--bucket;*dgroup_ptr(m,dgroup(0x5af8u+static_cast<u32>(row)*0x100u+i))=static_cast<u8>(bucket);}
 if(b.call_cbd4)b.call_cbd4(0x5af8u);
}
static_assert(0x4C7Fu-0x4B18u==359u); // 4C7F NOP alignment.

// 4C80..4CAF: seek to index*2 and read a 16-bit value into the low word of the handle argument slot.
[[nodiscard]]u32 function_4c80(PostActorBoundaries&b,u32 index,u32 handle)noexcept{u32 result=handle;if(b.seek_handle)(void)b.seek_handle(index*2u,0u,handle);const u16 word=b.read_u16_handle?b.read_u16_handle(handle):0u;set_low16(result,word);return result;}
static_assert(0x4CB0u-0x4C80u==48u);

// 4CB0..4D50: open resource 6C, read indices 0/2/0B into 7134/713C/712C, then open 74
// and retain that second handle at 04C8 after the B74C/C650 setup calls.
void function_4cb0(Machine&m,PostActorBoundaries&b)noexcept{
 s32 h=b.open_id?b.open_id(0x6cu):-1;if(h==-1&&b.fatal_9814)b.fatal_9814(load_le32(dgroup_ptr(m,dgroup(0x04bcu))));
 store_le32(dgroup_ptr(m,dgroup(0x7134u)),function_4c80(b,0u,static_cast<u32>(h)));store_le32(dgroup_ptr(m,dgroup(0x713cu)),function_4c80(b,2u,static_cast<u32>(h)));store_le32(dgroup_ptr(m,dgroup(0x712cu)),function_4c80(b,0x0bu,static_cast<u32>(h)));if(b.close_handle)b.close_handle(static_cast<u32>(h));
 h=b.open_id?b.open_id(0x74u):-1;if(h==-1&&b.fatal_9814)b.fatal_9814(load_le32(dgroup_ptr(m,dgroup(0x04c0u))));if(b.call_b74c)b.call_b74c(5u);if(b.call_c650)b.call_c650(6u);store_le32(dgroup_ptr(m,dgroup(0x04c8u)),static_cast<u32>(h));
}
static_assert(0x4D51u-0x4CB0u==161u); // 4D51..4D53 LEA alignment.

// 4D54..4DC3: load resource 78. The first dword is a record count; nonzero counts allocate
// count*65 bytes at 7128, read the payload, and close the file. A zero count leaves the handle open exactly as encoded.
void function_4d54(Machine&m,PostActorBoundaries&b)noexcept{s32 h=b.open_id?b.open_id(0x78u):-1;const u32 count=b.read_u32_handle?b.read_u32_handle(static_cast<u32>(h)):0u;store_le32(dgroup_ptr(m,dgroup(0x7120u)),count);if(count==0u)return;const u32 bytes=count*65u;const u32 dst=b.alloc_71c8?b.alloc_71c8(bytes):0u;store_le32(dgroup_ptr(m,dgroup(0x7128u)),dst);if(b.read_guest)b.read_guest(m,dst,bytes,static_cast<u32>(h));if(b.close_handle)b.close_handle(static_cast<u32>(h));}
static_assert(0x4DC4u-0x4D54u==112u);

// 4DC4..4EED: resource/video bootstrap for the character generator, including palette-table
// construction and the four 0x248-byte actor records initialized at 67F8.
void function_4dc4(Machine&m,PostActorBoundaries&b)noexcept{
 if(b.call_95bc)b.call_95bc(0x84u,0x0bu);if(b.call_9724)b.call_9724(0x88u,1u,0x3cu);function_4d54(m,b);function_4cb0(m,b);if(b.call_64dc)b.call_64dc(3u);if(b.call_c254)b.call_c254(0x64f8u,0,8,0x64,0x100);if(b.call_6618)b.call_6618(0x64f8u);if(b.call_9ac4)b.call_9ac4();function_4b18(m,b);function_4b00(m);function_4a0c(m,b);if(b.call_84a0)b.call_84a0(0x64f8u,0u,0x300u);if(b.call_6618)b.call_6618(0x64f8u);if(b.call_c254)b.call_c254(0x64f8u,0,8,0x64,0x100);
 store_le32(dgroup_ptr(m,dgroup(0x7138u)),1u);if(b.call_6588)b.call_6588(1u);if(b.call_65a4)b.call_65a4(0u);function_52ac(m,b);if(b.call_84a0)b.call_84a0(0x67f8u,0xffffffffu,0x920u);for(u32 actor=0;actor<4u;++actor){const u32 base=actor*0x248u;*dgroup_ptr(m,dgroup(base+0x6809u))|=0x80u;for(u32 i=0;i<4u;++i)store_le32(dgroup_ptr(m,dgroup(base+0x6903u+i*4u)),0u);}
}
static_assert(0x4EEEu-0x4DC4u==298u); // 4EEE..4EEF MOV EAX,EAX alignment.

// 4EF0..5013: finalize the first two actor records, duplicate primary stat bytes into their
// paired slots, synthesize class flags at 6929, then write the 920-byte actor block through resource 94.
void function_4ef0(Machine&m,PostActorBoundaries&b)noexcept{
 for(u32 actor=0;actor<2u;++actor){const u32 base=actor*0x248u;const u32 start=base+0x67f8u+0x10bu;const u32 end=(actor+1u)*0x248u+0x67f8u;if(b.call_84a0)b.call_84a0(start,0u,end-start);*dgroup_ptr(m,dgroup(base+0x6813u))=0x0au;store_le16(dgroup_ptr(m,dgroup(base+0x6814u)),load_le16(dgroup_ptr(m,dgroup(base+0x6816u))));for(u32 i=0;i<7u;++i)*dgroup_ptr(m,dgroup(base+0x682bu+i*2u))=*dgroup_ptr(m,dgroup(base+0x682au+i*2u));u8 flags=0;for(u32 i=0;i<3u;++i){const u8 c=*dgroup_ptr(m,dgroup(base+0x680du+i));if(c==3u)flags|=1u;if(c==4u)flags|=2u;}*dgroup_ptr(m,dgroup(base+0x6929u))=flags;}
 if(b.call_63a4)b.call_63a4(0x94u);u32 h=b.call_62d8?b.call_62d8(0x94u):0u;if(b.close_handle)b.close_handle(h);h=b.call_62c0?b.call_62c0(0x94u):0u;if(b.call_6320)b.call_6320(0x67f8u,0x920u,h);if(b.close_handle)b.close_handle(h);
}
static_assert(0x5014u-0x4EF0u==292u);

// 5014..5089: shutdown/exit dispatcher. Code 0B is normalized to 0A; code 0A with both
// actor slots populated executes 5F4C and 4EF0 before the shared CBF0 termination tail.
void function_5014(Machine&m,PostActorBoundaries&b,u32 code)noexcept{if(b.call_bb80)b.call_bb80(0x9c4u);if(b.call_bcb4)b.call_bcb4();if(code!=0x0bu){if(b.close_handle)b.close_handle(load_le32(dgroup_ptr(m,dgroup(0x04c8u))));function_538c(m,b);if(b.call_9778)b.call_9778();}else code=0x0au;if(code!=0x0au){if(b.call_64dc)b.call_64dc(0xffu);}else if(*dgroup_ptr(m,dgroup(0x04c4u))&&*dgroup_ptr(m,dgroup(0x04c5u))){if(b.call_5f4c)b.call_5f4c();function_4ef0(m,b);}if(b.terminate_cbf0)b.terminate_cbf0(code);}
static_assert(0x508Au-0x5014u==118u); // 508A..508B MOV EAX,EAX alignment.

// 508C..50BF: entry wrapper. Non-mode-2 invocation reports resource 9C then terminates with 1;
// mode 2 performs bootstrap, runs the controller, and forwards its result to 5014.
void function_508c(Machine&m,ActorStatsBoundaries&stats,PostActorBoundaries&b,u32 mode)noexcept{if(mode!=2u){if(b.call_61d0)b.call_61d0(0x9cu);if(b.terminate_cbf0)b.terminate_cbf0(1u);return;}function_4dc4(m,b);function_5014(m,b,static_cast<u32>(function_4978(m,stats,b)));}
static_assert(0x50C0u-0x508Cu==52u);

// 50C0..522E: cursor-aware rectangle update. Optional 84E0 scaling rewrites the X/Y arguments;
// when the mouse depth is active and coordinates change, the four 2401/03/05/07 words are saved,
// a full-screen rectangle cycle is performed, and those words are restored.
void function_50c0(Machine&m,PostActorBoundaries&b,u32, u16 tag,s32 y,s32 x,const s16 point[2])noexcept{
 y>>=1;if(b.call_84e0&&b.call_84e0()!=0u){x=static_cast<s32>((static_cast<std::int64_t>(point[1]+0x1fc0)*399+0x1fa0)/0x3f40);y=static_cast<s32>((static_cast<std::int64_t>(point[0]+0x1fc0)*319+0x1fa0)/0x3f40);}
 if(load_le16(dgroup_ptr(m,dgroup(0x73ecu)))>=1u&&load_le32(dgroup_ptr(m,dgroup(0x097cu)))==0u){if(static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x73e6u))))!=x||static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x73e8u))))!=y){store_le16(dgroup_ptr(m,dgroup(0x71d8u)),static_cast<u16>(x));store_le16(dgroup_ptr(m,dgroup(0x71d6u)),static_cast<u16>(y));store_le16(dgroup_ptr(m,dgroup(0x71d4u)),tag);for(u32 i=0;i<4u;++i)store_le16(dgroup_ptr(m,dgroup(0x7188u+i*2u)),load_le16(dgroup_ptr(m,dgroup(0x2401u+i*2u))));if(b.call_6c44)b.call_6c44(0x7178u);if(b.call_6bc0)b.call_6bc0(0,0,0x18f,0x13f);if(b.call_5780)b.call_5780();if(b.call_57dc)b.call_57dc();if(b.call_6bc0)b.call_6bc0(static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x7178u)))),static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x7180u)))),static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x717cu)))),static_cast<s32>(load_le32(dgroup_ptr(m,dgroup(0x7184u)))));for(u32 i=0;i<4u;++i)store_le16(dgroup_ptr(m,dgroup(0x2401u+i*2u)),load_le16(dgroup_ptr(m,dgroup(0x7188u+i*2u))));}}
 store_le16(dgroup_ptr(m,dgroup(0x71d4u)),tag);
}
static_assert(0x522Fu-0x50C0u==367u); // 522F NOP alignment.

// 5230..5279: build the 33h callback descriptor at 71AC/71B4/71B8/71C8 and invoke CEC8.
void function_5230(Machine&m,PostActorBoundaries&b,u16 value)noexcept{store_le16(dgroup_ptr(m,dgroup(0x71acu)),0x0cu);store_le16(dgroup_ptr(m,dgroup(0x71b4u)),value);store_le32(dgroup_ptr(m,dgroup(0x71b8u)),0x6104u);store_le16(dgroup_ptr(m,dgroup(0x71c8u)),m.cpu.cs);if(b.call_cec8)b.call_cec8(0x33u,0x71acu,0x7190u,0x71c8u);}
static_assert(0x527Au-0x5230u==74u); // 527A..527B MOV EAX,EAX alignment.

// 527C..52A9: copy cursor X/Y/tag globals to three caller-supplied guest words.
void function_527c(Machine&m,u32 out_x,u32 out_y,u32 out_tag)noexcept{store_le16(dgroup_ptr(m,dgroup(out_x)),load_le16(dgroup_ptr(m,dgroup(0x71d8u))));store_le16(dgroup_ptr(m,dgroup(out_y)),load_le16(dgroup_ptr(m,dgroup(0x71d6u))));store_le16(dgroup_ptr(m,dgroup(out_tag)),load_le16(dgroup_ptr(m,dgroup(0x71d4u))));}
static_assert(0x52AAu-0x527Cu==46u); // 52AA..52AB MOV EAX,EAX alignment.

// 52AC..538B: initialize mouse/cursor state, copy the 20A-byte cursor image to 71DC,
// and install the 33h callback only when 84D0 reports a nonzero mouse capability.
void function_52ac(Machine&m,PostActorBoundaries&b)noexcept{const u16 cap=b.call_84d0?b.call_84d0():0u;store_le16(dgroup_ptr(m,dgroup(0x71dau)),cap);if(cap==0u)return;if(b.call_d130)b.call_d130(0,0x18f);if(b.call_d10c)b.call_d10c(0,0x27e);if(b.call_d08c)b.call_d08c(0xc8,0xa0);const auto snap=b.call_d0b4?b.call_d0b4():PostActorBoundaries::MouseSnapshot{};store_le16(dgroup_ptr(m,dgroup(0x71d8u)),snap.x);store_le16(dgroup_ptr(m,dgroup(0x73e6u)),snap.x);store_le16(dgroup_ptr(m,dgroup(0x71d6u)),snap.y);store_le16(dgroup_ptr(m,dgroup(0x73e8u)),snap.y);store_le16(dgroup_ptr(m,dgroup(0x73ecu)),0);store_le16(dgroup_ptr(m,dgroup(0x71d4u)),0);store_le16(dgroup_ptr(m,dgroup(0x73eau)),0);*dgroup_ptr(m,dgroup(0x73eeu))=*dgroup_ptr(m,dgroup(0x23e6u));const u32 image=b.call_954c?b.call_954c(load_le32(dgroup_ptr(m,dgroup(0x7134u)))+0x2fu,0u):0u;if(b.call_d1e8)b.call_d1e8(0x71dcu,image,0x20au);if(b.call_6bc0)b.call_6bc0(0,0,0x18f,0x13f);function_5230(m,b,0xffffu);}
static_assert(0x538Cu-0x52ACu==224u);

// 538C..53B0: cursor shutdown wrapper.
void function_538c(Machine&m,PostActorBoundaries&b)noexcept{if(load_le16(dgroup_ptr(m,dgroup(0x71dau)))==0u)return;if(load_le16(dgroup_ptr(m,dgroup(0x73ecu)))!=0u&&b.call_5780)b.call_5780();if(b.call_cf98)b.call_cf98();}
static_assert(0x53B1u-0x538Cu==37u); // 53B1..53B3 LEA alignment.

// 53B4..53E1: poll event state; the low mouse-status word is mirrored at 73EA and sign-extended.
[[nodiscard]]s32 function_53b4(Machine&m,PostActorBoundaries&b)noexcept{if(b.call_9ae8)b.call_9ae8();if(load_le16(dgroup_ptr(m,dgroup(0x71dau)))==0u)return 0;const auto snap=b.call_d0b4?b.call_d0b4():PostActorBoundaries::MouseSnapshot{};store_le16(dgroup_ptr(m,dgroup(0x73eau)),snap.buttons);return static_cast<s16>(snap.buttons);}
static_assert(0x53E2u-0x53B4u==46u); // 53E2..53E3 MOV EAX,EAX alignment.

// 53E4..5450: page switch with cursor suppression/restoration.
void function_53e4(Machine&m,PostActorBoundaries&b,u32 page)noexcept{if(b.call_9ae8)b.call_9ae8();if(load_le16(dgroup_ptr(m,dgroup(0x71dau)))==0u)return;store_le32(dgroup_ptr(m,dgroup(0x097cu)),1u);*dgroup_ptr(m,dgroup(0x73eeu))=static_cast<u8>(page);store_le16(dgroup_ptr(m,dgroup(0x73ecu)),0);if(b.call_57dc)b.call_57dc();if(b.call_65a4)b.call_65a4(static_cast<u8>(page));*dgroup_ptr(m,dgroup(0x73eeu))=static_cast<u8>(page^1u);if(b.call_5780)b.call_5780();*dgroup_ptr(m,dgroup(0x73eeu))=static_cast<u8>(page);store_le16(dgroup_ptr(m,dgroup(0x73ecu)),1);store_le32(dgroup_ptr(m,dgroup(0x097cu)),0u);}
static_assert(0x5451u-0x53E4u==109u); // 5451..5453 LEA alignment.

// 5454..550A: page flip plus full 400-byte cursor backing-buffer copy.
void function_5454(Machine&m,PostActorBoundaries&b,u32 page)noexcept{if(b.call_9ae8)b.call_9ae8();if(load_le16(dgroup_ptr(m,dgroup(0x71dau)))==0u)return;store_le32(dgroup_ptr(m,dgroup(0x097cu)),1u);*dgroup_ptr(m,dgroup(0x73eeu))=static_cast<u8>(page);store_le16(dgroup_ptr(m,dgroup(0x73ecu)),0);if(b.call_57dc)b.call_57dc();if(b.call_65a4)b.call_65a4(static_cast<u8>(page));if(b.call_8740)b.call_8740(page);u32 other=page;set_low8(other,static_cast<u8>(low8(other)^1u));if(b.call_a390)b.call_a390(other);*dgroup_ptr(m,dgroup(0x73eeu))=low8(other);if(b.call_d1e8)b.call_d1e8(0x73efu+(other<<10),0x73efu+(page<<10),0x400u);if(b.call_5780)b.call_5780();if(b.call_8788)b.call_8788();*dgroup_ptr(m,dgroup(0x73eeu))=static_cast<u8>(page);store_le16(dgroup_ptr(m,dgroup(0x73ecu)),1);store_le32(dgroup_ptr(m,dgroup(0x097cu)),0u);}
static_assert(0x550Bu-0x5454u==183u); // 550B NOP alignment.

// 550C..55DA: region-aware page flip; A3E8 receives (page^1,x,y,x,y,w,h), then the
// complete 400-byte backing page is copied before cursor restoration.
void function_550c(Machine&m,PostActorBoundaries&b,u32 page,s32 x,s32 y,s32 w,s32 h)noexcept{if(b.call_9ae8)b.call_9ae8();if(load_le16(dgroup_ptr(m,dgroup(0x71dau)))==0u)return;store_le32(dgroup_ptr(m,dgroup(0x097cu)),1u);*dgroup_ptr(m,dgroup(0x73eeu))=static_cast<u8>(page);store_le16(dgroup_ptr(m,dgroup(0x73ecu)),0);if(b.call_57dc)b.call_57dc();if(b.call_65a4)b.call_65a4(static_cast<u8>(page));if(b.call_8740)b.call_8740(page);u32 other=page;set_low8(other,static_cast<u8>(low8(other)^1u));if(b.call_a3e8)b.call_a3e8(other,x,y,x,y,w,h);*dgroup_ptr(m,dgroup(0x73eeu))=low8(other);if(b.call_d1e8)b.call_d1e8(0x73efu+(other<<10),0x73efu+(page<<10),0x400u);if(b.call_5780)b.call_5780();if(b.call_8788)b.call_8788();*dgroup_ptr(m,dgroup(0x73eeu))=static_cast<u8>(page);store_le16(dgroup_ptr(m,dgroup(0x73ecu)),1);store_le32(dgroup_ptr(m,dgroup(0x097cu)),0u);}
static_assert(0x55DBu-0x550Cu==207u); // 55DB NOP alignment.

// 55DC..56DB: rectangle update that avoids disturbing the cursor when the 32x16 cursor box
// is disjoint; overlap uses the saved-page/depth path and copies the whole 400-byte backing buffer.
void function_55dc(Machine&m,PostActorBoundaries&b,u32 page,s32 x,s32 y,s32 w,s32 h)noexcept{if(load_le16(dgroup_ptr(m,dgroup(0x71dau)))==0u)return;store_le32(dgroup_ptr(m,dgroup(0x097cu)),1u);const s32 cx=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x73e6u)))),cy=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x73e8u))));const bool disjoint=(cx+0x20<x)||(cx>x+w)||(cy+0x10<y)||(cy>y+h);u32 other=page;set_low8(other,static_cast<u8>(low8(other)^1u));if(disjoint){if(b.call_a3e8)b.call_a3e8(other,x,y,x,y,w,h);}else{const u8 saved=*dgroup_ptr(m,dgroup(0x73eeu));*dgroup_ptr(m,dgroup(0x73eeu))=static_cast<u8>(page);store_le16(dgroup_ptr(m,dgroup(0x73ecu)),static_cast<u16>(load_le16(dgroup_ptr(m,dgroup(0x73ecu)))-1u));if(b.call_57dc)b.call_57dc();if(b.call_a3e8)b.call_a3e8(other,x,y,x,y,w,h);if(b.call_d1e8)b.call_d1e8(0x73efu+(other<<10),0x73efu+(page<<10),0x400u);if(b.call_5780)b.call_5780();*dgroup_ptr(m,dgroup(0x73eeu))=saved;store_le16(dgroup_ptr(m,dgroup(0x73ecu)),static_cast<u16>(load_le16(dgroup_ptr(m,dgroup(0x73ecu)))+1u));}store_le32(dgroup_ptr(m,dgroup(0x097cu)),0u);}
static_assert(0x56DCu-0x55DCu==256u);

// 56DC..577C: full backing-page switch driven by byte 23E6; depth is incremented after restore.
void function_56dc(Machine&m,PostActorBoundaries&b)noexcept{if(load_le16(dgroup_ptr(m,dgroup(0x71dau)))==0u)return;const u8 saved=*dgroup_ptr(m,dgroup(0x73eeu));store_le32(dgroup_ptr(m,dgroup(0x097cu)),1u);const u8 active=*dgroup_ptr(m,dgroup(0x23e6u));if(b.call_8740)b.call_8740(active);if(b.call_a390)b.call_a390(active^1u);*dgroup_ptr(m,dgroup(0x73eeu))=static_cast<u8>(active^1u);if(b.call_d1e8)b.call_d1e8(0x73efu+static_cast<u32>(*dgroup_ptr(m,dgroup(0x73eeu)))*0x400u,0x73efu+static_cast<u32>(saved)*0x400u,0x400u);if(b.call_5780)b.call_5780();if(b.call_8788)b.call_8788();*dgroup_ptr(m,dgroup(0x73eeu))=saved;store_le32(dgroup_ptr(m,dgroup(0x097cu)),0u);store_le16(dgroup_ptr(m,dgroup(0x73ecu)),static_cast<u16>(load_le16(dgroup_ptr(m,dgroup(0x73ecu)))+1u));}
static_assert(0x577Du-0x56DCu==161u); // 577D..577F LEA alignment.

// 5780..57DA: decrement cursor nesting depth; on transition to zero restore the saved 32x16 rectangle.
void function_5780(Machine&m,PostActorBoundaries&b)noexcept{if(load_le16(dgroup_ptr(m,dgroup(0x71dau)))==0u)return;const u16 depth=static_cast<u16>(load_le16(dgroup_ptr(m,dgroup(0x73ecu)))-1u);store_le16(dgroup_ptr(m,dgroup(0x73ecu)),depth);if(depth!=0u)return;const u8 page=*dgroup_ptr(m,dgroup(0x73eeu));if(b.call_8740)b.call_8740(page);if(b.call_6744)b.call_6744(0x73efu+static_cast<u32>(page)*0x400u,static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x73e6u)))),static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x73e8u)))),0x20u,0x10u);if(b.call_8788)b.call_8788();}
static_assert(0x57DBu-0x5780u==91u); // 57DB NOP alignment.

// 57DC..5878: increment cursor nesting depth. At depth zero, capture the underlying 32x16
// rectangle to the current backing page and draw the cursor resource at 71DC before incrementing.
void function_57dc(Machine&m,PostActorBoundaries&b)noexcept{if(load_le16(dgroup_ptr(m,dgroup(0x71dau)))==0u)return;u16 depth=load_le16(dgroup_ptr(m,dgroup(0x73ecu)));if(depth==0u){const u8 page=*dgroup_ptr(m,dgroup(0x73eeu));if(b.call_8740)b.call_8740(page);function_527c(m,0x73e6u,0x73e8u,0x73eau);const s32 x=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x73e6u)))),y=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x73e8u))));if(b.call_66c0)b.call_66c0(0x73efu+static_cast<u32>(page)*0x400u,x,y,0x20u,0x10u);if(b.call_6d54)b.call_6d54(0,x,y,0x71dcu);if(b.call_8788)b.call_8788();}store_le16(dgroup_ptr(m,dgroup(0x73ecu)),static_cast<u16>(depth+1u));}
static_assert(0x5879u-0x57DCu==157u);
// 5879..587F are seven zero bytes and are data/alignment.

// 5880..59AB: insert one 9-byte palette-search record into the binary tree rooted through
// record zero. Components rotate 0->1->2 while descending; word links +5/+7 use 0101h as sentinel.
void function_5880(Machine&m,const u8 packed[9],s16 index)noexcept{const s32 idx=index;const u32 rec=0x7ff0u+static_cast<u32>(idx*9);*dgroup_ptr(m,dgroup(rec+1u))=packed[1];*dgroup_ptr(m,dgroup(rec))=packed[0];*dgroup_ptr(m,dgroup(rec+2u))=packed[2];store_le16(dgroup_ptr(m,dgroup(rec+3u)),load_le16(packed+3));store_le16(dgroup_ptr(m,dgroup(rec+7u)),0x0101u);store_le16(dgroup_ptr(m,dgroup(rec+5u)),0x0101u);s16 node=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(0x7ff7u)))),parent=0,component=-1;bool right=true;while(node!=0x0101){parent=node;component=(component==2)?0:static_cast<s16>(component+1);right=false;const u32 base=0x7ff0u+static_cast<u32>(static_cast<s32>(node)*9);if(packed[component]>=*dgroup_ptr(m,dgroup(base+static_cast<u32>(component)))){node=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(base+7u))));right=true;}else node=static_cast<s16>(load_le16(dgroup_ptr(m,dgroup(base+5u))));}const u32 pbase=0x7ff0u+static_cast<u32>(static_cast<s32>(parent)*9);store_le16(dgroup_ptr(m,dgroup(pbase+(right?7u:5u))),static_cast<u16>(index));}
static_assert(0x59ACu-0x5880u==300u);

'''
s=s.replace(anchor,code+anchor);p.write_text(s)
pp=Path('Code1/PROGRESS.md');t=pp.read_text();assert '**28,889**' in t
t=t.replace('41.03%25-red','46.87%25-red').replace('**Verified conversion:** 41.03%','**Verified conversion:** 46.87%').replace('**Remaining:** 58.97%','**Remaining:** 53.13%').replace('**28,889**','**33,000**').replace('**41.0343%**','**46.8737%**').replace('Total integrated verified translated bytes: **28,889**.','Total integrated verified translated bytes: **33,000**.')
needle='- Startup/recovery regions through `CD96`: **259 bytes**.\n';assert needle in t;t=t.replace(needle,'- Post-generation/palette/resource/cursor/tree routines `4978..4A09`, `4A0C..4AFF`, `4B00..4B16`, `4B18..4C7E`, `4C80..4CAF`, `4CB0..4D50`, `4D54..4DC3`, `4DC4..4EED`, `4EF0..5013`, `5014..5089`, `508C..50BF`, `50C0..522E`, `5230..5279`, `527C..52A9`, `52AC..538B`, `538C..53B0`, `53B4..53E1`, `53E4..5450`, `5454..550A`, `550C..55DA`, `55DC..56DB`, `56DC..577C`, `5780..57DA`, `57DC..5878`, `5880..59AB`: **4,111 bytes**.\n'+needle);pp.write_text(t)
sp=Path('Code1/STAGING_LEDGER.md');sp.write_text(sp.read_text()+'\n- Integrated directly from authoritative disassembly: post-generation/palette/resource/cursor/tree span `4978..59AB` with verified code subranges totaling **4,111 bytes**. Alignment/data excluded: `4A0A..4A0B`, `4B17`, `4C7F`, `4D51..4D53`, `4EEE..4EEF`, `508A..508B`, `522F`, `527A..527B`, `52AA..52AB`, `53B1..53B3`, `53E2..53E3`, `5451..5453`, `550B`, `55DB`, `577D..577F`, `57DB`, and seven zero bytes `5879..587F`. Active staging unchanged.\n')
cp=Path('Code1/DECOMPILATION_CONTINUITY.md');cp.write_text(cp.read_text()+'\n\n## 2026-10-01 post-generation batch 07\nVerified total **33,000 / 70,402 = 46.87%**. Added **4,111** instruction-accounted bytes from `4978..59AB`, excluding enumerated alignment/data. This batch covers the character-generation controller, 5x256 palette remap construction, resource 6C/74/78/94 setup, actor finalization, shutdown wrapper, cursor callback/page state (`50C0..5878`), and first 9-byte palette search-tree insertion (`5880..59AB`). Preserve 0101h tree sentinel, 9-byte record stride, cursor depth at 73EC, page byte 73EE, 400-byte backing pages at 73EF, and the encoded zero-count resource-78 no-close behavior. Active staging remains **904 bytes**.\n')
