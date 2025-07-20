#include <windows.h>
#include <thread>
#include <mutex>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

struct ClickBehavior
{
    virtual ~ClickBehavior() {}
    virtual void run(int x, int y, HWND hwnd) = 0;
};

std::mutex clickMutex;

void SendClick(int x, int y, HWND hwnd)
{
    // Wait until the left mouse button is released before clicking.
    // This allows users to click and drag without interference.
    while ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0)
        Sleep(100);
    std::lock_guard<std::mutex> lock(clickMutex);
    LPARAM lParam = (y << 16) | (x & 0xFFFF);
    PostMessage(hwnd, WM_LBUTTONDOWN, MK_LBUTTON, lParam);
    PostMessage(hwnd, WM_LBUTTONUP, 0, lParam);
}

struct TimeClickBehavior : public ClickBehavior
{
    int wait_ms;
    TimeClickBehavior(int ms) : wait_ms(ms) {}
    void run(int x, int y, HWND hwnd) override
    {
        while (true)
        {
            SendClick(x, y, hwnd);
            Sleep(wait_ms);
        }
    }
};

struct ColorClickBehavior : public ClickBehavior
{
    COLORREF color;
    ColorClickBehavior(COLORREF c) : color(c) {}
    void run(int x, int y, HWND hwnd) override
    {
        POINT pt = {x, y};
        ClientToScreen(hwnd, &pt);
        HDC hdc = GetDC(NULL);
        while (true)
        {
            COLORREF pixel = GetPixel(hdc, pt.x, pt.y);
            if (pixel == color)
                SendClick(x, y, hwnd);
            Sleep(100);
        }
    }
};

// x and y are coordinates relative to the window's client area (content only, does not include the title bar).
struct ClickPoint
{
    int x;
    int y;
    ClickBehavior *behavior;
    ClickPoint(int x_, int y_, ClickBehavior *b) : x(x_), y(y_), behavior(b) {}
};

std::wstring windowTitle;

std::vector<ClickBehavior *> behaviors;
std::vector<ClickPoint> clickPoints;

void LoadConfig(const char *filename)
{
    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#')
            continue;
        if (line.rfind("window_title=", 0) == 0)
        {
            std::string title = line.substr(13);
            windowTitle = std::wstring(title.begin(), title.end());
            continue;
        }
        std::istringstream iss(line);
        int x, y;
        std::string type;
        iss >> x >> y >> type;
        if (type == "time")
        {
            int ms;
            iss >> ms;
            auto *beh = new TimeClickBehavior(ms);
            behaviors.push_back(beh);
            clickPoints.emplace_back(x, y, beh);
        }
        else if (type == "color")
        {
            int r, g, b;
            iss >> r >> g >> b;
            auto *beh = new ColorClickBehavior(RGB(r, g, b));
            behaviors.push_back(beh);
            clickPoints.emplace_back(x, y, beh);
        }
    }
}

void ClickPointThread(const ClickPoint &point, HWND hwnd)
{
    point.behavior->run(point.x, point.y, hwnd);
}

HWND GetMainWindowHandle()
{
    DWORD currentPID = GetCurrentProcessId();
    HWND hwnd = nullptr;
    struct EnumData
    {
        DWORD pid;
        HWND hwnd;
    } data = {currentPID, nullptr};
    auto EnumProc = [](HWND hWnd, LPARAM lParam) -> BOOL
    {
        EnumData *pData = (EnumData *)lParam;
        DWORD winPID = 0;
        GetWindowThreadProcessId(hWnd, &winPID);
        if (winPID == pData->pid && GetWindow(hWnd, GW_OWNER) == nullptr && IsWindowVisible(hWnd))
        {
            pData->hwnd = hWnd;
            return FALSE;
        }
        return TRUE;
    };
    EnumWindows(EnumProc, (LPARAM)&data);
    return data.hwnd;
}

void AutoClickThread()
{
    LoadConfig("config.ini");
    HWND hwnd = nullptr;
    while (!hwnd)
    {
        if (!windowTitle.empty())
            hwnd = FindWindowW(nullptr, windowTitle.c_str());
        else
            hwnd = GetMainWindowHandle();
        Sleep(1000);
    }
    for (const auto &point : clickPoints)
        std::thread(ClickPointThread, point, hwnd).detach();
}

// Proxy for DirectInput8Create: forwards calls to the original dinput8.dll from the system directory.
typedef HRESULT(WINAPI *DirectInput8Create_t)(
    HINSTANCE, DWORD, REFIID, LPVOID *, LPUNKNOWN);

HMODULE realDInput8 = nullptr;
DirectInput8Create_t realDirectInput8Create = nullptr;

extern "C" __declspec(dllexport) HRESULT WINAPI DirectInput8Create(
    HINSTANCE hinst, DWORD dwVersion, REFIID riid, LPVOID *ppvOut, LPUNKNOWN punkOuter)
{
    if (!realDInput8)
    {
        wchar_t sysPath[MAX_PATH];
        GetSystemDirectoryW(sysPath, MAX_PATH);
        wcscat_s(sysPath, L"\\dinput8.dll");
        realDInput8 = LoadLibraryW(sysPath);
        if (!realDInput8)
            return E_FAIL;
        realDirectInput8Create = (DirectInput8Create_t)GetProcAddress(realDInput8, "DirectInput8Create");
        if (!realDirectInput8Create)
            return E_FAIL;
    }
    return realDirectInput8Create(hinst, dwVersion, riid, ppvOut, punkOuter);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hModule);
        std::thread(AutoClickThread).detach();
    }
    return TRUE;
}
