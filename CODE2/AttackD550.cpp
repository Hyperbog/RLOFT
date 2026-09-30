#include <cstdint>
#include <functional>
namespace ravenloft::game {
struct AttackD550Ops{
 std::function<std::uint16_t(int)> entityType;
 std::function<std::uint16_t(int)> entityFlags;
 std::function<std::uint16_t(int)> characterState;
 std::function<int(int)> checkCEE4;
 std::function<void(int,int)> damageCD44;
 std::function<void(int,int,int,int,int)> create8A80;
 std::function<std::int32_t()> worldX,worldY;
 std::function<int(int,const void*)> gateD4E8;
 std::function<int(int,const void*,const void*)> hitC9A0;
 std::function<int(int,int,int,int)> dice;
 std::function<void(int)> levelLossD1E0;
 std::function<void(int,int)> effect1202C;
 std::function<std::int32_t()> clock19CC4;
 std::function<void(int,int,int,int,int)> effectE284;
 std::function<void(std::int32_t,int,int,int)> event28110;
 std::function<std::int32_t(int)> resource258C0;
 std::function<void()> partyGateCCF4;
};
void attackD550(const AttackD550Ops&o,int entity,const void*meta){
 const auto type=o.entityType(entity);
 if(type==0x0f){for(int ch=0;ch<4;++ch)if(o.characterState(ch)<0x4000u&&!o.checkCEE4(ch))o.damageCD44(ch,0x7fffffff);o.event28110(o.resource258C0(0x3f),0xff,0,1);return;}
 if(type==8 && (o.entityFlags(entity)&0xc000u)==0x8000u){unsigned dir=(o.entityFlags(entity)>>13)&7u;int angle=(8-static_cast<int>(dir))*45;if(angle>=360)angle-=360;o.create8A80(0,angle,-1,o.worldY(),o.worldX());return;}
 // Ordinary target selection, hit resolution and metadata-driven effects continue in original order.
 int target=0;for(int n=0;n<4;++n){if(!o.gateD4E8(target,meta))break;target=(target+1)&3;}
 if(o.gateD4E8(target,meta))return;
 if(!o.hitC9A0(target,nullptr,meta)){o.event28110(o.resource258C0(9),0xff,0,1);return;}
 const auto*m=static_cast<const std::uint8_t*>(meta);
 int damage=o.dice(m[0x1e],m[0x1f],m[0x20],0);
 if(m[0x3a]&2u){if(!o.checkCEE4(target))o.levelLossD1E0(target);}
 o.damageCD44(target,damage);o.partyGateCCF4();
}
}
