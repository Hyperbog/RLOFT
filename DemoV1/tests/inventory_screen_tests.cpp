#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/inventory_screen.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace {
void put16(std::vector<std::uint8_t>& b,std::size_t o,std::uint16_t v){
 b[o]=static_cast<std::uint8_t>(v);b[o+1]=static_cast<std::uint8_t>(v>>8U);
}
void put32(std::vector<std::uint8_t>& b,std::size_t o,std::uint32_t v){
 for(int i=0;i<4;++i)b[o+static_cast<std::size_t>(i)]=static_cast<std::uint8_t>(v>>(i*8));
}
rloft::ObjectCatalog catalog(){
 constexpr std::size_t count=0x148;
 std::vector<std::uint8_t> b(4U+count*0x41U,0);
 put32(b,0,static_cast<std::uint32_t>(count));
 for(std::size_t i=0;i<count;++i){
   put16(b,4U+i*0x41U+0x2D,static_cast<std::uint16_t>(i%168U));
 }
 return rloft::ObjectCatalog::parse(b);
}
}

int main(){
 rloft::PartyActorPresentation actor{};
 actor.actor_index=2;
 actor.race_code=4; // dwarf => side 1
 actor.items.fill(-1);
 actor.items[0]=5;
 actor.items[4]=7;
 actor.items[12]=8;
 actor.items[13]=9;
 actor.items[14]=0x109; // generic-hand substitution
 actor.items[15]=10;
 actor.items[16]=11;
 actor.items[17]=12;
 actor.items[18]=13;
 actor.items[19]=14;
 actor.items[20]=15;

 const auto model=rloft::InventoryScreenModel::build(actor,catalog(),168);
 assert(model.actor_index==2);
 assert(model.panel_base_y==160);
 assert(model.equipment_side==1);

 // Ordinary slots: column = slot/4, row = slot%4.
 assert(model.draws[0].slot==0);
 assert(model.draws[0].x==0x129);
 assert(model.draws[0].y==165);
 assert(model.draws[0].local_ui_graphic==5);
 assert(model.draws[0].absolute_res1_id==173);
 assert(model.draws[1].slot==4);
 assert(model.draws[1].x==0x129+0x22);
 assert(model.draws[1].y==165);

 bool saw_ring=false,saw_special_hand=false,saw_quiver=false,saw_bottom=false;
 for(const auto& d:model.draws){
   if(d.slot==12){
     saw_ring=true;
     assert(d.x==0xF8&&d.y==163);
     assert(d.clear_32x16_before_draw);
   }
   if(d.slot==14){
     saw_special_hand=true;
     assert(d.rendered_item_id==6);
     assert(d.local_ui_graphic==7); // object 6 graphic 6 + 1
     assert(d.x==0x72+0x12);
   }
   if(d.slot==19){
     saw_quiver=true;
     assert(d.rendered_item_id==6);
     assert(d.local_ui_graphic==6);
     assert(d.x==0x72+0x15);
   }
   if(d.role==rloft::InventoryDrawRole::BracerBottomMirrored){
     saw_bottom=true;
     assert(d.bottom_align_bias.has_value());
     assert(*d.bottom_align_bias==0x50);
     assert(d.temporarily_flip_sprite);
   }
 }
 assert(saw_ring&&saw_special_hand&&saw_quiver&&saw_bottom);

 // Original CODE.2 10A70 4B9EF RES2 graphic ID =9+the selected
 // subtype (0..6). The panel under the nested black item cells
 // is original RES2 artwork, not a generic invented gray frame.
 for(std::uint8_t subtype=0;subtype<=6U;++subtype) {
     assert(rloft::InventoryScreenModel::
         original_container_res2_graphic(subtype)==
         9+static_cast<int>(subtype));
 }
 bool invalid=false;
 try {
     (void)rloft::InventoryScreenModel::
         original_container_res2_graphic(7U);
 }catch(const std::out_of_range&) {
     invalid=true;
 }
 assert(invalid);

 // Original CODE.2 10AA7/10ACB has two distinct RES2 banks
 // for one container subtype: phases 1 and 2 have consecutive
 // graphics beginning at 0x14+2*subtype (row C4h); phase3
 // returns to 9+subtype (row CEh). Check every shipped subtype.
 for(std::uint8_t subtype=0U;subtype<=6U;++subtype) {
     const auto early=rloft::InventoryScreenModel::
         original_container_transition_res2_graphic(subtype,1U);
     const auto late=rloft::InventoryScreenModel::
         original_container_transition_res2_graphic(subtype,2U);
     assert(early==0x14+2*static_cast<int>(subtype));
     assert(late==early+1);
     assert(rloft::InventoryScreenModel::
         original_container_res2_graphic(subtype)==
         9+static_cast<int>(subtype));
 }
 for(const auto phase:{0U,3U,4U}) {
     bool rejected=false;
     try {
         (void)rloft::InventoryScreenModel::
             original_container_transition_res2_graphic(
                 0U,static_cast<std::uint8_t>(phase));
     } catch(const std::out_of_range&) { rejected=true; }
     assert(rejected);
 }
 bool rejected_subtype=false;
 try {
     (void)rloft::InventoryScreenModel::
         original_container_transition_res2_graphic(7U,1U);
 } catch(const std::out_of_range&) { rejected_subtype=true; }
 assert(rejected_subtype);
}
