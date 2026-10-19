// Authentic source SAVE.DAT load: compiled 07938 / shared 06CAD tail calls
// compiled original 181B4 direct-save loader, not a host-parsed substitute.
// Owner CODE.2 supplies the original initialized object-2 DGROUP, including
// the 62 serialized inventory-container descriptors at 7330.
// This is NOT a continuous DOS boot or an original DOS VGA reference.
#ifndef RLOFT_EMBEDDED_SAVE_LEVEL_GUEST
#define Session OriginalLevelGuest
#define main original_level_loader_main
#include "code2_original_level_loader.cpp"
#undef main
#undef Session
#endif
#include <array>
#include <fstream>

namespace {
using SaveLevel=OriginalLevelGuest;
struct SavedGame {
    SaveLevel& s;
    unsigned save_opens=0,level_opens=0,level_callbacks=0;
    bool error=false;
    static SavedGame* active;
    static SavedGame& live() noexcept {return *active;}
    explicit SavedGame(SaveLevel& state):s(state){
        VERIFY(!active);active=this;
    }
    ~SavedGame(){active=nullptr;}
    static bool path_is(u32 address,const char* expected) noexcept {
        auto& g=live().s.guest;
        const std::size_t len=std::char_traits<char>::length(expected);
        if(std::uint64_t(address)+len+1u>g.size())return false;
        for(std::size_t i=0;i<len;++i) {
            char c=static_cast<char>(g[address+i]);
            if(c>='a'&&c<='z')c=static_cast<char>(c-'a'+'A');
            if(c!=expected[i])return false;
        }
        return g[address+len]==0u;
    }
    static u32 open_save(u32 addr) noexcept {
        auto& t=live();
        if(addr!=0x12990u || !path_is(addr,"WORK\\SAVE.DAT")){
            t.error=true;return 0xFFFFFFFFu;
        }
        const auto r=t.s.files.open("SAVE.DAT",0u);
        if(r.carry){t.error=true;return 0xFFFFFFFFu;}
        ++t.save_opens;++t.s.opens;return r.ax;
    }
    static u32 open_level(u32 addr) noexcept {
        auto& t=live();
        if(addr!=0x819Cu || !path_is(addr,"WORK\\LEV005.DAT")){
            t.error=true;return 0xFFFFFFFFu;
        }
        const auto r=t.s.files.open("LEV005.DAT",0u);
        if(r.carry){t.error=true;return 0xFFFFFFFFu;}
        ++t.level_opens;++t.s.opens;return r.ax;
    }
    static void copy_guest(u32 dst,u32 src,u32 n) noexcept {
        auto& t=live();
        if(std::uint64_t(dst)+n>t.s.guest.size() ||
           std::uint64_t(src)+n>t.s.guest.size()){
            t.error=true;return;
        }
        (void)code2::function_30cd4(t.s.machine,dst,src,n);
    }
    static void set_heading(u32 heading) noexcept {
        auto& t=live();
        code2::function_1c0c0(t.s.machine,heading);
    }
    static void set_interrupt_ticks(u32 ticks) noexcept {
        code2::function_30738(live().s.machine,ticks);
    }
    static void set_game_ticks(u32 ticks) noexcept {
        code2::function_1e74c(live().s.machine,ticks);
    }
    static void restore_cursor(code2::s32 item,u32 known) noexcept {
        auto& t=live();
        code2::Application35Boundary b{};
        code2::function_1a344(t.s.machine,b,
                               static_cast<u32>(item),known);
    }
    static void update_daypart() noexcept {
        code2::function_31f94(live().s.machine);
    }
    static void set_environment(u32 raw) noexcept {
        code2::function_31edc(live().s.machine,
                               static_cast<code2::s32>(raw));
    }
    static void set_light(u32 raw) noexcept {
        code2::function_31f38(live().s.machine,
                               static_cast<code2::s32>(raw));
    }
    static void load_saved_level(u32 level) noexcept {
        auto& t=live();
        ++t.level_callbacks;
        if(level!=5u){t.error=true;return;}
        code2::Application32Boundary b{};
        b.call_312a0=&open_level;
        b.call_312a0_local=&SaveLevel::open_archive;
        b.call_3131c=&SaveLevel::seek;
        b.call_312ec=&SaveLevel::read_guest;
        b.call_312ec_local=&SaveLevel::read_local;
        b.call_312dc=&SaveLevel::close;
        b.call_30d30=&SaveLevel::fill;
        b.call_3104c=&SaveLevel::allocate;
        b.call_310b0=&SaveLevel::release;
        b.call_29050=&SaveLevel::graphic_bank;
        b.call_29048=&SaveLevel::graphic_mode;
        b.call_31edc=&set_environment;
        b.call_31f38=&set_light;
        b.call_31f94=&update_daypart;
        code2::function_181b4(t.s.machine,b,static_cast<code2::s32>(level));
    }
    static u32 no_object_at(code2::s32,code2::s32) noexcept {return 0u;}
    void execute(const std::vector<u8>& save,
                 const rloft::original_levels::LevelGrid& grid) {
        // Multiple owner-preserved, unmodified SAVE.DAT snapshots have
        // retail lengths 9288 and 9310. The original compiled 07938 parser
        // still controls whether the extra 22 source bytes are valid.
        VERIFY((save.size()==9288u || save.size()==9310u) &&
               grid.level==5u);
        code2::Application10Boundary b{};
        code2::Application09Boundary ui{};
        b.call_312a0=&open_save;
        b.call_3131c=&SaveLevel::seek;
        b.call_312ec=&SaveLevel::read_guest;
        b.call_312ec_local=&SaveLevel::read_local;
        b.call_312dc=&SaveLevel::close;
        b.call_3104c=&SaveLevel::allocate;
        b.call_310b0=&SaveLevel::release;
        b.call_30cd4=&copy_guest;
        b.call_1c0c0=&set_heading;
        b.call_30738=&set_interrupt_ticks;
        b.call_1a344=&restore_cursor;
        b.call_1e74c=&set_game_ticks;
        b.call_31edc=&set_environment;
        b.call_31f38=&set_light;
        b.call_181b4=&load_saved_level;
        ui.call_1e8c8=&no_object_at; // Isolated scene-object service unlinked.
        const u32 status=code2::function_07938(s.machine,b,ui);
        VERIFY(status==0u && !error && !s.failed);
        VERIFY(save_opens==1u && level_opens==1u && level_callbacks==1u);
        VERIFY(s.files.open_handles()==0u && s.opens==s.closes);
        VERIFY(code2::load_le32(s.guest.data()+0xC6C8u)==5u);
        // Never force SAVE10's specific pose, light or daypart onto
        // independent original saves. Cross-check authentic saved camera
        // bytes against the compiled 07938 output instead.
        VERIFY(code2::load_le32(s.guest.data()+0x255D0u)==
               code2::load_le32(save.data()+0x56u));
        VERIFY(code2::load_le32(s.guest.data()+0x255D4u)==
               code2::load_le32(save.data()+0x5Au));
        VERIFY(code2::load_le32(s.guest.data()+0xC658u)==
               code2::load_le32(save.data()+0x5Eu));
        VERIFY(code2::load_le32(s.guest.data()+0xC658u)<360u);
        VERIFY(code2::load_le32(s.guest.data()+0xCF4Cu)==
               code2::load_le32(save.data()+0x70u));
        // Saved placed-object count may differ across genuine owner saves.
        // Do not replace original 07938's dynamic count with SAVE10's zero.
        VERIFY(code2::load_le32(s.guest.data()+0xC038u)<=4096u);
        VERIFY(code2::load_le32(s.guest.data()+0xBF44u)==0x270Fu);
        VERIFY(std::equal(save.begin()+0x32u,save.begin()+0x52u,
                          s.guest.begin()+0x22200u));
        VERIFY(std::equal(grid.header.begin(),grid.header.end(),
                          s.guest.begin()+0x25618u));
        for(std::size_t i=0;i<grid.cells.size();++i)
            VERIFY(code2::load_le16(s.guest.data()+0x198E0u+i*2u)==grid.cells[i]);
        for(std::size_t i=0;i<4u;++i) {
            const auto saved=save.begin()+0x77u+i*0x248u;
            VERIFY(std::equal(saved,saved+16u,
                               s.guest.begin()+0x218E0u+i*0x248u));
        }
        std::cout<<"CODE2_07938_ORIGINAL_SAVE5_LOAD_AND_181B4_DIRECT_LEVEL_PASS"
                 <<" x16="<<code2::load_le32(s.guest.data()+0x255D0u)
                 <<" y16="<<code2::load_le32(s.guest.data()+0x255D4u)
                 <<" heading="<<code2::load_le32(s.guest.data()+0xC658u)
                 <<" party=4 containers=62 level_cells=4096\\n";
        std::cout<<"CODE2_CONTINUOUS_CODE1_GAME_BOOT_ACCEPTED=NO\\n";
        std::cout<<"CODE2_SOURCE_DOS_PIXEL_PARITY_ACCEPTED=NO\\n";
    }
};
SavedGame* SavedGame::active=nullptr;
}

int main(int argc,char** argv) {
    try {
        if(argc!=3)throw std::invalid_argument(
            "usage: rloft_original_code2_save_game /private/SAVE10 "
            "/private/CODE.2 (SINTAB must be in SAVE10)");
        const std::filesystem::path dir(argv[1]),original(argv[2]);
        VERIFY(std::filesystem::is_directory(dir) &&
               original.filename()=="CODE.2");
        const auto exe=rloft::original_levels::read_archive(original.string());
        constexpr std::size_t object2_source=0x75F24u;
        VERIFY(exe.size()==558689u && exe.size()>object2_source+0x7330u+620u);
        const auto save=rloft::original_levels::read_archive(
            (dir/"SAVE.DAT").string());
        const auto lev=rloft::original_levels::read_archive(
            (dir/"LEV005.DAT").string());
        const auto grid=rloft::original_levels::decode_saved_grid(lev,5u);
        auto source=std::make_unique<SaveLevel>(dir,true);
        std::fill(source->guest.begin(),source->guest.end(),0u);
        // Original CODE.2 initialized DGROUP object-2 image, followed by BSS.
        const std::size_t initialized=exe.size()-object2_source;
        std::copy_n(exe.begin()+object2_source,initialized,
                    source->guest.begin());
        VERIFY(source->guest[0x7330u]==4u &&
               source->guest[0x733Au]==8u &&
               code2::load_le32(source->guest.data()+0x7335u)==0x6FC4u);
        VERIFY(std::string(reinterpret_cast<char*>(source->guest.data()+0xC020u))=="WORK");
        constexpr char sintab[]="SINTAB";
        std::copy_n(sintab,sizeof(sintab),source->guest.begin()+0x24u);
        source->load_original_sintab();
        SavedGame gate(*source);
        gate.execute(save,grid);
        return 0;
    } catch(const std::exception& e) {
        std::cerr<<"CODE2_ORIGINAL_SOURCE_SAVE5_GATE_FAIL: "
                 <<e.what()<<"\\n";
        return 1;
    }
}
