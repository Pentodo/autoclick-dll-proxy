#include <windows.h>
#include <thread>
#include <mutex>

const wchar_t *WINDOW_TITLE = L"SpaceIdle";

struct ClickBehavior
{
    virtual ~ClickBehavior() {}
    virtual void run(int x, int y, HWND hwnd) = 0;
};

std::mutex clickMutex;

void SendClick(int x, int y, HWND hwnd)
{
    std::lock_guard<std::mutex> lock(clickMutex);
    LPARAM lParam = (y << 16) | (x & 0xFFFF);
    SendMessage(hwnd, WM_LBUTTONDOWN, MK_LBUTTON, lParam);
    SendMessage(hwnd, WM_LBUTTONUP, 0, lParam);
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

// Example: TIME MODE
// ----
// TimeClickBehavior shieldBoostInterval(45000);
// TimeClickBehavior laserBoostInterval(5000);
// TimeClickBehavior kineticVolleyInterval(45000);
// ----
// Benefits: more performance-friendly, less CPU usage.
// Case: if you want to click at specific intervals regardless of the pixel color.

// Example: COLOR MODE
// ----
ColorClickBehavior whiteColor(RGB(255, 255, 255));
// ----
// Benefits: more accurate, clicks only when the pixel color matches.
// Case: if you want to click only when a specific pixel color is present.

ClickPoint clickPoints[] = {
    {31, 807, &whiteColor},
    {99, 824, &whiteColor},
    {167, 817, &whiteColor}};
const int numPoints = sizeof(clickPoints) / sizeof(clickPoints[0]);

void ClickPointThread(const ClickPoint &point, HWND hwnd)
{
    point.behavior->run(point.x, point.y, hwnd);
}

void AutoClickThread()
{
    HWND hwnd = nullptr;
    while (!hwnd)
    {
        hwnd = FindWindowW(nullptr, WINDOW_TITLE);
        Sleep(1000);
    }
    for (int i = 0; i < numPoints; ++i)
        std::thread(ClickPointThread, clickPoints[i], hwnd).detach();
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
