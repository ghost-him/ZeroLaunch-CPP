#include "windowhook.h"

WindowHook* WindowHook::instance = nullptr;

WindowHook::WindowHook(): parentWidget(nullptr), callback(nullptr), hHook(NULL)
{
    instance = this;
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

WindowHook::~WindowHook()
{
    if (hHook) {
        UnhookWinEvent(hHook);
    }
    instance = nullptr;
}

void WindowHook::WinEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime)
{
    if (event == EVENT_SYSTEM_FOREGROUND) {
        if (instance && hwnd != (HWND)instance->parentWidget->winId()) {
            if (instance->callback) {
                instance->callback();
            }
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
