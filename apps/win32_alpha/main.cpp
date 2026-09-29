#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <shellapi.h>
#include <shlobj.h>

#include "engine/render/IndexedFrameBuffer.h"
#include "games/ravenloft/data/DefinitionDatabases.h"
#include "games/ravenloft/data/InitDatabase.h"
#include "games/ravenloft/data/LevelDatabase.h"
#include "games/ravenloft/data/PaletteDatabase.h"
#include "games/ravenloft/data/ResourceStore.h"
#include "games/ravenloft/data/RavenloftDataSource.h"
#include "games/ravenloft/data/SaveGame.h"
#include "games/ravenloft/data/TrigTable.h"
#include "games/ravenloft/game/LightingSystem.h"
#include "games/ravenloft/game/AdventureScreenRenderer.h"
#include "games/ravenloft/game/OpeningDemoSession.h"
#include "games/ravenloft/game/WorldRenderer.h"
#include "games/ravenloft/game/WorldScene.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <optional>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace ravenloft::data;
using namespace ravenloft::game;

namespace {

constexpr UINT_PTR kTimerId = 1;
constexpr UINT kTimerMs = 33; // Presentation harness cadence only; game math stays in recovered routines.

bool down(int vk) { return (GetAsyncKeyState(vk) & 0x8000) != 0; }

std::uint32_t biosTicksSinceLocalMidnight() {
    // CODE.2 0x311E0 reads the BIOS data-area tick at 0040:006C. DOS starts
    // it at local midnight and IRQ0 advances it at PIT/65536 (~18.2065 Hz).
    SYSTEMTIME st{};
    GetLocalTime(&st);
    const std::uint64_t ms =
        (((static_cast<std::uint64_t>(st.wHour) * 60u + st.wMinute) * 60u + st.wSecond) * 1000u) + st.wMilliseconds;
    constexpr std::uint64_t kPitHz = 1193182u;
    return static_cast<std::uint32_t>((ms * kPitHz) / (65536u * 1000u));
}

std::filesystem::path moduleDirectory() {
    std::wstring buf(32768, L'\0');
    const DWORD n = GetModuleFileNameW(nullptr, buf.data(), static_cast<DWORD>(buf.size()));
    if (!n || n >= buf.size()) throw std::runtime_error("Could not determine executable directory");
    buf.resize(n);
    return std::filesystem::path(buf).parent_path();
}

std::filesystem::path cacheDirectory() {
    PWSTR raw = nullptr;
    const HRESULT hr = SHGetKnownFolderPath(FOLDERID_LocalAppData, KF_FLAG_CREATE, nullptr, &raw);
    if (SUCCEEDED(hr) && raw) {
        std::filesystem::path p(raw);
        CoTaskMemFree(raw);
        p /= "RavenloftAlpha";
        p /= "Cache";
        std::filesystem::create_directories(p);
        return p;
    }
    if (raw) CoTaskMemFree(raw);
    const auto fallback = moduleDirectory() / "Cache";
    std::filesystem::create_directories(fallback);
    return fallback;
}

std::filesystem::path dataPathConfigFile() {
    return moduleDirectory() / "ravenloft_data_path.txt";
}

std::optional<std::filesystem::path> readSavedDataCandidate() {
    std::wifstream f(dataPathConfigFile());
    if (!f) return std::nullopt;
    std::wstring line;
    std::getline(f, line);
    if (line.empty()) return std::nullopt;
    const std::filesystem::path p(line);
    if (std::filesystem::exists(p)) return p;
    return std::nullopt;
}

void saveDataCandidate(const std::filesystem::path& p) {
    std::wofstream f(dataPathConfigFile(), std::ios::trunc);
    if (f) f << p.wstring();
}

std::optional<std::filesystem::path> browseForDataCandidate(HWND owner = nullptr) {
    BROWSEINFOW bi{};
    bi.hwndOwner = owner;
    bi.lpszTitle = L"Select your GOG Ravenloft installation folder, complete RLOFT folder, or the folder containing game.gog";
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

    PIDLIST_ABSOLUTE pidl = SHBrowseForFolderW(&bi);
    if (!pidl) return std::nullopt;

    std::array<wchar_t, MAX_PATH> path{};
    const BOOL ok = SHGetPathFromIDListW(pidl, path.data());
    CoTaskMemFree(pidl);
    if (!ok) return std::nullopt;
    return std::filesystem::path(path.data());
}

ravenloft::data::ResolvedRavenloftData findDataRoot() {
    const auto cache = cacheDirectory();
    auto tryResolve = [&](const std::filesystem::path& p)
        -> std::optional<ravenloft::data::ResolvedRavenloftData> {
        try {
            return ravenloft::data::resolveRavenloftData(p, cache);
        } catch (...) {
            return std::nullopt;
        }
    };

    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (argv) {
        for (int i = 1; i < argc; ++i) {
            const std::filesystem::path p(argv[i]);
            if (auto resolved = tryResolve(p)) {
                LocalFree(argv);
                saveDataCandidate(p);
                return *resolved;
            }
        }
        LocalFree(argv);
    }

    if (auto saved = readSavedDataCandidate()) {
        if (auto resolved = tryResolve(*saved)) return *resolved;
    }

    const auto exe = moduleDirectory();
    const std::array<std::filesystem::path, 7> candidates{
        exe,
        exe / "RLOFT",
        exe / "GameData",
        exe / "GameData" / "RLOFT",
        exe.parent_path(),
        exe.parent_path() / "RLOFT",
        std::filesystem::current_path()
    };
    for (const auto& p : candidates) {
        if (auto resolved = tryResolve(p)) {
            saveDataCandidate(p);
            return *resolved;
        }
    }

    if (auto picked = browseForDataCandidate()) {
        if (auto resolved = tryResolve(*picked)) {
            saveDataCandidate(*picked);
            return *resolved;
        }
    }

    throw std::runtime_error(
        "Ravenloft data was not found.\n\n"
        "Select your GOG Ravenloft installation folder (preferred; game.gog is read automatically), "
        "or a complete extracted RLOFT directory.");
}

std::wstring phaseName(const MistTransition* m) {
    if (!m) return L"Inactive";
    switch (m->phase()) {
    case MistTransition::Phase::TurnForX: return L"TurnForX";
    case MistTransition::Phase::MoveX: return L"MoveX";
    case MistTransition::Phase::TurnForY: return L"TurnForY";
    case MistTransition::Phase::MoveY: return L"MoveY";
    case MistTransition::Phase::TurnIntoMist: return L"TurnIntoMist";
    case MistTransition::Phase::StartMistEffect: return L"StartMistEffect";
    case MistTransition::Phase::WaitMistEffect: return L"WaitMistEffect";
    case MistTransition::Phase::TransferFrames: return L"TransferFrames";
    case MistTransition::Phase::Complete: return L"Complete";
    }
    return L"?";
}

class AlphaApp {
public:
    explicit AlphaApp(const std::filesystem::path& root)
        : root_(root),
          levels_(root / "LEVELS.DAT"),
          npcs_(root / "NPCS.DAT"),
          init_(root / "INIT"),
          palettes_(root / "COLORS"),
          resources_(root),
          save_(root / "SAVE.DAT"),
          trig_(root / "SINTAB"),
          lighting_(LightingSystem(trig_).fromQuickStart(save_.quickStart())),
          renderer_(init_, resources_, trig_),
          screenRenderer_(init_, resources_),
          framebuffer_(256, 256),
          screen_(320, 200) {
        reset();
    }

    void reset() {
        // Reconstruct the complete session from the shipped Quick Start snapshot.
        demo_ = std::make_unique<OpeningDemoSession>(root_, biosTicksSinceLocalMidnight());
        scene_ = std::make_unique<WorldScene>(levels_.load(22), init_, npcs_, demo_->assassin());
        lastTimerSample_ = std::chrono::steady_clock::now();
        irqAccumulatorUs_ = 0;
        demoComplete_ = false;
        lastAction_ = L"Quick Start state restored";
        render();
    }

    void tick() {
        if (demoComplete_) return;
        advanceOriginalIrqClock();
        if (demo_->mistTransitionActive()) {
            if (demo_->tickMistTransition()) {
                demoComplete_ = true;
                lastAction_ = L"Mist transition backend complete (demo endpoint)";
            }
            // The visual mist effect is not implemented yet. Render only while still on level 22.
            if (demo_->party().level == 22) render();
            return;
        }

        auto held = [](std::uint8_t scan) {
            switch (scan) {
            case 0x47: return down(VK_HOME) || down('Q') || down(VK_NUMPAD7);
            case 0x48: return down(VK_UP) || down('W') || down(VK_NUMPAD8);
            case 0x49: return down(VK_PRIOR) || down('E') || down(VK_NUMPAD9);
            case 0x50: return down(VK_DOWN) || down('S') || down(VK_NUMPAD2);
            default: return false;
            }
        };

        const bool up = held(0x48), dn = held(0x50), tl = held(0x47), tr = held(0x49);
        bool moved = false;
        if (up && tl) { demo_->moveOriginalKeyboardKey(0x0147, held); moved = true; }
        else if (up && tr) { demo_->moveOriginalKeyboardKey(0x0149, held); moved = true; }
        else if (dn && tl) { demo_->moveOriginalKeyboardKey(0x0147, held); moved = true; }
        else if (dn && tr) { demo_->moveOriginalKeyboardKey(0x0149, held); moved = true; }
        else if (up) { demo_->moveOriginalKeyboardKey(0x0148, held); moved = true; }
        else if (dn) { demo_->moveOriginalKeyboardKey(0x0150, held); moved = true; }
        else if (tl) { demo_->moveOriginalKeyboardKey(0x0147, held); moved = true; }
        else if (tr) { demo_->moveOriginalKeyboardKey(0x0149, held); moved = true; }
        else if (down(VK_LEFT) || down('A') || down(VK_NUMPAD4)) { demo_->moveOriginalKeyboardKey(0x014B, held); moved = true; }
        else if (down(VK_RIGHT) || down('D') || down(VK_NUMPAD6)) { demo_->moveOriginalKeyboardKey(0x014D, held); moved = true; }
        else { demo_->originalKeyboardIdleTick(); }

        if (moved) lastAction_ = L"Original movement/collision update";
        // CODE.2 main gameplay-frame ordering: attack timer service precedes
        // the 0x1F1C8 path that reaches the 0x1BDAC NPC scheduler. The 33-ms
        // Win32 timer only asks for another host frame; gameplay clocks below
        // are advanced independently at the original IRQ0 cadence.
        demo_->tickOriginalGameplayFrame();
        render();
    }

    void advanceOriginalIrqClock() {
        const auto now = std::chrono::steady_clock::now();
        const auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - lastTimerSample_).count();
        lastTimerSample_ = now;
        if (elapsed <= 0) return;
        irqAccumulatorUs_ += static_cast<std::uint64_t>(elapsed);
        constexpr std::uint64_t period = OriginalGameplayClock19C80::kOriginalTimerPeriodMicroseconds;
        while (irqAccumulatorUs_ >= period) {
            demo_->pulseOriginalTimerIrq();
            irqAccumulatorUs_ -= period;
        }
    }

    void attackHand(int characterIndex, int handIndex) {
        const auto result = demo_->attackOpeningHand(static_cast<std::size_t>(characterIndex), handIndex, 1);
        std::wostringstream s;
        s << L"Attack C" << (characterIndex + 1) << L" H" << (handIndex + 1);
        if (result.path != PlayerHandClickResult214B8::Path::AttackDispatched) {
            s << L" blocked";
        } else if (result.attack.path == PlayerHandAttackResult26018::Path::CoolingDown) {
            s << L" cooling down";
        } else if (result.attack.path == PlayerHandAttackResult26018::Path::Melee) {
            switch (result.attack.melee.path) {
            case PlayerNpcAttackResultE5F0::Path::NoTarget: s << L": no target"; break;
            case PlayerNpcAttackResultE5F0::Path::Miss: s << L": miss"; break;
            case PlayerNpcAttackResultE5F0::Path::Hit:
                s << L": hit for " << result.attack.melee.damage;
                if (demo_->assassin().hitPoints == 0) s << L"; assassin killed";
                break;
            case PlayerNpcAttackResultE5F0::Path::UnsupportedSpecialWeapon: s << L": unsupported special weapon"; break;
            }
        }
        lastAction_ = s.str();
    }

    void clickWorld(HWND hwnd, int clientX, int clientY) {
        RECT r{}; GetClientRect(hwnd, &r);
        const int panelW = 360;
        const int rawUsableW = (r.right - r.left) - panelW - 40;
        const int rawUsableH = (r.bottom - r.top) - 40;
        const int usableW = (rawUsableW > 320) ? rawUsableW : 320;
        const int usableH = (rawUsableH > 200) ? rawUsableH : 200;
        const int scaleCandidate = ((usableW / 320) < (usableH / 200))
            ? (usableW / 320) : (usableH / 200);
        const int scale = (scaleCandidate > 1) ? scaleCandidate : 1;
        const int drawW = 320 * scale, drawH = 200 * scale;
        const int drawX = 20;
        const int centeredY = ((r.bottom - r.top) - drawH) / 2;
        const int drawY = (centeredY > 20) ? centeredY : 20;
        if (clientX < drawX || clientY < drawY ||
            clientX >= drawX + drawW || clientY >= drawY + drawH) return;

        const int adventureX = (clientX - drawX) / scale;
        const int adventureY = (clientY - drawY) / scale;
        if(adventureX < 32 || adventureX >= 288 || adventureY < 4 || adventureY >= 132) return;
        const int screenX = adventureX - 32;
        // Temporary presentation bridge: the reconstructed world hit geometry
        // is still expressed in the 256-row staging coordinate space.
        const int screenY = (adventureY - 4) * 2;
        const auto result=demo_->pickupGroundObjectAtScreen(screenX,screenY,1,resources_);
        std::wostringstream msg;
        if(result.picked){
            msg << L"Picked ground object " << result.objectId;
            if(result.startMistTransition)msg << L" -> mist transition triggered";
        } else {
            msg << L"World click " << screenX << L"," << screenY << L": no ground object";
        }
        lastAction_=msg.str();
    }

    bool clickInventoryProxy(HWND hwnd, int clientX, int clientY) {
        RECT r{}; GetClientRect(hwnd, &r);
        const int invX = r.right - 120;
        const int invY = 20;
        constexpr int invW = 100;
        constexpr int invH = 320;
        if(clientX < invX || clientX >= invX + invW ||
           clientY < invY || clientY >= invY + invH) return false;

        // The proxy is a 1:1 copy of the original inventory-strip coordinates:
        // X 297..396 and four stacked 80-pixel character panels. This is not
        // final HUD art; it exists only to feed the reconstructed 0x12E70 /
        // 0x11FB0 selector the same coordinates the DOS interface supplied.
        const int originalX = InventoryGridClick11FB0::kLeft + (clientX - invX);
        const int originalY = clientY - invY;
        const auto result = demo_->clickInventoryAtOriginalScreen(originalX, originalY, 1);
        std::wostringstream msg;
        if(result.handled){
            msg << L"Inventory click: character " << (result.characterIndex + 1)
                << L", slot " << result.slotIndex;
            if(result.exchangeHandled) msg << L" -> 0x117A0 exchange";
            else msg << L" -> consumed/no exchange";
        } else {
            msg << L"Inventory proxy click outside active original slot region";
        }
        lastAction_ = msg.str();
        return true;
    }

    void click(HWND hwnd, int clientX, int clientY) {
        if(clickInventoryProxy(hwnd,clientX,clientY)) return;
        clickWorld(hwnd,clientX,clientY);
    }

    void render() {
        if (demo_->party().level != 22) return;
        renderer_.render(framebuffer_, *scene_, demo_->party(), lighting_);
        screenRenderer_.renderOpening(screen_, framebuffer_, demo_->party().level);
        const auto& pal = palettes_.at(scene_->paletteIndex());
        const auto src = screen_.pixels();
        bgra_.resize(static_cast<std::size_t>(screen_.width()) * screen_.height());
        for (std::size_t i = 0; i < src.size(); ++i) {
            const auto c = pal[src[i]];
            bgra_[i] = 0xFF000000u | (std::uint32_t(c.r) << 16) | (std::uint32_t(c.g) << 8) | c.b;
        }
    }

    void paint(HWND hwnd, HDC dc) const {
        RECT r{}; GetClientRect(hwnd, &r);
        FillRect(dc, &r, static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));

        const int panelW = 360;
        const int rawUsableW = (r.right - r.left) - panelW - 40;
        const int rawUsableH = (r.bottom - r.top) - 40;
        const int usableW = (rawUsableW > 320) ? rawUsableW : 320;
        const int usableH = (rawUsableH > 200) ? rawUsableH : 200;
        const int scaleCandidate = ((usableW / 320) < (usableH / 200))
            ? (usableW / 320)
            : (usableH / 200);
        const int scale = (scaleCandidate > 1) ? scaleCandidate : 1;
        const int drawW = 320 * scale, drawH = 200 * scale;
        const int x = 20;
        const int centeredY = ((r.bottom - r.top) - drawH) / 2;
        const int y = (centeredY > 20) ? centeredY : 20;

        BITMAPINFO bmi{};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = 320;
        bmi.bmiHeader.biHeight = -200; // top-down
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;
        StretchDIBits(dc, x, y, drawW, drawH, 0, 0, 320, 200,
                      bgra_.data(), &bmi, DIB_RGB_COLORS, SRCCOPY);

        SetBkMode(dc, TRANSPARENT);
        SetTextColor(dc, RGB(230, 230, 230));
        const int tx = x + drawW + 24;
        int ty = 28;
        auto line = [&](const std::wstring& text) {
            TextOutW(dc, tx, ty, text.c_str(), static_cast<int>(text.size()));
            ty += 22;
        };

        line(L"Ravenloft Port Alpha 0.2.1");
        line(L"Forgotten Realms Forest");
        ty += 8;
        const auto& p = demo_->party();
        std::wostringstream pos;
        pos.setf(std::ios::fixed); pos.precision(3);
        pos << L"Position: " << p.position.xd() << L", " << p.position.yd(); line(pos.str());
        line(L"Heading: " + std::to_wstring(p.headingDegrees) + L" deg");
        line(L"Level: " + std::to_wstring(p.level));
        line(L"Ground objects: " + std::to_wstring(demo_->groundObjects().size()));
        line(L"Owned objects: " + std::to_wstring(p.ownedObjects.size()));
        line(L"Mist: " + phaseName(demo_->mistTransition()));
        if (demo_->mistTransition()) line(L"Transfer frame: " + std::to_wstring(demo_->mistTransition()->transferFrame()));
        ty += 12;
        line(L"Controls");
        line(L"W/S or Up/Down: forward/back");
        line(L"A/D or Left/Right: strafe");
        line(L"Q/E or Home/PgUp: turn");
        line(L"F5: reset Quick Start state");
        line(L"Z/X: character 1 left/right hand attack");
        line(L"C/V: character 2 left/right hand attack");
        line(L"Left-click: original ground-object hit/pickup path");
        line(L"Inventory proxy: click a visible 3x4 slot -> 0x11FB0/0x117A0");
        line(L"Esc: quit");
        ty += 12;
        SetTextColor(dc, RGB(180, 205, 180));
        line(L"Current alpha limitations:");
        line(L"Static original HUD frame active; dynamic widgets unfinished");
        line(L"Mist visual effect unfinished");
        line(L"World renderer is the current decomp path");
        // Mechanical inventory proxy: exact original inventory-strip geometry
        // (100 x 320, four 80-pixel character panels). Final DOS HUD visuals
        // are intentionally deferred; this surface only supplies original input
        // coordinates to 0x12E70/0x11FB0.
        const int invX = r.right - 120;
        const int invY = 20;
        SetTextColor(dc, RGB(180, 205, 180));
        TextOutW(dc, invX, invY + 322, L"Inventory click proxy", 21);
        const auto& partyRecords = demo_->partyRecords();
        for(int ci=0; ci<4; ++ci){
            Rectangle(dc, invX, invY + ci*80, invX + 100, invY + ci*80 + 80);
            for(int slot=0; slot<12; ++slot){
                const int col=slot/4, row=slot%4;
                const int left=invX + col*34;
                const int top=invY + ci*80 + 5 + row*18;
                const int right=((left+34)<(invX+100)) ? (left+34) : (invX+100);
                const int bottom=((top+18)<(invY+ci*80+75)) ? (top+18) : (invY+ci*80+75);
                Rectangle(dc,left,top,right,bottom);
                const auto off=static_cast<std::size_t>(0x40+slot*2);
                const auto id=static_cast<std::uint16_t>(partyRecords[ci].raw[off]) |
                              static_cast<std::uint16_t>(static_cast<std::uint16_t>(partyRecords[ci].raw[off+1])<<8u);
                if(id!=0xFFFFu){
                    const std::wstring label=std::to_wstring(static_cast<std::int16_t>(id));
                    TextOutW(dc,left+2,top+2,label.c_str(),static_cast<int>(label.size()));
                }
            }
        }

        ty += 12;
        SetTextColor(dc, RGB(255, 220, 160));
        line(lastAction_);
        if (demoComplete_) {
            ty += 12;
            SetTextColor(dc, RGB(255, 255, 255));
            line(L"DEMO COMPLETE: mist backend reached endpoint.");
        }
    }

private:
    std::filesystem::path root_;
    LevelDatabase levels_;
    NpcDatabase npcs_;
    InitDatabase init_;
    PaletteDatabase palettes_;
    ResourceStore resources_;
    SaveGame save_;
    TrigTable trig_;
    std::unique_ptr<OpeningDemoSession> demo_;
    std::unique_ptr<WorldScene> scene_;
    LightingState lighting_;
    WorldRenderer renderer_;
    AdventureScreenRenderer screenRenderer_;
    ssi::render::IndexedFrameBuffer framebuffer_;
    ssi::render::IndexedFrameBuffer screen_;
    std::vector<std::uint32_t> bgra_;
    std::chrono::steady_clock::time_point lastTimerSample_{};
    std::uint64_t irqAccumulatorUs_ = 0;
    bool demoComplete_ = false;
    std::wstring lastAction_ = L"Loaded shipped Quick Start state";
};

std::unique_ptr<AlphaApp> gApp;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CREATE:
        SetTimer(hwnd, kTimerId, kTimerMs, nullptr);
        return 0;
    case WM_TIMER:
        if (wp == kTimerId && gApp) {
            gApp->tick();
            InvalidateRect(hwnd, nullptr, FALSE);
        }
        return 0;
    case WM_KEYDOWN:
        if (!gApp) break;
        if (wp == VK_ESCAPE) { DestroyWindow(hwnd); return 0; }
        if (wp == VK_F5) { gApp->reset(); InvalidateRect(hwnd, nullptr, FALSE); return 0; }
        if (wp == 'Z') { gApp->attackHand(0, 0); InvalidateRect(hwnd, nullptr, FALSE); return 0; }
        if (wp == 'X') { gApp->attackHand(0, 1); InvalidateRect(hwnd, nullptr, FALSE); return 0; }
        if (wp == 'C') { gApp->attackHand(1, 0); InvalidateRect(hwnd, nullptr, FALSE); return 0; }
        if (wp == 'V') { gApp->attackHand(1, 1); InvalidateRect(hwnd, nullptr, FALSE); return 0; }
        break;
    case WM_LBUTTONDOWN:
        if (gApp) {
            const int x = static_cast<short>(LOWORD(lp));
            const int y = static_cast<short>(HIWORD(lp));
            gApp->click(hwnd, x, y);
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }
        break;
    case WM_PAINT: {
        PAINTSTRUCT ps{};
        HDC dc = BeginPaint(hwnd, &ps);
        if (gApp) gApp->paint(hwnd, dc);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_DESTROY:
        KillTimer(hwnd, kTimerId);
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

} // namespace

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show) {
    const HRESULT ole = OleInitialize(nullptr);
    try {
        const auto data = findDataRoot();
        gApp = std::make_unique<AlphaApp>(data.root);

        const wchar_t* cls = L"RavenloftPortAlphaWindow";
        WNDCLASSW wc{};
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = WndProc;
        wc.hInstance = instance;
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
        wc.lpszClassName = cls;
        if (!RegisterClassW(&wc)) throw std::runtime_error("RegisterClassW failed");

        HWND hwnd = CreateWindowExW(0, cls, L"Ravenloft: Strahd's Possession - Port Alpha 0.2.1",
                                    WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                                    1180, 840, nullptr, nullptr, instance, nullptr);
        if (!hwnd) throw std::runtime_error("CreateWindowExW failed");
        ShowWindow(hwnd, show);
        UpdateWindow(hwnd);

        MSG msg{};
        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        gApp.reset();
        if (SUCCEEDED(ole)) OleUninitialize();
        return static_cast<int>(msg.wParam);
    } catch (const std::exception& e) {
        MessageBoxA(nullptr, e.what(), "Ravenloft Port Alpha - Startup Error", MB_OK | MB_ICONERROR);
        if (SUCCEEDED(ole)) OleUninitialize();
        return 1;
    }
}
