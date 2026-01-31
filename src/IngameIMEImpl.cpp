#include "imm/ImmInputContextImpl.hpp"
#include "IngameIME.hpp"
#include "tf/TfInputContextImpl.hpp"
#include <windows.h>

namespace IngameIME
{
struct EnumData
{
    DWORD pid;
    HWND  hWnd;
};

BOOL CALLBACK EnumProc(HWND hWnd, LPARAM lParam)
{
    EnumData& data = *(EnumData*)lParam;
    DWORD     pid  = 0;
    GetWindowThreadProcessId(hWnd, &pid);

    if (pid == data.pid && IsWindowVisible(hWnd) && GetWindow(hWnd, GW_OWNER) == NULL)
    {
        data.hWnd = hWnd;
        return FALSE;
    }
    return TRUE;
}

HWND FindProcessWindow()
{
    EnumData data = {GetCurrentProcessId(), NULL};
    EnumWindows(EnumProc, (LPARAM)&data);
    return data.hWnd;
}

InputContext* CreateInputContextWin32(HWND hWnd, const API api, const bool uiLess)
{
    if (hWnd == NULL || !IsWindow(hWnd) || (uintptr_t)hWnd > 0xFFFFFFFF)
    {
        HWND foundWnd = FindProcessWindow();
        if (foundWnd)
        {
            hWnd = foundWnd;
        }
    }

    if (api == API::TextServiceFramework)
        return new tf::InputContextImpl(hWnd, uiLess);
    else
        return new imm::InputContextImpl(hWnd, uiLess);
}
} // namespace IngameIME
