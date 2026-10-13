#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#include "rloft_native/native_win32_startup.hpp"
#include "rloft_native/native_startup_visuals.hpp"
#include "rloft_native/native_media.hpp"
#include "rloft_native/native_party_start.hpp"
#include "rloft_native/native_win32_gpu_exploration.hpp"
#include "rloft_native/native_presentation_timing.hpp"
#include "rloft/startup_presenter.hpp"
#include "rloft/anm_cinematic.hpp"
#include "rloft/boot_flow.hpp"
#include "rloft/character_creation_presenter.hpp"
#include "rloft/character_creation_flow.hpp"
#include "rloft/character_transfer.hpp"
#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <optional>
#include <utility>
#include <stdexcept>
#include <string>
#include <vector>
namespace rloft_native {
namespace {
constexpr UINT_PTR kTick=31;
constexpr unsigned kWidth=320,kHeight=400;
constexpr int kMenu=18;
constexpr int kCharacterOpening=19,kCharacterChoice=20,kCharacterTransferred=21;
constexpr std::array<unsigned,4> kLogoWaitMs{
    120U*1000U/18U,106U*1000U/18U,
    121U*1000U/18U,55U*1000U/18U};
struct Preview {
    NativeStartupVisualSource source;
    NativeMedia original_media;
    rloft::StartupPresenter presenter;
    rloft::BootFlow boot;
    rloft::CharacterCreationFlow creation;
    std::unique_ptr<rloft::CharacterCreationPresenter> character;
    std::unique_ptr<rloft::CharacterStatsModel> stats;
    std::array<std::optional<rloft::CharacterStatsModel>,2> actor_stats{};
    std::optional<rloft::CharacterTransferImage> completed_party;
    HWND world_window{};
    bool stats_editor{},final_decision{},name_entry{},existing_actor_decision{};
    bool name_cursor_on{true};
    int selected_stat{-1},existing_actor{-1},portrait_hover{-1};
    std::string pending_name;
    int character_frame{};
    ULONGLONG code1_started{};
    ULONGLONG name_blink_at{};
    std::unique_ptr<rloft::AnmCinematic> cine00;
    std::unique_ptr<rloft::AnmCinematic> cine12;
    unsigned cinematic_wait_ms{14U};
    unsigned cinematic_audio_events{};
    HWND hwnd{};
    bool ownership_transferred{};
    int phase{};
    int selection{-1};
    ULONGLONG phase_started{};
    rloft::Palette6 palette{};
    std::vector<std::uint8_t> bgra;
    explicit Preview(const NativeMedia& media)
        :source(media),original_media(media),presenter(source),
         bgra(std::size_t(kWidth)*kHeight*4U) {
        // This window inspects the original no-CHARS cold presentation
        // only. It never creates a fabricated party or bypasses CODE.1.
        boot.boot();
        boot.finish_code2_startup(false,false);
        presenter.first_logo();
        palette=presenter.palette14();
        // The shipped CD source stores ANM files inside game.gog or the
        // extracted CD RLOFT folder. A loose DOS-only installation may not.
        if(media.contains("CINE00.ANM") && media.contains("CINE12.ANM")) {
            cine00=std::make_unique<rloft::AnmCinematic>(
                media.read("CINE00.ANM"),0,source);
            cine12=std::make_unique<rloft::AnmCinematic>(
                media.read("CINE12.ANM"),12,source);
            phase=-2;
            render_cinematic_frame();
        } else refresh();
    }
    void refresh() {
        const auto& pixels=phase>=kCharacterOpening?
            character->framebuffer().pixels() : phase<0?
            (phase==-2?cine00->framebuffer().pixels():cine12->framebuffer().pixels())
            :presenter.framebuffer().pixels();
        if(pixels.size()!=std::size_t(kWidth)*kHeight)
            throw std::runtime_error("original CODE.2 menu framebuffer size invalid");
        for(std::size_t i=0;i<pixels.size();++i) {
            const auto at=std::size_t(pixels[i])*3U;
            const auto out=i*4U;
            const auto expand=[](std::uint8_t v) {
                return std::uint8_t((unsigned(v)*255U+31U)/63U);
            };
            bgra[out+0U]=expand(palette.rgb[at+2U]);
            bgra[out+1U]=expand(palette.rgb[at+1U]);
            bgra[out+2U]=expand(palette.rgb[at+0U]);
            bgra[out+3U]=255U;
        }
        if(hwnd)InvalidateRect(hwnd,nullptr,FALSE);
    }
    void render_cinematic_frame() {
        auto& current=phase==-2?*cine00:*cine12;
        if(current.finished()) {
            if(phase==-2) {
                phase=-1;
                cine12->reset();
                render_cinematic_frame();
            } else {
                select_phase(0);
            }
            return;
        }
        const auto frame=current.render_next();
        cinematic_audio_events+=unsigned(frame.audio.size());
        // Original DOS BIOS PIT 1193182/65536 = approximately 18.2065 Hz.
        // ANM type-3/8 frame overrides are already computed by the shared
        // source decoder. Music/VOC playback is a separate open A1 item.
        cinematic_wait_ms=frame.wait_ticks>0?
            unsigned((double(frame.wait_ticks)*1000.0)/
                rloft::StartupPresenter::kPitHz+0.5):14U;
        phase_started=GetTickCount64();
        palette=current.palette();
        refresh();
    }
    void select_phase(int next) {
        phase=next;
        phase_started=GetTickCount64();
        if(phase==0){presenter.first_logo();palette=presenter.palette14();}
        else if(phase==1){presenter.second_logo();palette=presenter.palette14();}
        else if(phase==2){presenter.third_logo();palette=presenter.palette9();}
        else if(phase==3){presenter.title_background();palette=presenter.palette12();}
        else if(phase>=4 && phase<kMenu) {
            presenter.menu_animation_frame(phase-3);
            palette=presenter.palette12();
        } else if(phase==kMenu) {
            if(boot.snapshot().stage==rloft::BootStage::ColdStartPresentation)
                boot.finish_cold_start_presentation();
            presenter.menu_frame(selection);palette=presenter.palette12();
        } else throw std::runtime_error("original startup preview phase outside bounded range");
        refresh();
    }
    void choose_startup_menu(int chosen) {
        if(phase!=kMenu||chosen<0||chosen>=4)
            throw std::logic_error("original startup menu selection invalid");
        if(chosen==rloft::BootFlow::kCharacterWorkflowMenuIndex) {
            const auto previous=boot;
            boot.select_startup_menu(chosen);
            try {
                begin_character();
            } catch(...) {
                // Incomplete original source assets must not strand the
                // supervisor in CODE.2's exit stage on a menu retry.
                boot=previous;
                character.reset();
                phase=kMenu;
                presenter.menu_frame(selection);
                palette=presenter.palette12();
                refresh();
                throw;
            }
            return;
        }
        boot.select_startup_menu(chosen);
        if(chosen==3) {
            // CODE.2 menu entry 3 returns original supervisor status 0.
            boot.supervisor_dispatch();
            DestroyWindow(hwnd);
        } else {
            // Entries 0/1 have separate original CODE.2 menu handlers.
            // Do not invent saves/loads before their source is integrated.
            MessageBoxW(hwnd,
                L"The original CODE.2 menu selection is recognized.\n"
                L"This menu handler is not implemented in the native game yet.",
                L"Original menu handler pending",MB_OK|MB_ICONINFORMATION);
        }
    }
    void render_character_screen() {
        if(!character)throw std::logic_error("CODE.1 source presenter is absent");
        if(name_entry) {
            if(!stats)throw std::logic_error("CODE.1 name entry lacks stats");
            character->name_entry_screen(creation,*stats,pending_name,name_cursor_on);
        } else if(final_decision) {
            if(!stats)throw std::logic_error("CODE.1 final choice lacks stats");
            character->final_decision_screen(creation,*stats);
        } else if(existing_actor_decision) {
            if(existing_actor<0||existing_actor>=2||
               !actor_stats[std::size_t(existing_actor)])
                throw std::logic_error("CODE.1 existing actor missing saved statistics");
            character->existing_actor_decision_screen(
                creation,existing_actor,*actor_stats[std::size_t(existing_actor)]);
        } else character->screen(
            creation,stats.get(),selected_stat,stats_editor,portrait_hover);
        palette=character->palette();
        refresh();
    }
    [[nodiscard]] std::uint32_t original_tick() const noexcept {
        // INT8 at 1193182 / 65536 Hz, relative to CODE.1 startup epoch.
        const auto elapsed=GetTickCount64()-code1_started;
        return std::uint32_t((elapsed*1193182ULL)/(65536ULL*1000ULL));
    }
    void handle_name_key(unsigned key) {
        if(!name_entry)return;
        if(key==13U) {
            const int actor=creation.draft().actor;
            if(actor<0||actor>=2||!stats)
                throw std::logic_error("CODE.1 cannot accept nameless actor state");
            actor_stats[std::size_t(actor)]=*stats;
            creation.finish_current_actor(true,pending_name);
            stats.reset();
            name_entry=false;
            final_decision=false;
            stats_editor=false;
            pending_name.clear();
            selected_stat=-1;
        } else if(key==8U) {
            if(!pending_name.empty())pending_name.pop_back();
        } else {
            // CODE.1 function_2DFC accepts ASCII uppercase, digits, space;
            // the original RES0 font determines the strict pixel-width cap.
            if(key>='a'&&key<='z')key-='a'-'A';
            const bool valid=(key>='A'&&key<='Z')||
                (key>='0'&&key<='9')||key==' ';
            if(!valid||pending_name.size()>=15U)return;
            auto candidate=pending_name+char(key);
            const auto font=source.res0_font(0);
            unsigned width=0U;
            for(const auto ch:candidate)
                width+=font.widths[std::uint8_t(ch)];
            if(width>=0x30U)return;
            pending_name=std::move(candidate);
        }
        name_cursor_on=true;
        name_blink_at=GetTickCount64()+500U;
        render_character_screen();
    }
    void enter_first_level() {
        if(!completed_party)
            throw std::logic_error("CODE.2 world entry requires source CHARS");
        if(world_window&&IsWindow(world_window)) {
            SetForegroundWindow(world_window);
            return;
        }
        const auto stage=boot.snapshot().stage;
        if(stage!=rloft::BootStage::Code2Startup &&
           stage!=rloft::BootStage::Exploration)
            throw std::logic_error("CODE.2 party world session cannot begin here");
        // The GPU renderer validates the real Level22 cell and original
        // source party before the supervisor advances to gameplay.
        // Reopen after the child closes is an explicit *fresh* session:
        // native state persistence remains an open A4 requirement.
        world_window=open_native_gpu_world_session(
            hwnd,original_media,*completed_party);
        if(stage==rloft::BootStage::Code2Startup)
            boot.finish_code2_startup(true,false);
        if(boot.snapshot().stage!=rloft::BootStage::Exploration)
            throw std::logic_error("CODE.2 first-party world stage not reached");
    }
    void complete_character_transfer() {
        if(!creation.both_present()||!creation.completed())
            throw std::logic_error("CODE.1 transfer before both actors finished");
        if(boot.snapshot().stage!=rloft::BootStage::CharacterCreation)
            throw std::logic_error("CODE.1 transfer requires active supervisor state");
        std::array<rloft::CharacterTransferActor,2> actors{};
        for(std::size_t i=0;i<actors.size();++i) {
            if(!actor_stats[i])
                throw std::logic_error("CODE.1 transfer actor statistics missing");
            actors[i].draft=creation.actor(int(i));
            actors[i].stats=actor_stats[i]->value();
        }
        // Original 90 family/race equipment rows and OBJECTS.DAT conflict
        // bits drive the *same* shared serializer as DemoV1. Native keeps
        // the resulting 0x920 bytes in memory; never writes into game media.
        const auto tables=rloft::CharacterTransferTables::from_original_bytes(
            original_media.read("CODE.1"),
            original_media.read("OBJECTS.DAT"));
        completed_party=rloft::build_character_transfer(actors,tables);
        if(completed_party->size()!=0x920U)
            throw std::logic_error("CODE.1 CHARS has incorrect byte length");
        boot.finish_character_creation(rloft::LegacyResult::ContinueModuleCycle);
        boot.supervisor_dispatch();
        if(!boot.snapshot().chars_present||
           boot.snapshot().stage!=rloft::BootStage::Code2Startup)
            throw std::logic_error("CODE.1 status-10 CHARS dispatch failed");
        phase=kCharacterTransferred;
        refresh();
        // First actual source-party path (renderer/gameplay still partial):
        // launch Level22 at original 4.5/4.5, facing 270 degrees.
        // If graphics fail, keep the exact CHARS bytes for Enter retry.
        enter_first_level();
    }
    void handle_stats_key(bool increase) {
        if(phase!=kCharacterChoice||name_entry||final_decision||
           existing_actor_decision||
           creation.state()!=rloft::CharacterCreationState::StatsReview||
           !stats)return;
        // CODE.1 043C scans keypad + and - while in the original
        // statistics controller. In editor mode the same keys adjust.
        if(stats_editor) {
            if(selected_stat>=0)
                stats->adjust(creation.draft(),selected_stat,increase);
        } else if(increase) {
            stats->generate(creation.draft());
        } else {
            stats_editor=true;
            selected_stat=-1;
        }
        render_character_screen();
    }
    void handle_character_click(int row,int column) {
        if(phase!=kCharacterChoice||name_entry)return;
        const auto hit_state=(final_decision||existing_actor_decision)?
            rloft::CharacterCreationState::ActorChoice:creation.state();
        const int event=rloft::CharacterCreationFlow::hit_test(
            hit_state,row,column);
        if(event<0)return;
        if(final_decision) {
            if(event==3) {
                name_entry=true;
                pending_name.clear();
                name_cursor_on=true;
                name_blink_at=GetTickCount64()+500U;
            } else if(event==4) {
                creation.finish_current_actor(false);
                stats.reset();
                final_decision=false;
                stats_editor=false;
                selected_stat=-1;
            } else return;
            render_character_screen();
            return;
        }
        if(existing_actor_decision) {
            if(event!=3&&event!=4)return;
            creation.resolve_existing_actor(existing_actor,event==3);
            if(event==4)actor_stats[std::size_t(existing_actor)].reset();
            existing_actor=-1;
            existing_actor_decision=false;
            render_character_screen();
            return;
        }
        const auto state=creation.state();
        if(state==rloft::CharacterCreationState::StatsReview) {
            if(!stats)throw std::logic_error("CODE.1 statistics were not generated");
            if(stats_editor) {
                if(event==0) {
                    stats_editor=false;selected_stat=-1;
                } else if(event==8) {
                    const int index=(row-0x60)/10;
                    selected_stat=(row>=0x60&&(index<6||index==10))?
                        index:-1;
                } else if((event==7||event==9)&&selected_stat>=0) {
                    stats->adjust(creation.draft(),selected_stat,event==7);
                }
            } else if(event==0) {
                creation.apply_event(0);stats.reset();
            } else if(event==7) {
                stats->generate(creation.draft());
            } else if(event==8) {
                final_decision=true;
            } else if(event==9) {
                stats_editor=true;selected_stat=-1;
            }
            render_character_screen();
            return;
        }
        if(state==rloft::CharacterCreationState::ActorChoice &&
           (event==3||event==4) && creation.actor_present(event-3)) {
            existing_actor=event-3;
            existing_actor_decision=true;
            render_character_screen();
            return;
        }
        if(!creation.apply_event(event))return;
        portrait_hover=-1;
        if(creation.state()==rloft::CharacterCreationState::StatsReview) {
            if(!stats)stats=std::make_unique<rloft::CharacterStatsModel>(source);
            stats->seed(original_tick());
            stats->generate(creation.draft());
            stats_editor=false;selected_stat=-1;
        }
        if(creation.completed()) {
            complete_character_transfer();
            return;
        }
        if(creation.aborted()) {
            boot.finish_character_creation(rloft::LegacyResult::OrdinaryTermination);
            boot.supervisor_dispatch();
            DestroyWindow(hwnd);
            return;
        }
        render_character_screen();
    }
    void begin_character() {
        // Real source presentation begins only after the CODE.2 supervisor
        // dispatched the selected character workflow to CODE.1.
        auto prepared=std::make_unique<rloft::CharacterCreationPresenter>(source);
        prepared->opening_frame(0); // validate original art before dispatch
        boot.supervisor_dispatch();
        character=std::move(prepared);
        character_frame=0;
        code1_started=GetTickCount64();
        phase=kCharacterOpening;
        palette=character->palette();
        phase_started=GetTickCount64();
        refresh();
    }
    void advance() {
        if(phase==kCharacterChoice) {
            if(name_entry&&GetTickCount64()>=name_blink_at) {
                name_blink_at=GetTickCount64()+500U;
                name_cursor_on=!name_cursor_on;
                render_character_screen();
            }
            return;
        }
        if(phase==kCharacterTransferred||phase==kMenu)return;
        const auto elapsed=GetTickCount64()-phase_started;
        if(phase==kCharacterOpening) {
            // Source CODE.1 opening is 70 original authored frames.
            // 14ms per frame made it finish in ~1 second; an
            // approximately 18.2 Hz PIT-like host pacing is the
            // deliberately conservative timing fallback.
            if(native_presentation_frame_due(
                   GetTickCount64(),phase_started,
                   kNativeLegacyAnimationFrameMs)) {
                ++character_frame;
                phase_started=GetTickCount64();
                if(character_frame<rloft::CharacterCreationPresenter::kOpeningFrameCount)
                    character->opening_frame(character_frame);
                else {
                    phase=kCharacterChoice;
                    render_character_screen();
                }
                if(phase==kCharacterOpening) {
                    palette=character->palette();
                    refresh();
                }
            }
            return;
        }
        if(phase<0) {
            if(elapsed>=cinematic_wait_ms)render_cinematic_frame();
            return;
        }
        const std::uint64_t wait=phase<4?
            kLogoWaitMs[std::size_t(phase)]:kNativeLegacyAnimationFrameMs;
        if(native_presentation_frame_due(GetTickCount64(),phase_started,wait))
            select_phase(phase+1);
    }
    void skip_to_menu() {
        if(phase<kMenu && boot.snapshot().stage==rloft::BootStage::ColdStartPresentation)
            boot.skip_cold_start_presentation();
        select_phase(kMenu);
    }
};
RECT image_rect(HWND hwnd) {
    RECT client{};GetClientRect(hwnd,&client);
    const int width=std::max(1,int(client.right));
    const int height=std::max(1,int(client.bottom)-66);
    const int scale=std::max(1,std::min(width/int(kWidth),height/int(kHeight)));
    const int w=int(kWidth)*scale,h=int(kHeight)*scale;
    return {(width-w)/2,24+(height-h)/2,(width+w)/2,24+(height+h)/2};
}
void report_failure(HWND hwnd,const char* what) {
    MessageBoxA(hwnd,what,
        "Original CODE.2 startup visual error (game data untouched)",
        MB_OK|MB_ICONERROR);
}
LRESULT CALLBACK wnd_proc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp) {
    auto* session=reinterpret_cast<Preview*>(GetWindowLongPtrW(hwnd,GWLP_USERDATA));
    if(msg==WM_NCCREATE) {
        auto* create=reinterpret_cast<CREATESTRUCTW*>(lp);
        session=static_cast<Preview*>(create->lpCreateParams);
        if(!session)return FALSE;
        session->hwnd=hwnd;
        SetWindowLongPtrW(hwnd,GWLP_USERDATA,
            reinterpret_cast<LONG_PTR>(session));
        return TRUE;
    }
    if(!session)return DefWindowProcW(hwnd,msg,wp,lp);
    switch(msg) {
    case WM_CREATE:
        session->phase_started=GetTickCount64();
        // Poll input/display at 16ms; animation advances by its own
        // source/PIT-like schedule, never once per timer event.
        if(!SetTimer(hwnd,kTick,
                     static_cast<UINT>(kNativeWin32PollIntervalMs),nullptr))
            return -1;
        return 0;
    case WM_TIMER:
        if(wp==kTick) {
            try {session->advance();}
            catch(const std::exception& e) {
                KillTimer(hwnd,kTick);report_failure(hwnd,e.what());
            }
            return 0;
        }
        break;
    case WM_KEYDOWN:
        if(wp==VK_ESCAPE){DestroyWindow(hwnd);return 0;}
        if(session->phase==kMenu) {
            if(wp==VK_UP||wp==VK_DOWN) {
                const int old=session->selection;
                const int delta=wp==VK_DOWN?1:-1;
                session->selection=old<0?0:(old+delta+4)%4;
                try {
                    session->presenter.menu_frame(session->selection);
                    session->refresh();
                } catch(const std::exception& e){report_failure(hwnd,e.what());}
                return 0;
            }
            if(wp==VK_RETURN && session->selection>=0) {
                try{session->choose_startup_menu(session->selection);}
                catch(const std::exception& e){report_failure(hwnd,e.what());}
                return 0;
            }
        }
        if(session->phase==kCharacterChoice &&
           (wp==VK_ADD||wp==VK_SUBTRACT)) {
            try{session->handle_stats_key(wp==VK_ADD);}
            catch(const std::exception& e){report_failure(hwnd,e.what());}
            return 0;
        }
        if(wp==VK_RETURN && session->phase==kCharacterTransferred) {
            try{session->enter_first_level();}
            catch(const std::exception& e){report_failure(hwnd,e.what());}
            return 0;
        }
        if(wp==VK_SPACE || wp==VK_RETURN) {
            if(session->phase<kMenu) {
                try{session->skip_to_menu();}
                catch(const std::exception& e){report_failure(hwnd,e.what());}
            }
            return 0;
        }
        break;
    case WM_CHAR:
        if(session->phase==kCharacterChoice&&session->name_entry) {
            try{session->handle_name_key(unsigned(wp));}
            catch(const std::exception& e){report_failure(hwnd,e.what());}
            return 0;
        }
        break;
    case WM_MOUSEMOVE:
        if(session->phase==kCharacterChoice &&
           !session->name_entry && !session->final_decision &&
           !session->existing_actor_decision &&
           session->creation.state()==
               rloft::CharacterCreationState::PortraitChoice) {
            const RECT bounds=image_rect(hwnd);
            const int x=static_cast<short>(LOWORD(lp));
            const int y=static_cast<short>(HIWORD(lp));
            int next=-1;
            if(x>=bounds.left&&x<bounds.right&&
               y>=bounds.top&&y<bounds.bottom) {
                const int column=(x-bounds.left)*int(kWidth)/
                    (bounds.right-bounds.left);
                const int row=(y-bounds.top)*int(kHeight)/
                    (bounds.bottom-bounds.top);
                const int event=rloft::CharacterCreationFlow::hit_test(
                    session->creation.state(),row,column);
                if(event>=1&&event<=6&&
                   session->creation.available_choices()[std::size_t(event-1)])
                    next=event-1;
            }
            if(session->portrait_hover!=next) {
                session->portrait_hover=next;
                try{session->render_character_screen();}
                catch(const std::exception& e){report_failure(hwnd,e.what());}
            }
            return 0;
        }
        if(session->phase==kMenu) {
            const RECT bounds=image_rect(hwnd);
            const int x=static_cast<short>(LOWORD(lp));
            const int y=static_cast<short>(HIWORD(lp));
            int next=-1;
            if(x>=bounds.left && x<bounds.right &&
               y>=bounds.top && y<bounds.bottom)
                next=rloft::StartupPresenter::menu_selection_from_y(
                    (y-bounds.top)*int(kHeight)/(bounds.bottom-bounds.top));
            if(next!=session->selection) {
                session->selection=next;
                try {session->presenter.menu_frame(next);session->refresh();}
                catch(const std::exception& e){report_failure(hwnd,e.what());}
            }
        }
        return 0;
    case WM_LBUTTONDOWN:
        if(session->phase==kMenu || session->phase==kCharacterChoice) {
            try {
                // Use this click's coordinates, never stale WM_MOUSEMOVE hover.
                const RECT bounds=image_rect(hwnd);
                const int x=static_cast<short>(LOWORD(lp));
                const int y=static_cast<short>(HIWORD(lp));
                if(x<bounds.left||x>=bounds.right||
                   y<bounds.top||y>=bounds.bottom)return 0;
                const int column=(x-bounds.left)*int(kWidth)/
                                 (bounds.right-bounds.left);
                const int row=(y-bounds.top)*int(kHeight)/
                              (bounds.bottom-bounds.top);
                if(session->phase==kCharacterChoice) {
                    session->handle_character_click(row,column);
                    return 0;
                }
                const int selected=rloft::StartupPresenter::
                    menu_selection_from_y(row);
                if(selected<0)return 0;
                session->choose_startup_menu(selected);
            } catch(const std::exception& e){report_failure(hwnd,e.what());}
            return 0;
        }
        return 0;
    case WM_ERASEBKGND:return 1;
    case WM_PAINT: {
        PAINTSTRUCT ps{};HDC dc=BeginPaint(hwnd,&ps);
        RECT whole{};GetClientRect(hwnd,&whole);
        auto bg=CreateSolidBrush(RGB(13,17,25));
        FillRect(dc,&whole,bg);DeleteObject(bg);
        const auto rect=image_rect(hwnd);
        BITMAPINFO info{};
        info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
        info.bmiHeader.biWidth=LONG(kWidth);
        info.bmiHeader.biHeight=-LONG(kHeight);
        info.bmiHeader.biPlanes=1;
        info.bmiHeader.biBitCount=32;
        info.bmiHeader.biCompression=BI_RGB;
        SetStretchBltMode(dc,COLORONCOLOR);
        StretchDIBits(dc,rect.left,rect.top,rect.right-rect.left,
            rect.bottom-rect.top,0,0,int(kWidth),int(kHeight),
            session->bgra.data(),&info,DIB_RGB_COLORS,SRCCOPY);
        SetBkMode(dc,TRANSPARENT);SetTextColor(dc,RGB(230,233,240));
        RECT help{14,whole.bottom-35,whole.right-10,whole.bottom-8};
        DrawTextW(dc,
            session->phase==kCharacterTransferred?
                L"Original CHARS party | Enter: open fresh Level 22 session | Esc closes"
                :(session->phase==kCharacterOpening?
                L"Original CODE.1 70-frame opening | Esc closes"
                :(session->phase==kCharacterChoice?
                    L"Original CODE.1 character creation | Esc closes"
                    :(session->phase==kMenu?
                        L"Original CODE.2 menu | Click or Up/Down + Enter | Esc quits"
                        :(session->phase<0?
                    L"Original CINE00/CINE12 indexed ANM (sound pending) | Space skips | Esc closes"
                    :L"Original RES0 startup presentation | Space skips to menu | Esc closes")))),
            -1,&help,DT_LEFT|DT_SINGLELINE|DT_END_ELLIPSIS);
        EndPaint(hwnd,&ps);return 0;
    }
    case WM_CLOSE:DestroyWindow(hwnd);return 0;
    case WM_NCDESTROY:
        KillTimer(hwnd,kTick);
        SetWindowLongPtrW(hwnd,GWLP_USERDATA,0);
        if(session->ownership_transferred)delete session;
        return DefWindowProcW(hwnd,msg,wp,lp);
    default:break;
    }
    return DefWindowProcW(hwnd,msg,wp,lp);
}
} // namespace
void open_native_startup(HWND owner,const NativeMedia& media) {
    auto session=std::make_unique<Preview>(media);
    WNDCLASSW wc{};
    wc.lpfnWndProc=wnd_proc;
    wc.hInstance=GetModuleHandleW(nullptr);
    wc.hCursor=LoadCursorW(nullptr,IDC_ARROW);
    wc.lpszClassName=L"RloftNativeSourceStartup";
    if(!RegisterClassW(&wc) && GetLastError()!=ERROR_CLASS_ALREADY_EXISTS)
        throw std::runtime_error("unable to register original CODE.2 startup window");
    HWND hwnd=CreateWindowExW(0,wc.lpszClassName,
        L"Ravenloft — Original CODE.2 Startup (Alpha A1)",
        WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,
        1020,920,owner,nullptr,wc.hInstance,session.get());
    if(!hwnd)throw std::runtime_error("unable to create original startup window");
    session->ownership_transferred=true;
    session.release();
    ShowWindow(hwnd,SW_SHOW);
    UpdateWindow(hwnd);
    SetFocus(hwnd);
}
} // namespace rloft_native
#endif
