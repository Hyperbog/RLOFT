#include "EntityRemove8C88.h"
namespace ravenloft::game {
void EntityRemove8C88::run(EntityRemove8C88State&s,const EntityRemove8C88Ops&o,int i){
 if(i>=s.count)o.message2F580(0);int bytes=(s.count-1-i)*0x16;if(bytes>0)std::memmove(s.entities.data()+std::size_t(i)*0x16,s.entities.data()+std::size_t(i+1)*0x16,std::size_t(bytes));--s.count;
}}
