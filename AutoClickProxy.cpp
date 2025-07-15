#include <windows.h>
#include <thread>

const wchar_t *WINDOW_TITLE = L"SpaceIdle";

// x and y are coordinates relative to the window's client area (content only, does not include the title bar).
struct ClickPoint
{
    int x;
    int y;
    int wait_ms;
};

// The wait_ms values below are chosen so that clicks never overlap.
// They are not multiples of each other and do not share common divisors.
const ClickPoint clickPoints[] = {
    {30, 834, 45001},
    {100, 834, 5003},
    {170, 834, 45011}};
const int numPoints = sizeof(clickPoints) / sizeof(clickPoints[0]);

void ClickPointThread(const ClickPoint &point, HWND hwnd)
{
    while (true)
    {
        LPARAM lParam = (point.y << 16) | (point.x & 0xFFFF);
        SendMessage(hwnd, WM_LBUTTONDOWN, MK_LBUTTON, lParam);
        SendMessage(hwnd, WM_LBUTTONUP, 0, lParam);
        Sleep(point.wait_ms);
    }
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
