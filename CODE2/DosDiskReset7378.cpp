#include "DosDiskReset7378.h"
namespace ravenloft::game {
void DosDiskReset7378::run(DosDiskReset7378State&s,const DosDiskReset7378Ops&o){s.status12B70=0;o.call33971(&s.status12B70,0x3b0,0x0b);o.dosInt21Ah13(&s.status12B70);}
}
