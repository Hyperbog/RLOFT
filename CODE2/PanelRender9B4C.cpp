#include "PanelRender9B4C.h"
namespace ravenloft::game {
static int word(const std::uint8_t*p,int n){return std::uint16_t(p[n])|(std::uint16_t(p[n+1])<<8);}
static int baseY(int ch){int q=ch/2,r=ch%2;return q*0xd3+r*0x35;}
void PanelRender9B4C::run(PanelRender9B4CState&s,const PanelRender9B4COps&o,int ch,int desc,int all){
 auto*d=s.descriptors+desc*0x21;auto icons=d+0x0d;int n=d[0x0c];
 if(!all){int y=baseY(ch),second=y+0x21;for(int i=0;i<n;++i){o.begin();s.characters[ch*0x248+0x17a]=1;o.prepare(0);o.draw(icons[i],d[4],y+d[8],s.resourceBase+*reinterpret_cast<const int*>(d),0);if(desc==5)o.draw(icons[i],d[4],second+d[8],s.resourceBase+*reinterpret_cast<const int*>(d),0);o.present(s.c050,0x113,y,0x7d,0x35);o.finish();o.step(3,0);}return;}
 for(int item=0;item<n;++item){o.begin();for(int c=0;c<4;++c){auto*p=s.characters+c*0x248;if(word(p,0x10)>=0x4000)continue;int y=baseY(c);p[0x17a]=1;o.prepare(0);o.draw(icons[item],d[4],y+d[8],s.resourceBase+*reinterpret_cast<const int*>(d),0);if(desc==5)o.draw(icons[item],d[4],y+0x21+d[8],s.resourceBase+*reinterpret_cast<const int*>(d),0);}o.present(s.c050,0x113,0,0x7d,0x140);o.finish();o.step(3,0);}
 s.characters[ch*0x248+0x17a]=1;o.prepare(0);o.present(s.c050,0x113,baseY(ch),0x7d,0x35);
}}
