#include "windowhook.h"

WindowHook::WindowHook(): parentWidget(nullptr), callback(nullptr), hHook(NULL)
{
    instance = this;
    isStop = false;
    installHook();
}

void WindowHook::setTargetWidget(QWidget *parentWidget)
{
    this->parentWidget = parentWidget;
}

void WindowHook::setCallback(std::function<void ()> callback)
{
    this->callback = callback;
}

void WindowHook::stop()
{
    if (hHook) {
        UnhookWinEvent(hHook);
    }
    instance = nullptr;
}

WId WindowHook::getWinID()
{
    return parentWidget->winId();
}

void WindowHook::doCallBack()
{
    if (callback) {
        callback();
    }
}

WindowHook::~WindowHook()
{
    if (hHook) {
        UnhookWinEvent(hHook);
    }
    instance = nullptr;
}

void WinEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime)
{
    if (event == EVENT_SYSTEM_FOREGROUND) {
        WindowHook& instance = WindowHook::getInstance();
        if (hwnd != (HWND)instance.getWinID()) {
            instance.doCallBack();
        }
    }
}

void WindowHook::installHook()
{
    if (!hHook) {
        hHook = SetWinEventHook(
            EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,
            NULL, WinEventProc,
            0, 0,
            WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS
            );

        if (!hHook) {
            qWarning("Failed to set event hook!");
        }
    }
}
