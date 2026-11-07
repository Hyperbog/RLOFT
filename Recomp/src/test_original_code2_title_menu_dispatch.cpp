// Bounded CODE.2 20440 -> 19888 -> 192CC/19464 startup/menu chain.
// Input and DOS callbacks are tests only; no rendered art or playable UI.
#include "../../Code2/Code2.cpp"
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {
using u8=std::uint8_t;
using u32=std::uint32_t;
void require(bool ok,const char* why) {
    if(!ok)throw std::runtime_error(why);
}
struct Gate {
    std::vector<u8> memory=std::vector<u8>(0x40000u,0u);
    code2::Machine machine{};
    code2::Application33Boundary ui{};
    code2::Application34Boundary frontend{};
    std::vector<u32> art{};
    unsigned polls=0,blits=0,restores=0,displays=0,overlays=0;
    unsigned titles=0,mouse_inits=0,mouse_updates=0,button_calls=0;
    unsigned palette_calls=0,new_game_calls=0,frontends=0;
    unsigned title_palette_requests=0,chars_checks=0;
    static Gate* active;
    static Gate& self() noexcept {return *active;}
    Gate() {
        require(active==nullptr,"fixture is not reentrant");
        active=this;
        machine.data=memory.data();
        // Recorded original INIT UI resource base and mouse coordinate
        // fixture. No replacement menu logic, images or text are created.
        code2::store_le32(memory.data()+0x258DCu,15u);
        code2::store_le16(memory.data()+0x9E31u,294u);
        code2::store_le16(memory.data()+0x9E33u,25u);
        // Original CODE.2 object-2 threshold dwords at 8298+12*i.
        code2::store_le32(memory.data()+0x8298u,0x11Du);
        code2::store_le32(memory.data()+0x8298u+12u,0x12Cu);
        code2::store_le32(memory.data()+0x8298u+24u,0x13Fu);
        code2::store_le32(memory.data()+0x8298u+36u,0x18Fu);
        code2::store_le32(memory.data()+0x8290u+12u,0x11Fu);
        code2::store_le32(memory.data()+0x8294u+12u,0x2Fu);
        // Genuine movie-bypass branch through (F050)+72.
        code2::store_le32(memory.data()+0xF050u,0x400u);
        memory[0x472u]=1u;
        memory[0xC02Cu]=2u;memory[0xC02Bu]=1u;
        frontend.app33=&ui;
        frontend.call_2f844=[]() noexcept {
            code2::function_2f844(self().machine);
        };
        ui.call_1a0b0=[]() noexcept {++self().mouse_inits;};
        ui.call_1a288=[]() noexcept {++self().mouse_updates;};
        ui.call_1aacc=[]() noexcept {++self().restores;};
        ui.call_1a1f4=[]() noexcept -> u32 {
            return self().polls++==0u?0u:1u;
        };
        ui.call_291b0=[](u32 page) noexcept {
            if(page>1u)std::terminate();
            ++self().displays;
        };
        ui.call_2e510=[](u32,u32,u32 resource,u32 bank) noexcept {
            if(bank!=0u)std::terminate();
            self().art.push_back(resource);
        };
        ui.call_3060c=[](u32 y,u32 x,u32 ptr,u32 font) noexcept {
            if(y!=0u||x!=0u||ptr!=0x82C0u||font!=1u)std::terminate();
            ++self().titles;
        };
        ui.call_1ea78=[]() noexcept {++self().overlays;};
        ui.call_1a9f8=[]() noexcept {};
        ui.call_2aa9c=[](u32 src,u32 dest,u32 count) noexcept {
            if(src!=0u||dest!=0x759Cu||count!=0x880Du)
                std::terminate();
            ++self().blits;
        };
        ui.call_2f29c=[](u32 item,u32 bank) noexcept -> u32 {
            if(item!=15u+0x2Fu||bank!=0u)std::terminate();
            return 0x30000u;
        };
        ui.call_30cd4=[](u32 dest,u32 src,u32 count) noexcept {
            if(dest!=0x880Du||src!=0x30000u||count!=0x1614u)
                std::terminate();
            ++self().blits;
        };
        ui.call_32220=[](u32,u32,u32,u32,u32,u32) noexcept {
            ++self().button_calls;
        };
        ui.call_20294=[]() noexcept {++self().displays;};
        ui.call_325b0=[](u32 target,u32 start,u32 bank,
                            u32 name,u32 count) noexcept {
            if(target!=0xC054u||start!=0u||bank!=12u||
               name!=0xCF4u||count!=0x100u)std::terminate();
            ++self().title_palette_requests;
        };
        ui.call_29234=[](u32 p) noexcept {
            if(p!=0xC054u)std::terminate();
            ++self().palette_calls;
        };
        ui.call_7b94=[]() noexcept -> u32 {
            ++self().new_game_calls;
            return 0u; // Original character-creation return=0.
        };
    }
    ~Gate(){active=nullptr;}
    void run() {
        code2::Application46Boundary boot{};
        boot.call_31218=[](u32 name,u32 flags) noexcept -> u32 {
            if(name!=0x1140u||flags!=0u)std::terminate();
            ++self().chars_checks;
            return 0u; // Synthetic test: CHARS missing.
        };
        boot.call_312a0=[](u32 name) noexcept -> u32 {
            if(name!=0x1148u)std::terminate();
            return 0u; // T handle is unused in movie-bypass case.
        };
        boot.call_19888=[]() noexcept {
            ++self().frontends;
            code2::function_19888(self().machine,self().frontend);
        };
        code2::function_20440(machine,boot,0u);
        const auto base=15u;
        require(frontends==1u && chars_checks==1u,
                "20440 did not dispatch missing-CHARS 19888");
        require(title_palette_requests==1u,
                "19888 omitted original COLORS bank-12 request");
        require(mouse_inits==1u,"19464 original mouse initialization");
        require(polls==2u,"19464 original hover-then-click polling");
        require(new_game_calls==1u,"19464 selection 1 did not call 7B94");
        require(art.size()==7u,"192CC title resources + mouse highlight");
        for(u32 i=0;i<5u;++i)
            require(art[i]==base+0x52u+i,"192CC source image order changed");
        require(art[5]==base+0x55u && art[6]==base+0x58u,
                "19464 selected-row source artwork changed");
        require(titles==1u,"192CC original title text missing");
        require(blits==3u,"19378/1942C original cursor backup calls");
        require(overlays>=2u,"192CC/19464 original VGA present callbacks");
        require(button_calls==1u,"19464 original modal clear missing");
        require(palette_calls==2u,"19888/19464 palette commits missing");
        require(memory[0xC02Cu]==2u && memory[0xC02Bu]==1u,
                "19464 source new-game state handoff mismatched");
        std::cout<<"CODE2_ORIGINAL_20440_19888_19464_TITLE_MENU_CHAIN_PASS"
                 <<" movie_branch=SKIPPED_BY_ORIGINAL_STATE"
                 <<" original_title_resources=5"
                 <<" original_mouse_highlight=YES"
                 <<" original_new_game_call_7B94=YES"
                 <<" original_state_handoff=YES"
                 <<" original_ART_pixels=NOT_CONNECTED"
                 <<" physical_input=NOT_CONNECTED"
                 <<" live_20440_handoff=NOT_CONNECTED\n";
    }
};
Gate* Gate::active=nullptr;
}
int main() {
    try {Gate gate;gate.run();return 0;}
    catch(const std::exception& e) {
        std::cerr<<"CODE2_ORIGINAL_19464_GATE_FAIL: "<<e.what()<<'\n';
        return 1;
    }
}
