#include "windowhook.h"

WindowHook::WindowHook(): parentWidget(nullptr), defaultCallback(nullptr), hHook(NULL)
{
    instance = this;
    isStop = false;
    installHook();
}

void WindowHook::stop()
{
    if (hHook) {
        UnhookWinEvent(hHook);
    }
    instance = nullptr;
}

WindowHook::~WindowHook()
{
    if (hHook) {
        UnhookWinEvent(hHook);
    }
    instance = nullptr;
}

void WindowHook::hideWindow(HWND targetWinId)
{
    if (store.contains((WId)targetWinId)) {
        store[(WId)targetWinId]();
    } else {
        if (defaultCallback) {
            defaultCallback();
        }
    }
}

void WindowHook::registerWinID(WId winID, std::function<void()> callbackFunc) {
    store[winID] = callbackFunc;
}

void WindowHook::registerDefaultCallback(std::function<void ()> callbackFunc)
{
    defaultCallback = callbackFunc;
}

void WinEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime)
{
    if (event == EVENT_OBJECT_FOCUS) {
        WindowHook& instance = WindowHook::getInstance();
        qDebug() << "target winid: " << (int)hwnd;
        instance.hideWindow(hwnd);
    }
}

void WindowHook::installHook()
{
    if (!hHook) {
        hHook = SetWinEventHook(
            EVENT_OBJECT_FOCUS, EVENT_OBJECT_FOCUS,
            NULL, WinEventProc,
            0, 0,
            WINEVENT_OUTOFCONTEXT
            );

        if (!hHook) {
            qWarning("Failed to set event hook!");
        }
    }
}
